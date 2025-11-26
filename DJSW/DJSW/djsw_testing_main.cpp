#include "djsw_testing_main.h"

#include <string>

#include "audiochannel.h"
#include "djsw_audio_analyzer.h"
#include "djsw_audio_api.h"
#include "djsw_file_metadata.h"
#include "djsw_input_hid.h"
#include "djsw_input_hid_controls.h"
#include "djsw_job_api.h"

static djWavMetaFile metafile;
static djWavGridData grid1 = { 0 };
static djWavGridData grid2 = { 0 };

static djAnalyzeJob analyzer0;
static djAnalyzeJob analyzer1;

static djJob job0;
static djJob job1;

static bool JobInit0()
{
    return analyzer0.Init();
}

static bool JobUpdate0()
{
    return analyzer0.Analyze();
}

static bool JobFinal0()
{
    if (analyzer0.Final())
    {
        djWavGridData data = analyzer0.result;

        std::wstring message = L"";
        message += L"analyzer-0 BPM: ";
        message += std::to_wstring(data.bpm);
        message += L", Offset: ";
        message += std::to_wstring(data.firstBarIndex);
        message += L", ErrorCode: ";
        message += std::to_wstring(job0.error);
        message += L"\n";
        OutputDebugStringW(message.c_str());

        analyzer0.Release();

        return true;
    }
    
    return false;
}

static bool JobInit1()
{
    return analyzer1.Init();
}

static bool JobUpdate1()
{
    return analyzer1.Analyze();
}

static bool JobFinal1()
{
    if (analyzer1.Final())
    {
        djWavGridData data = analyzer1.result;

        std::wstring message = L"";
        message += L"analyzer-1 BPM: ";
        message += std::to_wstring(data.bpm);
        message += L", Offset: ";
        message += std::to_wstring(data.firstBarIndex);
        message += L", ErrorCode: ";
        message += std::to_wstring(job1.error);
        message += L"\n";
        OutputDebugStringW(message.c_str());

        analyzer1.Release();

        return true;
    }

    return false;
}

int WINAPI TestInit(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	// if bypass true, then, hid loop doesn't push input event.
	hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_DEBUG].bypass = false;
    jobQueues[0].bypass = false;

    return 1;
}

int WINAPI TestUpdate(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    HidMessage hidmsg;

    if (job0.done.load(std::memory_order_acquire))
    {
        analyzer0.Release();
        job0.done.store(false, std::memory_order_release);
    }

    while (hidMessageQueues[DJSW_HID_MESSAGE_QUEUE_IDX_DEBUG].Pop(&hidmsg))
    {
        switch (hidmsg.hidKey)
        {
        case DJSW_HID_SPLIT1:
            if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
            {
                AudioChannel* channel = GetAudioChannel(0);

                analyzer0 = djAnalyzeJob(
                    channel->GetSource()->GetWavFilePath(),
                    96.0f, // bpmMin
                    140.0f, // bpmMax
                    0.25f // bpmUnit
                );

                job0.functions[DJSW_JOB_STATE_INIT].store(JobInit0, std::memory_order_release);
                job0.functions[DJSW_JOB_STATE_UPDATE].store(JobUpdate0, std::memory_order_release);
                job0.functions[DJSW_JOB_STATE_FINAL].store(JobFinal0, std::memory_order_release);

                jobQueues[0].Push(&job0);

                OutputDebugStringW(L"Push OK - 0\n");
            }
            break;
        case DJSW_HID_SPLIT2:
            if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
            {
                AudioChannel* channel = GetAudioChannel(1);

                analyzer1 = djAnalyzeJob(
                    channel->GetSource()->GetWavFilePath(),
                    96.0f, // bpmMin
                    140.0f, // bpmMax
                    0.25f // bpmUnit
                );

                job1.functions[DJSW_JOB_STATE_INIT].store(JobInit1, std::memory_order_release);
                job1.functions[DJSW_JOB_STATE_UPDATE].store(JobUpdate1, std::memory_order_release);
                job1.functions[DJSW_JOB_STATE_FINAL].store(JobFinal1, std::memory_order_release);

                jobQueues[0].Push(&job1);

                OutputDebugStringW(L"Push OK - 1\n");
            }
            break;
        case DJSW_HID_LD2:
            if (hidmsg.message == DJSW_HID_MASK_MESSAGE_KEY_DOWN)
            {
                // Do anything.
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