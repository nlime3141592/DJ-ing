#ifndef __ARDUINO_IDE__
#include <Arduino.h>
#endif

#include "ArduinoPortableDJ.h"

#define POLLING_TIME_MCS 10000

static uint8_t keyTable[][8] = {
    { HID_KEY_UNDEFINED, HID_KEY_SH1, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_SH2, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED },
    { HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED },
    { HID_KEY_PAD11, HID_KEY_PAD12, HID_KEY_PAD13, HID_KEY_PAD14, HID_KEY_PAD15, HID_KEY_PAD16, HID_KEY_PAD17, HID_KEY_PAD18 },
    { HID_KEY_PAD21, HID_KEY_PAD22, HID_KEY_PAD23, HID_KEY_PAD24, HID_KEY_PAD25, HID_KEY_PAD26, HID_KEY_PAD27, HID_KEY_PAD28 },
    { HID_KEY_PADFN11, HID_KEY_PADFN21, HID_KEY_PADFN21, HID_KEY_PADFN22, HID_KEY_PADFN13, HID_KEY_PADFN23, HID_KEY_PADFN14, HID_KEY_PADFN24 },
    { HID_KEY_TSH11, HID_KEY_TSH21, HID_KEY_TSH21, HID_KEY_TSH22, HID_KEY_TSH13, HID_KEY_TSH23, HID_KEY_TSH14, HID_KEY_TSH24 },
    { HID_KEY_LD1, HID_KEY_MUTE1, HID_KEY_PLAY1, HID_KEY_CUE1, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_SEL1, HID_KEY_SPLIT_LR },
    { HID_KEY_LD2, HID_KEY_MUTE2, HID_KEY_PLAY2, HID_KEY_CUE2, HID_KEY_UNDEFINED, HID_KEY_UNDEFINED, HID_KEY_SEL2, HID_KEY_SPLIT_RL }
};

static DigitalInputBuffer dinBuffer;
static AnalogInputBuffer ainBuffer;

static void SetAddress(int address)
{
    digitalWrite(PIN_ADDR_S0, address & 1);
    digitalWrite(PIN_ADDR_S1, address & 2);
    digitalWrite(PIN_ADDR_S2, address & 4);

    digitalWrite(PIN_SHLD, 0); // Set load mode.
    delayMicroseconds(5);

    digitalWrite(PIN_CLK, 1);
    delayMicroseconds(5);

    digitalWrite(PIN_CLK, 0);
    digitalWrite(PIN_SHLD, 1); // Set shift mode.
}

void setup()
{
    pinMode(PIN_FX11, INPUT);
    pinMode(PIN_FX12, INPUT);
    pinMode(PIN_FX21, INPUT);
    pinMode(PIN_FX22, INPUT);

    pinMode(PIN_VOL1, INPUT);
    pinMode(PIN_VOL2, INPUT);

    pinMode(PIN_TEMPO1, INPUT);
    pinMode(PIN_TEMPO2, INPUT);

    pinMode(PIN_XFADER, INPUT);

    pinMode(PIN_ADDR_S0, OUTPUT);
    pinMode(PIN_ADDR_S1, OUTPUT);
    pinMode(PIN_ADDR_S2, OUTPUT);

    pinMode(PIN_KEY_MATRIX_IN, INPUT);

    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_SHLD, OUTPUT);
    pinMode(PIN_CLR, OUTPUT);

    digitalWrite(PIN_CLK, 0);
    digitalWrite(PIN_SHLD, 1);
    digitalWrite(PIN_CLR, 1);
}

void loop()
{
    unsigned long tBegin = micros();

    // 제어 데이터 매트릭스 읽기
    for (int i = 0; i <= 1; ++i)
    {
        SetAddress(i); // 10 mcs
        delayMicroseconds(5); // 5 mcs

        dinBuffer.buffer[i] << 1;
        dinBuffer.buffer[i] |= digitalRead(PIN_KEY_MATRIX_IN);
    }

    // 키 데이터 매트릭스 읽기
    for (int i = 2; i <= 7; ++i)
    {
        SetAddress(i); // 10 mcs
        delayMicroseconds(5); // 5 mcs

        dinBuffer.buffer[i] = HID_KEY_UNDEFINED;

        for (int j = 0; j < 8; ++j)
        {
            // Read from key matrix.
            if (!digitalRead(PIN_KEY_MATRIX_IN))
            {
                continue;
            }
            
            if (dinBuffer.buffer[i] == HID_KEY_UNDEFINED)
            {
                dinBuffer.buffer[i] = keyTable[i][j];
            }
            else
            {
                dinBuffer.buffer[i] = HID_KEY_ERROR_ROLL_OVER;
                break;
            }
        }
    }

    ainBuffer.inputs.fx11 = (uint8_t)(analogRead(PIN_FX11) >> 2);
    ainBuffer.inputs.fx12 = (uint8_t)(analogRead(PIN_FX12) >> 2);
    ainBuffer.inputs.fx21 = (uint8_t)(analogRead(PIN_FX21) >> 2);
    ainBuffer.inputs.fx22 = (uint8_t)(analogRead(PIN_FX22) >> 2);
    ainBuffer.inputs.vol1 = (uint8_t)(analogRead(PIN_VOL1) >> 2);
    ainBuffer.inputs.vol2 = (uint8_t)(analogRead(PIN_VOL1) >> 2);
    ainBuffer.inputs.tempo1 = (uint8_t)(analogRead(PIN_TEMPO1) >> 2);
    ainBuffer.inputs.tempo2 = (uint8_t)(analogRead(PIN_TEMPO2) >> 2);

    ainBuffer.inputs.xfader = (uint8_t)(analogRead(PIN_XFADER) >> 2);

    delayMicroseconds(POLLING_TIME_MCS - (micros() - tBegin));
}