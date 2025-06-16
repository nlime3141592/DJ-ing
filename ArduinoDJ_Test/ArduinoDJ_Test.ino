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
  uint8_t value0 = analogReadUint8(A0, true);
  uint8_t value1 = analogReadUint8(A1, true);

  ictrl.interface.eq0 = (uint8_t)127;
  ictrl.interface.eq1 = (uint8_t)127;
  ictrl.interface.eq2 = (uint8_t)127;
  ictrl.interface.eq3 = (uint8_t)127;
  ictrl.interface.eq4 = (uint8_t)127;
  ictrl.interface.eq5 = (uint8_t)127;

  ictrl.interface.fx0 = (uint8_t)127;
  ictrl.interface.fx1 = (uint8_t)127;

  ictrl.interface.vf0 = (uint8_t)127;
  ictrl.interface.vf1 = (uint8_t)0;
  ictrl.interface.xf = (uint8_t)127;

  ideck1.interface.btnFlag0 = 0;
  ideck1.interface.btnFlag0 |= digitalRead(2) << 6;
  ideck1.interface.dWheel = (int32_t)value0 - (int32_t)value1;
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
