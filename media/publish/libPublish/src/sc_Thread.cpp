
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

BOOL CSCThread::Begin(
	ThreadProc time_proc,
	PVOID pUserData
	)
{
	if(time_proc == NULL)
		return FALSE;

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
		return FALSE;

	return TRUE;
}

BOOL CSCThread::End(long nWaitSec)
{
	if(m_hThread)
	{
		m_bStopThread = TRUE;
		DWORD dwRet = WaitForSingleObject(m_hThread,nWaitSec * 1000);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	return TRUE;
}

void CSCThread::ThreadProcProxy()
{
	m_Callback(m_pUserData);
}

BOOL CSCThread::GetStop()
{
	return m_bStopThread;
}