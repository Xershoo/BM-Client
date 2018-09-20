#define LOG_NAME _assert_log_
#define LOG_FILE_NAME "assert_log.txt"
#include "log.h"
#include "log.hpp"
#include "assert_log.h"


namespace biz_utility
{
	LogFile* assert0(bool b, std::string exp, std::string file, unsigned lineno)
	{
		static LogFile null_log_file(NULL, 0);
#ifdef _DEBUG
#define MAX_LOG_ONETIME 1024
		wchar_t pwszExp[MAX_LOG_ONETIME];
		int size = MultiByteToWideChar(CP_ACP, 0, exp.c_str(), exp.size(), pwszExp, MAX_LOG_ONETIME);
		pwszExp[size] = 0;

		wchar_t pwszFile[MAX_LOG_ONETIME];
		size = MultiByteToWideChar(CP_ACP, 0, file.c_str(), file.size(), pwszFile, MAX_LOG_ONETIME);
		pwszFile[size] = 0;

		;
		(void)( b || (LOG << "\n", _wassert(pwszExp, pwszFile, lineno), 0) );
		return &null_log_file;
#undef MAX_LOG_ONETIME
	}
#else
		return b ? &null_log_file :
			&(LOG << exp.c_str() << ", file: " << file.c_str() << ", lineno: " << lineno << ". -> ");
	}
#endif
};