#ifndef CXINE_AUDIO_DRIVERS_H
#define CXINE_AUDIO_DRIVERS_H

#include "common.h"

xine_audio_port_t *CXineOpenAudioDriver(const char *Spec);
xine_audio_port_t *CXineOpenAudio();
void CXineCloseAudio();
xine_audio_port_t *CXineCycleAudio();

#endif
