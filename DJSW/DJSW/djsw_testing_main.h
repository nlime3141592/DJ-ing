#pragma once

// NOTE: 테스트 로직은 렌더링 스레드 내에서 호출됩니다.

#include <Windows.h>

int WINAPI TestInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI TestUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI TestFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);