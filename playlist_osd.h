
#ifndef CXINE_PLAYLIST_OSD_H
#define CXINE_PLAYLIST_OSD_H

#include "osd.h"
#include "common.h"

int PlaylistOSDKeypress(void *X11Out, xine_stream_t *stream, int keychar, int modifier);
void PlaylistOSDUpdate();
void PlaylistOSDShow();
void PlaylistOSDHide();

#endif
