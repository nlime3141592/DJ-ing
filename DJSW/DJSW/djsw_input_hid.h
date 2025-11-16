#pragma once

#include <stdint.h>

#define DJSW_HID_KEY_COUNT 256

// Analog property indicies for Mixer
#define DJSW_IDX_CROSSFADER 0

// Analog property indicies for Deck
#define DJSW_IDX_VOLUME 0
#define DJSW_IDX_TEMPO 1
#define DJSW_IDX_EQ_LO 4
#define DJSW_IDX_EQ_MD 5
#define DJSW_IDX_EQ_HI 6
#define DJSW_IDX_FX 7

class djInputMutex
{
public:
	void (*callback)(void* keyStateBuffer); // DJSW_HID_KEY_COUNT 개의 키 상태를 반환합니다.
	bool ioFlag;

	djInputMutex();
};

void SetKeyStateFromExternal(uint8_t hidKey, bool isPressed);

bool IsHidConnected();

bool GetKeyDown(uint8_t hidKey);
bool GetKey(uint8_t hidKey);
bool GetKeyUp(uint8_t hidKey);

uint8_t GetAnalogMixer(int index);
uint8_t GetAnalogDeck1(int index);
uint8_t GetAnalogDeck2(int index);