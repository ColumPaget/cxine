#ifndef CXINE_BOOKMARKS_H
#define CXINE_BOOKMARKS_H

#include "common.h"

int LoadBookmark(const char *url);
void SaveBookmark(const char *url, xine_stream_t *stream);

#endif
