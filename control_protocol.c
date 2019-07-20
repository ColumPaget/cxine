#include "control_protocol.h"
#include "playback_control.h"
#include "playlist.h"

int ControlPipeOpen(int mode)
{
    char *Tempstr=NULL;

//0 is stdin, control_pipe can't be 0
    if (Config->control_pipe > 0) return(Config->control_pipe);

    if (StrLen(Config->control_pipe_path)) Tempstr=rstrcpy(Tempstr, Config->control_pipe_path);
    else
    {
        Tempstr=rstrcpy(Tempstr, xine_get_homedir());
        Tempstr=rstrcat(Tempstr, "/.xine/cxine.pipe");
    }

    if ( (access(Tempstr, F_OK)==0) || (mkfifo(Tempstr, 0700)==0) )
    {
        Config->control_pipe=open(Tempstr, mode);
    }

    destroy(Tempstr);

    return(Config->control_pipe);
}


int ControlHandleInput(int fd, xine_stream_t *stream)
{
    char *Tempstr=NULL, *Cmd=NULL, *Token=NULL, *URL=NULL, *Title=NULL;
    const char *ptr;
    int result, pos_msecs, len_msecs, pos, val;
    int RetVal=EVENT_NONE;

    Token=(char *) calloc(1, 256);
    result=read(fd, Token, 255);
    while (result > 0)
    {
        Token[result]='\0';
        Tempstr=rstrcat(Tempstr, Token);
        if (strchr(Token, '\n')) break;
        result=read(fd, Token, 255);
    }

//fd is closed at the other end. close it our end,
    if (result > 0)
    {
        xine_chomp(Tempstr);

        ptr=rstrtok(Tempstr, " 	",&Cmd);
        if (StrLen(Cmd))
        {
            switch (*Cmd)
            {
            case 'a':
                if (strcasecmp(Cmd, "add")==0)
                {
										PlaylistParseEntry(ptr, &URL, &Token, &Title);
                    PlaylistAdd(Config->playlist, URL, Token, Title);
                }
                break;

            case 'd':
                if (strcasecmp(Cmd, "dvdnav")==0)
                {
                    if (strcasecmp(ptr, "up")==0) CXineEventSend(Config, XINE_EVENT_INPUT_UP);
                    else if (strcasecmp(ptr, "down")==0) CXineEventSend(Config, XINE_EVENT_INPUT_DOWN);
                    else if (strcasecmp(ptr, "left")==0) CXineEventSend(Config, XINE_EVENT_INPUT_LEFT);
                    else if (strcasecmp(ptr, "right")==0) CXineEventSend(Config, XINE_EVENT_INPUT_RIGHT);
                    else if (strcasecmp(ptr, "select")==0) CXineEventSend(Config, XINE_EVENT_INPUT_SELECT);
                    //else if (strcasecmp(ptr, "menu")==0) CXineEventSend(Config, XINE_EVENT_INPUT_MENU);
                    else if (strcasecmp(ptr, "prev")==0) CXineEventSend(Config, XINE_EVENT_INPUT_PREVIOUS);
                }
                break;

            case 'g':
                if (strcasecmp(Cmd, "get_audio_bitrate")==0)
                {

                    val=xine_get_stream_info(stream, XINE_STREAM_INFO_BITRATE);
                    if (val==0) val=xine_get_stream_info(stream, XINE_STREAM_INFO_AUDIO_BITRATE);
                    printf("ANS_AUDIO_BITRATE='%d kbps'\n", val / 1000);
                }
                else if (strcasecmp(Cmd, "get_audio_codec")==0) printf("ANS_AUDIO_CODEC='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_AUDIOCODEC));
                else if (strcasecmp(Cmd, "get_video_codec")==0) printf("ANS_VIDEO_CODEC='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_VIDEOCODEC));
                else if (strcasecmp(Cmd, "get_meta_album")==0) printf("ANS_META_ALBUM='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_ALBUM));
                else if (strcasecmp(Cmd, "get_meta_title")==0)
                {
                    Token=PlaylistCurrTitle(Token);
                    printf("ANS_META_TITLE='%s'\n", Token);
                }
                else if (strcasecmp(Cmd, "get_meta_artist")==0) printf("ANS_META_ARTIST='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_ARTIST));
                else if (strcasecmp(Cmd, "get_meta_genre")==0) printf("ANS_META_GENRE='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_GENRE));
                else if (strcasecmp(Cmd, "get_meta_year")==0) printf("ANS_META_YEAR='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_YEAR));
                else if (strcasecmp(Cmd, "get_meta_comment")==0) printf("ANS_META_COMMENT='%s'\n", xine_get_meta_info(stream, XINE_META_INFO_COMMENT));
                else if (strcasecmp(Cmd, "get_video_resolution")==0) printf("ANS_VIDEO_RESOLUTION='%d x %d'\n", xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_WIDTH), xine_get_stream_info(stream, XINE_STREAM_INFO_VIDEO_HEIGHT));
                else if (strcasecmp(Cmd, "get_file_name")==0) printf("%s\n", ptr=StringListCurr(Config->playlist));
                else if (strcasecmp(Cmd, "get_percent_pos")==0)
                {
                    xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
                    printf("ANS_PERCENT_POSITION=%d\n", pos * 100 / 65535);
                }
                else if (strcasecmp(Cmd, "get_time_pos")==0)
                {
                    xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
                    printf("ANS_TIME_POSITION=%0.1f\n", ((float) pos_msecs) / 1000.0);
                }
                else if (strcasecmp(Cmd, "get_time_length")==0)
                {
                    xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
                    printf("ANS_LENGTH=%0.1f\n", ((float) len_msecs) / 1000.0);
                }
                break;

            case 'l':
                if (strcasecmp(Cmd, "loadfile")==0)
                {
                    ptr=rstrtok(ptr, " 	",&Token);
                    val=StringListAdd(Config->playlist, Token);
                    if (strtol(ptr, NULL, 10) == 0) CXinePlayStream(Config, StringListGet(Config->playlist, val));
                }
              else if (strcasecmp(Cmd, "lower")==0) 
							{
									if (Config->state & STATE_LOWERED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ZORDER");
									else X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_BELOW");
							}

                break;

            case 'm':
                if (strcasecmp(Cmd, "mute")==0)
                {
                    if (StrLen(ptr)==0) CXineMute(stream, TOGGLE);
                    else CXineMute(stream, atoi(ptr));
                }
                else if (strcasecmp(Cmd, "minimize")==0)
								{
									if (Config->state & STATE_ICONIZED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_RESTORED");
									else  X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ICONIZED");
								}

                break;

						case 'n':
                if (strcasecmp(Cmd, "next")==0) CXineSelectStream(Config, PLAY_PREV);
						break;

            case 'p':
                if (strcasecmp(Cmd, "pause")==0) CXinePause(Config);
                if (strcasecmp(Cmd, "prev")==0) CXineSelectStream(Config, PLAY_PREV);
                break;

            case 'q':
                if (strcasecmp(Cmd, "quit")==0) RetVal=EVENT_EXIT;
                break;

						case 'r':
              if (strcasecmp(Cmd, "raise")==0) 
							{
									if (Config->state & STATE_RAISED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ZORDER");
									else X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ABOVE");
							}
						break;

            case 's':
                if (strcasecmp(Cmd, "stop")==0) xine_stop(stream);
                else if (strcasecmp(Cmd, "seek")==0)
                {
#define CXINE_SEEK_CUR 0
#define CXINE_SEEK_PER 1
#define CXINE_SEEK_SET 2
                    ptr=rstrtok(ptr, " 	",&Token);
                    val=atoi(Token); //the actual seek offset
                    result=atoi(ptr); // 0==seconds from curr, 1==percent, 2==seconds from start

                    xine_get_pos_length(stream, &pos, &pos_msecs, &len_msecs);
                    if (result==CXINE_SEEK_SET) xine_play(stream, 0, val * 1000);
                    else if (result==CXINE_SEEK_CUR) xine_play(stream, 0, pos_msecs + val * 1000);
                    else if (result==CXINE_SEEK_PER)
                    {
                        pos=((float) val / 100.0) * len_msecs;
                        xine_play(stream, 0, pos);
                    }
                }
                else if (strcasecmp(Cmd, "shade")==0) 
								{
									if (Config->state & STATE_SHADED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_UNSHADE");
                	else X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_SHADED");
								}
                break;


            case 'v':
                if (strcasecmp(Cmd, "volume")==0)
                {
                    ptr=rstrtok(ptr, " 	",&Token);
                    val=atoi(Token);
                    if (atoi(ptr) !=0) CXineVolume(stream, SET_ADD, val);
                    CXineVolume(stream, 0, val);
                }
                break;

						case 'z':
                if (strcasecmp(Cmd, "zcycle")==0)
								{
									if (Config->state & STATE_LOWERED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ZORDER");
									else if (Config->state & STATE_RAISED) X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_BELOW");
									else X11SetWindowState(Config->X11Out,  "_NET_WM_STATE_ABOVE");
								}
						break;
						}
        }

    fflush(stdout);
		}

    destroy(Tempstr);
    destroy(Token);
    destroy(Title);
    destroy(URL);
    destroy(Cmd);

    return(RetVal);
}


int ControlSendMessage(const char *Msg)
{
    int fd, result=-1;

    fd=ControlPipeOpen(O_WRONLY | O_NONBLOCK);
    if ( (fd > -1) && (lockf(fd, F_LOCK, 0)==0) )
    {
        result=write(fd, Msg, StrLen(Msg));
    }

    return(result > 0);
}


