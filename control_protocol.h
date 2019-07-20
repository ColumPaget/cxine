#ifndef CXINE_CONTROL_H
#define CXINE_CONTROL_H

#include "common.h"

int ControlPipeOpen(int mode);
int ControlHandleInput(int fd, xine_stream_t *stream);
int ControlSendMessage(const char *Msg);

#endif
