#include "djsw_testing_main.h"

#include <string>

#include "djsw_file_metadata.h"
#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

static djWavMetaFile metafile;

int WINAPI TestInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	// if bypass true, then, hid loop doesn't push input event.
	hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_DEBUG].bypass = false;

    return 1;
}

int WINAPI TestUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
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

    return 1;
}

int WINAPI TestFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    return 1;
}