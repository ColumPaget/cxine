CFLAGS=-g -O2 -I/usr/X11R7/include
LDFLAGS= -L/usr/X11R7/lib
LIBS=-lXss -lX11 -lxine -lm 
prefix=/usr/local

OBJ=common.o config.o playback_control.o control_protocol.o command_line.o playlist.o help.o keypress.o bookmarks.o now_playing.o playlist_files.o download.o osd.o plugins.o X11.o 

all: $(OBJ)
	$(CC) -Wall $(CFLAGS) $(LIBS) $(LDFLAGS) $(OBJ) cxine.c -o cxine

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

playlist_files.o: playlist_files.h playlist_files.c common.h
	$(CC) $(CFLAGS) -c playlist_files.c

download.o: download.h download.c common.h
	$(CC) $(CFLAGS) -c download.c

keypress.o: keypress.h keypress.c common.h
	$(CC) $(CFLAGS) -c keypress.c

osd.o: osd.h osd.c common.h
	$(CC) $(CFLAGS) -c osd.c

plugins.o: plugins.h plugins.c common.h
	$(CC) $(CFLAGS) -c plugins.c


X11.o: X11.h X11.c common.h
	$(CC) $(CFLAGS)  -c X11.c

clean:
	rm -f *.o cxine

install:
	-mkdir -p $(prefix)/bin
	cp cxine $(prefix)/bin
