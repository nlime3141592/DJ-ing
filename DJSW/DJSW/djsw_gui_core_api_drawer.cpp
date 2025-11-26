#include "djsw_gui_core_api.h"

#include <assert.h>

djColor GetColorByRGB(uint8_t r, uint8_t g, uint8_t b)
{
	djColor color;

	color.r = (float)r / 255.0f;
	color.g = (float)g / 255.0f;
	color.b = (float)b / 255.0f;

	return color;
}

void DrawLine(djRectLTWH rect, djColor color)
{
	djRectLTRB ltrb;
	ltrb.left = rect.topLeftX;
	ltrb.top = rect.topLeftY;
	ltrb.right = rect.topLeftX + rect.width;
	ltrb.bottom = rect.topLeftY + rect.height;

	DrawLine(ltrb, color);
}

void DrawLine(djRectLTRB rect, djColor color)
{
	djView* view = GetCurrentView();

	assert(view != NULL);

	float rx = view->viewport.width;
	float ry = view->viewport.height;

	float l = 2.0f * rect.left / rx - 1.0f;
	float t = 1.0f - 2.0f * rect.top / ry;
	float r = 2.0f * rect.right / rx - 1.0f;
	float b = 1.0f - 2.0f * rect.bottom / ry;

	djVertexRGB vertices[6] =
	{
		// 직선 1
		{ { l, t, 0.0f }, { color.r, color.g, color.b } },  // LT
		{ { r, b, 0.0f }, { color.r, color.g, color.b } },  // RB
	};

	AddVertices(vertices, sizeof(djVertexRGB), 2);
}

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
	djView* view = GetCurrentView();

	assert(view != NULL);

	float rx = view->viewport.width;
	float ry = view->viewport.height;

	float l = 2.0f * rect.left / rx - 1.0f;
	float t = 1.0f - 2.0f * rect.top / ry;
	float r = 2.0f * rect.right / rx - 1.0f;
	float b = 1.0f - 2.0f * rect.bottom / ry;

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

	AddVertices(vertices, sizeof(djVertexRGB), 6);
}
