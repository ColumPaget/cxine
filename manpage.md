title: cxine
mansection: 1
date: 2020/05/21


NAME
====

cxine - a broadly mplayer-compatible libxine based media player.


DESCRIPTION
===========

Cxine is a lib-xine based media player that emulates some aspects of mplayer, and adds a number of features of its own. Cxine displays video on a single window and any control output is overlaid over that.


SYNOPSIS
========

	cxine [options] [url]


OPTIONS
=======

-add <url> [title]
 : Add <url> to the playlist of an existing cxine, then exit. 'title' is an optional title to be displayed while playing
-queue <url>
 : If no cxine is currently running, then start up and play <url>, else add it to the playlist of an existing cxine.
+queue <url> [title]
 : Enhanced version of queue that accepts a 'title' argument
-enqueue <url>
 : MPlayer compatible equivalent of '-queue'
-esc
 : Allow the 'escape' key to exit the app.
-win
 : <win id>       Window settings (see below)
-into
 : <win id>       Window to reparent into (xterm style option).
-wid
 : <win id>       Window to reparent into (mplayer style option).
-parent
 : <win id>       Window to reparent into (maximum clarity style).
-s <wid>x<high>
 : Window size. Default is 480x360.
-r
 : Load directories recursively, adding their contents to the playlist.
-esc
 : Allow the 'escape' key to exit the app.
-ao <ao name>
 : Audio output plugins list (default = alsa). This is a comma-seperated list of audio outputs to try, cxine will use the first one that works. For alsa  and oss different output devices can be specified by appending ':0', ':1' to select devices by number. For Jack, Sun and Pulse appending the device path/name instead of a number *may* work. If a list of devices is provided, then the 'a' key can be used at runtime to cycle through them.
-vo <vo name>
 : Video output plugin name (default = Xv).
-identify
 : Output machine-readable track information on stdout (mplayer feature).
-root
 : Draw on root window (equivalent to -win root).
-rootwin
 : Draw on root window (mplayer style option).
-fs
 : Play fullscreen (mplayer style option equivalent to '-win fullscreen').
-background <path>
 : Path to background image when not playing anything else. Needed for OSD to mix into.
-use-stdin
 : Read media stream from stdin
-bcast <port>
 : Broadcast to slave libxine players on <port>
-pause
 : Start paused.
-mute
 : Start muted.
-shuffle
 : Shuffle playlist.
-loop <n>
 : Repeat playback <n> times.
-show-playlist
 : Start with playlist displayed
-title <title>
 : Set title displayed for this track. This argument is positional and must preceede the track it names, so for example 'cxine -title 'title 1' track1.mp4 -title 'title 2' track2.mp4'
-image-time <ms>
 : Number of milliseconds to pause on an image for, until starting to play/display the next track.
-imagems <ms>
 : Number of milliseconds to pause on an image for, until starting to play/display the next track.
-stop_xscreensaver
 : Disable screensaver during playing, re-enable it on exit, or if playback paused.
-screensaver
 : Disable screensaver during playing, re-enable it on exit, or if playback paused.
+screensaver
 : Enable screensaver during playing (use this to turn off 'disable screensaver' if you saved the config).
-ss
 : Disable screensaver during playing, re-enable it on exit, or if playback paused.
-SS
 : Disable screensaver during playing, re-enable it on exit BUT NOT IF PLAYBACK PAUSED.
+ss
 : Enable screensaver during playing.
-persist
 : Don't exit when nothing left to do, wait for commands on stdin.
-quit
 : Exit when nothing left to do (use this if you saved -persist config and want to turn it off)
-slave
 : Mplayer compatible flag, equivalent to '-persist -startms 0'.
-cache <age>
 : Max age of items in the download cache. Number is in seconds, but can be postfixed with 'm' for minutes, 'h' for hours and 'd' for days. e.g. '-cache 10h' means 'cache for ten hours'
-input <path>
 : Path to pipe to read commands from (by default cxine creates a pipe in '~/.cxine/cxine.pipe'). This accepts mplayer compatible form '-input file:<path>'.
-cmd <cmd>
 : Tell cxine to send a command to an already running cxine. Run 'cxine -cmd help' or 'cxine --help cmd' to learn more..
-nowplay <path>
 : Path to pipe to write 'now playing' info to (cxine will create this pipe if it doesn't already exist).
-startms <millisecs>
 : Start playing at <millisecs>'.
-dvd-device <path>
 : Path to DVD device to use (default /dev/dvd)
-dvd-region <n>
 : DVD region num (if dvd player supports this), range 1-8
-dvd-lang <lang>
 : Default language for DVDs
-osd-show
 : Show On Screen Display.
-osd-hide
 : Hide On Screen Display'.
-osd <format>
 : Define format of OSD bar.
-slow
 : Play halfspeed.
-vslow
 : Play quarterspeed.
-loud
 : Play with raised volume.
-vloud
 : Play with maximum volume.
-stream
 : Don't download remote urls in playlists etc. This currently only works for 'http:' (not https:) urls. This allow streaming internet radio urls.
-webcast
 : Implies '-stream', treats playlists as webcast announcement files, only containing one item.
-prefix
 : Append a prefix to a media url. This is mostly used with playlists, where the playlist file just names files, and -prefix is used to point the the directory they're in.
-keygrab
 : Register keygrabs, a comma-separated list of keys. See '-help keygrabs' below.
-helpers <config>
 : Register list of helper apps.
+helpers <config>
 : Prepend to list of helper apps (these helpers will be tried first).
-prio <value>
 : Set process priority in range 0-39 (requires superuser capabilities).
-nice <value>
 : Set process priority with 'nice' semantics (requires superuser capabilities).
-ac <value>
 : Set audio compression. This boosts quiet sounds, lessening the volume range. value is a percent > 100 to multiply quiet sounds by.
-ap <plugins>
 : Comma separated list of audio post processing plugins.
-aplug <plugins>
 : Comma separated list of audio post processing plugins.
-no-video
 : Don't output video (for use in combination with -ap for visualization plugins).
-novideo
 : Don't output video (for use in combination with -ap for visualization plugins).
-bookmark
 : Remember where media left-off playing.
-no-bookmark
 : Don't remember where media left-off playing.
-save-config
 : Save current cxine setup.
-defaults
 : Reset to default config (this will wipe any settings prior on the command line).
-list-extn
 : Print to stdout a list of file extensions supported by this player.
-list-mime
 : Print to stdout a list of mime-types supported by this player.
-version
 : Show program version.
--version
 : Show program version.
-?
 : Show this help.
-help
 : Show this help.
--help
 : Show this help.
--help keys
 : Show keybindings.
--help keygrabs
 : Show help for keygrab config.
--help osd
 : Show help for OSD config.
--help slave
 : Show help for MPlayer-compatible slave mode.
--help plugins
 : List available plugins.



WINDOW SETTINGS 
===============

The -win argument accepts a window-type and an optional comma-separated list of extra settings. Window types are:

none
 : don't open an X11 window
normal
 : normal X11 window (is the default if no type specified)
root
 : output on desktop root window
fullscreen
 : fullscreen window
max
 : fullscreen window
`0x<win id>`
 : the hexidecimal (starting with 0x) id of a window to embed/reparent into
`<win id>` 
 : the decimal id of a window to embed/reparent into

Additional window settings are:

sticky
 : appear on all desktops
iconized
 : start with iconized/minimized window
iconic
 : start with iconized/minimized window
minimized
 : start with iconized/minimized window
min
 : start with iconized/minimized window
shaded
 : start with 'shaded' window (if supported by window manager)
ontop
 : keep above all other windows
above
 : keep above all other windows
below
 : keep below all other windows
stickontop
 : shorthand for sticky,ontop
stickabove
 : shorthand for sticky,ontop
stickbelow
 : shorthand for sticky,below

Examples:

```
  -win fullscreen,min         fullscreen window that's minimized at startup
  -win sticky                 normal window that appears on all desktops
  -win normal,shaded,below    normal window starts shaded and below other windows
  -win 0x1f2bc                embed into window with hexadecimal id 1f2bc
```

When embedding cxine into another application (e.g. a frontend) window ids can be given to '-win' either in hex (with a leading 0x) or in decimal.


BOOKMARKS
=========

By default cxine 'bookmarks' the position in a video if exit is requested, and restarts from that position if the media is played again. This can be disabled with '-no-bookmark' or  '-startms 0'. 


DVD PLAYBACK
============

DVDs can be played using the urls 'dvd://0' to use dvdnav menu screen, or 'dvd://1', 'dvd://2', etc to play a particular track on disk. The particular dvd device to read from can be set with '-dvd-device'.


DOWNLOADS AND REMOTE MEDIA STREAMS
==================================

CXine downloads media using helper apps. Default config will use curl, wget or Twighbright links, elinks, lynx, or snarf, depending on which are found in your path your path. Cxine should be able to accept ftp: ftps: sftp: and smb: urls via curl, but these have not been tested.

You can also use 'ssh:' (not sftp, this actually streams files over ssh) urls if ssh is in your path, although these have to be set up in your '.ssh/config' to auto-login. So, for example the url 'ssh://myhost/home/music/BinaryFinary.mp3' will log into 'myhost' and use ssh to pull the file '/home/music/BinaryFinary.mp3' provided that 'myhost' has been set up in your .config with an ssh key to log in with.

More information about helpers is available with 'cxine --help helpers'

The '-stream' option is intended for use with internet radio, and only works for http:// streams. If '-stream' is supplied then http:// urls will be streamed without being downloaded to the cache directory


XINE BROADCAST
==============

Cxine supports libxine-style broadcast. If the '-bcast' option is used to specify a port then any libxine player (xine, cxine, oxine etc) should be able to connect to it using the url 'slave://<address>:<port> and mirror it's output.


SAVE CONFIGURATION
==================

If '-save-config' is given then cxine will remember the following settings if they are supplied:

```
		-vo, -ao, -ac, -ap, -prefix, -keygrab, -persist/-quit, -bookmark/-no-bookmark, -screensaver/+screensaver, -show-osd/-hide-osd, -background, -input, -cache, -nowplay,  -dvd-device, -dvd-region, -dvd-lang, -helpers
```

-defaults resets the config to default. It will also reset any settings prior to it  on the command-line, so it's best to pass it as the first option. You can set configs to defaults, make some changes, and then save, like this:

```
		cxine -defaults -background myimage.jpg -ao alsa -keygrab media -save
```


On Screen Displays
------------------

CXine supports on-screen-displays at the top and bottom of the screen. These are defined using the `-osd` switch like so:

cxine -osd `top,%t now playing: %T` -osd `bottom,%tP%% %ts/%tl`

The argument of the -osd option is a pair of comma-seperated strings. The first is `top` or `bottom` indicating which area of the screen the OSD should be displayed in. The second is the string to display, with the following printf-style `%` substitutions supported:

```
%%     output '%'
%B      output current stream bitrate
%w      output current stream video width
%h      output current stream video height
%o      output current stream AV offset
%A      output artist of current track
%T      output title of current track
%C      output comment of current track
%v      output audio volume (0-100)
%av     output audio volume (0-100)
%ac     output audio file FourCC
%ab     output audio bitrate
%as     output audio samplerate
%aw     output audio width (compression level)
%Ls     output size of playlist (number of queued tracks)
%Ls     output curr track in playlist being played
%ma     output artist for current track
%mA     output album for current track
%mt     output title of current track
%mT     output title of current track
%mY     output year for current track
%mG     output genre for current track
%mR     output copyright for current track
%mc     output DVD chapter number
%mC     output number of chapters on DVD
%ts     output number of seconds into track
%tS     output seconds into and duration of track as <position>/<track length>
%tl     output length of current track in seconds
%tP     output percent into current track
%tt     output current time in form HH:MM
%tT     output current time in form HH:MM:SS
%td     output current date in form YY/mm/dd
%tD     output current date in form YYYY/mm/dd
%tN     output current date and time in form YYYY/mm/dd HH:MM:SS
%tw     output position in track as HH:MM:SS
%tW     output length of track as HH:MM:SS

```

Onscreen displays only work if there's a video stream for them to mix into. Thus, to have an OSD when playing audio files you should either use the -background option to set a default background image, or use one of the audio post-processing visualizations, or else supply cxine with a .jpeg or .png image as the first `track` to play, and use the `-image-time` or `-imagems` options to cause the next track to start playing after the image is displayed. The image will persist and be the `video` stream for the OSD to mix into.


Playlist OSD
------------

Pressing 'p' brings up the Playlist On Screen Display. This is a simple menu whose cursor is controlled with the arrow keys on the keyboard. Putting the cursor on a particular item and pressing 'enter' will switch playback to that item. Pressing 'u' or 'd' when an item has the cursor on it will move the item up and down in the playlist. Pressing 'delete' or 'backspace' will delete an item from the playlist. Finally pressing 'p' again will dismiss the Playlist OSD.

Load Files OSD
------------

Pressing 'l' brings up the Load Files On Screen Display. This is a simple menu whose cursor is controlled with the arrow keys on the keyboard. Putting the cursor on a particular item and pressing 'enter' will either enter a directory, or add a file to the playlist. Pressing 'delete' or 'backspace' go up one directory level. Pressing 'l' again will dismiss this menu.


Keybindings
-----------

CXine supports the following keys

```
<escape>        exit app (requires -esc command-line option)
<spacebar>      pause playback
<pause>         pause playback
<home>          seek to start of playback
<end>           seek to near end of playback
<enter>         next item in playlist
<left arrow>    seek back 10 seconds
                shift: previous item in playlist
                ctrl: step back (not frame accurate like ctrl-right)
<right arrow>   seek forward 10 seconds
                shift: next item in playlist
                ctrl: single-step 1-frame forwards
<page up>       forward dvd chapter 
<page down>     back dvd chapter 
+ (or '=')      increase volume
                shift: increase audio compression
- (or '_')      decrease volume
                shift: decrease audio compression
l               display 'load files' menu
m               mute
o               toggle online display
p               display playlist menu
f               toggle 'fast' playback (4*speed, no sound)
s               toggle 'slow' playback (1/4 speed, no sound)
<tab>           toggle between 'raised' and 'lowered' window modes
                ctrl: toggle 'shaded' window mode (if supported by window-manager)
<delete>        reset window to 'normal' mode. (turns off 'above', 'below' and 'shaded' modes)
.               reset window to 'normal' mode. (turns off 'above', 'below' and 'shaded' modes)
<               prev item in playlist
>               next item in playlist
1,2,3...        seek to n*10 percent (so 5 seeks to 50% of stream)


The '+' and '-' keys represent a departure from mplayer. On the keypad they work as expected, with the 'shift' modifier alowing change of audio compression. However, on UK keyboards you have to press shift to get '+' on the non-keypad part of the keyboard. This creates confusion. Hence on the normal keyboard '=' and '-' change volume up and down, and their shifted versions '_' and '+' alter audio compression.

The following keys on 'internet' or 'media' keyboards are supported
<play>
<stop>
<pause playback>
<next>
<previous>
<volume up>     with 'shift' causes a larger increase
<volume down>   with 'shift' causes a larger decrease
<volume mute>

```


Keygrabs
--------

CXine can grab keys for its exclusive use, meaning that that those keystrokes will be sent to it regardless of what window currently has input focus. Keygrabs are supplied to the program in a comma-seperated list after the `-keygrab` option. Recognized key names are either alphanumeric characters and punctuation, or the following names:

```
up, down, left, right       arrow keys
pgup, pgdn, home, end       page-up, page-down, home and end keys
pgup, pgdn, home, end       page-up, page-down, home and end keys
ins, del                    insert and delete keys
vup, vdown, mute            volume up, down and mute keys found on 'internet' keyboards
stop, play, next, prev      playback keys found on 'internet' keyboards
vpause                      media pause key found on 'internet' keyboards
```

CXine also supports the following 'group' names that grab groups of keys:
```
arrow                       left right up down
volume                      vup, vdown, mute
page                        pgup, pgdn
nav                         left, right, up, down, pgup, pgdn, home, end
media                       vup, vdown, mute, stop, play, prev, next, vpause
keypad                      up, down, left, right pgup, pgdn, home, end, +, -
```

All key and group names can have a modifier prepended. Available modifiers are `shift-`, `cntrl-` and `alt-`. If a key-modifier pair isn`t bound to an action, then cxine will treat the key as though it had no modifier. Thus `alt-o` can be bound to turn OSD display on-and-off without depriving other programs of use of the `o` key.

