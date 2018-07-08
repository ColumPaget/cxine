
#ifndef CXINE_COMMON_H
#define CXINE_COMMON_H

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <xine.h>
#include <xine/xineutils.h>

#define StrLen(s) ((s) ? strlen(s) : 0)
#define destroy(s) ((s) ? free(s) : 0)


#define TOGGLE -1
#define SET_ADD 1

#define LOOP_FOREVER -1

#define EVENT_NONE 1
#define EVENT_EXPOSE 2
#define EVENT_RESIZE 3
#define EVENT_KEYPRESS 4
#define EVENT_CLOSE 99
#define EVENT_TIMEOUT 100
#define EVENT_EXIT 200


typedef struct
{
int type;
int arg1;
int arg2;
} TEvent;

typedef struct
{
int next;
int size;
char **list;
} TStringList;


#define CONFIG_MUTE 1
#define CONFIG_HALFSPEED 2
#define CONFIG_QUARTSPEED 4
#define CONFIG_PAUSE 8
#define CONFIG_LOUD  16
#define CONFIG_VLOUD 32
#define CONFIG_BOOKMARK 128
#define CONFIG_PERSIST  256
#define CONFIG_OSD      512
#define CONFIG_CONTROL  1024
#define CONFIG_IDENTIFY 2048
#define CONFIG_SHUFFLE  4096
#define CONFIG_ALLOW_KEY_EXIT 8192
#define DISABLE_SCREENSAVER   16384

//these mustn't clash with CONFIG_ 
#define STATE_PLAYING  32768
#define STATE_NEWTITLE 65536
#define STATE_RAISED 131072
#define STATE_SHADED 262144


typedef struct
{
int flags;
int width;
int height;
int zoom;
int audio_compression;
int brightness;
int contrast;
char *parent;
char *vo_driver;
char *ao_driver;
TStringList *playlist;
char *CurrTitle;
double startms;
int DVDNavButtons;
int control_pipe;
int nowplay_pipe;
char *audio_plugins;
char *control_pipe_path;
char *top_osd_text;
char *bottom_osd_text;
char *keygrabs;
int priority;
int loop;
int image_ms;
} TConfig;

extern TConfig *Config;
extern int running;

const char *cbasename(const char *Path);
char *rstrcat(char *Dest, const char *Src);
char *rstrcpy(char *Dest, const char *Src);
char *rstrlcat(char *Dest, const char *Src, int SrcLen);
char *rstrlcpy(char *Dest, const char *Src, int SrcLen);
const char *rstrtok(const char *Str, const char *Separators, char **Token);

TStringList *StringListCreate(int argc, char **argv);
int StringListAdd(TStringList *sl, const char *str);
int StringListSplit(TStringList *sl, const char *str, const char *separators);
const char *StringListGet(TStringList *sl, unsigned int pos);
void StringListSet(TStringList *sl, unsigned int pos, const char *Str);
const char *StringListCurr(TStringList *sl);
const char *StringListPrev(TStringList *sl);
const char *StringListNext(TStringList *sl);
void StrListDestroy(TStringList *sl);

#endif
