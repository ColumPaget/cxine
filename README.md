CXine - A mostly mplayer compatible libxine media player
--------------------------------------------------------

CXine is a basic, keyboard-driven, X11, lib-xine based media player that is intended to be used as a keyboard-driven standalone app, or embedded in a frontend. It supports a subset of the mplayer 'slave mode' command protocol. It supports user-configurable on screen displays; embedding into another window; sticky, shaded and stayontop windows; drawing on the root window; fullscreen playback, keygrabs and 'bookmarking' (remembering position in a media item). Its only dependancies are libxine, libX11, libc and libm.

At current CXine is completely keyboard-driven, with no mouse control at all. This may change in future.

Author
------
cxine is (C) 2018 Colum Paget. It is released under the GPLv3 so you may do anything with them that the GPL allows.

Email: colums.projects@gmail.com


Usage
-----
```
	cxine [options] [url]
```

Options
-------

```
  -add <url> [title]      Add <url> to the playlist of an existing cxine, then exit. 'title' is an optional title to be displayed while playing
  -queue <url>            If no cxine is currently running, then start up and play <url>, else add it to the playlist of an existing cxine.
  +queue <url> [title]    Enhanced version of queue that accepts a 'title' argument
  -enqueue <url>          MPlayer compatible equivalent of '-queue'
  -esc                    Allow the 'escape' key to exit the app.
  -win     <win id>       Window settings (see below)
  -into    <win id>       Window to reparent into (xterm style option).
  -wid     <win id>       Window to reparent into (mplayer style option).
  -parent  <win id>       Window to reparent into (maximum clarity style).
  -s <wid>x<high>         Window size. Default is 480x360.
  -esc                    Allow the 'escape' key to exit the app.
  -ao <ao name>           Audio output plugin name (default = alsa). For alsa  and oss different output devices can be selected by appending ':0', ':1' to select devices by number. For Jack, Sun and Pulse appending the device path/name instead of a number *may* work.
  -vo <vo name>           Video output plugin name (default = Xv).
  -identify               Output machine-readable track information on stdout (mplayer feature).
  -root                   Draw on root window (equivalent to -win root).
  -rootwin                Draw on root window (mplayer style option).
  -fs                     Play fullscreen (mplayer style option equivalent to '-win fullscreen').
  -background <path>      Path to background image when not playing anything else. Needed for OSD to mix into.
  -use-stdin              Read media stream from stdin (see 'Reading from Stdin' below)
  +stdin-ctrl             Read keypresses from stdin (see 'Reading from Stdin' below)
  -stdin-ctrl             Dont read keypresses from stdin
  -C                      Read keypresses from stdin (see 'Reading from Stdin' below)
  +C                      Dont read keypresses from stdin
  -bcast <port>           Specify port that slave libxine players can connect to to recieve broadcasted stream.
  -noauto                 No autoplay, items must be selected from the playlist.
  -pause                  Start paused.
  -mute                   Start muted.
  -shuffle                Shuffle playlist.
  -loop <n>               Repeat playback <n> times.
  -title <title>          Set title displayed for this track. This argument is positional and must preceede the track it names, so for example: "cxine -title 'title 1' track1.mp4 -title 'title 2' track2.mp4".
  -image-time <ms>        Number of milliseconds to pause on an image for, until starting to play/display the next track.
  -imagems <ms>           Number of milliseconds to pause on an image for, until starting to play/display the next track.
  -stop_xscreensaver      Disable screensaver during playing, re-enable it on exit, or if playback paused.
  -screensaver            Disable screensaver during playing, re-enable it on exit, or if playback paused.
  +screensaver            Enable screensaver during playing (use this to turn off 'disable screensaver' if you saved the config).
  -ss                     Disable screensaver during playing, re-enable it on exit, or if playback paused.
  -SS                     Disable screensaver during playing, re-enable it on exit BUT NOT IF PLAYBACK PAUSED.
  +ss                     Enable screensaver during playing.
  -idle                   Don't exit when nothing left to do, wait for commands on stdin.
  -persist                Don't exit when nothing left to do, wait for commands on stdin.
  -quit                   Exit when nothing left to do (use this if you saved -persist config and want to turn it off)
  -slave                  Mplayer compatible flag, equivalent to '-persist -startms 0'.
  -cache <age>            Max age of items in the download cache. Number is in seconds, but can be postfixed with 'm' for minutes, 'h' for hours and 'd' for days. e.g. '-cache 10h' means 'cache for ten hours'

  -input <path>           Path to pipe to read commands from (by default cxine creates a pipe in '~/.cxine/cxine.pipe'). This accepts mplayer compatible form '-input file:<path>'.
  -nowplay <path>         Path to pipe to write 'now playing' info to (cxine will create this pipe if it doesn't already exist).
  -startms <millisecs>    Start playing at <millisecs>'.
  -dvd-device <path>      Path to DVD device to use (default /dev/dvd)
  -dvd-region <n>         DVD region num (if dvd player supports this), range 1-8
  -dvd-lang <lang>        Default language for DVDs
  -osd-show               Show On Screen Display.
  -osd-hide               Hide On Screen Display'.
  -osd <format>           Define format of OSD bar.
  -slow                   Play halfspeed.
  -vslow                  Play quarterspeed.
  -loud                   Play with raised volume.
  -vloud                  Play with maximum volume.
  -stream                 Don't download remote urls in playlists etc. This currently only works for 'http:' (not https:) urls. This allow streaming internet radio urls.
  -stream                 Don't download remote urls in playlists etc. This currently only works for 'http:' (not https:) urls. This allow streaming internet radio urls.
  -webcast                Implies '-stream', treats playlists as webcast announcement files, only containing one item.
  -playlist               Treat paths on command-line as playlist files.
  -podcast                Implies -playlist -noauto and -show-playlist.
  -prefix                 Append a prefix to a media url. This is mostly used with playlists, where the playlist file just names files, and -prefix is used to point the the directory they're in.
  -keygrab                Register keygrabs, a comma-separated list of keys. See '-help keygrabs' below.
  -helpers <config>       Register list of helper apps.
  +helpers <config>       Prepend to list of helper apps (these helpers will be tried first).
  -prio <value>           Set process priority in range 0-39 (requires superuser capabilities).
  -nice <value>           Set process priority with 'nice' semantics (requires superuser capabilities).
  -ac <value>             Set audio compression. This boosts quiet sounds, lessening the volume range. value is a percent > 100 to multiply quiet sounds by.
  -ap <plugins>           Comma separated list of audio post processing plugins.
  -aplug <plugins>        Comma separated list of audio post processing plugins.
  -no-video               Don't output video (for use in combination with -ap for visualization plugins).
  -novideo                Don't output video (for use in combination with -ap for visualization plugins).
  -bookmark               Remember where media left-off playing.
  -no-bookmark            Don't remember where media left-off playing.
  -save-config            Save current cxine setup.
  -defaults               Reset to default config (this will wipe any settings prior on the command line).
  -?                      Show this help.
  -help                   Show this help.
  --help                  Show this help.
  --help keys             Show keybindings.
  --help keygrabs         Show help for keygrab config.
  --help osd              Show help for OSD config.
  --help slave            Show help for MPlayer-compatible slave mode.
  --help plugins          List available plugins.
```


Reading From Stdin
------------------

Reading media from stdin can be done using one of the following command lines:


```
cxine -use-stdin
cxine -
cxine stdin:
cxine stdin://
```

However, there are some issues/caveats with this system.

The xine On Screen Display system requires something to be displayed so that it can 'mix' the onscreen text into the video. Thus cxine provides a default 'background image' (splashscreen) that can serve this purpose. The method it uses to do this is by pipeing a PNG image into it's own stdin, and then telling the xine library to read from stdin. Unfortunately this approach can't be used if we actually want to read media from stdin. Thus, if cxine detects a request to read from stdin, it won't inject the 'splashscreen'. This is fine for any media that has video output, but with audio-only media, the On Screen Display will not work. Solutions to this are:

1) use an audio postprocessor that outputs video. For example:

```
cxine -ap goom stdin://
```

2) specify a different background image with  the '-background' option.

```
cxine -background /tmp/myimage.png stdin:// 
```

This method doesn't mess with stdin.

3) specify an image as the first media to be played, and use '-image-ms' to advance to the next media url, like so:

```
cxine -image-ms 1 /tmp/myimage.png stdin:// 
```

These same caveats apply to situations where you want to read control keystrokes from stdin, and if doing that you cannot supply media on stdin.


Window Types
------------

The -win argument accepts a window-type and an optional comma-separated list of extra settings. Window types are:

```
  none           don't open an X11 window
  normal         normal X11 window (is the default if no type specified)
  root           output on desktop root window
  fullscreen     fullscreen window
  max            fullscreen window
  0x<win id>     the hexidecimal (starting with 0x) id of a window to embed/reparent into
  <win id>       the decimal id of a window to embed/reparent into
```

Additional window settings are:
```
  sticky         appear on all desktops
  iconized       start with iconized/minimized window
  iconic         start with iconized/minimized window
  minimized      start with iconized/minimized window
  min            start with iconized/minimized window
  shaded         start with 'shaded' window (if supported by window manager)
  ontop          keep above all other windows
  above          keep above all other windows
  below          keep below all other windows
  stickontop     shorthand for sticky,ontop
  stickabove     shorthand for sticky,ontop
  stickbelow     shorthand for sticky,below
```

Examples:

```
  -win fullscreen,min         fullscreen window that's minimized at startup
  -win sticky                 normal window that appears on all desktops
  -win normal,shaded,below    normal window starts shaded and below other windows
  -win 0x1f2bc                embed into window with hexadecimal id 1f2bc
When embedding cxine into another application (e.g. a frontend) window ids can be given to '-win' either in hex (with a leading 0x) or in decimal.
```



Media Types
-----------

DVDs can be played using the urls `dvd://0` to use dvdnav menu screen (as with a standalone DVD player), or `dvd://1`, `dvd://2`, etc to play a particular track on disk. 

Data from stdin can be read with a url of the form `stdin://`. 


Bookmarks
---------

By default cxine 'bookmarks' the position in a video if exit is requested, and restarts from that position if the media is played again. This may not work for all media types, but it should for .mp4 videos and the like.

Bookmark playback can be disabled with `-no-bookmarks` or the `-startms 0` option, which tells cxine to start 0 milliseconds into the media (i.e. at the beginning).


Escape to exit, or not
----------------------

As a Vim user my pet peeve with mplayer is having an mplayer window popup after a download completes, while I'm in the middle of programming, and just as I hit 'escape' to switch from insert-mode to command-mode in vim. The keystroke winds up going to mplayer causing it to shutdown without playing the media. Thus cxine doesn't honor the escape key by default, but will if supplied the `-esc` option at startup. 


Control Pipes
-------------

By default cxine creates a pipe in `~/.cxine/cxine.pipe` and is always ready to read 'slave mode' commands from there or from stdin (unlike mplayer that only does that when the `-slave` option is supplied). The path to this pipe can be changed with the `-input` option to point to a different pipe. If the pipe does not exist, cxine will create it, otherwise it will just attach ot it in reading mode. The `-input` option can take its argument raw, as `-input <path to pipe>` or in `MPlayer compatible format` which is `-input file:<path to pipe>`.

The `-persist` option is normally needed in conjunction with the `-input` option. `-persist` tells cxine to stay running even if it has nothing to play (otherwise it will exit). The `loadfile` slave-mode command can then be used to load up media to be played.

The mplayer-compatible `-slave` option is a combination of `-persist` and `-startms 0`, causing CXine to stay running and to start all playback from the beginning of the media.

CXine also supports a `now playing` pipe specified with `-nowplay <path>`. If the `-now play` option is provided, then CXine will write information about the currently playing media to the pipe. If the pipe does not exist, cxine will create it.


Add URL to Playlist of an Existing CXine
---------------------------------------

You can add items to the playlist of a running CXine (which is perhaps running in `-persist` mode) with the `-add` and `-queue` options. Start up another cxine like so:

```
cxine -add <url>
```

This will pass the url to a running cxine using the default cxine control pipe. The `-queue` option works the same way, except if a cxine isn't already running then it starts one up.

You can also copy-paste a URL into the cxine window.


X11 Properties
--------------

CXine sets X11 properties on its main window with the names 'CXINE:Filename', 'CXINE:Artist', 'CXINE:Title', 'CXINE:Album', 'CXINE:Track', 'CXINE:Year' and 'CXINE:Genre' that hold information about the currently playing media. It also sets the properties 'CXINE:WIN' and 'CXINE:PID' on the root window so that applications can find it's process and window.

CXine also outputs its window id on stdout at startup, in a message of the form `win_id: 0x2400001`. This is intended to allow a player application to reparent buttons into the cxine window itself.


Audio Post Plugins
------------------

CXine can add postprocessing plugins to an audio stream. This is done with the '-ap' option. Available audio plugins are:

```
        goom            - Psychadelic audio visualization.
        oscope          - Oscilloscope audio visualization.
        fftscope        - FFT Scope audio visualization.
        fftgraph        - FFT Graph audio visualization. 
        tdaudioanalyzer - Time Domain Audio Analyzer Visualisation
        volnorm         - Normalize volume
        noise           - Adds noise
```


Audio Compression
-----------------

the -ac switch is used to set the audio compression level. This feature in used on media where sound effects (e.g. gunshots, car chase noises) are much louder than the actor's voices. The `-ac` switch takes an argument which is a percentage amount to boost quiet sounds by. Below 100% this does nothing, but the argument `-ac 150` will boost quiet sounds to a maximum of 150% their default value, while boosting louder sounds less or not at all.


Image Slideshows and Images for Audio Tracks
--------------------------------------------

The `-image-time` or `-imagems` options allow setting a number of milliseconds that cxine will 'pause' on an image before loading the next media item. This can be used to either create slideshows of images, or to supply an image to be displayed during the playing of audio tracks by setting the number of milliseconds to a low value. e.g.

```
cxine -image-time 2 /home/images/img1.jpg /home/mp3/track1.mp3 /home/images/img2.png /home/mp3/track2.mp3
```

The `-background` option allows a default image to be displayed when nothing is playing. This image is redisplayed between every track, and thus will be the background for any tracks that don't have video output.


Downloads
---------

CXine downloads media using helper apps. Either Curl, Wget or Twighbright Links need to be in your path for https/http. CXine should also be able to accept ftp: ftps: sftp: and smb: urls via curl, but these have not been tested. You can also use 'ssh:' (not sftp, this actually streams files over ssh) urls if ssh is in your path, although these have to be set up in your '.ssh/config' to auto-login. 
So, for example the url 

```
'ssh://myhost/home/music/BinaryFinary.mp3' 
```

will log into 'myhost' and use ssh to pull the file '/home/music/BinaryFinary.mp3' provided that 'myhost' has been set up in your .config with an ssh key to log in with.

Broadcast
---------

Cxine supports libxine-style broadcast. If the '-bcast' option is used to specify a port then any libxine player (xine, cxine, oxine etc) should be able to connect to it using the url "slave://<address>:<port>" and mirror its output.

Streaming
---------

The '-stream' option is intended for use with internet radio, and only works for http:// streams. If '-stream' is supplied then http:// urls will be streamed without being downloaded to the cache directory

Podcasts
--------

The '-podcast' option is intended for use with RSS podcast urls. This feature was added in v6.0 and is somewhat experimental. '-podcast' implies '-noauto' and '-playlist' so that all urls passed on the command-line will be considered to be playlists, and tracks/episodes will not be auto-downloaded and played.



On Screen Displays
------------------

CXine supports on-screen-displays at the top and bottom of the screen. These are defined using the `-osd` switch like so:

cxine -osd `top,%t now playing: %T` -osd `bottom,%tP%% %ts/%tl`

There's also a console-status bar that is output on stdout can be modified with:

cxine -osd `console,%t %tP%% %ts/%tl`

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
%a4     output audio file FourCC
%ac     output audio channels (mono, stereo, etc)
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
%v4     output video file FourCC
%vc     output video channels
%vC     output video codec
%vb     output video bitrate
%vs     output video samplerate
%vw     output video width 
%vh     output video height

```

Onscreen displays only work if there`s a video stream for them to mix into. Thus, to have an OSD when playing audio files you should either use the -background option to set a default background image, or use one of the audio post-processing visualizations, or else supply cxine with a .jpeg or .png image as the first `track` to play, and use the `-image-time` or `-imagems` options to cause the next track to start playing after the image is displayed. The image will persist and be the `video` stream for the OSD to mix into.


Playlist OSD
------------

Pressing 'p' brings up the Playlist On Screen Display. This is a simple menu whose cursor is controlled with the arrow keys on the keyboard. Putting the cursor on a particular item and pressing 'enter' will switch playback to that item. Pressing 'u' or 'd' when an item has the cursor on it will move the item up and down in the playlist. Pressing 'delete' or 'backspace' will delete an item from the playlist. Finally pressing 'p' again will dismiss the Playlist OSD.

Load Files OSD
--------------

Pressing 'l' brings up the Load Files On Screen Display. This is a simple menu whose cursor is controlled with the arrow keys on the keyboard. Putting the cursor on a particular item and pressing 'enter' will either enter a directory, or add a file to the playlist. Pressing 'delete' or 'backspace' go up one directory level. Pressing 'l' again will dismiss this menu.


Tracklists
----------

Some media files contain many tracks, but lack any tracklisting info. This can be provided with an external 'tracklist file'. These files are made of text lines in the format:

```
mm:ss name
```

Where 'mm' is minutes into the media, and 'ss' is seconds, and 'name' is the track name or whatever else is to be displayed when this point is reached.

By default cxine looks for tracklist files at '$(mrl).tracklist', where '$(mrl)' is the media url or file path of the media file. e.g. a file at '/home/music/binary_finary.mp3' will cause a lookup for a tracklist file at '/home/music/binary_finary.mp3.tracklist'.

The tracklist path can be changed using the '-tl' or '-tracklist' command-line switches. e.g. 'cxine https://myserver/music/mixtape.mp3 -tracklist /home/tracklists/$(name).tracklist'.

In addition to '$(mrl)' there are also the variables '$(path)' and $(name). 'path' is the none-network part of a url. e.g., in 'https://myhost/music/rock.mp3' the path would be '/music/rock.mp3'. 'name' is the filename, so in 'https://myhost/music/rock.mp3' it would be 'rock.mp3'.



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
o               toggle online status display
i               toggle media info display
j               jump to playlist position (currently only works in console mode)
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
```

The '+' and '-' keys represent a departure from mplayer. On the keypad they work as expected, with the 'shift' modifier alowing change of audio compression. However, on UK keyboards you have to press shift to get '+' on the non-keypad part of the keyboard. This creates confusion. Hence on the normal keyboard '=' and '-' change volume up and down, and their shifted versions '_' and '+' alter audio compression.

When in console mode (no X11 window) the 'p' key prints out the current playlist, rather than presenting a menu. To allow movement through the playlist the 'j' key triggers a query into which either a number can by typed, or a shell-style pattern. The playlist will jump to either the track at the numbered position, or to the next track whose title or filename matches the pattern.


The following keys on 'internet' or 'media' keyboards are supported

```
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

Command Mode
------------

The -cmd option sends commands to another, already running, cxine. Many of the commands toggle between two states, like raised/unraised or paused/unpaused. Available commands are:

```
pause     pause/unpause playback
stop      stop playback
mute      mute/unmute audio
next      next track
prev      previous track
shade     shade/unshade window
mini      minimize/restore window
icon      minimize/restore window
minimize  minimize/restore window
lower     lower window below others
raise     raise window above others
zcycle    cycle between raised, normal, and lowered window
```


Saving config
-------------

If '-save-config' is given then cxine will remember the following settings if they are supplied:
```
		-vo, -ao, -ac, -ap, -prefix, -keygrab, -persist/-quit, -bookmark/-no-bookmark, -screensaver/+screensaver, -show-osd/-hide-osd, -background, -input, -nowplay,  -dvd-device, -dvd-region, -dvd-lang, -helpers/+helpers
```

`-defaults` resets the config to default. It will also reset any settings prior to it  on the command-line, so it's best to pass it as the first option. You can set configs to defaults, make some changes, and then save, like this:
```
		cxine -defaults -background myimage.jpg -ao alsa -keygrab media -save
```
