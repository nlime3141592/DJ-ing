#include "LoopKeyboard.h"

static uint8_t _keyStateMask[32];
static __m256i _keyStateMaskVector;

static uint8_t _keyboardKeyStates[KEYBOARD_KEY_COUNT];

static void HandleKeyStates()
{
	for (int i = 0; i < KEYBOARD_KEY_COUNT; i += 32)
	{
		__m256i vector = _mm256_loadu_si256((const __m256i*)(_keyboardKeyStates + i));
		vector = _mm256_add_epi8(vector, vector); // 1bit left shift
		vector = _mm256_and_epi32(vector, _keyStateMaskVector);
		_mm256_storeu_si256((__m256i*)(_keyboardKeyStates + i), vector);
	}

	for (int i = 0; i < KEYBOARD_KEY_COUNT; ++i)
	{
		int pressed = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
		_keyboardKeyStates[i] |= pressed;
	}
}

static void ClearKeyboardInputs()
{
	memset(_keyboardKeyStates, 0x00, sizeof(_keyboardKeyStates));
}

BOOL GetKeyDown(int virtualKeyCode)
{
	return _keyboardKeyStates[virtualKeyCode] == 0x01;
}

BOOL GetKey(int virtualKeyCode)
{
	return _keyboardKeyStates[virtualKeyCode] & 0x01;
}

BOOL GetKeyUp(int virtualKeyCode)
{
	return _keyboardKeyStates[virtualKeyCode] == 0x02;
}

void KeyboardInit()
{
	memset(_keyStateMask, 0x03, sizeof(_keyStateMask));
	_keyStateMaskVector = _mm256_loadu_si256((const __m256i*)_keyStateMask);

	ClearKeyboardInputs();
}

void KeyboardLoop()
{
	HandleKeyStates();
}

void KeyboardFinal()
{
	// This block is intentionally empty blank.
}