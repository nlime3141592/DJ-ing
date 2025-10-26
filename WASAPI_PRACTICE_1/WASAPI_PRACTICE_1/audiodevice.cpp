#include "audiodevice.h"

void InitAudioDevice_Global()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
	assert(hr == S_OK);
}

bool AudioDevice::FindDefaultDevice()
{
	IMMDeviceEnumerator* deviceEnumerator;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)(&deviceEnumerator));

	if (FAILED(hr))
		return false;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);
	deviceEnumerator->Release();

	return SUCCEEDED(hr);
}

bool AudioDevice::Connect()
{
	if (!audioDevice)
		return false;

	HRESULT hr = audioDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (LPVOID*)&audioClient);

	if (FAILED(hr))
		return false;

	return true;
}

void AudioDevice::Disconnect()
{
	if (audioRenderClient)
	{
		audioRenderClient->Release();
		audioRenderClient = nullptr;
	}

	if (audioClient)
	{
		audioClient->Release();
		audioClient = nullptr;
	}

	if (audioDevice)
	{
		audioDevice->Release();
		audioDevice = nullptr;
	}
}

void AudioDevice::Init()
{
	IMMDeviceEnumerator* deviceEnumerator;
	HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)(&deviceEnumerator));
	assert(hr == S_OK);

	//IMMDevice* audioDevice;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);
	assert(hr == S_OK);

	deviceEnumerator->Release();

	hr = audioDevice->Activate(__uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (LPVOID*)(&audioClient));
	assert(hr == S_OK);

	audioDevice->Release();
}

void AudioDevice::Start()
{
	WAVEFORMATEX mixFormat = {};
	mixFormat.wFormatTag = WAVE_FORMAT_PCM;
	mixFormat.nChannels = 2;
	mixFormat.nSamplesPerSec = 44100; //defaultMixFormat->nSamplesPerSec;
	mixFormat.wBitsPerSample = 16;
	mixFormat.nBlockAlign = (mixFormat.nChannels * mixFormat.wBitsPerSample) / 8;
	mixFormat.nAvgBytesPerSec = mixFormat.nSamplesPerSec * mixFormat.nBlockAlign;

	const float BUFFER_SIZE_IN_SECONDS = 2.0f;
	const int64_t REFTIMES_PER_SEC = 10000000; // hundred nanoseconds
	REFERENCE_TIME requestedSoundBufferDuration = (REFERENCE_TIME)(REFTIMES_PER_SEC * BUFFER_SIZE_IN_SECONDS);
	DWORD initStreamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST
		| AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
		| AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
	HRESULT hr = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
		initStreamFlags,
		requestedSoundBufferDuration,
		0, &mixFormat, nullptr);
	assert(hr == S_OK);

	//IAudioRenderClient* audioRenderClient;
	hr = audioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID*)(&audioRenderClient));
	assert(hr == S_OK);

	UINT32 bufferSizeInFrames;
	hr = audioClient->GetBufferSize(&bufferSizeInFrames);
	assert(hr == S_OK);

	hr = audioClient->Start();
	assert(hr == S_OK);
}