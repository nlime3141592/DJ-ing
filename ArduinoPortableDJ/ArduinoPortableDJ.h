#ifndef __ARDUINO_PORTABLE_DJ_H__
#define __ARDUINO_PORTABLE_DJ_H__

#ifndef __ARDUINO_IDE__
#include <Arduino.h>
#endif

#include "DJKey.h"
#include "DJPin.h"

// 제어 입력 플래그 마스크
#define FLAG_DIN_LCTRL 0x80
#define FLAG_DIN_LSHIFT 0x40
#define FLAG_DIN_LALT 0x20
#define FLAG_DIN_LGUI 0x10
#define FLAG_DIN_RCTRL 0x08
#define FLAG_DIN_RSHIFT 0x04
#define FLAG_DIN_RALT 0x02
#define FLAG_DIN_RGUI 0x01

// USB HID Report 구조를 따릅니다.
typedef struct _digital_input
{
    // 제어 입력 플래그
    uint8_t control1; // LCTRL | LSHIFT | LALT | LGUI | RCTRL | RSHIFT | RALT | RGUI
    uint8_t reserved; // DEPRECATED.

    // 키 데이터
    uint8_t key1;
    uint8_t key2;
    uint8_t key3;
    uint8_t key4;
    uint8_t key5;
    uint8_t key6;
} DigitalInput;

typedef struct _analog_input
{
    uint8_t fx11;
    uint8_t fx12;
    uint8_t fx21;
    uint8_t fx22;
    uint8_t vol1;
    uint8_t vol2;
    uint8_t tempo1;
    uint8_t tempo2;

    uint8_t xfader;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
    uint8_t reserved6;
    uint8_t reserved7;
} AnalogInput;

typedef union _digital_input_buffer
{
    uint8_t buffer[8];
    DigitalInput inputs;
} DigitalInputBuffer;

typedef union _analog_input_buffer
{
    uint8_t buffer[16];
    AnalogInput inputs;
} AnalogInputBuffer;

#endif