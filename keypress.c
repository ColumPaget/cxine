#include "keypress.h"
#include "playback_control.h"
#include "X11.h"

#define SKIP_SMALL 10000
#define SKIP_LARGE 60000


void KeyPressSetPos(xine_stream_t *stream, int skip)
{
int val, pos_msecs, len_msecs;

		xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
		val = pos_msecs + skip;
		if (val > len_msecs) val=len_msecs - SKIP_SMALL;
		if (val < 0) val=0;
		xine_play(stream, 0, val);
}



void HandleKeyPress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
int val, pos_msecs, len_msecs;

switch (keychar)
{
	case KEY_ESC:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_PREVIOUS);
	else if (Config->flags & CONFIG_ALLOW_KEY_EXIT) running=0;
	break;

	case ' ': 
	case KEY_PAUSE:
	case KEY_PLAY:
		XinePause(stream);
	break;

	case 'm':
	case KEY_MUTE:
		XineMute(stream, TOGGLE);
	break;

	case 'o':
		Config->flags ^= CONFIG_OSD;
	break;


	case 's':
		val=xine_get_param (stream, XINE_PARAM_SPEED);
		if (val == XINE_SPEED_SLOW_4) val=XINE_SPEED_NORMAL;
		else val=XINE_SPEED_SLOW_4;
		xine_set_param (stream, XINE_PARAM_SPEED, val);
	break;


	case 'f':
		val=xine_get_param (stream, XINE_PARAM_SPEED);
		if (val == XINE_SPEED_FAST_4) val=XINE_SPEED_NORMAL;
		else val=XINE_SPEED_FAST_4;
		xine_set_param (stream, XINE_PARAM_SPEED, val);
	break;


	case KEY_HOME:
		xine_play(stream, 0, 0);
	break;

	case KEY_END:
		xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
		xine_play(stream, 0, len_msecs - 20000);
	break;


	case KEY_LEFT:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_LEFT);
	else if (modifier & KEYMOD_SHIFT) XineSelectStream(stream, PLAY_PREV);
	else if (modifier & KEYMOD_CTRL) 
	{
		KeyPressSetPos(stream, -1);
		xine_set_param (stream,  XINE_PARAM_VO_SINGLE_STEP, 1);
	}
	else KeyPressSetPos(stream, 0-SKIP_SMALL);
	break;

	case KEY_RIGHT:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_RIGHT);
	else if (modifier & KEYMOD_SHIFT) XineSelectStream(stream, PLAY_NEXT);
	else if (modifier & KEYMOD_CTRL) xine_set_param (stream,  XINE_PARAM_VO_SINGLE_STEP, 1);
	else KeyPressSetPos(stream, SKIP_SMALL);
	break;

	case KEY_UP:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_UP);
	else KeyPressSetPos(stream, SKIP_LARGE);
	break;

	case KEY_DOWN:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_DOWN);
	else KeyPressSetPos(stream, 0-SKIP_LARGE);
	break;


	case KEY_PGDN:
	if (xine_get_stream_info (stream,  XINE_STREAM_INFO_HAS_CHAPTERS)) XineEventSend(stream, XINE_EVENT_INPUT_PREVIOUS);
	else KeyPressSetPos(stream, 0-SKIP_LARGE*10);
	break;

	case KEY_PGUP:
	if (xine_get_stream_info (stream,  XINE_STREAM_INFO_HAS_CHAPTERS)) XineEventSend(stream, XINE_EVENT_INPUT_NEXT);
	else KeyPressSetPos(stream, SKIP_LARGE*10);
	break;

	case KEY_ENTER:
	if (Config->DVDNavButtons > 1) XineEventSend(stream, XINE_EVENT_INPUT_SELECT);
	else XineSelectStream(stream, PLAY_NEXT);
	break;


	case KEY_TAB:
	if (modifier & KEYMOD_CTRL)
	{	
	if (Config->flags & STATE_SHADED) X11SetWindowState(X11Out,  "_NET_WM_STATE_UNSHADE");
	else X11SetWindowState(X11Out,  "_NET_WM_STATE_SHADED");
	}
	else
	{					
	if (Config->flags & STATE_RAISED) X11SetWindowState(X11Out,  "_NET_WM_STATE_BELOW");
	else X11SetWindowState(X11Out,  "_NET_WM_STATE_ABOVE");
	}
	break;

	case '.':
	case KEY_DELETE:
	X11SetWindowState(X11Out,  "_NET_WM_STATE_NORMAL");
	break;

	case '>':
	case KEY_NEXT:
		XineSelectStream(stream, PLAY_NEXT);
	break;

	case '<':
	case KEY_PREV:
		XineSelectStream(stream, PLAY_PREV);
	break;

	case '+':
	case KEY_VOLUME_UP:
		if (modifier & KEYMOD_SHIFT) XineAudioComp(stream, SET_ADD, 25);
		else XineVolume(stream, SET_ADD, 5);
	break;

	case '-':
	case KEY_VOLUME_DOWN:
		if (modifier & KEYMOD_SHIFT) XineAudioComp(stream, SET_ADD, -25);
		else XineVolume(stream, SET_ADD, -5);
	break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
		val= (keychar - '0') * len_msecs / 10;
		xine_play(stream, 0, val);
	break;
}

}


void KeyGrabAdd(void *X11Out, const char *Mod, const char *KeyStr)
{
char *Tempstr=NULL;

Tempstr=rstrcpy(Tempstr, Mod);
Tempstr=rstrcat(Tempstr, KeyStr);

X11GrabKey(X11Out, Tempstr);

destroy(Tempstr);
}


void KeyGrabsSetup(void *X11Out)
{
char *Token=NULL;
const char *ptr, *kptr, *mod;

ptr=rstrtok(Config->keygrabs, ",",&Token);
while (ptr)
{
mod="";
if (strncmp(Token,"shift-",6)==0)
{
mod="shift-";
kptr=Token+6;
}
else if (strncmp(Token,"cntrl-",6)==0)
{
mod="cntrl-";
kptr=Token+6;
}
else if (strncmp(Token,"alt-",4)==0)
{
mod="alt-";
kptr=Token+4;
}
else kptr=Token;

if (strcmp(kptr,"volume")==0)
{
KeyGrabAdd(X11Out, mod, "vup");
KeyGrabAdd(X11Out, mod, "vdown");
KeyGrabAdd(X11Out, mod, "mute");
}
else if (strcmp(kptr, "media")==0)
{
KeyGrabAdd(X11Out, mod, "vup");
KeyGrabAdd(X11Out, mod, "vdown");
KeyGrabAdd(X11Out, mod, "mute");
KeyGrabAdd(X11Out, mod, "stop");
KeyGrabAdd(X11Out, mod, "play");
KeyGrabAdd(X11Out, mod, "prev");
KeyGrabAdd(X11Out, mod, "next");
KeyGrabAdd(X11Out, mod, "vpause");
}
else if ((strcmp(kptr, "arrow")==0) || (strcmp(kptr, "arrows")==0))
{
KeyGrabAdd(X11Out, mod, "up");
KeyGrabAdd(X11Out, mod, "down");
KeyGrabAdd(X11Out, mod, "left");
KeyGrabAdd(X11Out, mod, "right");
}
else if (strcmp(kptr, "page")==0)
{
KeyGrabAdd(X11Out, mod, "pgup");
KeyGrabAdd(X11Out, mod, "pgdn");
KeyGrabAdd(X11Out, mod, "home");
KeyGrabAdd(X11Out, mod, "end");
}
else if (strcmp(kptr, "nav")==0)
{
KeyGrabAdd(X11Out, mod, "up");
KeyGrabAdd(X11Out, mod, "down");
KeyGrabAdd(X11Out, mod, "left");
KeyGrabAdd(X11Out, mod, "right");
KeyGrabAdd(X11Out, mod, "pgup");
KeyGrabAdd(X11Out, mod, "pgdn");
KeyGrabAdd(X11Out, mod, "home");
KeyGrabAdd(X11Out, mod, "end");
}
else if (strcmp(kptr, "keypad")==0)
{
KeyGrabAdd(X11Out, mod, "up");
KeyGrabAdd(X11Out, mod, "down");
KeyGrabAdd(X11Out, mod, "left");
KeyGrabAdd(X11Out, mod, "right");
KeyGrabAdd(X11Out, mod, "pgup");
KeyGrabAdd(X11Out, mod, "pgdn");
KeyGrabAdd(X11Out, mod, "home");
KeyGrabAdd(X11Out, mod, "end");
KeyGrabAdd(X11Out, mod, "+");
KeyGrabAdd(X11Out, mod, "-");
}
else X11GrabKey(X11Out, Token);

ptr=rstrtok(ptr, ",",&Token);
}

destroy(Token);
}
