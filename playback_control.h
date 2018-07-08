#ifndef CXINE_PLAYBACK_H
#define CXINE_PLAYBACK_H

#include "common.h"


#define PLAY_NEXT -1
#define PLAY_PREV -2


#define XineVolume(s, t, v) ( XineSetRangeValue((s), XINE_PARAM_AUDIO_VOLUME, (t), (v)) )
#define XineAudioComp(s, t, v) ( XineSetRangeValue((s), XINE_PARAM_AUDIO_COMPR_LEVEL, (t), (v)) )
#define XineBrightness(s, t, v) ( XineSetRangeValue((s), XINE_PARAM_VO_BRIGHTNESS, (t), (v)) )
#define XineContrast(s, t, v) ( XineSetRangeValue((s), XINE_PARAM_VO_CONTRAST, (t), (v)) )


void XineEventSend(xine_stream_t *stream, int type);
void XineMute(xine_stream_t *stream, int Setting);
void XinePause(xine_stream_t *stream);
void XineSetRangeValue(xine_stream_t *stream, int Type, int SetType, int Value);
int XinePlayStream(xine_stream_t *stream, const char *url);
int XineSelectStream(xine_stream_t *stream, int Which);


#endif
