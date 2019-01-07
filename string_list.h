#ifndef CXINE_STRING_LIST_H
#define CXINE_STRING_LIST_H

typedef struct
{
int next;
int size;
char **list;
} TStringList;


#define StringListSize(sl) ((sl)->size)
#define StringListPos(sl) ((sl)->next -1)
TStringList *StringListCreate(int argc, char **argv);
int StringListAdd(TStringList *sl, const char *str);
int StringListSplit(TStringList *sl, const char *str, const char *separators);
const char *StringListItem(TStringList *sl, unsigned int pos);
const char *StringListGet(TStringList *sl, unsigned int pos);
void StringListSet(TStringList *sl, unsigned int pos, const char *Str);

#define StringListFirst(sl) StringListGet((sl), 0)
const char *StringListCurr(TStringList *sl);
const char *StringListPrev(TStringList *sl);
const char *StringListNext(TStringList *sl);
void StringListDestroy(TStringList *sl);

#endif
