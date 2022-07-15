#ifndef CXINE_STDIN_H
#define CXINE_STDIN_H

#include "common.h"

void StdInSetup();
void StdInReset();
int StdInNewPipe(int Flags);
void StdInAskJump();
char *StdInReadString(char *RetStr);

#endif

