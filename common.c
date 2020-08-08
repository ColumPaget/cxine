/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "common.h"
#include <utime.h>

TConfig *Config=NULL;
int  running = 0;
time_t Now;

#define FNV_INIT_VAL 2166136261
unsigned int fnv_hash(unsigned const char *key, int NoOfItems)
{
    unsigned const char *p;
    unsigned int h = FNV_INIT_VAL;

    for (p=key; *p !='\0' ; p++ ) h = ( h * 16777619 ) ^ *p;

    return(h % NoOfItems);
}

const char *cbasename(const char *Path)
{
    char *ptr;

    if (! Path) return("");
    ptr=strrchr(Path, '/');
    if (ptr) 
		{
			if (*(ptr+1)=='\0')
			{
				ptr--;
				while ((ptr > Path) && (*ptr !='/')) ptr--;
			}
			if (*ptr=='/') return(ptr+1);
			return(ptr);
		}
    return(Path);
}

char *rstrlcat(char *Dest, const char *Src, int SrcLen)
{
    int dlen, len;

    dlen=StrLen(Dest);
    len=dlen + SrcLen;

    Dest=realloc(Dest, len + 1);
    strncpy(Dest+dlen, Src, SrcLen);
    Dest[len]='\0';

    return(Dest);
}

char *rstrcat(char *Dest, const char *Src)
{
    return(rstrlcat(Dest, Src, StrLen(Src)));
}


char *rstrlcpy(char *Dest, const char *Src, int SrcLen)
{
    if (Dest) *Dest=0;
    return(rstrlcat(Dest, Src, SrcLen));
}

char *rstrcpy(char *Dest, const char *Src)
{
    if (Dest) *Dest=0;
    return(rstrcat(Dest,Src));
}

void strrep(char *Str, char c1, char c2)
{
    char *ptr;

    for (ptr=Str; *ptr != '\0'; ptr++)
    {
        if (*ptr==c1) *ptr=c2;
    }

}

const char *advanceto(const char *ptr, char term)
{
    for (; *ptr!=term; ptr++)
    {
        if (*ptr=='\\') ptr++;
        if (*ptr=='\0') break;
    }

    return(ptr);
}


const char *rstrtok(const char *Str, const char *Separators, char **Token)
{
    const char *ptr;

    if ((! Str) || (*Str=='\0')) return(NULL);

    for (ptr=Str; *ptr!='\0'; ptr++)
    {
        if (*ptr=='"') ptr=advanceto(ptr+1, '"');
        else if (*ptr=='\'') ptr=advanceto(ptr+1, '\'');
        else if (*ptr=='\\')
        {
            ptr++;
            if (*ptr=='\0') break;
        }
        else if (strchr(Separators, *ptr)) break;
    }

    *Token=rstrlcpy(*Token, Str, ptr-Str);

    if (*ptr !='\0') ptr++;
    return(ptr);
}


char *rstrquot(char *RetStr, const char *Str, const char *QuoteChars)
{
    const char *ptr;

    RetStr=rstrcpy(RetStr, "");
    ptr=Str;
    while (ptr && (*ptr !='\0'))
    {
        if (strchr(QuoteChars, *ptr)) RetStr=rstrcat(RetStr, "\\");
        RetStr=rstrlcat(RetStr, ptr, 1);
        ptr++;
    }

    return(RetStr);
}

char *rstrunquot(char *RetStr, const char *Str)
{
    const char *ptr;

    RetStr=rstrcpy(RetStr, "");
    ptr=Str;
    while (ptr && (*ptr !='\0'))
    {
        if (*ptr=='\\') ptr++;
        RetStr=rstrlcat(RetStr, ptr, 1);
        ptr++;
    }

    return(RetStr);
}


void StripQuotes(char *Str)
{
int len;

if ( (*Str=='\'') || (*Str=='\"') )
{
	len=strlen(Str);
	Str[len-1]='\0';
	memmove(Str, Str+1, len);
}

}


void TouchFile(const char *Path)
{
    struct utimbuf times;
    struct stat Stat;

    if (stat(Path, &Stat)==0)
    {
        times.actime=time(NULL);
        times.modtime=Stat.st_mtime;
        utime(Path, &times);
    }

}



char *PathSearch(char *RetStr, const char *FileName, const char *Path)
{
    char *Dir=NULL;
    const char *ptr;

    if (*FileName=='/') return(rstrcpy(RetStr, FileName));

    RetStr=rstrcpy(RetStr, "");
    ptr=rstrtok(Path, ":", &Dir);
    while (ptr)
    {
        RetStr=rstrcpy(RetStr, Dir);
        RetStr=rstrcat(RetStr, "/");
        RetStr=rstrcat(RetStr, FileName);

        if (access(RetStr, F_OK)==0) break;

        RetStr=rstrcpy(RetStr, "");
        ptr=rstrtok(ptr, ":", &Dir);
    }

    destroy(Dir);
    return(RetStr);
}



int ParseURL(const char *URL, char **Proto, char **Host, char **Port, char **Path)
{
    const char *ptr;
    char *Tempstr=NULL;

    ptr=strchr(URL, ':');
    if (! ptr) return(FALSE);

    ptr=rstrtok(URL, ":", Proto);
    while (*ptr=='/') ptr++;
    ptr=rstrtok(ptr, "/", &Tempstr);
    ptr--; /*to get to '/' */
    *Path=rstrcpy(*Path, ptr);

    ptr=rstrtok(Tempstr, ":", Host);
    *Port=rstrcpy(*Port, ptr);

    destroy(Tempstr);
    return(TRUE);
}

void MkDirPath(const char *Dir)
{
    char *Token=NULL, *Path=NULL;
    const char *ptr;

    ptr=rstrtok(Dir, "/", &Token);
    while (ptr)
    {
        Path=rstrcat(Path, Token);
        mkdir(Path, 0700);
        Path=rstrcat(Path, "/");
        ptr=rstrtok(ptr, "/", &Token);
    }

    destroy(Token);
    destroy(Path);
}


void LongFormatMetric(char *Str, int len, long value)
{
	if (value > 1000000000) snprintf(Str, len, "%0.2fG", ((float) value) / 1000000000.0);
	else if (value > 1000000) snprintf(Str, len, "%0.2fM", ((float) value) / 1000000.0);
	else if (value > 1000) snprintf(Str, len, "%0.2fk", ((float) value) / 1000000.0);
	else snprintf(Str, len, "%lu", value);
}


void Exec(const char *CmdLine)
{
    const char *ptr;
    char *Token=NULL, **Args=NULL;
    int count=0;

    ptr=rstrtok(CmdLine, " ", &Token);
    while (ptr)
    {
				StripQuotes(Token);
        Args=realloc(Args, (count+10) * sizeof(char *));
        Args[count]=strdup(Token);
        count++;
        ptr=rstrtok(ptr, " ", &Token);
    }
    Args[count]=NULL;

    destroy(Token);
    execv(Args[0], Args);

//should never reach here
    _exit(1);
}
