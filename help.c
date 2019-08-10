/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "help.h"
#include "plugins.h"


static void HelpKeys()
{
    printf("Keys:\n");
    printf("<escape>        exit app (requires -esc command-line option)\n");
    printf("                shift: raise/lower window if window started with 'raised'\n");
    printf("                ctrl:  shade/unshade window\n");
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
		printf("a               cycle audio outputs. Choice of outputs must have been provided on command-line, e.g. '-ao alsa:0,alsa:1,alsa:2\n");
    printf("o               toggle online display\n");
    printf("f               toggle fullscreen\n");
    printf("T               toggle stay-on-top\n");
    printf("p               display playlist menu\n");
		printf("[               slower playback (no sound)\n");
		printf("]               faster playback (no sound)\n");
    printf("{               toggle 'fast' playback (4*speed, no sound)\n");
    printf("}               toggle 'slow' playback (1/4 speed, no sound)\n");
		printf("<backspace>     reset playback speed to normal\n");
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

static void HelpKeygrabs()
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


static void HelpCommandSend()
{
printf("The -cmd option sends commands to another, already running, cxine. Many of the commands toggle between two states, like raised/unraised or paused/unpaused. Available commands are:\n");
printf("		pause     pause/unpause playback\n");
printf("		stop      stop playback\n");
printf("		mute      mute/unmute audio\n");
printf("		next      next track\n");
printf("		prev      previous track\n");
printf("		shade     shade/unshade window\n");
printf("		mini      minimize/restore window\n");
printf("		icon      minimize/restore window\n");
printf("		minimize  minimize/restore window\n");
printf("		lower     lower window below others\n");
printf("		raise     raise window above others\n");
printf("		zcycle    cycle between raised, normal, and lowered window\n");
}


static void HelpHelpers()
{
    char *Tempstr=NULL, *Helper=NULL, *Protos=NULL;
    const char *ptr;

    printf("CXine uses external programs to download remote media. The configuration of known programs can be changed using the -helpers or +helpers command-line switch. Helper entries have the form:\n\n");
    printf("  <protocol>,<protocol>:<helper>\n\n");
    printf("e.g.:\n\n");
    printf("  http,https,ftp:links -source $(mrl)\n\n");
    printf("The string '$(mrl)' is replaced with the media url to download. These helper configs can be strung together using semicolons.\n");
    printf("The default config is:\n\n");
    printf("  %s\n\n", Config->helpers);


    printf("Protocol helpers config:\n");
    ptr=rstrtok(Config->helpers, ";", &Tempstr);
    while (ptr)
    {
        Helper=rstrcpy(Helper, rstrtok(Tempstr, ":", &Protos));
        printf("%s  %s\n", Protos, Helper);
        ptr=rstrtok(ptr, ";", &Tempstr);
    }

    destroy(Tempstr);
    destroy(Helper);
    destroy(Protos);
}


static void HelpOSD()
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
    printf("%%C      output comment of current track\n");
    printf("%%v      output audio volume (0-100)\n");
    printf("%%av     output audio volume (0-100)\n");
    printf("%%ac     output audio file FourCC\n");
    printf("%%ab     output audio bitrate\n");
    printf("%%as     output audio samplerate\n");
    printf("%%aw     output audio width (compression level)\n");
    printf("%%Ls     output size of playlist (number of queued tracks)\n");
    printf("%%Ls     output curr track in playlist being played\n");
    printf("%%ma     output artist for current track\n");
    printf("%%mA     output album for current track\n");
    printf("%%mt     output title of current track\n");
    printf("%%mT     output title of current track\n");
    printf("%%mY     output year for current track\n");
    printf("%%mG     output genre for current track\n");
    printf("%%mR     output copyright for current track\n");
    printf("%%mc     output DVD chapter number\n");
    printf("%%mC     output number of chapters on DVD\n");
    printf("%%ts     output number of seconds into track\n");
    printf("%%tS     output seconds into and duration of track as <position>/<track length>\n");
    printf("%%tl     output length of current track in seconds\n");
    printf("%%tP     output percent into current track\n");
    printf("%%tt     output current time in form HH:MM\n");
    printf("%%tT     output current time in form HH:MM:SS\n");
    printf("%%td     output current date in form YY/mm/dd\n");
    printf("%%tD     output current date in form YYYY/mm/dd\n");
    printf("%%tN     output current date and time in form YYYY/mm/dd HH:MM:SS\n");
    printf("%%tw     output position in track as HH:MM:SS\n");
    printf("%%tW     output length of track as HH:MM:SS\n");
}


void HelpSlaveMode()
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
    printf("mute <val>                set audio mute (val='1' muted, val='0' unmute)\n");
    printf("vo_fullscreen             toggle fullscreen\n");
    printf("vo_fullscreen <val>       set fullscreen (val='1' fullscreen, val='0' normal)\n");
    printf("vo_ontop                  toggle ontop\n");
    printf("vo_ontop <val>            set ontop (val='1' ontop, val='0' normal)\n");
    printf("pause                     pause playback\n");
    printf("next                      play next track\n");
    printf("prev                      play prev track\n");
    printf("raise                     toggle raise window above others (stay on top)\n");
    printf("lower                     toggle lower window below others (stay below) \n");
    printf("zcycle                    cycle between 'stay on top', 'stay below' and 'normal' window \n");
    printf("shade                     toggle window shade/unshade\n");
    printf("minimize                  iconize window\n");
    printf("quit                      tell cxine to exit\n");
    printf("stop                      stop playing\n");
    printf("seek <offset> <type>      seek to offset. Type is '0': seconds from current, '1': absolute percent, '2': seconds from start\n");
    printf("volume <value>            set volume (0-100)\n");
}


//char *xine_get_file_extensions (xine_t *self) XINE_PROTECTED;
void HelpMimeTypes(xine_t *xine)
{
    char *MimeList=NULL, *Tempstr=NULL, *Demux=NULL;
    const char *ptr;

    MimeList=xine_get_mime_types(xine);
    ptr=rstrtok(MimeList, " ", &Tempstr);
    while (ptr)
    {
        Demux=xine_get_demux_for_mime_type(xine, Tempstr);
        strrep(Tempstr, '\n', ' ');
        if (StrLen(Demux))
        {
            printf("%s [%s]\n",Tempstr, Demux);
        }
        else printf("[%s]\n",Tempstr);
        ptr=rstrtok(ptr, " ", &Tempstr);
    }

    destroy(Demux);
    destroy(Tempstr);
    destroy(MimeList);
}

/* get a list of file extensions for file types supported by xine
 *  * the list is separated by spaces
 *   *
 *    * the pointer returned can be free()ed when no longer used */

/* get a list of mime types supported by xine
 *  *
 *   * the pointer returned can be free()ed when no longer used */

/* get the demuxer identifier that handles a given mime type
 *  *
 *   * the pointer returned can be free()ed when no longer used
 *    * returns NULL if no demuxer is available to handle this. */


void Help(const char *Page)
{
    if ( StrLen(Page) && (strcmp(Page, "keys")==0) ) HelpKeys();
    else if ( StrLen(Page) && (strcmp(Page, "keygrabs")==0) ) HelpKeygrabs();
    else if ( StrLen(Page) && (strcmp(Page, "helpers")==0) ) HelpHelpers();
    else if ( StrLen(Page) && (strcmp(Page, "osd")==0) ) HelpOSD();
    else if ( StrLen(Page) && (strcmp(Page, "slave")==0) ) HelpSlaveMode();
    else if ( StrLen(Page) && (strcmp(Page, "plugins")==0) ) CXineDisplayPlugins(Config);
    else if ( StrLen(Page) && (strcmp(Page, "cmd")==0) ) HelpCommandSend();
    else
    {
        printf("Usage:\n");
        printf("	cxine [options] [url]\n");
        printf("Options:\n");
        printf("  -add <url> [title]      Add <url> to the playlist of an existing cxine, then exit. 'title' is an optional title to be displayed while playing\n");
        printf("  -queue <url>            If no cxine is currently running, then start up and play <url>, else add it to the playlist of an existing cxine.\n");
        printf("  +queue <url> [title]    Enhanced version of queue that accepts a 'title' argument\n");
        printf("  -enqueue <url>          MPlayer compatible equivalent of '-queue'\n");
        printf("  -esc                    Allow the 'escape' key to exit the app.\n");
        printf("  -win     <win id>       Window settings (see below)\n");
        printf("  -into    <win id>       Window to reparent into (xterm style option).\n");
        printf("  -wid     <win id>       Window to reparent into (mplayer style option).\n");
        printf("  -parent  <win id>       Window to reparent into (maximum clarity style).\n");
        printf("  -s <wid>x<high>         Window size. Default is 480x360.\n");
        printf("  -esc                    Allow the 'escape' key to exit the app.\n");
        printf("  -ao <ao name>           Audio output plugins list (default = alsa). This is a comma-seperated list of audio outputs to try, cxine will use the first one that works. For alsa  and oss different output devices can be specified by appending ':0', ':1' to select devices by number. For Jack, Sun and Pulse appending the device path/name instead of a number *may* work. If a list of devices is provided, then the 'a' key can be used at runtime to cycle through them.\n");
        printf("  -vo <vo name>           Video output plugin name (default = Xv).\n");
        printf("  -identify               Output machine-readable track information on stdout (mplayer feature).\n");
        printf("  -root                   Draw on root window (equivalent to -win root).\n");
        printf("  -rootwin                Draw on root window (mplayer style option).\n");
        printf("  -fs                     Play fullscreen (mplayer style option equivalent to '-win fullscreen').\n");
        printf("  -background <path>      Path to background image when not playing anything else. Needed for OSD to mix into.\n");
        printf("  -use-stdin              Read media stream from stdin\n");
        printf("  -bcast <port>           Broadcast to slave libxine players on <port>\n");
        printf("  -pause                  Start paused.\n");
        printf("  -mute                   Start muted.\n");
        printf("  -shuffle                Shuffle playlist.\n");
        printf("  -loop <n>               Repeat playback <n> times.\n");
				printf("  -show-playlist          Start with playlist displayed\n");
        printf("  -title <title>          Set title displayed for this track. This argument is positional and must preceede the track it names, so for exampe 'cxine -title 'title 1' track1.mp4 -title 'title 2' track2.mp4.\n");
        printf("  -image-time <ms>        Number of milliseconds to pause on an image for, until starting to play/display the next track.\n");
        printf("  -imagems <ms>           Number of milliseconds to pause on an image for, until starting to play/display the next track.\n");
        printf("  -stop_xscreensaver      Disable screensaver during playing, re-enable it on exit, or if playback paused.\n");
        printf("  -screensaver            Disable screensaver during playing, re-enable it on exit, or if playback paused.\n");
        printf("  +screensaver            Enable screensaver during playing (use this to turn off 'disable screensaver' if you saved the config).\n");
        printf("  -ss                     Disable screensaver during playing, re-enable it on exit, or if playback paused.\n");
        printf("  -SS                     Disable screensaver during playing, re-enable it on exit BUT NOT IF PLAYBACK PAUSED.\n");
        printf("  +ss                     Enable screensaver during playing.\n");
        printf("  -persist                Don't exit when nothing left to do, wait for commands on stdin.\n");
        printf("  -quit                   Exit when nothing left to do (use this if you saved -persist config and want to turn it off)\n");
        printf("  -slave                  Mplayer compatible flag, equivalent to '-persist -startms 0'.\n");
        printf("  -cache <age>            Max age of items in the download cache. Number is in seconds, but can be postfixed with 'm' for minutes, 'h' for hours and 'd' for days. e.g. '-cache 10h' means 'cache for ten hours'\n");
        printf("  -input <path>           Path to pipe to read commands from (by default cxine creates a pipe in '~/.cxine/cxine.pipe'). This accepts mplayer compatible form '-input file:<path>'.\n");
        printf("  -cmd <cmd>              Tell cxine to send a command to an already running cxine. Run 'cxine -cmd help' or 'cxine --help cmd' to learn more..\n");
        printf("  -nowplay <path>         Path to pipe to write 'now playing' info to (cxine will create this pipe if it doesn't already exist).\n");
        printf("  -startms <millisecs>    Start playing at <millisecs>'.\n");
        printf("  -dvd-device <path>      Path to DVD device to use (default /dev/dvd)\n");
        printf("  -dvd-region <n>         DVD region num (if dvd player supports this), range 1-8\n");
        printf("  -dvd-lang <lang>        Default language for DVDs\n");
        printf("  -osd-show               Show On Screen Display.\n");
        printf("  -osd-hide               Hide On Screen Display'.\n");
        printf("  -osd <format>           Define format of OSD bar.\n");
        printf("  -slow                   Play halfspeed.\n");
        printf("  -vslow                  Play quarterspeed.\n");
        printf("  -loud                   Play with raised volume.\n");
        printf("  -vloud                  Play with maximum volume.\n");
        printf("  -stream                 Don't download remote urls in playlists etc. This currently only works for 'http:' (not https:) urls. This allow streaming internet radio urls.\n");
        printf("  -webcast                Implies '-stream', treats playlists as webcast announcement files, only containing one item.\n");
        printf("  -prefix                 Append a prefix to a media url. This is mostly used with playlists, where the playlist file just names files, and -prefix is used to point the the directory they're in.\n");
        printf("  -keygrab                Register keygrabs, a comma-separated list of keys. See '-help keygrabs' below.\n");
        printf("  -helpers <config>       Register list of helper apps.\n");
        printf("  +helpers <config>       Prepend to list of helper apps (these helpers will be tried first).\n");
        printf("  -prio <value>           Set process priority in range 0-39 (requires superuser capabilities).\n");
        printf("  -nice <value>           Set process priority with 'nice' semantics (requires superuser capabilities).\n");
        printf("  -ac <value>             Set audio compression. This boosts quiet sounds, lessening the volume range. value is a percent > 100 to multiply quiet sounds by.\n");
        printf("  -ap <plugins>           Comma separated list of audio post processing plugins.\n");
        printf("  -aplug <plugins>        Comma separated list of audio post processing plugins.\n");
        printf("  -no-video               Don't output video (for use in combination with -ap for visualization plugins).\n");
        printf("  -novideo                Don't output video (for use in combination with -ap for visualization plugins).\n");
        printf("  -bookmark               Remember where media left-off playing.\n");
        printf("  -no-bookmark            Don't remember where media left-off playing.\n");
        printf("  -save-config            Save current cxine setup.\n");
        printf("  -defaults               Reset to default config (this will wipe any settings prior on the command line).\n");
        printf("  -version                Show program version.\n");
        printf("  --version               Show program version.\n");
        printf("  -?                      Show this help.\n");
        printf("  -help                   Show this help.\n");
        printf("  --help                  Show this help.\n");
        printf("  --help keys             Show keybindings.\n");
        printf("  --help keygrabs         Show help for keygrab config.\n");
        printf("  --help osd              Show help for OSD config.\n");
        printf("  --help slave            Show help for MPlayer-compatible slave mode.\n");
        printf("  --help plugins          List available plugins.\n");
        printf("\nWindow Settings:\n");
        printf("The -win argument accepts a window-type and an optional comma-separated list of extra settings. Window types are:\n");
        printf("  none           don't open an X11 window\n");
        printf("  normal         normal X11 window (is the default if no type specified)\n");
        printf("  root           output on desktop root window\n");
        printf("  fullscreen     fullscreen window\n");
        printf("  max            fullscreen window\n");
        printf("  0x<win id>     the hexidecimal (starting with 0x) id of a window to embed/reparent into\n");
        printf("  <win id>       the decimal id of a window to embed/reparent into\n");
        printf("Additional window settings are:\n");
        printf("  sticky         appear on all desktops\n");
        printf("  iconized       start with iconized/minimized window\n");
        printf("  iconic         start with iconized/minimized window\n");
        printf("  minimized      start with iconized/minimized window\n");
        printf("  min            start with iconized/minimized window\n");
        printf("  shaded         start with 'shaded' window (if supported by window manager)\n");
        printf("  ontop          keep above all other windows\n");
        printf("  above          keep above all other windows\n");
        printf("  below          keep below all other windows\n");
        printf("  stickontop     shorthand for sticky,ontop\n");
        printf("  stickabove     shorthand for sticky,ontop\n");
        printf("  stickbelow     shorthand for sticky,below\n");
        printf("Examples:\n");
        printf("  -win fullscreen,min         fullscreen window that's minimized at startup\n");
        printf("  -win sticky                 normal window that appears on all desktops\n");
        printf("  -win normal,shaded,below    normal window starts shaded and below other windows\n");
        printf("  -win 0x1f2bc                embed into window with hexadecimal id 1f2bc\n");
        printf("When embedding cxine into another application (e.g. a frontend) window ids can be given to '-win' either in hex (with a leading 0x) or in decimal.\n");
        printf("\nBookmarks:\n");

        printf("By default cxine 'bookmarks' the position in a video if exit is requested, and restarts from that position if the media is played again. This can be disabled with '-no-bookmark' or  '-startms 0'. \n");
        printf("\nDVDs can be played using the urls 'dvd://0' to use dvdnav menu screen, or dvd://1, dvd://2, etc to play a particular track on disk. The particular dvd device to read from can be set with '-dvd-device'.\n");
        printf("\nData from stdin can be read with a url of the form 'stdin://', and from a pipe with fifo://.\n");
        printf("\nMedia urls can be cut-and-pasted into cxine to add them to the playlist (drag and drop not implemented yet).\n");
        printf("\nDownloads\n");
        printf("CXine downloads media using helper apps. Default config will use curl, wget or Twighbright links, elinks, lynx, or snarf, depending on which are found in your path your path. Cxine should be able to accept ftp: ftps: sftp: and smb: urls via curl, but these have not been tested.\n");
        printf("You can also use 'ssh:' (not sftp, this actually streams files over ssh) urls if ssh is in your path, although these have to be set up in your '.ssh/config' to auto-login. So, for example the url 'ssh://myhost/home/music/BinaryFinary.mp3' will log into 'myhost' and use ssh to pull the file '/home/music/BinaryFinary.mp3' provided that 'myhost' has been set up in your .config with an ssh key to log in with.\n");
        printf("More information about helpers is available with 'cxine --help helpers'\n");
        printf("\nStreaming\n");
        printf("The '-stream' option is intended for use with internet radio, and only works for http:// streams. If '-stream' is supplied then http:// urls will be streamed without being downloaded to the cache directory\n");

				printf("\nBroadcast\n");
				printf("Cxine supports libxine-style broadcast. If the '-bcast' option is used to specify a port then any libxine player (xine, cxine, oxine etc) should be able to connect to it using the url 'slave://<address>:<port>' and mirror it's output.\n");
	
        printf("\nSaving config\n");
        printf("If '-save-config' is given then cxine will remember the following settings if they are supplied:\n");
        printf("		-vo, -ao, -ac, -ap, -prefix, -keygrab, -persist/-quit, -bookmark/-no-bookmark, -screensaver/+screensaver, -show-osd/-hide-osd, -background, -input, -cache, -nowplay,  -dvd-device, -dvd-region, -dvd-lang, -helpers\n");
        printf("-defaults resets the config to default. It will also reset any settings prior to it  on the command-line, so it's best to pass it as the first option. You can set configs to defaults, make some changes, and then save, like this:\n");
        printf("		cxine -defaults -background myimage.jpg -ao alsa -keygrab media -save\n");
    }


    exit(0);
}


