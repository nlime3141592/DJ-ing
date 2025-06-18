#include "djinput.h"

InputController ictrl = { 0 };
InputDeck ideck1 = { 0 };
InputDeck ideck2 = { 0 };

#define DIR_ROTARY_CW 1
#define DIR_ROTARY_CCW -1
#define DIR_ROTARY_IDLE 0
#define DIR_ROTARY_UNDEFINED 0

static int8_t rotaryPatterns[16] = {
  DIR_ROTARY_IDLE, DIR_ROTARY_CW, DIR_ROTARY_CCW, DIR_ROTARY_UNDEFINED,
  DIR_ROTARY_CCW, DIR_ROTARY_IDLE, DIR_ROTARY_UNDEFINED, DIR_ROTARY_CW,
  DIR_ROTARY_CW, DIR_ROTARY_UNDEFINED, DIR_ROTARY_IDLE, DIR_ROTARY_CCW,
  DIR_ROTARY_UNDEFINED, DIR_ROTARY_CCW, DIR_ROTARY_CW, DIR_ROTARY_IDLE
};

uint8_t analogReadUint8(uint8_t pin, bool toComplement = false)
{
  uint8_t value = (uint8_t)(analogRead(pin) >> 2);

  if (toComplement)
    return 255 - value;
  else
    return value;
}

int8_t rotaryRead(uint8_t* state, uint8_t phase)
{
  *state = *state << 2 | phase;
  *state &= 15;

  return rotaryPatterns[*state];
}