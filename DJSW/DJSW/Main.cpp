#include <Windows.h>

#include <assert.h>
#include "LoopBase.h"

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

    // Message Loop
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        // Windows Message Queue 기반 메시지 확인
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LoopUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }

    LoopFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return 0;
}
