#ifndef CXINE_CONFIG_H
#define CXINE_CONFIG_H

#include "common.h"

TConfig *ConfigInit(xine_t *xine);
void ConfigDefaults(TConfig *Config);
void CXineConfigModify(xine_t *xine, const char *Key, const char *Value);
void CXineConfigNumericModify(xine_t *xine, const char *Key, int Value);
void CXineConfigSave(TConfig *Config);


#endif
