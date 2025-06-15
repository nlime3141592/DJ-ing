#ifndef _DJINPUT_H
#define _DJINPUT_H

#include <Arduino.h>

typedef union InputController
{
  char rawdata[16];

  struct
  {
    int32_t dSelect;
    uint8_t eq0;
    uint8_t eq1;
    uint8_t eq2;
    uint8_t eq3;
    uint8_t eq4;
    uint8_t eq5;
    uint8_t fx0;
    uint8_t fx1;
    uint8_t vf0;
    uint8_t vf1;
    uint8_t xf;
    uint8_t btnFlag0;
  } interface;
};

typedef union InputDeck
{
  char rawdata[6];

  struct
  {
    int32_t dWheel;
    uint8_t tempo;
    uint8_t btnFlag0;
  } interface;
};

extern InputController ictrl;
extern InputDeck ideck1;
extern InputDeck ideck2;

uint8_t analogReadUint8(uint8_t analogPin, bool toComplement = false);
int8_t rotaryRead(uint8_t* state, uint8_t digitalPin0, uint8_t digitalPin1);

#endif