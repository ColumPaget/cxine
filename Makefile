CFLAGS=-g -O2 -I/include -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_LIBM=1 -DHAVE_LIBXINE=1 -DHAVE_XINE=1 -DHAVE_LIBX11=1 -DHAVE_X11=1
LDFLAGS= -L/lib
LIBS=-lX11 -lxine -lm 
prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
mandir=${prefix}/share/man

OBJ=string_list.o common.o config.o playback_control.o control_protocol.o command_line.o playlist.o help.o keypress.o bookmarks.o tracklist.o now_playing.o playlist_files.o download.o osd.o download_osd.o playlist_osd.o load_files_osd.o media_info_osd.o help_osd.o plugins.o audio_drivers.o splashscreen.o stdin_fd.o X11.o 

all: $(OBJ)
	$(CC) -o cxine -Wall $(CFLAGS) $(LDFLAGS) $(OBJ) cxine.c $(LIBS) 

string_list.o: string_list.h string_list.c
	$(CC) $(CFLAGS) -c string_list.c

common.o: common.h common.c
	$(CC) $(CFLAGS) -c common.c

config.o: config.h config.c
	$(CC) $(CFLAGS) -c config.c

control_protocol.o: control_protocol.h control_protocol.c
	$(CC) $(CFLAGS) -c control_protocol.c

command_line.o: command_line.h command_line.c
	$(CC) $(CFLAGS) -c command_line.c

playlist.o: playlist.h playlist.c
	$(CC) $(CFLAGS) -c playlist.c

help.o: help.h help.c
	$(CC) $(CFLAGS) -c help.c

playback_control.o: playback_control.h playback_control.c common.h
	$(CC) $(CFLAGS) -c playback_control.c

now_playing.o: now_playing.h now_playing.c common.h
	$(CC) $(CFLAGS) -c now_playing.c

bookmarks.o: bookmarks.h bookmarks.c common.h
	$(CC) $(CFLAGS) -c bookmarks.c

tracklist.o: tracklist.h tracklist.c common.h
	$(CC) $(CFLAGS) -c tracklist.c

playlist_files.o: playlist_files.h playlist_files.c common.h
	$(CC) $(CFLAGS) -c playlist_files.c

download.o: download.h download.c common.h
	$(CC) $(CFLAGS) -c download.c


stdin_fd.o: stdin_fd.h stdin_fd.c common.h
	$(CC) $(CFLAGS) -c stdin_fd.c

keypress.o: keypress.h keypress.c common.h
	$(CC) $(CFLAGS) -c keypress.c

osd.o: osd.h osd.c common.h
	$(CC) $(CFLAGS) -c osd.c

download_osd.o: download_osd.h download_osd.c common.h
	$(CC) $(CFLAGS) -c download_osd.c

playlist_osd.o: playlist_osd.h playlist_osd.c common.h
	$(CC) $(CFLAGS) -c playlist_osd.c

load_files_osd.o: load_files_osd.h load_files_osd.c common.h
	$(CC) $(CFLAGS) -c load_files_osd.c

media_info_osd.o: media_info_osd.h media_info_osd.c common.h
	$(CC) $(CFLAGS) -c media_info_osd.c

help_osd.o: help_osd.h help_osd.c common.h
	$(CC) $(CFLAGS) -c help_osd.c

plugins.o: plugins.h plugins.c common.h
	$(CC) $(CFLAGS) -c plugins.c

audio_drivers.o: audio_drivers.h audio_drivers.c common.h
	$(CC) $(CFLAGS) -c audio_drivers.c

splashscreen.o: splashscreen.h splashscreen.c common.h
	$(CC) $(CFLAGS) -c splashscreen.c

X11.o: X11.h X11.c common.h
	$(CC) $(CFLAGS)  -c X11.c

clean:
	rm -f *.o *.orig cxine 

install:
	-mkdir -p $(DESTDIR)/$(bindir)
	cp cxine $(DESTDIR)/$(bindir)
	-mkdir -p $(DESTDIR)/$(mandir)/man1
	cp cxine.1 $(DESTDIR)/$(mandir)/man1

test:
	-echo "no tests"
