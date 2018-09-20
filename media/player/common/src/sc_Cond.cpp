#include <Windows.h>
#include <assert.h>
#include "sc_Cond.h"


CSCCond::CSCCond()
{
	m_waittime = -1;
	m_cond = (HANDLE)CreateEvent (NULL, TRUE, FALSE, NULL);
	assert(m_cond);
}

CSCCond::~CSCCond()
{
	if(m_cond)
	{
		CloseHandle ((HANDLE)m_cond);
		m_cond=NULL;
	}
}

void CSCCond::cond_wait()
{
	if(m_cond)
	{
		if(m_waittime >0)
			WaitForSingleObject ((HANDLE)m_cond, m_waittime);
		else
			WaitForSingleObject ((HANDLE)m_cond, INFINITE);
		
		if(!ResetEvent ((HANDLE)m_cond))
		{
			ResetEvent ((HANDLE)m_cond);
		}
	}
}

void CSCCond::cond_signal()
{
	if(m_cond)
		SetEvent ((HANDLE)m_cond);
}

void CSCCond::conde_setwaittime(int waitTime)
{
	m_waittime = waitTime;
}