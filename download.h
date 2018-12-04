#ifndef CXINE_DOWNLOAD_H
#define CXINE_DOWNLOAD_H

#include "common.h"

void DownloadCleanCacheDir();
int DownloadAddHelper(const char *Protocols, const char *Helper);
int DownloadLaunch(const char *MRL);
int DownloadDone(char **MRL);

#endif
