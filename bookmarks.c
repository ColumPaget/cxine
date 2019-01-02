#include "bookmarks.h"

#define MAX_LINE 1024

int LoadBookmark(const char *url)
{
    FILE *inf;
    char *Line=NULL, *Tempstr=NULL;
    const char *ptr;
    int RetVal=0;

    Tempstr=rstrcpy(Tempstr, xine_get_homedir());
    Tempstr=rstrcat(Tempstr, "/.xine/cxine.bookmarks");
    inf=fopen(Tempstr, "r");
    if (inf)
    {
        Line=calloc(MAX_LINE, sizeof(char));
        while (fgets(Line, MAX_LINE, inf))
        {
            Line=xine_chomp(Line);
            if (StrLen(Line))
            {
                ptr=rstrtok(Line, " 	", &Tempstr);
                if (strcmp(ptr, url) ==0) RetVal=atoi(Tempstr);
            }
        }
        fclose(inf);
    }

    destroy(Tempstr);
    destroy(Line);
    return(RetVal);
}


void SaveBookmark(const char *url, xine_stream_t *stream)
{
    FILE *outf, *inf;
    char *Line=NULL, *InPath=NULL, *OutPath=NULL, *Tempstr=NULL;
    const char *ptr;
    int val, pos, size;

    if (! url) return;
    OutPath=rstrcpy(OutPath, xine_get_homedir());
    OutPath=rstrcat(OutPath, "/.xine/cxine.bookmarks+");
    outf=fopen(OutPath, "w");
    if (outf)
    {
        InPath=rstrcpy(InPath, xine_get_homedir());
        InPath=rstrcat(InPath, "/.xine/cxine.bookmarks");
        inf=fopen(InPath, "r");
        if (inf)
        {
            Line=calloc(MAX_LINE, sizeof(char));
            while (fgets(Line, MAX_LINE, inf))
            {
                Line=xine_chomp(Line);
                if (StrLen(Line))
                {
                    ptr=rstrtok(Line, " 	", &Tempstr);
                    if (ptr && strcmp(ptr, url) !=0) fprintf(outf, "%s\n", Line);
                }
            }
            fclose(inf);
        }
        xine_get_pos_length (stream, &val, &pos, &size);
        if ((size - pos) > 20000) fprintf(outf, "%d %s\n", pos, url);
        fclose(outf);
        rename(OutPath, InPath);
    }

    free(Tempstr);
    free(OutPath);
    free(InPath);
    free(Line);
}
