#include "LoopHID.h"

static uint8_t _keyStateMask[32];
static __m256i _keyStateMaskVector;

static hid_device* _hidDevice;

static uint8_t _hidBuffer[HID_BUFFER_LENGTH];
static uint8_t _hidKeyStates[HID_KEY_COUNT];

static HIDKeyboardReport* _pKeyboardReport;
static HIDAnalogReport* _pMixerReport;
static HIDAnalogReport* _pDeckReport1;
static HIDAnalogReport* _pDeckReport2;

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

static void HandleKeyStates()
{
	for (int i = 0; i < HID_KEY_COUNT; i += 32)
	{
		__m256i vector = _mm256_loadu_si256((const __m256i*)(_hidKeyStates + i));
		vector = _mm256_add_epi8(vector, vector); // 1bit left shift
		vector = _mm256_and_epi32(vector, _keyStateMaskVector);
		_mm256_storeu_si256((__m256i*)(_hidKeyStates + i), vector);
	}

	for (int i = 0; i < 6; ++i)
	{
		uint8_t data = _pKeyboardReport->data[i];
		_hidKeyStates[data] |= (data != 0);
	}
}

static void ClearHIDInputs()
{
	memset(_hidBuffer, 0x00, sizeof(_hidBuffer));
	memset(_hidKeyStates, 0x00, sizeof(_hidKeyStates));
}

BOOL IsHIDConnected()
{
	return _hidDevice != NULL;
}

BOOL GetHIDKeyDown(uint8_t keycode)
{
	return _hidKeyStates[keycode] == 0x01;
}

BOOL GetHIDKey(uint8_t keycode)
{
	return _hidKeyStates[keycode] & 0x01;
}

BOOL GetHIDKeyUp(uint8_t keycode)
{
	return _hidKeyStates[keycode] == 0x02;
}

uint8_t GetAnalogMixer(int32_t idxProperty)
{
	return _pMixerReport->data[idxProperty];
}

uint8_t GetAnalogDeck1(int32_t idxProperty)
{
	return _pDeckReport1->data[idxProperty];
}

uint8_t GetAnalogDeck2(int32_t idxProperty)
{
	return _pDeckReport2->data[idxProperty];
}

void HIDInit()
{
	memset(_keyStateMask, 0x03, sizeof(_keyStateMask));
	_keyStateMaskVector = _mm256_loadu_si256((const __m256i*)_keyStateMask);

	_hidDevice = NULL;

	ClearHIDInputs();

	_pKeyboardReport = (HIDKeyboardReport*)(_hidBuffer + 1);
	_pMixerReport = (HIDAnalogReport*)(_hidBuffer + 9);
	_pDeckReport1 = (HIDAnalogReport*)(_hidBuffer + 17);
	_pDeckReport2 = (HIDAnalogReport*)(_hidBuffer + 25);
}

void HIDLoop()
{
	if (!_hidDevice)
	{
		_hidDevice = GetDeviceOrNull();

		if (!_hidDevice)
			return;

		OutputDebugStringW(L"HID 장치가 연결되었습니다.\n");
	}

	int result = hid_read(_hidDevice, (uint8_t*)&_hidBuffer, sizeof(_hidBuffer));

	if (result > 0)
	{
		HandleKeyStates();
	}
	else if (result < 0)
	{
		_hidDevice = NULL;
		ClearHIDInputs();

		OutputDebugStringW(L"HID 장치와의 연결이 끊어졌습니다.\n");
	}
}

void HIDFinal()
{
	if (!_hidDevice)
	{
		hid_close(_hidDevice);
		_hidDevice = NULL;
	}
}