#include "djsw_gui_app.h"

#include "djsw_gui_app_wave.h"

static djWaveView _waveView1;
static djWaveView _waveView2;

static void OnGuiInit_WaveView1()
{
	_waveView1.OnGuiInit();

	_waveView1.viewport.topLeftX = 1280;
	_waveView1.viewport.topLeftY = 0;
	_waveView1.viewport.width = 2560;
	_waveView1.viewport.height = 360;

	_waveView1.idxChannel = 0;

	SetView(&_waveView1, 0);
}

static void OnGuiInit_WaveView2()
{
	_waveView2.OnGuiInit();

	_waveView2.viewport.topLeftX = 1280;
	_waveView2.viewport.topLeftY = 360;
	_waveView2.viewport.width = 2560;
	_waveView2.viewport.height = 360;

	_waveView2.idxChannel = 1;

	SetView(&_waveView2, 1);
}

void OnGuiInit_App()
{
	OnGuiInit_WaveView1();
	OnGuiInit_WaveView2();
}