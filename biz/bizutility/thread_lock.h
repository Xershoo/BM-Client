#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


namespace biz_utility
{
	class Mutex
	{
	private:
		Mutex(const Mutex&);
		void operator = (const Mutex&);


	public:
		Mutex(){ InitializeCriticalSection(&_cs); }
		~Mutex(){ DeleteCriticalSection(&_cs); }
		void Lock(){ EnterCriticalSection(&_cs); }
		void UnLock(){ LeaveCriticalSection(&_cs); }
		bool TryLock(){ return !!TryEnterCriticalSection(&_cs); }


	private:
		CRITICAL_SECTION _cs;
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