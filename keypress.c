/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "keypress.h"
#include "playback_control.h"
#include "playlist_osd.h"
#include "load_files_osd.h"
#include "media_info_osd.h"
#include "X11.h"
#include "audio_drivers.h"
#include "stdin_fd.h"

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


void MainScreenHandleKeyPress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
    int val, pos_msecs, len_msecs;

    switch (keychar)
    {
    case KEY_ESC:
        if (modifier & KEYMOD_CTRL)
        {
            if (Config->state & STATE_SHADED)
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_UNSHADE");
                X11UnGrabKey(X11Out, "ctrl-esc");
            }
            else
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_SHADED");
                X11GrabKey(X11Out, "ctrl-esc");
            }
        }
        else if (modifier & KEYMOD_SHIFT)
        {
            if (Config->state & STATE_RAISED)
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_BELOW");
                X11GrabKey(X11Out, "shift-esc");
            }
            else
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_ABOVE");
                X11UnGrabKey(X11Out, "shift-esc");
            }
        }
        else if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_PREVIOUS);
        else if (Config->flags & CONFIG_ALLOW_KEY_EXIT) running=0;
        break;

    case ' ':
    case KEY_PAUSE:
    case KEY_PLAY:
        val=CXinePause(Config);
        if (! (Config->flags & KILL_SCREENSAVER)) X11ScreenSaver(X11Out, (val != XINE_SPEED_PAUSE));
        break;

    case 'a':
        CXineCycleAudio();
        break;


    case 'f':
        if (Config->state & STATE_FULLSCREEN) X11SetWindowState(X11Out,  "_NET_WM_STATE_RESTORED");
        else X11SetWindowState(X11Out,  "_NET_WM_STATE_FULLSCREEN");
        break;

    case 'T':
        if (Config->state & STATE_RAISED) X11SetWindowState(X11Out,  "_NET_WM_STATE_ZORDER");
        else X11SetWindowState(X11Out,  "_NET_WM_STATE_ABOVE");
        break;


    case KEY_HOME:
        xine_play(stream, 0, 0);
        break;

    case KEY_END:
        xine_get_pos_length (stream, &val, &pos_msecs, &len_msecs);
        xine_play(stream, 0, len_msecs - 20000);
        break;


    case KEY_LEFT:
        if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_LEFT);
        else if (modifier & KEYMOD_SHIFT) CXineSelectStream(Config, PLAY_PREV);
#ifdef XINE_PARAM_VO_SINGLE_STEP //earlier versions of libxine lack this
        else if (modifier & KEYMOD_CTRL)
        {
            CXineSetPos(stream, -30);
            xine_set_param (stream,  XINE_PARAM_VO_SINGLE_STEP, 1);
        }
#endif
        else CXineSetPos(stream, 0-SKIP_SMALL);
        break;

    case KEY_RIGHT:
        if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_RIGHT);
        else if (modifier & KEYMOD_SHIFT) CXineSelectStream(Config, PLAY_NEXT);
#ifdef XINE_PARAM_VO_SINGLE_STEP //earlier versions of libxine lack this
        else if (modifier & KEYMOD_CTRL) xine_set_param (stream,  XINE_PARAM_VO_SINGLE_STEP, 1);
#endif
        else CXineSetPos(stream, SKIP_SMALL);
        break;

    case KEY_UP:
        if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_UP);
        else if (modifier & KEYMOD_SHIFT) CXineSelectStream(Config, PLAY_NEXT);
        else CXineSetPos(stream, SKIP_LARGE);
        break;

    case KEY_DOWN:
        if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_DOWN);
        else if (modifier & KEYMOD_SHIFT) CXineSelectStream(Config, PLAY_PREV);
        else CXineSetPos(stream, 0-SKIP_LARGE);
        break;

    case KEY_PGDN:
        if (xine_get_stream_info (stream,  XINE_STREAM_INFO_HAS_CHAPTERS)) CXineEventSend(Config, XINE_EVENT_INPUT_PREVIOUS);
        else CXineSetPos(stream, 0-SKIP_LARGE*10);
        break;

    case KEY_PGUP:
        if (xine_get_stream_info (stream,  XINE_STREAM_INFO_HAS_CHAPTERS)) CXineEventSend(Config, XINE_EVENT_INPUT_NEXT);
        else CXineSetPos(stream, SKIP_LARGE*10);
        break;

    case KEY_ENTER:
    case KEY_RETURN:
        if (Config->DVDNavButtons > 1) CXineEventSend(Config, XINE_EVENT_INPUT_SELECT);
        else CXineSelectStream(Config, PLAY_NEXT);
        break;

    case KEY_TAB:
        if (modifier & KEYMOD_CTRL)
        {
            if (Config->state & STATE_SHADED)
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_UNSHADE");
                X11UnGrabKey(X11Out, "ctrl-tab");
            }
            else
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_SHADED");
                X11GrabKey(X11Out, "ctrl-tab");
            }
        }
        else if (modifier & KEYMOD_SHIFT)
        {
            if (Config->state & STATE_RAISED)
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_BELOW");
                X11GrabKey(X11Out, "shift-tab");
            }
            else
            {
                X11SetWindowState(X11Out,  "_NET_WM_STATE_ABOVE");
                X11UnGrabKey(X11Out, "shift-tab");
            }
        }
        break;

    case '.':
    case KEY_DELETE:
        X11SetWindowState(X11Out,  "_NET_WM_STATE_NORMAL");
        break;

    case '>':
    case KEY_NEXT:
        CXineSelectStream(Config, PLAY_NEXT);
        break;

    case '<':
    case KEY_PREV:
        CXineSelectStream(Config, PLAY_PREV);
        break;

    case '[':
        Config->speed=xine_get_param (stream, XINE_PARAM_FINE_SPEED);
        if (Config->speed > 0) Config->speed-=10000;
        xine_set_param (stream, XINE_PARAM_FINE_SPEED, Config->speed);
        break;

    case ']':
        Config->speed=xine_get_param (stream, XINE_PARAM_FINE_SPEED);
        Config->speed+=10000;
        xine_set_param (stream, XINE_PARAM_FINE_SPEED, Config->speed);
        break;

    case '{':
        Config->speed=xine_get_param (stream, XINE_PARAM_SPEED);
        if (Config->speed == XINE_SPEED_SLOW_4) Config->speed=XINE_SPEED_NORMAL;
        else Config->speed=XINE_SPEED_SLOW_4;
        xine_set_param (stream, XINE_PARAM_SPEED, Config->speed);
        break;

    case '}':
        Config->speed=xine_get_param (stream, XINE_PARAM_SPEED);
        if (Config->speed == XINE_SPEED_FAST_4) Config->speed=XINE_SPEED_NORMAL;
        else Config->speed=XINE_SPEED_FAST_4;
        xine_set_param (stream, XINE_PARAM_SPEED, Config->speed);
        break;

    case KEY_BACKSPACE:
        xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
        break;

    case '+':
    case '=':
    case KEY_VOLUME_UP:
        if (modifier & KEYMOD_SHIFT) CXineSwitchAudioChannel(stream, 1);
        else if (modifier & KEYMOD_SHIFT) CXineAudioComp(stream, SET_ADD, 25);
        else CXineVolume(stream, SET_ADD, 5);
        break;

    case '_':
    case '-':
    case KEY_VOLUME_DOWN:
        if (modifier & KEYMOD_SHIFT) CXineSwitchAudioChannel(stream, -1);
        else if (modifier & KEYMOD_SHIFT) CXineAudioComp(stream, SET_ADD, -25);
        else CXineVolume(stream, SET_ADD, -5);
        break;

    case 'i':
        if (Config->state & STATE_INFO_DISPLAYED) InfoOSDHide();
        else InfoOSDShow();
        break;

    case 'j':
        if (! Config->X11Out) StdInAskJump();
        break;

    case 'l':
        if (Config->state & STATE_LOADFILES_DISPLAYED) LoadFilesOSDHide();
        else LoadFilesOSDShow();
        break;

    case 'm':
    case KEY_MUTE:
        CXineMute(stream, TOGGLE);
        break;

    case 'o':
        Config->flags ^= CONFIG_OSD;
        break;

    case 'p':
        if (Config->state & STATE_PLAYLIST_DISPLAYED) PlaylistOSDHide();
        else PlaylistOSDShow();
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



void HandleKeyPress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
    //if playlist is displayed, and playlist takes/recognizes keypress, then
    //don't handle it in the main keypress code
    if ( (Config->state & STATE_PLAYLIST_DISPLAYED) && PlaylistOSDKeypress(X11Out, stream, keychar, modifier) ) /*do nothing, OSD took the keypress */ ;
    else if ( (Config->state & STATE_LOADFILES_DISPLAYED) && LoadFilesOSDKeypress(X11Out, stream, keychar, modifier) ) /*do nothing, OSD took the keypress */ ;
    else MainScreenHandleKeyPress(X11Out, stream, keychar, modifier);
}





const char *KeypressHandleEscapeSequence(const char *Sequence, xine_stream_t *stream)
{

    if (strcmp(Sequence, "\x1b[A")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_UP, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[B")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_DOWN, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[C")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_RIGHT, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[D")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_LEFT, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[a")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_UP, KEYMOD_SHIFT);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[b")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_DOWN, KEYMOD_SHIFT);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[c")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_RIGHT, KEYMOD_SHIFT);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[d")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_LEFT, KEYMOD_SHIFT);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[5A")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_UP, KEYMOD_CTRL);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[5B")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_DOWN, KEYMOD_CTRL);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[5C")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_RIGHT, KEYMOD_CTRL);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[5D")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_LEFT, KEYMOD_CTRL);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[H")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_HOME, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[F")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_END, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[Z")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_END, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[P")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_PAUSE, 0);
        return(Sequence+3);
    }
    else if (strcmp(Sequence, "\x1b[5~")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_PGUP, 0);
        return(Sequence+4);
    }
    else if (strcmp(Sequence, "\x1b[6~")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_PGDN, 0);
        return(Sequence+4);
    }
    else if (strcmp(Sequence, "\x1b")==0)
    {
        HandleKeyPress(Config->X11Out, stream, KEY_ESC, 0);
        return(Sequence+1);
    }




    return(Sequence);
}


int KeypressHandleStdIn(int fd, xine_stream_t *stream)
{
    char *Token=NULL;
    int result, i;

    Token=(char *) calloc(1, 256);
    result=read(fd, Token, 255);
    if (result > 0)
    {
        Token[result]='\0';
        if (Token[0]=='\x1b')
        {
            //escape key pressed, this is likely an escape sequence for keys like the arrow keys
            //or pageup/pagedown
            KeypressHandleEscapeSequence(Token, stream);
        }
        else for (i=0; i < result; i++)
            {
                HandleKeyPress(Config->X11Out, stream, Token[i], 0);
            }
    }

    destroy(Token);
}


