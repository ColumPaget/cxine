#ifndef CXINE_PLAYLIST_H
#define CXINE_PLAYLIST_H

#include "common.h"

typedef struct
{
char *URL;
char *ID;
char *Title;
char *Description;
} TPlaylistItem;

void PlaylistItemDestroy(void *Item);
TPlaylistItem *PlaylistDecodeEntry(const char *info);
void PlaylistShuffle();
char *PlaylistFormatEntry(char *RetStr, const char *URL, const char *ID, const char *Title);
int PlaylistMoveItem(TStringList *playlist, int pos, int move);
void PlaylistAdd(TStringList *playlist, const char *URL, const char *ID, const char *Title);
void PlaylistParseAndAdd(TStringList *playlist, const char *Config);
TStringList *PlaylistExpandCurr(TStringList *playlist, const char *URL, const char *LocalPath);
void PlaylistLoadFromURL(const char *URL, const char *LocalPath);
char *PlaylistCurrTitle(char *RetStr);
void PlaylistInit(TStringList *playlist);

#endif

