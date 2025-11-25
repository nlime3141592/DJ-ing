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
};