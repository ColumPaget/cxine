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


void PlaylistAdd(TStringList *playlist, const char *iURL, const char *Title)
{
char *Tempstr=NULL, *URL=NULL;
CXineOSD *OSD=NULL;

printf("PLA: [%s] [%s]\n", iURL, Title);
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
			) Tempstr=rstrcpy(Tempstr, Config->path_prefix);
	else Tempstr=rstrcpy(Tempstr, "");

	Tempstr=rstrcat(Tempstr, URL);

	URL=rstrquot(URL, Tempstr, " ");
	Tempstr=rstrcpy(Tempstr, URL);

	if (StrLen(Title))
	{	
		Tempstr=rstrcat(Tempstr, " ");
		Tempstr=rstrcat(Tempstr, Title);
	}

printf("PLA: %s\n",Tempstr);
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
