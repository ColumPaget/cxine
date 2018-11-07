
#ifndef CXINE_PLAYLISTFILES_H
#define CXINE_PLAYLISTFILES_H

#include "common.h"

int IsPlaylist(const char *MRL);
void PLSPlaylistLoad(TStringList *List, const char *MRL);
void M3UPlaylistLoad(TStringList *List, const char *MRL);
void XSPFPlaylistLoad(TStringList *List, const char *MRL);
int PlaylistLoad(TStringList *List, const char *MRL);

#endif
