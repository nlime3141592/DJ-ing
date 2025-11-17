#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <Windows.h>

#include "audiodevice.h"
#include "audiochannel.h"
#include "LoopBase.h"
#include "LoopHID.h"
#include "LoopInterrupt.h"

typedef struct
{
	LoopBaseParams loopBaseParams;
} AudioParams;

// 오디오 스레드(코어) 진입점
DWORD WINAPI AudioMain(LPVOID lpParams);