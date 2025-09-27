#include <Arduino.h>
#include <Keyboard.h>

#define PIN_HID 2

#define PIN_LED 10
#define PIN_SWITCH 21

void setup()
{
    pinMode(PIN_HID, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    if (digitalRead(PIN_HID))
    {
        return;
    }

    if (digitalRead(PIN_SWITCH))
    {
        digitalWrite(PIN_LED, 1);
        Keyboard.begin();
        Keyboard.write('a');
        Keyboard.end();
        delay(1000);
        digitalWrite(PIN_LED, 0);
    }
}