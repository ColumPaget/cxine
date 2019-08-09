#ifndef CXINE_CONFIG_H
#define CXINE_CONFIG_H

#include "common.h"

TConfig *ConfigInit(xine_t *xine);
void ConfigDefaults(TConfig *Config);
int CXineConfigModify(xine_t *xine, const char *Key, const char *Value);
int CXineConfigNumericModify(xine_t *xine, const char *Key, int Value);
void CXineConfigModifyOrCreate(xine_t *xine, const char *Key, const char *Value, const char *Help);
void CXineConfigSave(TConfig *Config);


#endif
