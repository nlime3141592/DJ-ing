#pragma once

#include <Windows.h>
#include <stdint.h>

#define AUDIO_THREAD_ID 0

typedef struct
{
	BOOL intrHaltThread;
} AudioParams;

DWORD WINAPI AudioMain(LPVOID lpParam);