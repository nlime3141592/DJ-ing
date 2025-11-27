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
			32,
			0,
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
			32,
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
		32,
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
		_clrTempoSlider[0],
		_clrTempoSlider[1],
		_clrTempoSlider[2]
	);

	float fx1 = -0.001f + 1.002f * (1.0f - channel->fx1);
	float fx2 = -0.001f + 1.002f * (1.0f - channel->fx2);

	// FX1 Nobe
	DrawGauge2(
		fx1,
		0.0f, 1.0f,
		{ 980, 0, 1060, 360 },
		{ 0, 20, 5, 20 },
		32,
		0,
		_clrFXsMax[0],
		_clrFXsMin[0]
	);

	// FX2 Nobe
	DrawGauge2(
		fx2,
		0.0f, 1.0f,
		{ 900, 0, 980, 360 },
		{ 0, 20, 5, 20 },
		32,
		0,
		_clrFXsMax[1],
		_clrFXsMin[1]
	);

	djRectLTWH rcButton;
	
	// Pad Button Properties
	int32_t wButton = 140;
	int32_t hButton = 140;
	int32_t gButton = 20;
	
	int32_t wOffset = 260;
	int32_t hOffset = 30;

	rcButton.topLeftX = wOffset;
	rcButton.topLeftY = hOffset;
	rcButton.width = wButton;
	rcButton.height = hButton;

	// Pad1
	DrawPadButton(
		GetPadCondition(0),
		rcButton,
		_clrHotCues[0],
		_clrDisables[3]);
	// Pad2
	rcButton.topLeftX = wOffset + 1 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(1),
		rcButton,
		_clrHotCues[1],
		_clrDisables[3]);
	// Pad3
	rcButton.topLeftX = wOffset + 2 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(2),
		rcButton,
		_clrHotCues[2],
		_clrDisables[3]);
	// Pad4
	rcButton.topLeftX = wOffset + 3 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(3),
		rcButton,
		_clrHotCues[3],
		_clrDisables[3]);
	// Pad5
	rcButton.topLeftX = wOffset;
	rcButton.topLeftY = hOffset + 1 * (hButton + gButton);
	DrawPadButton(
		GetPadCondition(4),
		rcButton,
		_clrHotCues[4],
		_clrDisables[3]);
	// Pad6
	rcButton.topLeftX = wOffset + 1 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(5),
		rcButton,
		_clrHotCues[5],
		_clrDisables[3]);
	// Pad7
	rcButton.topLeftX = wOffset + 2 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(6),
		rcButton,
		_clrHotCues[6],
		_clrDisables[3]);
	// Pad8
	rcButton.topLeftX = wOffset + 3 * (wButton + gButton);
	DrawPadButton(
		GetPadCondition(7),
		rcButton,
		_clrHotCues[7],
		_clrDisables[3]);

	// PadFn Button Properties
	wButton = 50;
	hButton = 50;
	gButton = 10;

	wOffset = 200;
	hOffset = 30;

	rcButton.topLeftX = wOffset;
	rcButton.topLeftY = hOffset;
	rcButton.width = wButton;
	rcButton.height = hButton;

	// PadFn1
	DrawPadButton(
		GetPadFnCondition(0),
		rcButton,
		_clrPadFn,
		_clrDisables[3]);
	// PadFn2
	rcButton.topLeftY = hOffset + 1 * (hButton + gButton);
	DrawPadButton(
		GetPadFnCondition(1),
		rcButton,
		_clrPadFn,
		_clrDisables[3]);
	// PadFn3
	rcButton.topLeftY = hOffset + 2 * (hButton + gButton);
	DrawPadButton(
		GetPadFnCondition(2),
		rcButton,
		_clrPadFn,
		_clrDisables[3]);
	// PadFn4
	rcButton.topLeftY = hOffset + 3 * (hButton + gButton);
	DrawPadButton(
		GetPadFnCondition(3),
		rcButton,
		_clrPadFn,
		_clrDisables[3]);

	// Play Button
	DrawRegularTriangle({ 55.0f, 300.0f, 0.0f }, 45.0f, 0.0f, _clrPlayButton[1]);
	if (channel->GetSource()->IsPlaying())
		DrawRegularTriangle({ 55.0f, 290.0f, 0.0f }, 45.0f, 0.0f, _clrPlayButton[0]);
	else
		DrawRegularTriangle({ 55.0f, 290.0f, 0.0f }, 45.0f, 0.0f, _clrDisables[3]);

	// Global Cue Button
	DrawRegularTriangle({ 55.0f, 180.0f, 0.0f }, 45.0f, 0.0f, _clrGlobalCueButton[1]);
	if (channel->GetSource()->IsGlobalCueEnabled())
		DrawRegularTriangle({ 55.0f, 170.0f, 0.0f }, 45.0f, 0.0f, _clrGlobalCueButton[0]);
	else
		DrawRegularTriangle({ 55.0f, 170.0f, 0.0f }, 45.0f, 0.0f, _clrDisables[3]);
}

bool djControllerView::GetPadCondition(int index)
{
	switch (channel->padNumber)
	{
	case 0:
		break;
	case 1:
		return channel->GetSource()->GetHotCue(index) >= 0;
	case 2:
		return channel->fxNumber == index + 1;
	case 3:
		return channel->fxNumber == index + 1;
	case 4:
		return false;
	default:
		return false;
	}
}

bool djControllerView::GetPadFnCondition(int index)
{
	return channel->padNumber == index + 1;
}

void djControllerView::DrawPadButton(
	bool enable,
	djRectLTWH ltwh,
	djColor colorEnable,
	djColor colorDisable)
{
	if (enable)
		DrawRectangle(ltwh, colorEnable);
	else
		DrawRectangle(ltwh, colorDisable);
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
	djColor color;

	// Draw Guideline
	int32_t guidelineWidth = 3;
	int32_t guidelinePadding = 20;
	rect.left = ltrb.left + padding.left + w / 2 - guidelineWidth;
	rect.right = rect.left + guidelineWidth;
	rect.top = guidelinePadding;
	rect.bottom = this->viewport.height - guidelinePadding;
	DrawRectangle(rect, GetColorByRGB(255, 255, 255));

	// Draw Gauge
	rect.left = ltrb.left + padding.left;
	rect.top = ltrb.top + padding.top;

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
	djColor clr0;
	djColor clr1;
	djColor color;
	
	// Draw Guideline
	int32_t guidelineWidth = 3;
	int32_t guidelinePadding = 20;
	rect.left = ltrb.left + padding.left + w / 2 - guidelineWidth;
	rect.right = rect.left + guidelineWidth;
	rect.top = guidelinePadding;
	rect.bottom = this->viewport.height - guidelinePadding;
	DrawRectangle(rect, GetColorByRGB(255, 255, 255));

	// Draw Gauge
	rect.left = ltrb.left + padding.left;
	rect.top = ltrb.top + padding.top;

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