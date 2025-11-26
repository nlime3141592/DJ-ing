#pragma once
#include "LoopRender.h"

#include <d2d1.h>
#include <d2d1_3.h>
#include <d3d11.h>
#include <d3d11on12.h>
#include <dwrite.h>
#include <dxgi1_4.h>
#include <wrl.h>

#include "djsw_gui_core_api.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")

#define DJSW_VERTEX_THROUGHPUT 0x10000 // 16-bit address space
#define DJSW_VERTEX_CAPACITY 0x100000 // 20-bit address space

using namespace Microsoft::WRL;

// -------------------- 팩토리 생성 --------------------
extern ComPtr<IDXGIFactory6> _factory;

// -------------------- 디바이스 생성 --------------------
extern ComPtr<ID3D12Device> _d3d12Device;

// -------------------- 명령큐 생성 --------------------
extern ComPtr<ID3D12CommandQueue> _cmdQueue;

// -------------------- 11on12 디바이스 생성 --------------------
extern ID3D11DeviceContext* _d3d11DeviceContext;
extern ComPtr<ID3D11On12Device> _d3d11On12Device;

// -------------------- D2D, DWrite 생성 --------------------
extern ComPtr<ID2D1Factory3> _d2dFactory;
extern ComPtr<ID2D1Device3> _d2dDevice;
extern ComPtr<ID2D1DeviceContext2> _d2dDeviceContext;
extern ComPtr<IDWriteFactory> _dWriteFactory;

// -------------------- 스왑 체인 생성 --------------------
extern DXGI_SWAP_CHAIN_DESC1 _swapChainDesc;
extern ComPtr<IDXGISwapChain3> _swapChain0;
extern ComPtr<IDXGISwapChain1> _swapChain1;
extern UINT _frameIndex;
extern HWND _hwnd;

// -------------------- 렌더 타겟 뷰 서술자 힙 생성 --------------------
extern D3D12_DESCRIPTOR_HEAP_DESC _rtvHeapDesc;
extern ComPtr<ID3D12DescriptorHeap> _rtvHeap;
extern UINT _rtvDescriptorSize;

// -------------------- 렌더 타겟 뷰 생성 --------------------
#define DJSW_RENDER_TARGET_COUNT 2

extern CD3DX12_CPU_DESCRIPTOR_HANDLE _rtvHandle;
extern ComPtr<ID3D12Resource> _renderTargets[DJSW_RENDER_TARGET_COUNT];
extern ComPtr<ID3D11Resource> _wrappedBackBuffers[DJSW_RENDER_TARGET_COUNT];
extern ComPtr<ID2D1Bitmap1> _d2dRenderTargets[DJSW_RENDER_TARGET_COUNT];

// -------------------- 커맨드 할당자 / 리스트 생성 --------------------
extern ComPtr<ID3D12CommandAllocator> _cmdAllocator;
extern ComPtr<ID3D12GraphicsCommandList> _cmdList;

// -------------------- 루트 시그니처 생성 --------------------
extern ComPtr<ID3D12RootSignature> _rootSignature;

// -------------------- 펜스 정의 --------------------
extern UINT16 _fenceValue;
extern ComPtr<ID3D12Fence> _fence;
extern HANDLE _fenceEvent;

// -------------------- 정점 버퍼 정의 --------------------
extern ComPtr<ID3D12Resource> _vertexBuffer;
extern UINT8* _pVertexDataBegin;
extern D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

// -------------------- 셰이더 생성 --------------------
extern ComPtr<ID3DBlob> _vertexShader;
extern ComPtr<ID3DBlob> _pixelShader;
extern size_t _vertexCount;
extern size_t _vertexDrawCount;

// -------------------- 파이프라인 상태 객체 정의 --------------------
extern D3D12_GRAPHICS_PIPELINE_STATE_DESC _psoDesc_Triangle;
extern D3D12_GRAPHICS_PIPELINE_STATE_DESC _psoDesc_Line;
extern ComPtr<ID3D12PipelineState> _psoState_Triangle;
extern ComPtr<ID3D12PipelineState> _psoState_Line;

extern D3D12_INPUT_ELEMENT_DESC _inputElementDescs[];