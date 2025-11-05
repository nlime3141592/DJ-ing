#include <Keyboard.h>

// digital io pins
#define PIN_S0 16
#define PIN_S1 15
#define PIN_S2 16

#define PIN_SHLD 7
#define PIN_CLR 2
#define PIN_CLK 3
#define PIN_DOUT 5

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
  { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
  { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 },
  { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 },
  { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 },
  { 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28 },
  { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 },
  { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 },
  { 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58 }
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

// 21 mcs delay
uint8_t ReadFlag(int address, uint8_t* keySet)
{
  LoadParallel(address);

  uint8_t flag = 0;

  for (int i = 0; i < 8; ++i)
  {
    if (digitalRead(PIN_DOUT))
      flag += keySet[i];

    Clock();
  }

  return flag;
}

void setup_buffer()
{
  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferKey[1] = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferMixer[1] = 0;
  pBufferMixer[2] = 0;
  pBufferMixer[3] = 0;
  pBufferMixer[4] = 0;
  pBufferMixer[5] = 0;
  pBufferMixer[6] = 0;
  pBufferMixer[7] = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferDeck1[2] = 0;
  pBufferDeck1[3] = 0;
  pBufferDeck1[5] = 0;
  pBufferDeck1[6] = 0;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferDeck2[2] = 0;
  pBufferDeck2[3] = 0;
  pBufferDeck2[5] = 0;
  pBufferDeck2[6] = 0;
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

  // report buffer initialization
  pBufferKey = buffer;
  pBufferMixer = buffer + 8;
  pBufferDeck1 = buffer + 16;
  pBufferDeck2 = buffer + 24;

  setup_buffer();

  // initial states
  digitalWrite(PIN_CLR, 1);
  digitalWrite(PIN_CLK, 0);
  digitalWrite(PIN_SHLD, 1);
}

void loop()
{
  // 본 코드는 Soft Real Time을 따릅니다.
  // 본 루프가 1 밀리초(1000 마이크로초) 이내에 실행될 수 있도록 코딩하는 것이 권장됩니다.
  // delayMicroseconds(100);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  pBufferKey[0] = ReadFlag(0, keyTable[0]);
  
  for (int i = 2; i < 8; ++i)
  {
    pBufferKey[i] = 0x80 + i;//ReadData(i, keyTable[i]);
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
