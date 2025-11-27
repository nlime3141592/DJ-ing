#include "djsw_gui_app.h"

#include "djsw_audio_api.h"
#include "djsw_gui_app_controller.h"
#include "djsw_gui_app_wave.h"

djColor _clrChannels[4] = {
	GetColorByRGB(255, 64, 0),
	GetColorByRGB(0, 64, 255),
	GetColorByRGB(255, 0, 64),
	GetColorByRGB(64, 255, 0)
};

djColor _clrGlobalCue = GetColorByRGB(255, 255, 255);
djColor _clrGlobalCueButton[2] = {
	GetColorByRGB(242, 213, 129),
	GetColorByRGB(217, 191, 116)
};
djColor _clrPlayButton[2] = {
	GetColorByRGB(120, 240, 111),
	GetColorByRGB(93, 185, 86)
};

djColor _clrPadFn = GetColorByRGB(255, 201, 14);
djColor _clrHotCues[8] = {
	GetColorByRGB(255, 55, 111),
	GetColorByRGB(69, 172, 219),
	GetColorByRGB(125, 193, 61),
	GetColorByRGB(170, 114, 255),
	GetColorByRGB(48, 210, 110),
	GetColorByRGB(224, 100, 27),
	GetColorByRGB(48, 90, 255),
	GetColorByRGB(195, 175, 4)
};

djColor _clrLoop = GetColorByRGB(255, 201, 14);
djColor _clrCrossfader = GetColorByRGB(200, 191, 231);

djColor _clrFXsMin[4] = {
	GetColorByRGB(39, 42, 61),
	GetColorByRGB(39, 42, 61),
	GetColorByRGB(39, 42, 61),
	GetColorByRGB(39, 42, 61)
};

djColor _clrFXsMax[4] = {
	GetColorByRGB(167, 248, 76),
	GetColorByRGB(255, 64, 96),
	GetColorByRGB(125, 193, 61),
	GetColorByRGB(48, 168, 255)
	//GetColorByRGB(170, 114, 255)
};

djColor _clrTempoSlider[3] = {
	GetColorByRGB(128, 255, 128),
	GetColorByRGB(225, 255, 225),
	GetColorByRGB(255, 128, 128)
};

djColor _clrDisables[4] = {
	GetColorAsGrayscale(0.21f),
	GetColorAsGrayscale(0.23f),
	GetColorAsGrayscale(0.25f),
	GetColorAsGrayscale(0.27f)
};
djColor _clrGuideline = GetColorAsGrayscale(0.85f);

static djControllerView _ctrlView1;
static djControllerView _ctrlView2;

static djWaveView _waveView1;
static djWaveView _waveView2;

static void OnGuiInit_CtrlView1()
{
	_ctrlView1.OnGuiInit();

	_ctrlView1.viewport.topLeftX = 0;
	_ctrlView1.viewport.topLeftY = 0;
	_ctrlView1.viewport.width = 1280;
	_ctrlView1.viewport.height = 360;

	_ctrlView1.channel = GetAudioChannel(0);

	SetView(&_ctrlView1, 2);
}

static void OnGuiInit_CtrlView2()
{
	_ctrlView2.OnGuiInit();

	_ctrlView2.viewport.topLeftX = 0;
	_ctrlView2.viewport.topLeftY = 360;
	_ctrlView2.viewport.width = 1280;
	_ctrlView2.viewport.height = 360;

	_ctrlView2.channel = GetAudioChannel(1);

	SetView(&_ctrlView2, 3);
}

static void OnGuiInit_WaveView1()
{
	_waveView1.OnGuiInit();

	_waveView1.viewport.topLeftX = 1280;
	_waveView1.viewport.topLeftY = 0;
	_waveView1.viewport.width = 2560;
	_waveView1.viewport.height = 360;

	_waveView1.channel = GetAudioChannel(0);

	SetView(&_waveView1, 0);
}

static void OnGuiInit_WaveView2()
{
	_waveView2.OnGuiInit();

	_waveView2.viewport.topLeftX = 1280;
	_waveView2.viewport.topLeftY = 360;
	_waveView2.viewport.width = 2560;
	_waveView2.viewport.height = 360;

	_waveView2.channel = GetAudioChannel(1);

	SetView(&_waveView2, 1);
}

void OnGuiInit_App()
{
	OnGuiInit_CtrlView1();
	OnGuiInit_CtrlView2();

	OnGuiInit_WaveView1();
	OnGuiInit_WaveView2();
}