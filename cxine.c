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
#include "keypress.h"
#include "download.h"
#include "playlist.h"
#include "playback_control.h"
#include "control_protocol.h"
#include "now_playing.h"
#include "command_line.h"
#include <sys/resource.h>
#include <sys/wait.h>
#include <pwd.h>

#define DEFAULT_CACHE_DIR "~/.cxine/cache"

static CXineOSD *DownloadOSD=NULL;


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
        Config->CurrTitle=rstrlcpy(Config->CurrTitle, msg->str, msg->str_len);
        Config->state |= STATE_NEWTITLE;
        CXineNowPlaying(Config);
        break;

    case XINE_EVENT_UI_NUM_BUTTONS:
        msg=(xine_ui_data_t *) event->data;
        Config->DVDNavButtons=msg->num_buttons;
        break;
    }

}









/*
# device used for mono output
# string, default: default
#audio.device.alsa_default_device:default

# device used for stereo output
# string, default: plug:front:default
#audio.device.alsa_front_device:plug:front:default

# sound card can do mmap
# bool, default: 0
#audio.device.alsa_mmap_enable:0

# device used for 5.1-channel output
# string, default: iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2
#audio.device.alsa_passthrough_device:iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2

# device used for 4-channel output
# string, default: plug:surround40:0
#audio.device.alsa_surround40_device:plug:surround40:0

# device used for 5.1-channel output
# string, default: plug:surround51:0
#audio.device.alsa_surround51_device:plug:surround51:0

# OSS audio device name
# { auto  /dev/dsp  /dev/sound/dsp }, default: 0
#audio.device.oss_device_name:auto
*/

xine_audio_port_t *CXineOpenAudioDriver(const char *Spec)
{
    char *Type=NULL, *Tempstr=NULL;
    xine_audio_port_t *ao_port=NULL;
    const char *ptr;
    static const char * const devname_opts[] = {"auto", "/dev/dsp", "/dev/sound/dsp", NULL};

    ptr=rstrtok(Spec,":",&Type);

    if (StrLen(ptr))
    {
        if (strcmp(Type, "oss")==0)
        {
            xine_config_register_enum(Config->xine, "audio.device.oss_device_name", 1, (char **)devname_opts, "OSS audio device name","", 10, NULL, NULL);
            xine_config_register_num(Config->xine, "audio.device.oss_device_number", atoi(ptr), "OSS audio device number, -1 for none","", 10, NULL, NULL);
        }
        else if (strcmp(Type, "alsa")==0)
        {
            Tempstr=rstrcpy(Tempstr,"plug:default:");
            Tempstr=rstrcat(Tempstr,ptr);
            xine_config_register_string(Config->xine, "audio.device.alsa_default_device", Tempstr, "ALSA device for mono output","", 10, NULL, NULL);
            Tempstr=rstrcpy(Tempstr,"plug:front:");
            Tempstr=rstrcat(Tempstr,ptr);
            xine_config_register_string(Config->xine, "audio.device.alsa_front_device", Tempstr, "ALSA device for stereo output","", 10, NULL, NULL);
        }
        else if (strcmp(Type, "jack")==0)
        {
            if (StrLen(ptr)) xine_config_register_string(Config->xine, "audio.device.jack_device_name", ptr, "Jack output device","", 10, NULL, NULL);
        }
        else if (strcmp(Type, "sun")==0)
        {
            xine_config_register_filename(Config->xine, "audio.device.sun_audio_device", ptr, XINE_CONFIG_STRING_IS_DEVICE_NAME, "Sun output device","", XINE_CONFIG_SECURITY, NULL, NULL);
        }
        else if (strcmp(Type, "pulse")==0)
        {
            if (StrLen(ptr)) xine_config_register_string(Config->xine, "audio.device.pulseaudio_device", ptr, "Pulseaudio output device","", 10, NULL, NULL);
        }


    }

    ao_port=xine_open_audio_driver(Config->xine , Type, NULL);
    destroy(Tempstr);
    destroy(Type);

    return(ao_port);
}



xine_audio_port_t *CXineOpenAudio(const char *Audio)
{
    xine_audio_port_t *ao_port=NULL;
    char *Token=NULL;
    const char *ptr;

    ptr=rstrtok(Audio,",",&Token);
    while (ptr)
    {
        ao_port=CXineOpenAudioDriver(Token);
        if (ao_port) break;
        ptr=rstrtok(ptr,",",&Token);
    }

    destroy(Token);
    return(ao_port);
}



void DisplayDownloadProgress()
{
    char *Tempstr=NULL, *Title=NULL, *URL=NULL, *Text=NULL;
    const char *ptr;

    Title=PlaylistCurrTitle(Title);

    ptr=StringListCurr(Config->playlist);
    if (ptr)
    {
        rstrtok(ptr, " ", &URL);


        Text=rstrcpy(Text, "Downloading: ");
        Text=rstrcat(Text, Title);

        Tempstr=(char *) calloc(1, 256);
        snprintf(Tempstr, 255, "\nrx: %lu", (unsigned long) DownloadTransferred(URL));
        Text=rstrcat(Text, Tempstr);
        if (! DownloadOSD) DownloadOSD=OSDMessage(10, 50, Text);
        else DownloadOSD->Contents=rstrcpy(DownloadOSD->Contents, Text);
        OSDUpdateSingle(DownloadOSD, TRUE);
    }

    destroy(Tempstr);
    destroy(Title);
    destroy(Text);
    destroy(URL);
}



//This to do every so many ms
void PeriodicProcessing()
{
    char *URL=NULL;
    const char *ptr;
    int pos;

// Sweep through playlist and start downloading things that need downloading
    if (Config->playlist && (! (Config->flags & CONFIG_STREAM)))
    {
        pos=Config->playlist->next;
        ptr=StringListFirst(Config->playlist);
        while (ptr)
        {
            URL=rstrcpy(URL, ptr);
            if (! DownloadDone(&URL)) break;
            ptr=StringListNext(Config->playlist);
        }
        Config->playlist->next=pos;
    }

// if current item is an image, then it must have had it's display time
// so mark us as not playing
    ptr=xine_get_meta_info(Config->stream, XINE_STREAM_INFO_VIDEO_FOURCC);
    if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) Config->state &= ~STATE_PLAYING;


//if there's not a 'nowplaying' pipe, perhaps because a client disconnected, then open one
    if ((Config->nowplay_pipe==-1) && (StrLen(Config->nowplay_pipe_path))) Config->nowplay_pipe=open(Config->nowplay_pipe_path, O_CREAT |O_WRONLY | O_NONBLOCK);

    OSDUpdate(Config->flags & CONFIG_OSD);

    if (Config->state & STATE_DOWNLOADING) DisplayDownloadProgress();

    destroy(URL);
}



int WatchFileDescriptors(TConfig *Config, int stdin_fd, int control_pipe, int sleep_ms)
{
    fd_set select_set;
    int high_fd=0, display_fd=-1, result;
    static struct timeval *tv=NULL;
    TEvent Event;

    Event.type=EVENT_NONE;

//first time around we will sleep for 0 ms, but tv is a static, so from then on we will sleep for SLEEP_TIME
    if (! tv) tv=(struct timeval *) calloc(1,sizeof(struct timeval));

    FD_ZERO(&select_set);

    if ((stdin_fd > -1) && (! (Config->state & STATE_STDIN_URL)) && (Config->flags & CONFIG_CONTROL) && (fcntl(stdin_fd, F_GETFD) > -1))
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
            if (ControlHandleInput(stdin_fd, Config->stream) ==EVENT_CLOSE)
            {
                close(stdin_fd);
                stdin_fd=-1;
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

    if (tv->tv_usec == 0)
    {
        tv->tv_usec=sleep_ms * 1000;
        return(EVENT_TIMEOUT);
    }

    return(Event.type);
}


void LoadConfigFile()
{
    /*
      char              configfile[2048];

      sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
      xine_config_load(Config->xine, configfile);
    */

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
    if (val & MM_ACCEL_X86_SSE3) printf("sse3, ");
    if (val & MM_ACCEL_X86_SSE4) printf("sse4, ");
    if (val & MM_ACCEL_X86_SSE42) printf("sse4.2, ");
    if (val & MM_ACCEL_X86_AVX) printf("avx, ");
//if (val & MM_ACCEL_PPC_ALTIVEC) printf("ppc_altivec, ");
//if (val & MM_ACCEL_PPC_CACHE32) printf("ppc_cache32, ");

    printf("\n");
}


void CXineSwitchUser()
{
    uid_t real, effective, saved;

    getresuid(&real, &effective, &saved);
    if (effective==0)
    {
        if (real != 0) setresuid(real, real, real);
        else setresuid(saved, saved, saved);
    }

}






void CXineOutputs(xine_t *xine, xine_stream_t *stream)
{
    xine_post_out_t *out;

    out=xine_get_video_source(Config->stream);
    if (out) printf("vid: %s\n",out->name);
    out=xine_get_audio_source(Config->stream);
    if (out) printf("aud: %s\n",out->name);
}







void CXineExit(TConfig *Config)
{
    if (xine_get_stream_info(Config->stream, XINE_STREAM_INFO_SEEKABLE)) SaveBookmark(StringListCurr(Config->playlist), Config->stream);

    xine_close(Config->stream);
    xine_event_dispose_queue(Config->event_queue);
    xine_dispose(Config->stream);
    if(Config->ao_port) xine_close_audio_driver(Config->xine, Config->ao_port);
    if (Config->vo_port)  xine_close_video_driver(Config->xine, Config->vo_port);


    xine_exit(Config->xine);
    X11Close(Config->X11Out);
}




int main(int argc, char **argv)
{
    int control_pipe=-1, stdin_fd=-1, result, sleep_ms;
    const char *ptr;

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

    signal(SIGPIPE, SIG_IGN);

    if (Config->flags & CONFIG_PERSIST) stdin_fd=0;
    if (Config->priority > 0) setpriority(PRIO_PROCESS, getpid(), Config->priority - 21);

    if (Config->debug > 0) xine_engine_set_param(Config->xine, XINE_ENGINE_PARAM_VERBOSITY, Config->debug);
    //CXineDisplayPlugins(Config->xine);
    //HelpMimeTypes(Config->xine);



    control_pipe=ControlPipeOpen(O_RDWR | O_NONBLOCK);
    Config->X11Out=X11Init(Config->parent, 0, 0, Config->width, Config->height);
    Config->vo_port=X11BindCXineOutput(Config);
    Config->ao_port = CXineOpenAudio(Config->ao_driver);
    OutputAccellerationTypes();
    Config->stream=xine_stream_new(Config->xine, Config->ao_port, Config->vo_port);
    Config->event_queue = xine_event_new_queue(Config->stream);
    xine_event_create_listener_thread(Config->event_queue, event_listener, NULL);

    if (Config->vo_port)
    {
        X11ActivateCXineOutput(Config->X11Out, Config->vo_port);
        OSDSetup(Config);
    }
    CXineSwitchUser();
    KeyGrabsSetup(Config->X11Out);

    CXineOutputs(Config->xine, Config->stream);
//	xine_set_param (Config->stream, XINE_PARAM_VERBOSITY, XINE_VERBOSITY_DEBUG);

    running = 1;
    while(running)
    {

        if (Config->state & STATE_PLAYING)
        {
            if (DownloadOSD)
            {
                OSDDestroy(DownloadOSD);
                DownloadOSD=NULL;
            }
        }
        else
        {
            if (! (Config->state & STATE_BACKGROUND_DISPLAYED))
            {
                if (StrLen(Config->background))
                {
                    if (xine_open(Config->stream, Config->background)) xine_play(Config->stream, 0, 0);
                    Config->state |= STATE_BACKGROUND_DISPLAYED;
                }
            }

            result=CXineSelectStream(Config, PLAY_NEXT);

            if  (! result)
            {
                if (    (! (Config->state & STATE_DOWNLOADING)) &&
                        (StringListPos(Config->playlist) >= StringListSize(Config->playlist)) &&
                        (! (Config->flags & (CONFIG_PERSIST)))
                   ) running=0;
            }
        }

//		if (Config->state & STATE_NEWTITLE) CXineNewTitle(Config);

        ptr=xine_get_meta_info(Config->stream, XINE_STREAM_INFO_VIDEO_FOURCC);
        if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) sleep_ms=Config->image_ms;
        else sleep_ms=200;

        result=WatchFileDescriptors(Config, stdin_fd, control_pipe, sleep_ms);
        if (result==EVENT_RESIZE) OSDSetup(Config);

        if (result==EVENT_TIMEOUT) PeriodicProcessing();

        waitpid(-1, NULL, WNOHANG);
    }
    CXineExit(Config);

    return(1);
}

