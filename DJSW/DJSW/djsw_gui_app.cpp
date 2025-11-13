#include "djsw_gui_app.h"

#include "djsw_gui_app_wave1.h"

static djWaveView _waveView1;

void OnGuiInit_App()
{
	_waveView1.OnGuiInit();

	SetView(&_waveView1, 0);
}