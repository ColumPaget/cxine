
#ifndef CXINE_PLAYLISTFILES_H
#define CXINE_PLAYLISTFILES_H

#include "common.h"

#define PLAYLIST_FILE_NOT  0 
#define PLAYLIST_FILE_M3U  1 
#define PLAYLIST_FILE_PLS  2 
#define PLAYLIST_FILE_XSPF 3
#define PLAYLIST_FILE_ASX  4
#define PLAYLIST_FILE_RSS  5
#define PLAYLIST_FILE_XML  9999

int IsPlaylist(const char *MRL);
int PlaylistFileNeedsUpdate(const char *MRL, const char *Path);
int PLSPlaylistLoad(TStringList *List, const char *MRL);
int M3UPlaylistLoad(TStringList *List, const char *MRL);
int XSPFPlaylistLoad(TStringList *List, const char *MRL);
int PlaylistLoad(TStringList *List, const char *MRL);

#endif
