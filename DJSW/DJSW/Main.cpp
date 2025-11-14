#include <Windows.h>

#include <assert.h>
#include "LoopBase.h"

#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

void OnInputHandler(uint8_t hidKey, int keyState)
{
    switch (hidKey)
    {
    case HID_PAD11:
        if (keyState == 0x01)
            OutputDebugStringW(L"Hello, Pad 11 !\n");
        else if (keyState == 0x02)
            OutputDebugStringW(L"Bye, Pad 11 !\n");
    }
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    HRESULT hr;
    
    //HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    assert(hr == S_OK);

    LoopInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    
    RegisterHidHandler(OnInputHandler, 0);

    while (LoopUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        // Debug logic here.
    }
    
    LoopFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return 0;
}
