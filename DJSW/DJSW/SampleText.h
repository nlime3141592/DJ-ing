#pragma once

#include <Windows.h>
#include <dwrite.h>
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class SampleText
{
public:
	SampleText();
	~SampleText();

	HRESULT Init(HWND hwnd);
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT Draw();
	HRESULT DrawD2DContent();

//private:

	HWND _hwnd;

	float _dpiScaleX;
	float _dpiScaleY;

	// Direct2D
	ID2D1Factory* _pD2DFactory;
	ID2D1HwndRenderTarget* _pRT;
	ID2D1SolidColorBrush* _pBlackBrush;

	// DirectWrite
	IDWriteFactory* _pDWriteFactory;
	IDWriteTextFormat* _pTextFormat;

	// content
	const wchar_t* _wszText;
	UINT32 _cTextLength;
};