
#include <windows.h>
#include "sc_Thread.h"
#include <process.h>

CSCThread::CSCThread(void)
	: m_Callback(NULL)
	, m_pUserData(NULL)
	, m_hThread(NULL)
	, m_bStopThread(TRUE)
{
}

CSCThread::~CSCThread(void)
{
	End();
}

unsigned int WINAPI ProcSCThread(
	PVOID lpParameter
	)
{
	CSCThread *pThread = (CSCThread*)lpParameter;
	pThread->ThreadProcProxy();
	_endthreadex(0);
	return 0;
}

bool CSCThread::Begin(
	ThreadProc time_proc,
	PVOID pUserData
	)
{
	if(time_proc == NULL)
		return false;

	m_Callback = time_proc;
	m_pUserData = pUserData;
	m_bStopThread = FALSE;

	m_hThread = (HANDLE_THREAD)_beginthreadex(
		NULL, 
		0, 
		ProcSCThread, 
		this, 
		0, 
		NULL
		);
	if(m_hThread != NULL)
	{
		SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
	}
	
	if(m_hThread == NULL)
		return false;

	return true;
}

bool CSCThread::End(long nWaitSec)
{
	if(m_hThread)
	{
		m_bStopThread = TRUE;
		DWORD dwRet = WaitForSingleObject(m_hThread,nWaitSec * 1000);
		if(dwRet != WAIT_OBJECT_0)
		{
			TerminateThread(m_hThread,0);
		}

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	return true;
}

void CSCThread::ThreadProcProxy()
{
	m_Callback(m_pUserData);
}

bool CSCThread::GetStop()
{
	return m_bStopThread;
}