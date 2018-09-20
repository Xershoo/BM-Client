#pragma once

#include "../timerqueue.h"


class ATimer : public ITimerQueue
{
public:
	virtual void TimerArrive()
	{
		static int cnt = 0;
		printf("%d\n", ++cnt);
	}
};

ATimer atimer;

void test_timerqueue_start()
{
	atimer.StartTimer(1);
};


void test_timerqueue_stop()
{
	atimer.StopTimer();
}