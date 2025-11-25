#include "djsw_gui_app_wave.h"

#include "djsw_audio_api.h"

void djWaveView::OnGuiInit()
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

void djWaveView::OnDrawWave()
{
	int w = this->viewport.width;
	int h = this->viewport.height;
	int xHalf = w / 2.0f;
	float yHalf = (float)h / 2.0f;

	int scale = 128;
	float amplitude = 0.7f;

	int16_t out[2] = { 0 };
	float sum[2] = { 0 };
	djColor color[2] = {
		{ 1.0f, 0.0f, 0.2f },
		{ 0.0f, 0.2f, 1.0f },
	};

	djRectLTRB ltrb;

	djAudioSource* audioSource = this->channel->GetSource();

	if (audioSource->IsLoaded())
	{
		for (int i = 0; i < w; ++i)
		{
			for (int32_t j = 0; j < audioSource->GetNumChannels(); ++j)
			{
				int16_t min;
				int16_t max;

				audioSource->Peek(scale, scale * (i - xHalf), j, &min, &max);

				ltrb.left = i;
				ltrb.right = ltrb.left;

				float yMin = yHalf - (min / 32767.0f) * yHalf * amplitude;
				float yMax = yHalf - (max / 32767.0f) * yHalf * amplitude;
				ltrb.top = yMin;
				ltrb.bottom = yMax;
				DrawLine(ltrb, color[j]);
			}
		}
	}

	int wGuideline = 4;

	for (int i = 0; i < wGuideline; ++i)
	{
		float grayscale = (float)(0) / wGuideline;

		if (i == 0)
			grayscale = 1.0f;

		ltrb.left = xHalf - i;
		ltrb.top = 0;
		ltrb.right = ltrb.left;
		ltrb.bottom = h;
		DrawLine(ltrb, { grayscale, grayscale, grayscale });

		ltrb.left = xHalf + i;
		ltrb.top = 0;
		ltrb.right = ltrb.left;
		ltrb.bottom = h;
		DrawLine(ltrb, { grayscale, grayscale, grayscale });
	}
}