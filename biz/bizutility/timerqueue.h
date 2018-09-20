#pragma once

// start,stop 在调用线程里, 但是回调TimerArrive也是不保证线程的。
#include <Windows.h>


namespace biz_utility
{
	class ITimerQueue
	{
	public:
		ITimerQueue();
		~ITimerQueue();

		bool StartTimer(UINT period);
		void StopTimer();


	private:
		virtual void TimerArrive() = 0;
		static VOID CALLBACK TimerQueueCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);


	private:
		HANDLE	_timerQue;
		HANDLE	_timer;
		HANDLE	_hCompleteEvent;
		UINT	_period;
		LONG	_isStop;
	};
}