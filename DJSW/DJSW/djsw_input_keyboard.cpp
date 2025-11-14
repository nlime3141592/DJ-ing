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

	SetKeyStateFromExternal(HID_PAD11, ((GetAsyncKeyState(STDHID_PAD11) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD12, ((GetAsyncKeyState(STDHID_PAD12) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD13, ((GetAsyncKeyState(STDHID_PAD13) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD14, ((GetAsyncKeyState(STDHID_PAD14) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD15, ((GetAsyncKeyState(STDHID_PAD15) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD16, ((GetAsyncKeyState(STDHID_PAD16) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD17, ((GetAsyncKeyState(STDHID_PAD17) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PAD18, ((GetAsyncKeyState(STDHID_PAD18) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(HID_PADFN11, ((GetAsyncKeyState(STDHID_PADFN11) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PADFN12, ((GetAsyncKeyState(STDHID_PADFN12) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PADFN13, ((GetAsyncKeyState(STDHID_PADFN13) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_PADFN14, ((GetAsyncKeyState(STDHID_PADFN14) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(HID_PLAY1, ((GetAsyncKeyState(STDHID_SEL1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_CUE1, ((GetAsyncKeyState(STDHID_SEL1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_LD1, ((GetAsyncKeyState(STDHID_LD1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_LD2, ((GetAsyncKeyState(STDHID_LD2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_LD3, ((GetAsyncKeyState(STDHID_LD3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_LD4, ((GetAsyncKeyState(STDHID_LD4) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_SEL1, ((GetAsyncKeyState(STDHID_SEL1) & 0x8001) != 0) ? true : false);

	//SetKeyStateFromExternal(HID_RESET, ((GetAsyncKeyState(STDHID_RESET) & 0x8001) != 0) ? true : false);

	SetKeyStateFromExternal(HID_SPLIT1, ((GetAsyncKeyState(STDHID_SPLIT1) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_SPLIT2, ((GetAsyncKeyState(STDHID_SPLIT2) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_SPLIT3, ((GetAsyncKeyState(STDHID_SPLIT3) & 0x8001) != 0) ? true : false);
	SetKeyStateFromExternal(HID_SPLIT4, ((GetAsyncKeyState(STDHID_SPLIT4) & 0x8001) != 0) ? true : false);
}

void InputFinal_Keyboard()
{

}