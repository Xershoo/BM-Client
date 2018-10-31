#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include <windows.h>

class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION* pCS):m_pCriticalSection(pCS)
	{
		if(m_pCriticalSection)
		{
			::EnterCriticalSection(m_pCriticalSection);
		}
	}

	virtual ~CAutoLock()
	{
		if(m_pCriticalSection)
		{
			::LeaveCriticalSection(m_pCriticalSection);
		}
	}

	virtual void unlock()
	{
		if(m_pCriticalSection)
		{
			::LeaveCriticalSection(m_pCriticalSection);
			m_pCriticalSection = NULL;
		}
	}

protected:
	CRITICAL_SECTION* m_pCriticalSection;
};

#endif