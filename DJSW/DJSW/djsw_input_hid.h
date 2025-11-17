#pragma once

#include <atomic>
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

#define DJSW_HID_MESSAGE_QUEUE_COUNT 2
#define DJSW_HID_MESSAGE_QUEUE_CAPACITY 1024
#define DJSW_HID_MESSAGE_QUEUE_IDX_AUDIO 0
#define DJSW_HID_MESSAGE_QUEUE_IDX_RENDER 1

#define DJSW_HID_MESSAGE_TYPE_DIGITAL 0
#define DJSW_HID_MESSAGE_TYPE_ANALOG 1

// Bit Mask
#define DJSW_HID_MESSAGE_KEY_PRESS 1
#define DJSW_HID_MESSAGE_KEY_DOWN 2
#define DJSW_HID_MESSAGE_KEY_UP 4

struct HidMessage
{
	uint8_t hidKey;
	uint8_t messageType;
	uint8_t message;
};

class HidMessageQueue
{
public:
	HidMessageQueue();

	bool Push(HidMessage* message);
	bool Pop(HidMessage* message);

private:
	HidMessage queue[DJSW_HID_MESSAGE_QUEUE_CAPACITY];
	std::atomic<int> head;
	std::atomic<int> tail;
};

extern HidMessageQueue hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_COUNT];

void SetKeyStateFromExternal(uint8_t hidKey, bool isPressed);

bool IsHidConnected();

bool GetKeyDown(uint8_t hidKey);
bool GetKey(uint8_t hidKey);
bool GetKeyUp(uint8_t hidKey);

uint8_t GetAnalogMixer(int index);
uint8_t GetAnalogDeck1(int index);
uint8_t GetAnalogDeck2(int index);