#include "djsw_gui_core.h"

#include <assert.h>

#include "LoopRender.h"

static djView _views[DJSW_MAX_BROWSER_COUNT];

static ComPtr<ID3D12GraphicsCommandList> _cmdList;
static djView* _currentView;
static float _resolutionX;
static float _resolutionY;

void DrawRectangle(djRectLTWH rect, djColor color)
{
	djRectLTRB ltrb;
	ltrb.left = rect.topLeftX;
	ltrb.top = rect.topLeftY;
	ltrb.right = rect.topLeftX + rect.width;
	ltrb.bottom = rect.topLeftY + rect.height;

	DrawRectangle(ltrb, color);
}

void DrawRectangle(djRectLTRB rect, djColor color)
{
	rect.left += _currentView->viewport.topLeftX;
	rect.top += _currentView->viewport.topLeftY;
	rect.right += _currentView->viewport.topLeftX;
	rect.bottom += _currentView->viewport.topLeftY;

	float l = 2.0f * rect.left / _resolutionX - 1.0f;
	float t = 1.0f - 2.0f * rect.top / _resolutionY;
	float r = 2.0f * rect.right / _resolutionX - 1.0f;
	float b = 1.0f - 2.0f * rect.bottom / _resolutionY;

	djVertexRGB vertices[6] =
	{
		// 삼각형 1
		{ { l, t, 0.0f }, { color.r, color.g, color.b } },  // 왼쪽 위
		{ { r, t, 0.0f }, { color.r, color.g, color.b } },  // 오른쪽 위
		{ { l, b, 0.0f }, { color.r, color.g, color.b } },  // 왼쪽 아래

		// 삼각형 2
		{ { r, t, 0.0f }, { color.r, color.g, color.b } },  // 오른쪽 위
		{ { r, b, 0.0f }, { color.r, color.g, color.b } },  // 오른쪽 아래
		{ { l, b, 0.0f }, { color.r, color.g, color.b } },  // 왼쪽 아래
	};

	AddVertices(vertices, 6);
}

void GetView(djView** view, int index)
{
	*view = _views + index;
}

void OnGuiInit_Core()
{
	memset(_views, 0x00, sizeof(_views));
}

void OnGuiUpdate_Core(
	ComPtr<ID3D12GraphicsCommandList> cmdList,
	float resolutionX,
	float resolutionY)
{
	_resolutionX = resolutionX;
	_resolutionY = resolutionY;

	djRectLTRB djViewport = { 0 };
	djRectLTRB djScissorRect = { 0 };
	
	for (int i = 0; i < DJSW_MAX_BROWSER_COUNT; ++i)
	{
		if (!_views[i].shouldRender)
			continue;

		_currentView = _views + i;

		CD3DX12_VIEWPORT viewport(
			_views[i].viewport.topLeftX,
			_views[i].viewport.topLeftY,
			_views[i].viewport.width,
			_views[i].viewport.height);
		CD3DX12_RECT scissorRect(
			_views[i].viewport.topLeftX,
			_views[i].viewport.topLeftY,
			_views[i].viewport.topLeftX + _views[i].viewport.width,
			_views[i].viewport.topLeftY + _views[i].viewport.height);

		cmdList->RSSetViewports(1, &viewport);
		cmdList->RSSetScissorRects(1, &scissorRect);

		assert(_views[i].onGuiUpdateHandler != NULL);

		_resolutionX = _views[i].viewport.width;
		_resolutionY = _views[i].viewport.height;

		djViewport.right = _views[i].viewport.width;
		djViewport.bottom = _views[i].viewport.height;
		djScissorRect.left = _views[i].scissor.left;
		djScissorRect.top = _views[i].scissor.top;
		djScissorRect.right = djViewport.right - _views[i].scissor.right - _views[i].viewport.topLeftX * 2;
		djScissorRect.bottom = djViewport.bottom - _views[i].scissor.bottom - _views[i].viewport.topLeftY * 2;

		DrawRectangle(djViewport, _views[i].backgroundColor);
		DrawRectangle(djScissorRect, _views[i].foregroundColor);

		_views[i].onGuiUpdateHandler();
	}
}

void OnGuiFinal_Core()
{
	// Do anything.
}