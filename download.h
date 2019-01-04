#ifndef CXINE_DOWNLOAD_H
#define CXINE_DOWNLOAD_H

#include "common.h"

void DownloadCleanCacheDir();
void DownloadAddHelper(const char *Protocols, const char *Helper);
void DownloadAddHelpers(const char *Helpers);
int DownloadLaunch(const char *MRL);
int DownloadDone(char **MRL, const char *ID);
size_t DownloadTransferred(const char *MRL);

#endif
