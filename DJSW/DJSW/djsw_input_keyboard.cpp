#include "djsw_input_keyboard.h"

#include <Windows.h>

#include "KeyboardControls.h"

#define MAX_KEY_CAPACITY 256

static int _defaultDeckHandling;
static int _keyStates[MAX_KEY_CAPACITY];

static int GetKeyDown_Keyboard(int vKey)
{
	_keyStates[vKey] |= ((GetAsyncKeyState(vKey) & 0x8001) != 0) ? 1 : 0;
	return (_keyStates[vKey] & 0x03) == 0x01 ? 1 : 0;
}

static int GetKey_Keyboard(int vKey)
{
	_keyStates[vKey] |= ((GetAsyncKeyState(vKey) & 0x8001) != 0) ? 1 : 0;
	return (_keyStates[vKey] & 0x01) == 0x01 ? 1 : 0;
}

static int GetKeyUp_Keyboard(int vKey)
{
	_keyStates[vKey] |= ((GetAsyncKeyState(vKey) & 0x8001) != 0) ? 1 : 0;
	return (_keyStates[vKey] & 0x03) == 0x02 ? 1 : 0;
}

void InputInit_Keyboard()
{
	_defaultDeckHandling = 1;
	memset(_keyStates, 0x00, sizeof(_keyStates));
}

void InputUpdate_Keyboard()
{
	for (int i = 0; i < MAX_KEY_CAPACITY; ++i)
	{
		_keyStates[i] <<= 1;
	}

	int deckFlag = 0;

	deckFlag |= GetKey_Keyboard(VK_F1) << 0;
	deckFlag |= GetKey_Keyboard(VK_F2) << 1;
	deckFlag |= GetKey_Keyboard(VK_F3) << 2;
	deckFlag |= GetKey_Keyboard(VK_F4) << 3;
	deckFlag |= 1 << (_defaultDeckHandling - 1);

	if (GetKeyDown_Keyboard(STDHID_PAD11)) { OutputDebugStringW(L"Hello Pad11\n"); }
	if (GetKeyDown_Keyboard(STDHID_PAD12)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD13)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD14)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD15)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD16)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD17)) { return;	 }
	if (GetKeyDown_Keyboard(STDHID_PAD18)) { return;	 }

	if (GetKeyDown_Keyboard(STDHID_PADFN11)) { return; }
	if (GetKeyDown_Keyboard(STDHID_PADFN12)) { return; }
	if (GetKeyDown_Keyboard(STDHID_PADFN13)) { return; }
	if (GetKeyDown_Keyboard(STDHID_PADFN14)) { return; }

	if (GetKeyDown_Keyboard(STDHID_PLAY1)) { return; }
	if (GetKeyDown_Keyboard(STDHID_CUE1)) { return; }
	if (GetKeyDown_Keyboard(STDHID_LD1)) { return; }
	if (GetKeyDown_Keyboard(STDHID_LD2)) { return; }
	if (GetKeyDown_Keyboard(STDHID_LD3)) { return; }
	if (GetKeyDown_Keyboard(STDHID_LD4)) { return; }
	if (GetKeyDown_Keyboard(STDHID_SEL1)) { return; }

	if (GetKeyDown_Keyboard(STDHID_RESET)) { return; }

	if (GetKeyDown_Keyboard(STDHID_SPLIT1)) { return; }
	if (GetKeyDown_Keyboard(STDHID_SPLIT2)) { return; }
	if (GetKeyDown_Keyboard(STDHID_SPLIT3)) { return; }
	if (GetKeyDown_Keyboard(STDHID_SPLIT4)) { return; }
}

void InputFinal_Keyboard()
{

}