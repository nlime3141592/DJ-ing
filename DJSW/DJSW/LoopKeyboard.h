#pragma once

#include <immintrin.h> // for AVX2 Instruction Set
#include <stdint.h>
#include <Windows.h>

#define KEYBOARD_KEY_COUNT 256

// NOTE: virtual key lists == (https://learn.microsoft.com/ko-kr/windows/win32/inputdev/virtual-key-codes)
BOOL GetKeyDown(int virtualKeyCode);
BOOL GetKey(int virtualKeyCode);
BOOL GetKeyUp(int virtualKeyCode);

void KeyboardInit();
void KeyboardLoop();
void KeyboardFinal();