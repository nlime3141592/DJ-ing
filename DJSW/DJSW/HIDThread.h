#pragma once

#include <Windows.h>
#include <stdint.h>

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

BOOL GetKeyDown(uint8_t nKey);
BOOL GetKey(uint8_t nKey);
BOOL GetKeyUp(uint8_t nKey);

uint8_t GetAnalog0(int index);
uint8_t GetAnalog1(int index);
uint8_t GetAnalog2(int index);

void HIDInit();
void HIDLoop();
void HIDFinal();