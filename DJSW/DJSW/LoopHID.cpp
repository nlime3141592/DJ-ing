#include "LoopHID.h"

#include "djsw_input_keyboard.h"

static hid_device* _hidDevice;
static BOOL _isHidDeviceConnected;

static uint8_t _hidBuffer[HID_BUFFER_LENGTH];
static uint8_t _hidKeyStates[HID_KEY_COUNT];

static HIDKeyboardReport* _pKeyboardReport;
static HIDAnalogReport* _pMixerReport;
static HIDAnalogReport* _pDeckReport1;
static HIDAnalogReport* _pDeckReport2;

static uint8_t _stdHidKeyStates[HID_KEY_COUNT];

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

static void HandleVendorHIDKeyStates()
{
	for (int i = 0; i < HID_KEY_COUNT; ++i)
	{
		_hidKeyStates[i] <<= 1;
		_stdHidKeyStates[i] <<= 1;
	}

	for (int i = 0; i < 6; ++i)
	{
		uint8_t data = _pKeyboardReport->data[i];
		_hidKeyStates[data] |= (data != 0);
	}
}

static void HandleStandardHIDKeyStates()
{
	for (int i = 0; i < HID_KEY_COUNT; ++i)
	{
		_stdHidKeyStates[i] <<= 1;
	}
}

static void ClearVendorHIDInputs()
{
	memset(_hidBuffer, 0x00, sizeof(_hidBuffer));
	memset(_hidKeyStates, 0x00, sizeof(_hidKeyStates));
}

static void VendorHIDInit()
{
	_hidDevice = NULL;
	_isHidDeviceConnected = false;

	ClearVendorHIDInputs();

	_pKeyboardReport = (HIDKeyboardReport*)(_hidBuffer + 1);
	_pMixerReport = (HIDAnalogReport*)(_hidBuffer + 9);
	_pDeckReport1 = (HIDAnalogReport*)(_hidBuffer + 17);
	_pDeckReport2 = (HIDAnalogReport*)(_hidBuffer + 25);
}

static void VendorHIDUpdate()
{
	if (!_hidDevice)
	{
		_hidDevice = GetDeviceOrNull();

		if (!_hidDevice)
		{
			HandleStandardHIDKeyStates();
			Sleep(8);
			return;
		}

		_isHidDeviceConnected = true;
		OutputDebugStringW(L"HID 장치가 연결되었습니다.\n");
	}

	int result = hid_read(_hidDevice, (uint8_t*)&_hidBuffer, sizeof(_hidBuffer));

	if (result > 0)
	{
		HandleVendorHIDKeyStates();
	}
	else if (result < 0)
	{
		_hidDevice = NULL;
		_isHidDeviceConnected = false;
		ClearVendorHIDInputs();

		OutputDebugStringW(L"HID 장치와의 연결이 끊어졌습니다.\n");
	}
}

static void VendorHIDFinal()
{
	if (!_hidDevice)
	{
		hid_close(_hidDevice);
		_hidDevice = NULL;
		_isHidDeviceConnected = false;
	}
}

static void ClearStandardHIDInputs()
{
	memset(_stdHidKeyStates, 0x00, sizeof(_stdHidKeyStates));
}

static void StandardHIDInit()
{
	ClearStandardHIDInputs();
}

static void StandardHIDUpdate()
{
	ClearStandardHIDInputs();
	Sleep(10);
}

static void StandardHIDFinal()
{
	// This block is intentionally left blank.
}

BOOL IsVendorHIDConnected()
{
	return _isHidDeviceConnected;
}

BOOL GetVendorHIDKeyDown(uint8_t keycode)
{
	return (_hidKeyStates[keycode] & 0x03) == 0x01;
}

BOOL GetVendorHIDKey(uint8_t keycode)
{
	return (_hidKeyStates[keycode] & 0x01) == 0x01;
}

BOOL GetVendorHIDKeyUp(uint8_t keycode)
{
	return (_hidKeyStates[keycode] & 0x03) == 0x02;
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

void SetStandardHIDKeyDown(uint8_t keycode)
{
	_stdHidKeyStates[keycode] |= 0x01;
}

void SetStandardHIDKeyUp(uint8_t keycode)
{
	_stdHidKeyStates[keycode] &= 0xFE;
}

BOOL GetStandardHIDKeyDown(uint8_t keycode)
{
	return (_stdHidKeyStates[keycode] & 0x03) == 0x01;
}

BOOL GetStandardHIDKey(uint8_t keycode)
{
	return (_stdHidKeyStates[keycode] & 0x01) == 0x01;
}

BOOL GetStandardHIDKeyUp(uint8_t keycode)
{
	return (_stdHidKeyStates[keycode] & 0x03) == 0x02;
}

DWORD WINAPI HIDMain(LPVOID lpParams)
{
	HIDParams* hidParams = (HIDParams*)lpParams;

	VendorHIDInit();
	StandardHIDInit();

	InputInit_Keyboard();

	while (hidParams->loopBaseParams.interruptNumber != 1)
	{
		InputUpdate_Keyboard();
		VendorHIDUpdate();
	}

	VendorHIDFinal();
	StandardHIDFinal();

	return 0;
}