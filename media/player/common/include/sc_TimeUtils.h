#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include "wtypes.h"
#if WIN32 
#define ScSystemTime SYSTEMTIME
#endif
//====================================================================
//判断闰年
//====================================================================
BOOL TIME_GudgeYear(int year);

//====================================================================
//计算now日期nDay前的日期
//====================================================================
void TIME_CalculateDate(ScSystemTime now, int nDay, ScSystemTime &before);

//====================================================================
//计算now日期nSecond秒之前的日期
//====================================================================
void TIME_CalculateDateBySecond(ScSystemTime now, int nSecond, ScSystemTime &before);

//====================================================================
//	milli-seccond of time1 - time2
//====================================================================
int TIME_GetSystemTimeDiff(ScSystemTime t1, ScSystemTime t2);

//====================================================================
//	if time1 is earlier than time2
//====================================================================
BOOL TIME_TimeEarlier(const ScSystemTime time1, const ScSystemTime time2);


int TIME_GetSystemTimeDiffDay(ScSystemTime t1, ScSystemTime t2);

//返回两时间只差 ，秒级
int  TIME_GetSystemTimeDiffSecond(ScSystemTime t1, ScSystemTime t2);

void TIME_GetLocalTime(ScSystemTime* t);

void TIME_ScSystemTime2timeval(ScSystemTime scst,struct timeval* val);

void TIME_timeval2ScSystemTime(struct timeval val,ScSystemTime* scst);

#endif