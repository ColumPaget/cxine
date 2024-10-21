/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "help_osd.h"
#include "X11.h"

static CXineOSD *OSD=NULL;


void HelpOSDUpdate()
{
    if (! (Config->state & STATE_HELP_DISPLAYED) ) return;
    if (! Config->X11Out) return;

    if (OSD)
    {
        OSD->Contents=rstrcpy(OSD->Contents, "spacebar:pause <: prev item >: next item\nm: mute -: vol down +: vol up\nf: toggle fullscreen T: toggle stay-on-top\na: cycle audio outputs\no: toggle status display  i: toggle media info\np: show playlist  l: load file menu\nshift-tab: toggle stay-on-top\nleft: back 10 secs  shift-left: prev item\nright: back 10 secs  shift-right: next item");
        OSDUpdateSingle(OSD,TRUE);
    }
}


void HelpOSDHide()
{
    if (! Config->X11Out) return;

    if (OSD)
    {
        xine_osd_hide(OSD->osd, 0);
        OSDDestroy(OSD);
        OSD=NULL;
    }
    Config->state &= ~ STATE_HELP_DISPLAYED;
}



void HelpOSDShow()
{

    if (Config->state & STATE_HELP_DISPLAYED) return;
    if (! Config->X11Out) return;

    if (! OSD)
    {
        OSD=OSDCreate(Config->X11Out, Config->stream, "0,24,-20,-20 font=mono", "");
    }
    Config->state |= STATE_HELP_DISPLAYED;
    HelpOSDUpdate();
}


void HelpOSDToggle()
{
    if (! Config->X11Out) return;

    if (Config->state & STATE_HELP_DISPLAYED) HelpOSDHide();
    else HelpOSDShow();
}
