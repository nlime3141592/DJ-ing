#pragma once

#include <mmdeviceapi.h>
#include <Audioclient.h>

#include <stdint.h>
#include <assert.h>

void InitAudioDevice_Global();

class AudioDevice
{
public:
	void Init();
	void Start();

	bool FindDefaultDevice();
	bool Connect();
	void Disconnect();

	IMMDevice* audioDevice;
	IAudioClient2* audioClient;
	IAudioRenderClient* audioRenderClient;

private:
	
};