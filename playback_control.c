
#include "playback_control.h"
#include "bookmarks.h"
#include "plugins.h"

void XineStreamInitConfig(xine_stream_t *stream, TConfig *Config)
{
if ((Config->flags & CONFIG_MUTE) && (! xine_get_param (stream, XINE_PARAM_AUDIO_MUTE))) XineMute(stream, 1);
else XineMute(stream, 0);

if (Config->flags & CONFIG_HALFSPEED) xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_2);
if (Config->flags & CONFIG_QUARTSPEED) xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_SLOW_4);
if (Config->flags & CONFIG_PAUSE) xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
if (Config->flags & CONFIG_LOUD) xine_set_param (stream, XINE_PARAM_AUDIO_AMP_LEVEL, 150);
if (Config->flags & CONFIG_VLOUD)
{
xine_set_param (stream, XINE_PARAM_AUDIO_VOLUME, 100);
xine_set_param (stream, XINE_PARAM_AUDIO_AMP_LEVEL, 200);
}

xine_set_param (stream, XINE_PARAM_AUDIO_COMPR_LEVEL, Config->audio_compression);
if (Config->zoom > 0)
{
  xine_set_param (stream, XINE_PARAM_VO_ZOOM_X, Config->zoom);
  xine_set_param (stream, XINE_PARAM_VO_ZOOM_Y, Config->zoom);
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

void XineEventSend(xine_stream_t *stream, int type)
{
xine_event_t event;

event.stream=stream;
event.type=type;
event.data=NULL;
event.data_length=0;
gettimeofday(&(event.tv), NULL);
xine_event_send(stream, &event);
}


void XineSwitchAudioChannel(xine_stream_t *stream, int skip)
{
int pos;

pos=xine_get_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL);

xine_set_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, pos + skip);
}


void XineMute(xine_stream_t *stream, int Setting)
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

void XineSetPos(xine_stream_t *stream, int skip)
{
int val, pos_msecs, len_msecs;

    xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
    val = pos_msecs + skip;
    if (val > len_msecs) val=len_msecs - SKIP_SMALL;
    if (val < 0) val=0;
    xine_play(stream, 0, val);
}


void XinePause(xine_stream_t *stream)
{
int val;

		val=xine_get_param (stream, XINE_PARAM_SPEED);
		if (val == XINE_SPEED_PAUSE) val=XINE_SPEED_NORMAL;
		else val=XINE_SPEED_PAUSE;
		xine_set_param (stream, XINE_PARAM_SPEED, val);

		//pause seems to lose its place in the audio stream, so
		//we seek back slightly to resync
		if (val==XINE_SPEED_NORMAL) XineSetPos(stream, -1);
}


void XineSetRangeValue(xine_stream_t *stream, int Type, int SetType, int Value)
{
int val;

		val=xine_get_param (stream, Type);
		if (SetType==SET_ADD) val+=Value;
		else val=Value;
		if ((Type==XINE_PARAM_AUDIO_COMPR_LEVEL) && (val < 100)) val=100;
		xine_set_param (stream, Type, val);
}




int XinePlayStream(xine_stream_t *stream, const char *url)
{
int startms;
int len;

	len=StrLen(url);
  if (len==0) return(0);

	//xine interprets anything starting with '-' to mean 'stdin', and sits there trying to read from stdin. 
	//so if we get a file path starting with '-' (probably a command-line option that we don't recognize)
	//then we don't want to pass it to xine unless the file really exists
	if ((*url=='-') && (len > 1) && (access(url, F_OK) !=0)) return(0);

  if (! xine_open(stream, url))
  {
    printf("Unable to open url '%s'\n", url);
    return(0);
  }
  Config->CurrTitle=rstrcpy(Config->CurrTitle, cbasename(url));
  //do this before calling play..
  XineStreamInitConfig(stream, Config);

  startms=Config->startms;
  if ((startms==0) && xine_get_stream_info(stream, XINE_STREAM_INFO_SEEKABLE))
  {
    startms=LoadBookmark(url);
  }

  if (! xine_play(stream, 0, startms))
  {
   printf("Unable to play url '%s'\n", url);
   return(0);
  }

  //and again after, because some configs require the stream to be playing to take effect (pause)
  XineStreamInitConfig(stream, Config);

  Config->flags |= STATE_PLAYING;

  return(1);
}


int XineSelectStream(xine_stream_t *stream, int Which)
{
const char *ptr;

if (Which==PLAY_NEXT) 
{
				ptr=StringListNext(Config->playlist);
				if ( (ptr==NULL)  )
				{
					if (Config->loop != 0) Config->loop--;
					if (Config->loop !=0) ptr=StringListGet(Config->playlist, 0);
				}


}
else if (Which==PLAY_PREV) ptr=StringListPrev(Config->playlist);

if (ptr)
{
  xine_stop(stream);
  if (XinePlayStream(stream, ptr)) Config->flags |= STATE_NEWTITLE;
  return(1);
}
return(0);
}

