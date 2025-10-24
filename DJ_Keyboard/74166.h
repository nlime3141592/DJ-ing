#ifndef _74166_H
#define _74166_H

#include <Arduino.h>

#define DELAY_MCS_74166 10
#define DELAY_74166() delayMicroseconds(DELAY_MCS_74166)
// #define DELAY_74166() delay(DELAY_MCS_74166)

typedef struct
{
  uint8_t pinS0;
  uint8_t pinS1;
  uint8_t pinS2;
  uint8_t pinSHLD;
  uint8_t pinCLR;
  uint8_t pinCLK;
  uint8_t pinCLKINH;
  uint8_t pinDIN;
  uint8_t pinDOUT;

  uint8_t pinDA;
  uint8_t pinDB;
  uint8_t pinDC;
  uint8_t pinDD;
  uint8_t pinDE;
  uint8_t pinDF;
  uint8_t pinDG;
  uint8_t pinDH;
} IC74166;

void init74166(const IC74166* ic);
void clock74166(const IC74166* ic);
void write74166(const IC74166* ic, uint8_t data);
int peak74166(const IC74166* ic);
void setAddress74166(const IC74166* ic, uint8_t address);
void setMode74166(const IC74166* ic, uint8_t mode);
void setModeToParallel74166(const IC74166* ic);
void setModeToSerial74166(const IC74166* ic);

#endif