#include "download.h"
#include "playlist_files.h"
#include <fcntl.h>
#include <sys/file.h>
#include <glob.h>

TStringList *Helpers=NULL;

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

int DownloadAddHelper(const char *Protocols, const char *Helper)
{
char *Tempstr=NULL;

	if (! Helpers) Helpers=StringListCreate(0, NULL);

	Tempstr=rstrcpy(Tempstr, Protocols);
	Tempstr=rstrcat(Tempstr, " ");
	Tempstr=rstrcat(Tempstr, Helper);
	StringListAdd(Helpers, Tempstr);
	destroy(Tempstr);
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

static int DownloadLaunchHelper(const char *Helper, const char *MRL) 
{
char *Cmd=NULL, *ProgName=NULL, *Tempstr=NULL, *Path=NULL;
const char *ptr;
pid_t pid;
int fd, result=FALSE;

Tempstr=DownloadFormatHelperCommand(Tempstr, Helper, MRL);

ptr=rstrtok(Tempstr, " 	", &ProgName);
Cmd=PathSearch(Cmd, ProgName, getenv("PATH"));
if (StrLen(Cmd)) 
{
Cmd=rstrcat(Cmd, " ");
Cmd=rstrcat(Cmd, ptr);

Path=DownloadFormatPath(Path, MRL);
Tempstr=rstrcpy(Tempstr, Path);
Tempstr=rstrcat(Tempstr, ".init");
fd=open(Tempstr, O_WRONLY | O_CREAT, 0600);
if (fd > -1)
{
	result=TRUE;
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
 close(fd);
}

}
else if (Config->flags & CONFIG_DEBUG) printf("Can't find program: '%s'",Cmd);


destroy(ProgName);
destroy(Tempstr);
destroy(Path);
destroy(Cmd);

return(result);
}


static int DownloadLaunchTry(const char *HelperInfo, const char *MRL)
{
const char *ptr;
char *Protocols=NULL, *Cmd=NULL, *Proto=NULL;
int result=FALSE;

ptr=rstrtok(HelperInfo, " ", &Protocols);
Cmd=rstrcpy(Cmd, ptr);

ptr=rstrtok(Protocols, ",", &Proto);
while (ptr)
{
	if (Config->flags & CONFIG_DEBUG) printf("Consider helper: '%s' (%s) for '%s'...",Cmd, Protocols, MRL);
	if (strncasecmp(MRL, Proto, strlen(Proto))==0) 
	{
		if (DownloadLaunchHelper(Cmd, MRL)) 
		{
			result=TRUE;
			break;			
		}
	}
	if (Config->flags & CONFIG_DEBUG) 
	{
		if (result) printf("YES.\n");
		else printf("no.\n");
	}

	ptr=rstrtok(ptr, ",", &Proto);
}

destroy(Protocols);
destroy(Proto);
destroy(Cmd);

return(result);
}


int DownloadLaunch(const char *MRL)
{
const char *ptr;
int result=FALSE;

ptr=StringListFirst(Helpers);
while (ptr)
{
	if (DownloadLaunchTry(ptr, MRL)) 
	{
		result=TRUE;
		break;
	}

	ptr=StringListNext(Helpers);
}

return(result);
}



int DownloadDone(char **MRL)
{
const char *ptr;
char *FName=NULL;
int fd, result=FALSE;
struct flock Lock;

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
if (flock(fd, LOCK_EX | LOCK_NB)==0)
{
	*MRL=rstrcpy(*MRL, FName);
	result=TRUE;
}

close(fd);
}
destroy(FName);

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
