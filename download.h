#ifndef CXINE_DOWNLOAD_H
#define CXINE_DOWNLOAD_H

#include "common.h"

#define DOWNLOAD_NONE   0
#define DOWNLOAD_DONE   1
#define DOWNLOAD_ACTIVE 2

#define DOWNLOAD_LAUNCH 1
#define DOWNLOAD_PLAY   2

void DownloadCleanCacheDir();
void DownloadAddHelper(const char *Protocols, const char *Helper);
void DownloadAddHelpers(const char *Helpers);
int DownloadState(char **MRL, const char *ID);
int DownloadProcess(char **MRL, const char *ID, int DoDownload);
size_t DownloadTransferred(const char *MRL);
char *DownloadFormatHelperCommand(char *RetStr, const char *Cmd, const char *MRL);

#endif
