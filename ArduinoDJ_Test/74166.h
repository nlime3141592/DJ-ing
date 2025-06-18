#ifndef _74166_H
#define _74166_H

#include <Arduino.h>

void init74166(uint8_t pinPe);
void clock74166(uint8_t pinClk);
void inputParallel(uint8_t pinPe, uint8_t pinClk);
int readSerial(uint8_t pinDout);

#endif