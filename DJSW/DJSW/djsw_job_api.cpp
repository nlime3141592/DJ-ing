#include "djsw_job_api.h"

#pragma warning(disable: 26495)
djJob::djJob() :
	done(false),
	error(DJSW_JOB_NO_ERROR),
	state(DJSW_JOB_STATE_BEGIN)
{
	
}

djJob::djJob(const djJob& job)
{
	done.store(job.done.load(std::memory_order_relaxed), std::memory_order_relaxed);
	error.store(job.error.load(std::memory_order_relaxed), std::memory_order_relaxed);
	state.store(job.state.load(std::memory_order_relaxed), std::memory_order_relaxed);

	for (int i = 0; i < DJSW_JOB_STATE_BEGIN; ++i)
		functions[i].store(job.functions[i].load(std::memory_order_relaxed), std::memory_order_relaxed);
}

djChronoMCS djJob::Run()
{
	djChronoClock beg = CHRONO_NOW;

	switch (state)
	{
	case DJSW_JOB_STATE_BEGIN:
		state.fetch_sub(1, std::memory_order_relaxed);
		break;
	case DJSW_JOB_STATE_INIT:
		if (functions[state].load(std::memory_order_acquire)())
		{
			state.fetch_sub(1, std::memory_order_relaxed);
		}
		else
		{
			error.store(DJSW_JOB_ERROR_ON_INIT, std::memory_order_release);
			state.store(DJSW_JOB_STATE_END, std::memory_order_release);
			done.store(true, std::memory_order_release);
		}
		break;
	case DJSW_JOB_STATE_UPDATE:
		if (functions[state].load(std::memory_order_acquire)())
			state.fetch_sub(1, std::memory_order_relaxed);
		break;
	case DJSW_JOB_STATE_FINAL:
		if (functions[state].load(std::memory_order_acquire)())
		{
			state.fetch_sub(1, std::memory_order_relaxed);
		}
		else
		{
			error.store(DJSW_JOB_ERROR_ON_FINAL, std::memory_order_release);
			state.store(DJSW_JOB_STATE_END, std::memory_order_release);
			done.store(true, std::memory_order_release);
		}
		break;
	case DJSW_JOB_STATE_END:
		if (!done.load(std::memory_order_acquire))
			done.store(true, std::memory_order_release);
		break;
	}

	return CHRONO_LENGTH_MCS(beg, CHRONO_NOW);
}