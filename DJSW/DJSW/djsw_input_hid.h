#pragma once

#include <stdint.h>

// Analog property indicies for Mixer
#define DJSW_IDX_CROSSFADER 0

// Analog property indicies for Deck
#define DJSW_IDX_VOLUME 0
#define DJSW_IDX_TEMPO 1
#define DJSW_IDX_EQ_LO 4
#define DJSW_IDX_EQ_MD 5
#define DJSW_IDX_EQ_HI 6
#define DJSW_IDX_FX 7

void SetKeyStateFromExternal(uint8_t hidKey, bool isPressed);
void RegisterHidHandler(void (*handler)(uint8_t hidKey, int keyState), int index);

bool IsHidConnected();

bool GetKeyDown(uint8_t hidKey);
bool GetKey(uint8_t hidKey);
bool GetKeyUp(uint8_t hidKey);

uint8_t GetAnalogMixer(int index);
uint8_t GetAnalogDeck1(int index);
uint8_t GetAnalogDeck2(int index);