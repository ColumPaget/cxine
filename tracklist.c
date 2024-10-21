//this module relates to tracklistings within a single media file,
//so instances where tracks are concatanated into a 'mix tape'
//it simply reads timestamps and names from a text file, and
//displays the names when a timestamp is reached


#include "tracklist.h"
#include "string_list.h"
#include "download.h"
#include <ctype.h>

TStringList *CurrTrackList=NULL;


int TrackListLoad(const char *MRL)
{
    FILE *F;
    char *Tempstr=NULL;

    if (CurrTrackList)
    {
        StringListDestroy(CurrTrackList);
        CurrTrackList=NULL;
    }

    if (! Config->tracklist) return(FALSE);
    Tempstr=DownloadFormatHelperCommand(Tempstr, Config->tracklist, MRL);

    F=fopen(Tempstr, "r");
    if (F)
    {
        CurrTrackList=StringListCreate(0, NULL);
        Tempstr=realloc(Tempstr, 256);
        while (fgets(Tempstr, 255, F))
        {
            if (strlen(Tempstr) > 0) StringListAdd(CurrTrackList, Tempstr);
        }
        fclose(F);
    }

    destroy(Tempstr);
}


static void TrackListParseTimestamp(const char *Timestamp, int *Mins, int *Secs)
{
    char *Tok=NULL;
    const char *ptr;
    int count=0;
	
    ptr=strchr(Timestamp, ':');
    while (ptr)
    {
    ptr++;
    count++;
    ptr=strchr(ptr, ':');
    }

    ptr=Timestamp;
    if (count > 1) 
    {
    ptr=rstrtok(ptr, ":", &Tok);
    *Mins=atoi(Tok) * 60;
    }

    ptr=rstrtok(ptr, ":", &Tok);
    *Mins=atoi(Tok);
    ptr=rstrtok(ptr, ":", &Tok);
    *Secs=atoi(Tok);

    destroy(Tok);
}


static int TrackListCurr(char **Timestamp, char **Name)
{
    const char *ptr;

    if (! CurrTrackList) return(FALSE);
    if (CurrTrackList->size < 1) return(FALSE);
    if (CurrTrackList->next == 0)
    {
        *Timestamp=rstrcpy(*Timestamp, "0:00");
        *Name=rstrcpy(*Name, "");
        return(TRUE);
    }

    ptr=StringListCurr(CurrTrackList);
    if (! ptr) return(FALSE);

    ptr=rstrtok(ptr, " 	", Timestamp);
    while (isspace(*ptr)) ptr++;
    *Name=rstrcpy(*Name, ptr);

    return(TRUE);
}


int TrackListCheck(const char *Timestamp, char **RetStr)
{
    char *Tempstr=NULL, *Tok=NULL, *TrackName=NULL;
    const char *ptr;
    int CurrMins, CurrSecs, TrackMins=0, TrackSecs=0;
    int Changed=FALSE;

    *RetStr=rstrcpy(*RetStr, "");
    TrackListParseTimestamp(Timestamp, &CurrMins, &CurrSecs);

    while (TrackListCurr(&Tok, &TrackName))
    {
        TrackListParseTimestamp(Tok, &TrackMins, &TrackSecs);

        //if the track is in the future, then don't consider it
        if ((TrackMins > CurrMins)) break;
        if ((TrackMins == CurrMins) && (TrackSecs > CurrSecs)) break;

        //if the track is current, update it's name
        *RetStr=rstrcpy(*RetStr, TrackName);
        if (! StringListNext(CurrTrackList)) break;
        Changed=TRUE;
    }


    destroy(Tempstr);
    destroy(TrackName);
    destroy(Tok);

    return(Changed);
}
