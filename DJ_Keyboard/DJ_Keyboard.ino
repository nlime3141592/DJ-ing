#include <Keyboard.h>

int rd0 = 0;

int blink = 0;

uint8_t buffer0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t buffer1[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };
uint8_t buffer2[8] = { 0, 2, 0, 0, 0, 0, 0, 0 };
uint8_t buffer3[8] = { 0, 3, 0, 0, 0, 0, 0, 0 };

void update_modifier(int lShift01, int rShift01)
{
  uint8_t flag = 0;

  flag |= (lShift01 << 6);
  flag |= (rShift01 << 2);

  buffer0[0] = flag;
  buffer1[0] = flag;
  buffer2[0] = flag;
  buffer3[0] = flag;
}

void setup()
{
  pinMode(10, INPUT);
  pinMode(A3, INPUT);

  pinMode(14, INPUT);
  pinMode(15, INPUT);

  Keyboard.begin();
}

void loop()
{
  delay(10);

  int rd1 = digitalRead(10);
  blink = 1 - blink;

  update_modifier(digitalRead(14), digitalRead(15));

  if (!rd0 && rd1) // On Key Press
  {
    buffer0[5] = 'a';
  }
  else if (rd0 && !rd1) // On Key Release
  {
    buffer0[5] = 0;
  }

  buffer1[2] = 254 + blink;
  buffer2[3] = 254 + blink;
  buffer3[4] = 254 + blink;
  
  buffer2[4] = analogRead(A3) >> 2;

  Keyboard.sendReport((KeyReport*)(buffer0));
  Keyboard.sendReport((KeyReport*)(buffer1));
  Keyboard.sendReport((KeyReport*)(buffer2));
  Keyboard.sendReport((KeyReport*)(buffer3));

  rd0 = rd1;
}
