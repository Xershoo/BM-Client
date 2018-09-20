#include <stdio.h>

#include "test_signal/test_signal1_1.h"
//#include "test_thread_lock.h"
//#include "test_signal/test_sigslot0.h"
//#include "test_signal/test_sigslot1.h"
//#include "test_signal/test_sigslot2.h"
//#include "test_signal/test_sigslot3.h"
//#include "test_signal/test_sigslot4.h"
//#include "test_log/test_log.h"
//#include "test_log/test_log2.h"
//#include "test_timerqueue.h"
//#include "test_func_dispatcher.h"


int main()
{
	test_signal1();

	//test_sigslot0();
	//test_sigslot1();
	//test_sigslot2();
	//test_sigslot3();
	//test_sigslot4();

	//test_log();
	//test_log2();

	//for (int i = 0; i<1000000; ++i)
	//{
	//	test_timerqueue_start();
	//	getchar();
	//	test_timerqueue_stop();
	//	getchar();
	//}

	//test_func_dispatcher();


	getchar();
	return 0;
}