#include "common.h"
#include "string_list.h"

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
    if (str)
    {
        sl->list=realloc(sl->list, (sl->size+1) * sizeof(char *));
        sl->list[sl->size]=strdup(str);
        sl->size++;
    }
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
    if (sl->next==0) return(NULL);
    if (sl->next > sl->size) return(NULL);
    return(sl->list[sl->next-1]);
}

const char *StringListPrev(TStringList *sl)
{
    if (! sl) return(NULL);
    if (sl->next ==0) return(NULL);
    if (sl->next > 1) sl->next--;
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

