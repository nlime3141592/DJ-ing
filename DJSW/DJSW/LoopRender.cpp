#include "LoopRender.h"

#include "djsw_gui_app.h"
#include "djsw_gui_core_api_internal.h"

#include "djsw_util_timer.h"
#include <string>

#define DJSW_TIMER_EVENT_RESIZING 1

#define DJSW_VERTEX_THROUGHPUT 0x10000 // 16-bit address space
#define DJSW_VERTEX_CAPACITY 0x100000 // 20-bit address space

static ComPtr<ID3D12CommandQueue> _cmdQueue;

static DXGI_SWAP_CHAIN_DESC1 _swapChainDesc;
static ComPtr<IDXGISwapChain3> _swapChain0;
static ComPtr<IDXGISwapChain1> _swapChain1;
static UINT _frameIndex;

static ComPtr<ID3D12DescriptorHeap> _rtvHeap;
static ComPtr<ID3D12Resource> _renderTargets[FrameCount];
static UINT _rtvDescriptorSize;

ComPtr<ID3D12CommandAllocator> _cmdAllocator;
ComPtr<ID3D12GraphicsCommandList> _cmdList;

static ComPtr<ID3D12Fence> _fence;
static UINT64 _fenceValue = 0;
static HANDLE _fenceEvent;

static ComPtr<ID3D12RootSignature> _rootSignature;

static ComPtr<ID3D12Resource> _vertexBuffer;
static D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
static size_t _vertexCount;

static ComPtr<ID3D12PipelineState> _pipelineState;

static UINT8* _pVertexDataBegin;

static BOOL _isResizing;

/*static void InitPSO_0()
{
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = _rootSignature.Get();
    psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 이 곳에서 파이프라인 상태를 변경할 수 있음.
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
}*/

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

djErrorCode AddVertices(void* vertices, size_t szVertex, size_t cntVertex)
{
    if (_vertexCount + cntVertex > DJSW_VERTEX_CAPACITY)
        return DJSW_ERR_ERROR;

    int offset = szVertex * _vertexCount;
    int byteCount = szVertex * cntVertex;

    memcpy(_pVertexDataBegin + offset, vertices, byteCount);
    _vertexCount += cntVertex;

    return DJSW_ERR_NO_ERROR;
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

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"DJSW",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
    ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    ComPtr<ID3D12Device> device;
    D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    // -------------------- 커맨드 큐 --------------------
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));

    // -------------------- 스왑 체인 --------------------
    _swapChainDesc = {};
    _swapChainDesc.BufferCount = FrameCount;
    //swapChainDesc.Width = 800;
    _swapChainDesc.Width = 3840;
    //swapChainDesc.Height = 600;
    _swapChainDesc.Height = 2160;
    _swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    _swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    _swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    _swapChainDesc.SampleDesc.Count = 1;

    factory->CreateSwapChainForHwnd(
        _cmdQueue.Get(),
        hwnd,
        &_swapChainDesc,
        nullptr,
        nullptr,
        &_swapChain1
    );

    _swapChain1.As(&_swapChain0);
    //UINT frameIndex = _swapChain0->GetCurrentBackBufferIndex();
    _frameIndex = _swapChain0->GetCurrentBackBufferIndex();

    // -------------------- RTV (렌더 타겟 뷰) --------------------
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap));

    _rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < FrameCount; n++) {
        _swapChain0->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n]));
        device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, _rtvDescriptorSize);
    }

    // -------------------- 커맨드 할당자 & 리스트 --------------------
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    _cmdList->Close();

    // -------------------- 펜스 --------------------
    _fenceValue = 0;
    device->CreateFence(_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // -------------------- 정점 버퍼 정의 --------------------
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    // 정점 버퍼의 크기 정의
    //CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(djVertexRGB) * DJSW_VERTEX_CAPACITY);

    // 업로드 힙으로 버텍스 버퍼 생성
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_vertexBuffer)
    );

    //UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    _vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&_pVertexDataBegin));
    //memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    //_vertexBuffer->Unmap(0, nullptr);

    _vertexBufferView = {};
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(djVertexRGB);
    //_vertexBufferView.SizeInBytes = vertexBufferSize;
    _vertexBufferView.SizeInBytes = sizeof(djVertexRGB) * DJSW_VERTEX_CAPACITY;

    // -------------------- 셰이더 컴파일 --------------------
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
    D3DCompile(
        "struct VS_INPUT { float3 pos: POSITION; float3 col: COLOR; };"
        "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
        "PS_INPUT VSMain(VS_INPUT input){ PS_INPUT o; o.pos=float4(input.pos,1); o.col=input.col; return o; }"
        "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }",
        strlen(
            "struct VS_INPUT { float3 pos: POSITION; float3 col: COLOR; };"
            "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
            "PS_INPUT VSMain(VS_INPUT input){ PS_INPUT o; o.pos=float4(input.pos,1); o.col=input.col; return o; }"
            "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }"
        ),
        nullptr, nullptr, nullptr,
        "VSMain", "vs_5_0", 0, 0, &vertexShader, nullptr
    );
    D3DCompile(
        "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
        "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }",
        strlen(
            "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
            "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }"
        ),
        nullptr, nullptr, nullptr,
        "PSMain", "ps_5_0", 0, 0, &pixelShader, nullptr
    );

    // -------------------- 루트 시그니처 --------------------
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));

    // -------------------- 파이프라인 상태 객체 --------------------
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = _rootSignature.Get();
    psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 이 곳에서 파이프라인 상태를 변경할 수 있음.
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState));

    _isResizing = false;

    OnGuiInit_Core();
    OnGuiInit_App();

    return 1;
}

int WINAPI RenderUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // -------------------- 렌더링 --------------------
    
    // 커맨드 준비
    _cmdAllocator->Reset();
    _cmdList->Reset(_cmdAllocator.Get(), _pipelineState.Get());

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
    //const float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
    const float clearColor[] = { 0.13f, 0.13f, 0.13f, 1.0f };
    _cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    _cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    _cmdList->IASetVertexBuffers(0, 1, &_vertexBufferView);

    OnGuiUpdate_Core(_cmdList);

    // 실제 그리기 작업을 수행하는 명령
    size_t vertexScaler = 3;
    for (size_t i = 0; i < _vertexCount; i += DJSW_VERTEX_THROUGHPUT)
    {
        size_t drawCount = _vertexCount - i;

        if (drawCount > DJSW_VERTEX_THROUGHPUT)
            drawCount = DJSW_VERTEX_THROUGHPUT;

        _cmdList->DrawInstanced(drawCount * vertexScaler, 1, i * vertexScaler, 0);
    }

    CD3DX12_RESOURCE_BARRIER transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
        _renderTargets[_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    _cmdList->ResourceBarrier(1, &transition2);

    _cmdList->Close();

    // 실행
    ID3D12CommandList* ppCommandLists[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    _vertexCount = 0;

    //_swapChain0->Present(1, 0); // 수직 동기화 1
    _swapChain0->Present(0, 0); // 수직 동기화 0 (즉시 렌더링)
    WaitForPreviousFrame();

    return 1;
}

int WINAPI RenderFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    OnGuiFinal_Core();

    WaitForPreviousFrame();
    CloseHandle(_fenceEvent);

    return 1;
}