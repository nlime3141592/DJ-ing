#include "djsw_gui_app_wave1.h"

void djWaveView::OnGuiInit()
{
	this->viewport.topLeftX = 100;
	this->viewport.topLeftY = 100;
	this->viewport.width = 1920;
	this->viewport.height = 1080;
	this->bounds.left = 10;
	this->bounds.top = 10;
	this->bounds.right = 10;
	this->bounds.bottom = 10;
	this->backgroundColor.r = 1.0f;
	this->backgroundColor.g = 1.0f;
	this->backgroundColor.b = 1.0f;
	this->boundColor.r = 0.5f;
	this->boundColor.g = 0.5f;
	this->boundColor.b = 0.5f;
	this->shouldRender = true;
}

void djWaveView::OnGuiUpdate()
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