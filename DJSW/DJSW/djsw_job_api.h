#pragma once

#include <atomic>

#include "djsw_job_api.h"
#include "djsw_message_queue.h"
#include "djsw_util_timer.h"

#define DJSW_JOB_QUEUE_COUNT 4
#define DJSW_JOB_TIME_SLICE_MCS 3000

#define DJSW_JOB_NO_ERROR 0
#define DJSW_JOB_ERROR_ON_INIT 1
#define DJSW_JOB_ERROR_ON_UPDATE 2
#define DJSW_JOB_ERROR_ON_FINAL 3

#define DJSW_JOB_STATE_BEGIN 3
#define DJSW_JOB_STATE_INIT 2
#define DJSW_JOB_STATE_UPDATE 1
#define DJSW_JOB_STATE_FINAL 0
#define DJSW_JOB_STATE_END -1

class djJob
{
public:
	djJob();
	djJob(const djJob& job);

	djChronoMCS Run();

	std::atomic<bool> done;
	std::atomic<int> error;
	std::atomic<int> state;

	std::atomic<bool(*)()> functions[DJSW_JOB_STATE_BEGIN];

	djJob& operator=(const djJob& other)
	{
		done.store(other.done.load(std::memory_order_relaxed), std::memory_order_relaxed);
		error.store(other.error.load(std::memory_order_relaxed), std::memory_order_relaxed);
		state.store(other.state.load(std::memory_order_relaxed), std::memory_order_relaxed);

		for (int i = 0; i < DJSW_JOB_STATE_BEGIN; ++i)
		{
			functions[i].store(other.functions[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
		}

		return *this;
	}

private:
};

extern djMessageQueue<djJob> jobQueues[DJSW_JOB_QUEUE_COUNT];