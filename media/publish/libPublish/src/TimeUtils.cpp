#include <stdafx.h>
#include "TimeUtils.h"

#ifndef DELTA_EPOCH_IN_USEC
#define DELTA_EPOCH_IN_USEC 11644473600000000Ui64 
#endif

const int g_rgnDaysOfMonth[] = {0, 31, 28, 31,
30, 31, 30,
31, 31, 30,
31, 30, 31
};
const int g_drgnDaysOfMonth[] = {0, 31, 29, 31,
30, 31, 30,
31, 31, 30,
31, 30, 31
};

const int g_rgnAccDaysOfMonth[] = {0, 31, 59, 90,
120, 151, 181,
212, 243, 273,
304, 334, 365
};
const int g_drgnAccDaysOfMonth[] = {0, 31, 60, 91,
121, 152, 182,
213, 244, 274,
305, 335, 366
};

//判断闰年
BOOL TIME_GudgeYear(int year)
{
	return( (year%4==0 && year%100!=0 ) || year%400==0 );
}

//计算now日期nDay前的日期
void TIME_CalculateDate(ScSystemTime now, int nDay, ScSystemTime &before)
{
	before.wYear = now.wYear;
	before.wMonth = now.wMonth;
	before.wDay = now.wDay;

	int nTempDay = before.wDay;
	nTempDay -= nDay;
	while( nTempDay <= 0)
	{
		if(before.wMonth==1)
		{
			before.wMonth = 12;
			before.wYear --;
		}
		else
		{
			before.wMonth --;
		}
		nTempDay += g_rgnDaysOfMonth[before.wMonth];
		if(before.wMonth == 2)
		{
			nTempDay += TIME_GudgeYear(before.wYear);
		}		
	}
	before.wDay = nTempDay;
}

//计算now日期nSecond秒之前的日期
//====================================================================
void TIME_CalculateDateBySecond(ScSystemTime now, int nSecond, ScSystemTime &before)
{

	int nDay = nSecond/(60 * 60 * 24);
	int nTempSecond = nSecond %(60 * 60 * 24);
	int nNowSecond = now.wHour * 60 * 60 + now.wMinute * 60 + now.wSecond;
	if(nTempSecond > nNowSecond)
	{
		nDay += 1;
		nNowSecond += (60 * 60 * 24);
	}
	TIME_CalculateDate(now,nDay,before);
	int nTemp = nNowSecond - nTempSecond;
	before.wHour = nTemp / 3600;
	before.wMinute = (nTemp %3600)/60 ;
	before.wSecond = nTemp %60;
	before.wMilliseconds = now.wMilliseconds;

}

//最多隔一天
//return milli-seconds t1 - t2
int TIME_GetSystemTimeDiff(ScSystemTime t1, ScSystemTime t2)
{
	int nDaysOfYearDiff = 0;
	if(t1.wYear < t2.wYear)
	{
		for(int i = t1.wYear; i < t2.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff -= 366;
			}
			else
			{
				nDaysOfYearDiff -= 365;
			}
		}
	}
	else if(t1.wYear > t2.wYear)
	{
		for(int i = t2.wYear; i < t1.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff += 366;
			}
			else
			{
				nDaysOfYearDiff += 365;
			}
		}
	}

	int nDaysOfMonthDiff = g_rgnAccDaysOfMonth[t1.wMonth-1];
	if(TIME_GudgeYear(t1.wYear) && (t1.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff ++;
	}

	nDaysOfMonthDiff -= g_rgnAccDaysOfMonth[t2.wMonth-1];
	if(TIME_GudgeYear(t2.wYear) && (t2.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff --;
	}


	int dday = t1.wDay - t2.wDay + nDaysOfYearDiff + nDaysOfMonthDiff;
	int dhour = t1.wHour-t2.wHour;
	int dminute = t1.wMinute - t2.wMinute;
	int dsecond = t1.wSecond - t2.wSecond;
	int dmillisecond = t1.wMilliseconds - t2.wMilliseconds;

	return abs(dday*24*3600*1000 + dhour*3600*1000
		+ dminute*60*1000 + dsecond*1000+dmillisecond);
}


BOOL TIME_TimeEarlier(const ScSystemTime time1, const ScSystemTime time2)
{
	return ( (time1.wYear < time2.wYear
		) ||
		(time1.wYear == time2.wYear && time1.wMonth < time2.wMonth
		) ||
		(time1.wYear == time2.wYear && time1.wMonth == time2.wMonth && time1.wDay < time2.wDay
		) ||
		(time1.wYear == time2.wYear && time1.wMonth == time2.wMonth && time1.wDay == time2.wDay && 
		time1.wHour < time2.wHour
		) ||
		(time1.wYear == time2.wYear && time1.wMonth == time2.wMonth && time1.wDay == time2.wDay &&
		time1.wHour == time2.wHour && time1.wMinute < time2.wMinute 
		) ||
		( time1.wYear == time2.wYear && time1.wMonth == time2.wMonth && time1.wDay == time2.wDay && 
		time1.wHour == time2.wHour && time1.wMinute == time2.wMinute && time1.wSecond < time2.wSecond
		) ||
		( time1.wYear == time2.wYear && time1.wMonth == time2.wMonth && time1.wDay == time2.wDay && 
		time1.wHour == time2.wHour && time1.wMinute == time2.wMinute && time1.wSecond == time2.wSecond && 
		time1.wMilliseconds <= time2.wMilliseconds
		)  );
}	


//最多隔一天
//return DAY t1 - t2
int TIME_GetSystemTimeDiffDay(ScSystemTime t1, ScSystemTime t2)
{
	int nDaysOfYearDiff = 0;
	if(t1.wYear < t2.wYear)
	{
		for(int i = t1.wYear; i < t2.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff -= 366;
			}
			else
			{
				nDaysOfYearDiff -= 365;
			}
		}
	}
	else if(t1.wYear > t2.wYear)
	{
		for(int i = t2.wYear; i < t1.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff += 366;
			}
			else
			{
				nDaysOfYearDiff += 365;
			}
		}
	}

	int nDaysOfMonthDiff = g_rgnAccDaysOfMonth[t1.wMonth-1];
	if(TIME_GudgeYear(t1.wYear) && (t1.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff ++;
	}

	nDaysOfMonthDiff -= g_rgnAccDaysOfMonth[t2.wMonth-1];
	if(TIME_GudgeYear(t2.wYear) && (t2.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff --;
	}

	int dday = t1.wDay - t2.wDay + nDaysOfYearDiff + nDaysOfMonthDiff;
	return dday;
}

//返回两时间只差 ，秒级
int  TIME_GetSystemTimeDiffSecond(ScSystemTime t1, ScSystemTime t2)
{
	int nDaysOfYearDiff = 0;
	if(t1.wYear < t2.wYear)
	{
		for(int i = t1.wYear; i < t2.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff -= 366;
			}
			else
			{
				nDaysOfYearDiff -= 365;
			}
		}
	}
	else if(t1.wYear > t2.wYear)
	{
		for(int i = t2.wYear; i < t1.wYear; i++)
		{
			if(TIME_GudgeYear(i))
			{
				nDaysOfYearDiff += 366;
			}
			else
			{
				nDaysOfYearDiff += 365;
			}
		}
	}

	int nDaysOfMonthDiff = g_rgnAccDaysOfMonth[t1.wMonth-1];
	if(TIME_GudgeYear(t1.wYear) && (t1.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff ++;
	}

	nDaysOfMonthDiff -= g_rgnAccDaysOfMonth[t2.wMonth-1];
	if(TIME_GudgeYear(t2.wYear) && (t2.wMonth-1) >= 2)
	{
		nDaysOfMonthDiff --;
	}

	int dday = t1.wDay - t2.wDay + nDaysOfYearDiff + nDaysOfMonthDiff;
	int dhour = t1.wHour-t2.wHour;
	int dminute = t1.wMinute - t2.wMinute;
	int dsecond = t1.wSecond - t2.wSecond;
	return abs(dday*24*3600 + dhour*3600+ dminute*60 + dsecond);
}
void TIME_GetLocalTime(ScSystemTime* t)
{
	GetLocalTime(t);
}

void TIME_ScSystemTime2timeval(ScSystemTime scst,struct timeval *val)
{
	SYSTEMTIME st = (SYSTEMTIME)scst;

	FILETIME ft; 
	LARGE_INTEGER li; 
	__int64 t; 
	static int tzflag; 
	 static const unsigned __int64 epoch = 116444736000000000LL;

	timeval tv;
	{
		SystemTimeToFileTime(IN &st, OUT &ft);

		li.LowPart = ft.dwLowDateTime; 
		li.HighPart = ft.dwHighDateTime; 

		tv.tv_sec = (long) ((li.QuadPart - epoch) / 10000000L);
		tv.tv_usec = (long) (st.wMilliseconds*1000);
		*val = tv;

		/*t = li.QuadPart; /* In 100-nanosecond intervals */ 
		//t -= DELTA_EPOCH_IN_USEC; /* Offset to the Epoch time */ 
		//t /= 10; /* In microseconds */ 
		//tv.tv_sec = (long)(t / 1000000); 
		//tv.tv_usec = (long)(t % 1000000); */
	}
	//return tv;
}

void TIME_timeval2ScSystemTime(struct timeval val,ScSystemTime* scst)
{
	ScSystemTime sysTime;
	FILETIME fl;
	LARGE_INTEGER li; 
	 static const unsigned __int64 epoch = 116444736000000000LL;
	 unsigned __int64 nTemp= val.tv_sec ;
	 unsigned __int64 nSec = nTemp * 10000000L;
	li.QuadPart = (LONGLONG)(nSec + epoch + val.tv_usec*10) ;
	fl.dwHighDateTime = li.HighPart ;
	fl.dwLowDateTime= li.LowPart ;
	FileTimeToSystemTime(&fl,scst);
	//scst->wMilliseconds = val.tv_usec / 1000;
	//SystemTimeToTzSpecificLocalTime(NULL, &sysTime, scst);
}

