/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "bookmarks.h"

#define MAX_LINE 1024


void SaveBookmark(const char *url, xine_stream_t *stream)
{
    FILE *outf, *inf;
    char *Line=NULL, *InPath=NULL, *OutPath=NULL, *Tempstr=NULL;
    const char *ptr;
    int val, pos, size;

		//if no url get the hell out of here!
    if (! url) return;

		//if it's not currently playing then we either got to the end, or haven't started
		//either way there's no point saving a bookmark
    if (! (Config->state & STATE_PLAYING)) return;

		//if it's not a seekable stream, don't save a bookmark
    if (stream && (! xine_get_stream_info(Config->stream, XINE_STREAM_INFO_SEEKABLE))) return;

    OutPath=rstrcpy(OutPath, xine_get_homedir());
    OutPath=rstrcat(OutPath, "/.cxine/cxine.bookmarks+");
    outf=fopen(OutPath, "w");

		//first copy all bookmarks that *aren't* our target bookmark. This ensures there
		//should only be one entry for a given url
    if (outf)
    {
        InPath=rstrcpy(InPath, xine_get_homedir());
        InPath=rstrcat(InPath, "/.cxine/cxine.bookmarks");
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
	
				//if stream is NULL we don't write a bookmark to the file. This can be used to delete
				//an existing bookmark for a stream		
				if (stream)
				{
        xine_get_pos_length (stream, &val, &pos, &size);
        if ((size - pos) > 20000) fprintf(outf, "%d %s\n", pos, url);
				}

        fclose(outf);
        rename(OutPath, InPath);
    }

    free(Tempstr);
    free(OutPath);
    free(InPath);
    free(Line);
}


int LoadBookmark(const char *url)
{
    FILE *inf;
    char *Line=NULL, *Tempstr=NULL;
    const char *ptr;
    int RetVal=0;

    Tempstr=rstrcpy(Tempstr, xine_get_homedir());
    Tempstr=rstrcat(Tempstr, "/.cxine/cxine.bookmarks");
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

		//delete bookmark now we've used it
		SaveBookmark(url, NULL);

    destroy(Tempstr);
    destroy(Line);
    return(RetVal);
}


