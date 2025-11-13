#pragma once

#include <Windows.h>

#include "d3dx12.h"
#include "djsw_errorcode.h"

// -------------------- Constant Definitions --------------------
#define DJSW_MAX_VIEW_COUNT D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE

// -------------------- Struct Definitions --------------------
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
    float x;
    float y;
    float z;
} djVector3;

typedef struct
{
    float r;
    float g;
    float b;
} djColor;

typedef struct
{
    djVector3 position;
    djColor color;
} djVertexRGB;

// -------------------- Class Definitions --------------------
class djView
{
public:
    djRectLTWH viewport;
    djRectLTRB bounds;
    djColor backgroundColor;
    djColor boundColor;
    bool shouldRender;

public:
    virtual void OnGuiInit() = 0;
    void OnDrawBackground();
    virtual void OnGuiUpdate() = 0;
    void OnDrawBounds();
};

// -------------------- Global Method Definitions --------------------
djErrorCode AddVertices(void* vertices, size_t szVertex, size_t cntVertex);

void SetView(djView* view, int index);
void GetView(djView** view, int index);
djView* GetCurrentView();

// -------------------- Drawing API Definitions --------------------
void DrawRectangle(djRectLTWH rect, djColor color);
void DrawRectangle(djRectLTRB rect, djColor color);
