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

	if (IsAudioLoaded(this->idxChannel))
	{
		int origin = GetPosition(this->idxChannel);
		int numChannel = 2;
		origin = origin - origin % numChannel;

		for (int i = 0; i < w; ++i)
		{
			int pl = origin + numChannel * scale * (i - xHalf);

			if (pl < 0)
				continue;

			sum[0] = 0;
			sum[1] = 0;

			float min0 = 32767.0f;
			float max0 = -32768.0f;
			float min1 = 32767.0f;
			float max1 = -32768.0f;

			for (int j = 0; j < scale; ++j)
			{
				PeekSample(out, this->idxChannel, pl + numChannel * j);

				if (out[0] < min0)
					min0 = out[0];
				else if (out[0] > max0)
					max0 = out[0];

				if (out[1] < min1)
					min1 = out[1];
				else if (out[1] > max1)
					max1 = out[1];
			}

			ltrb.left = i;
			ltrb.right = ltrb.left;

			float yMin = yHalf - (min0 / 32767.0f) * yHalf * amplitude;
			float yMax = yHalf - (max0 / 32767.0f) * yHalf * amplitude;
			ltrb.top = yMin;
			ltrb.bottom = yMax;
			DrawLine(ltrb, color[0]);

			yMin = yHalf - (min1 / 32767.0f) * yHalf * amplitude;
			yMax = yHalf - (max1 / 32767.0f) * yHalf * amplitude;
			ltrb.top = yMin;
			ltrb.bottom = yMax;
			DrawLine(ltrb, color[1]);
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