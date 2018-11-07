#include "playlist.h"
#include "playlist_files.h"
#include "download.h"

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


void PlaylistAdd(const char *iURL, const char *Title)
{
char *Tempstr=NULL, *URL=NULL;

URL=rstrcpy(URL, iURL);
while (! DownloadDone(&URL));

if ( ! PlaylistLoad(Config->playlist, URL) )
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
	StringListAdd(Config->playlist, Tempstr);
}


destroy(Tempstr);
destroy(URL);
}

