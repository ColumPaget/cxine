#include "playlist.h"
#include "playlist_files.h"
#include "download.h"
#include "osd.h"

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
char *Quoted=NULL;

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
	Quoted=rstrcpy(Quoted, Title);
	StripQuotes(Quoted);
	RetStr=rstrcat(RetStr, Quoted);
	RetStr=rstrcat(RetStr, "'");
}

destroy(Quoted);
return(RetStr);
}


void PlaylistAdd(TStringList *playlist, const char *iURL, const char *ID, const char *Title)
{
    char *Tempstr=NULL, *URL=NULL;
    CXineOSD *OSD=NULL;

    URL=rstrcpy(URL, iURL);
    if (IsPlaylist(URL))
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



void PlaylistParseEntry(const char *info, char **URL, char **ID, char **Title)
{
char *Tempstr=NULL;
const char *ptr;

  ptr=rstrtok(info, " ", &Tempstr);
  *URL=rstrunquot(*URL, Tempstr);
  while (ptr)
  {
    ptr=rstrtok(ptr, " ", &Tempstr);
    if (StrLen(Tempstr))
    {
	    if (strncmp(Tempstr, "id=", 3)==0)
			{
				if (ID) 
				{
					*ID=rstrcpy(*ID, Tempstr+3);
					StripQuotes(*ID);
				}
			}
	    else if (strncmp(Tempstr, "title=", 6)==0) 
			{
				if (Title) 
				{
					*Title=rstrcpy(*Title, Tempstr+6);
					StripQuotes(*Title);
				}
			}
	    else 
			{
				if (Title) *Title=rstrcpy(*Title, Tempstr);
			}
    }
  }

destroy(Tempstr);
}



char *PlaylistCurrTitle(char *RetStr)
{
    char *URL=NULL;

		RetStr=rstrcpy(RetStr, "");
    if (Config->stream) RetStr=rstrcpy(RetStr, xine_get_meta_info(Config->stream, XINE_META_INFO_TITLE));
    if (StrLen(RetStr)==0)
    {
				PlaylistParseEntry(StringListCurr(Config->playlist), &URL, NULL, &RetStr);
        if (StrLen(RetStr) ==0) RetStr=rstrcpy(RetStr, cbasename(URL));
    }

    destroy(URL);

    return(RetStr);
}
