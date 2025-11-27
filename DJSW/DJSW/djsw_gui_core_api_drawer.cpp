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

djColor GetColorAsGrayscale(float grayscale01)
{
	djColor color;

	color.r = grayscale01;
	color.g = grayscale01;
	color.b = grayscale01;

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

void DrawTriangle(djRectLTWH rect, float eulerAngleZ, djColor color)
{

}

void DrawTriangle(djRectLTRB rect, float eulerAngleZ, djColor color)
{

}

void DrawRegularTriangle(djVector3 center, float radius, float eulerAngleZ, djColor color)
{
	djView* view = GetCurrentView();

	float pi = 3.1415926535897932f;
	float deg2rad = pi / 180.0f;
	float radAngleZ = eulerAngleZ * deg2rad;
	float radAngle120 = 120.0f * deg2rad;

	// eulerAngleZ에 의한 회전 행렬
	float cZ = cosf(radAngleZ);
	float sZ = sinf(radAngleZ);

	// 120도 회전 행렬
	float c120 = cosf(radAngle120);
	float s120 = sinf(radAngle120);

	float x = radius;
	float y = 0.0f;

	float tx = 0.0f;
	float ty = 0.0f;

	tx = x * cZ - y * sZ;
	ty = x * sZ + y * cZ;
	x = tx;
	y = ty;

	djVertexRGB vertices[3];

	for (int i = 0; i < 3; ++i)
	{
		float tx = x;
		float ty = y;

		tx = x * c120 - y * s120;
		ty = x * s120 + y * c120;
		x = tx;
		y = ty;

		vertices[i].position.x = (2.0f * (center.x + x) / view->viewport.width - 1.0f);
		vertices[i].position.y = (1.0f - 2.0f * (center.y + y) / view->viewport.height);
		vertices[i].position.z = 0.0f;
		vertices[i].color = color;
	}

	AddVertices(vertices, sizeof(djVertexRGB), 3);
}