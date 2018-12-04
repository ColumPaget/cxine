#ifndef CXINE_PLAYLIST_H
#define CXINE_PLAYLIST_H

#include "common.h"

void PlaylistShuffle();
void PlaylistAdd(TStringList *playlist, const char *URL, const char *Title);
TStringList *PlaylistExpandCurr(TStringList *playlist, const char *URL, const char *LocalPath);
char *PlaylistCurrTitle(char *RetStr);

#endif

