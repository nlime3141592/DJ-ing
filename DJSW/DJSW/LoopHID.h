#pragma once

#include <immintrin.h> // for AVX2 Instruction Set
#include <stdint.h>
#include <Windows.h>

#include "hidapi.h"

#define HID_VID 0x2341
#define HID_PID 0x8036

#define HID_BUFFER_LENGTH 65
#define HID_KEY_COUNT 256

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

BOOL IsHIDConnected();

BOOL GetHIDKeyDown(uint8_t keycode);
BOOL GetHIDKey(uint8_t keycode);
BOOL GetHIDKeyUp(uint8_t keycode);

uint8_t GetAnalogMixer(int32_t idxProperty);
uint8_t GetAnalogDeck1(int32_t idxProperty);
uint8_t GetAnalogDeck2(int32_t idxProperty);

void HIDInit();
void HIDLoop();
void HIDFinal();