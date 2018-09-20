#pragma once
#include "ITimer.h"
#include "ExtraTimer.h"


template<typename T>
class TimerHandler : public ITimerSenseObj
{
	typedef void (T::*callback)();
public:
	TimerHandler()
	{
		_x = NULL;
		__start = false;
	}
	TimerHandler(T* x){
		_x = x;
		__start = false;
	}

	~TimerHandler(){
		stop();
	}

	void init(T* x, callback cb)
	{
		_x = x;
		ptr = cb;
	}
	bool hasStart(){
			return __start;
	}
	void start(uint32_t timeout)
	{
		CTimer::Instance()->UnRegisterListener(this);
		__start = true;
		CTimer::Instance()->RegisterListener(this, timeout);
	}
	void stop()
	{
		__start = false;
		CTimer::Instance()->UnRegisterListener(this);
	}
	virtual void TimeArrive()
	{
		if(_x) 
		{		
			(_x->*ptr)();
		}
	}
protected:
	T* _x;
	callback ptr;
	bool __start;
};