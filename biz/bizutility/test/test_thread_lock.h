#pragma once

#include "../thread_lock.h"
using namespace biz_utility;


class LockObj
{
public:
	void thread1()
	{
		Scoped::Scoped(_mutex);
		// xxx for thread1
	}

	void thread2()
	{
		Scoped::Scoped(_mutex);
		// xxx for thread2
	}


private:
	Mutex _mutex;
};


void test_thread_lock()
{
}