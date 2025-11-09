#pragma once

#include <Windows.h>
#include <stdint.h>

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