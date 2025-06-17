#include <Arduino.h>

#include "djdef.h"
#include "djinput.h"

#include "74166.h"
#include "744051.h"

static uint32_t communicateTimestamp;

static IC74166 icCtrl;
static IC74166 icDeck1;
static IC74166 icDeck2;
static IC744051 icEq;

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

  ictrl.interface.eq0 = readAnalogMultiplexerUint8(&icEq, 0);
  ictrl.interface.eq1 = readAnalogMultiplexerUint8(&icEq, 1);
  ictrl.interface.eq2 = readAnalogMultiplexerUint8(&icEq, 2);
  ictrl.interface.eq3 = readAnalogMultiplexerUint8(&icEq, 3);
  ictrl.interface.eq4 = readAnalogMultiplexerUint8(&icEq, 4);
  ictrl.interface.eq5 = readAnalogMultiplexerUint8(&icEq, 5);
  // ictrl.interface.eq0 = value0;
  // ictrl.interface.eq1 = (uint8_t)127;
  // ictrl.interface.eq2 = value1;
  // ictrl.interface.eq3 = (uint8_t)127;
  // ictrl.interface.eq4 = (uint8_t)127;
  // ictrl.interface.eq5 = (uint8_t)127;

  ictrl.interface.fx0 = readAnalogMultiplexerUint8(&icEq, 6);
  ictrl.interface.fx1 = readAnalogMultiplexerUint8(&icEq, 7);
  // ictrl.interface.fx0 = (uint8_t)127;
  // ictrl.interface.fx1 = (uint8_t)127;

  ictrl.interface.vf0 = analogReadUint8(PIN_VF_1);
  ictrl.interface.vf1 = analogReadUint8(PIN_VF_2);
  ictrl.interface.xf = analogReadUint8(PIN_XF);
  // ictrl.interface.vf0 = (uint8_t)127;
  // ictrl.interface.vf1 = (uint8_t)127;
  // ictrl.interface.xf = (uint8_t)127;

  inputParallel(&icCtrl);
  inputParallel(&icDeck1);
  inputParallel(&icDeck2);

  ictrl.interface.btnFlag0 = 0;
  ideck1.interface.btnFlag0 = 0;
  ideck2.interface.btnFlag0 = 0;

  // Test Button Click
  // ideck1.interface.btnFlag0 |= digitalRead(2) << 6;

  for (int i = 0; i < 8; ++i)
  {
    ictrl.interface.btnFlag0 |= (readSerial(&icCtrl) << i);
    ideck1.interface.btnFlag0 |= (readSerial(&icDeck1) << i);
    ideck2.interface.btnFlag0 |= (readSerial(&icDeck2) << i);
  }

  // ictrl.interface.btnFlag0의 플래그로 휠 값 설정
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
