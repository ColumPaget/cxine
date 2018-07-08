#ifndef CXINE_PLUGINS_H
#define CXINE_PLUGINS_H

#include "common.h"

void XineDisplayPlugins(xine_t *xine);
void XineAddAudioPostPlugins(xine_t *xine, xine_stream_t *stream, const char *plugins, xine_audio_port_t **ao_port, xine_video_port_t **vo_port);
void XineAddVideoPostPlugins(xine_t *xine, xine_stream_t *stream, xine_audio_port_t **ao_port, xine_video_port_t **vo_port);

#endif
