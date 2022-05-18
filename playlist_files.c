/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "playlist_files.h"
#include "playlist.h"
#include "xine/xmlparser.h"


typedef struct
{
    char *Path;
    char *Title;
    int Length;
} TPlayListItem;





//If the file contains a 'NumberOfEntries' line, then we use that to allocate a list of TPlayListItem
//structures to hold this. If it doesn't contain this entry, then we just add a file when we encounter
//the 'File[n]=' line. If the number of entries is more than specified, we fall back on the 'just add
//a file to the playlist' method, which can result in the files being out of sequence, but the .pls
//file was screwed up anyways.
int PLSPlaylistLoad(TStringList *List, const char *MRL)
{
    FILE *f;
    char *Tempstr=NULL, *Token=NULL;
    const char *ptr;
    int result=FALSE;
    int NoOfEntries=0, AddedEntries=0, id;
    TPlayListItem *Items=NULL;

    f=fopen(MRL, "r");
    if (f)
    {
        Tempstr=realloc(Tempstr, 256);
        while (fgets(Tempstr, 255, f))
        {
            Tempstr=xine_chomp(Tempstr);
            ptr=rstrtok(Tempstr, "=", &Token);
            if (strcasecmp(Token, "numberofentries")==0)
            {
                NoOfEntries=atoi(ptr);
                Items=calloc(NoOfEntries, sizeof(TPlayListItem));
            }
            else if (strncasecmp(Token, "File", 4)==0)
            {
                id=atoi(Token+4)-1;
                if (id < NoOfEntries) Items[id].Path=rstrcpy(Items[id].Path, ptr);
                else
                {
                    PlaylistAdd(List, ptr, "", "");
                    if (Config->flags & CONFIG_WEBCAST) break;
                }
            }
            else if (strncasecmp(Token, "Title", 5)==0)
            {
                id=atoi(Token+5)-1;
                if (id < NoOfEntries) Items[id].Title=rstrcpy(Items[id].Title, ptr);
            }

        }
        fclose(f);

        for (id=0; id < NoOfEntries; id++)
        {
            if (StrLen(Items[id].Path))
            {
                PlaylistAdd(List, Items[id].Path, "", Items[id].Title);
                if (Config->flags & CONFIG_WEBCAST) break;
            }
        }

        result=TRUE;
    }

    destroy(Tempstr);
    destroy(Token);
    destroy(Items);

    return(result);
}

char *M3UReadTitle(char *Title, char *Data)
{
    char *Token=NULL;
    const char *ptr;

//lead token is the track duration
    ptr=rstrtok(Data, ",", &Token);
    Title=rstrcpy(Title, ptr);

    destroy(Token);
    return(Title);
}

int M3UPlaylistLoad(TStringList *List, const char *MRL)
{
    FILE *f;
    char *Tempstr=NULL, *Title=NULL;
    int result=FALSE;

    Title=rstrcpy(Title,"");
    f=fopen(MRL, "r");
    if (f)
    {
        Tempstr=realloc(Tempstr, 256);
        while (fgets(Tempstr, 255, f))
        {
            Tempstr=xine_chomp(Tempstr);

            if ( StrLen(Tempstr) )
            {
                if (strncmp(Tempstr,"#EXTINF:",8)==0) Title=M3UReadTitle(Title, Tempstr+8);
                else if (*Tempstr != '#')
                {
                    PlaylistAdd(List, Tempstr, "", Title);
                    Title=rstrcpy(Title,"");
                    if (Config->flags & CONFIG_WEBCAST) break;
                }
            }
        }
        fclose(f);
        result=TRUE;
    }

    destroy(Tempstr);
    destroy(Title);
    return(result);
}


xml_node_t *PlayListXMLLoad(const char *MRL)
{
    struct stat FStat;
    char *Tempstr=NULL;
    xml_node_t *root=NULL;
    xml_parser_t *parser;
    FILE *f;

    stat(MRL, &FStat);
    Tempstr=calloc(FStat.st_size +20, sizeof(char));
    f=fopen(MRL, "r");
    if (f)
    {
        fread(Tempstr, FStat.st_size, sizeof(char), f);
        fclose(f);

        parser = xml_parser_init_r (Tempstr, FStat.st_size, XML_PARSER_CASE_INSENSITIVE);
        if (parser)
        {
            xml_parser_build_tree_r(parser, &root);
            xml_parser_finalize_r (parser);
        }
    }

    destroy(Tempstr);

    return(root);
}



void XSPFPlaylistParseItem(TStringList *List, xml_node_t *item)
{
    xml_node_t *node;
    const char *p_title=NULL, *p_path=NULL;

    for (node = item->child; node != NULL; node = node->next)
    {
        if (strcasecmp(node->name, "title")==0) p_title = node->data;
        else if (strcasecmp(node->name, "location")==0) p_path = node->data;
    }

    if (p_path) PlaylistAdd(List, p_path, "", p_title);
}


void XSPFPlaylistParseTracklist(TStringList *List, xml_node_t *parent)
{
    xml_node_t *node;

    for (node = parent->child; node; node = node->next)
    {
        if (strcasecmp (node->name, "tracklist")==0) XSPFPlaylistParseTracklist(List, node);
        else if (strcasecmp (node->name, "track")==0) XSPFPlaylistParseItem(List, node);
    }
}



void RSSPlaylistParseItem(TStringList *List, xml_node_t *item)
{
    xml_node_t *node;
    xml_property_t *prop;
    char *Title=NULL;

    const char *p_title=NULL, *p_path=NULL, *p_description=NULL;;

    for (node = item->child; node != NULL; node = node->next)
    {
        if (strcasecmp(node->name, "title")==0) p_title=node->data;
        else if (strcasecmp(node->name, "description")==0) p_description=node->data;
        else if (strcasecmp(node->name, "enclosure")==0)
        {
            for (prop=node->props; prop !=NULL; prop=prop->next)
            {
                if (strcasecmp(prop->name, "url")==0) p_path = prop->value;
            }
        }
    }

    Title=rstrcpy(Title, p_title);
    if (StrLen(Title) < 10)
    {
        Title=rstrcat(Title, " ");
        Title=rstrcat(Title, p_description);
    }
    if (p_path) PlaylistAdd(List, p_path, "", Title);

    destroy(Title);
}


void RSSPlaylistParseTracklist(TStringList *List, xml_node_t *parent)
{
    xml_node_t *node;

    for (node = parent->child; node; node = node->next)
    {
        if (strcasecmp (node->name, "channel")==0) RSSPlaylistParseTracklist(List, node);
        else if (strcasecmp (node->name, "item")==0) RSSPlaylistParseItem(List, node);
    }
}


void ASXPlaylistParseItem(TStringList *List, xml_node_t *item)
{
    xml_node_t *node;
    const char *p_title=NULL, *p_path=NULL;

    for (node = item->child; node != NULL; node = node->next)
    {
        if (strcasecmp(node->name, "title")==0) p_title = node->data;
        else if (strcasecmp(node->name, "ref")==0) p_path = xml_parser_get_property (node, "href");
    }

    if (p_path) PlaylistAdd(List, p_path, "", p_title);

}


void ASXPlaylistParseTracklist(TStringList *List, xml_node_t *parent)
{
    xml_node_t *node;

    for (node = parent->child; node; node = node->next)
    {
        if (strcasecmp (node->name, "entry")==0) ASXPlaylistParseItem(List, node);
    }
}


int XMLPlaylistLoad(TStringList *List, const char *MRL)
{
    xml_node_t *root;

    root=PlayListXMLLoad(MRL);
    if (root)
    {
        if (strcasecmp(root->name, "rss")==0) RSSPlaylistParseTracklist(List, root);
        else if (strcasecmp(root->name, "playlist")==0) XSPFPlaylistParseTracklist(List, root);
        else if (strcasecmp(root->name, "asx")==0) ASXPlaylistParseTracklist(List, root);
        xml_parser_free_tree(root);
        return(TRUE);
    }
    return(FALSE);
}



int DetectPlaylist4CC(const char *MRL)
{
    int fd, result;
    char Buffer[11];
    const char *ptr;

    ptr=strrchr(MRL, '.');
    if (ptr)
    {
        if (strcasecmp(ptr, ".pls")==0) return(PLAYLIST_FILE_PLS);
        else if (strcasecmp(ptr, ".xspf")==0) return(PLAYLIST_FILE_XSPF);
        else if (strcasecmp(ptr, ".m3u")==0) return(PLAYLIST_FILE_M3U);
        else if (strcasecmp(ptr, ".asx")==0) return(PLAYLIST_FILE_ASX);
        else if (strcasecmp(ptr, ".rss")==0) return(PLAYLIST_FILE_RSS);
    }

    fd=open(MRL, O_RDONLY);
    if (fd > -1)
    {
        result=read(fd, Buffer, 10);
        close(fd);

        if (result > 9)
        {
            //fsking unicode! this pattern is a unicode Byte Order Mark to tell us the
            //document is in utf8
            if (memcmp(Buffer, "\xef\xbb\xbf", 3)==0)
            {
                result-=3;
                memmove(Buffer, Buffer+3, result);
            }

            if (strncasecmp(Buffer, "[playlist]", 10)==0) return(PLAYLIST_FILE_PLS);
            Buffer[6]='\0';
            if (strncasecmp(Buffer, "<?xml", 5)==0) return(PLAYLIST_FILE_XML);
            if (strncasecmp(Buffer, "http", 4)==0) return(PLAYLIST_FILE_M3U);
        }
    }

    return(PLAYLIST_FILE_NOT);
}


int IsPlaylist(const char *MRL)
{
    if (DetectPlaylist4CC(MRL) != PLAYLIST_FILE_NOT) return(TRUE);
    return(FALSE);
}



int PlaylistLoad(TStringList *List, const char *MRL)
{
    switch (DetectPlaylist4CC(MRL))
    {
    case PLAYLIST_FILE_PLS:
        return(PLSPlaylistLoad(List, MRL));
        break;

    case PLAYLIST_FILE_M3U:
        return(M3UPlaylistLoad(List, MRL));
        break;

    case PLAYLIST_FILE_ASX:
    case PLAYLIST_FILE_XML:
    case PLAYLIST_FILE_XSPF:
    case PLAYLIST_FILE_RSS:
        return(XMLPlaylistLoad(List, MRL));
        break;
    }

    return(FALSE);
}

int PlaylistFileNeedsUpdate(const char *MRL, const char *Path)
{
    struct stat Stat;

    if (! StrLen(MRL)) return(FALSE);

    if (*MRL == '/') return(FALSE);
    if (strcmp(MRL, Path)==0) return(FALSE);

    stat(Path, &Stat);
    if ( (time(NULL) - Stat.st_mtime) > Config->playlist_update_time) return(TRUE);
    return(FALSE);
}
