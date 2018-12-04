
#ifndef CXINE_OSD_H
#define CXINE_OSD_H

#include "common.h"

#define DEFAULT_TOPOSD_STRING "%tt %Lp/%Ls %T - %A"
#define DEFAULT_BOTTOMOSD_STRING "POS: %tP%% %tw  VOL: %v  ac: %aw"

typedef struct
{
int type;
int x;
int y;
int wid;
int high;
char *Contents;
char *Prev;
char *Font;
void *X11Win;
xine_osd_t *osd;
xine_stream_t *stream;
} CXineOSD;

void OSDSetup(TConfig *Config);
CXineOSD *OSDCreate(void *X11Win, xine_stream_t *stream, const char *config, const char *text);
CXineOSD *OSDButtonsCreate(void *X11Win, xine_stream_t *stream, const char *config);
CXineOSD *OSDMessage(int x, int y, const char *Text);
void OSDUpdateSingle(CXineOSD *OSD, int show);
void OSDUpdate(int Show);
void OSDDestroy(CXineOSD *OSD);

#endif
