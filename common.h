
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

#include "string_list.h"


#ifndef VERSION
#define VERSION                   "6.6"
#endif


#define StrLen(s) ((s) ? strlen(s) : 0)
#define destroy(s) ((s) ? free(s) : 0)

#define TRUE 1
#define FALSE 0

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


#define CONFIG_MUTE 1
#define CONFIG_HALFSPEED 2
#define CONFIG_QUARTSPEED 4
#define CONFIG_PAUSE 8
#define CONFIG_LOUD  16
#define CONFIG_VLOUD 32
#define CONFIG_NOAUTOPLAY    64
#define CONFIG_BOOKMARK     128
#define CONFIG_PERSIST      256
#define CONFIG_OSD          512
#define CONFIG_CONSOLE_OSD 1024
#define CONFIG_RECURSIVE   2048 
#define CONFIG_SHUFFLE     4096
#define CONFIG_ALLOW_KEY_EXIT 8192
#define DISABLE_SCREENSAVER   16384
#define KILL_SCREENSAVER      32768
#define CONFIG_NOVIDEO        65536
#define CONFIG_STREAM         131072
#define CONFIG_WEBCAST        262144
#define CONFIG_DEBUG   1048576
#define CONFIG_SAVE    2097152
#define CONFIG_SLAVE   4194304
#define CONFIG_CONTROL 8388608
#define CONFIG_READ_STDIN 16777216
#define CONFIG_PLAYLIST   33554432
#define CONFIG_IDENTIFY   67108864


#define STATE_NEWTITLE 1
#define STATE_RAISED   2
#define STATE_LOWERED  4
#define STATE_ICONIZED 8
#define STATE_SHADED   16
#define STATE_FULLSCREEN 32
#define STATE_STDIN_URL 1024
#define STATE_BACKGROUND_DISPLAYED 2048
#define STATE_PLAYLIST_DISPLAYED 4096
#define STATE_PLAYLIST_REQUESTED 8192
#define STATE_DOWNLOADING 16384
#define STATE_PLAYING 32768
#define STATE_LOADFILES_DISPLAYED 65536
#define STATE_INFO_DISPLAYED 131172
#define STATE_HELP_DISPLAYED 262344



typedef struct
{
int flags;
int state;
int width;
int height;
int zoom;
int loop;
int speed;
int debug;
// file descriptors. stdin is a file descriptor for stdin after we've moved it from fd0
// 'to_cxine' is the new fd0 which the cxine player process considers to be stdin, and
// down which we can pipe media. 'control_pipe' is the file descriptor for mplayer-style
// control commands, and 'nowplay_pipe' is a pipe down which we send our 'now playing'
// status
int stdin;
int to_xine;
int control_pipe;
int nowplay_pipe;
int audio_compression;
int brightness;
int contrast;
char *parent;
char *vo_driver;
char *ao_driver;
const char *ao_curr;
TStringList *playlist;
char *CurrTitle;
double startms;
char *path_prefix;
char *audio_plugins;
char *control_pipe_path;
char *nowplay_pipe_path;
char *top_osd_text;
char *bottom_osd_text;
char *console_osd_text;
char *keygrabs;
char *background;
char *cache_dir;
char *helpers;
int cache_maxage;
int playlist_update_time;
int priority;
int image_ms;
int bcast_port;
void *X11Out;
int DVDNavButtons;
xine_t              *xine;
xine_stream_t       *stream;
xine_video_port_t   *vo_port;
xine_audio_port_t   *ao_port;
xine_event_queue_t  *event_queue;
} TConfig;

extern TConfig *Config;
extern int running;
extern time_t Now;

unsigned int fnv_hash(unsigned const char *key, int NoOfItems);
const char *cbasename(const char *Path);
void strrep(char *Str, char c1, char c2);
char *rstrcat(char *Dest, const char *Src);
char *rstrcpy(char *Dest, const char *Src);
char *rstrlcat(char *Dest, const char *Src, int SrcLen);
char *rstrlcpy(char *Dest, const char *Src, int SrcLen);
const char *rstrtok(const char *Str, const char *Separators, char **Token);
char *rstrquot(char *RetStr, const char *Str, const char *QuoteChars);
char *rstrunquot(char *RetStr, const char *Str);

void TouchFile(const char *Path);
void MkDirPath(const char *Dir);
char *PathSearch(char *RetStr, const char *FileName, const char *Path);
void StripQuotes(char *Str);

void LongFormatMetric(char *Str, int len, long value);

int ParseURL(const char *URL, char **Proto, char **Host, char **Port, char **Path);
void Exec(const char *CmdLine);

#endif
