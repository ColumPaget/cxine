/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "playlist_osd.h"
#include "playlist.h"
#include "playback_control.h"
#include "keypress.h"
#include "X11.h"

static CXineOSD *OSD=NULL;
static int pos=0;

int PlaylistOSDKeypress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
    int result=FALSE, old;

    switch (keychar)
    {
    case KEY_DOWN:
        if (pos < (Config->playlist->size-1))
        {
            if (modifier & KEYMOD_SHIFT) pos=PlaylistMoveItem(Config->playlist, pos, +1);
            else pos++;
        }
        result=TRUE;
        break;

    case KEY_UP:
        if (pos > 0)
        {
            if (modifier & KEYMOD_SHIFT) pos=PlaylistMoveItem(Config->playlist, pos, -1);
            else pos--;
        }
        result=TRUE;
        break;

    case 'u':
        //change pos so cursor moves with item
        pos=PlaylistMoveItem(Config->playlist, pos, -1);
        result=TRUE;
        break;

    case 'd':
        //change pos so cursor moves with item
        pos=PlaylistMoveItem(Config->playlist, pos, +1);
        result=TRUE;
        break;


    case 'p':
        PlaylistOSDHide();
        result=TRUE;
        break;

    case KEY_ENTER:
        CXineSelectStream(Config, pos);
        result=TRUE;
        break;

    case KEY_DELETE:
    case KEY_BACKSPACE:
        //if we delete an item and now 'pos' is beyond the end of the playlist
        //them we must move 'pos' up to the previous item
        if (StringListDel(Config->playlist, pos) < pos) pos--;
        if (pos < 0) pos=0;
        result=TRUE;
        break;
    }
    PlaylistOSDUpdate();

    return(result);
}




void PlaylistOSDUpdate()
{
    char *Tempstr=NULL;
    const char *ptr;
    TPlaylistItem *PI;
    int i, y=0, wid, high, osd_high=0, start=0, count=0, per_page;
    int playing=-1, val;

    if (! (Config->state & STATE_PLAYLIST_DISPLAYED)) return;

    xine_osd_clear(OSD->osd);
    xine_osd_set_text_palette(OSD->osd, XINE_TEXTPALETTE_YELLOW_BLACK_TRANSPARENT, XINE_OSD_TEXT2);

    playing=StringListPos(Config->playlist);

    osd_high=OSDGetScreenHeight(OSD);
    xine_osd_get_text_size(OSD->osd, "TEST", &wid, &high);
    per_page=osd_high / high;
    per_page-=3;

    if ((pos) > per_page) start=pos - per_page;
    if (start < 0) start=0;

    for (i=start; i < Config->playlist->size; i++)
    {
        ptr=StringListItem(Config->playlist, i);
        PI=PlaylistDecodeEntry(ptr);

        if (i==playing) val=XINE_OSD_TEXT2;
        else val=XINE_OSD_TEXT1;

        if (i == pos) xine_osd_draw_text(OSD->osd, 0, y, ">", val);
        xine_osd_draw_text(OSD->osd, 10, y, PI->Title, val);

        y+=high;
        count++;
        PlaylistItemDestroy(PI);

        if (count > per_page) break;
    }

    Tempstr=rstrcpy(Tempstr, "arrows: cursor, enter: play, bksp/del: delete, u/d/shift-arrows: move");
    xine_osd_draw_text(OSD->osd, 10, osd_high-high, Tempstr, XINE_OSD_TEXT1);
    xine_osd_show_unscaled(OSD->osd, 0);

    destroy(Tempstr);
}


void PlaylistOSDHide()
{
    if (OSD)
    {
        xine_osd_hide(OSD->osd, 0);
        OSDDestroy(OSD);
        OSD=NULL;
    }
    Config->state &= ~ STATE_PLAYLIST_DISPLAYED;
}



void PlaylistOSDShow()
{
    if (Config->state & STATE_LOADFILES_DISPLAYED) return;
    if (! OSD) OSD=OSDCreate(Config->X11Out, Config->stream, "0,10,-20,-20 font=mono", "");
    Config->state |= STATE_PLAYLIST_DISPLAYED;
    PlaylistOSDUpdate();
}
