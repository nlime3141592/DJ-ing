#include "djsw_testing_main.h"

#include <string>

#include "audiochannel.h"
#include "djsw_audio_analyzer.h"
#include "djsw_audio_api.h"
#include "djsw_file_metadata.h"
#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"

static djWavMetaFile metafile;
static djWavGridData grid1 = { 0 };
static djWavGridData grid2 = { 0 };

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
                djAudioAnalyzerParams params;
                
                params.bpmMin = 96.0f;
                params.bpmMax = 140.0f;
                params.bpmUnit = 0.25f;
                params.channelCount = 2;
                params.sampleRate = 44100;

                params.sampleCount = 0;
                params.samples = 0;

                AudioChannel* channel = GetChannel(0);
                params.sampleCount = channel->numWavSamples;
                params.samples = channel->wavSamples;

                AnalyzeGridData(&params, &grid1);

                std::wstring message = L"";
                message += L"BPM: ";
                message += std::to_wstring(grid1.bpm);
                message += L", Offset: ";
                message += std::to_wstring(grid1.firstBarIndex);
                message += L"\n";
                OutputDebugStringW(message.c_str());
            }
            break;
        case DJSW_HID_SPLIT2:
            if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
            {
                djAudioAnalyzerParams params;
                
                params.bpmMin = 125.0f;
                params.bpmMax = 135.0f;
                params.bpmUnit = 0.25f;
                params.channelCount = 2;
                params.sampleRate = 44100;

                params.sampleCount = 0;
                params.samples = 0;

                AudioChannel* channel = GetChannel(1);
                params.sampleCount = channel->numWavSamples;
                params.samples = channel->wavSamples;

                AnalyzeGridData(&params, &grid2);

                std::wstring message = L"";
                message += L"BPM: ";
                message += std::to_wstring(grid2.bpm);
                message += L", Offset: ";
                message += std::to_wstring(grid2.firstBarIndex);
                message += L"\n";
                OutputDebugStringW(message.c_str());
            }
            break;
        case DJSW_HID_LD2:
            if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
            {
                AudioChannel* channel = GetChannel(1);
                channel->JumpImmediate(grid2.firstBarIndex);
            }
            break;
        default:
            break;
        }
    }

    return 1;
}

int WINAPI TestFinal(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    return 1;
}