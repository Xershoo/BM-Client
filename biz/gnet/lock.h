#pragma once

#include "gnet/platform.h"

//只能进程内使用的锁


namespace gnet
{
	class Mutex
	{
	private:
		Mutex(const Mutex&);
		void operator = (const Mutex&);


	public:
		Mutex(){InitializeCriticalSection(&m_cs);}
		~Mutex(){DeleteCriticalSection(&m_cs);}
		void Lock(){EnterCriticalSection(&m_cs);}
		void UnLock(){LeaveCriticalSection(&m_cs);}


	private:
		CRITICAL_SECTION m_cs;
	};



	class Scoped
	{
	private:
		Scoped(const Scoped&);
		void operator = (const Scoped&);


	public:
		explicit Scoped(Mutex& m):mutex(m){mutex.Lock();}
		~Scoped(){mutex.UnLock();}


	private:
		Mutex& mutex;
	};
}