#include "LoopRender.h"
#include "LoopRender_Init.h"

#include <string>

#include "djsw_gui_app.h"
#include "djsw_gui_core_api_internal.h"
#include "djsw_util_timer.h"

#include "SampleText.h"
#include <d2d1_1.h>

#define DJSW_TIMER_EVENT_RESIZING 1

static size_t _vertexCount;
static size_t _vertexDrawCount;
static size_t _vertexBegin;

static BOOL _isResizing;

static SampleText _sampleText;

static void WaitForPreviousFrame()
{
    const UINT64 currentFenceValue = _fenceValue;

    _cmdQueue->Signal(_fence.Get(), currentFenceValue);
    
    _fenceValue++;
    if (_fence->GetCompletedValue() < currentFenceValue)
    {
        _fence->SetEventOnCompletion(currentFenceValue, _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);
    }

    _frameIndex = _swapChain0->GetCurrentBackBufferIndex();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int x, y;
    std::wstring log;

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        //SetStandardHIDKeyDown((uint8_t)wParam);
        break;
    case WM_KEYUP:
        //SetStandardHIDKeyUp((uint8_t)wParam);
        break;
    case WM_LBUTTONDOWN:
        x = GET_X_LPARAM(lParam);
        y = GET_Y_LPARAM(lParam);
        log = L"WM_LBUTTONDOWN: " + std::to_wstring(x) + L", " + std::to_wstring(y) + L"\n";
        OutputDebugStringW(log.c_str());
        break;

    // 싱글 스레드 환경에서 PeekMessage Blocking 현상을 파훼하고자 작성한 코드
    case WM_ENTERSIZEMOVE:
        _isResizing = true;
        SetTimer(hWnd, DJSW_TIMER_EVENT_RESIZING, 8, NULL);
        break;
    case WM_EXITSIZEMOVE:
        _isResizing = false;
        KillTimer(hWnd, DJSW_TIMER_EVENT_RESIZING);
        break;
    case WM_TIMER:
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI RenderInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // -------------------- 윈도우 생성 --------------------
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DJSW";
    RegisterClassEx(&wc);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"DJSW",
        style, CW_USEDEFAULT, CW_USEDEFAULT, 1366, 256,
        nullptr, nullptr, hInstance, nullptr
    );
    ShowWindow(hwnd, nCmdShow);

#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
    }
#endif

    // -------------------- DXGI 팩토리 및 디바이스 --------------------
    _hwnd = hwnd;
    PipelineInit();

    // -------------------- D2D - D3D 간 연동 --------------------
    _sampleText.Init(hwnd);

    // -------------------- UI 구현 초기화 --------------------
    OnGuiInit_Core();
    OnGuiInit_App();

    return 1;
}

void SetMode_Line()
{
    _cmdList->SetPipelineState(_psoState_Line.Get());
    _cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    //_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void SetMode_Triangle()
{
    _cmdList->SetPipelineState(_psoState_Triangle.Get());
    _cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

djErrorCode AddVertices(void* vertices, size_t szVertex, size_t cntVertex)
{
    if (_vertexCount + cntVertex > DJSW_VERTEX_CAPACITY)
        return DJSW_ERR_ERROR;

    int offset = szVertex * _vertexCount;
    int byteCount = szVertex * cntVertex;

    memcpy(_pVertexDataBegin + offset, vertices, byteCount);
    _vertexCount += cntVertex;
    _vertexDrawCount += cntVertex;

    return DJSW_ERR_NO_ERROR;
}

void DrawCall()
{
    // 실제 그리기 작업을 수행하는 명령
    for (size_t i = 0; i < _vertexDrawCount; i += DJSW_VERTEX_THROUGHPUT)
    {
        size_t drawCount = _vertexDrawCount - i;

        if (drawCount > DJSW_VERTEX_THROUGHPUT)
            drawCount = DJSW_VERTEX_THROUGHPUT;

        _cmdList->DrawInstanced(drawCount, 1, _vertexBegin, 0);
        _vertexBegin += drawCount;
        _vertexDrawCount -= drawCount;
    }
}

void TestRender2D()
{
    _d3d11On12Device->AcquireWrappedResources(_wrappedBackBuffers[_frameIndex].GetAddressOf(), 1);

    _d2dDeviceContext->SetTarget(_d2dRenderTargets[_frameIndex].Get());
    _d2dDeviceContext->BeginDraw();

    // TODO: Actual 2D rendering here.
    {
        RECT rc;
        GetClientRect(_hwnd, &rc);

        D2D1_RECT_F layoutRect = D2D1::RectF(
            static_cast<FLOAT>(rc.left),
            static_cast<FLOAT>(rc.top),
            static_cast<FLOAT>(rc.right),
            static_cast<FLOAT>(rc.bottom)
        );

        std::wstring text = L"Hello World";

        _d2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Translation(10, 10));
        _d2dDeviceContext->DrawText(
            text.c_str(),
            static_cast<UINT32>(text.size()),
            _sampleText._pTextFormat,
            &layoutRect,
            _sampleText._pBlackBrush
        );
    }

    _d2dDeviceContext->EndDraw();

    _d3d11On12Device->ReleaseWrappedResources(_wrappedBackBuffers[_frameIndex].GetAddressOf(), 1);
    _d3d11DeviceContext->Flush();
}

int WINAPI RenderUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // -------------------- 렌더링 --------------------
    
    // 커맨드 준비
    _cmdAllocator->Reset();
    _cmdList->Reset(_cmdAllocator.Get(), _psoState_Triangle.Get());

    _cmdList->SetGraphicsRootSignature(_rootSignature.Get());

    auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
        _rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        _frameIndex,
        _rtvDescriptorSize
    );

    CD3DX12_RESOURCE_BARRIER transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
        _renderTargets[_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    
    _cmdList->ResourceBarrier(1, &transition1);
    _cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // 기본 렌더링 색상 칠하기
    const float clearColor[] = { 0.13f, 0.13f, 0.13f, 1.0f };
    _cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    _cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);
    OnGuiUpdate_Core(_cmdList);

    // TODO: Render 2D here.
    //TestRender2D();

    // 렌더 타겟 상태 되돌리기
    CD3DX12_RESOURCE_BARRIER transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
        _renderTargets[_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    _cmdList->ResourceBarrier(1, &transition2);

    // 커맨드 리스트 닫기
    _cmdList->Close();

    // 실행
    ID3D12CommandList* ppCommandLists[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    //_swapChain0->Present(1, 0); // 수직 동기화 1
    _swapChain0->Present(0, 0); // 수직 동기화 0 (즉시 렌더링)
    WaitForPreviousFrame();

    _vertexCount = 0;
    _vertexDrawCount = 0;
    _vertexBegin = 0;

    return 1;
}

int WINAPI RenderFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    OnGuiFinal_Core();

    WaitForPreviousFrame();
    CloseHandle(_fenceEvent);

    return 1;
}