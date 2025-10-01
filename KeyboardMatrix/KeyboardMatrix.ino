#include <Arduino.h>

#define PIN_SI_0 2
#define PIN_SI_1 3
#define PIN_SI_2 4

#define PIN_SO_0 7
#define PIN_SO_1 8

#define PIN_SO_Y 9

#define PIN_DEBUG_IN 5
#define PIN_DEBUG_EN 21

char keyAlloc[][3] = {
    { 'a', 'b', 'c' },
    { 'A', 'B', 'C' },
    { '0', '1', '2' },
    { '7', '8', '9' }
};

int keyPress[][3] = {
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 }
};

void setaddr2(int addr, int s0, int s1)
{
    digitalWrite(s0, addr & 1);
    digitalWrite(s1, addr & 2);
}

void setaddr3(int addr, int s0, int s1, int s2)
{
    setaddr2(addr, s0, s1);
    digitalWrite(s2, addr & 4);
}

void setaddri(int addr)
{
    setaddr3(addr, PIN_SI_0, PIN_SI_1, PIN_SI_2);
    delayMicroseconds(10);
}

void setaddro(int addr)
{
    setaddr2(addr, PIN_SO_0, PIN_SO_1);
    delayMicroseconds(10);
}

void setup()
{
    pinMode(PIN_SI_0, OUTPUT);
    pinMode(PIN_SI_1, OUTPUT);
    pinMode(PIN_SI_2, OUTPUT);

    pinMode(PIN_SO_0, OUTPUT);
    pinMode(PIN_SO_1, OUTPUT);

    pinMode(PIN_SO_Y, INPUT_PULLUP);

    pinMode(PIN_DEBUG_IN, INPUT);
    pinMode(PIN_DEBUG_EN, INPUT_PULLUP);

    Serial.begin(9600);

    delay(1000);

    digitalWrite(PIN_DEBUG_IN, 0);
}

void loop()
{
    for (int i = 0; i < 3; ++i)
    {
        setaddri(i);

        for (int j = 0; j < 3; ++j)
        {
            setaddro(j);

            if (digitalRead(PIN_SO_Y) == HIGH)
            {
                if (keyPress[i][j] == 0)
                {
                    keyPress[i][j] = 1;
                    Serial.println(keyAlloc[i][j]);
                }
            }
            else if (keyPress[i][j] != 0)
            {
                keyPress[i][j] = 0;
            }
        }
    }

    // digitalWrite(PIN_DEBUG_IN, !digitalRead(21));

    delayMicroseconds(10000);
}