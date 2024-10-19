#ifndef CXINE_NOWPLAYING_H
#define CXINE_NOWPLAYING_H

#include "common.h"

char *CXineGetCurrTrack(char *RetStr);
void NowPlayingNewFile(TConfig *Config);
void NowPlayingNewTitle(TConfig *Config);

#endif

