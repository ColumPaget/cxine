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
#include "bookmarks.h"
#include "osd.h"
#include "plugins.h"
#include "keypress.h"
#include "playback_control.h"
#include "control_protocol.h"
#include "command_line.h"
#include <sys/resource.h>

#ifndef VERSION
#define VERSION                   "1.1"
#endif

#define DEFAULT_TOPOSD_STRING "top,%tt %T - %A"
#define DEFAULT_BOTTOMOSD_STRING "bottom,POS: %tP%% %tw  VOL: %v  ac: %aw"


static xine_t              *xine;
static xine_stream_t       *stream;
static xine_video_port_t   *vo_port;
static xine_audio_port_t   *ao_port;
static xine_event_queue_t  *event_queue;


CXineOSD *topOSD=NULL;
CXineOSD *bottomOSD=NULL;

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
		if (Config) Config->flags &= ~STATE_PLAYING;
//    running = 0;
    break;

	case XINE_EVENT_UI_SET_TITLE:
		msg=(xine_ui_data_t *) event->data;
		Config->CurrTitle=rstrlcpy(Config->CurrTitle, msg->str, msg->str_len);
printf("UI SET TITLE [%s]\n", Config->CurrTitle);
		Config->flags |= STATE_NEWTITLE;
	break;

	case XINE_EVENT_UI_NUM_BUTTONS:
		msg=(xine_ui_data_t *) event->data;
		Config->DVDNavButtons=msg->num_buttons;
	break; 
  }

}




TConfig *ConfigInit()
{
TConfig *Config=NULL;

	Config=(TConfig *) calloc(1, sizeof(TConfig));
	Config->playlist=StringListCreate(0,NULL);
	Config->audio_compression=150;
	Config->top_osd_text=rstrcpy(Config->top_osd_text, DEFAULT_TOPOSD_STRING);
	Config->bottom_osd_text=rstrcpy(Config->bottom_osd_text, DEFAULT_BOTTOMOSD_STRING);
	Config->flags |= CONFIG_CONTROL;
	Config->control_pipe=-1;
	Config->nowplay_pipe=-1;

	return(Config);
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

xine_audio_port_t *XineOpenAudioDriver(const char *Spec)
{
char *Type=NULL, *Tempstr=NULL;
const char *ptr;
xine_cfg_entry_t entry;
xine_audio_port_t *ao_port=NULL;
int result;

ptr=rstrtok(Spec,":",&Type);
ao_port=xine_open_audio_driver(xine , Type, NULL);
if (strcmp(Type, "oss")==0) 
{
	result=xine_config_lookup_entry (xine, "audio.device.oss_device_name",  &entry);
	Tempstr=rstrcpy(Tempstr, "/dev/dsp");
	Tempstr=rstrcat(Tempstr, ptr);
	entry.str_value=Tempstr;
	xine_config_update_entry (xine, &entry);
	
	result=xine_config_lookup_entry (xine, "audio.device.oss_device_number",  &entry);
	entry.num_value=atoi(ptr);
	xine_config_update_entry (xine, &entry);
}
else if (strcmp(Type, "alsa")==0) 
{
	result=xine_config_lookup_entry (xine, "audio.device.alsa_front_device",  &entry);
	//result=xine_config_lookup_entry (xine, "audio.device.alsa_default_device",  &entry);
	Tempstr=rstrcpy(Tempstr, "plug:front:");
	//Tempstr=rstrcpy(Tempstr, "default:");
	Tempstr=rstrcat(Tempstr, ptr);
	printf("asla: %s\n",Tempstr);
	entry.str_value=Tempstr;
	xine_config_update_entry (xine, &entry);
}

destroy(Tempstr);
destroy(Type);

return(ao_port);
}



xine_audio_port_t *XineOpenAudio(const char *Audio)
{
xine_audio_port_t *ao_port=NULL;
char *Token=NULL;
const char *ptr;

ptr=rstrtok(Audio,",",&Token);
while (ptr)
{
ao_port=XineOpenAudioDriver(Token);
if (ao_port) break;
ptr=rstrtok(ptr,",",&Token);
}

destroy(Token);
return(ao_port);
}





int WatchFileDescriptors(int stdin_fd, int control_pipe, void *X11Out, int sleep_ms)
{
fd_set select_set;
int high_fd=0, display_fd=-1, result;
static struct timeval *tv=NULL;
TEvent Event;

Event.type=EVENT_NONE;

//first time around we will sleep for 0 ms, but tv is a static, so from then on we will sleep for SLEEP_TIME
if (! tv) tv=(struct timeval *) calloc(1,sizeof(struct timeval));

FD_ZERO(&select_set);

if ((stdin_fd > -1) && (! (Config->flags & STATE_STDIN_URL)) && (Config->flags & CONFIG_CONTROL) && (fcntl(stdin_fd, F_GETFD) > -1))
{
  FD_SET(stdin_fd, &select_set);
	if (stdin_fd > high_fd) high_fd=stdin_fd;
}

if (control_pipe && (fcntl(control_pipe, F_GETFD) > -1))
{
  FD_SET(control_pipe, &select_set);
	if (control_pipe > high_fd) high_fd=control_pipe;
}

display_fd=X11GetFileDescriptor(X11Out);
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
		if (ControlHandleInput(stdin_fd, stream) ==EVENT_CLOSE) 
		{
						close(stdin_fd);
						stdin_fd=-1;
		}
	}

	if ((control_pipe > -1) && FD_ISSET(control_pipe, &select_set)) 
	{
		if (ControlHandleInput(control_pipe, stream) == EVENT_CLOSE) 
		{
						close(control_pipe);
						control_pipe=-1;
		}
	}

	if ((display_fd > -1) && FD_ISSET(display_fd, &select_set)) 
	{
		X11NextEvent(X11Out, vo_port, &Event);
		if (Event.type==EVENT_KEYPRESS) HandleKeyPress(X11Out, stream, Event.arg1, Event.arg2);
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
  xine_config_load(xine, configfile);
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


void XineSwitchUser()
{
uid_t real, effective, saved;

getresuid(&real, &effective, &saved);
if (effective==0)
{
 if (real != 0) setresuid(real, real, real);
 else setresuid(saved, saved, saved);
}

}

void OSDSetup(void *X11Out, xine_stream_t *stream)
{
	if (StrLen(Config->top_osd_text)) 
	{
		if (topOSD) OSDDestroy(topOSD);
		topOSD=OSDCreate(X11Out, stream, Config->top_osd_text);
	}
	
	if (StrLen(Config->bottom_osd_text)) 
	{
		if (bottomOSD) OSDDestroy(bottomOSD);
		bottomOSD=OSDCreate(X11Out, stream, Config->bottom_osd_text);
	}
}


/*
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


const char *XineGetStringValue(xine_stream_t *stream, int ValID, int Quote)
{
const char *ptr;
char *Tempstr=NULL;

ptr=xine_get_meta_info(stream, ValID);
if (! ptr) 
{
	if (ValID==XINE_META_INFO_TITLE) 
	{
		ptr=StringListCurr(Config->playlist);
		if (StrLen(ptr)) 
		{
			//first arg is the URL
			ptr=rstrtok(ptr, " ", &Tempstr);
			if (! StrLen(ptr)) ptr=StringListCurr(Config->playlist);
		}
	}
	return("");
}
return(ptr);
}


void XineNowPlaying(void *X11Out, xine_stream_t *stream)
{
const char *ClipInfoTypes[]={"Title","Artist","Album","Year","Comment","Track","Genre",NULL};
int ClipInfoID[]={XINE_META_INFO_TITLE, XINE_META_INFO_ARTIST, XINE_META_INFO_ALBUM, XINE_META_INFO_YEAR, XINE_META_INFO_COMMENT, XINE_META_INFO_TRACK_NUMBER, XINE_META_INFO_GENRE};
const char *ptr;
int i, pos_msecs, len_msecs, pos;
char *Tempstr=NULL, *PipeStr=NULL;


X11WindowSetTitle(X11Out, Config->CurrTitle,"cxine");

printf("\nClip info:\n");
for (i=0; ClipInfoTypes[i] !=NULL; i++)
{
ptr=XineGetStringValue(stream, ClipInfoID[i], 0);
printf(" %s: %s\n",ClipInfoTypes[i], ptr);

Tempstr=rstrcpy(Tempstr, "CXINE:");
Tempstr=rstrcat(Tempstr, ClipInfoTypes[i]);

X11SetTextProperty(X11Out, Tempstr, ptr);

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

PipeStr=rstrcat(PipeStr, "\n");
if (Config->nowplay_pipe > 0) write(Config->nowplay_pipe, PipeStr, StrLen(PipeStr));

xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
ptr=StringListCurr(Config->playlist);

rstrtok(ptr, " ", &Tempstr);
X11SetTextProperty(X11Out, "CXINE:Filename", Tempstr);

if (Config->flags & CONFIG_IDENTIFY)
{
printf("ID_FILENAME=%s\n", Tempstr);
printf("ID_AUDIO_BITRATE=%d\n", xine_get_stream_info(stream, XINE_STREAM_INFO_AUDIO_BITRATE));
printf("ID_AUDIO_RATE=%d\n", xine_get_stream_info(stream, XINE_STREAM_INFO_AUDIO_SAMPLERATE)); 
printf("ID_SEEKABLE=%d\n", xine_get_stream_info(stream, XINE_STREAM_INFO_SEEKABLE));
printf("ID_CHAPTERS=%d\n", xine_get_stream_info(stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT));
printf("ID_LENGTH=%0.2f\n", ((float) len_msecs) / 1000.0);
}


destroy(Tempstr);
destroy(PipeStr);
}

int main(int argc, char **argv) 
{
	int control_pipe=-1, stdin_fd=-1, result, sleep_ms;
	const char *ptr;
	void *X11Out=NULL;
	
	//call 'SignalHandler' with a signal it ignores as it will set up
	//handlers for SIGINT and SIGTERM
	SignalHandler(SIGCONT);
	Config=ConfigInit();

	ParseCommandLine(argc, argv, Config);

	signal(SIGPIPE, SIG_IGN);

	if (Config->flags & CONFIG_PERSIST) stdin_fd=0;
	if (Config->priority > 0) setpriority(PRIO_PROCESS, getpid(), Config->priority - 21);

  xine = xine_new();
  xine_init(xine);
	//XineDisplayPlugins(xine);
 
	control_pipe=ControlPipeOpen(O_RDWR | O_NONBLOCK);
	X11Out=X11Init(Config->parent, 0, 0, Config->width, Config->height);
	vo_port=X11BindXineOutput(X11Out, xine, Config->vo_driver);
	ao_port = XineOpenAudio(Config->ao_driver);
	OutputAccellerationTypes();
  stream      = xine_stream_new(xine, ao_port, vo_port);
  event_queue = xine_event_new_queue(stream);
  xine_event_create_listener_thread(event_queue, event_listener, NULL);

	if (vo_port) 
	{
		X11ActivateXineOutput(X11Out, vo_port);
		OSDSetup(X11Out, stream);
	}
	XineSwitchUser();
	KeyGrabsSetup(X11Out);


  running = 1;
  while(running)
	{
		if (! (Config->flags & STATE_PLAYING)) 
		{
			result=XineSelectStream(stream, PLAY_NEXT);

	  if (
      StrLen(Config->audio_plugins) &&
      (! xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_VIDEO)) &&
      (xine_get_stream_info(stream, XINE_STREAM_INFO_HAS_AUDIO))
    ) XineAddAudioPostPlugins(xine, stream, Config->audio_plugins, &ao_port, &vo_port);

			if (vo_port) XineAddVideoPostPlugins(xine, stream, &ao_port, &vo_port);

			if ((! result) && (! (Config->flags & CONFIG_PERSIST))) running=0;
		}

		if (Config->flags & STATE_NEWTITLE)
		{
			XineNowPlaying(X11Out, stream);
			Config->flags &= ~STATE_NEWTITLE;
		}

		ptr=xine_get_meta_info(stream, XINE_STREAM_INFO_VIDEO_FOURCC);
		if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) sleep_ms=Config->image_ms;
		else sleep_ms=100;

		result=WatchFileDescriptors(stdin_fd, control_pipe, X11Out, sleep_ms);
		if (result==EVENT_RESIZE) OSDSetup(X11Out, stream);

		if (result==EVENT_TIMEOUT)
		{
			ptr=xine_get_meta_info(stream, XINE_STREAM_INFO_VIDEO_FOURCC);
			if ((Config->image_ms > 0) && (strcmp(ptr, "imagedmx")==0)) 
			{
							Config->flags &= ~STATE_PLAYING;
			}
			if (topOSD) OSDUpdate(topOSD, Config->flags & CONFIG_OSD);
			if (bottomOSD) OSDUpdate(bottomOSD, Config->flags & CONFIG_OSD);
		}
  }

  if (xine_get_stream_info(stream, XINE_STREAM_INFO_SEEKABLE)) SaveBookmark(StringListCurr(Config->playlist), stream);

  xine_close(stream);
  xine_event_dispose_queue(event_queue);
  xine_dispose(stream);
  if(ao_port) xine_close_audio_driver(xine, ao_port);  
  if (vo_port)  xine_close_video_driver(xine, vo_port);  


  xine_exit(xine);
	X11Close(X11Out);
  return 1;
}

