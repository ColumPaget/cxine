#include "splashscreen.h"
#include "stdin_fd.h"
#include <errno.h>
#include "X11.h"

const char *cxine_img="\
iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAIAAAAlC+aJAAADAFBMVEUAAAD/AAAA/wD//wAAAP//\
AP8A///////b29u2traSkpJtbW1JSUkkJCTbAAC2AACSAABtAABJAAAkAAAA2wAAtgAAkgAAbQAA\
SQAAJADb2wC2tgCSkgBtbQBJSQAkJAAAANsAALYAAJIAAG0AAEkAACTbANu2ALaSAJJtAG1JAEkk\
ACQA29sAtrYAkpIAbW0ASUkAJCT/29vbtra2kpKSbW1tSUlJJCT/trbbkpK2bW2SSUltJCT/kpLb\
bW22SUmSJCT/bW3bSUm2JCT/SUnbJCT/JCTb/9u227aStpJtkm1JbUkkSSS2/7aS25Jttm1Jkkkk\
bSSS/5Jt221JtkkkkiRt/21J20kktiRJ/0kk2yQk/yTb2/+2ttuSkrZtbZJJSW0kJEm2tv+Skttt\
bbZJSZIkJG2Skv9tbdtJSbYkJJJtbf9JSdskJLZJSf8kJNskJP///9vb27a2tpKSkm1tbUlJSST/\
/7bb25K2tm2SkkltbST//5Lb2222tkmSkiT//23b20m2tiT//0nb2yT//yT/2//bttu2kraSbZJt\
SW1JJEn/tv/bktu2bbaSSZJtJG3/kv/bbdu2SbaSJJL/bf/bSdu2JLb/Sf/bJNv/JP/b//+229uS\
trZtkpJJbW0kSUm2//+S29tttrZJkpIkbW2S//9t29tJtrYkkpJt//9J29sktrZJ//8k29sk////\
27bbtpK2km2SbUltSSRJJAD/tpLbkm22bUmSSSRtJAD/ttvbkra2bZKSSW1tJElJACT/krbbbZK2\
SW2SJEltACTbtv+2ktuSbbZtSZJJJG0kAEm2kv+SbdttSbZJJJIkAG222/+SttttkrZJbZIkSW0A\
JEmStv9tkttJbbYkSZIAJG22/9uS27ZttpJJkm0kbUkASSSS/7Zt25JJtm0kkkkAbSTb/7a225KS\
tm1tkklJbSQkSQC2/5KS221ttklJkiQkbQD/tgDbkgC2bQCSSQD/ALbbAJK2AG2SAEkAtv8AktsA\
bbYASZIAAAAAAADPKgIEAAAA60lEQVRo3u2awQrEIAxEjRn/wf//UfeQZVGWgrdmwniQNNiDHYR5\
Tm3O2ZgHAKy1zIx0hrtLgVcV6L2bWewmulw1xhgVzgDjt4+6yhng3QMAtNbiOaThquHuFRT4398+\
MvdvFdjfTLX+UIDSSvwUYDzBXy9Er0ARN0rq50oowOulRWRiYilQgAfombiIAk9uNn+/ChNX4AEp\
8DYPkHJZOSKju1kRE4uJxcQnE+8zRf/IyPZxo2OG9crIlJEpI1NGlo6JuWoxcc6MjOhmRTyQx41K\
gUwK3HjaJH0lNMrIlJHpv9FETEx3p1KBiT8mtDd/rD9XBQAAAABJRU5ErkJggg==\
";

void CxineInjectSplashScreen(xine_t *xine)
{
    size_t len;
    int fd;
    pid_t pid;
    uint8_t *decoded=NULL;

		// take a copy of stdin so that we don't lose our controlling terminal
    // this may get us killed by SIGHUP
		dup(0);

    StdInNewPipe(0);
    decoded=calloc(4096, 1);

    if (fork()==0)
    {
//must close this or confusion occurs with two processes holding stdin
        close(Config->stdin);
        X11Disassociate(Config->X11Out);

        decoded=calloc(strlen(cxine_img) * 4, 1);
        len=xine_base64_decode(cxine_img, decoded);
        len=write(Config->to_xine, decoded, len);

        close(Config->to_xine);
        free(decoded);
        _exit(1);
    }

//must close our end of this
    close(Config->to_xine);
    if (xine_open(Config->stream, "stdin://"))
    {
        xine_play(Config->stream, 0, 0);
    }

    close(0);
    dup(Config->stdin);
}
