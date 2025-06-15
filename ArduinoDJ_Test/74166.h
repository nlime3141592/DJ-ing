#ifndef _74166_H
#define _74166_H

#include <Arduino.h>

typedef struct _IC74166
{
    uint8_t pinClk;
    uint8_t pinPe;
    uint8_t pinDin; // deprecated
    uint8_t pinDout;
} IC74166;

void init(IC74166* ic);
void inputParallel(IC74166* ic);
int readSerial(IC74166* ic);

#endif