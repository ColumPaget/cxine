#include "download_osd.h"

static CXineOSD *DownloadOSD=NULL;



void DownloadOSDHide()
{
            if (DownloadOSD)
            {
                OSDDestroy(DownloadOSD);
                DownloadOSD=NULL;
            }
}



void DownloadOSDDisplay()
{
    char *Tempstr=NULL, *Title=NULL, *URL=NULL, *Text=NULL;
    static char *PrevSizeStr=NULL;
    const char *ptr;

		//don't display it if anything else is currently being shown
		if (Config->state & (STATE_PLAYLIST_DISPLAYED | STATE_LOADFILES_DISPLAYED | STATE_INFO_DISPLAYED)) 
		{
			DownloadOSDHide();
			return;
		}

    Title=PlaylistCurrTitle(Title);

    ptr=StringListCurr(Config->playlist);
    if (ptr)
    {
        rstrtok(ptr, " ", &URL);

        Tempstr=(char *) calloc(1, 256);
        LongFormatMetric(Tempstr, 255, (unsigned long) DownloadTransferred(URL));

        //don't bother to update everything if the value of the download display
        //hasn't changed
        if ( (! PrevSizeStr) || (strcmp(Tempstr, PrevSizeStr) !=0) )
        {
            PrevSizeStr=rstrcpy(PrevSizeStr, Tempstr);

            Text=rstrcpy(Text, "Downloading: ");
            Text=rstrcat(Text, Title);
            Text=rstrcat(Text, "\n");

            Text=rstrcat(Text, Tempstr);
            Text=rstrcat(Text, " bytes received\n");

            if (! DownloadOSD) DownloadOSD=OSDMessage(10, 50, Text);
            else DownloadOSD->Contents=rstrcpy(DownloadOSD->Contents, Text);
            OSDUpdateSingle(DownloadOSD, TRUE);

            Tempstr=rstrcat(Tempstr, " bytes of: ");
            Tempstr=rstrcat(Tempstr, Title);
            X11WindowSetTitle(Config->X11Out, Tempstr, "cxine");
        }
    }

    destroy(Tempstr);
    destroy(Title);
    destroy(Text);
    destroy(URL);
}



