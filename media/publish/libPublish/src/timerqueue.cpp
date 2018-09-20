#include "timerqueue.h"
#include <assert.h>


namespace biz_utility
{
	ITimerQueue::ITimerQueue()
		:	_timerQue(NULL),
			_timer(NULL),
			_hCompleteEvent(NULL),
			_period(0),
			_isStop(0),
			m_TimerArrive(0),
			m_pUser(0)
	{}


	ITimerQueue::~ITimerQueue() { StopTimer(); }


	bool ITimerQueue::StartTimer(UINT period,pTimerArrive  ptimerArrive,void * pUser)
	{
		m_TimerArrive = ptimerArrive ;
		m_pUser = pUser;
		_period = period;
		InterlockedExchange(&_isStop, 0);
		_timerQue = CreateTimerQueue();
		if (NULL == _timerQue)
			return false;

		BOOL bRet = CreateTimerQueueTimer(&_timer, _timerQue,
			TimerQueueCallback, this, _period, 0, WT_EXECUTEDEFAULT);
		if (!bRet)	return false;

		_hCompleteEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
		return true;
	}


	void ITimerQueue::StopTimer()
	{
		InterlockedExchange(&_isStop, 1);
		if (_hCompleteEvent)
			WaitForSingleObject(_hCompleteEvent, INFINITE);

		if (_timer)
		{
			// 等待回调执行完成
			DeleteTimerQueueTimer(_timerQue, _timer, INVALID_HANDLE_VALUE);
			_timer = NULL;
		}

		if (_timerQue)
		{
			CloseHandle(_hCompleteEvent);
			_hCompleteEvent = NULL;
		}

		if (_hCompleteEvent)
		{
			CloseHandle(_hCompleteEvent);
			_hCompleteEvent = NULL;
		}
	}


	VOID CALLBACK ITimerQueue::TimerQueueCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
	{
		ITimerQueue* pTimer = reinterpret_cast<ITimerQueue*>(lpParameter);
		ResetEvent(pTimer->_hCompleteEvent);
		assert(TRUE == TimerOrWaitFired);

		if(pTimer->m_TimerArrive)
		{
			pTimer->m_TimerArrive(pTimer->m_pUser);
		}

		if (0 == InterlockedCompareExchange(&pTimer->_isStop, 0, 0))
		{
			// not INVALID_HANDLE_VALUE
			DeleteTimerQueueTimer(pTimer->_timerQue, pTimer->_timer, NULL);
			BOOL bRet = CreateTimerQueueTimer(&pTimer->_timer, pTimer->_timerQue,
				TimerQueueCallback, pTimer, pTimer->_period, 0, WT_EXECUTEDEFAULT);
			assert(bRet);
			assert(pTimer->_timer);
		}

		SetEvent(pTimer->_hCompleteEvent);
	}
}