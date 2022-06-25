#ifndef CXINE_STDIN_H
#define CXINE_STDIN_H

#include "common.h"

void StdinSetup();
void StdinReset();
int StdinNewPipe(int Flags);
char *StdInReadString(char *RetStr);

#endif

