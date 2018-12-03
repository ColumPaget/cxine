
#ifndef CXINE_PLAYLISTFILES_H
#define CXINE_PLAYLISTFILES_H

#include "common.h"

int IsPlaylist(const char *MRL);
int PLSPlaylistLoad(TStringList *List, const char *MRL);
int M3UPlaylistLoad(TStringList *List, const char *MRL);
int XSPFPlaylistLoad(TStringList *List, const char *MRL);
int PlaylistLoad(TStringList *List, const char *MRL);

#endif
