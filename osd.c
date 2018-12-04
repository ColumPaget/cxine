#include "X11.h"
#include "osd.h"
#include "playlist.h"
#include <time.h>

#define VALUE_STR_LEN 255

#define FMT_SECS     1
#define FMT_POS_SECS 2
#define FMT_LEN_SECS 3
#define FMT_PERCENT  4
#define FMT_EXPENDED  5
#define FMT_DURATION  6

#define MIN_MINSECS 60000



#define OSD_TEXT    0
#define OSD_BUTTONS 1


CXineOSD *topOSD=NULL;
CXineOSD *bottomOSD=NULL;
CXineOSD *buttonsOSD=NULL;



char *OSDFormatMSecs(char *ValueStr, int msecs)
{
int mins, secs;

	mins=msecs / MIN_MINSECS;
	secs=(msecs % MIN_MINSECS) / 1000;
	snprintf(ValueStr, VALUE_STR_LEN, "%d:%d", mins, secs);

return(ValueStr);
}


char *OSDFormatTime(char *ValueStr, const char *Format)
{
time_t Now;
struct tm *tms;

time(&Now);
tms=localtime(&Now);

strftime(ValueStr, VALUE_STR_LEN, Format, tms);

return(ValueStr);
}


char *OSDFormatDuration(char *ValueStr, xine_stream_t *stream, int FormatType)
{
int pos, pos_msecs, len_msecs;

xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
switch (FormatType)
{
case FMT_SECS:
	snprintf(ValueStr, VALUE_STR_LEN, "%d/%d", pos_msecs	/1000, len_msecs / 1000);
break;

case FMT_POS_SECS:
	snprintf(ValueStr, VALUE_STR_LEN, "%0.3f", ((float) pos_msecs)	/1000);
break;

case FMT_LEN_SECS:
	snprintf(ValueStr, VALUE_STR_LEN, "%0.3f", ((float) len_msecs)	/1000);
break;

case FMT_PERCENT:
	snprintf(ValueStr, VALUE_STR_LEN, "%0.1f", (float) pos * 100 / 65535.0);
break;

case FMT_EXPENDED:
	OSDFormatMSecs(ValueStr, pos_msecs);
break;

case FMT_DURATION:
	OSDFormatMSecs(ValueStr, len_msecs);
break;

}

return(ValueStr);
}


char *OSDFormatParam(char *ValueStr, xine_stream_t *stream, int Type)
{
int val;

switch (Type)
{
case XINE_PARAM_FINE_SPEED:
snprintf(ValueStr, VALUE_STR_LEN, "%0.2f", ((float) xine_get_param(stream, Type)) / 1000000.0);
break;

case XINE_PARAM_AUDIO_VOLUME:
val=xine_get_param (stream, XINE_PARAM_AUDIO_AMP_MUTE);
if (val==1) strncpy(ValueStr, "mute", VALUE_STR_LEN);
else
{
	val=xine_get_param(stream, Type);
	if (val == -1) strncpy(ValueStr, "err", VALUE_STR_LEN);
	else snprintf(ValueStr, VALUE_STR_LEN, "%d", xine_get_param(stream, Type));
}
break;

case XINE_PARAM_AUDIO_COMPR_LEVEL:
val=xine_get_param (stream, Type);
if (val <= 100) strcpy(ValueStr, "off");
else snprintf(ValueStr, VALUE_STR_LEN, "%d", xine_get_param(stream, Type));
break;

default:
snprintf(ValueStr, VALUE_STR_LEN, "%d", xine_get_param(stream, Type));
break;
}

return(ValueStr);
}


char *OSDFormatValue(char *ValueStr, xine_stream_t *stream, int Type)
{

switch (Type)
{
default:
snprintf(ValueStr, VALUE_STR_LEN, "%d", xine_get_stream_info(stream, Type));
break;
}

return(ValueStr);
}




char *OSDFormatString(char *RetStr, const char *fmt, xine_stream_t *stream)
{
const char *ptr, *prev, *vptr;
char *ValueStr=NULL, *Tempstr=NULL;

prev=fmt;
ptr=strchr(fmt, '%');
if (! ptr) 
{
	RetStr=rstrcpy(RetStr, prev);
	return(RetStr);
}

ValueStr=(char *) realloc(ValueStr, VALUE_STR_LEN);
RetStr=rstrcpy(RetStr, "");
while (ptr && (*ptr !='\0'))
{
RetStr=rstrlcat(RetStr, prev, ptr-prev);
ptr++;
switch (*ptr)
{
				case '\0':
				case '%': RetStr=rstrcat(RetStr, "%"); break;
				case 'b': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_VO_BRIGHTNESS)); break;
				case 'B': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_BITRATE)); break;
				case 'w': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_VIDEO_WIDTH)); break;
				case 'h': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_VIDEO_HEIGHT)); break;

				case 't':
					ptr++;
					switch (*ptr)
					{					
					case 's': RetStr=rstrcat(RetStr, OSDFormatDuration(ValueStr, stream, FMT_POS_SECS)); break;
					case 'l': RetStr=rstrcat(RetStr, OSDFormatDuration(ValueStr, stream, FMT_LEN_SECS)); break;
					case 'w': RetStr=rstrcat(RetStr, OSDFormatDuration(ValueStr, stream, FMT_EXPENDED)); break;
					case 'S': RetStr=rstrcat(RetStr, OSDFormatDuration(ValueStr, stream, FMT_SECS)); break;
					case 'P': RetStr=rstrcat(RetStr, OSDFormatDuration(ValueStr, stream, FMT_PERCENT)); break;
					case 't': RetStr=rstrcat(RetStr, OSDFormatTime(ValueStr, "%H:%M")); break;
					case 'T': RetStr=rstrcat(RetStr, OSDFormatTime(ValueStr, "%H:%M:%S")); break;
					case 'd': RetStr=rstrcat(RetStr, OSDFormatTime(ValueStr, "y/%m/%d")); break;
					case 'D': RetStr=rstrcat(RetStr, OSDFormatTime(ValueStr, "Y/%m/%d")); break;
					case 'N': RetStr=rstrcat(RetStr, OSDFormatTime(ValueStr, "%Y/%m/%d %H:%M:%S")); break;
					}	
				break;	

				case 'o': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_AV_OFFSET)); break;
				case 'v': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_AUDIO_VOLUME)); break;
				case 'f': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_FINE_SPEED)); break;
				case 'A': RetStr=rstrcat(RetStr, xine_get_meta_info(stream, XINE_META_INFO_ARTIST)); break;
				case 'C': RetStr=rstrcat(RetStr, xine_get_meta_info(stream, XINE_META_INFO_COMMENT)); break;

				// Audio info
				case 'a': 
					ptr++;
					switch (*ptr)
					{
					case 'c': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_AUDIO_FOURCC)); break;
					case 'b': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_AUDIO_BITRATE)); break;
					case 's': RetStr=rstrcat(RetStr, OSDFormatValue(ValueStr, stream, XINE_STREAM_INFO_AUDIO_SAMPLERATE)); break;
					case 'v': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_AUDIO_VOLUME)); break;
					case 'w': RetStr=rstrcat(RetStr, OSDFormatParam(ValueStr, stream, XINE_PARAM_AUDIO_COMPR_LEVEL)); break;
					}
				break;

				// Playlist info
				case 'L':
					ptr++;
					switch (*ptr)
					{
						case 's': 
							snprintf(ValueStr, VALUE_STR_LEN, "%d",	StringListSize(Config->playlist));
							RetStr=rstrcat(RetStr, ValueStr);
						break;

						case 'p': 
							snprintf(ValueStr, VALUE_STR_LEN, "%d",	StringListPos(Config->playlist));
							RetStr=rstrcat(RetStr, ValueStr);
						break;
					}
				break;

				// Track metainfo 
				case 'm':
					ptr++;
					switch (*ptr)
					{
						case 'a':
						vptr=xine_get_meta_info(stream, XINE_META_INFO_ARTIST);
						RetStr=rstrcat(RetStr, vptr); 
						break;

						case 'A':
						vptr=xine_get_meta_info(stream, XINE_META_INFO_ALBUM);
						RetStr=rstrcat(RetStr, vptr); 
						break;

						case 'Y':
						vptr=xine_get_meta_info(stream, XINE_META_INFO_YEAR);
						RetStr=rstrcat(RetStr, vptr); 
						break;

						case 'G':
						vptr=xine_get_meta_info(stream, XINE_META_INFO_GENRE);
						RetStr=rstrcat(RetStr, vptr); 
						break;

						case 'c': 
						snprintf(ValueStr, VALUE_STR_LEN, "%d",	xine_get_stream_info(stream, XINE_STREAM_INFO_DVD_CHAPTER_NUMBER));
						RetStr=rstrcat(RetStr, ValueStr); 
						break;

						case 'C': 
						snprintf(ValueStr, VALUE_STR_LEN, "%d",	xine_get_stream_info(stream, XINE_STREAM_INFO_DVD_CHAPTER_COUNT));
						RetStr=rstrcat(RetStr, ValueStr); 
						break;


						case 'R': RetStr=rstrcat(RetStr, xine_get_meta_info(stream, XINE_META_INFO_COPYRIGHT)); break;

						case 't':
						case 'T': 
						Tempstr=PlaylistCurrTitle(Tempstr);
						RetStr=rstrcat(RetStr, Tempstr); 
						break;
					}
				break;

				case 'T': 
					Tempstr=PlaylistCurrTitle(Tempstr);
					RetStr=rstrcat(RetStr, Tempstr); 
				break;
}
if (*ptr != '\0') ptr++;
prev=ptr;
ptr=strchr(ptr, '%');
}
if (prev) RetStr=rstrcat(RetStr, prev);

destroy(ValueStr);
destroy(Tempstr);
return(RetStr);
}

void OSDDrawButton(CXineOSD *OSD, const char *Text, int x, int y, int *wid, int *high)
{
int ex, ey;

	xine_osd_get_text_size(OSD->osd, Text, wid, high);
	ex=x + *wid  + 4;
	ey=y + *high + 2;

	xine_osd_draw_rect(OSD->osd, x - 2, y, ex,  ey, 1, 0); 
	xine_osd_draw_text(OSD->osd, x, y, Text, XINE_OSD_TEXT1);
}


void OSDDrawButtons(CXineOSD *OSD)
{
int x=4, y=4, wid, high;

	OSDDrawButton(OSD, "<<", x, y, &wid, &high);
	x+=wid *2;

	OSDDrawButton(OSD, ">", x, y, &wid, &high);
	x+=wid *2;

	OSDDrawButton(OSD, "|", x, y, &wid, &high);
	x+=wid *2;

	OSDDrawButton(OSD, ">>", x, y, &wid, &high);
}


void OSDUpdateSingle(CXineOSD *OSD, int show)
{
int wid, high, result, pos;
char *Tempstr=NULL;
const char *ptr;

if (! OSD) return;

//xine_osd_set_palette(osd, &textpalettes_color, &textpalettes_trans);
//xine_osd_draw_rect(osd, 10, 10, 100, 30, XINE_OSD_TEXT1, 0);
/*
#define TEXTPALETTE_WHITE_BLACK_TRANSPARENT    0
#define TEXTPALETTE_WHITE_NONE_TRANSPARENT     1
#define TEXTPALETTE_WHITE_NONE_TRANSLUCID      2
#define TEXTPALETTE_YELLOW_BLACK_TRANSPARENT   3
*/

xine_osd_set_text_palette(OSD->osd, XINE_TEXTPALETTE_WHITE_NONE_TRANSLUCID, XINE_OSD_TEXT1);
xine_osd_set_encoding(OSD->osd, "");

ptr=rstrtok(OSD->Font, ",", &Tempstr);
while (ptr)
{
if (xine_osd_set_font(OSD->osd, Tempstr, 18)) break;
ptr=rstrtok(ptr, ",", &Tempstr);
}

switch (OSD->type)
{
case OSD_BUTTONS:
	OSDDrawButtons(OSD);
break;

default:
	Tempstr=OSDFormatString(Tempstr, OSD->Contents, OSD->stream);
	if (strcmp(Tempstr, OSD->Prev) !=0)
	{
	xine_osd_clear(OSD->osd);
	xine_osd_get_text_size(OSD->osd, Tempstr, &wid, &high);
	xine_osd_draw_text(OSD->osd, 0, 0, Tempstr, XINE_OSD_TEXT1);
	OSD->Prev=rstrcpy(OSD->Prev, Tempstr);
	}
break;
}

if (show) xine_osd_show_unscaled(OSD->osd, 0);
else xine_osd_hide(OSD->osd, 0);

destroy(Tempstr);
}


void OSDUpdate(int show)
{
OSDUpdateSingle(topOSD, show);
OSDUpdateSingle(bottomOSD, show);
//OSDUpdateSingle(buttonsOSD, show);
}



void OSDSetupGeometry(CXineOSD *OSD, const char *config)
{
char *Token=NULL;
const char *ptr;

ptr=rstrtok(config, ",", &Token);
if (  (! StrLen(Token)) || (strcmp(Token, "top")==0) )
{
	OSD->x=0;
	OSD->y=0;
	OSD->wid=0;
	OSD->high=20;
	if (ptr) OSD->Contents=strdup(ptr);
}
else if (strcmp(Token, "bottom")==0)
{
	OSD->x=0;
	OSD->y=-20;
	OSD->wid=0;
	OSD->high=-20;
	if (ptr) OSD->Contents=strdup(ptr);
}
else
{
	OSD->x=strtol(config,(char **) &ptr,10);
	if (ptr) ptr++;
	OSD->y=strtol(ptr,(char **) &ptr,10);
	if (ptr) ptr++;
	OSD->wid=strtol(ptr,(char **) &ptr,10);
	if (ptr) ptr++;
	OSD->high=strtol(ptr,(char **) &ptr,10);
	if (ptr) ptr++;
}

destroy(Token);
}


CXineOSD *OSDCreate(void *X11Win, xine_stream_t *stream, const char *config, const char *text)
{
xine_osd_t *osd;
int x=0, y=0, wid=0, high=0;
char *Token=NULL;
const char *ptr;
CXineOSD *OSD;

OSD=(CXineOSD *) calloc(1, sizeof(CXineOSD));
OSD->stream=stream;
OSD->X11Win=X11Win;
OSD->Prev=rstrcpy(NULL, "");

ptr=rstrtok(config, " ", &Token);
OSDSetupGeometry(OSD, Token);
ptr=rstrtok(ptr, " ", &Token);
while (ptr)
{
if (strncmp(Token,"font=", 5)==0) OSD->Font=rstrcpy(OSD->Font, Token+5);
if (strncmp(Token,"x=", 2)==0) OSD->x=atoi(Token+2);
if (strncmp(Token,"y=", 2)==0) OSD->y=atoi(Token+2);
if (strncmp(Token,"wid=", 5)==0) OSD->wid=atoi(Token+4);
if (strncmp(Token,"high=", 2)==0) OSD->high=atoi(Token+5);
if (strncmp(Token,"buttons", 2)==0) OSD->type=OSD_BUTTONS;

ptr=rstrtok(ptr, " ", &Token);
}
OSD->Contents=strdup(text);

x=OSD->x;
y=OSD->y;
wid=OSD->wid;
high=OSD->high;
X11Fit(X11Win, &x, &y, &wid, &high);

OSD->osd=xine_osd_new(stream, x, y, wid, high);
xine_osd_show(OSD->osd, 0);

destroy(Token);

return(OSD);
}


CXineOSD *OSDMessage(int x, int y, const char *Text)
{
char *Tempstr=NULL;
CXineOSD *OSD;

 Tempstr=realloc(Tempstr, 1024);
 snprintf(Tempstr, 1024, "%d,%d,-10,-10 font=mono",x,y);
 OSD=OSDCreate(Config->X11Out, Config->stream, Tempstr, Text);

 destroy(Tempstr);
 return(OSD);
}



void OSDDestroy(CXineOSD *OSD)
{
xine_osd_free(OSD->osd);
free(OSD->Contents);
free(OSD->Prev);
free(OSD);
}



void OSDSetup(TConfig *Config)
{
  if (StrLen(Config->top_osd_text))
  {
    if (topOSD) OSDDestroy(topOSD);
    topOSD=OSDCreate(Config->X11Out, Config->stream, "top font=mono", Config->top_osd_text);
  }

  if (StrLen(Config->bottom_osd_text))
  {
    if (bottomOSD) OSDDestroy(bottomOSD);
    bottomOSD=OSDCreate(Config->X11Out, Config->stream, "bottom font=mono", Config->bottom_osd_text);
  }

	/*
  if (buttonsOSD) OSDDestroy(buttonsOSD);
	buttonsOSD=OSDCreate(Config->X11Out, Config->stream, "top font=mono buttons x=10 y=-40 wid=-20 high=25", "");
	*/
}


