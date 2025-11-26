#pragma once

#include "audiochannel.h"
#include "djsw_gui_app.h"

class djControllerView : public djView
{
public:
	AudioChannel* channel;

	virtual void OnGuiInit();
	virtual void OnGuiUpdate();

private:
	void DrawGauge2(
		float value,
		float min,
		float max,
		djRectLTRB ltrb,
		djRectLTRB padding,
		int32_t count,
		int32_t gap,
		djColor c0,
		djColor c1);
	void DrawGauge3(float value,
		float min,
		float max,
		djRectLTRB ltrb,
		djRectLTRB padding,
		int32_t count,
		int32_t gap,
		djColor c0,
		djColor c1,
		djColor c2);
};