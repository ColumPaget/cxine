#ifndef CXINE_PLAYLIST_H
#define CXINE_PLAYLIST_H

#include "common.h"

void PlaylistShuffle();
void PlaylistParseEntry(const char *info, char **URL, char **ID, char **Title);
char *PlaylistFormatEntry(char *RetStr, const char *URL, const char *ID, const char *Title);
int PlaylistMoveItem(TStringList *playlist, int pos, int move);
void PlaylistAdd(TStringList *playlist, const char *URL, const char *ID, const char *Title);
TStringList *PlaylistExpandCurr(TStringList *playlist, const char *URL, const char *LocalPath);
void PlaylistLoadFromURL(const char *URL, const char *LocalPath);
char *PlaylistCurrTitle(char *RetStr);
void PlaylistInit(TStringList *playlist);

#endif

