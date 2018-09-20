#include <windows.h>
#include "sc_CSLock.h"


CMutexLock::CMutexLock(void)
{
	m_pLock = NULL;
	m_pLock = new CRITICAL_SECTION();
	InitializeCriticalSection((CRITICAL_SECTION*)m_pLock);
}

CMutexLock::~CMutexLock(void)
{
	if(m_pLock)
	{
		DeleteCriticalSection((CRITICAL_SECTION*)m_pLock);
		delete m_pLock;
		m_pLock = NULL;
	}
}

void CMutexLock::Lock()
{
	if(m_pLock)
	{
		EnterCriticalSection((CRITICAL_SECTION*)m_pLock);
	}
}

void CMutexLock::Unlock()
{
	if(m_pLock)
	{
		LeaveCriticalSection((CRITICAL_SECTION*)m_pLock);
	}
}

CAutoLock::CAutoLock(CMutexLock& lock)
{
    lock.Lock();
    m_pLock = &lock;
}

CAutoLock::CAutoLock()
{
    m_pLock = NULL;
}
CAutoLock::~CAutoLock()
{
    m_pLock->Unlock();
}