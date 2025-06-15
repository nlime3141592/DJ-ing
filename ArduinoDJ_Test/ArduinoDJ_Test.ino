#include <Arduino.h>

#include "djdef.h"
#include "djinput.h"

static uint32_t communicateTimestamp;

void setup()
{
  IO_SETUP_TEMPO_1;
  IO_SETUP_TEMPO_2;
  IO_SETUP_VF_1;
  IO_SETUP_VF_2;
  IO_SETUP_XF;
  IO_SETUP_EQ;
  IO_SETUP_BUTTON_ROTARY;
  IO_SETUP_SOFTWARE_CLK;
  IO_SETUP_74166_SIN;
  IO_SETUP_BUS_S0;
  IO_SETUP_BUS_S1;
  IO_SETUP_BUS_S2;
  IO_SETUP_VF_1;
  IO_SETUP_VF_2;
  IO_SETUP_BUTTON_CTRL;
  IO_SETUP_BUTTON_DECK_1;
  IO_SETUP_BUTTON_DECK_2;

  Serial.begin(BAUD_RATE);

  communicateTimestamp = micros();
}

void testlogic()
{
  uint8_t value = analogReadUint8(A0, true);

  ictrl.interface.eq5 = value;
}

void communicate()
{
  uint32_t currentTimestamp = micros();

  if (currentTimestamp - communicateTimestamp < COMMUNICATION_PERIOD_MICROSECONDS)
    return;

  communicateTimestamp = currentTimestamp;

  if (!Serial.available())
    return;

  Serial.read(); // ignore

  Serial.write((const char*)ictrl.rawdata, sizeof(InputController));
  Serial.write((const char*)ideck1.rawdata, sizeof(InputDeck));
  Serial.write((const char*)ideck2.rawdata, sizeof(InputDeck));
}

void loop()
{
  testlogic();
  communicate();
}
