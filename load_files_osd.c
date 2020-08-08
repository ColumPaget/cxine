/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "playlist_osd.h"
#include "playlist.h"
#include "playback_control.h"
#include "keypress.h"
#include "X11.h"
#include <glob.h>

static CXineOSD *OSD=NULL;
static char *LoadFilesCurrDir=NULL;
static TStringList *FilesList=NULL;
static int pos=0;



static void LoadFiles(const char *Dir)
{
glob_t Glob;
char *Tempstr=NULL;
int i;

if (! FilesList) FilesList=(TStringList *) calloc(1, sizeof(TStringList));
StringListClear(FilesList);
pos=0;

StringListAdd(FilesList, "../");
Tempstr=rstrcpy(Tempstr, Dir);
Tempstr=rstrcat(Tempstr, "/*");

glob(Tempstr, GLOB_MARK, 0, &Glob);
for (i=0; i < Glob.gl_pathc; i++)
{
	StringListAdd(FilesList, Glob.gl_pathv[i]);
}
globfree(&Glob);

destroy(Tempstr);
}


static void LoadFilesOSDEnterDirectory(const char *dir)
{
char *ptr;

	//if it's '../' then it's 'parent directory', so we 
	//chop one level off 'LoadFilesCurrDir'
	if (strcmp(dir, "../")==0)
	{
			ptr=strrchr(LoadFilesCurrDir, '/');
			if (ptr) *ptr='\0';
	}
	//otherwise it set the item as the new directory	
	else LoadFilesCurrDir=rstrcpy(LoadFilesCurrDir, dir);
				
	//as we've changed directory, reload file list
	LoadFiles(LoadFilesCurrDir);
}


int LoadFilesOSDKeypress(void *X11Out, xine_stream_t *stream, int keychar, int modifier)
{
int result=FALSE, old;
char *ptr, *item;

	switch (keychar)
	{
		case KEY_DOWN:
			if (pos < (FilesList->size-1)) 
			{
			if (modifier & KEYMOD_SHIFT) pos=PlaylistMoveItem(FilesList, pos, +1);
			else pos++;
			}
			result=TRUE;
		break;

		case KEY_UP:
			if (pos > 0) 
			{
			if (modifier & KEYMOD_SHIFT) pos=PlaylistMoveItem(FilesList, pos, -1);
			else pos--;
			}
			result=TRUE;
		break;

		case 'l':
			LoadFilesOSDHide();
			result=TRUE;
		break;

		case KEY_ENTER:
			item=StringListGet(FilesList, pos);
			ptr=strrchr(item, '/');
			//if it ends in a '/' it's a directory
			if ( ptr && (*(ptr+1)=='\0') ) LoadFilesOSDEnterDirectory(item);
			// if it's not a directory add it to the playlist
			else PlaylistAdd(Config->playlist, item, "", "");
			result=TRUE;
		break;

		case KEY_DELETE:
		case KEY_BACKSPACE:
			LoadFilesOSDEnterDirectory("../");
		break;
	}
	LoadFilesOSDUpdate();

	return(result);
}





void LoadFilesOSDUpdate()
{
char *Title=NULL;
const char *ptr;
int i, y=0, wid, high, osd_high=0, start=0, count=0, per_page;
int val;

if (! (Config->state & STATE_LOADFILES_DISPLAYED)) return;
xine_osd_clear(OSD->osd);
xine_osd_set_text_palette(OSD->osd, XINE_TEXTPALETTE_YELLOW_BLACK_TRANSPARENT, XINE_OSD_TEXT2);


osd_high=OSDGetScreenHeight(OSD);
xine_osd_get_text_size(OSD->osd, "TEST", &wid, &high);
per_page=osd_high / high;
per_page-=3;

if ((pos) > per_page) start=pos - per_page;
if (start < 0) start=0;

for (i=start; i < FilesList->size; i++)
{
	ptr=StringListItem(FilesList, i);
	PlaylistParseEntry(ptr, NULL, NULL, &Title);

	val=XINE_OSD_TEXT1;

	if (i == pos) xine_osd_draw_text(OSD->osd, 0, y, ">", val);
	xine_osd_draw_text(OSD->osd, 10, y, Title, val);
	y+=high;
	count++;
	if (count > per_page) break;
}


Title=rstrcpy(Title, "arrows: cursor, enter: add, bksp/del:cd up");
xine_osd_draw_text(OSD->osd, 10, osd_high-high, Title, XINE_OSD_TEXT1);
xine_osd_show_unscaled(OSD->osd, 0);

destroy(Title);
}


void LoadFilesOSDHide()
{
	if (OSD) 
	{
		xine_osd_hide(OSD->osd, 0);
//		OSDDestroy(OSD);
//		OSD=NULL;
	}
	Config->state &= ~ STATE_LOADFILES_DISPLAYED;
}



void LoadFilesOSDShow()
{
char *Tempstr=NULL;

	if (Config->state & STATE_PLAYLIST_DISPLAYED) return;
	if (! OSD) 
	{
		OSD=OSDCreate(Config->X11Out, Config->stream, "0,10,-20,-20 font=mono", "");
		Tempstr=calloc(1024, sizeof(char));
		getcwd(Tempstr, 1024);
		LoadFilesCurrDir=rstrcpy(LoadFilesCurrDir, Tempstr);
		LoadFiles(LoadFilesCurrDir);
	}
	Config->state |= STATE_LOADFILES_DISPLAYED;
	LoadFilesOSDUpdate();
	destroy(Tempstr);
}
