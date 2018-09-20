#pragma once

// start,stop �ڵ����߳���, ���ǻص�TimerArriveҲ�ǲ���֤�̵߳ġ�
#include <Windows.h>

namespace biz_utility
{
	typedef  void (*pTimerArrive)(void * pUser);
	class ITimerQueue
	{
	public:
		ITimerQueue();
		~ITimerQueue();

		bool StartTimer(UINT period,pTimerArrive  ptimerArrive,void * pUser);
		void StopTimer();

	public:
		pTimerArrive  m_TimerArrive;
	private:
		static VOID CALLBACK TimerQueueCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);
	private:
		HANDLE	_timerQue;
		HANDLE	_timer;
		HANDLE	_hCompleteEvent;
		UINT	_period;
		LONG	_isStop;
		void*    m_pUser;
	};
}