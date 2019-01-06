#ifndef CXINE_X11_H
#define CXINE_X11_H

#include "common.h"


void X11SetTextProperty(void *p_Win, const char *Name, const char *Str);
void X11SetWindowState(void *p_Win, const char *StateStr);
void *X11Init(const char *parent, int xpos, int ypos, int width, int height);
int X11GetFileDescriptor(void *p_Win);
xine_video_port_t *X11BindCXineOutput(TConfig *Config);
void X11ActivateCXineOutput(void *p_Win, xine_video_port_t *vo_port);
int X11NextEvent(void *p_Win, xine_video_port_t *vo_port, TEvent *Event);
void X11Close(void *Win);
void X11WindowSetTitle(void *p_Win, const char *MainTitle, const char *IconTitle);
void X11WindowDimensions(void *p_Win, int *width, int *height);
void X11Fit(void *p_Win, int *x, int *y, int *wid, int *high);
int X11GrabKey(void *p_Win, const char *keystr);
int X11UnGrabKey(void *p_Win, const char *keystr);
void X11ScreenSaver(void *p_Win, int OnOrOff);

#endif
