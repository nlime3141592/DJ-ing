#pragma once

#include <wrl.h>

#include "djsw_gui_core_api.h"

using namespace Microsoft::WRL;

void SetMode_Line();
void SetMode_Triangle();
void DrawCall();

void OnGuiInit_Core();
void OnGuiUpdate_Core(ComPtr<ID3D12GraphicsCommandList> cmdList);
void OnGuiFinal_Core();