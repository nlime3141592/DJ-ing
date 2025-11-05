#pragma once

#include <stdint.h>
#include <Windows.h>

#include "hidapi.h"

#define HID_VID 0x2341
#define HID_PID 0x8036

#define HID_BUFFER_LENGTH 65
#define HID_KEY_COUNT 256

typedef struct
{
	int coreIndex;
	HANDLE threadHandle;
	DWORD threadId;

	int interruptNumber;
} HIDParams;

typedef struct
{
	uint8_t modifier;
	uint8_t reserved;
	uint8_t data[6];
} HIDKeyboardReport;

typedef struct
{
	uint8_t data[8];
} HIDAnalogReport;

BOOL IsVendorHIDConnected();

BOOL GetVendorHIDKeyDown(uint8_t keycode);
BOOL GetVendorHIDKey(uint8_t keycode);
BOOL GetVendorHIDKeyUp(uint8_t keycode);

uint8_t GetAnalogMixer(int32_t idxProperty);
uint8_t GetAnalogDeck1(int32_t idxProperty);
uint8_t GetAnalogDeck2(int32_t idxProperty);

void SetStandardHIDKeyDown(uint8_t keycode);
void SetStandardHIDKeyUp(uint8_t keycode);

BOOL GetStandardHIDKeyDown(uint8_t keycode);
BOOL GetStandardHIDKey(uint8_t keycode);
BOOL GetStandardHIDKeyUp(uint8_t keycode);

DWORD WINAPI HIDMain(LPVOID lpParams);