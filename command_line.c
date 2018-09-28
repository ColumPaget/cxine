#include "command_line.h"
#include "control_protocol.h"
#include "help.h"




static void ParseCommandLineWindowSize(char *arg, TConfig *Config)
{
char *ptr;

Config->width=strtol(arg, &ptr, 10);
if (ptr) ptr++;
Config->height=strtol(ptr, &ptr, 10);
}


static int XineAddURL(const char *URL, const char *Title)
{
int fd, result=-1;
char *Tempstr=NULL;

fd=ControlPipeOpen(O_WRONLY | O_NONBLOCK);
if ( (fd > -1) && (lockf(fd, F_LOCK, 0)==0) )
{
Tempstr=rstrcpy(Tempstr, "add ");
Tempstr=rstrcat(Tempstr, URL);
if (StrLen(Title))
{
Tempstr=rstrcpy(Tempstr, " ");
Tempstr=rstrcat(Tempstr, Title);
}
result=write(fd, Tempstr, StrLen(Tempstr));
close(fd);
Config->control_pipe=-1;
}
destroy(Tempstr);

return(result > 0);
}


static int XineQueueURL(const char *URL, const char *Title)
{
char *Tempstr=NULL;

if (XineAddURL(URL, Title)) exit(0);

Tempstr=rstrquot(Tempstr, URL, " ");
if (StrLen(Title))
{
Tempstr=rstrcat(Tempstr, " ");
Tempstr=rstrcat(Tempstr, Title);
}

StringListAdd(Config->playlist, Tempstr);

destroy(Tempstr);
}



void ShufflePlaylist()
{
char *Tempstr=NULL;
int rec1, rec2, i;

srand(time(NULL));
for (i=0; i < 100; i++)
{
rec1=rand() % Config->playlist->size;
rec2=rand() % Config->playlist->size;

Tempstr=rstrcpy(Tempstr, StringListGet(Config->playlist, rec1));
StringListSet(Config->playlist, rec1, StringListGet(Config->playlist, rec2));
StringListSet(Config->playlist, rec2, Tempstr);
}
Config->playlist->next=0;

destroy(Tempstr);
}


static void CommandLineAddPlayItem(const char *URL, const char *Title)
{
char *Tempstr=NULL;

	Tempstr=rstrcpy(Tempstr, URL);
	if (StrLen(Title))
	{
		Tempstr=rstrcat(Tempstr, " ");
		Tempstr=rstrcat(Tempstr, Title);
	}
	StringListAdd(Config->playlist, Tempstr);

destroy(Tempstr);
}


int ParseCommandLine(int argc, char *argv[], TConfig *Config)
{
  int i;
	char *Token=NULL, *Title=NULL;
	const char *ptr;

  for(i = 1; i < argc; i++) 
	{
      if (strcmp(argv[i], "-add")==0 ) XineAddURL(argv[++i], "");
      else if (strcmp(argv[i], "+add")==0 ) 
			{
				i++;
				XineAddURL(argv[i], argv[i+1]);
				i++;
			}
			else if ( strcmp(argv[i], "-queue")==0 ) XineQueueURL(argv[++i], "");
			else if ( strcmp(argv[i], "+queue")==0 )
			{
				i++;
				XineQueueURL(argv[i], argv[i+1]);
				i++;
			}
      else if ( strcmp(argv[i], "-vo")==0 ) Config->vo_driver = strdup(argv[++i]);
      else if ( strcmp(argv[i], "-ao")==0 ) Config->ao_driver = strdup(argv[++i]);
      else if ( strcmp(argv[i], "-esc")==0 ) Config->flags |= CONFIG_ALLOW_KEY_EXIT;
      else if ( strcmp(argv[i], "-s")==0 ) ParseCommandLineWindowSize(argv[++i], Config);
      else if ( strcmp(argv[i], "-into")==0 ) Config->parent=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-win")==0 ) Config->parent=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-wid")==0 ) Config->parent=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-parent")==0 ) Config->parent=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-root")==0 ) Config->parent=strdup("root");
      else if ( strcmp(argv[i], "-rootwin")==0 ) Config->parent=strdup("root");
      else if ( strcmp(argv[i], "-startms")==0 ) 
			{
				Config->startms=strtof(argv[++i], NULL);
				if (Config->startms==0) Config->startms=1;
			}
      else if ( strcmp(argv[i], "-pause")==0 ) Config->flags |= CONFIG_PAUSE;
      else if ( strcmp(argv[i], "-image-time")==0 ) Config->image_ms=atoi(argv[++i]);
      else if ( strcmp(argv[i], "-imagems")==0 ) Config->image_ms=atoi(argv[++i]);
      else if ( strcmp(argv[i], "-mute")==0 ) Config->flags |= CONFIG_MUTE;
      else if ( strcmp(argv[i], "-halfspeed")==0 ) Config->flags |= CONFIG_HALFSPEED;
      else if ( strcmp(argv[i], "-slow")==0 ) Config->flags |= CONFIG_HALFSPEED;
      else if ( strcmp(argv[i], "-quartspeed")==0 ) Config->flags |= CONFIG_QUARTSPEED;
      else if ( strcmp(argv[i], "-vslow")==0 ) Config->flags |= CONFIG_QUARTSPEED;
      else if ( strcmp(argv[i], "-loud")==0 ) Config->flags |= CONFIG_LOUD;
      else if ( strcmp(argv[i], "-vloud")==0 ) Config->flags |= CONFIG_VLOUD;
      else if ( strcmp(argv[i], "-identify")==0 ) Config->flags |=CONFIG_IDENTIFY;
      else if ( strcmp(argv[i], "-bookmark")==0 ) Config->flags |= CONFIG_BOOKMARK;
      else if ( strcmp(argv[i], "-persist")==0 ) Config->flags |= CONFIG_PERSIST;
      else if ( strcmp(argv[i], "-stop_xscreensaver")==0 ) Config->flags |= DISABLE_SCREENSAVER;
      else if ( strcmp(argv[i], "-slave")==0 ) 
			{
			Config->flags |= CONFIG_PERSIST;
			Config->startms=1;
			}
      else if ( strcmp(argv[i], "-input")==0 ) 
			{
				i++;
				if (strncmp(argv[i],"file:",5)==0) Config->control_pipe_path=strdup(argv[i]+5);
				else Config->control_pipe_path=strdup(argv[i]);
			}
      else if ( strcmp(argv[i], "-fs")==0 ) Config->parent=strdup("fullscreen");
      else if ( strcmp(argv[i], "-raise")==0 ) Config->parent=strdup("raised");
      else if ( strcmp(argv[i], "-zoom")==0 ) Config->zoom=atoi(argv[++i]);
      else if ( strcmp(argv[i], "-nice")==0 ) Config->priority=atoi(argv[++i])+21;
      else if ( strcmp(argv[i], "-prio")==0 ) Config->priority=atoi(argv[++i])+1;
      else if ( strcmp(argv[i], "-shuffle")==0 ) Config->flags |= CONFIG_SHUFFLE;
      else if ( strcmp(argv[i], "-loop")==0 ) Config->loop=atoi(argv[++i]);
      else if ( strcmp(argv[i], "-keygrab")==0 ) Config->keygrabs=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-nowplay")==0 ) 
			{
				i++;
				if ( (access(argv[i], F_OK)==0) || (mkfifo(argv[i], 0700)==0) ) Config->nowplay_pipe=open(argv[i], O_WRONLY | O_NONBLOCK);
			}
      else if ( strcmp(argv[i], "-ac")==0 ) Config->audio_compression=atoi(argv[++i]);
      else if ( strcmp(argv[i], "-ap")==0 ) Config->audio_plugins=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-aplug")==0 ) Config->audio_plugins=strdup(argv[++i]);
      else if ( strcmp(argv[i], "-osd-show")==0 ) Config->flags |= CONFIG_OSD;
      else if ( strcmp(argv[i], "-osd-hide")==0 ) Config->flags &= ~CONFIG_OSD;
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
				CommandLineAddPlayItem("stdin://", Title);
//			Config->flags &= ~CONFIG_CONTROL;
			}
      else if ( strcmp(argv[i], "-?")==0 ) Help(argv[++i]);
      else if ( strcmp(argv[i], "-help")==0 ) Help(argv[++i]);
      else if ( strcmp(argv[i], "--help")==0 ) Help(argv[++i]);
      else CommandLineAddPlayItem(argv[i], Title);
	}

if (! StrLen(Config->vo_driver)) Config->vo_driver=strdup("auto");
if (! StrLen(Config->ao_driver)) Config->ao_driver=strdup("auto");
if (Config->flags & CONFIG_SHUFFLE) ShufflePlaylist();

destroy(Token);
destroy(Title);
return(1);
}




