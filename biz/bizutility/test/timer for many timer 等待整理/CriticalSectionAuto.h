#pragma once

#include <atlsync.h>

namespace pubfunc
{

class CCriticalSectionAuto  
{
public:
	explicit CCriticalSectionAuto(CCriticalSection* Critical)
		: m_Critical(Critical)
	{
		m_Critical->Enter();
	}

	~CCriticalSectionAuto(void)
	{
		if(m_Critical != 0)
			m_Critical->Leave();
	}

private:
	CCriticalSectionAuto(const CCriticalSectionAuto&);
	CCriticalSectionAuto& operator=(const CCriticalSectionAuto&);

	CCriticalSection* m_Critical;
};

}
