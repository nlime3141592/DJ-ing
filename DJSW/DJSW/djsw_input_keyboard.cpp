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
	//int deckFlag = 0;
	//deckFlag |= GetKey_Keyboard(VK_F1) << 0;
	//deckFlag |= GetKey_Keyboard(VK_F2) << 1;
	//deckFlag |= GetKey_Keyboard(VK_F3) << 2;
	//deckFlag |= GetKey_Keyboard(VK_F4) << 3;
	//deckFlag |= 1 << (_defaultDeckHandling - 1);

	SetKeyStateFromExternal(DJSW_HID_PAD11, ((GetAsyncKeyState(DJSW_VK_PAD11) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD12, ((GetAsyncKeyState(DJSW_VK_PAD12) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD13, ((GetAsyncKeyState(DJSW_VK_PAD13) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD14, ((GetAsyncKeyState(DJSW_VK_PAD14) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD15, ((GetAsyncKeyState(DJSW_VK_PAD15) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD16, ((GetAsyncKeyState(DJSW_VK_PAD16) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD17, ((GetAsyncKeyState(DJSW_VK_PAD17) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PAD18, ((GetAsyncKeyState(DJSW_VK_PAD18) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(DJSW_HID_PADFN11, ((GetAsyncKeyState(DJSW_VK_PADFN11) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PADFN12, ((GetAsyncKeyState(DJSW_VK_PADFN12) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PADFN13, ((GetAsyncKeyState(DJSW_VK_PADFN13) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_PADFN14, ((GetAsyncKeyState(DJSW_VK_PADFN14) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(DJSW_HID_PLAY1, ((GetAsyncKeyState(DJSW_VK_SEL1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_CUE1, ((GetAsyncKeyState(DJSW_VK_SEL1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_LD1, ((GetAsyncKeyState(DJSW_VK_LD1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_LD2, ((GetAsyncKeyState(DJSW_VK_LD2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_LD3, ((GetAsyncKeyState(DJSW_VK_LD3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_LD4, ((GetAsyncKeyState(DJSW_VK_LD4) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SEL1, ((GetAsyncKeyState(DJSW_VK_SEL1) & 0x8001) != 0) ? true : false);

	//SetKeyStateFromExternal(HID_RESET, ((GetAsyncKeyState(DJSW_VK_RESET) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(DJSW_HID_SPLIT1, ((GetAsyncKeyState(DJSW_VK_SPLIT1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT2, ((GetAsyncKeyState(DJSW_VK_SPLIT2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT3, ((GetAsyncKeyState(DJSW_VK_SPLIT3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(DJSW_HID_SPLIT4, ((GetAsyncKeyState(DJSW_VK_SPLIT4) & 0x8001) != 0) ? true : false);
}

void InputFinal_Keyboard()
{

}