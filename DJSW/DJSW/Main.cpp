#include <windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <stdexcept>
#include <vector>

#include "d3dx12.h"

using namespace Microsoft::WRL;

const UINT FrameCount = 2;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

struct Vertex {
    float position[3];
    float color[3];
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // -------------------- 윈도우 생성 --------------------
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"DX12TriangleClass";
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0, wc.lpszClassName, L"DirectX 12 - Triangle",
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
    ComPtr<ID3D12CommandQueue> commandQueue;
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));

    // -------------------- 스왑 체인 --------------------
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = 800;
    swapChainDesc.Height = 600;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain1;
    factory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1
    );

    ComPtr<IDXGISwapChain3> swapChain;
    swapChain1.As(&swapChain);
    UINT frameIndex = swapChain->GetCurrentBackBufferIndex();

    // -------------------- RTV (렌더 타겟 뷰) --------------------
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

    UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    ComPtr<ID3D12Resource> renderTargets[FrameCount];
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT n = 0; n < FrameCount; n++) {
        swapChain->GetBuffer(n, IID_PPV_ARGS(&renderTargets[n]));
        device->CreateRenderTargetView(renderTargets[n].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    // -------------------- 커맨드 할당자 & 리스트 --------------------
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));

    ComPtr<ID3D12GraphicsCommandList> commandList;
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    commandList->Close();

    // -------------------- 펜스 --------------------
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue = 0;
    device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    auto WaitForPreviousFrame = [&]() {
        const UINT64 currentFenceValue = fenceValue;
        commandQueue->Signal(fence.Get(), currentFenceValue);
        fenceValue++;
        if (fence->GetCompletedValue() < currentFenceValue) {
            fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
        }
        frameIndex = swapChain->GetCurrentBackBufferIndex();
        };

    // -------------------- 삼각형 버텍스 --------------------
    Vertex triangleVertices[] = {
        { {  0.0f,  0.25f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
    };

    ComPtr<ID3D12Resource> vertexBuffer;
    const UINT vertexBufferSize = sizeof(triangleVertices);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

    // 업로드 힙으로 버텍스 버퍼 생성
    device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer)
    );

    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
    vertexBuffer->Unmap(0, nullptr);

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};
    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.StrideInBytes = sizeof(Vertex);
    vertexBufferView.SizeInBytes = vertexBufferSize;

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
    ComPtr<ID3D12RootSignature> rootSignature;
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

    // -------------------- 파이프라인 상태 객체 --------------------
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
    psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pipelineState;
    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // -------------------- 메시지 루프 & 렌더링 --------------------
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // 커맨드 준비
        commandAllocator->Reset();
        commandList->Reset(commandAllocator.Get(), pipelineState.Get());

        CD3DX12_VIEWPORT viewport(0.0f, 0.0f, 800.0f, 600.0f);
        CD3DX12_RECT rect(0, 0, 800, 600);

        commandList->SetGraphicsRootSignature(rootSignature.Get());
        commandList->RSSetViewports(1, &viewport);
        commandList->RSSetScissorRects(1, &rect);

        auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
            rtvHeap->GetCPUDescriptorHandleForHeapStart(),
            frameIndex,
            rtvDescriptorSize
        );

        CD3DX12_RESOURCE_BARRIER transition1 = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[frameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        commandList->ResourceBarrier(1, &transition1);

        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        const float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
        commandList->DrawInstanced(3, 1, 0, 0);

        CD3DX12_RESOURCE_BARRIER transition2 = CD3DX12_RESOURCE_BARRIER::Transition(
            renderTargets[frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT
        );

        commandList->ResourceBarrier(1, &transition2);

        commandList->Close();

        // 실행
        ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
        commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        swapChain->Present(1, 0);
        WaitForPreviousFrame();
    }

    WaitForPreviousFrame();
    CloseHandle(fenceEvent);
    return 0;
}
