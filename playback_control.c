
/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "playback_control.h"
#include "now_playing.h"
#include "bookmarks.h"
#include "download.h"
#include "playlist.h"
#include "playlist_files.h"
#include "plugins.h"
#include "xine/xine_internal.h"
#include "playlist_osd.h"
#include "media_info_osd.h"
#include "audio_drivers.h"
#include "X11.h"

static void CXineStreamInitConfig(TConfig *Config)
{
    if ((Config->flags & CONFIG_MUTE) && (! xine_get_param (Config->stream, XINE_PARAM_AUDIO_MUTE))) CXineMute(Config->stream, 1);
    else CXineMute(Config->stream, 0);

    if (Config->flags & CONFIG_NOVIDEO) xine_set_param (Config->stream, XINE_PARAM_IGNORE_VIDEO, 1);
    if (Config->flags & CONFIG_HALFSPEED) xine_set_param (Config->stream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_2);
    if (Config->flags & CONFIG_QUARTSPEED) xine_set_param (Config->stream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_4);
    if (Config->flags & CONFIG_PAUSE) xine_set_param (Config->stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    if (Config->flags & CONFIG_LOUD) xine_set_param (Config->stream, XINE_PARAM_AUDIO_AMP_LEVEL, 150);
    if (Config->flags & CONFIG_VLOUD)
    {
        xine_set_param (Config->stream, XINE_PARAM_AUDIO_VOLUME, 100);
        xine_set_param (Config->stream, XINE_PARAM_AUDIO_AMP_LEVEL, 200);
    }

    xine_set_param (Config->stream, XINE_PARAM_AUDIO_COMPR_LEVEL, Config->audio_compression);
    if (Config->zoom > 0)
    {
        xine_set_param (Config->stream, XINE_PARAM_VO_ZOOM_X, Config->zoom);
        xine_set_param (Config->stream, XINE_PARAM_VO_ZOOM_Y, Config->zoom);
    }

//if (Config->brightness > -1) xine_set_param (stream, XINE_PARAM_VO_BRIGHTNESS, Config->brightness);
//if (Config->contrast > -1) xine_set_param (stream, XINE_PARAM_VO_CONTRAST, Config->contrast);

    /*
    #define XINE_PARAM_VO_HUE                  0x01000002
    #define XINE_PARAM_VO_SATURATION           0x01000003
    #define XINE_PARAM_VO_CONTRAST             0x01000004
    #define XINE_PARAM_VO_BRIGHTNESS           0x01000005
    #define XINE_PARAM_VO_GAMMA                0x0100000c
    */

}

void CXineEventSend(TConfig *Config, int type)
{
    xine_event_t event;

    event.stream=Config->stream;
    event.type=type;
    event.data=NULL;
    event.data_length=0;
    gettimeofday(&(event.tv), NULL);
    xine_event_send(Config->stream, &event);
}


void CXineSwitchAudioChannel(xine_stream_t *stream, int skip)
{
    int pos;

    pos=xine_get_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);

    xine_set_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, pos + skip);
}


void CXineMute(xine_stream_t *stream, int Setting)
{
    int val;

    val=xine_get_param (stream, XINE_PARAM_AUDIO_AMP_MUTE);
    if (Setting==TOGGLE)
    {
        if (val==0) val=1;
        else val=0;
    }
    else val=Setting;

    xine_set_param (stream, XINE_PARAM_AUDIO_MUTE, val);
    xine_set_param (stream, XINE_PARAM_AUDIO_AMP_MUTE, val);
}

void CXineSetPos(xine_stream_t *stream, int skip)
{
    int val, pos_msecs, len_msecs;
    long speed;

    speed=xine_get_param(Config->stream, XINE_PARAM_FINE_SPEED);

    xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
    val = pos_msecs + skip;
    if (val > len_msecs) val=len_msecs - SKIP_SMALL;
    if (val < 0) val=0;
    xine_play(stream, 0, val);

    //have to reset speed because xine_play defaults back to normal speed
    xine_set_param(Config->stream, XINE_PARAM_FINE_SPEED, speed);
}


int CXinePause(TConfig *Config)
{
    int val, fine;
    static int prev_speed=0;

    val=xine_get_param(Config->stream, XINE_PARAM_SPEED);
    if (val == XINE_SPEED_PAUSE) val=XINE_SPEED_NORMAL;
    else
    {
        prev_speed=xine_get_param(Config->stream, XINE_PARAM_FINE_SPEED);
        val=XINE_SPEED_PAUSE;
    }
    xine_set_param (Config->stream, XINE_PARAM_SPEED, val);


    if (val == XINE_SPEED_NORMAL) xine_set_param(Config->stream, XINE_PARAM_FINE_SPEED, prev_speed);
    //pause seems to lose its place in the audio stream, so
    //we seek back slightly to resync
    //if (val==XINE_SPEED_NORMAL) CXineSetPos(Config->stream, -1);


    return(val==XINE_SPEED_PAUSE);
}


int CXineIsPaused(TConfig *Config)
{
    int val;

    val=xine_get_param (Config->stream, XINE_PARAM_SPEED);
    return(val==XINE_SPEED_PAUSE);
}


void CXinePlaybackEnd()
{
    if (xine_get_status(Config->stream) == XINE_STATUS_PLAY)  xine_stop(Config->stream);
    xine_close(Config->stream);
    CXineCloseAudio();
}


void CXineSetRangeValue(xine_stream_t *stream, int Type, int SetType, int Value)
{
    int val;

    val=xine_get_param (stream, Type);
    if (SetType==SET_ADD) val+=Value;
    else val=Value;
    if ((Type==XINE_PARAM_AUDIO_COMPR_LEVEL) && (val < 100)) val=100;
    xine_set_param (stream, Type, val);
}



void CXineNewTitle(TConfig *Config)
{
    CXineNowPlaying(Config);

    //if (Config->vo_port) CXineAddVideoPostPlugins(Config->xine, Config->stream, &Config->ao_port, &Config->vo_port);
    Config->state &= ~STATE_NEWTITLE;
}


int CXinePlayStream(TConfig *Config, const char *info)
{
    char *Tempstr=NULL, *URL=NULL;
    TPlaylistItem *PI;
    const char *ptr=NULL;
    int startms;
    int len, result=0, fd;
    TStringList *NewPlaylist;

    PI=PlaylistDecodeEntry(info);
    URL=rstrcpy(URL, PI->URL);
    len=StrLen(URL);
    if (len >0)
    {
        //xine interprets anything starting with '-' to mean 'stdin', and sits there trying to read from stdin.
        //so if we get a file path starting with '-' (probably a command-line option that we don't recognize)
        //then we don't want to pass it to xine unless the file really exists
        if ((*URL=='-') && (len > 1) && (access(URL, F_OK) !=0)) /* do nothing*/ ;
        else if (DownloadProcess(&URL, PI->ID, DOWNLOAD_PLAY)==DOWNLOAD_ACTIVE)
        {
            Config->state |= STATE_DOWNLOADING;
        }
        else if (IsPlaylist(URL))
        {
            PlaylistLoadFromURL(Tempstr, URL);
            Config->state &= ~STATE_DOWNLOADING;
        }
        else if (xine_open(Config->stream, URL))
        {
            TouchFile(URL);
            Config->state &= ~STATE_DOWNLOADING;
            if (StrLen(PI->Title)) Config->CurrTitle=rstrcpy(Config->CurrTitle, PI->Title);
            else Config->CurrTitle=rstrcpy(Config->CurrTitle, cbasename(URL));

            //do this before calling play..
            CXineStreamInitConfig(Config);

            startms=Config->startms;
            if ((startms==0) && xine_get_stream_info(Config->stream, XINE_STREAM_INFO_SEEKABLE))
            {
                startms=LoadBookmark(URL);
            }

            CXineNewTitle(Config);
            CXineOpenAudio();

            if (xine_play(Config->stream, 0, startms))
            {
                //there is a delay before all this happens, I'm not sure why. It's as though the calling thread allows
                //some time for the playing thread launched by 'xine_play' to get a head start or something. Hence
                //XineNewTitle must be called *before* xine_play

                //and again after, because some configs require the stream to be playing to take effect (pause)
                CXineStreamInitConfig(Config);
                Config->state &= ~STATE_BACKGROUND_DISPLAYED;
                Config->state |= STATE_PLAYING;
                if (strncmp(URL,"stdin:",6)==0) Config->state |= STATE_STDIN_URL;
                else Config->state &= ~STATE_STDIN_URL;
                Config->state |= STATE_NEWTITLE;
                result=1;
                PlaylistOSDUpdate();
                InfoOSDUpdate();
            }
            else
            {
                printf("Unable to play URL '%s'\n", URL);
                result=PLAY_FAIL;
            }
        }
        else
        {
            printf("Unable to open URL '%s'\n", URL);
            result=PLAY_FAIL;
            Config->state &= ~STATE_DOWNLOADING;
        }
    }

    PlaylistItemDestroy(PI);

    destroy(URL);
    destroy(Tempstr);

    return(result);
}


int CXineSelectStream(TConfig *Config, int Which)
{
    const char *ptr;
    int speed=0, result;

    if (Config->state & STATE_DOWNLOADING)
    {
        return(CXinePlayStream(Config, StringListCurr(Config->playlist)));
    }

    //bookmark current stream
    ptr=StringListCurr(Config->playlist);
    if (StrLen(ptr) && Config->stream)
    {
        SaveBookmark(ptr, Config->stream);
    }

    if (Which==PLAY_NEXT)
    {
        ptr=StringListNext(Config->playlist);
        if ( (ptr==NULL)  )
        {
            //if it's -1 then this won't happen. -1 means 'loop forever'
            if (Config->loop > 0) Config->loop--;
            if (Config->loop !=0) ptr=StringListGet(Config->playlist, 0);
        }
    }
    else if (Which==PLAY_PREV)
    {
        ptr=StringListPrev(Config->playlist);
    }
    else ptr=StringListGet(Config->playlist, Which);

    if (ptr)
    {
        CXinePlaybackEnd();
        result=CXinePlayStream(Config, ptr);
        if (Config->speed > 0) xine_set_param (Config->stream, XINE_PARAM_FINE_SPEED, Config->speed);
        return(result);
    }
    return(0);
}

