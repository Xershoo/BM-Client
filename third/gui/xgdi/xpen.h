#pragma once
#include "xgdi.h"

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CPenT
	: implement IPen
{
public:
	CPenT(HPEN hPen = NULL)
	{
		m_hPen = hPen;
	}

	CPenT(int nStyle, int nWidth, int nColor)
	{
		if(t_bManaged)
		{
			m_hPen = ::CreatePen(nStyle, nWidth, nColor);
		}
		else
		{
			assert(true);
		}
	}

	~CPenT()
	{
		if(t_bManaged && m_hPen)
		{
			DeleteObject();
		}
	}

	CPenT<t_bManaged>& operator =(HPEN hPen)
	{
		Attach(hPen);
		return *this;
	}

	operator HPEN() const
	{
		return m_hPen;
	}

	BOOL Attach(const UI_HANDLE& hPen)
	{
		if(t_bManaged && m_hPen != NULL && m_hPen != hPen)
			::DeleteObject(m_hPen);
		m_hPen = hPen;

        return TRUE;
	}

	BOOL CreatePen(int nStyle, int nWidth, int nColor)
	{
		assert(m_hPen == NULL);
		m_hPen = ::CreatePen(nStyle, nWidth, nColor);
		return (m_hPen ? TRUE: FALSE);
	}

	BOOL DeleteObject()
	{
		assert(m_hPen != NULL);
		BOOL bRet = ::DeleteObject((HGDIOBJ)m_hPen);
		if(bRet) m_hPen = NULL;
		return bRet;
	}

	UI_HANDLE Detach()
	{
		HPEN hPen = m_hPen;
		m_hPen = NULL;
		return hPen;
	}

    UI_HANDLE GetUIHandle() const
    {
        return m_hPen;
    }

	bool IsNull() const
	{
		return (m_hPen == NULL);
	}

	virtual void Release()
	{
		delete this;
	}

public:
	HPEN m_hPen;
};

typedef CPenT<true>		CPen;
typedef CPenT<false>	CPenHandle;

NAMESPACE_END(xgdi)