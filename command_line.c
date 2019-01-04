#include "command_line.h"
#include "control_protocol.h"
#include "help.h"
#include "playlist.h"
#include "config.h"




static void ParseCommandLineWindowSize(char *arg, TConfig *Config)
{
    char *ptr;

    Config->width=strtol(arg, &ptr, 10);
    if (ptr) ptr++;
    Config->height=strtol(ptr, &ptr, 10);
}


static int CXineAddURL(const char *URL, const char *ID, const char *Title)
{
    int fd, result=-1;
    char *Tempstr=NULL, *Msg=NULL;

    fd=ControlPipeOpen(O_WRONLY | O_NONBLOCK);
    if ( (fd > -1) && (lockf(fd, F_LOCK, 0)==0) )
    {
        Msg=rstrcpy(Msg, "add ");
				Tempstr=PlaylistFormatEntry(Tempstr, URL, ID, Title);
        Msg=rstrcat(Msg, Tempstr);
        Msg=rstrcat(Msg, "\n");

        result=write(fd, Msg, StrLen(Msg));
        close(fd);
        Config->control_pipe=-1;
    }
    destroy(Tempstr);
    destroy(Msg);

    return(result > 0);
}


static int CXineQueueURL(const char *URL, const char *ID, const char *Title)
{
//if a cxine is already running, then send it there
    if (CXineAddURL(URL, ID, Title)) exit(0);

    PlaylistAdd(Config->playlist, URL, ID, Title);
}


void CommandLineParseCache(const char *Arg)
{
    char *ptr;
    long val;

    val=strtol(Arg, &ptr, 10);

    switch (*ptr)
    {
    case 'm':
        val *= 60;
        break;
    case 'h':
        val *= 3600;
        break;
    case 'd':
        val *= (3600 * 24);
        break;
    }

    Config->cache_maxage=val;
}


int ParseCommandLine(int argc, char *argv[], TConfig *Config)
{
    int i;
    char *Token=NULL, *Title=NULL, *ID=NULL;
    const char *ptr;

    for(i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-add")==0 ) CXineAddURL(argv[++i], ID, "");
        else if (strcmp(argv[i], "+add")==0 )
        {
            i++;
            CXineAddURL(argv[i], ID, argv[i+1]);
            i++;
        }
        else if ( strcmp(argv[i], "-queue")==0 ) CXineQueueURL(argv[++i], ID, "");
        else if ( strcmp(argv[i], "-enqueue")==0 ) CXineQueueURL(argv[++i], ID, "");
        else if ( strcmp(argv[i], "+queue")==0 )
        {
            i++;
            CXineQueueURL(argv[i], ID, argv[i+1]);
            i++;
        }
        else if ( strcmp(argv[i], "-vo")==0 ) Config->vo_driver = rstrcpy(Config->vo_driver, argv[++i]);
        else if ( strcmp(argv[i], "-ao")==0 ) Config->ao_driver = rstrcpy(Config->ao_driver, argv[++i]);
        else if ( strcmp(argv[i], "-esc")==0 ) Config->flags |= CONFIG_ALLOW_KEY_EXIT;
        else if ( strcmp(argv[i], "-s")==0 ) ParseCommandLineWindowSize(argv[++i], Config);
        else if ( strcmp(argv[i], "-into")==0 ) Config->parent=rstrcpy(Config->parent, argv[++i]);
        else if ( strcmp(argv[i], "-win")==0 ) Config->parent=rstrcpy(Config->parent,argv[++i]);
        else if ( strcmp(argv[i], "-wid")==0 ) Config->parent=rstrcpy(Config->parent,argv[++i]);
        else if ( strcmp(argv[i], "-parent")==0 ) Config->parent=rstrcpy(Config->parent,argv[++i]);
        else if ( strcmp(argv[i], "-root")==0 ) Config->parent=rstrcpy(Config->parent,"root");
        else if ( strcmp(argv[i], "-fs")==0 ) Config->parent=rstrcpy(Config->parent,"fullscreen");
        else if ( strcmp(argv[i], "-raise")==0 ) Config->parent=rstrcpy(Config->parent,"raised");
        else if ( strcmp(argv[i], "-rootwin")==0 ) Config->parent=rstrcpy(Config->parent,"root");
        else if ( strcmp(argv[i], "-startms")==0 )
        {
            Config->startms=strtof(argv[++i], NULL);
            if (Config->startms==0) Config->startms=1;
        }
        else if ( strcmp(argv[i], "-debug")==0 ) Config->flags |= CONFIG_DEBUG;
        else if ( strcmp(argv[i], "-save-config")==0 ) Config->flags |= CONFIG_SAVE;
        else if ( strcmp(argv[i], "-defaults")==0 ) ConfigDefaults(Config);
        else if ( strcmp(argv[i], "-pause")==0 ) Config->flags |= CONFIG_PAUSE;
        else if ( strcmp(argv[i], "-mute")==0 ) Config->flags |= CONFIG_MUTE;
        else if ( strcmp(argv[i], "-image-time")==0 ) Config->image_ms=atoi(argv[++i]);
        else if ( strcmp(argv[i], "-imagems")==0 ) Config->image_ms=atoi(argv[++i]);
        else if ( strcmp(argv[i], "-prefix")==0 ) Config->path_prefix=rstrcpy(Config->path_prefix, argv[++i]);
        else if ( strcmp(argv[i], "-halfspeed")==0 ) Config->flags |= CONFIG_HALFSPEED;
        else if ( strcmp(argv[i], "-slow")==0 ) Config->flags |= CONFIG_HALFSPEED;
        else if ( strcmp(argv[i], "-quartspeed")==0 ) Config->flags |= CONFIG_QUARTSPEED;
        else if ( strcmp(argv[i], "-vslow")==0 ) Config->flags |= CONFIG_QUARTSPEED;
        else if ( strcmp(argv[i], "-loud")==0 ) Config->flags |= CONFIG_LOUD;
        else if ( strcmp(argv[i], "-vloud")==0 ) Config->flags |= CONFIG_VLOUD;
        else if ( strcmp(argv[i], "-identify")==0 ) Config->flags |=CONFIG_IDENTIFY;
        else if ( strcmp(argv[i], "-bookmark")==0 ) Config->flags |= CONFIG_BOOKMARK;
        else if ( strcmp(argv[i], "-no-bookmark")==0 ) Config->flags &= ~CONFIG_BOOKMARK;
        else if ( strcmp(argv[i], "-stream")==0 ) Config->flags |= CONFIG_STREAM;
        else if ( strcmp(argv[i], "-background")==0 ) Config->background=rstrcpy(Config->background, argv[++i]);
        else if ( strcmp(argv[i], "-persist")==0 ) Config->flags |= CONFIG_PERSIST;
        else if ( strcmp(argv[i], "-quit")==0 ) Config->flags &= ~CONFIG_PERSIST;
        else if ( strcmp(argv[i], "-stop_xscreensaver")==0 ) Config->flags |= DISABLE_SCREENSAVER;
        else if ( strcmp(argv[i], "-screensaver")==0 ) Config->flags |= DISABLE_SCREENSAVER;
        else if ( strcmp(argv[i], "+screensaver")==0 ) Config->flags &= ~ (DISABLE_SCREENSAVER | KILL_SCREENSAVER);
        else if ( strcmp(argv[i], "-ss")==0 ) Config->flags |= DISABLE_SCREENSAVER;
        else if ( strcmp(argv[i], "-SS")==0 ) Config->flags |= DISABLE_SCREENSAVER | KILL_SCREENSAVER;
        else if ( strcmp(argv[i], "+ss")==0 ) Config->flags &= ~ (DISABLE_SCREENSAVER | KILL_SCREENSAVER);
        else if ( strcmp(argv[i], "-slave")==0 )
        {
            Config->flags |= CONFIG_PERSIST;
            Config->startms=1;
        }
        else if ( strcmp(argv[i], "-input")==0 )
        {
            i++;
            if (strncmp(argv[i],"file:",5)==0) Config->control_pipe_path=rstrcpy(Config->control_pipe_path,argv[i]+5);
            else Config->control_pipe_path=rstrcpy(Config->control_pipe_path,argv[i]);
        }
        else if ( strcmp(argv[i], "-zoom")==0 ) Config->zoom=atoi(argv[++i]);
        else if ( strcmp(argv[i], "-nice")==0 ) Config->priority=atoi(argv[++i])+21;
        else if ( strcmp(argv[i], "-prio")==0 ) Config->priority=atoi(argv[++i])+1;
        else if ( strcmp(argv[i], "-shuffle")==0 ) Config->flags |= CONFIG_SHUFFLE;
        else if ( strcmp(argv[i], "-loop")==0 ) Config->loop=atoi(argv[++i]);
        else if ( strcmp(argv[i], "-keygrab")==0 ) Config->keygrabs=rstrcpy(Config->keygrabs, argv[++i]);
        else if ( strcmp(argv[i], "-helpers")==0 ) Config->helpers=rstrcpy(Config->helpers, argv[++i]);
        else if ( strcmp(argv[i], "+helpers")==0 )
        {
            Token=rstrcpy(Token, Config->helpers);
            Config->helpers=rstrcpy(Config->helpers, argv[++i]);
            Config->helpers=rstrcat(Config->helpers, ";");
            Config->helpers=rstrcat(Config->helpers, Token);
        }
        else if ( strcmp(argv[i], "-cache")==0 ) CommandLineParseCache(argv[++i]);
        else if ( strcmp(argv[i], "-id")==0 ) ID=rstrcpy(ID, argv[++i]);
        else if ( strcmp(argv[i], "-nowplay")==0 )
        {
            i++;
            if ( (access(argv[i], F_OK)==0) || (mkfifo(argv[i], 0700)==0) ) Config->nowplay_pipe_path=rstrcpy(Config->nowplay_pipe_path, argv[i]);
        }
        else if ( strcmp(argv[i], "-ac")==0 ) Config->audio_compression=atoi(argv[++i]);
        else if ( strcmp(argv[i], "-ap")==0 ) Config->audio_plugins=rstrcpy(Config->audio_plugins, argv[++i]);
        else if ( strcmp(argv[i], "-aplug")==0 ) Config->audio_plugins=rstrcpy(Config->audio_plugins, argv[++i]);
        else if ( strcmp(argv[i], "-no-video")==0 ) Config->flags |= CONFIG_NOVIDEO;
        else if ( strcmp(argv[i], "-novideo")==0 ) Config->flags |= CONFIG_NOVIDEO;
        else if ( strcmp(argv[i], "-dvd-device")==0) CXineConfigModify(Config->xine, "media.dvd.device", argv[++i]);
        else if ( strcmp(argv[i], "-dvd-lang")==0) CXineConfigModify(Config->xine, "media.dvd.language", argv[++i]);
        else if ( strcmp(argv[i], "-dvd-region")==0) CXineConfigNumericModify(Config->xine, "media.dvd.region", atoi(argv[++i]));
        else if ( strcmp(argv[i], "-alang")==0) CXineConfigModify(Config->xine, "media.dvd.language", argv[++i]);
        else if ( strcmp(argv[i], "-osd-show")==0 ) Config->flags |= CONFIG_OSD;
        else if ( strcmp(argv[i], "-show-osd")==0 ) Config->flags |= CONFIG_OSD;
        else if ( strcmp(argv[i], "-osd-hide")==0 ) Config->flags &= ~CONFIG_OSD;
        else if ( strcmp(argv[i], "-hide-osd")==0 ) Config->flags &= ~CONFIG_OSD;
        else if ( strcmp(argv[i], "-osd")==0 )
        {
            Config->flags |= CONFIG_OSD;
            ptr=rstrtok(argv[++i],",",&Token);
            if (strcmp(Token,"bottom")==0) Config->bottom_osd_text=rstrcpy(Config->bottom_osd_text, argv[i]);
            else Config->bottom_osd_text=rstrcpy(Config->bottom_osd_text, argv[i]);
        }
        else if ( strcmp(argv[i], "-title")==0 ) Title=rstrcpy(Title, argv[++i]);
        else if (
            ( strcmp(argv[i], "-use-stdin")==0 ) ||
            ( strcmp(argv[i], "-")==0 )
        )
        {
            PlaylistAdd(Config->playlist, "stdin://", ID, Title);
//			Config->flags &= ~CONFIG_CONTROL;
        }
        else if ( strcmp(argv[i], "-v")==0 ) Config->debug++;
        else if ( strcmp(argv[i], "-version")==0 )
				{
					printf("cxine %s\n", VERSION);
					exit(0);
				}
        else if ( strcmp(argv[i], "--version")==0 )
				{
					printf("cxine %s\n", VERSION);
					exit(0);
				}

        else if ( strcmp(argv[i], "-?")==0 ) Help(argv[++i]);
        else if ( strcmp(argv[i], "-help")==0 ) Help(argv[++i]);
        else if ( strcmp(argv[i], "--help")==0 ) Help(argv[++i]);
        else PlaylistAdd(Config->playlist, argv[i], ID, Title);
    }


    destroy(Token);
    destroy(Title);
    return(1);
}




