#pragma once

// start,stop �ڵ����߳���, ���ǻص�TimerArriveҲ�ǲ���֤�̵߳ġ�
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