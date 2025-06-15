#include "74166.h"

void init(IC74166* ic)
{
    digitalWrite(ic->pinPe, HIGH);
}

void inputParallel(IC74166* ic)
{
    digitalWrite(ic->pinPe, LOW);
    digitalWrite(ic->pinClk, HIGH);
    digitalWrite(ic->pinClk, LOW);
    digitalWrite(ic->pinPe, HIGH);
}

int readSerial(IC74166* ic)
{
    int value = digitalRead(ic->pinDout);
    digitalWrite(ic->pinClk, HIGH);
    digitalWrite(ic->pinClk, LOW);
    return value;
}