#include <Keyboard.h>

#define PIN_SHLD 7
#define PIN_Q 5
#define PIN_CLK 3

#define PIN_S0 14
#define PIN_S1 15
#define PIN_S2 16

uint8_t buffer0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t buffer1[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };
uint8_t buffer2[8] = { 0, 2, 0, 0, 0, 0, 0, 0 };
uint8_t buffer3[8] = { 0, 3, 0, 0, 0, 0, 0, 0 };

uint8_t keyset[][8] = {
  { 1, 2, 3, 4, 5, 6, 7, 8 },
  { 9, 10, 11, 12, 13, 14, 15, 16 },
  { 17, 18, 19, 20, 21, 22, 23, 24 },
  { 25, 26, 27, 28, 29, 30, 31, 32 },
  { 33, 34, 35, 36, 37, 38, 39, 40 },
  { 41, 42, 43, 44, 45, 46, 47, 48 }
};

void setaddr(int addr)
{
  digitalWrite(PIN_S0, addr & 1);
  digitalWrite(PIN_S1, addr & 2);
  digitalWrite(PIN_S2, addr & 4);
}

int read_matrix()
{
  digitalWrite(PIN_CLK, 0);
  delayMicroseconds(5);
  digitalWrite(PIN_CLK, 1);
  delayMicroseconds(5);
  return digitalRead(PIN_Q);
}

void update_modifier(int channel)
{
  uint8_t flag = 0;

  for (int i = 0; i < 8; ++i)
  {
    flag <<= 1;
    flag |= read_matrix();
  }

  buffer0[0] = flag;
  buffer1[0] = flag;
  buffer2[0] = flag;
  buffer3[0] = flag;
}

void update_channel(int channel)
{
  // Load shift register.
  setaddr(channel);
  digitalWrite(PIN_SHLD, 0);
  delayMicroseconds(5);
  digitalWrite(PIN_CLK, 0);
  delayMicroseconds(5);
  digitalWrite(PIN_CLK, 1);
  delayMicroseconds(5); // Load end here.
  digitalWrite(PIN_SHLD, 1);
  delayMicroseconds(5);

  for (int i = 0; i < 8; ++i)
  {
    if (!read_matrix())
    {
      continue;
    }
    else if (buffer0[channel] == 0)
    {
      buffer0[channel] = keyset[channel][i];
    }
    else
    {
      buffer0[channel] = 0x00;
      return;
    }
  }
}

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  pinMode(PIN_SHLD, INPUT);
  pinMode(PIN_CLK, OUTPUT);
  pinMode(PIN_Q, INPUT);

  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);

  pinMode(14, INPUT);
  pinMode(15, INPUT);

  digitalWrite(PIN_CLK, 1);
  digitalWrite(PIN_SHLD, 1);

  Keyboard.begin();
}

void loop()
{
  delay(10);

  update_channel(0);
  update_channel(1);
  update_channel(2);
  update_channel(3);
  update_modifier(4);

  buffer1[2] = analogRead(A0) >> 2;
  buffer2[2] = analogRead(A1) >> 2;

  Keyboard.sendReport((KeyReport*)(buffer0));
  Keyboard.sendReport((KeyReport*)(buffer1));
  Keyboard.sendReport((KeyReport*)(buffer2));
  Keyboard.sendReport((KeyReport*)(buffer3));
}
