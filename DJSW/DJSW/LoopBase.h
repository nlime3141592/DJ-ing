#pragma once

#include <Windows.h>
#include <stdint.h>

#define DJSW_INT_NULL 0
#define DJSW_INT_LOOPEND 1
#define DJSW_INT_SYNC -1

typedef struct
{
	HANDLE threadHandle;
	DWORD threadId;
	DWORD threadAffinityMask;

	uint32_t interruptNumber;
} LoopBaseParams;

int WINAPI LoopInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI LoopUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI LoopFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);