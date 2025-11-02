#include "HIDThread.h"

#include <atomic>

#include "hidapi.h"

#define ID_VENDOR 0x2341
#define ID_PRODUCT 0x8036

#define DIGITAL_KEY_COUNT 64
#define DIGITAL_KEY_CAPACITY 128

static std::atomic<uint8_t> keyStates[DIGITAL_KEY_COUNT];
static uint8_t valStates0[6] = { 0 };
static uint8_t valStates1[6] = { 0 };
static uint8_t valStates2[6] = { 0 };

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

static void HandleInput_Digital(HIDReport* report)
{
	for (int i = 0; i < DIGITAL_KEY_COUNT; ++i)
	{
		keyStates[i].store(keyStates[i].load() << 1, std::memory_order_relaxed);
	}

	for (int i = 0; i < 6; ++i)
	{
		uint8_t data = report->data[i];
		keyStates[data].fetch_or(data != 0, std::memory_order_relaxed);
	}
}

static void HandleInput_Analog_Mixer(HIDReport* report)
{
	for (int i = 0; i < sizeof(report->data); ++i)
	{
		valStates0[i] = report->data[i];
	}
}

static void HandleInput_Analog_Deck1(HIDReport* report)
{
	for (int i = 0; i < sizeof(report->data); ++i)
	{
		valStates1[i] = report->data[i];
	}
}

static void HandleInput_Analog_Deck2(HIDReport* report)
{
	for (int i = 0; i < sizeof(report->data); ++i)
	{
		valStates2[i] = report->data[i];
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
	return valStates0[index];
}

uint8_t GetAnalog1(int index)
{
	return valStates1[index];
}

uint8_t GetAnalog2(int index)
{
	return valStates2[index];
}

DWORD WINAPI HIDMain(LPVOID lpParam)
{
	HIDParams* params = (HIDParams*)lpParam;
	hid_device* device = NULL;
	HIDReport report;
	void (*reportHandlers[4])(HIDReport*) = {
		HandleInput_Digital,
		HandleInput_Analog_Mixer,
		HandleInput_Analog_Deck1,
		HandleInput_Analog_Deck2
	};

	OutputDebugStringW(L"새 HID 스레드가 시작되었습니다.\n");

	while (!params->intrHaltThread)
	{
		if (!device)
		{
			device = GetDeviceOrNull();

			if (device)
			{
				OutputDebugStringW(L"HID 장치가 연결되었습니다.\n");
			}

			continue;
		}

		int result = hid_read(device, (uint8_t*)&report, sizeof(report));

		if (result < 0)
		{
			device = NULL;
			OutputDebugStringW(L"HID 장치와의 연결이 끊어졌습니다.\n");

			continue;
		}
		else if (result == 0)
		{
			// no data
			Sleep(1);
			continue;
		}
		else
		{
			reportHandlers[report.index](&report);
			Sleep(1);
		}
	}
	
	OutputDebugStringW(L"HID 스레드가 종료되었습니다.\n");
	return 0;
}