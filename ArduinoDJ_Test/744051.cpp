#include "744051.h"

int readAnalogMultiplexer(IC744051* ic, int address, bool toComplement = false)
{
    digitalWrite(ic->pinS0, address & 1);
    digitalWrite(ic->pinS1, address & 2);
    digitalWrite(ic->pinS2, address & 4);
    
    return analogRead(ic->pinOut);
}

uint8_t readAnalogMultiplexerUint8(IC744051* ic, int address)
{
    int value = readAnalogMultiplexer(ic, address);

    return (uint8_t)(value >> 2);
}
