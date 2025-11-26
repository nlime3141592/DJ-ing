#pragma once

#include "audiochannel.h"
#include "djsw_gui_app.h"

class djWaveView : public djView
{
public:
	AudioChannel* channel;

	virtual void OnGuiInit();
	virtual void OnGuiUpdate();
	virtual void OnDrawWave();

private:
	void DrawDownArrow(
		djAudioSource* source,
		int32_t scale,
		int32_t cueIndex,
		djColor color,
		int32_t triangleWidth);

	void DrawUpArrow(
		djAudioSource* source,
		int32_t scale,
		int32_t cueIndex,
		djColor color,
		int32_t triangleWidth);

	void DrawCuePoint(
		djAudioSource* source,
		int32_t scale,
		int32_t cueIndex,
		int32_t begIndex,
		int32_t endIndex,
		djColor color,
		int32_t lineWidth);

	void DrawLoop(
		djAudioSource* source,
		int32_t scale,
		int32_t begIndex,
		int32_t endIndex,
		djColor color,
		int32_t lineHeight);
};