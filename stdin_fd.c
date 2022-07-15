#include "stdin_fd.h"
#include "playlist.h"
#include "playback_control.h"
#include <termios.h>

struct termios tty_old;



//if we are not in slave mode, then we read keypresses from stdin
//to do this we have to switch the terminal out of 'canonical' (line editing) mode
void StdInSetup()
{
    struct termios tty_new;

    tcgetattr(Config->stdin, &tty_old);
    tcgetattr(Config->stdin, &tty_new);
    tty_new.c_lflag ^= ICANON;
    tty_new.c_lflag ^= ECHO;
    tty_new.c_lflag |= ISIG;
    tty_new.c_cc[VMIN]=1;
    tty_new.c_cc[VTIME]=0;
    tcsetattr(Config->stdin, TCSANOW, &tty_new);
}

//reset StdIn back to whatever it was at program startup
void StdInReset()
{
    if (Config->flags & CONFIG_CONTROL) tcsetattr(Config->stdin, TCSANOW, &tty_old);
}

int StdInNewPipe(int Flags)
{
    int pipes[2];

    //rearrange file descriptors
    pipe(pipes);
    close(0);
    if (Config->to_xine > -1) close(Config->to_xine);
    dup(pipes[0]);
    Config->to_xine=dup(pipes[1]);
    close(pipes[0]);
    close(pipes[1]);
    if (Flags & CONFIG_CONTROL) StdInSetup();
}



char *StdInReadString(char *RetStr)
{
    char Key[2];
    int result;

    RetStr=rstrcpy(RetStr, "");
    memset(Key, 0, 2);
    while (1)
    {
        result=read(Config->stdin, Key, 1);
        if (result < 1) break;
        if (Key[0]=='\n') break;
        if (Key[0]=='\0x1b')
        {
            RetStr=rstrcpy(RetStr, "");
            break;
        }
        printf("%c", Key[0]);
        fflush(NULL);
        RetStr=rstrcat(RetStr, Key);
    }

    return(RetStr);
}


void StdInAskJump()
{
    char *Entry=NULL;
    int pos;

    printf("Enter Jump: ");
    fflush(stdout);
    Entry=StdInReadString(Entry);

    pos=PlaylistFindMatch(Entry);
    if (pos < 1) pos=atoi(Entry);
    if (pos > 0) CXineSelectStream(Config, pos);

    destroy(Entry);
}

