#include "74166.h"

void init74166(uint8_t pinPe)
{
    digitalWrite(pinPe, HIGH);
}

void clock74166(uint8_t pinClk)
{
    digitalWrite(pinClk, HIGH);
    delayMicroseconds(5);
    digitalWrite(pinClk, LOW);
    delayMicroseconds(5);
}

void inputParallel(uint8_t pinPe, uint8_t pinClk)
{
    digitalWrite(pinPe, LOW);
    delayMicroseconds(5);

    clock74166(pinClk);

    digitalWrite(pinPe, HIGH);
    delayMicroseconds(5);
}

int readSerial(uint8_t pinDout)
{
    int value = digitalRead(pinDout);
    return value;
}