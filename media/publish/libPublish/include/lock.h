#pragma once
#include <Windows.h>

class CLock
{
private:
	CRITICAL_SECTION	m_csLock;
	HANDLE				m_hEvent;

public:
	CLock()
	{
		InitializeCriticalSection(&m_csLock); 
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_csLock); 
		CloseHandle(m_hEvent);
	}

	class CAutoLock
	{
	public:
		CAutoLock(CLock & lock):m_lock(lock){m_lock.Locked();};
		~CAutoLock(){m_lock.UnLock();};

	private:
		CLock & m_lock;
	};

public:
	void Wait(DWORD t = INFINITE){WaitForSingleObject(m_hEvent, t);}
	void Notify(){SetEvent(m_hEvent);}
	void Locked(){EnterCriticalSection(&m_csLock);}
	void UnLock(){LeaveCriticalSection(&m_csLock);}
	BOOL TryLock(){return TryEnterCriticalSection(&m_csLock);}
};
