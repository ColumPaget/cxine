#ifndef CXINE_PLAYBACK_H
#define CXINE_PLAYBACK_H

#include "common.h"


#define PLAY_NEXT -1
#define PLAY_PREV -2
#define PLAY_FAIL -3


#define SKIP_SMALL 10000
#define SKIP_LARGE 60000

#define CXineVolume(s, t, v) ( CXineSetRangeValue((s), XINE_PARAM_AUDIO_VOLUME, (t), (v)) )
#define CXineAudioComp(s, t, v) ( CXineSetRangeValue((s), XINE_PARAM_AUDIO_COMPR_LEVEL, (t), (v)) )
#define CXineBrightness(s, t, v) ( CXineSetRangeValue((s), XINE_PARAM_VO_BRIGHTNESS, (t), (v)) )
#define CXineContrast(s, t, v) ( CXineSetRangeValue((s), XINE_PARAM_VO_CONTRAST, (t), (v)) )


void CXineEventSend(TConfig *Config, int type);
void CXineMute(xine_stream_t *stream, int Setting);
void CXineSwitchAudioChannel(xine_stream_t *stream, int skip);
void CXineSetRangeValue(xine_stream_t *stream, int Type, int SetType, int Value);
int CXinePause(TConfig *Config);
int CXinePlayStream(TConfig *Config, const char *url);
int CXineSelectStream(TConfig *Config, int Which);
void CXineSetPos(xine_stream_t *stream, int skip);
int CXineIsPaused(TConfig *Config);
void CXinePlaybackEnd();

#endif
