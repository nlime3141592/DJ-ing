#include "djsw_gui_app_wave1.h"

void OnGuiInit_Wave1(djView* view)
{
	view->viewport.topLeftX = 100;
	view->viewport.topLeftY = 100;
	view->viewport.width = 1920;
	view->viewport.height = 1080;
	view->scissor.left = 100;
	view->scissor.top = 100;
	view->scissor.right = 100;
	view->scissor.bottom = 100;
	view->backgroundColor.r = 1.0f;
	view->backgroundColor.g = 1.0f;
	view->backgroundColor.b = 1.0f;
	view->foregroundColor.r = 0.5f;
	view->foregroundColor.g = 0.5f;
	view->foregroundColor.b = 0.5f;
	view->shouldRender = true;
	view->onGuiUpdateHandler = OnGuiUpdate_Wave1;
}

void OnGuiUpdate_Wave1()
{
	djRectLTRB ltrb;
	ltrb.left = 10;
	ltrb.top = 10;
	ltrb.right = 1080;
	ltrb.bottom = 720;

	djColor color;
	color.r = 0.75f;
	color.g = 0.75f;
	color.b = 0.75f;

	DrawRectangle(ltrb, color);
}