#include "test_log.h"
#include "log_test.h"
using namespace biz_utility;


void test_log()
{
	LOG << 5 << "abc";
	LOG << "test" << 34.5;
	LOG << LogColor::RedFront() << LogColor::GreenBack()
		<< "rrrrred";
}