#ifndef _744051_H
#define _744051_H

#include <Arduino.h>

typedef struct _IC744051
{
    uint8_t pinEnable; // deprecated

    // 주소 버스 핀
    uint8_t pinS0;
    uint8_t pinS1;
    uint8_t pinS2;

    // 직렬 출력 핀
    uint8_t pinOut;
} IC744051;

int readAnalogMultiplexer(IC744051* ic, int address, bool toComplement = false);
uint8_t readAnalogMultiplexerUint8(IC744051* ic, int address);

#endif