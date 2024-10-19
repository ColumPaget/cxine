//this module relates to tracklistings within a single media file,
//so instances where tracks are concatanated into a 'mix tape'
//it simply reads timestamps and names from a text file, and 
//displays the names when a timestamp is reached

#ifndef CXINE_TRACKLIST_H
#define CXINE_TRACKLIST_H

#include "common.h"

int TrackListLoad(const char *MRL);
int TrackListCheck(const char *Timestamp, char **TrackName);

#endif
