#include <Windows.h>

#include <assert.h>
#include "LoopBase.h"

#include "djsw_file_metadata.h"
#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

#include <string>

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    HRESULT hr;
    
    //HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    assert(hr == S_OK);

    LoopInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    // if bypass true, then, hid loop doesn't push input event.
    hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_DEBUG].bypass = false;

    djWavMetaFile metafile = djWavMetaFile();

    while (LoopUpdate(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
    {
        // Debug logic here.
        HidMessage hidmsg;

        while (hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_DEBUG].Pop(&hidmsg))
        {
            switch (hidmsg.hidKey)
            {
            case DJSW_HID_SPLIT1:
                if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
                {
                    metafile.Open(L"C:\\Test\\meta.djmeta");
                    OutputDebugStringW(L"File Opened.\n");
                }
                break;
            case DJSW_HID_SPLIT2:
                if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
                {
                    metafile.SetHotCueIndex(2, 3456);
                    metafile.Save();
                    metafile.Close();
                    OutputDebugStringW(L"File Closed.\n");
                }
            }
        }
    }
    
    LoopFinal(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return 0;
}
