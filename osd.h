
#ifndef CXINE_OSD_H
#define CXINE_OSD_H

#include "common.h"

typedef struct
{
int x;
int y;
int wid;
int high;
char *Contents;
void *X11Win;
xine_osd_t *osd;
xine_stream_t *stream;
} CXineOSD;

CXineOSD *OSDCreate(void *X11Win, xine_stream_t *stream, const char *config);
void OSDUpdate(CXineOSD *OSD, int Show);
void *OSDDestroy(CXineOSD *OSD);

#endif
