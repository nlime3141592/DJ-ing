#include "LoopRender_Init.h"

// -------------------- 팩토리 생성 --------------------
ComPtr<IDXGIFactory6> _factory;

static void CreateFactory()
{
	CreateDXGIFactory1(IID_PPV_ARGS(&_factory));
}

// -------------------- 디바이스 생성 --------------------
ComPtr<ID3D12Device> _d3d12Device;

static void CreateDevice()
{
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_d3d12Device));
}

// -------------------- 명령큐 생성 --------------------
ComPtr<ID3D12CommandQueue> _cmdQueue;

static void CreateCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};

	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmdQueue));
}

// -------------------- 11on12 디바이스 생성 --------------------
ID3D11DeviceContext* _d3d11DeviceContext;
ComPtr<ID3D11On12Device> _d3d11On12Device;

static void CreateD3D11On12Device()
{
    // Create an 11 device wrapped around the 12 device and share 12's command queue.
    ComPtr<ID3D11Device> d3d11Device;
    D3D11On12CreateDevice(
        _d3d12Device.Get(),
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(_cmdQueue.GetAddressOf()),
        1,
        0,
        &d3d11Device,
        &_d3d11DeviceContext,
        nullptr
    );

    // Query the 11On12 device from the 11 device.
    d3d11Device.As(&_d3d11On12Device);
}

// -------------------- D2D, DWrite 생성 --------------------
ComPtr<ID2D1Factory3> _d2dFactory;
ComPtr<ID2D1Device3> _d2dDevice;
ComPtr<ID2D1DeviceContext2> _d2dDeviceContext;
ComPtr<IDWriteFactory> _dWriteFactory;

static void CreateD2DDevice()
{
    // Create D2D/DWrite components.
    D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};
    D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &d2dFactoryOptions, &_d2dFactory);

    ComPtr<IDXGIDevice> dxgiDevice;
    _d3d11On12Device.As(&dxgiDevice);
    _d2dFactory->CreateDevice(dxgiDevice.Get(), reinterpret_cast<ID2D1Device**>(_d2dDevice.GetAddressOf()));
    _d2dDevice->CreateDeviceContext(deviceOptions, reinterpret_cast<ID2D1DeviceContext**>(_d2dDeviceContext.GetAddressOf()));
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &_dWriteFactory);
}

// -------------------- 스왑 체인 생성 --------------------
DXGI_SWAP_CHAIN_DESC1 _swapChainDesc;
ComPtr<IDXGISwapChain3> _swapChain0;
ComPtr<IDXGISwapChain1> _swapChain1;
UINT _frameIndex;
HWND _hwnd;

static void CreateSwapChain()
{
    _swapChainDesc.BufferCount = FrameCount;
    //swapChainDesc.Width = 800;
    _swapChainDesc.Width = 3840;
    //swapChainDesc.Height = 600;
    //_swapChainDesc.Height = 2160;
    _swapChainDesc.Height = 720;
    //_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    _swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    _swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    _swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    _swapChainDesc.SampleDesc.Count = 1;

    _factory->CreateSwapChainForHwnd(
        _cmdQueue.Get(),
        _hwnd,
        &_swapChainDesc,
        nullptr,
        nullptr,
        &_swapChain1
    );

    _swapChain1.As(&_swapChain0);
    _frameIndex = _swapChain0->GetCurrentBackBufferIndex();
}

// -------------------- 렌더 타겟 뷰 서술자 힙 생성 --------------------
D3D12_DESCRIPTOR_HEAP_DESC _rtvHeapDesc;
ComPtr<ID3D12DescriptorHeap> _rtvHeap;
UINT _rtvDescriptorSize;

static void CreateRtvDescriptorHeap()
{
    _rtvHeapDesc = {};
    _rtvHeapDesc.NumDescriptors = FrameCount;
    _rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    _d3d12Device->CreateDescriptorHeap(&_rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap));

    _rtvDescriptorSize = _d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

// -------------------- 렌더 타겟 뷰 생성 --------------------
#define DJSW_RENDER_TARGET_COUNT 2

CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle;
ComPtr<ID3D12Resource> _renderTargets[DJSW_RENDER_TARGET_COUNT];
ComPtr<ID3D11Resource> _wrappedBackBuffers[DJSW_RENDER_TARGET_COUNT];
ComPtr<ID2D1Bitmap1> _d2dRenderTargets[DJSW_RENDER_TARGET_COUNT];

static void CreateRenderTargetView()
{
    float dpiX = 96.0f;
    float dpiY = 96.0f;

    // Deprecated.
    //_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
        dpiX,
        dpiY
    );

    _rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT n = 0; n < FrameCount; ++n)
    {
        _swapChain0->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n]));
        _d3d12Device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, _rtvHandle);

        // Create a wrapped 11On12 resource of this back buffer. Since we are 
        // rendering all D3D12 content first and then all D2D content, we specify 
        // the In resource state as RENDER_TARGET - because D3D12 will have last 
        // used it in this state - and the Out resource state as PRESENT. When 
        // ReleaseWrappedResources() is called on the 11On12 device, the resource 
        // will be transitioned to the PRESENT state.
        D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
        _d3d11On12Device->CreateWrappedResource(
            _renderTargets[n].Get(),
            &d3d11Flags,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT,
            IID_PPV_ARGS(&_wrappedBackBuffers[n])
        );

        // Create a render target for D2D to draw directly to this back buffer.
        ComPtr<IDXGISurface> surface;
        _wrappedBackBuffers[n].As(&surface);
        _d2dDeviceContext->CreateBitmapFromDxgiSurface(
            surface.Get(),
            &bitmapProperties,
            &_d2dRenderTargets[n]
        );

        _rtvHandle.Offset(1, _rtvDescriptorSize);
    }
}

// -------------------- 커맨드 할당자 / 리스트 생성 --------------------
ComPtr<ID3D12CommandAllocator> _cmdAllocator;
ComPtr<ID3D12GraphicsCommandList> _cmdList;

static void CreateCommandList()
{
    _d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
    _d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&_cmdList));
    _cmdList->Close();
}

// -------------------- 루트 시그니처 생성 --------------------
ComPtr<ID3D12RootSignature> _rootSignature;

static void CreateRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    _d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));
}

// -------------------- 펜스 정의 --------------------
UINT16 _fenceValue;
ComPtr<ID3D12Fence> _fence;
HANDLE _fenceEvent;

static void CreateFence()
{
    _fenceValue = 0;
    _d3d12Device->CreateFence(_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// -------------------- 정점 버퍼 정의 --------------------
ComPtr<ID3D12Resource> _vertexBuffer;
D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
UINT8* _pVertexDataBegin;

static void CreateVertexBuffer()
{
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

    // 정점 버퍼의 크기 정의
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(djVertexRGB) * DJSW_VERTEX_CAPACITY);

    // 업로드 힙으로 버텍스 버퍼 생성
    _d3d12Device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_vertexBuffer)
    );

    CD3DX12_RANGE readRange(0, 0);
    _vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&_pVertexDataBegin));
    
    _vertexBufferView = {};
    _vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
    _vertexBufferView.StrideInBytes = sizeof(djVertexRGB);
    _vertexBufferView.SizeInBytes = sizeof(djVertexRGB) * DJSW_VERTEX_CAPACITY;
}

// -------------------- 셰이더 생성 --------------------
ComPtr<ID3DBlob> _vertexShader;
ComPtr<ID3DBlob> _pixelShader;
size_t _vertexCount;
size_t _vertexDrawCount;

static void CreateShader()
{
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
        "VSMain", "vs_5_0", 0, 0, &_vertexShader, nullptr
    );

    D3DCompile(
        "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
        "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }",
        strlen(
            "struct PS_INPUT { float4 pos: SV_POSITION; float3 col: COLOR; };"
            "float4 PSMain(PS_INPUT input) : SV_TARGET { return float4(input.col,1); }"
        ),
        nullptr, nullptr, nullptr,
        "PSMain", "ps_5_0", 0, 0, &_pixelShader, nullptr
    );

    _vertexCount = 0;
    _vertexDrawCount = 0;
}

// -------------------- 파이프라인 상태 객체 정의 --------------------
D3D12_GRAPHICS_PIPELINE_STATE_DESC _psoDesc_Triangle;
D3D12_GRAPHICS_PIPELINE_STATE_DESC _psoDesc_Line;
ComPtr<ID3D12PipelineState> _psoState_Triangle;
ComPtr<ID3D12PipelineState> _psoState_Line;

D3D12_INPUT_ELEMENT_DESC _inputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};

static void CreatePSO_Triangle()
{
    _psoDesc_Triangle = {};
    _psoDesc_Triangle.InputLayout = { _inputElementDescs, _countof(_inputElementDescs) };
    _psoDesc_Triangle.pRootSignature = _rootSignature.Get();
    _psoDesc_Triangle.VS = { reinterpret_cast<BYTE*>(_vertexShader->GetBufferPointer()), _vertexShader->GetBufferSize() };
    _psoDesc_Triangle.PS = { reinterpret_cast<BYTE*>(_pixelShader->GetBufferPointer()), _pixelShader->GetBufferSize() };
    _psoDesc_Triangle.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    _psoDesc_Triangle.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    _psoDesc_Triangle.DepthStencilState.DepthEnable = FALSE;
    _psoDesc_Triangle.DepthStencilState.StencilEnable = FALSE;
    _psoDesc_Triangle.SampleMask = UINT_MAX;
    _psoDesc_Triangle.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // 이 곳에서 파이프라인 상태를 변경할 수 있음.
    _psoDesc_Triangle.NumRenderTargets = 1;
    _psoDesc_Triangle.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    _psoDesc_Triangle.SampleDesc.Count = 1;
}

static void CreatePSO_Line()
{
    _psoDesc_Line = {};
    _psoDesc_Line.InputLayout = { _inputElementDescs, _countof(_inputElementDescs) };
    _psoDesc_Line.pRootSignature = _rootSignature.Get();
    _psoDesc_Line.VS = { reinterpret_cast<BYTE*>(_vertexShader->GetBufferPointer()), _vertexShader->GetBufferSize() };
    _psoDesc_Line.PS = { reinterpret_cast<BYTE*>(_pixelShader->GetBufferPointer()), _pixelShader->GetBufferSize() };
    _psoDesc_Line.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    _psoDesc_Line.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    _psoDesc_Line.DepthStencilState.DepthEnable = FALSE;
    _psoDesc_Line.DepthStencilState.StencilEnable = FALSE;
    _psoDesc_Line.SampleMask = UINT_MAX;
    _psoDesc_Line.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    _psoDesc_Line.NumRenderTargets = 1;
    _psoDesc_Line.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    _psoDesc_Line.SampleDesc.Count = 1;
}

static void CreatePipelineState()
{
    CreatePSO_Triangle();
    CreatePSO_Line();

    _d3d12Device->CreateGraphicsPipelineState(&_psoDesc_Triangle, IID_PPV_ARGS(&_psoState_Triangle));
    _d3d12Device->CreateGraphicsPipelineState(&_psoDesc_Line, IID_PPV_ARGS(&_psoState_Line));
}

// -------------------- 파이프라인 초기화 --------------------
void PipelineInit()
{
    CreateFactory();
    CreateDevice();
    CreateCommandQueue();
    CreateD3D11On12Device();
    CreateD2DDevice();
    CreateSwapChain();
    CreateRtvDescriptorHeap();
    CreateRenderTargetView();
    CreateCommandList();
    CreateFence();
    CreateVertexBuffer();
    CreateShader();
    CreateRootSignature();
    CreatePipelineState();
}