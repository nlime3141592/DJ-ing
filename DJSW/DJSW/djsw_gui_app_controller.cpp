#include "djsw_gui_app_controller.h"

#include "djsw_audio_api.h"

void djControllerView::OnGuiInit()
{
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

void djControllerView::OnGuiUpdate()
{
	djRectLTRB ltrb;
	ltrb.left = 0;
	ltrb.top = 0;
	ltrb.right = this->viewport.width;
	ltrb.bottom = this->viewport.height;

	DrawRectangle(ltrb, GetColorByRGB(18, 18, 18));

	float mVolume = -0.001f + 1.002f * (1.0f - channel->masterVolume);
	float xVolume = -0.001f + 1.002f * (1.0f - channel->crossVolume);

	// Master Volume
	if (channel->mute == 0.0f)
	{
		DrawGauge2(
			mVolume,
			0.0f, 1.0f,
			{ 1200, 0, 1280, 360 },
			{ 0, 20, 5, 20 },
			20,
			5,
			GetColorByRGB(252, 152, 154),
			GetColorByRGB(39, 42, 61)
		);
	}
	else
	{
		DrawGauge2(
			mVolume,
			0.0f, 1.0f,
			{ 1200, 0, 1280, 360 },
			{ 0, 20, 5, 20 },
			18,
			0,
			GetColorByRGB(129, 201, 245),
			GetColorByRGB(39, 42, 61)
		);
	}

	// Crossfader Volume
	DrawGauge2(
		xVolume,
		0.0f, 1.0f,
		{ 1120, 0, 1200, 360 },
		{ 0, 20, 5, 20 },
		18,
		0,
		GetColorByRGB(200, 191, 231),
		GetColorByRGB(39, 42, 61)
	);

	// Tempo Slider
	float tempo = (float)channel->GetSource()->GetHopDistance() / channel->GetSource()->GetNumChannels();
	tempo += DJSW_WSOLA_TEMPO_RANGE - 1;

	DrawGauge3(
		tempo,
		0.0f, 2 * DJSW_WSOLA_TEMPO_RANGE - 1,
		{ 1060, 0, 1120, 360 },
		{ 0, 20, 5, 20 },
		2 * DJSW_WSOLA_TEMPO_RANGE - 1,
		0,
		GetColorByRGB(128, 255, 128),
		GetColorByRGB(225, 255, 225),
		GetColorByRGB(128, 255, 128)
	);
}

void djControllerView::DrawGauge2(
	float value,
	float min,
	float max,
	djRectLTRB ltrb,
	djRectLTRB padding,
	int32_t count,
	int32_t gap,
	djColor c0,
	djColor c1)
{
	float w = (float)(ltrb.right - ltrb.left) - (float)(padding.left + padding.right);
	float h = (float)((ltrb.bottom - ltrb.top) - gap * (count - 1) - (padding.top + padding.bottom)) / (count);
	float g = (float)gap;

	djRectLTRB rect;
	rect.left = ltrb.left + padding.left;
	rect.top = ltrb.top + padding.top;

	djColor color;

	for (int32_t i = 0; i < count; ++i)
	{
		float v = (float)i / (float)(count - 1);
		
		if (value <= (max - min) * v)
		{
			rect.right = rect.left + w;
			rect.bottom = rect.top + h;

			color.r = c0.r + (c1.r - c0.r) * v;
			color.g = c0.g + (c1.g - c0.g) * v;
			color.b = c0.b + (c1.b - c0.b) * v;

			DrawRectangle(rect, color);
		}

		rect.top += (h + g);
	}
}

void djControllerView::DrawGauge3(
	float value,
	float min,
	float max,
	djRectLTRB ltrb,
	djRectLTRB padding,
	int32_t count,
	int32_t gap,
	djColor c0,
	djColor c1,
	djColor c2)
{
	float w = (float)(ltrb.right - ltrb.left) - (float)(padding.left + padding.right);
	float h = (float)((ltrb.bottom - ltrb.top) - gap * (count - 1) - (padding.top + padding.bottom)) / count;
	float g = (float)gap;

	djRectLTRB rect;
	rect.left = ltrb.left + padding.left;
	rect.top = ltrb.top + padding.top;

	djColor clr0;
	djColor clr1;
	djColor color;

	for (int32_t i = 0; i < count; ++i)
	{
		float v = (float)i / (float)(count - 1);

		if (value <= (max - min) * v)
		{
			if (v <= 0.5f)
			{
				clr0 = c0;
				clr1 = c1;
				v *= 2.0f;
			}
			else
			{
				clr0 = c1;
				clr1 = c2;
				v -= 0.5f;
				v *= 2.0f;
			}

			rect.right = rect.left + w;
			rect.bottom = rect.top + h;

			color.r = clr0.r + (clr1.r - clr0.r) * v;
			color.g = clr0.g + (clr1.g - clr0.g) * v;
			color.b = clr0.b + (clr1.b - clr0.b) * v;

			DrawRectangle(rect, color);
		}
		rect.top += (h + g);
	}
}