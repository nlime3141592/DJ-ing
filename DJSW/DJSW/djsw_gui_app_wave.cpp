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
	djColor globalCueColor = { 1.0f, 0.0f, 0.2f };
	djColor hotCueColor[8] = {
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
		{ 1.0f, 0.0f, 0.2f },
	};

	djRectLTRB ltrb;

	djAudioSource* audioSource = this->channel->GetSource();
	int16_t min;
	int16_t max;

	if (audioSource->IsLoaded())
	{
		// 1. 파형 그리기
		for (int i = 0; i < w; ++i)
		{
			for (int32_t j = 0; j < audioSource->GetNumChannels(); ++j)
			{
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

		int32_t beg = audioSource->Peek(scale, -scale * xHalf * audioSource->GetNumChannels(), 0, &min, &max);
		int32_t end = audioSource->Peek(scale, scale * xHalf * audioSource->GetNumChannels(), 0, &min, &max);

		// 2. 글로벌 큐 포인트 그리기
		DrawCuePoint(
			audioSource,
			scale,
			audioSource->GetGlobalCueIndex(),
			beg,
			end,
			globalCueColor,
			1);

		// 3. 핫 큐 포인트 그리기
		for (int i = 0; i < 8; ++i)
		{
			int32_t hotCueIndex = audioSource->GetHotCue(i);

			if (hotCueIndex < 0)
				continue;

			DrawCuePoint(
				audioSource,
				scale,
				hotCueIndex,
				beg,
				end,
				hotCueColor[i],
				1);
		}
	}

	// 4. 가이드라인 그리기
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

void djWaveView::DrawCuePoint(
	djAudioSource* source,
	int32_t scale,
	int32_t cueIndex,
	int32_t begIndex,
	int32_t endIndex,
	djColor color,
	int32_t lineWidth)
{
	djRectLTRB ltrb;
	int16_t min;
	int16_t max;

	float xHalf = this->viewport.width / 2.0f;
	int y = this->viewport.height;

	int32_t offset = cueIndex - source->GetPosition();
	int32_t index = source->Peek(scale, offset, 0, &min, &max);
	int32_t c = source->GetNumChannels();

	if (index < begIndex || index > endIndex)
		return;

	for (int32_t i = -lineWidth; i <= lineWidth; ++i)
	{
		ltrb.left = offset / (scale * c) + xHalf + i;
		ltrb.right = ltrb.left;
		ltrb.top = 0;
		ltrb.bottom = y;
		DrawLine(ltrb, color);
	}
}