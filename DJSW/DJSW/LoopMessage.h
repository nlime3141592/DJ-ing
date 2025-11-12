#pragma once

#include "djsw_errorcode.h"

typedef struct
{

} djMessage;

djErrorCode PeekMessage(djMessage* msg);
djErrorCode GetMessage(djMessage* msg);
djErrorCode AddMessage(djMessage* msg);

djErrorCode MessageInit();
djErrorCode MessageUpdate();
djErrorCode MessageFinal();
