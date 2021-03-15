/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "download.h"
#include "playlist.h"
#include "playback_control.h"
#include "playlist_files.h"
#include "X11.h"
#include <fcntl.h>
#include <sys/file.h>
#include <glob.h>

TStringList *Helpers=NULL;
xine_list_t *Downloads=NULL;

typedef struct
{
    char *MRL;
    char *Helpers;
    pid_t Pid;
    int Flags;
} TDownload;


static TDownload *DownloadsFind(const char *MRL)
{
    xine_list_iterator_t curr;
    TDownload *Download;

    if (Downloads)
    {
        curr=xine_list_front(Downloads);
        while (curr)
        {
            Download=(TDownload *) xine_list_get_value(Downloads, curr);
            if (strcmp(Download->MRL, MRL)==0) return(Download);

            curr=xine_list_next(Downloads, curr);
        }
    }


    return(NULL);
}


static void DownloadsDelete(TDownload *Download)
{
    xine_list_iterator_t itr;

    itr=xine_list_find(Downloads, Download);
    if (itr) xine_list_remove(Downloads, itr);

    destroy(Download->MRL);
    destroy(Download->Helpers);
    destroy(Download);
}


void DownloadCleanCacheDir()
{
    const char *ptr;
    char *Tempstr=NULL;
    struct stat Stat;
    glob_t Glob;
    time_t Then;
    int i;

    time(&Now);

    Tempstr=rstrcpy(Tempstr, Config->cache_dir);
    Tempstr=rstrcat(Tempstr, "/*");

    glob(Tempstr, 0, 0, &Glob);
    for (i=0; i < Glob.gl_pathc; i++)
    {
        stat(Glob.gl_pathv[i], &Stat);
        Then=Stat.st_atime;
        if (Stat.st_mtime > Then) Then=Stat.st_mtime;

        if ((Now - Then) > Config->cache_maxage)
        {
            if (Config->flags & CONFIG_DEBUG) printf("Unlink: %s\n", Glob.gl_pathv[i]);
            unlink(Glob.gl_pathv[i]);
        }
    }

    destroy(Tempstr);
}


char * DownloadFormatPath(char *RetStr, const char *MRL)
{
    const char *ptr;
    char *Tempstr=NULL;

    RetStr=rstrcpy(RetStr, Config->cache_dir);
    RetStr=rstrcat(RetStr, "/");
    Tempstr=realloc(Tempstr, 255);
    snprintf(Tempstr, 255, "%x", fnv_hash(MRL, INT_MAX));
    RetStr=rstrcat(RetStr, Tempstr);
    RetStr=rstrcat(RetStr, "-");

//to extract the path we fist want to avoid any arguments in the URL
    rstrtok(MRL, "?", &Tempstr);
    ptr=strrchr(Tempstr, '/');
    if (ptr) RetStr=rstrcat(RetStr, ptr+1);
    else RetStr=rstrcat(RetStr, Tempstr);

    destroy(Tempstr);

    return(RetStr);
}

void DownloadAddHelper(const char *Protocols, const char *Helper)
{
    char *Tempstr=NULL;

    if (! Helpers) Helpers=StringListCreate(0, NULL);

    Tempstr=rstrcpy(Tempstr, Protocols);
    Tempstr=rstrcat(Tempstr, " ");
    Tempstr=rstrcat(Tempstr, Helper);
    StringListAdd(Helpers, Tempstr);
    destroy(Tempstr);
}

void DownloadAddHelpers(const char *Helpers)
{
    const char *ptr;
    char *Tempstr=NULL, *Protos=NULL, *Helper=NULL;

    ptr=rstrtok(Helpers, ";", &Tempstr);
    while (ptr)
    {
        Helper=rstrcpy(Helper, rstrtok(Tempstr, ":", &Protos));
        DownloadAddHelper(Protos, Helper);
        ptr=rstrtok(ptr, ";", &Tempstr);
    }

    destroy(Tempstr);
    destroy(Helper);
    destroy(Protos);
}

static char *DownloadFormatHelperCommand(char *RetStr, const char *Cmd, const char *MRL)
{
    char *Proto=NULL, *Host=NULL, *Port=NULL, *Path=NULL;
    const char *ptr;

    RetStr=rstrcpy(RetStr, "");
    ParseURL(MRL, &Proto, &Host, &Port, &Path);
    for (ptr=Cmd; *ptr !='\0'; ptr++)
    {
        if (*ptr=='$')
        {
            ptr++;
            if (strncmp(ptr, "(mrl)", 5)==0)
            {
                RetStr=rstrcat(RetStr, MRL);
                ptr+=4;
            }
            else if (strncmp(ptr, "(host)", 6)==0)
            {
                RetStr=rstrcat(RetStr, Host);
                ptr+=5;
            }
            else if (strncmp(ptr, "(port)", 6)==0)
            {
                RetStr=rstrcat(RetStr, Port);
                ptr+=5;
            }
            else if (strncmp(ptr, "(path)", 6)==0)
            {
                RetStr=rstrcat(RetStr, Path);
                ptr+=5;
            }
            else if (strncmp(ptr, "(proto)", 7)==0)
            {
                RetStr=rstrcat(RetStr, Proto);
                ptr+=6;
            }
        }
        else RetStr=rstrlcat(RetStr, ptr, 1);
    }

    destroy(Proto);
    destroy(Host);
    destroy(Port);
    destroy(Path);

    return(RetStr);
}


static pid_t DownloadLaunchHelper(TDownload *Download)
{
    char *Cmd=NULL, *ProgName=NULL, *Tempstr=NULL, *Path=NULL;
    const char *ptr;
    pid_t pid=0;
    int fd=-1;

    Path=DownloadFormatPath(Path, Download->MRL);

    //get next helper. This will gradually consume all the helpers booked
    //against the download
    ptr=rstrtok(Download->Helpers, ";", &Cmd);
    if (! StrLen(ptr))
    {
        destroy(Cmd);
        return(-1);
    }

    Download->Helpers=memmove(Download->Helpers, ptr, StrLen(ptr) +1);

    fd=open(Path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if ( (fd > -1) && (flock(fd, LOCK_EX | LOCK_NB)==0) )
    {
        pid=fork();
        if (pid == 0)
        {
            //things can get in a right pickle if child processes have access to X11
            X11Disassociate(Config->X11Out);

            Tempstr=DownloadFormatHelperCommand(Tempstr, Cmd, Download->MRL);

            ptr=rstrtok(Tempstr, " 	", &ProgName);
            Cmd=PathSearch(Cmd, ProgName, getenv("PATH"));
            if (StrLen(Cmd))
            {
                Cmd=rstrcat(Cmd, " ");
                Cmd=rstrcat(Cmd, ptr);

                // set stdout to be fd, then we can close fd
                // DO NOT PRINTF ANYTHING AFTER HERE. It will go in the file
                close(1);
                dup(fd);
                close(fd);

                //forked program will effectively end here as we exec the helper
                Exec(Cmd);
            }
            else if (Config->flags & CONFIG_DEBUG) printf("Can't find program: '%s'",Cmd);

            //we can get here if we couldn't find the command to run
            _exit(0);
        }
        else
        {
            Download->Pid=pid;
            usleep(100);
        }
    }

    if (fd > -1) close(fd);

    destroy(ProgName);
    destroy(Tempstr);
    destroy(Path);
    destroy(Cmd);

    return(pid);
}



static char *DownloadConsiderHelper(char *SelectedHelpers, const char *HelperInfo, const char *MRL)
{
    const char *ptr;
    char *Protocols=NULL, *Cmd=NULL, *Proto=NULL;

    ptr=rstrtok(HelperInfo, " ", &Protocols);
    Cmd=rstrcpy(Cmd, ptr);

    ptr=rstrtok(Protocols, ",", &Proto);
    while (ptr)
    {
        if (Config->flags & CONFIG_DEBUG) printf("Consider helper: '%s' (%s) for '%s'...",Cmd, Protocols, MRL);

        if (strncasecmp(MRL, Proto, strlen(Proto))==0)
        {
            SelectedHelpers=rstrcat(SelectedHelpers, Cmd);
            SelectedHelpers=rstrcat(SelectedHelpers, ";");

            if (Config->flags & CONFIG_DEBUG) printf("YES.\n");
        }
        else if (Config->flags & CONFIG_DEBUG) printf("no.\n");

        ptr=rstrtok(ptr, ",", &Proto);
    }


    destroy(Protocols);
    destroy(Proto);
    destroy(Cmd);

    return(SelectedHelpers);
}



//start a download
static int DownloadLaunch(const char *MRL, int Flags)
{
    char *SelectedHelpers=NULL;
    TDownload *download;
    const char *ptr;
    int result=DOWNLOAD_DONE; //failure condition is 'download is done'

    ptr=StringListFirst(Helpers);
    while (ptr)
    {
        SelectedHelpers=DownloadConsiderHelper(SelectedHelpers, ptr, MRL);
        ptr=StringListNext(Helpers);
    }
    if (StrLen(SelectedHelpers))
    {
        download=(TDownload *) calloc(1, sizeof(TDownload));
        download->Helpers=rstrcpy(download->Helpers, SelectedHelpers);
        download->MRL=rstrcpy(download->MRL, MRL);
        download->Flags=Flags;
        if (! Downloads) Downloads=xine_list_new();
        xine_list_push_back(Downloads, download);

        if (DownloadLaunchHelper(download) > 0) result=DOWNLOAD_ACTIVE;
    }

    destroy(SelectedHelpers);
    return(result);
}


//how much transferred so far
size_t DownloadTransferred(const char *MRL)
{
    char *Tempstr=NULL;
    struct stat Stat;
    size_t size=0;

    Tempstr=DownloadFormatPath(Tempstr, MRL);
    if (stat(Tempstr, &Stat) > -1) size=Stat.st_size;

    destroy(Tempstr);

    return(size);
}



//check if a file in the cache has really downloaded and is > 0 bytes in size
int DownloadPostProcessFile(const char *Path, const char *MRL)
{
    int fd;
    int size=0;
    struct flock Lock;
    struct stat Stat;
    int result=DOWNLOAD_NONE, Play=FALSE;
    TDownload *Download;

    //if the file isn't locked, then it's finished downloading
    fd=open(Path, O_RDWR);
    if (fd > -1)
    {
        if (flock(fd, LOCK_EX | LOCK_NB)==0)
        {
            fstat(fd, &Stat);
            //close early so it's not locked by our flock check
            close(fd);


            //download is complete even if size is zero
            result=DOWNLOAD_DONE;
            Download=DownloadsFind(MRL);

            if (Stat.st_size > 0)
            {
								//At this point we have downloaded someting, so we can delete the 
								//download entry from the download queue and mark the thing ready to play
                if (Download)
                {
                    if (Download->Flags & DOWNLOAD_PLAY) Play=TRUE;
                    DownloadsDelete(Download);
                }

                if (IsPlaylist(Path))
                {
								// don't play playlists
										Play=FALSE;
                    if (PlaylistFileNeedsUpdate(MRL, Path))
                    {
                        unlink(Path);
                        result=DOWNLOAD_NONE;
                    }
                    else
                    {
                        PlaylistLoadFromURL(MRL, Path);
                    }
                }


                if (Play) CXinePlayStream(Config, MRL);
            }
            else
            {
                unlink(Path);
                //if file was zero size try download again with the next helper
                if (Download)
                {
                    if (DownloadLaunchHelper(Download) > 0) result=DOWNLOAD_NONE;
                }
                else result=DOWNLOAD_NONE;
            }
        }
        else
        {
            result=DOWNLOAD_ACTIVE;
	    close(fd);
        }
    }

    return(result);
}


//if the download done or not? If it is, return TRUE so cxine knows the download is ready for processing
int DownloadState(char **MRL, const char *ID)
{
    const char *ptr;
    char *CachePath=NULL;
    int fd, result=DOWNLOAD_NONE;

// if MRL starts with a '/' then it's a file path, not a download, and we declare it done
    if (**MRL=='/') return(DOWNLOAD_DONE);

//if MRL exists on disk (thus isn't a URL but a file path), then declare 'download done'
    if (access(*MRL, F_OK) ==0) return(DOWNLOAD_DONE);

//if we're in streaming mode and it's not a playlist, then declare 'download done'
    if ( (Config->flags & CONFIG_STREAM) && (! IsPlaylist(*MRL)) ) return(DOWNLOAD_DONE);

    MkDirPath(Config->cache_dir);

// format cache path
    CachePath=DownloadFormatPath(CachePath, *MRL);

// if Path already in cache then it's downloading already
    if (access(CachePath, F_OK)==0)
    {
        result=DownloadPostProcessFile(CachePath, *MRL);
        if (result==DOWNLOAD_DONE) *MRL=rstrcpy(*MRL, CachePath);
    }

    destroy(CachePath);

    return(result);
}



int DownloadProcess(char **MRL, const char *ID, int Flags)
{
    int result;

    result=DownloadState(MRL, ID);
    if (Flags)
    {
        // if download isn't done, then launch a new download
        if (! result) result=DownloadLaunch(*MRL, Flags);
    }

    return(result);
}
