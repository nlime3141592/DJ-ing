#include "SampleText.h"

SampleText::SampleText() :
	_hwnd(NULL),
	_wszText(NULL),
	_cTextLength(0),
	_pD2DFactory(NULL),
	_pRT(NULL),
	_pBlackBrush(NULL),
	_pDWriteFactory(NULL),
	_pTextFormat(NULL)
{
	
}

SampleText::~SampleText()
{
	if (_pD2DFactory != NULL)
		_pD2DFactory->Release();
	if (_pRT != NULL)
		_pRT->Release();
	if (_pBlackBrush != NULL)
		_pBlackBrush->Release();
	if (_pDWriteFactory != NULL)
		_pDWriteFactory->Release();
	if (_pTextFormat != NULL)
		_pTextFormat->Release();
}

HRESULT SampleText::Init(HWND hwnd)
{
	HRESULT hr = S_OK;

	_hwnd = hwnd;

	hr = CreateDeviceIndependentResources();
	hr = CreateDeviceResources();

	return hr;
}

HRESULT SampleText::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Direct2D 팩토리 만들기
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&_pD2DFactory
	);

	// Shared DirectWrite 팩토리 만들기
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&_pDWriteFactory)
		);
	}

	// 텍스트 문자열 초기화
	_wszText = L"Hello World using DirectWrite!";
	_cTextLength = (UINT32)wcslen(_wszText);

	// 글자 스타일 지정
	if (SUCCEEDED(hr))
	{
		hr = _pDWriteFactory->CreateTextFormat(
			L"Arial",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			L"en-us",
			&_pTextFormat
		);
	}

	// 글자 정렬
	if (SUCCEEDED(hr))
	{
		hr = _pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = _pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	return hr;
}

HRESULT SampleText::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(_hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	if (!_pRT)
	{
		// Direct2D 렌더 타겟 만들기
		//D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		//D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(_hwnd, size);

		hr = _pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(
				_hwnd,
				size
			),
			&_pRT
		);

		if (SUCCEEDED(hr))
		{
			hr = _pRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Cyan),
				&_pBlackBrush);
		}
	}

	return hr;
}

void SampleText::DiscardDeviceResources()
{
	if (_pRT != NULL)
		_pRT->Release();
	if (_pBlackBrush != NULL)
		_pBlackBrush->Release();
}

HRESULT SampleText::Draw()
{
	RECT rc;

	GetClientRect(
		_hwnd,
		&rc);

	D2D1_RECT_F layoutRect = D2D1::RectF(
		static_cast<FLOAT>(rc.top) / _dpiScaleY,
		static_cast<FLOAT>(rc.left) / _dpiScaleX,
		static_cast<FLOAT>(rc.right - rc.left) / _dpiScaleX,
		static_cast<FLOAT>(rc.bottom - rc.top) / _dpiScaleY
	);

	_pRT->DrawTextW(
		_wszText,
		_cTextLength,
		_pTextFormat,
		layoutRect,
		_pBlackBrush
	);

	return S_OK;
}

HRESULT SampleText::DrawD2DContent()
{
	HRESULT hr;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		_pRT->BeginDraw();
		_pRT->SetTransform(D2D1::IdentityMatrix());
		_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

		hr = Draw();

		if (SUCCEEDED(hr))
		{
			hr = _pRT->EndDraw();
		}
	}

	if (FAILED(hr))
	{
		DiscardDeviceResources();
	}

	return hr;
}