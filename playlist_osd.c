#include "playlist_osd.h"
#include "playlist.h"
#include "playback_control.h"
#include "keypress.h"
#include "X11.h"

CXineOSD *OSD=NULL;
int pos=0;

void PlaylistOSDKeypress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
	switch (keychar)
	{
		case KEY_DOWN:
			if (pos < (Config->playlist->size-1)) pos++;
		break;

		case KEY_UP:
			if (pos > 0) pos--;
		break;

		case 'p':
			PlaylistOSDHide();
		break;

		case KEY_ENTER:
		case KEY_RIGHT:
			 CXineSelectStream(Config, pos);
		break;
	}
	if (Config->state & STATE_PLAYLIST_DISPLAYED) PlaylistOSDUpdate();
}


int PlaylistOSDGetScreenHeight(CXineOSD *OSD)
{
int x=0, y=0, wid, high;

wid=OSD->wid;
high=OSD->high;

X11Fit(Config->X11Out, &x, &y, &wid, &high);

return(high);
}


void PlaylistOSDUpdate()
{
char *Title=NULL;
const char *ptr;
int i, y=0, wid, high, osd_high=0, start=0, per_page;
int playing=-1, val;

if (! OSD) OSD=OSDCreate(Config->X11Out, Config->stream, "0,10,-20,-20 font=mono", "");
xine_osd_clear(OSD->osd);
xine_osd_set_text_palette(OSD->osd, XINE_TEXTPALETTE_YELLOW_BLACK_TRANSPARENT, XINE_OSD_TEXT2);

playing=StringListPos(Config->playlist);

osd_high=PlaylistOSDGetScreenHeight(OSD);
xine_osd_get_text_size(OSD->osd, "TEST", &wid, &high);
per_page=osd_high / high;
per_page-=2;

if ((pos) > per_page) start=pos - per_page;
if (start < 0) start=0;

for (i=start; i < Config->playlist->size; i++)
{
	ptr=StringListItem(Config->playlist, i);
	PlaylistParseEntry(ptr, NULL, NULL, &Title);

	if (i==playing) val=XINE_OSD_TEXT2;
	else val=XINE_OSD_TEXT1;

	if (i == pos) xine_osd_draw_text(OSD->osd, 0, y, ">", val);
	xine_osd_draw_text(OSD->osd, 10, y, Title, val);
	
	y+=high;
}

xine_osd_show_unscaled(OSD->osd, 0);

destroy(Title);
}


void PlaylistOSDHide()
{
	if (OSD) xine_osd_hide(OSD->osd, 0);
	Config->state &= ~ STATE_PLAYLIST_DISPLAYED;
}



void PlaylistOSDShow()
{
	PlaylistOSDUpdate();
	Config->state |= STATE_PLAYLIST_DISPLAYED;
}
