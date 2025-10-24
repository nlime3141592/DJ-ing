#include <Keyboard.h>

#include "DJ_Keyboard.h"
#include "74166.h"

static IC74166 pin74166 = {
  .pinS0 = 14,
  .pinS1 = 15,
  .pinS2 = 16,
  .pinSHLD = 7,
  .pinCLR = 0,
  .pinCLK = 3,
  .pinCLKINH = 0,
  .pinDIN = 0,
  .pinDOUT = 5,

  .pinDA = 0,
  .pinDB = 0,
  .pinDC = 0,
  .pinDD = 0,
  .pinDE = 0,
  .pinDF = 0,
  .pinDG = 0,
  .pinDH = 0
};

HIDBuffer buffer0 = { 0 };
HIDBuffer buffer1 = { 0 };
HIDBuffer buffer2 = { 0 };
HIDBuffer buffer3 = { 0 };

uint8_t keytable[][8] = {
  { 1, 2, 3, 4, 5, 6, 7, 8 },
  { 9, 10, 11, 12, 13, 14, 15, 16 },
  { 17, 18, 19, 20, 21, 22, 23, 24 },
  { 25, 26, 27, 28, 29, 30, 31, 32 },
  { 33, 34, 35, 36, 37, 38, 39, 40 },
  { 41, 42, 43, 44, 45, 46, 47, 48 }
};

static uint8_t readDataChannel(const IC74166* ic, int channel, uint8_t* keyset)
{
  uint8_t value = 0;

  setAddress74166(ic, channel);
  DELAY_74166();

  setModeToParallel74166(ic);
  DELAY_74166();
  clock74166(ic);
  setModeToSerial74166(ic);
  DELAY_74166();

  for (int i = 0; i < 8; ++i)
  {
    int flag = peak74166(ic);
    DELAY_74166();
    clock74166(ic);

    if (!flag)
      continue;
    else if (value != 0)
    {
      value = 0;
      break;
    }
    else
      value = keyset[i];
  }

  return value;
}

static uint8_t readFlagChannel(const IC74166* ic, int channel)
{
  uint8_t value = 0;

  setAddress74166(ic, channel);
  DELAY_74166();

  setModeToParallel74166(ic);
  DELAY_74166();
  clock74166(ic);
  setModeToSerial74166(ic);
  DELAY_74166();

  for (int i = 0; i < 8; ++i)
  {
    int flag = peak74166(ic);
    DELAY_74166();
    clock74166(ic);

    value <<= 1;
    value |= flag;
  }

  return value;
}

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  init74166(&pin74166);

  buffer0.channel.channelNumber = 0;
  buffer1.channel.channelNumber = 1;
  buffer2.channel.channelNumber = 2;
  buffer3.channel.channelNumber = 3;

  Keyboard.begin();

  // for Debug
  pinMode(9, INPUT);
}

void loop_debug()
{
  for (int i = 0; i < 4; ++i)
  {
    setAddress74166(&pin74166, i);
    DELAY_74166();
    Serial.println(digitalRead(9));
  }
}

void loop()
{
  delay(10);

  for (int address = 0; address < 4; ++address)
  {
    uint8_t data = readDataChannel(&pin74166, address, keytable[address]);
    buffer0.channel.data[address] = data;
  }

  DELAY_74166();
  
  buffer0.channel.modifier = readFlagChannel(&pin74166, 4);

  buffer1.channel.data[0] = analogRead(A0) >> 2;
  buffer2.channel.data[0] = analogRead(A1) >> 2;

  Keyboard.sendReport((KeyReport*)&buffer0);
  Keyboard.sendReport((KeyReport*)&buffer1);
  Keyboard.sendReport((KeyReport*)&buffer2);
  Keyboard.sendReport((KeyReport*)&buffer3);
}
