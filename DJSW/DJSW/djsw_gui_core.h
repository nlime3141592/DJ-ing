#pragma once

#include <Windows.h>
#include <wrl.h>

#include "d3dx12.h"
#include "djsw_errorcode.h"

#define DJSW_MAX_BROWSER_COUNT D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE

using namespace Microsoft::WRL;

typedef struct
{
    LONG topLeftX;
    LONG topLeftY;
    LONG width;
    LONG height;
} djRectLTWH;

typedef struct
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} djRectLTRB;

typedef struct
{
    float r;
    float g;
    float b;
} djColor;

typedef struct
{
    djRectLTWH viewport;
    djRectLTRB scissor;
    djColor backgroundColor;
    djColor foregroundColor;
    BOOL shouldRender;
    void(*onGuiUpdateHandler)();
} djView;

void DrawRectangle(djRectLTWH rect, djColor color);
void DrawRectangle(djRectLTRB rect, djColor color);

void GetView(djView** view, int index);

void OnGuiInit_Core();
void OnGuiUpdate_Core(
    ComPtr<ID3D12GraphicsCommandList> cmdList,
    float resolutionX,
    float resolutionY);
void OnGuiFinal_Core();