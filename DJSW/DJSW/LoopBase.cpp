#include "LoopBase.h"

#include "LoopHID.h"
#include "LoopAudio.h"
#include "LoopMessage.h"
#include "LoopRender.h"

#include "LoopInterrupt.h"

#define CORE_INDEX_HID 0
#define CORE_INDEX_AUDIO 1

static HIDParams _hidParams;
static AudioParams _audioParams;
static RenderParams _renderParams;

int WINAPI LoopInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    BOOL execResult;
    HRESULT hr;

    // 실시간 멀티코어 처리를 위한 설정
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    ULONG_PTR defaultProcessAffinityMask;
    ULONG_PTR defaultSystemAffinityMask;
    execResult = GetProcessAffinityMask(
        GetCurrentProcess(),
        &defaultProcessAffinityMask,
        &defaultSystemAffinityMask
    );
    assert(execResult);

    _hidParams.loopBaseParams.threadAffinityMask = 1ULL << CORE_INDEX_HID;
    _hidParams.loopBaseParams.interruptNumber = DJSW_INT_NULL;
    _hidParams.loopBaseParams.threadHandle = CreateThread(
        NULL,
        0,
        HIDMain,
        &_hidParams,
        0,
        &_hidParams.loopBaseParams.threadId);
    assert(_hidParams.loopBaseParams.threadHandle != NULL);

    _audioParams.loopBaseParams.threadAffinityMask = 1ULL << CORE_INDEX_AUDIO;
    _audioParams.loopBaseParams.interruptNumber = DJSW_INT_NULL;
    _audioParams.loopBaseParams.threadHandle = CreateThread(
        NULL,
        0,
        AudioMain,
        &_audioParams,
        0,
        &_audioParams.loopBaseParams.threadId);
    assert(_audioParams.loopBaseParams.threadHandle != NULL);

    // 실행되는 코어 위치 고정
    if (info.dwNumberOfProcessors >= 2)
    {
        SetThreadAffinityMask(_hidParams.loopBaseParams.threadHandle, _hidParams.loopBaseParams.threadAffinityMask);
        SetThreadAffinityMask(_audioParams.loopBaseParams.threadHandle, _audioParams.loopBaseParams.threadAffinityMask);
    }

    // 완전 실시간 처리 스레드로 고정
    if (info.dwNumberOfProcessors >= 8)
    {
        SetThreadPriority(_audioParams.loopBaseParams.threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
        SetThreadPriority(_hidParams.loopBaseParams.threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
    }

    MessageInit();
    RenderInit(hInstance, NULL, NULL, nCmdShow);

    return 1;
}

int WINAPI LoopUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    MSG msg;

    // Windows Message Queue 기반 메시지 확인
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return 0;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MessageUpdate();
    RenderUpdate(hInstance, NULL, NULL, nCmdShow);

    return 1;
}

int WINAPI LoopFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _hidParams.loopBaseParams.interruptNumber = DJSW_INT_LOOPEND;
    _audioParams.loopBaseParams.interruptNumber = DJSW_INT_LOOPEND;

    WaitForSingleObject(_hidParams.loopBaseParams.threadHandle, 1000);
    WaitForSingleObject(_audioParams.loopBaseParams.threadHandle, 1000);

    RenderFinal(hInstance, NULL, NULL, nCmdShow);

    return 1;
}