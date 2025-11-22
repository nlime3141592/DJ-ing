#include <assert.h>
#include <Windows.h>

#include "LoopBase.h"

#ifdef _DEBUG
#include "djsw_testing_main.h"
#endif

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

#ifdef _DEBUG
    TestInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif

    while (LoopUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
#ifdef _DEBUG
        TestUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif
    }
    
    LoopFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

#ifdef _DEBUG
    TestFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#endif

    return 0;
}
