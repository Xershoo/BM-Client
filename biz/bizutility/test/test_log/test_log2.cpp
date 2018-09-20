#include "test_log2.h"
#include "log_test2.h"
#include "../../log/assert_log.h"

void test_log2()
{
	LOG.SetID(12345);
	LOG << "test output for log2";

	*assert_(0) << "avf" << 23;
	//assert_(false);

	LOG("xxx[%S]xxx", L"abc");
}