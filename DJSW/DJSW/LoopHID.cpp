#include "LoopHID.h"

#include <assert.h>

#include "djsw_input_keyboard.h"
#include "djsw_input_hid.h"

#define DJSW_MAX_HID_HANDLER_COUNT 16

#define DJSW_IO_MUTEX_COUNT 8

static hid_device* _hidDevice;
static BOOL _isHidDeviceConnected;

static uint8_t _hidBuffer[DJSW_HID_BUFFER_LENGTH];
static uint8_t _hidKeyStates[DJSW_HID_KEY_COUNT];

static djHidDigitalReport* _pKeyboardReport;
static djHidAnalogReport* _pMixerReport;
static djHidAnalogReport* _pDeckReport1;
static djHidAnalogReport* _pDeckReport2;

HidMessageQueue hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_COUNT];

// -------------------- HID device communication handling --------------------
static hid_device* GetDeviceOrNull()
{
	hid_device_info* devInfo = hid_enumerate(HID_VID, HID_PID);
	hid_device_info* curInfo = devInfo;

	if (!curInfo)
	{
		hid_free_enumeration(devInfo);
		return NULL;
	}

	while (curInfo)
	{
		// you can print current HID device information here.

		curInfo = curInfo->next;
	}

	hid_free_enumeration(devInfo);

	hid_device* device = hid_open(HID_VID, HID_PID, NULL);

	if (!device)
	{
		return NULL;
	}

	hid_set_nonblocking(device, true);

	return device;
}

static void ShiftKeyStates(uint8_t keyStates[DJSW_HID_KEY_COUNT])
{
	for (int i = 0; i < DJSW_HID_KEY_COUNT; ++i)
	{
		keyStates[i] <<= 1;
	}
}

static void CaptureKeyStates(uint8_t keyStates[DJSW_HID_KEY_COUNT])
{
	for (int i = 0; i < DJSW_HID_KEY_COUNT; ++i)
	{
		keyStates[i] |= _hidKeyStates[i] & 1;
	}
}

static void ClearKeyStates()
{
	memset(_hidBuffer, 0x00, sizeof(_hidBuffer));
	memset(_hidKeyStates, 0x00, sizeof(_hidKeyStates));
}

static void InputInit_HID()
{
	_hidDevice = NULL;
	_isHidDeviceConnected = false;

	ClearKeyStates();

	_pKeyboardReport = (djHidDigitalReport*)(_hidBuffer + 1);
	_pMixerReport = (djHidAnalogReport*)(_hidBuffer + 9);
	_pDeckReport1 = (djHidAnalogReport*)(_hidBuffer + 17);
	_pDeckReport2 = (djHidAnalogReport*)(_hidBuffer + 25);
}

static bool CheckHidDeviceConnection()
{
	if (!_hidDevice)
	{
		_hidDevice = GetDeviceOrNull();

		if (!_hidDevice)
		{
			// TODO: Sleep() 함수가 꼭 필요할까? 고민해보기.
			//Sleep(8);

			return true;
		}

		_isHidDeviceConnected = true;
		OutputDebugStringW(L"HID 장치가 연결되었습니다.\n");
	}

	int result = hid_read(_hidDevice, (uint8_t*)&_hidBuffer, sizeof(_hidBuffer));

	if (result < 0)
	{
		_hidDevice = NULL;
		_isHidDeviceConnected = false;
		ClearKeyStates();

		OutputDebugStringW(L"HID 장치와의 연결이 끊어졌습니다.\n");
		return false;
	}
	else
	{
		return result > 0;
	}
}

static void InputUpdate_HID()
{
	for (int i = 0; i < 6; ++i)
	{
		uint8_t data = _pKeyboardReport->data[i];

		if (data != 0)
			_hidKeyStates[data] |= 0x01;
	}
}

static void InputPublish(HidMessageQueue* queue)
{
	HidMessage message;
	message.messageType = DJSW_HID_MESSAGE_TYPE_DIGITAL;

	for (int i = 0; i < DJSW_HID_KEY_COUNT; ++i)
	{
		int b0 = _hidKeyStates[i] & 0x01;
		int b1 = _hidKeyStates[i] & 0x02;

		message.hidKey = i;

		if (b0)
		{
			if (!b1)
			{
				message.message = DJSW_HID_MESSAGE_KEY_DOWN;
				queue->Push(&message);
			}

			message.message = DJSW_HID_MESSAGE_KEY_PRESS;
			queue->Push(&message);
		}
		else if (b1)
		{
			message.message = DJSW_HID_MESSAGE_KEY_UP;
			queue->Push(&message);
		}
	}
}

static void InputFinal_HID()
{
	if (!_hidDevice)
	{
		hid_close(_hidDevice);
		_hidDevice = NULL;
		_isHidDeviceConnected = false;
	}
}

// -------------------- djsw_input_hid.h implementations --------------------
void SetKeyStateFromExternal(uint8_t hidKey, bool isPressed)
{
	if (isPressed)
	{
		_hidKeyStates[hidKey] |= 0x01;
	}
}

bool IsHidConnected()
{
	return _isHidDeviceConnected;
}

HidMessageQueue::HidMessageQueue() :
	head(0),
	tail(0)
{
	memset(queue, 0x00, sizeof(queue));
}

bool HidMessageQueue::Push(HidMessage* message)
{
	int t = tail.load(std::memory_order_relaxed);
	int next = (t + 1) % DJSW_HID_MESSAGE_QUEUE_CAPACITY;

	if (next == head.load(std::memory_order_acquire))
		return false;

	queue[t] = *message; // 데이터를 먼저 저장
	tail.store(next, std::memory_order_release); // 인덱스를 나중에 올려줌

	return true;
}

bool HidMessageQueue::Pop(HidMessage* message)
{
	int h = head.load(std::memory_order_relaxed);

	if (h == tail.load(std::memory_order_acquire))
		return false;

	*message = queue[h];
	head.store((h + 1) % DJSW_HID_MESSAGE_QUEUE_CAPACITY, std::memory_order_release);

	return true;
}

bool GetKeyDown(uint8_t hidKey)
{
	return (_hidKeyStates[hidKey] & 0x03) == 0x01;
}

bool GetKey(uint8_t hidKey)
{
	return (_hidKeyStates[hidKey] & 0x01) == 0x01;
}

bool GetKeyUp(uint8_t hidKey)
{
	return (_hidKeyStates[hidKey] & 0x03) == 0x02;
}

uint8_t GetAnalogMixer(int index)
{
	return _pMixerReport->data[index];
}

uint8_t GetAnalogDeck1(int index)
{
	return _pDeckReport1->data[index];
}

uint8_t GetAnalogDeck2(int index)
{
	return _pDeckReport2->data[index];
}

// -------------------- LoopHid.h implementations --------------------
void InputInit()
{
	InputInit_HID();
	InputInit_Keyboard();
}

void InputUpdate()
{
	CheckHidDeviceConnection();
	ShiftKeyStates(_hidKeyStates);
	InputUpdate_HID();
	InputUpdate_Keyboard();

	for (int i = 0; i < DJSW_HID_MESSAGE_QUEUE_COUNT; ++i)
	{
		InputPublish(hidMessageQueues + i);
	}
}

void InputFinal()
{
	InputFinal_HID();
	InputFinal_Keyboard();
}

DWORD WINAPI HidMain(LPVOID lpParams)
{
	HidParams* hidParams = (HidParams*)lpParams;

	InputInit();

	while (hidParams->loopBaseParams.interruptNumber != 1)
	{
		InputUpdate();
	}

	InputFinal();

	return 1;
}