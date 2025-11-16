#pragma once

#include <stdint.h>
#include <Windows.h>

#include "hidapi.h"
#include "LoopBase.h"
#include "LoopInterrupt.h"

#include "djsw_input_hid.h"
#include "djsw_input_keyboard.h"

#define HID_VID 0x2341
#define HID_PID 0x8036

#define DJSW_HID_BUFFER_LENGTH 65

typedef struct
{
	LoopBaseParams loopBaseParams;
} HIDParams;

typedef struct
{
	uint8_t modifier;
	uint8_t reserved;
	uint8_t data[6];
} djHidDigitalReport;

typedef struct
{
	uint8_t data[8];
} djHidAnalogReport;

void InputInit();
void InputUpdate();
void InputFinal();