#pragma once

#include <Windows.h>
#include <stdint.h>

#define HID_THREAD_ID 0

typedef struct
{
	BOOL intrHaltThread;
} HIDParams;

typedef struct
{
	uint8_t idReport;
	uint8_t modifier;
	uint8_t index;
	uint8_t data[6];
} HIDReport;

BOOL GetKeyDown(uint8_t nKey);
BOOL GetKey(uint8_t nKey);
BOOL GetKeyUp(uint8_t nKey);

uint8_t GetAnalog0(int index);
uint8_t GetAnalog1(int index);
uint8_t GetAnalog2(int index);

DWORD WINAPI HIDMain(LPVOID lpParam);