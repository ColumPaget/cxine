
#ifndef CXINE_LOADFILES_OSD_H
#define CXINE_LOADFILES_OSD_H

#include "osd.h"
#include "common.h"

int LoadFilesOSDKeypress(void *X11Out, xine_stream_t *stream, int keychar, int modifier);
void LoadFilesOSDUpdate();
void LoadFilesOSDShow();
void LoadFilesOSDHide();

#endif
