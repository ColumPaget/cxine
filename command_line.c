#include "command_line.h"
#include "control_protocol.h"

void PrintUsage(const char *Page)
{
	if ( StrLen(Page) && (strcmp(Page, "keys")==0) )
	{
	printf("Keys:\n");
	printf("<escape>        exit app (requires -esc command-line option)\n");
	printf("<spacebar>      pause playback\n");
	printf("<pause>         pause playback\n");
	printf("<home>          seek to start of playback\n");
	printf("<end>           seek to near end of playback\n");
	printf("<enter>         next item in playlist\n");
	printf("<left arrow>    seek back 10 seconds\n");
	printf("                shift: previous item in playlist\n");
	printf("                ctrl: step back (not frame accurate like ctrl-right)\n");
	printf("<right arrow>   seek forward 10 seconds\n");
	printf("                shift: next item in playlist\n");
	printf("                ctrl: single-step 1-frame forwards\n");
	printf("<page up>       forward dvd chapter \n");
	printf("<page down>     back dvd chapter \n");
	printf("+               increase volume\n");
	printf("-               decrease volume\n");
	printf("m               mute\n");
	printf("o               toggle online display\n");
	printf("f               toggle 'fast' playback (4*speed, no sound)\n");
	printf("s               toggle 'slow' playback (1/4 speed, no sound)\n");
	printf("<tab>           toggle between 'raised' and 'lowered' window modes\n");
	printf("                ctrl: toggle 'shaded' window mode (if supported by window-manager)\n");
	printf("<delete>        reset window to 'normal' mode. (turns off 'above', 'below' and 'shaded' modes)\n");
	printf(".               reset window to 'normal' mode. (turns off 'above', 'below' and 'shaded' modes)\n");
	printf("<               prev item in playlist\n");
	printf(">               next item in playlist\n");
	printf("1,2,3...        seek to n*10 percent (so 5 seeks to 50%% of stream)\n");
	printf("\n");
	printf("The following keys on 'internet' or 'media' keyboards are supported\n");
	printf("<play>\n");	
	printf("<pause playback>\n");	
	printf("<next>\n");	
	printf("<previous>\n");	
	printf("<volume up>     with 'shift' causes a larger increase\n");	
	printf("<volume down>   with 'shift' casues a larger decrease\n");	
	printf("<volume mute>\n");	
	}
	else if ( StrLen(Page) && (strcmp(Page, "keygrabs")==0) )
	{
		printf("Keygrabs:\n");
		printf("cxine can grab keys for its exclusive use. Keys are described by a comma-separated list of strings like 'alt-k' or 'cntrl-left'. Cxine recognises the following key names:\n");
		printf("up, down, left, right       arrow keys\n");
		printf("pgup, pgdn, home, end       page-up, page-down, home and end keys\n");
		printf("pgup, pgdn, home, end       page-up, page-down, home and end keys\n");
		printf("ins, del                    insert and delete keys\n");
		printf("vup, vdown, mute            volume up, down and mute keys found on 'internet' keyboards\n");
		printf("stop, play, next, prev      playback keys found on 'internet' keyboards\n");
		printf("vpause                      media pause key found on 'internet' keyboards\n");
		printf("\nCXine also supports the following key group names:\n");
		printf("arrow                       left right up down\n");
		printf("volume                      vup, vdown, mute\n");
		printf("page                        pgup, pgdn\n");
		printf("nav                         left, right, up, down, pgup, pgdn, home, end\n");
		printf("media                       vup, vdown, mute, stop, play, prev, next, vpause\n");
		printf("keypad                      up, down, left, right pgup, pgdn, home, end, +, -\n");
		printf("\nKeynames and keygroups can be prefixed with the modifiers 'alt-', 'shift', and 'cntrl'. If cxine receives a keypress that it doesn't have a binding for, like alt-righ... oh dear... like 'alt-up' for example, it will read it as the unmodified key. Hence keys like the arrow keys can be bound using 'alt' and used as globally bound keystrokes to navigate a stream.\n");
	}
	else if ( StrLen(Page) && (strcmp(Page, "osd")==0) )
	{
	printf("OSD configure:\n\n");
	printf("On Screen Displays can be configured at the top and the bottom of the screen. This is achieved with the '-osd' command-line option. This option takes an argument which is a string that is a comma-seperated pair of strings, where the first string is 'top' or 'bottom' to specify where this osd should be displayed, and the second string is a definition of what is to be displayed. This second string can include the following printf-style '%%' substitutions.\n\n");
	printf("%%%%     output '%%'\n");
	printf("%%B      output current stream bitrate\n");
	printf("%%w      output current stream video width\n");
	printf("%%h      output current stream video height\n");
	printf("%%o      output current stream AV offset\n");
	printf("%%A      output artist of current track\n");
	printf("%%T      output title of current track\n");
	printf("%%v      output audio volume (0-100)\n");
	printf("%%av     output audio volume (0-100)\n");
	printf("%%ac     output audio file FourCC\n");
	printf("%%ab     output audio bitrate\n");
	printf("%%as     output audio samplerate\n");
	printf("%%aw     output audio width (compression level)\n");
	printf("%%ma     output artist for current track\n");
	printf("%%mA     output album for current track\n");
	printf("%%mt     output title of current track\n");
	printf("%%mT     output title of current track\n");
	printf("%%mY     output year for current track\n");
	printf("%%mG     output genre for current track\n");
	printf("%%mc     output DVD chapter number\n");
	printf("%%mC     output number of chapters on DVD\n");
	printf("%%ts     output number of seconds into track\n");
	printf("%%tl     output length of current track in seconds\n");
	printf("%%tP     output percent into current track\n");
	printf("%%tt     output current time in form HH:MM\n");
	printf("%%tT     output current time in form HH:MM:SS\n");
	printf("%%td     output current date in form YY/mm/dd\n");
	printf("%%tD     output current date in form YYYY/mm/dd\n");
	printf("%%tN     output current date and time in form YYYY/mm/dd HH:MM:SS\n");
	}
	else if ( StrLen(Page) && (strcmp(Page, "slave")==0) )
	{
		printf("Slave mode:\n\n");
		printf("This is a (mostly) MPlayer compatible protocol to control cxine through a pipe, or through commands on stdin. It is mostly used by frontends.\n\n");
		printf("add <url>                 add a url to the playlist\n");
		printf("dvdnav up                 send event to the dvdnav system\n");
		printf("dvdnav down               send event to the dvdnav system\n");
		printf("dvdnav left               send event to the dvdnav system\n");
		printf("dvdnav right              send event to the dvdnav system\n");
		printf("dvdnav select             send event to the dvdnav system\n");
		printf("dvdnav prev               send event to the dvdnav system\n");
		printf("get_audio_bitrate         return bitrate of current track\n");
		printf("get_audio_codec           return codec of current track\n");
		printf("get_video_codec           return codec of current track\n");
		printf("get_meta_album            return album name of current track\n");
		printf("get_meta_title            return title of current track\n");
		printf("get_meta_artist           return artist name for current track\n");
		printf("get_meta_artist           return artist name for current track\n");
		printf("get_meta_genre            return genre  name for current track\n");
		printf("get_meta_year             return year for current track\n");
		printf("get_meta_comment          return comment for current track\n");
		printf("get_video_resolution      return width and height or current video\n");
		printf("get_file_name             return url of currently playing item\n");
		printf("get_percent_pos           return percent of track played\n");
		printf("get_time_pos              return number of seconds of track played\n");
		printf("get_time_length           return total number of seconds of track\n");
		printf("loadfile <url> <append>   load url to playlist. if <append> is a non-zero value then this is appended to playlist without switching to the new track\n");
		printf("mute                      toggle audio mute\n");
		printf("pause                     pause playback\n");
		printf("quit                      tell cxine to exit\n");
		printf("stop                      stop playing\n");
		printf("seek <offset> <type>      seek to offset. Type is '0': seconds from current, '1': absolute percent, '2': seconds from start\n");
		printf("volume <value>            set volume (0-100)\n");
  }
	else
	{
	printf("Usage:\n");
	printf("	cxine [options] [url]\n");
	printf("Options:\n");
	printf("  -add <url>             Add <url> to the playlist of an existing cxine, then exit.\n");
	printf("  -queue <url>           If no cxine is currently running, then start up and play <url>, else add it to the playlist of an existing cxine.\n");
	printf("  -esc                   Allow the 'escape' key to exit the app.\n");
	printf("  -win     <win id>      'root', 'fullscreen', 'sticky', 'shaded', 'ontop', 'stickontop' or id of window to reparent into.\n");
	printf("  -into    <win id>      Window to reparent into (xterm style option).\n");
	printf("  -wid     <win id>      Window to reparent into (mplayer style option).\n");
	printf("  -parent  <win id>      Window to reparent into (maximum clarity style).\n");
	printf("  -s <wid>x<high>        Window size. Default is 480x360.\n");
	printf("  -esc                   Allow the 'escape' key to exit the app.\n");
	printf("  -ao <ao name>          Audio output plugin name (default = alsa).\n");
	printf("  -vo <vo name>          Video output plugin name (default = Xv).\n");
	printf("  -identify              Output machine-readable track information on stdout (mplayer feature).\n");
	printf("  -root                  Draw on root window (equivalent to -win root).\n");
	printf("  -rootwin               Draw on root window (mplayer style option).\n");
	printf("  -fs                    Play fullscreen (mplayer style option equivalent to '-win fullscreen').\n");
	printf("  -use-stdin             Read media stream from stdin\n");
	printf("  -pause                 Start paused.\n");
	printf("  -mute                  Start muted.\n");
	printf("  -shuffle               Shuffle playlist.\n");
	printf("  -loop <n>              Repeat playback <n> times.\n");
	printf("  -image-time <ms>       Number of milliseconds to pause on an image for, until starting to play/display the next track.\n");
	printf("  -imagems <ms>          Number of milliseconds to pause on an image for, until starting to play/display the next track.\n");
	printf("  -stop_xscreensaver     Disable screensaver during playing, re-enable it on exit.\n");
	printf("  -persist               Don't exit when nothing left to do, wait for commands on stdin.\n");
	printf("  -slave                 Mplayer compatible flag, equivalent to '-persist -startms 0'.\n");
	printf("  -input <path>          Path to pipe to read commands from (by default cxine creates a pipe in '~/.cxine/cxine.pipe'). This accepts mplayer compatible form '-input file:<path>'.\n");
	printf("  -nowplay <path>        Path to pipe to write 'now playing' info to (cxine will create this pipe if it doesn't already exist).\n");
	printf("  -startms <millisecs>   Start playing at <millisecs>'.\n");
	printf("  -osd-show              Show On Screen Display.\n");
	printf("  -osd-hide              Hide On Screen Display'.\n");
	printf("  -osd <format>          Define format of OSD bar.\n");
	printf("  -slow                  Play halfspeed.\n");
	printf("  -vslow                 Play quarterspeed.\n");
	printf("  -loud                  Play with raised volume.\n");
	printf("  -vloud                 Play with maximum volume.\n");
	printf("  -keygrab               Register keygrabs, a comma-separated list of keys. See keygrabs below.\n");
	printf("  -prio <value>          Set process priority in range 0-39 (requires superuser capabilities).\n");
	printf("  -nice <value>          Set process priority with 'nice' semantics (requires superuser capabilities).\n");
	printf("  -ac <value>            Set audio compression. This boosts quiet sounds, lessening the volume range. value is a percent > 100 to multiply quiet sounds by.\n");
	printf("  -ap <plugins>          Comma separated list of audio post processing plugins.\n");
	printf("  -aplug <plugins>       Comma separated list of audio post processing plugins.\n");
	printf("  -?                     Show this help.\n");
	printf("  -help                  Show this help.\n");
	printf("  --help                 Show this help.\n");
	printf("  --help keys            Show keybindings.\n");
	printf("  --help keygrabs        Show help for keygrab config.\n");
	printf("  --help osd             Show help for OSD config.\n");
	printf("  --help slave           Show help for MPlayer-compatible slave mode.\n");
	printf("\nBy default cxine 'bookmarks' the position in a video if exit is requested, and restarts from that position if the media is played again. This can be disabled with '-startms 0'. \n");
	printf("\nWindow ids given to '-win' and siblings, can either be expressed in hex (with a leading 0x) or in decimal.\n");
	printf("\nDVDs can be played using the urls 'dvd://0' to use dvdnav menu screen, or dvd://1, dvd://2, etc to play a particular track on disk. \n");
	printf("\nData from stdin can be read with a url of the form 'stdin://'. \n");
	}


	exit(0);
}      




void ParseCommandLineWindowSize(char *arg, TConfig *Config)
{
char *ptr;

Config->width=strtol(arg, &ptr, 10);
if (ptr) ptr++;
Config->height=strtol(ptr, &ptr, 10);
}


int XineAddURL(const char *URL)
{
int fd, result=-1;
char *Tempstr=NULL;

fd=ControlPipeOpen(O_WRONLY | O_NONBLOCK);
if ( (fd > -1) && (lockf(fd, F_LOCK, 0)==0) )
{
Tempstr=rstrcpy(Tempstr, "add ");
Tempstr=rstrcat(Tempstr, URL);
result=write(fd, Tempstr, StrLen(Tempstr));
close(fd);
Config->control_pipe=-1;
}
destroy(Tempstr);

return(result > 0);
}


int XineQueueURL(const char *URL)
{
if (XineAddURL(URL)) exit(0);

StringListAdd(Config->playlist, URL);
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


int ParseCommandLine(int argc, char *argv[], TConfig *Config)
{
  int i;
	char *Token=NULL;
	const char *ptr;

  for(i = 1; i < argc; i++) 
	{
      if (strcmp(argv[i], "-add")==0 ) XineAddURL(argv[++i]);
			else if ( strcmp(argv[i], "-queue")==0 ) XineQueueURL(argv[++i]);
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
      else if (
								( strcmp(argv[i], "-use-stdin")==0 ) ||
								( strcmp(argv[i], "-")==0 ) 
						)
			{
				StringListAdd(Config->playlist, "stdin://");
				Config->flags &= ~CONFIG_CONTROL;
			}
      else if ( strcmp(argv[i], "-?")==0 ) PrintUsage(argv[++i]);
      else if ( strcmp(argv[i], "-help")==0 ) PrintUsage(argv[++i]);
      else if ( strcmp(argv[i], "--help")==0 ) PrintUsage(argv[++i]);
      else StringListAdd(Config->playlist, argv[i]);
	}

if (! StrLen(Config->vo_driver)) Config->vo_driver=strdup("auto");
if (! StrLen(Config->ao_driver)) Config->ao_driver=strdup("auto");
if (Config->flags & CONFIG_SHUFFLE) ShufflePlaylist();

destroy(Token);
return(1);
}




