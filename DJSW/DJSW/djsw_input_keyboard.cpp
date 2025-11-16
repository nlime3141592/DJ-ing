#include "djsw_input_keyboard.h"

#include <Windows.h>

#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"
#include "djsw_input_keyboard_controls.h"

#define MAX_KEY_CAPACITY 256

static int _defaultDeckHandling;

void InputInit_Keyboard()
{
	_defaultDeckHandling = 1;
}

void InputUpdate_Keyboard()
{
	// TODO:
	// 키보드에서 키+덱번호 또는 덱번호+키 조합 모두가 사용 가능한 상태.
	// 반드시 덱번호+키 순서를 강제할 방안을 마련.
	for (int i = 0; i < 4; ++i)
	{
		if ((GetAsyncKeyState(VK_F1 + i) & 0x8001) == 0)
			continue;

		uint8_t padOffset = DJSW_HID_PAD11 + i * 0x10 - 1;
		SetKeyStateFromExternal(padOffset + 1, ((GetAsyncKeyState(DJSW_VK_PAD1) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 2, ((GetAsyncKeyState(DJSW_VK_PAD2) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 3, ((GetAsyncKeyState(DJSW_VK_PAD3) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 4, ((GetAsyncKeyState(DJSW_VK_PAD4) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 5, ((GetAsyncKeyState(DJSW_VK_PAD5) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 6, ((GetAsyncKeyState(DJSW_VK_PAD6) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 7, ((GetAsyncKeyState(DJSW_VK_PAD7) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padOffset + 8, ((GetAsyncKeyState(DJSW_VK_PAD8) & 0x8001) != 0) ? true : false);

		uint8_t padfnOffset = DJSW_HID_PADFN11 + i * 0x30 - 1;
		SetKeyStateFromExternal(padfnOffset + 1, ((GetAsyncKeyState(DJSW_VK_PADFN1) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padfnOffset + 2, ((GetAsyncKeyState(DJSW_VK_PADFN2) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padfnOffset + 3, ((GetAsyncKeyState(DJSW_VK_PADFN3) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(padfnOffset + 4, ((GetAsyncKeyState(DJSW_VK_PADFN4) & 0x8001) != 0) ? true : false);

		SetKeyStateFromExternal(DJSW_HID_PLAY1 + i, ((GetAsyncKeyState(DJSW_VK_PLAY) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(DJSW_HID_CUE1 + i, ((GetAsyncKeyState(DJSW_VK_CUE) & 0x8001) != 0) ? true : false);
		SetKeyStateFromExternal(DJSW_HID_LD1 + i, ((GetAsyncKeyState(DJSW_VK_LD) & 0x8001) != 0) ? true : false);
	}

	SetKeyStateFromExternal(DJSW_HID_SEL1, ((GetAsyncKeyState(DJSW_VK_SEL1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SEL2, ((GetAsyncKeyState(DJSW_VK_SEL2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SEL3, ((GetAsyncKeyState(DJSW_VK_SEL3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SEL4, ((GetAsyncKeyState(DJSW_VK_SEL4) & 0x8001) != 0) ? true : false);

	//SetKeyStateFromExternal(HID_RESET, ((GetAsyncKeyState(DJSW_VK_RESET) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(DJSW_HID_SPLIT1, ((GetAsyncKeyState(DJSW_VK_SPLIT1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT2, ((GetAsyncKeyState(DJSW_VK_SPLIT2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT3, ((GetAsyncKeyState(DJSW_VK_SPLIT3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT4, ((GetAsyncKeyState(DJSW_VK_SPLIT4) & 0x8001) != 0) ? true : false);
}

void InputFinal_Keyboard()
{

}