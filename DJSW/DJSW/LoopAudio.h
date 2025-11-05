#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiodevice.h"
#include "audiochannel.h"
#include "LoopHID.h"

typedef struct
{
	int coreIndex;
	HANDLE threadHandle;
	DWORD threadId;

	int interruptNumber;
} AudioParams;

DWORD WINAPI AudioMain(LPVOID lpParams);