#ifndef _DJ_KEYBOARD_H
#define _DJ_KEYBOARD_H

#include <stdint.h>

#include "74166.h"

typedef struct
{
    uint8_t modifier;
    uint8_t channelNumber;
    uint8_t data[6];
} HIDChannel;

typedef union
{
    uint8_t buffer[8];
    HIDChannel channel;
} HIDBuffer;

#endif