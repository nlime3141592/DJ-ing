#include "HIDThread.h"

#include <atomic>

#include "hidapi.h"

#define ID_VENDOR 0x2341
#define ID_PRODUCT 0x8036

#define DIGITAL_KEY_COUNT 256

static uint8_t keyStates[DIGITAL_KEY_COUNT];

static hid_device* device;
static uint8_t buffer[64];

static HIDKeyboardReport* pKeyboardReport;
static HIDAnalogReport* pMixerReport;
static HIDAnalogReport* pDeckReport1;
static HIDAnalogReport* pDeckReport2;

static hid_device* GetDeviceOrNull()
{
	hid_device_info* devInfo = hid_enumerate(ID_VENDOR, ID_PRODUCT);
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

	hid_device* device = hid_open(ID_VENDOR, ID_PRODUCT, NULL);

	if (!device)
	{
		return NULL;
	}

	hid_set_nonblocking(device, true);

	return device;
}

static void HandleInput_Keyboard()
{
	for (int i = 0; i < DIGITAL_KEY_COUNT; ++i)
	{
		keyStates[i] <<= 1;
	}

	for (int i = 0; i < 6; ++i)
	{
		uint8_t data = pKeyboardReport->data[i];
		keyStates[data] |= (data != 0);
	}
}

BOOL GetKeyDown(uint8_t nKey)
{
	return (keyStates[nKey] & 0x03) == 0x01;
}

BOOL GetKey(uint8_t nKey)
{
	return (keyStates[nKey] & 0x01) == 0x01;
}

BOOL GetKeyUp(uint8_t nKey)
{
	return (keyStates[nKey] & 0x03) == 0x02;
}

uint8_t GetAnalog0(int index)
{
	return pMixerReport->data[index];
}

uint8_t GetAnalog1(int index)
{
	return pDeckReport1->data[index];
}

uint8_t GetAnalog2(int index)
{
	return pDeckReport2->data[index];
}

void HIDInit()
{
	device = NULL;

	pKeyboardReport = (HIDKeyboardReport*)(buffer + 1);
	pMixerReport = (HIDAnalogReport*)(buffer + 9);
	pDeckReport1 = (HIDAnalogReport*)(buffer + 17);
	pDeckReport2 = (HIDAnalogReport*)(buffer + 25);
}

void HIDLoop()
{
	if (!device)
	{
		device = GetDeviceOrNull();

		if (device)
		{
			OutputDebugStringW(L"HID 장치가 연결되었습니다.\n");
		}

		return;
	}

	int result = hid_read(device, (uint8_t*)&buffer, sizeof(buffer));

	if (result < 0)
	{
		device = NULL;
		OutputDebugStringW(L"HID 장치와의 연결이 끊어졌습니다.\n");

		return;
	}
	else if (result == 0)
	{
		// no data
		return;
	}
	else
	{
		//reportHandlers[report.index](&report);
		HandleInput_Keyboard();
	}
}

void HIDFinal()
{
	if (!device)
	{
		hid_close(device);
		device = NULL;
	}
}