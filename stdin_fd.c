#include "stdin_fd.h"
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
void StdinReset()
{
	if (Config->flags & CONFIG_CONTROL) tcsetattr(Config->stdin, TCSANOW, &tty_old);
}



int StdinNewPipe(int Flags)
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


