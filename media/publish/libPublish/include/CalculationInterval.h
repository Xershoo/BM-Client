#pragma once
#include <Windows.h>
#include <time.h>
#include <stdio.h>

// ±º‰º∆À„ 
class CCalculationInterval
{
public:
	CCalculationInterval()
	{
		QueryPerformanceFrequency(&m_Frequency);
	}
	~CCalculationInterval()
	{
	}
	void Begin()
	{
		QueryPerformanceCounter(&m_liPerfStart);
	}
	float End()
	{
		LARGE_INTEGER liPerfNow={0};
		QueryPerformanceCounter(&liPerfNow); 
		return (float(liPerfNow.QuadPart - m_liPerfStart.QuadPart)/(float)(m_Frequency.QuadPart))*1000;
	}

private:
	clock_t			 		  start_time;
#ifdef _WIN32
	LARGE_INTEGER			  m_Frequency;
	LARGE_INTEGER			  m_liPerfStart;
#endif
};
