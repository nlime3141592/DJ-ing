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
	ltrb.left = 0;
	ltrb.top = 0;
	ltrb.right = this->viewport.width;
	ltrb.bottom = this->viewport.height;

	DrawRectangle(ltrb, GetColorByRGB(18, 18, 18));
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
		GetColorByRGB(255, 64, 0),
		GetColorByRGB(0, 64, 255),
	};
	djColor globalCueColor = GetColorByRGB(255, 255, 255);
	djColor hotCueColor[8] = {
		GetColorByRGB(255, 55, 111),
		GetColorByRGB(69, 172, 219),
		GetColorByRGB(125, 193, 61),
		GetColorByRGB(170, 114, 255),
		GetColorByRGB(48, 210, 110),
		GetColorByRGB(224, 100, 27),
		GetColorByRGB(48, 90, 255),
		GetColorByRGB(195, 175, 4),
	};
	djColor loopColor = GetColorByRGB(255, 201, 14);

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

		// 2. 루프 영역 그리기
		if (audioSource->IsLoop())
		{
			int32_t beg = audioSource->GetLoopIndex();
			int32_t end = beg + audioSource->GetLoopLength();

			DrawLoop(
				audioSource,
				scale,
				beg,
				end,
				loopColor,
				(int32_t)((float)h * 0.1f));
		}

		int32_t beg = audioSource->Peek(scale, -scale * xHalf * audioSource->GetNumChannels(), 0, &min, &max);
		int32_t end = audioSource->Peek(scale, scale * xHalf * audioSource->GetNumChannels(), 0, &min, &max);

		// 3. 글로벌 큐 포인트 그리기
		DrawCuePoint(
			audioSource,
			scale,
			audioSource->GetGlobalCueIndex(),
			beg,
			end,
			globalCueColor,
			1);
		DrawUpArrow(
			audioSource,
			scale,
			audioSource->GetGlobalCueIndex(),
			globalCueColor,
			(int32_t)((float)h * 0.1f));

		// 4. 핫 큐 포인트 그리기
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
			DrawDownArrow(
				audioSource,
				scale,
				hotCueIndex,
				hotCueColor[i],
				(int32_t)((float)h * 0.1f));
		}
	}

	// 5. 가이드라인 그리기
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

void djWaveView::DrawDownArrow(
	djAudioSource* source,
	int32_t scale,
	int32_t cueIndex,
	djColor color,
	int32_t triangleWidth)
{
	djRectLTRB ltrb;
	int16_t min;
	int16_t max;

	float xHalf = this->viewport.width / 2.0f;

	int32_t offset = cueIndex - source->GetPosition();
	int32_t index = source->Peek(scale, offset, 0, &min, &max);
	int32_t c = source->GetNumChannels();

	for (int32_t i = -triangleWidth; i <= triangleWidth; ++i)
	{
		ltrb.left = offset / (scale * c) + xHalf + i;
		ltrb.top = 0;
		ltrb.right = ltrb.left;
		ltrb.bottom = triangleWidth - abs(i);
		DrawLine(ltrb, color);
	}
}

void djWaveView::DrawUpArrow(
	djAudioSource* source,
	int32_t scale,
	int32_t cueIndex,
	djColor color,
	int32_t triangleWidth)
{
	djRectLTRB ltrb;
	int16_t min;
	int16_t max;

	float xHalf = this->viewport.width / 2.0f;
	int y = this->viewport.height;

	int32_t offset = cueIndex - source->GetPosition();
	int32_t index = source->Peek(scale, offset, 0, &min, &max);
	int32_t c = source->GetNumChannels();

	for (int32_t i = -triangleWidth; i <= triangleWidth; ++i)
	{
		ltrb.left = offset / (scale * c) + xHalf + i;
		ltrb.top = y - triangleWidth + abs(i);
		ltrb.right = ltrb.left;
		ltrb.bottom = y;
		DrawLine(ltrb, color);
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

void djWaveView::DrawLoop(
	djAudioSource* source,
	int32_t scale,
	int32_t begIndex,
	int32_t endIndex,
	djColor color,
	int32_t lineHeight)
{
	djRectLTRB ltrb;
	int16_t min;
	int16_t max;

	float xHalf = this->viewport.width / 2.0f;
	int y = this->viewport.height;

	int32_t begOffset = begIndex - source->GetPosition();
	int32_t endOffset = endIndex - source->GetPosition();
	int32_t c = source->GetNumChannels();
	
	
	ltrb.left = begOffset / (scale * c) + xHalf;
	ltrb.top = y;
	ltrb.right = endOffset / (scale * c) + xHalf;
	ltrb.bottom = y;

	for (int32_t i = 0; i < lineHeight; ++i)
	{
		DrawLine(ltrb, color);
		--ltrb.top;
		--ltrb.bottom;
	}
}