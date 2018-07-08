#ifndef CXINE_COMMAND_LINE_H
#define CXINE_COMMAND_LINE_H

#include "common.h"

void PrintUsage();
int ParseCommandLine(int argc, char *argv[], TConfig *Config);

#endif
