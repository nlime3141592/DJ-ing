#pragma once

#include <stdint.h>
#include <Windows.h>

#include "djsw_job_api.h"
#include "djsw_message_queue.h"
#include "LoopBase.h"

typedef struct
{
	LoopBaseParams loopBaseParams;
} JobParams;

DWORD WINAPI JobMain(LPVOID lpParams);