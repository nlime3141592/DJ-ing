#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>
#include <stdio.h>
#include <winerror.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>

#define REFTIMES_PER_SEC 10000000

// 참고자료:
// https://stackoverflow.com/questions/6464378/difference-safe-release-safe-delete
//#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = 0; }}
#define SAFE_DELETE(p) { if((a) != NULL) delete (a); (a) = NULL; }

#define EXIT_ON_ERROR(hres) \
			if (FAILED(hres)) { goto Exit; }

#define SAFE_RELEASE(punk) \
			if ((punk) != NULL) \
				{ (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

// 참고자료:
// https://learn.microsoft.com/ko-kr/windows/win32/coreaudio/device-properties
void PrintEndpointNames()
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDeviceCollection* pCollection = NULL; // 개별 IMMDevice 객체를 얻기 위해 이 포인터를 이용해야 함.
	IMMDevice* pEndpoint = NULL;
	IPropertyStore* pProps = NULL;
	LPWSTR pwszID = NULL;

	// 참고자료:
	// https://learn.microsoft.com/ko-kr/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator,
		NULL,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
	EXIT_ON_ERROR(hr);

	UINT count;

	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr);

	if (count == 0)
	{
		wprintf(L"No endpoints found.\n");
	}

	// Each loop prints the name of an endpoint device.
	for (ULONG i = 0; i < count; ++i)
	{
		// Get pointer to endpoint #i.
		hr = pCollection->Item(i, &pEndpoint);
		EXIT_ON_ERROR(hr);

		// Get the endpoint ID string.
		hr = pEndpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr);
		hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
		EXIT_ON_ERROR(hr);

		PROPVARIANT varName;

		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		// 참고자료:
		// https://learn.microsoft.com/ko-kr/windows/win32/coreaudio/pkey-device-friendlyname
		hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr);

		// GetValue succeeds and returns S_OK if PKEY_Device_FriendlyName is not found.
		// In this case vartName.vt is set to VT_EMPTY.
		if (varName.vt != VT_EMPTY)
		{
			// Print endpoint friendly name and endpoint ID.
			wprintf(L"Endpoint %d: \"%s\" (%s)\n", i, varName.pwszVal, pwszID);
		}

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);
		SAFE_RELEASE(pProps);
		SAFE_RELEASE(pEndpoint);
	}

	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);

	return;

Exit:
	wprintf(L"Error\n");
	CoTaskMemFree(pwszID);
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);
	SAFE_RELEASE(pEndpoint);
	SAFE_RELEASE(pProps);
}

void PrintDevice(IMMDevice* pDevice)
{
	HRESULT hr = S_OK;
	IPropertyStore* pProps = NULL;
	LPWSTR pwszID = NULL;

	// Get the endpoint ID string.
	hr = pDevice->GetId(&pwszID);
	EXIT_ON_ERROR(hr);
	hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
	EXIT_ON_ERROR(hr);

	PROPVARIANT varName;

	// Initialize container for property value.
	PropVariantInit(&varName);

	// Get the endpoint's friendly-name property.
	// 참고자료:
	// https://learn.microsoft.com/ko-kr/windows/win32/coreaudio/pkey-device-friendlyname
	hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
	EXIT_ON_ERROR(hr);

	// GetValue succeeds and returns S_OK if PKEY_Device_FriendlyName is not found.
	// In this case vartName.vt is set to VT_EMPTY.
	if (varName.vt != VT_EMPTY)
	{
		// Print endpoint friendly name and endpoint ID.
		wprintf(L"Device: \"%s\" (%s)\n", varName.pwszVal, pwszID);
	}

	CoTaskMemFree(pwszID);
	pwszID = NULL;
	PropVariantClear(&varName);
	SAFE_RELEASE(pProps);

	return;

Exit:
	wprintf(L"Error\n");
	CoTaskMemFree(pwszID);
	SAFE_RELEASE(pProps);
}

UINT GetDevices(IMMDeviceCollection** ppDeviceCollection)
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* pEnumerator = NULL;
	IMMDeviceCollection* pCollection = NULL; // 개별 IMMDevice 객체를 얻기 위해 이 포인터를 이용해야 함.
	
	// 참고자료:
	// https://learn.microsoft.com/ko-kr/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator,
		NULL,
		CLSCTX_ALL,
		IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);
	EXIT_ON_ERROR(hr);

	hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
	EXIT_ON_ERROR(hr);

	UINT count;

	hr = pCollection->GetCount(&count);
	EXIT_ON_ERROR(hr);

	if (count == 0)
	{
		*ppDeviceCollection = NULL;
		wprintf(L"No endpoints found.\n");
		return count;
	}

	*ppDeviceCollection = pCollection;
	return count;

Exit:
	wprintf(L"Error\n");
	SAFE_RELEASE(pEnumerator);
	SAFE_RELEASE(pCollection);
}

// 참고자료:
// https://learn.microsoft.com/en-us/windows/win32/api/audioclient/nf-audioclient-iaudioclient-initialize
HRESULT CreateAudioClient(IMMDevice* pDevice, IAudioClient** ppAudioClient)
{
	if (!pDevice)
	{
		return E_INVALIDARG;
	}

	if (!ppAudioClient)
	{
		return E_POINTER;
	}

	HRESULT hr = S_OK;
	WAVEFORMATEX* pwfx = NULL;
	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	UINT32 nFrames = 0;

	IAudioClient* pAudioClient = NULL;

	// Get the audio client.
	hr = pDevice->Activate(
		__uuidof(IAudioClient),
		CLSCTX_ALL,
		NULL,
		(void**)&pAudioClient);

	// Get the device format.
	hr = pAudioClient->GetMixFormat(&pwfx);

	// Open the stream and associate it with an audio session.
	hr = pAudioClient->Initialize(
		AUDCLNT_SHAREMODE_EXCLUSIVE,
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		hnsRequestedDuration,
		hnsRequestedDuration,
		pwfx,
		NULL);

	// If the requested buffer size is not aligned...
	if (hr == AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED)
	{
		// Get the next aligned frame.
		hr = pAudioClient->GetBufferSize(&nFrames);

		hnsRequestedDuration = (REFERENCE_TIME)((10000.0 * 1000 / pwfx->nSamplesPerSec * nFrames) + 0.5);

		// Release the previous allocations.
		SAFE_RELEASE(pAudioClient);
		CoTaskMemFree(pwfx);

		// Create a new audio client.
		hr = pDevice->Activate(
			_uuidof(IAudioClient),
			CLSCTX_ALL,
			NULL,
			(void**)&pAudioClient);

		// Get the device format.
		hr = pAudioClient->GetMixFormat(&pwfx);

		// Open the stream and associate it with an audio session.
		hr = pAudioClient->Initialize(
			AUDCLNT_SHAREMODE_EXCLUSIVE,
			AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
			hnsRequestedDuration,
			hnsRequestedDuration,
			pwfx,
			NULL);
	}
	else
	{
		// CHECK_HR(hr);
	}

	// Return to the caller.
	*(ppAudioClient) = pAudioClient;
	(*ppAudioClient)->AddRef();

	// Clean up.
	CoTaskMemFree(pwfx);
	SAFE_RELEASE(pAudioClient);

	return hr;
}

int main1()
{
	// 인코딩 문제 해결을 위함
	// wprintf() + _setmode() 구조
	_setmode(_fileno(stdout), _O_U16TEXT);

	// 해당 함수가 호출되어야만 COM 라이브러리를 사용할 수 있음.
	// 참고자료:
	// https://learn.microsoft.com/ko-kr/windows/win32/api/objbase/nf-objbase-coinitialize
	CoInitialize(NULL);
	//PrintEndpointNames();

	IMMDeviceCollection* pCollection = NULL;
	UINT count = 0;

	count = GetDevices(&pCollection);
	wprintf(L"Count == %d\n", count);

	for (ULONG i = 0; i < count; ++i)
	{
		IMMDevice* device = NULL;
		pCollection->Item(i, &device);
		PrintDevice(device);
	}

	CoUninitialize();

	wprintf(L"hello world\n");

	return 0;
}