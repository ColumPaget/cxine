#include "common.h"

TConfig *Config=NULL;
int  running = 0;

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
if (ptr) return(ptr+1);
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
	if (*ptr=='"') ptr=advanceto(ptr, '"');
	if (*ptr=='\\') 
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



TStringList *StringListCreate(int argc, char **argv)
{
TStringList *sl;

sl=(TStringList *) calloc(1, sizeof(TStringList));
sl->list=calloc(argc, sizeof(char *));
for (sl->size=0; sl->size < argc; sl->size++)
{
sl->list[sl->size]=strdup(argv[sl->size]);
}

return(sl);
}

int StringListAdd(TStringList *sl, const char *str)
{
sl->list=realloc(sl->list, (sl->size+1) * sizeof(char *));
sl->list[sl->size]=strdup(str);
sl->size++;
return(sl->size -1);
}

int StringListSplit(TStringList *sl, const char *str, const char *separators)
{
char *Token=NULL;
const char *ptr;

ptr=rstrtok(str, separators, &Token);
while (ptr)
{
StringListAdd(sl, Token);
ptr=rstrtok(ptr, separators, &Token);
}

destroy(Token);
return(sl->size);
}

const char *StringListGet(TStringList *sl, unsigned int pos)
{
if (pos >= sl->size) return(NULL);
sl->next=pos+1;
return(sl->list[pos]);
}

void StringListSet(TStringList *sl, unsigned int pos, const char *Str)
{
if (pos >= sl->size) return;
sl->list[pos]=rstrcpy(sl->list[pos], Str);
}


const char *StringListCurr(TStringList *sl)
{
if (! sl) return(NULL);
if (sl->size==0) return(NULL);
if (sl->next > sl->size) return(NULL);
return(sl->list[sl->next-1]);
}

const char *StringListPrev(TStringList *sl)
{
if (! sl) return(NULL);
if (sl->next ==1) return(NULL);
sl->next--;
return(sl->list[sl->next-1]);
}


const char *StringListNext(TStringList *sl)
{
if (! sl) return(NULL);
if (sl->next >= sl->size) return(NULL);
sl->next++;
return(sl->list[sl->next-1]);
}


void StringListDestroy(TStringList *sl)
{
int i;

for (i=0; i < sl->size; i++)
{
	if (sl->list[i]) free(sl->list[i]);
}

free(sl->list);
free(sl);
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


void Exec(const char *CmdLine)
{
const char *ptr;
char *Token=NULL, **Args=NULL;
int count=0;

ptr=rstrtok(CmdLine, " ", &Token);
while (ptr)
{
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
