/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "media_info_osd.h"
#include "X11.h"
#include <glob.h>

static CXineOSD *OSD=NULL;


void InfoOSDUpdate()
{
if (! (Config->state & STATE_INFO_DISPLAYED) ) return;

OSD->Contents=OSDFormatString(OSD->Contents, "Playing: %P:%T\nVideo: [%v4] %wx%h %vC\nAudio: [%a4] %ac %aC\nArtist: %ma\nTitle: %mt\n", Config->stream);
OSDUpdateSingle(OSD,TRUE);
}


void InfoOSDHide()
{
	if (OSD) 
	{
		xine_osd_hide(OSD->osd, 0);
		OSDDestroy(OSD);
		OSD=NULL;
	}
	Config->state &= ~ STATE_INFO_DISPLAYED;
}



void InfoOSDShow()
{

	if (Config->state & STATE_INFO_DISPLAYED) return;
	if (! OSD) 
	{
		OSD=OSDCreate(Config->X11Out, Config->stream, "0,10,-20,-20 font=mono", "");
	}
	Config->state |= STATE_INFO_DISPLAYED;
	InfoOSDUpdate();
}
