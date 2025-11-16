#pragma once

#include "djsw_gui_app.h"

class djWaveView : public djView
{
public:
	int idxChannel;

	virtual void OnGuiInit();
	virtual void OnGuiUpdate();
	virtual void OnDrawWave();
};