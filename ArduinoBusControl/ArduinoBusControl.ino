#include <Arduino.h>

#define PIN_S0 6
#define PIN_S1 7
#define PIN_S2 8

int address;

void setup()
{
    pinMode(PIN_S0, OUTPUT);
    pinMode(PIN_S1, OUTPUT);
    pinMode(PIN_S2, OUTPUT);

    address = 0;

    delay(1000);
}

void loop()
{
    digitalWrite(PIN_S0, address & 1);
    digitalWrite(PIN_S1, address & 2);
    digitalWrite(PIN_S2, address & 4);

    address++;
    
    delay(500);
}