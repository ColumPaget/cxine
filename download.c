#include "download.h"
#include "playlist_files.h"
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
} TDownload;


TDownload *DownloadsFind(const char *MRL)
{
xine_list_iterator_t curr;
TDownload *Download;

curr=xine_list_front(Downloads);
while (curr)
{
Download=(TDownload *) xine_list_get_value(Downloads, curr);
if (strcmp(Download->MRL, MRL)==0) return(Download);

curr=xine_list_next(Downloads, curr);
}


return(NULL);
}


void DownloadsDelete(TDownload *Download)
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
time_t Now, Then;
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
int fd;

ptr=rstrtok(Download->Helpers, ";", &Cmd);
printf("HELP: %s\n",Cmd);
Download->Helpers=memmove(Download->Helpers, ptr, StrLen(ptr) +1);

Tempstr=DownloadFormatHelperCommand(Tempstr, Cmd, Download->MRL);

ptr=rstrtok(Tempstr, " 	", &ProgName);
Cmd=PathSearch(Cmd, ProgName, getenv("PATH"));
if (StrLen(Cmd)) 
{
Cmd=rstrcat(Cmd, " ");
Cmd=rstrcat(Cmd, ptr);

Path=DownloadFormatPath(Path, Download->MRL);
Tempstr=rstrcpy(Tempstr, Path);
Tempstr=rstrcat(Tempstr, ".init");
fd=open(Tempstr, O_WRONLY | O_CREAT, 0600);
if (fd > -1)
{
	pid=fork();
	if (pid==0)
	{
		close(1);
		dup(fd);
		close(fd);
		flock(1, LOCK_EX);
		rename(Tempstr, Path);
		Exec(Cmd);
	}
	else Download->Pid=pid;
	close(fd);
}

}
else if (Config->flags & CONFIG_DEBUG) printf("Can't find program: '%s'",Cmd);


destroy(ProgName);
destroy(Tempstr);
destroy(Path);
destroy(Cmd);

return(pid);
}



char *DownloadConsiderHelper(char *SelectedHelpers, const char *HelperInfo, const char *MRL)
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




int DownloadLaunch(const char *MRL)
{
char *SelectedHelpers=NULL;
TDownload *download;
const char *ptr;
int result=FALSE;

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
if (! Downloads) Downloads=xine_list_new();
xine_list_push_back(Downloads, download);

if (DownloadLaunchHelper(download) > 0) result=TRUE;
}

destroy(SelectedHelpers);
return(result);
}


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



int DownloadDone(char **MRL)
{
const char *ptr;
char *FName=NULL;
int fd, result=FALSE;
struct flock Lock;
TDownload *Download;

// if MRL starts with a '/' then it's a file path, not a download, and we declare it done
if (**MRL=='/') return(TRUE);

//if MRL exists on disk (thus isn't a URL but a file path), then declare 'download done'
if (access(*MRL, F_OK) ==0) return(TRUE);

//if we're in streaming mode and it's not a playlist, then declare 'download done'
if ( (Config->flags & CONFIG_STREAM) && (! IsPlaylist(*MRL)) ) return(TRUE);

MkDirPath(Config->cache_dir);

// format cache path
FName=DownloadFormatPath(FName, *MRL);

// if Path already in cache then it's downloading already
if (access(FName, F_OK) !=0)
{
	destroy(FName);
	//if we couldn't find a helper to handle this path, then declare download done
	if (! DownloadLaunch(*MRL)) return(TRUE);
	return(FALSE);
}

//if the file isn't locked, then it's finished downloading
fd=open(FName, O_RDWR);
if (fd > -1)
{
if (flock(fd, LOCK_EX | LOCK_NB)==0) result=TRUE;
close(fd);
}


//if it's finished downloading, then check it's size, if it's zero size then we need to 
//restart download with another helper
if (result)
{
	Download=DownloadsFind(*MRL);
	if (DownloadTransferred(*MRL)==0)
	{
		if (Download && (DownloadLaunchHelper(Download) > 0)) result=FALSE;
	}
	else	*MRL=rstrcpy(*MRL, FName);

	if (result && Download) DownloadsDelete(Download);
}


destroy(FName);

return(result);
}



