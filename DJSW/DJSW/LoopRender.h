#pragma once

#include <assert.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <vector>
#include <wrl.h>
#include <Windows.h>
#include <windowsx.h>
#include <string>
#include "d3dx12.h"

#include "djsw_errorcode.h"

#include "LoopBase.h"
#include "LoopHID.h"
#include "LoopRender_Init.h"

using namespace Microsoft::WRL;

const UINT FrameCount = 2;

typedef struct
{
	LoopBaseParams loopBaseParams;
} RenderParams;

void PipelineInit();

int WINAPI RenderInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI RenderUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);
int WINAPI RenderFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow);