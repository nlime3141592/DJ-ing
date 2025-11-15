#include <Windows.h>

#include <assert.h>
#include "LoopBase.h"

#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

#include <string>

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

    while (LoopUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        // Debug logic here.
    }
    
    LoopFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return 0;
}
