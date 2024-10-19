/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

/*
** Copyright (C) 2018 Colum Paget <colums.projects@gmail.com>
** This program was built upon/influenced by 'muxine.c' https://xine-project.org/samplecode
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**
*/

#include "common.h"
#include <sys/time.h>
#include <signal.h>

#include "X11.h"
#include "config.h"
#include "bookmarks.h"
#include "osd.h"
#include "plugins.h"
#include "audio_drivers.h"
#include "keypress.h"
#include "download.h"
#include "download_osd.h"
#include "playlist.h"
#include "playlist_osd.h"
#include "playback_control.h"
#include "control_protocol.h"
#include "now_playing.h"
#include "command_line.h"
#include "splashscreen.h"
#include "stdin_fd.h"
#include <sys/resource.h>
#include <sys/wait.h>
#include <pwd.h>
#include <xine/broadcaster.h>
#include <xine/xineutils.h>
#include <errno.h>

#define DEFAULT_CACHE_DIR "~/.cxine/cache"





void SignalHandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
    case SIGTERM:
        running=0;
        break;
    }

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
}


static void event_listener(void *user_data, const xine_event_t *event)
{
    xine_ui_data_t *msg;

    switch(event->type)
    {
    case XINE_EVENT_UI_PLAYBACK_FINISHED:
        if (Config) Config->state &= ~STATE_PLAYING;
//    running = 0;
        break;

    case XINE_EVENT_UI_SET_TITLE:
        msg=(xine_ui_data_t *) event->data;
        Config->curr_subitem=rstrlcpy(Config->curr_subitem, msg->str, msg->str_len);
        Config->state |= STATE_NEWTITLE;
        break;

    case XINE_EVENT_UI_NUM_BUTTONS:
        msg=(xine_ui_data_t *) event->data;
        Config->DVDNavButtons=msg->num_buttons;
        break;
    }

}


static void cxine_set_background(int startup)
{
    if (StrLen(Config->background))
    {
        if (xine_open(Config->stream, Config->background))
        {
            xine_play(Config->stream, 0, 0);
        }
        Config->state |= STATE_BACKGROUND_DISPLAYED;
    }
    else if (startup && (! (Config->flags & (CONFIG_READ_STDIN | CONFIG_CONTROL)))) CxineInjectSplashScreen(Config->xine);
}



//This to do every so many ms
void PeriodicProcessing()
{
    char *URL=NULL;
    TPlaylistItem *PI;
    const char *ptr;
    int pos, DoDownload;

// Sweep through playlist and start downloading things that need downloading
    if (Config->playlist && (! (Config->flags & CONFIG_STREAM)))
    {
        pos=Config->playlist->next;
        ptr=StringListFirst(Config->playlist);
        while (ptr)
        {
            PI=PlaylistDecodeEntry(ptr);
            if (Config->flags & CONFIG_NOAUTOPLAY) DoDownload=FALSE;
            else DoDownload=TRUE;

            URL=rstrcpy(URL, PI->URL);
            if (DownloadProcess(&URL, PI->ID, DoDownload)==DOWNLOAD_ACTIVE) break;
            PlaylistItemDestroy(PI);

            ptr=StringListNext(Config->playlist);
        }
        Config->playlist->next=pos;
    }

// if current item is an image, then it must have had it's display time
// so mark us as not playing
    ptr=xine_get_meta_info(Config->stream, XINE_STREAM_INFO_VIDEO_FOURCC);
    if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) Config->state &= ~STATE_PLAYING;


//if there's not a 'nowplaying' pipe, perhaps because a client disconnected, then open one
    if ((Config->nowplay_pipe==-1) && (StrLen(Config->nowplay_pipe_path))) Config->nowplay_pipe=open(Config->nowplay_pipe_path, O_CREAT |O_WRONLY | O_NONBLOCK, 0660);

		//if (Config->state & STATE_NEWTITLE) NowPlayingNewTitle(Config);
    OSDUpdate((Config->flags & CONFIG_OSD) && (! (Config->state & STATE_PLAYLIST_DISPLAYED)));

    if (Config->state & STATE_DOWNLOADING) DownloadOSDDisplay();

    destroy(URL);
}


int FDCopyBytes(int in, int out)
{
    char Buffer[1024];
    int result;

    result=read(in, Buffer, 1024);
    write(out, Buffer, result);
    return(result);
}


int WatchFileDescriptors(TConfig *Config, int stdin_fd, int control_pipe)
{
    fd_set select_set;
    int high_fd=0, display_fd=-1, result, sleep_ms;
    static struct timeval *tv=NULL;
    const char *ptr;
    TEvent Event;

    Event.type=EVENT_NONE;

    //decide how log we will wait in 'select' for
    ptr=xine_get_meta_info(Config->stream, XINE_STREAM_INFO_VIDEO_FOURCC);
    if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) sleep_ms=Config->image_ms;
    else sleep_ms=200;


//first time around we will sleep for 0 ms, but tv is a static, so from then on we will sleep for SLEEP_TIME
    if (! tv) tv=(struct timeval *) calloc(1,sizeof(struct timeval));

    FD_ZERO(&select_set);


    if ((stdin_fd > -1) && (! (Config->state & STATE_STDIN_URL)) && (fcntl(stdin_fd, F_GETFD) > -1))
    {
        FD_SET(stdin_fd, &select_set);
        if (stdin_fd > high_fd) high_fd=stdin_fd;
    }

    if (control_pipe && (fcntl(control_pipe, F_GETFD) > -1))
    {
        FD_SET(control_pipe, &select_set);
        if (control_pipe > high_fd) high_fd=control_pipe;
    }

    display_fd=X11GetFileDescriptor(Config->X11Out);
    if (fcntl(display_fd, F_GETFD) > -1)
    {
        FD_SET(display_fd, &select_set);
        if (display_fd > high_fd) high_fd=display_fd;
    }


    result=select(high_fd+1, &select_set, NULL, NULL, tv);
    if (result > 0)
    {
        if ((stdin_fd > -1) && FD_ISSET(stdin_fd, &select_set))
        {
            //if in slave mode we read strings from StdIn with 'ControlHandleInput'
            if (Config->flags & CONFIG_SLAVE)
            {
                if (ControlHandleInput(stdin_fd, Config->stream) ==EVENT_CLOSE)
                {
                    close(stdin_fd);
                    stdin_fd=-1;
                }
            }
            //if NOT in slave mode, we read keypresses from stdin
            else if (Config->flags & CONFIG_CONTROL)
            {
                KeypressHandleStdIn(stdin_fd, Config->stream);
            }
            else
            {
                result=FDCopyBytes(stdin_fd, Config->to_xine);
                if (result < 1)
                {
                    close(stdin_fd);
                    stdin_fd=-1;
                }
            }
        }

        if ((control_pipe > -1) && FD_ISSET(control_pipe, &select_set))
        {
            if (ControlHandleInput(control_pipe, Config->stream) == EVENT_CLOSE)
            {
                close(control_pipe);
                control_pipe=-1;
            }
        }

        if ((display_fd > -1) && FD_ISSET(display_fd, &select_set))
        {
            X11NextEvent(Config->X11Out, Config->vo_port, &Event);
            if (Event.type==EVENT_KEYPRESS) HandleKeyPress(Config->X11Out, Config->stream, Event.arg1, Event.arg2);
            if (Event.type==EVENT_RESIZE)
            {
            }
            if (Event.type==EVENT_CLOSE) running=0;
        }
    }
    else if ((tv->tv_usec == 0) || (errno==ETIMEDOUT))
    {
        tv->tv_usec=sleep_ms * 1000;
        time(&Now);
        return(EVENT_TIMEOUT);
    }

    return(Event.type);
}




void OutputAccellerationTypes()
{
    uint32_t val;

    printf("accelleration: ");

    val=xine_mm_accel();
    if (val & MM_ACCEL_MLIB) printf("mlib, ");
    if (val & MM_ACCEL_X86_MMX) printf("mmx, ");
    if (val & MM_ACCEL_X86_3DNOW) printf("3Dnow, ");
    if (val & MM_ACCEL_X86_SSE) printf("sse, ");
    if (val & MM_ACCEL_X86_SSE2) printf("sse2, ");

//version of libxine in ubuntu trusty seems to lack all the below defines
#ifdef MM_ACCEL_X86_SSE3
    if (val & MM_ACCEL_X86_SSE3) printf("sse3, ");
#endif

#ifdef MM_ACCEL_X86_SSE4
    if (val & MM_ACCEL_X86_SSE4) printf("sse4, ");
#endif

#ifdef MM_ACCEL_X86_SSE42
    if (val & MM_ACCEL_X86_SSE42) printf("sse4.2, ");
#endif

#ifdef MM_ACCEL_X86_AVX
    if (val & MM_ACCEL_X86_AVX) printf("avx, ");
#endif
//if (val & MM_ACCEL_PPC_ALTIVEC) printf("ppc_altivec, ");
//if (val & MM_ACCEL_PPC_CACHE32) printf("ppc_cache32, ");

    printf("\n");
}


void CXineSwitchUser()
{
    uid_t real, effective, saved;
    int result=-1;

    getresuid(&real, &effective, &saved);
    //if we are root, or effectively root, then switch to another user
    if (effective==0)
    {
        if (real != 0) result=setresuid(real, real, real);
        if ((real==0) || (result != 0))
        {
            if (saved != 0) result=setresuid(saved, saved, saved);
            if (result !=0)
            {
                printf("WARNING: cannot switch to a saved user, you are running this app as root\n");
            }
        }
    }

}






void CXinePrintOutputs(xine_t *xine, xine_stream_t *stream)
{
    xine_post_out_t *out;

    out=xine_get_video_source(Config->stream);
    if (out) printf("vid: %s\n",out->name);
    out=xine_get_audio_source(Config->stream);
    if (out) printf("aud: %s\n",out->name);
}







void CXineExit(TConfig *Config)
{
    //will only save bookmark if stream is still playing
    SaveBookmark(StringListCurr(Config->playlist), Config->stream);

    CXinePlaybackEnd();
    xine_event_dispose_queue(Config->event_queue);
    xine_dispose(Config->stream);
    if (Config->vo_port)  xine_close_video_driver(Config->xine, Config->vo_port);
    xine_exit(Config->xine);
    X11Close(Config->X11Out);
    StdInReset();
}


char *CXineFormatXineList(char *RetStr, const char * const *List)
{
    const char * const *ptr;

    RetStr=rstrcpy(RetStr, "");

    for (ptr=List; *ptr !=NULL; ptr++)
    {
        if (StrLen(RetStr)) RetStr=rstrcat(RetStr, ", ");
        RetStr=rstrcat(RetStr, *ptr);
    }

    return(RetStr);
}

void CXineShowSystemSetup()
{
    char *Tempstr=NULL;

    OutputAccellerationTypes();
    Tempstr=CXineFormatXineList(Tempstr, xine_list_audio_output_plugins(Config->xine));
    printf("Audio Drivers:  %s\n", Tempstr);

    Tempstr=CXineFormatXineList(Tempstr, xine_list_video_output_plugins(Config->xine));
    printf("Video Drivers:  %s\n", Tempstr);

    destroy(Tempstr);
}


int main(int argc, char **argv)
{
    int control_pipe=-1, result;
    broadcaster_t *bcast=NULL;

    //call 'SignalHandler' with a signal it ignores as it will set up
    //handlers for SIGINT and SIGTERM
    SignalHandler(SIGCONT);

    Config=ConfigInit(xine_new());
    xine_init(Config->xine);
    ParseCommandLine(argc, argv, Config);

    DownloadAddHelpers(Config->helpers);
    if (Config->flags & CONFIG_SHUFFLE) PlaylistShuffle();
    if (Config->flags & CONFIG_SAVE) CXineConfigSave(Config);
    DownloadCleanCacheDir();
    PlaylistInit(Config->playlist);

    signal(SIGPIPE, SIG_IGN);


    if (Config->priority > 0) setpriority(PRIO_PROCESS, getpid(), Config->priority - 21);

    if (Config->debug > 0) xine_engine_set_param(Config->xine, XINE_ENGINE_PARAM_VERBOSITY, Config->debug);

    CXineShowSystemSetup();

    control_pipe=ControlPipeOpen(O_RDWR | O_NONBLOCK);

    if (strcmp(Config->vo_driver, "none")==0)
    {
        printf("vo_driver: none. No video output\n");
        if (
            (! (Config->flags & (CONFIG_SLAVE | CONFIG_READ_STDIN))) &&
            (isatty(0))
        ) Config->flags |= CONFIG_CONTROL;
    }
    else
    {
        Config->X11Out=X11Init(Config->parent, 0, 0, Config->width, Config->height);
        Config->vo_port=X11BindCXineOutput(Config);
    }

    Config->ao_port = CXineOpenAudioDriver("none");
    Config->stream=xine_stream_new(Config->xine, Config->ao_port, Config->vo_port);
    Config->event_queue = xine_event_new_queue(Config->stream);
    xine_event_create_listener_thread(Config->event_queue, event_listener, NULL);

    if (Config->bcast_port > 0) bcast=_x_init_broadcaster(Config->stream, Config->bcast_port);

    if (Config->vo_port)
    {
        X11ActivateCXineOutput(Config->X11Out, Config->vo_port);
        OSDSetup(Config);
    }


    //open stdin late, as X11 setup above can override its use
    if (Config->flags & (CONFIG_CONTROL | CONFIG_SLAVE | CONFIG_READ_STDIN))
    {
        Config->stdin=dup(0);
        if (Config->flags & CONFIG_CONTROL) StdInSetup();
    }
    //don't try to use stdin if our setyp doesn't explicitly need it
    else Config->stdin=-1;


    //must do this after open stdin, as injecting splashscreen uses stdin
    if (Config->vo_port) cxine_set_background(TRUE);

    CXineSwitchUser();
    KeyGrabsSetup(Config->X11Out);

    CXinePrintOutputs(Config->xine, Config->stream);
//	xine_set_param (Config->stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);


    running = 1;
    while(running)
    {
        if (Config->state & STATE_PLAYLIST_REQUESTED)
        {
            PlaylistOSDShow();
            Config->state &= ~STATE_PLAYLIST_REQUESTED;
        }

        if (Config->state & STATE_PLAYING)
        {
            DownloadOSDHide();
        }
        else
        {
            if (! (Config->state & STATE_BACKGROUND_DISPLAYED)) cxine_set_background(FALSE);

            if (! (Config->flags & CONFIG_NOAUTOPLAY))
            {
                result=CXineSelectStream(Config, PLAY_NEXT);

                if  ((! result) && (! (Config->state & STATE_DOWNLOADING)))
                {
                    //we are at the end
                    if ((StringListPos(Config->playlist)+1) >= StringListSize(Config->playlist))
                    {
                        if (! (Config->flags & CONFIG_PERSIST)) running=0;
                    }
                }
            }
        }


        result=WatchFileDescriptors(Config, Config->stdin, control_pipe);
        switch (result)
        {
        case EVENT_RESIZE:
            OSDSetup(Config);
            break;

        case EVENT_TIMEOUT:
            PeriodicProcessing();
            break;
        }

        waitpid(-1, NULL, WNOHANG);
    }

    if (bcast) _x_close_broadcaster(bcast);
    CXineExit(Config);

    return(1);
}

