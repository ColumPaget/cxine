/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <X11/XF86keysym.h>
#include "X11.h"
#include "common.h"
#include "keypress.h"
#include "playlist.h"
#include <ctype.h>


#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)


typedef struct
{
    Display *display;
    int screen;
    int x;
    int y;
    int width;
    int height;
    double pixel_aspect;
    Window parent;
    Window drawable;
    Window inputonly;
} X11Window;

Atom XA_UTF8_STRING, WMProtocols, WMDeleteWindow;




int X11GetFileDescriptor(void *p_Win)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;
    return(XConnectionNumber(Win->display));
}


void X11SetProperty(Display *display, Window Win, Atom Type, const char *Name, const unsigned char *Data, int Len)
{
    Atom Prop;
    int Size=32;

    if (! Win) return;

    if (Type==XA_STRING) Size=8;
    else if (Type==XA_UTF8_STRING) Size=8;
    else Size=32;

    XLockDisplay(display);
    Prop=XInternAtom(display, Name, False);
    if (Prop) XChangeProperty(display, Win, Prop, Type, Size, PropModeReplace, Data, Len);
    XUnlockDisplay(display);
}


void X11RequestSelection(Display *XDisplay, Window XWin, Atom Selection, int FirstTry)
{
    Window SelOwner;
    Atom SelectProp;

    SelectProp=XInternAtom(XDisplay, "SelectProp",False);
    SelOwner=XGetSelectionOwner(XDisplay, Selection);
    if (SelOwner !=None)
    {
        XConvertSelection(XDisplay, Selection, XA_STRING, SelectProp, XWin, CurrentTime);
        sleep(0);
    }
}


void X11HandleSelectionAvailableEvent(Display *XDisplay, Window XWin, int Selection)
{
    unsigned char *Data=NULL;
    Atom PType, SelectProp;
    int PFormat;
    unsigned long dlen=0, Dummy;

    SelectProp=XInternAtom(XDisplay, "SelectProp",False);
    Data=(unsigned char *) calloc(1024, sizeof(uint32_t));
    XGetWindowProperty(XDisplay, XWin, SelectProp,0, 1024, 0, AnyPropertyType, &PType, &PFormat, &dlen, &Dummy, &Data);

    Data[dlen]='\0';

    PlaylistAdd(Config->playlist, Data, "", "");
    XDeleteProperty(XDisplay,XWin,SelectProp);
}



void X11SetTextProperty(void *p_Win, const char *Name, const char *Str)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;
    X11SetProperty(Win->display,  Win->drawable, XA_STRING, Name, (const unsigned char *) Str, StrLen(Str));
}



void X11SendSetStateEvent(void *p_Win, int AddOrDel, const char *StateStr)
{
    Atom StateAtom, StateValue;
    XEvent event;
    X11Window *Win;

    Win=(X11Window *) p_Win;


    StateAtom=XInternAtom(Win->display, "_NET_WM_STATE", False);
    StateValue=XInternAtom(Win->display, StateStr, False);


    memset( &event, 0, sizeof (XEvent) );
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.window = Win->drawable;
    event.xclient.message_type = StateAtom;
    event.xclient.send_event=True;
    event.xclient.format = 32;
    event.xclient.data.l[0] = AddOrDel;
    event.xclient.data.l[1] = StateValue;
    event.xclient.data.l[2] = 0;
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;


    XSendEvent(Win->display, DefaultRootWindow(Win->display ), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
}

#define WINSTATE_DEL 0
#define WINSTATE_ADD 1
#define WINSTATE_TOGGLE 2


void X11SetWindowState(void *p_Win, const char *StateStr)
{

    if (strcmp(StateStr,  "_NET_WM_STATE_ABOVE") ==0)
    {
        Config->state |=STATE_RAISED;
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_BELOW");
        X11SendSetStateEvent(p_Win, WINSTATE_ADD, StateStr);
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_BELOW") ==0)
    {
        Config->state &= ~STATE_RAISED;
        Config->state |= STATE_LOWERED;
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_ABOVE");
        X11SendSetStateEvent(p_Win, WINSTATE_ADD, StateStr);
    }
		else if (strcmp(StateStr, "_NET_WM_STATE_ZORDER")==0)
		{
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_ABOVE");
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_BELOW");
        Config->state &= ~STATE_RAISED;
        Config->state &= ~STATE_LOWERED;
		}
    else if (strcmp(StateStr,  "_NET_WM_STATE_SHADED") ==0)
    {
        Config->state |= STATE_SHADED;
        X11SendSetStateEvent(p_Win, WINSTATE_ADD, "_NET_WM_STATE_SHADED");
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_UNSHADE") ==0)
    {
        Config->state &= ~STATE_SHADED;
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_SHADED");
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_FULLSCREEN") ==0)
    {
        Config->state |= STATE_FULLSCREEN;
        X11SendSetStateEvent(p_Win, WINSTATE_ADD, "_NET_WM_STATE_FULLSCREEN");
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_ICONIZED") ==0)
    {
        Config->state |= STATE_ICONIZED;
        X11SendSetStateEvent(p_Win, WINSTATE_ADD, "_NET_WM_STATE_HIDDEN");
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_RESTORED") ==0)
    {
        Config->state &= ~STATE_ICONIZED;
        Config->state &= ~STATE_FULLSCREEN;
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_HIDDEN");
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_FULLSCREEN");
    }
    else if (strcmp(StateStr,  "_NET_WM_STATE_NORMAL") ==0)
    {
        Config->state &= ~STATE_RAISED;
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_ABOVE");
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_BELOW");
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_SHADED");
        X11SendSetStateEvent(p_Win, WINSTATE_DEL, "_NET_WM_STATE_HIDDEN");
    }
    else X11SendSetStateEvent(p_Win, WINSTATE_ADD, StateStr);
}


void X11SetDefaultProps(X11Window *Win)
{
    pid_t pid;
    const char *DefaultName="cxine";
    const char *ClassStrings="cxine\0cxine\0";
    Window root_win;

    root_win=XDefaultRootWindow(Win->display);
    if (Win->drawable) XSetWMProtocols(Win->display, Win->drawable, &WMDeleteWindow, 1);
    pid=getpid();
    X11SetProperty(Win->display, Win->drawable,  XA_CARDINAL, "_NET_WM_PID", (const unsigned char *) &pid, 1);
    X11SetProperty(Win->display, Win->drawable,  XA_STRING, "WM_CLASS", ClassStrings, 12);
    X11SetProperty(Win->display, root_win, XA_CARDINAL, "CXINE:PID", (const unsigned char *) &pid, 1);
    X11SetProperty(Win->display, root_win, XA_WINDOW, "CXINE:WIN", (const unsigned char *) &Win->drawable, 1);

    X11SetProperty(Win->display, Win->drawable,  XA_UTF8_STRING, "_NET_WM_NAME", (const unsigned char *) DefaultName, StrLen(DefaultName));
    X11SetProperty(Win->display, Win->drawable,  XA_UTF8_STRING, "_NET_WM_VISIBLE_NAME", (const unsigned char *) DefaultName, StrLen(DefaultName));
    X11SetProperty(Win->display, Win->drawable,  XA_UTF8_STRING, "_NET_WM_ICON_NAME", (const unsigned char *) DefaultName, StrLen(DefaultName));
    X11SetProperty(Win->display, Win->drawable,  XA_UTF8_STRING, "_NET_WM_VISIBLE_ICON_NAME", (const unsigned char *) DefaultName, StrLen(DefaultName));
}


#define WIN_SETTING_STICKY 1
#define WIN_SETTING_ONTOP  2
#define WIN_SETTING_BELOW  4
#define WIN_SETTING_FULLSCREEN 8
#define WIN_SETTING_SHADED 16
#define WIN_SETTING_ICONIZED 32

#define WT_NONE    0
#define WT_NORMAL  1
#define WT_ROOT    2
#define WT_FULLSCREEN 3
#define WT_EMBEDDED 4

X11Window *X11WindowCreate(Display *display, const char *ParentID, int x, int y, int width, int height)
{
    X11Window *Win;
    Window root_win;
    unsigned int screen, tmpx, tmpy, tmpwidth, tmpheight, tmpborder, tmpdepth;
    int WinSettings=0, WinType=WT_NORMAL;
    XSetWindowAttributes WinAttr;
    int WinAttrSet=0, val;
    char *Token=NULL;
    const char *ptr;

    Win=(X11Window *) calloc(1, sizeof(X11Window));
    Win->display=display;
    Win->screen= XDefaultScreen(display);
    root_win=XDefaultRootWindow(display);
    Win->parent=root_win;

    ptr=rstrtok(ParentID, ",", &Token);
    while (ptr)
    {
        if (strcmp(Token, "none")==0) WinType=WT_NONE;
        else if (strcmp(Token, "root")==0) WinType=WT_ROOT;
        else if (strcmp(Token, "fullscreen")==0) WinType=WT_FULLSCREEN;
        else if (strcmp(Token, "max")==0) WinType=WT_FULLSCREEN;
        else if (strcmp(Token, "shaded")==0) WinSettings |= WIN_SETTING_SHADED;
        else if (strcmp(Token, "sticky")==0) WinSettings |= WIN_SETTING_STICKY;
        else if (strcmp(Token, "ontop")==0) WinSettings |= WIN_SETTING_ONTOP;
        else if (strcmp(Token, "above")==0) WinSettings |= WIN_SETTING_ONTOP;
        else if (strcmp(Token, "below")==0) WinSettings |= WIN_SETTING_BELOW;
        else if (strcmp(Token, "iconized")==0) WinSettings |= WIN_SETTING_ICONIZED;
        else if (strcmp(Token, "iconic")==0) WinSettings |= WIN_SETTING_ICONIZED;
        else if (strcmp(Token, "icon")==0) WinSettings |= WIN_SETTING_ICONIZED;
        else if (strcmp(Token, "minimized")==0) WinSettings |= WIN_SETTING_ICONIZED;
        else if (strcmp(Token, "min")==0) WinSettings |= WIN_SETTING_ICONIZED;
        else if (strcmp(Token, "stickontop")==0) WinSettings |= WIN_SETTING_ONTOP | WIN_SETTING_STICKY;
        else if (strcmp(Token, "stickabove")==0) WinSettings |= WIN_SETTING_ONTOP | WIN_SETTING_STICKY;
        else if (strcmp(Token, "stickbelow")==0) WinSettings |= WIN_SETTING_BELOW | WIN_SETTING_STICKY;
        else if (strncmp(Token,"0x",2)==0)
        {
            Win->parent=strtol(Token, NULL, 16);
            WinType=WT_EMBEDDED;
        }
        else if (isdigit(*Token))
        {
            Win->parent=strtol(Token, NULL, 10);
            WinType=WT_EMBEDDED;
        }

        ptr=rstrtok(ptr, ",", &Token);
    }


    switch (WinType)
    {
    case WT_NONE:
        Win->inputonly=XCreateWindow(display, root_win, 0, 0, 1, 1, 0, CopyFromParent, InputOnly, CopyFromParent, 0, NULL);
        break;

    case WT_NORMAL:
        if (width==0) width=460;
        if (height==0) height=380;
        Win->drawable = XCreateSimpleWindow(display, Win->parent, x, y, width, height, 1, 0, 0);
        break;

    case WT_ROOT:
        Win->drawable=root_win;
        XGetGeometry(display, Win->parent, &root_win, &x, &y, &width, &height, &tmpborder, &tmpdepth);
        break;

    case WT_FULLSCREEN:
        XGetGeometry(display, root_win, &root_win, &x, &y, &width, &height, &tmpborder, &tmpdepth);
        Win->drawable = XCreateSimpleWindow(display, Win->parent, 0, 0, width, height, 1, 0, 0);
        WinSettings |= WIN_SETTING_FULLSCREEN;
        break;

    case WT_EMBEDDED:
        XGetGeometry(display, Win->parent, &root_win, &tmpx, &tmpy, &tmpwidth, &tmpheight, &tmpborder, &tmpdepth);
        if (width==0) width=tmpwidth;
        if (height==0) height=tmpheight;
        Win->drawable = XCreateSimpleWindow(display, Win->parent, x, y, width, height, 1, 0, 0);
        break;
    }


    X11SetDefaultProps(Win);
    Win->x=x;
    Win->y=y;
    Win->width=width;
    Win->height=height;

    if (Win->drawable)
    {
        if (Win->drawable != root_win)
        {
            XSelectInput (display, Win->drawable, INPUT_MOTION);
            if (WinSettings & WIN_SETTING_ICONIZED | WIN_SETTING_BELOW) XMapWindow(display, Win->drawable);
            else XMapRaised(display, Win->drawable);
        }

        if (WinSettings & WIN_SETTING_STICKY)
        {
            val=0xFFFFFFFF;
            X11SetProperty(Win->display, Win->drawable, XA_CARDINAL, "_NET_WM_DESKTOP", (const unsigned char *) &val, 1);
        }

        if (WinSettings & WIN_SETTING_ONTOP) X11SetWindowState(Win, "_NET_WM_STATE_ABOVE");
        if (WinSettings & WIN_SETTING_BELOW) X11SetWindowState(Win, "_NET_WM_STATE_BELOW");
        if (WinSettings & WIN_SETTING_SHADED) X11SetWindowState(Win, "_NET_WM_STATE_SHADED");
        if (WinSettings & WIN_SETTING_ICONIZED) X11SetWindowState(Win, "_NET_WM_STATE_ICONIZED");
        if (WinSettings & WIN_SETTING_FULLSCREEN)
        {
            X11SetWindowState(Win, "_NET_WM_STATE_FULLSCREEN");
            if (Win->drawable) XRaiseWindow(display, Win->drawable);
            WinAttr.override_redirect=True;
            WinAttrSet |= CWOverrideRedirect;
            XChangeWindowAttributes(display, Win->drawable, WinAttrSet, &WinAttr);
            XGrabKeyboard(display, Win->drawable, True, GrabModeAsync, GrabModeAsync, CurrentTime);
        }

        if (Win->inputonly) XSelectInput (display, Win->inputonly, INPUT_MOTION);


        printf("win_id: 0x%lx\n", Win->drawable);
    }

    return(Win);
}


void X11ScreenSaver(void *p_Win, int OnOrOff)
{
#ifdef HAVE_XSCREENSAVER
    XScreenSaverSuspend(((X11Window *) p_Win)->display, OnOrOff);
#endif
}




void *X11Init(const char *ParentID, int xpos, int ypos, int width, int height)
{
    X11Window *Win;
    Display *display;
    int resx, resy;

    if(!XInitThreads())
    {
        printf("XInitThreads() failed\n");
        return(NULL);
    }


    if((display = XOpenDisplay(getenv("DISPLAY"))) == NULL)
    {
        printf("XOpenDisplay() failed.\n");
        return(NULL);
    }

    XLockDisplay(display);
    if (Config->flags & DISABLE_SCREENSAVER) X11ScreenSaver(Win, False);

    XA_UTF8_STRING=XInternAtom(display,"UTF8_STRING",False);
    WMProtocols=XInternAtom(display,"WM_PROTOCOLS",False);
    WMDeleteWindow=XInternAtom(display,"WM_DELETE_WINDOW",False);

    Win=X11WindowCreate(display, ParentID, xpos, ypos, width, height);

    resx = (DisplayWidth(display, Win->screen) * 1000 / DisplayWidthMM(display, Win->screen));
    resy = (DisplayHeight(display, Win->screen) * 1000 / DisplayHeightMM(display, Win->screen));
    Win->pixel_aspect= (float) resy / (float) resx;
		if (Config->flags & CONFIG_DEBUG) printf("pixel aspect: %d/%d = '%f'\n", resy,  resx, Win->pixel_aspect);

    if(Win->pixel_aspect < 0.01) 
		{
			Win->pixel_aspect = 1.0;
			if (Config->flags & CONFIG_DEBUG) printf("adjusted pixel aspect to: '%f'\n", Win->pixel_aspect);
		}

    XSync(display, False);
    XUnlockDisplay(display);

    return((void *) Win);
}

void X11WindowDimensions(void *p_Win, int *width, int *height)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;
    *width=Win->width;
    *height=Win->height;
}

//these callbacks go here not because they're X11 related, but becasue they are only referenced in
//X11BindCXineOutput
static void dest_size_cb(void *p_Win, int video_width, int video_height, double video_pixel_aspect, int *dest_width, int *dest_height, double *dest_pixel_aspect)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;


    *dest_width        = Win->width;
    *dest_height       = Win->height;
    *dest_pixel_aspect = Win->pixel_aspect;
}

static void frame_output_cb(void *p_Win, int video_width, int video_height, double video_pixel_aspect, int *dest_x, int *dest_y, int *dest_width, int *dest_height, double *dest_pixel_aspect, int *win_x, int *win_y)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;

    *dest_x            = 0;
    *dest_y            = 0;
    *win_x             = Win->x;
    *win_y             = Win->y;
    *dest_width        = Win->width;
    *dest_height       = Win->height;
    *dest_pixel_aspect = Win->pixel_aspect;
}


xine_video_port_t *X11BindCXineOutput(TConfig *Config)
{
    X11Window *Win;
    xine_video_port_t *vo_port;
    x11_visual_t vis;

    Win=(X11Window *) Config->X11Out;
    if (!Win->drawable) return(NULL);

    vis.display           = Win->display;
    vis.screen            = Win->screen;
    vis.d                 = Win->drawable;
    vis.dest_size_cb      = dest_size_cb;
    vis.frame_output_cb   = frame_output_cb;
    vis.user_data         = Win;

    vo_port = xine_open_video_driver(Config->xine, Config->vo_driver, XINE_VISUAL_TYPE_X11, (void *) &vis);
    if (vo_port == NULL) printf("I'm unable to initialize '%s' video driver. Giving up.\n", Config->vo_driver);

    return(vo_port);
}


void X11ActivateCXineOutput(void *p_Win, xine_video_port_t *vo_port)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;

    if (Win->drawable) xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) Win->drawable);
    xine_port_send_gui_data(vo_port, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);
}


void X11HandleConfigureEvent(X11Window *Win, XEvent *xevent)
{
    XConfigureEvent *cev = (XConfigureEvent *) xevent;
    Window           tmp_win;

    Win->width  = cev->width;
    Win->height = cev->height;

    if((cev->x == 0) && (cev->y == 0))
    {
        XLockDisplay(Win->display);
        XTranslateCoordinates(Win->display, cev->window, DefaultRootWindow(cev->display), 0, 0, &Win->x, &Win->y, &tmp_win);
        XUnlockDisplay(Win->display);
    }
    else
    {
        Win->x = cev->x;
        Win->y = cev->y;
    }
}

void X11HandleKeyPress(X11Window *Win, XEvent *xevent, TEvent *Event)
{
    char *kbuf;
    KeySym ksym;
    int len;

    Event->type=EVENT_KEYPRESS;
    XLockDisplay(Win->display);
    kbuf=calloc( 255, sizeof(char));
    len = XLookupString(&(xevent->xkey), kbuf, 255 * sizeof(char), &ksym, NULL);
    XUnlockDisplay(Win->display);

    switch (ksym)
    {
    case XK_Escape:
        Event->arg1=KEY_ESC;
        break;
    case XK_Return:
        Event->arg1=KEY_ENTER;
        break;
    case XK_Left:
        Event->arg1=KEY_LEFT;
        break;
    case XK_Right:
        Event->arg1=KEY_RIGHT;
        break;
    case XK_Up:
        Event->arg1=KEY_UP;
        break;
    case XK_Down:
        Event->arg1=KEY_DOWN;
        break;
    case XK_Page_Up:
        Event->arg1=KEY_PGUP;
        break;
    case XK_Page_Down:
        Event->arg1=KEY_PGDN;
        break;
    case XK_Home:
        Event->arg1=KEY_HOME;
        break;
    case XK_End:
        Event->arg1=KEY_END;
        break;
    case XK_F1:
        Event->arg1=KEY_F1;
        break;
    case XK_F2:
        Event->arg1=KEY_F2;
        break;
    case XK_F3:
        Event->arg1=KEY_F3;
        break;
    case XK_F4:
        Event->arg1=KEY_F4;
        break;
    case XK_F5:
        Event->arg1=KEY_F5;
        break;
    case XK_F6:
        Event->arg1=KEY_F6;
        break;
    case XK_F7:
        Event->arg1=KEY_F7;
        break;
    case XK_F8:
        Event->arg1=KEY_F8;
        break;
    case XK_F9:
        Event->arg1=KEY_F9;
        break;
    case XK_F10:
        Event->arg1=KEY_F10;
        break;
    case XK_F11:
        Event->arg1=KEY_F11;
        break;
    case XK_F12:
        Event->arg1=KEY_F12;
        break;
    case XK_F13:
        Event->arg1=KEY_F13;
        break;
    case XK_F14:
        Event->arg1=KEY_F14;
        break;
    case XK_F15:
        Event->arg1=KEY_F15;
        break;
    case XK_Super_L:
        Event->arg1=KEY_SUPERL;
        break;
    case XK_Super_R:
        Event->arg1=KEY_SUPERR;
        break;
    case XK_Insert:
        Event->arg1=KEY_INSERT;
        break;
    case XK_Delete:
        Event->arg1=KEY_DELETE;
        break;
    case XK_Pause:
        Event->arg1=KEY_PAUSE;
        break;
    case XK_BackSpace:
        Event->arg1=KEY_BACKSPACE;
        break;
  case XK_Tab:
        Event->arg1=KEY_TAB;
        break;


#ifdef XF86XK_AudioMute
    case XF86XK_AudioMute:
        Event->arg1=KEY_MUTE;
        break;
#endif

#ifdef XF86XK_AudioLowerVolume
    case XF86XK_AudioLowerVolume:
        Event->arg1=KEY_VOLUME_DOWN;
        break;
#endif

#ifdef XF86XK_AudioRaiseVolume
    case XF86XK_AudioRaiseVolume:
        Event->arg1=KEY_VOLUME_UP;
        break;
#endif

#ifdef XF86XK_AudioStop
    case XF86XK_AudioStop:
        Event->arg1=KEY_STOP;
        break;
#endif

#ifdef XF86XK_AudioPlay
    case XF86XK_AudioPlay:
        Event->arg1=KEY_PLAY;
        break;
#endif

#ifdef XF86XK_AudioPrev
    case XF86XK_AudioPrev:
        Event->arg1=KEY_PREV;
        break;
#endif

#ifdef XF86XK_AudioNext
    case XF86XK_AudioNext:
        Event->arg1=KEY_NEXT;
        break;
#endif

#ifdef XF86XK_AudioPause
    case XF86XK_AudioPause:
        Event->arg1=KEY_PAUSE;
        break;
#endif


    default:
        Event->arg1=*kbuf;
        break;
    }

    Event->arg2=0;
    if (xevent->xkey.state & ShiftMask) Event->arg2 |= KEYMOD_SHIFT;
    if (xevent->xkey.state & ControlMask) Event->arg2 |= KEYMOD_CTRL;
    if (xevent->xkey.state & Mod1Mask) Event->arg2 |= KEYMOD_ALT;

    if (Event->arg1==KEY_INSERT) X11RequestSelection(Win->display, Win->drawable, XA_PRIMARY, 0);
}


int X11NextEvent(void *p_Win, xine_video_port_t *vo_port, TEvent *Event)
{
    XEvent xevent;
    int got_event;
    X11Window *Win;

    Win=(X11Window *) p_Win;
    Event->type=EVENT_NONE;

    XLockDisplay(Win->display);
    got_event=XPending(Win->display);
    XUnlockDisplay(Win->display);

    while (got_event)
    {
        XLockDisplay(Win->display);
        XNextEvent(Win->display, &xevent);
        XUnlockDisplay(Win->display);

        switch(xevent.type)
        {
        case Expose:
            Event->type=EVENT_EXPOSE;
            if (vo_port && (xevent.xexpose.count == 0)) xine_port_send_gui_data(vo_port, XINE_GUI_SEND_EXPOSE_EVENT, &xevent);
            break;

        case ConfigureNotify:
            Event->type=EVENT_RESIZE;
            X11HandleConfigureEvent(Win, &xevent);
            break;

        case KeyPress:
            X11HandleKeyPress(Win, &xevent, Event);
            break;

        case SelectionNotify:
            xevent.xselection.selection;
            if (xevent.xselection.property !=None)
            {
                X11HandleSelectionAvailableEvent(Win->display, Win->drawable, xevent.xselection.selection);
                Event->type=EVENT_NONE;
            }
            else
            {
                Event->type=EVENT_NONE;
                //X11TryRequestSelectionType(X11Win, NextEvent.xselection.selection, FALSE);
            }
            break;

        case ClientMessage:
            if (
                (xevent.xclient.message_type==WMProtocols) &&
                (xevent.xclient.data.l[0]==WMDeleteWindow)
            )
                Event->type=EVENT_CLOSE;
            break;

        case EnterNotify:
            break;

        case LeaveNotify:
            break;

        }

        XLockDisplay(Win->display);
        got_event=XPending(Win->display);
        XUnlockDisplay(Win->display);
    }

    return(1);
}



void X11WindowSetTitle(void *p_Win, const char *MainTitle, const char *IconTitle)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;
    if (! IconTitle) IconTitle="";
    if (! MainTitle) MainTitle=IconTitle;
    X11SetTextProperty(Win, "WM_ICON_NAME", IconTitle);
    X11SetTextProperty(Win, "_NET_WM_ICON_NAME", IconTitle);

    X11SetTextProperty(Win, "WM_NAME", MainTitle);
    X11SetProperty(Win->display, Win->drawable, XA_UTF8_STRING, "_NET_WM_NAME", (const unsigned char *) MainTitle, StrLen(MainTitle));
    X11SetProperty(Win->display, Win->drawable, XA_UTF8_STRING, "_NET_WM_VISIBLE_NAME", (const unsigned char *) MainTitle, StrLen(MainTitle));
}


void X11Fit(void *p_Win, int *x, int *y, int *wid, int *high)
{
    int parent_wid, parent_high;

    X11WindowDimensions(p_Win, &parent_wid, &parent_high);


//addition 'cos negative values
    if (*x < 0) *x=parent_wid+*x;
    if (*y < 0) *y=parent_high+*y;

//addition 'cos negative values
    if (*wid <= 0) *wid=parent_wid + (*wid);
    if (*high < 0) *high=parent_high + (*high);

}


int X11KeyGrabsControl(void *p_Win, int Grab, const char *keystr)
{
    X11Window *Win;
    int keycode, keysym=0, modifiers=0;
    char tmpBuff[2];
    const char *ptr;

    Win=(X11Window *) p_Win;

    ptr=keystr;

    if (strncmp(keystr, "shift-",6)==0)
    {
        modifiers |= ShiftMask;
        ptr+=6;
    }

    if (strncmp(keystr, "ctrl-",5)==0)
    {
        modifiers |= ControlMask;
        ptr+=5;
    }

    if (strncmp(keystr, "alt-",4)==0)
    {
        modifiers |= Mod1Mask;
        ptr+=4;
    }


    if (StrLen(ptr)==1)
    {
        tmpBuff[0]=*ptr & 0xFF;
        tmpBuff[1]='\0';
        keysym = XStringToKeysym(tmpBuff);
    }
    else
    {
        if (strcmp(ptr,"left")==0) keysym=XK_Left;
        if (strcmp(ptr,"right")==0) keysym=XK_Right;
        if (strcmp(ptr,"up")==0) keysym=XK_Up;
        if (strcmp(ptr,"down")==0) keysym=XK_Down;
        if (strcmp(ptr,"pgup")==0) keysym=XK_Page_Up;
        if (strcmp(ptr,"pgdn")==0) keysym=XK_Page_Down;
        if (strcmp(ptr,"home")==0) keysym=XK_Home;
        if (strcmp(ptr,"end")==0) keysym=XK_End;
        if (strcmp(ptr,"ins")==0) keysym=XK_Insert;
        if (strcmp(ptr,"del")==0) keysym=XK_Delete;
        if (strcmp(ptr,"insert")==0) keysym=XK_Insert;
        if (strcmp(ptr,"delete")==0) keysym=XK_Delete;
        if (strcmp(ptr,"esc")==0) keysym=XK_Escape;
        if (strcmp(ptr,"escape")==0) keysym=XK_Escape;
        if (strcmp(ptr,"pause")==0) keysym=XK_Pause;
        if (strcmp(ptr,"tab")==0) keysym=XK_Tab;

#ifdef XF86XK_AudioMute
        if (strcmp(ptr, "mute")==0) keysym=XF86XK_AudioMute;
#endif

#ifdef XF86XK_AudioLowerVolume
        if (strcmp(ptr, "vdown")==0) keysym=XF86XK_AudioLowerVolume;
#endif

#ifdef XF86XK_AudioRaiseVolume
        if (strcmp(ptr, "vup")==0) keysym=XF86XK_AudioRaiseVolume;
#endif

#ifdef XF86XK_AudioStop
        if (strcmp(ptr, "stop")==0) keysym=XF86XK_AudioStop;
#endif

#ifdef XF86XK_AudioPlay
        if (strcmp(ptr, "play")==0) keysym=XF86XK_AudioPlay;
#endif

#ifdef XF86XK_AudioPrev
        if (strcmp(ptr, "prev")==0) keysym=XF86XK_AudioPrev;
#endif

#ifdef XF86XK_AudioNext
        if (strcmp(ptr, "next")==0) keysym=XF86XK_AudioNext;
#endif

#ifdef XF86XK_AudioPause
        if (strcmp(ptr, "vpause")==0) keysym=XF86XK_AudioPause;
#endif
    }


    if (keysym > 0)
    {
        keycode = XKeysymToKeycode (Win->display, keysym);
				//keycode 0 will grab everything! But is returned if key doesn't exist on this keyboard
				if (keycode > 0)
				{
				if (Grab) XGrabKey(Win->display, keycode, modifiers, DefaultRootWindow(Win->display), False, GrabModeAsync, GrabModeAsync);
        else XUngrabKey(Win->display, keycode, modifiers, DefaultRootWindow(Win->display));
				}
    }

}



int X11GrabKey(void *p_Win, const char *keystr)
{
return(X11KeyGrabsControl(p_Win, TRUE, keystr));
}


int X11UnGrabKey(void *p_Win, const char *keystr)
{
return(X11KeyGrabsControl(p_Win, TRUE, keystr));
}

void X11Disassociate(void *p_Win)
{
    X11Window *Win;
		int fd;

    Win=(X11Window *) p_Win;
    fd=XConnectionNumber(Win->display);
		close(fd);
}

void X11Close(void *p_Win)
{
    X11Window *Win;

    Win=(X11Window *) p_Win;
    XLockDisplay(Win->display);
    if (Win->drawable)
    {
        XUnmapWindow(Win->display, Win->drawable);
        XDestroyWindow(Win->display, Win->drawable);
    }
#ifdef HAVE_XSCREENSAVER
    if (Config->flags & DISABLE_SCREENSAVER) X11ScreenSaver(p_Win, True);
#endif
    XUnlockDisplay(Win->display);
    XCloseDisplay (Win->display);
    free(Win);
}
