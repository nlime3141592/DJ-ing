#ifndef __DJSW_IO_H__
#define __DJSW_IO_H__

#include "djsw_type.h"

int GetKeyDown(djFunctionCode_t fncode);
int GetKey(djFunctionCode_t fncode);
int GetKeyUp(djFunctionCode_t fncode);

djAnalog8_t GetAnalog8(djFunctionCode_t fncode);
djErrorCode_t TryGetAnalog8(djFunctionCode_t fncode, djAnalog8_t* outValue);

#endif
