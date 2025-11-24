#include "LoopJob.h"

djMessageQueue<djJob> jobQueues[DJSW_JOB_QUEUE_COUNT];

static void JobInit()
{
	
}

static void ProcessJob(djMessageQueue<djJob>* queue)
{
	djJob message;
	djChronoMCS _jobTime = 0;

	if (!queue->Pop(&message))
		return;

	while (_jobTime < DJSW_JOB_TIME_SLICE_MCS)
		_jobTime += message.Run();

	if (message.done.load(std::memory_order_acquire))
		return;

	// TODO: 임시 구조, 큐가 꽉 찬 경우 무한 루프가 걸릴 수 있음. 해결 필요.
	while (!queue->Push(&message));
}

static void JobUpdate()
{
	for (int i = 0; i < DJSW_JOB_QUEUE_COUNT; ++i)
	{
		ProcessJob(jobQueues + i);
	}
}

static void JobFinal()
{

}

DWORD WINAPI JobMain(LPVOID lpParams)
{
	JobParams* jobParams = (JobParams*)lpParams;

	JobInit();

	// 초기화 로직 동기화를 위함.
	jobParams->loopBaseParams.interruptNumber = DJSW_INT_NULL;

	while (jobParams->loopBaseParams.interruptNumber != 1)
	{
		JobUpdate();
	}

	JobFinal();

	return 1;
}