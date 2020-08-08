#ifndef CXINE_KEYPRESS_H
#define CXINE_KEYPRESS_H

#include "common.h"

#define KEYMOD_SHIFT 1
#define KEYMOD_CTRL 2
#define KEYMOD_ALT   4


#define KEY_ENTER 1
#define KEY_UP 2
#define KEY_DOWN 3
#define KEY_LEFT 4
#define KEY_RIGHT 5
#define KEY_PGUP 6
#define KEY_PGDN 7
#define KEY_TAB  9
#define KEY_RETURN 10

#define KEY_F1 11
#define KEY_F2 12
#define KEY_F3 13
#define KEY_F4 14
#define KEY_F5 15
#define KEY_F6 16
#define KEY_F7 17
#define KEY_F8 18
#define KEY_F9 19
#define KEY_F10 20
#define KEY_F11 21
#define KEY_F12 22
#define KEY_F13 23
#define KEY_F14 24
#define KEY_F15 25

#define KEY_SUPERL 26
#define KEY_SUPERR 27
#define KEY_INSERT 28
#define KEY_PAUSE  30
#define KEY_BACKSPACE 31
//from 32 to 127 is taken up by ascii characters

#define KEY_DELETE 127
#define KEY_ESC  128
#define KEY_HOME 129
#define KEY_END  130

//internet-enabled and media-enabled keyboards 
#define KEY_MUTE 200
#define KEY_VOLUME_DOWN 201
#define KEY_VOLUME_UP   202
#define KEY_STOP 203
#define KEY_PLAY 204
#define KEY_PREV 205
#define KEY_NEXT 206


void HandleKeyPress(void *X11Out, xine_stream_t *stream, int keychar, int modifier);
void KeyGrabsSetup(void *X11Out);

int KeypressHandleStdIn(int fd, xine_stream_t *stream);
#endif

