#include "djsw_gui_core_api_internal.h"

#include <assert.h>

static djView* _views[DJSW_MAX_VIEW_COUNT];
static int _idxCurrentView;

static ComPtr<ID3D12GraphicsCommandList> _cmdList;
static djView* _currentView;

void SetView(djView* view, int index)
{
	assert(index >= 0 && index < DJSW_MAX_VIEW_COUNT);

	_views[index] = view;
}

void GetView(djView** view, int index)
{
	assert(index >= 0 && index < DJSW_MAX_VIEW_COUNT);

	*view = _views[index];
}

djView* GetCurrentView()
{
	return _views[_idxCurrentView];
}

int iii = 0;

static bool OnEnterViewport(ComPtr<ID3D12GraphicsCommandList> cmdList, djView* view)
{
	if (view == NULL || !view->shouldRender)
		return false;

	_currentView = view;

	CD3DX12_VIEWPORT viewport(
		view->viewport.topLeftX,
		view->viewport.topLeftY,
		view->viewport.width,
		view->viewport.height);

	CD3DX12_RECT scissorRect(
		view->viewport.topLeftX,
		view->viewport.topLeftY,
		view->viewport.topLeftX + view->viewport.width,
		view->viewport.topLeftY + view->viewport.height);

	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissorRect);

	return true;
}

void OnGuiInit_Core()
{
	memset(_views, 0x00, sizeof(_views));
}

void OnGuiUpdate_Core(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	for (int i = 0; i < DJSW_MAX_VIEW_COUNT; ++i)
	{
		if (!OnEnterViewport(cmdList, _views[i]))
			continue;

		// Drawing Pipelines
		_views[i]->OnDrawBackground();
		_views[i]->OnGuiUpdate();
		_views[i]->OnDrawBounds();

		// Test Draws
		djVertexRGB vertices[6] = {
			{ { -1.1f,  0.5f, 0.0f }, { 1, 0, 0 } },
			{ {  1.0f,  0.5f, 0.0f }, { 0, 1, 0 } },
			{ {  0.0f, -1.0f, 0.0f }, { 0, 0, 1 } },
			{ {  1.1f,  0.5f, 0.0f }, { 0, 1, 0 } },
			{ {  0.5f, -0.5f, 0.0f }, { 1, 1, 0 } },
			{ {  0.0f, -1.0f, 0.0f }, { 0, 0, 1 } }
		};

		iii = (iii + 1) % 256;
		vertices[0].position.y = (iii / 255.0f) * 2.2f - 1.1f;

		//AddVertices(vertices, sizeof(djVertexRGB), 6);
	}
}

void OnGuiFinal_Core()
{
	// Do anything.
}