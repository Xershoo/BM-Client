#include <time.h>

namespace biz_utility
{
	LogFile& GetLog()
	{
		static LogFile* pLog = NULL;
		if (NULL == pLog)
		{
			pLog = new LogFile(LOG_FILE_NAME, LOG_FILE_SIZE);
#ifdef LOG_HEAD
			*pLog << LOG_HEAD << "\n";
#endif
		}
		else
			*pLog << "\n";

		if (pLog->GetID() != 0)
			*pLog << "[" << pLog->GetID() << "] ";

		time_t t;
		struct tm tmt;
		struct tm* pt = &tmt;
		wchar_t buf[1024] = {0};
		time(&t);
		localtime_s(pt, &t);
		pt->tm_year += 1900;
		pt->tm_mon++;
		swprintf_s(buf, sizeof(buf)/2,
			L"[%04d-%02d-%02d %02d:%02d:%02d]\t",
			pt->tm_year, pt->tm_mon, pt->tm_mday,
			pt->tm_hour, pt->tm_min, pt->tm_sec);

		*pLog << buf;

		return *pLog;
	}
}