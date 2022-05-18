/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "playlist.h"
#include "playlist_files.h"
#include "playlist_osd.h"
#include "download.h"
#include "osd.h"
#include <glob.h>



void PlaylistItemDestroy(void *p_Item)
{
    TPlaylistItem *PI;

    PI=(TPlaylistItem *) p_Item;
    destroy(PI->URL);
    destroy(PI->ID);
    destroy(PI->Title);
    destroy(PI->Description);
}



void PlaylistShuffle()
{
    char *Tempstr=NULL;
    int rec1, rec2, i;

    srand(time(NULL));
    for (i=0; i < 100; i++)
    {
        rec1=rand() % Config->playlist->size;
        rec2=rand() % Config->playlist->size;

        Tempstr=rstrcpy(Tempstr, StringListGet(Config->playlist, rec1));
        StringListSet(Config->playlist, rec1, StringListGet(Config->playlist, rec2));
        StringListSet(Config->playlist, rec2, Tempstr);
    }
    Config->playlist->next=0;

    destroy(Tempstr);
}



char *PlaylistFormatEntry(char *RetStr, const char *URL, const char *ID, const char *Title)
{
    char *Quoted=NULL, *Tempstr=NULL;

    Quoted=rstrquot(Quoted, URL, "\\  ");
    RetStr=rstrcat(RetStr, Quoted);

    if (StrLen(ID))
    {
        RetStr=rstrcat(RetStr, " id='");
        Quoted=rstrcpy(Quoted, ID);
        StripQuotes(Quoted);
        RetStr=rstrcat(RetStr, Quoted);
        RetStr=rstrcat(RetStr, "'");
    }

    if (StrLen(Title))
    {
        RetStr=rstrcat(RetStr, " title='");
        Tempstr=rstrcpy(Tempstr, Title);
        StripQuotes(Tempstr);
        Quoted=rstrquot(Quoted, Tempstr, "\'");
        RetStr=rstrcat(RetStr, Quoted);
        RetStr=rstrcat(RetStr, "'");

    }

    destroy(Tempstr);
    destroy(Quoted);
    return(RetStr);
}


static int IsDir(const char *URL)
{
    struct stat Stat;

    if (stat(URL, &Stat) !=0) return(FALSE);
    if (S_ISDIR(Stat.st_mode)) return(TRUE);
    return(FALSE);
}


void PlaylistAdd(TStringList *playlist, const char *iURL, const char *ID, const char *Title)
{
    char *Tempstr=NULL, *URL=NULL;
    CXineOSD *OSD=NULL;
    glob_t Glob;
    int i;

    URL=rstrcpy(URL, iURL);

    if (IsDir(URL))
    {
        if (Config->flags & CONFIG_RECURSIVE)
        {
            Tempstr=rstrcpy(Tempstr, iURL);
            Tempstr=rstrcat(Tempstr, "/*");
            glob(Tempstr, 0, 0, &Glob);
            for (i=0; i < Glob.gl_pathc; i++)
            {
                PlaylistAdd(playlist, Glob.gl_pathv[i], ID, Title);
            }
            globfree(&Glob);
        }
    }
    else if (IsPlaylist(URL))
    {
        if ( ! PlaylistLoad(playlist, URL) ) StringListAdd(playlist, URL);
    }
    else if ( ! PlaylistLoad(playlist, URL) )
    {

        if (
            (*URL != '/' ) &&
            (! strchr(URL, ':')) &&
            (StrLen(Config->path_prefix))
        )
        {
            //rebuild URL with leading prefix
            Tempstr=rstrcpy(Tempstr, Config->path_prefix);
            Tempstr=rstrcat(Tempstr, URL);
            URL=rstrcpy(URL, Tempstr);
        }

        Tempstr=PlaylistFormatEntry(Tempstr, URL, ID, Title);
        StringListAdd(playlist, Tempstr);
    }


    destroy(Tempstr);
    destroy(URL);
}


void PlaylistParseAndAdd(TStringList *playlist, const char *ItemInfo)
{
    TPlaylistItem *PI;

    PI=PlaylistDecodeEntry(ItemInfo);
    PlaylistAdd(Config->playlist, PI->URL, PI->ID, PI->Title);
    PlaylistItemDestroy(PI);
}


int PlaylistMoveItem(TStringList *playlist, int pos, int move)
{
    int newpos;

    newpos=pos+move;
    StringListSwap(playlist, pos, newpos);

    return(newpos);
}


TStringList *PlaylistExpandCurr(TStringList *playlist, const char *URL, const char *LocalPath)
{
    TStringList *Items;
    const char *ptr;
    int size, item_count, pos, i;

    pos=playlist->next-1;
    Items=StringListCreate(0, NULL);
    for (i=0; i < pos; i++)
    {
        StringListAdd(Items, StringListGet(playlist, i));
    }
    PlaylistLoad(Items, LocalPath);

    for (i=pos+1; i < StringListSize(playlist); i++)
    {
        StringListAdd(Items, StringListGet(playlist, i));
    }
    Items->next=0;

    return(Items);
}


void PlaylistLoadFromURL(const char *URL, const char *LocalPath)
{
    TStringList *NewPlaylist;

    NewPlaylist=PlaylistExpandCurr(Config->playlist, URL, LocalPath);
    StringListDestroy(Config->playlist);
    Config->playlist=NewPlaylist;
    PlaylistOSDUpdate();
}



TPlaylistItem *PlaylistDecodeEntry(const char *info)
{
    char *Path=NULL, *Tempstr=NULL, *UnQuote=NULL;
    TPlaylistItem *PI;
    const char *ptr;

    PI=(TPlaylistItem *) calloc(1, sizeof(TPlaylistItem));
    if (info != NULL)
    {
        ptr=rstrtok(info, " ", &Path);
        PI->URL=rstrunquot(PI->URL, Path);

        while (ptr)
        {
            ptr=rstrtok(ptr, " ", &Tempstr);
            if (StrLen(Tempstr))
            {
                if (strncmp(Tempstr, "id=", 3)==0)
                {
                    PI->ID=rstrcpy(PI->ID, Tempstr+3);
                    StripQuotes(PI->ID);
                }
                else if (strncmp(Tempstr, "title=", 6)==0)
                {
                    UnQuote=rstrcpy(UnQuote, Tempstr+6);
                    StripQuotes(UnQuote);
                    PI->Title=rstrunquot(PI->Title, UnQuote);
                }
                else
                {
                    PI->Title=rstrunquot(PI->Title, cbasename(Tempstr));
                }
            }
        }

        if (StrLen(PI->Title)==0) PI->Title=rstrunquot(PI->Title, cbasename(Path));
    }

    destroy(Tempstr);
    destroy(UnQuote);
    destroy(Path);

    return(PI);
}





char *PlaylistCurrTitle(char *RetStr)
{
    TPlaylistItem *PI;

    RetStr=rstrcpy(RetStr, "");
    if (Config->stream) RetStr=rstrcpy(RetStr, xine_get_meta_info(Config->stream, XINE_META_INFO_TITLE));
    if (StrLen(RetStr)==0)
    {
        PI=PlaylistDecodeEntry(StringListCurr(Config->playlist));
        RetStr=rstrcpy(RetStr, PI->Title);
        if (StrLen(RetStr) ==0) RetStr=rstrcpy(RetStr, cbasename(PI->URL));
        PlaylistItemDestroy(PI);
    }


    return(RetStr);
}


//This function does some setting up work on the initial playlist loaded from
//command-line args
void PlaylistInit(TStringList *playlist)
{
    char *URL=NULL;
    TPlaylistItem *PI;
    int i;

    if (Config->flags & CONFIG_PLAYLIST)
    {
        for (i=0; i < StringListSize(playlist); i++)
        {
            PI=PlaylistDecodeEntry(StringListGet(Config->playlist, i));
            URL=rstrcpy(URL, PI->URL);
            DownloadProcess(&URL, PI->ID, DOWNLOAD_PLAY);
            PlaylistItemDestroy(PI);
        }
    }

    destroy(URL);
}

