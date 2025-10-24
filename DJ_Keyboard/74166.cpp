#include "74166.h"

static void init_safe_input(uint8_t pin)
{
  if (pin == 0)
    return;

  pinMode(pin, INPUT);
}

static void init_safe_output(uint8_t pin)
{
  if (pin == 0)
    return;

  pinMode(pin, OUTPUT);
}

static void write_safe(uint8_t pin, uint8_t data)
{
  if (pin == 0)
    return;

  digitalWrite(pin, data);
}

void init74166(const IC74166* ic)
{
  init_safe_output(ic->pinS0);
  init_safe_output(ic->pinS1);
  init_safe_output(ic->pinS2);

  init_safe_output(ic->pinSHLD);
  init_safe_output(ic->pinCLR);
  init_safe_output(ic->pinCLK);
  init_safe_output(ic->pinCLKINH);

  init_safe_output(ic->pinDIN);
  init_safe_input(ic->pinDOUT);

  init_safe_output(ic->pinDA);
  init_safe_output(ic->pinDB);
  init_safe_output(ic->pinDC);
  init_safe_output(ic->pinDD);
  init_safe_output(ic->pinDE);
  init_safe_output(ic->pinDF);
  init_safe_output(ic->pinDG);
  init_safe_output(ic->pinDH);

  write_safe(ic->pinCLK, 0);
  write_safe(ic->pinSHLD, 1);
}

void clock74166(const IC74166* ic)
{
    digitalWrite(ic->pinCLK, 1);
    delayMicroseconds(DELAY_MCS_74166);
    digitalWrite(ic->pinCLK, 0);
    delayMicroseconds(DELAY_MCS_74166);
}

void write74166(const IC74166* ic, uint8_t data)
{
  digitalWrite(ic->pinDA, data & 0x80);
  digitalWrite(ic->pinDB, data & 0x40);
  digitalWrite(ic->pinDC, data & 0x20);
  digitalWrite(ic->pinDD, data & 0x10);
  digitalWrite(ic->pinDE, data & 0x08);
  digitalWrite(ic->pinDF, data & 0x04);
  digitalWrite(ic->pinDG, data & 0x02);
  digitalWrite(ic->pinDH, data & 0x01);
}

int peak74166(const IC74166* ic)
{
  int value = digitalRead(ic->pinDOUT);
  return value;
}

void setAddress74166(const IC74166* ic, uint8_t address)
{
  digitalWrite(ic->pinS0, (address & 0x01));
  digitalWrite(ic->pinS1, (address & 0x02));
  digitalWrite(ic->pinS2, (address & 0x04));
}

void setMode74166(const IC74166* ic, uint8_t mode)
{
  digitalWrite(ic->pinSHLD, mode);
}

void setModeToParallel74166(const IC74166* ic)
{
  setMode74166(ic, 0);
}

void setModeToSerial74166(const IC74166* ic)
{
  setMode74166(ic, 1);
}