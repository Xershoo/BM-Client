#pragma once
//如果是debug则断言，如果是release则记录到文件

#include <string>
#include <assert.h>
#include <windows.h>	//MultiByteToWideChar


#define assert_(exp)	biz_utility::assert0(!!(exp), #exp, __FILE__, __LINE__)


namespace biz_utility
{
	class LogFile;
	LogFile* assert0(bool b, std::string exp, std::string file, unsigned lineno);
	// 之所以不用LogFile&是因为如果别的cpp只包含asser_log.h
	// 使用assert0会导致找不到LogFile的错误.
}