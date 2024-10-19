/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "now_playing.h"
#include "playlist.h"
#include "X11.h"


/* String Values used in 'now playing'
ID_AUDIO_ID=0
Audio only file format detected.
Clip info:
 Title: Asian Vibes
ID_CLIP_INFO_NAME0=Title
ID_CLIP_INFO_VALUE0=Asian Vibes
 Artist: Joi
ID_CLIP_INFO_NAME1=Artist
ID_CLIP_INFO_VALUE1=Joi
 Album: One and One is One
ID_CLIP_INFO_NAME2=Album
ID_CLIP_INFO_VALUE2=One and One is One
 Year:
ID_CLIP_INFO_NAME3=Year
ID_CLIP_INFO_VALUE3=
 Comment:
ID_CLIP_INFO_NAME4=Comment
ID_CLIP_INFO_VALUE4=
 Track: 3
ID_CLIP_INFO_NAME5=Track
ID_CLIP_INFO_VALUE5=3
 Genre: Other
ID_CLIP_INFO_NAME6=Genre
ID_CLIP_INFO_VALUE6=Other
ID_CLIP_INFO_N=7
Load subtitles in /home/colum/
ID_FILENAME=/home/colum/Asian Vibes.mp3
ID_DEMUXER=audio
ID_AUDIO_FORMAT=85
ID_AUDIO_BITRATE=128000
ID_AUDIO_RATE=44100
ID_AUDIO_NCH=0
ID_START_TIME=0.00
ID_LENGTH=346.00
ID_SEEKABLE=1
ID_CHAPTERS=0
*/


static const char *CXineGetStringValue(xine_stream_t *stream, int ValID, int Quote)
{
    const char *ptr;

    ptr=xine_get_meta_info(Config->stream, ValID);
    if (! ptr) return("");
    return(ptr);
}


char *CXineGetCurrTrack(char *RetStr)
{
if (StrLen(Config->curr_title)) RetStr=rstrcpy(RetStr, Config->curr_title);
if (StrLen(Config->curr_subitem)) 
{
RetStr=rstrcat(RetStr, ": ");
RetStr=rstrcat(RetStr, Config->curr_subitem);
}

return(RetStr);
}


void NowPlayingSetWindowTitle(const char *WindowTitle, const char *TrackName)
{
X11WindowSetTitle(Config->X11Out, WindowTitle, "cxine");
if (Config->flags & CONFIG_XTERM) printf("\033]0;cxine: %s\007", WindowTitle);
if (StrLen(TrackName)) printf("\rnow playing: %s\n", TrackName);
}


void NowPlayingNewFile(TConfig *Config)
{
    const char *ClipInfoTypes[]= {"Title","Artist","Album","Year","Comment","Track","Genre","Video", "Audio", NULL};
    int ClipInfoID[]= {XINE_META_INFO_TITLE, XINE_META_INFO_ARTIST, XINE_META_INFO_ALBUM, XINE_META_INFO_YEAR, XINE_META_INFO_COMMENT, XINE_META_INFO_TRACK_NUMBER, XINE_META_INFO_GENRE, XINE_META_INFO_VIDEOCODEC, XINE_META_INFO_AUDIOCODEC};
    const char *ptr;
    int i, pos_msecs, len_msecs, pos;
    char *Tempstr=NULL, *Title=NULL, *PipeStr=NULL;


   printf("\nClip info:\n");
    for (i=0; ClipInfoTypes[i] !=NULL; i++)
    {
        if (ClipInfoID[i]==XINE_META_INFO_TITLE)
        {
            Title=PlaylistCurrTitle(Title);
            ptr=Title;
        }
        else ptr=CXineGetStringValue(Config->stream, ClipInfoID[i], 0);


        if (ptr && (*ptr != '\0')) printf(" %s: %s\n",ClipInfoTypes[i], ptr);
        Tempstr=rstrcpy(Tempstr, "CXINE:");
        Tempstr=rstrcat(Tempstr, ClipInfoTypes[i]);

        X11SetTextProperty(Config->X11Out, Tempstr, ptr);

        PipeStr=rstrcat(PipeStr, ClipInfoTypes[i]);
        PipeStr=rstrcat(PipeStr, "='");
        PipeStr=rstrcat(PipeStr, ptr);
        PipeStr=rstrcat(PipeStr, "' ");

        if (Config->flags & CONFIG_IDENTIFY)
        {
            printf("ID_CLIP_INFO_NAME%d=%s\n",i,ClipInfoTypes[i]);
            printf("ID_CLIP_INFO_VALUE%d=%s\n",i,ptr);
        }
    }
		printf("\n");

		NowPlayingSetWindowTitle(Title, Title);
    PipeStr=rstrcat(PipeStr, "\n");
    if (Config->nowplay_pipe > 0) write(Config->nowplay_pipe, PipeStr, StrLen(PipeStr));

    xine_get_pos_length(Config->stream, &pos, &pos_msecs, &len_msecs);
    ptr=StringListCurr(Config->playlist);

    rstrtok(ptr, " ", &Tempstr);
    X11SetTextProperty(Config->X11Out, "CXINE:Filename", Tempstr);

    if (Config->flags & CONFIG_IDENTIFY)
    {
        printf("ID_FILENAME=%s\n", Tempstr);
        printf("ID_AUDIO_BITRATE=%d\n", xine_get_stream_info(Config->stream, XINE_STREAM_INFO_AUDIO_BITRATE));
        printf("ID_AUDIO_RATE=%d\n", xine_get_stream_info(Config->stream, XINE_STREAM_INFO_AUDIO_SAMPLERATE));
        printf("ID_SEEKABLE=%d\n", xine_get_stream_info(Config->stream, XINE_STREAM_INFO_SEEKABLE));
        printf("ID_CHAPTERS=%d\n", xine_get_stream_info(Config->stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT));
        printf("ID_LENGTH=%0.2f\n", ((float) len_msecs) / 1000.0);
    }


    destroy(Tempstr);
    destroy(PipeStr);
    destroy(Title);
}


void NowPlayingNewTitle(TConfig *Config)
{
char *Tempstr=NULL, *TrackName=NULL;

		if (StrLen(Config->curr_subitem))
		{
		TrackName=CXineGetCurrTrack(TrackName);
		NowPlayingSetWindowTitle(TrackName, Config->curr_subitem);
    X11SetTextProperty(Config->X11Out, "CXINE:Title", TrackName);

		Tempstr=rstrcpy(Tempstr, "Title='");
		Tempstr=rstrcat(Tempstr, TrackName);
		Tempstr=rstrcat(Tempstr, "'\n");
    if (Config->nowplay_pipe > 0) write(Config->nowplay_pipe, Tempstr, StrLen(Tempstr));

    //if (Config->vo_port) CXineAddVideoPostPlugins(Config->xine, Config->stream, &Config->ao_port, &Config->vo_port);
    Config->state &= ~STATE_NEWTITLE;
		}

destroy(TrackName);
destroy(Tempstr);
}


