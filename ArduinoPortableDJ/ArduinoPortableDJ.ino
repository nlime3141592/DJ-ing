#include <Keyboard.h>

#include "HIDControls.h"

// digital io pins
#define PIN_S0 14
#define PIN_S1 15
#define PIN_S2 16

#define PIN_SHLD 3
#define PIN_CLR 5
#define PIN_CLK 2
#define PIN_DOUT 7

// analog input pins
#define PIN_FX11 A7
#define PIN_FX12 A6
#define PIN_VOL1 A8
#define PIN_TMP1 A9

#define PIN_FX21 A2
#define PIN_FX22 A3
#define PIN_VOL2 A1
#define PIN_TMP2 A0

#define PIN_XFADE A10

// 74166 io mode constants
#define MODE_PARALLEL_74166 0
#define MODE_SERIAL_74166 1

uint8_t keyTable[][8] = {
  { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 },
  { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
  { HID_PAD11, HID_PAD12, HID_PAD13, HID_PAD14, HID_PAD15, HID_PAD16, HID_PAD17, HID_PAD18 },
  { HID_PAD21, HID_PAD22, HID_PAD23, HID_PAD24, HID_PAD25, HID_PAD26, HID_PAD27, HID_PAD28 },
  { HID_PADFN11, HID_PADFN21, HID_PADFN12, HID_PADFN22, HID_PADFN13, HID_PADFN23, HID_PADFN14, HID_PADFN24 },
  { HID_TSH11, HID_TSH21, HID_TSH12, HID_TSH22, HID_TSH13, HID_TSH23, HID_TSH14, HID_TSH24 },
  { HID_LD1, HID_MUTE1, HID_PLAY1, HID_CUE1, HID_UNDEFINED, HID_UNDEFINED, HID_SEL1, HID_SPLIT1 },
  { HID_LD2, HID_MUTE2, HID_PLAY2, HID_CUE2, HID_UNDEFINED, HID_UNDEFINED, HID_SEL2, HID_SPLIT2 }
};

uint8_t buffer[32] = { 0 };
uint8_t* pBufferKey;
uint8_t* pBufferMixer;
uint8_t* pBufferDeck1;
uint8_t* pBufferDeck2;

void SetAddress(int address)
{
  digitalWrite(PIN_S0, address & 1);
  digitalWrite(PIN_S1, address & 2);
  digitalWrite(PIN_S2, address & 4);
}

// 2 mcs delays
void Clock()
{
  digitalWrite(PIN_CLK, 1);
  delayMicroseconds(1);
  digitalWrite(PIN_CLK, 0);
  delayMicroseconds(1);
}

// 5 mcs delays
void LoadParallel(int address)
{
  SetAddress(address);
  delayMicroseconds(1);

  digitalWrite(PIN_SHLD, MODE_PARALLEL_74166);
  delayMicroseconds(1);
  Clock();
  digitalWrite(PIN_SHLD, MODE_SERIAL_74166);
  delayMicroseconds(1);
}

// 21 mcs delay
uint8_t ReadFlag(int address, uint8_t* keySet)
{
  LoadParallel(address);

  uint8_t flag = 0;

  for (int i = 0; i < 8; ++i)
  {
    if (digitalRead(PIN_DOUT))
      flag |= keySet[i];

    Clock();
  }

  return flag;
}

// 29 mcs delays
uint8_t ReadData(int address, uint8_t* keySet)
{
  LoadParallel(address);

  uint8_t data = 0;

  for (int i = 0; i < 8; ++i)
  {
    int flag = digitalRead(PIN_DOUT);
    delayMicroseconds(1);
    Clock();

    if (!flag)
      continue;
    else if (data != 0)
    {
      data = 0;
      break;
    }
    else
      data = keySet[i];
  }

  return data;
}

void setup_buffer()
{
  memset(buffer, 0x00, sizeof(buffer));

  pBufferKey = buffer;
  pBufferMixer = buffer + 8;
  pBufferDeck1 = buffer + 16;
  pBufferDeck2 = buffer + 24;
}

void setup()
{
  // digital io initialization
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);

  pinMode(PIN_SHLD, OUTPUT);
  pinMode(PIN_CLR, OUTPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_DOUT, INPUT);

  // analog input initialization
  pinMode(PIN_FX11, INPUT);
  pinMode(PIN_FX12, INPUT);
  pinMode(PIN_VOL1, INPUT);
  pinMode(PIN_TMP1, INPUT);

  pinMode(PIN_FX21, INPUT);
  pinMode(PIN_FX22, INPUT);
  pinMode(PIN_VOL2, INPUT);
  pinMode(PIN_TMP2, INPUT);

  pinMode(PIN_XFADE, INPUT);

  // initial states
  digitalWrite(PIN_CLR, 1);
  digitalWrite(PIN_CLK, 0);
  digitalWrite(PIN_SHLD, 1);

  // report buffer initialization
  setup_buffer();
}

void loop()
{
  // 루프 1회 실행 시간: 평균 2080 마이크로초 (2.08 밀리초)
  // 디지털 입력: 약 1020 마이크로초 (1.02 밀리초)
  // 아날로그 입력: 약 1060 마이크로초 (1.06 밀리초)

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferKey[0] = ReadFlag(0, keyTable[0]);

  for (int i = 2; i < 8; ++i)
  {
    pBufferKey[i] = ReadData(i, keyTable[i]);
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferMixer[0] = (uint8_t)(analogRead(PIN_XFADE) >> 2);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferDeck1[0] = (uint8_t)(analogRead(PIN_VOL1) >> 2);
  pBufferDeck1[1] = (uint8_t)(analogRead(PIN_TMP1) >> 2);
  pBufferDeck1[4] = (uint8_t)(analogRead(PIN_FX11) >> 2);
  pBufferDeck1[7] = (uint8_t)(analogRead(PIN_FX12) >> 2);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferDeck2[0] = (uint8_t)(analogRead(PIN_VOL2) >> 2);
  pBufferDeck2[1] = (uint8_t)(analogRead(PIN_TMP2) >> 2);
  pBufferDeck2[4] = (uint8_t)(analogRead(PIN_FX21) >> 2);
  pBufferDeck2[7] = (uint8_t)(analogRead(PIN_FX22) >> 2);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  Keyboard.sendBuffer(buffer, sizeof(buffer));
}
