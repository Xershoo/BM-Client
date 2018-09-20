#include <process.h>
#include "../func_dispatcher/func_dispatcher.h"
using namespace biz_utility;


AnsyFuncDispcher funcdispacher;
unsigned int CALLBACK thread2(void*);


unsigned int CALLBACK thread1(void*)
{
	funcdispacher.Startup();
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, thread2, NULL, 0, NULL);

	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage (&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	funcdispacher.Cleanup();

	return 0;
}


class FuncTest
{
public:
	void f1(int k)
	{
		printf("FuncTest::f1(%d)\n", k);
	}
} ft;


unsigned int CALLBACK thread2(void*)
{
	for (int i=0; i<10; ++i)
	{
		static int cnt = 0;
		funcdispacher.PostTask(&ft, &FuncTest::f1, cnt);
		++cnt;
		Sleep(1000);
	}
	return 0;
}


inline void test_func_dispatcher()
{
	HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, thread1, NULL, 0, NULL);
}