#pragma once
#include "xgdi.h"

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CBrushT :
	implement IBrush
{
public:
	CBrushT(HBRUSH hBrush = NULL)
	{
		m_hBrush = hBrush;
	}

	CBrushT(int nColorRef)
	{
		if(t_bManaged)
		{
			m_hBrush = ::CreateSolidBrush(nColorRef);
		}
		else
		{
			assert(true);
		}
	}

	~CBrushT()
	{
		if(t_bManaged && m_hBrush)
		{
			DeleteObject();
		}
	}

	CBrushT<t_bManaged>& operator =(HBRUSH hBrush)
	{
		Attach(hBrush);
		return *this;
	}

	operator HBRUSH() const
	{
		return m_hBrush;
	}

    BOOL Attach(const UI_HANDLE& hBrush)
	{
		if(t_bManaged && m_hBrush != NULL && m_hBrush != hBrush)
			::DeleteObject(m_hBrush);
		m_hBrush = hBrush;
        
        return TRUE;
	}

	IBrush* CreatePatternBrush(IImage* pImage)
	{
		assert(m_hBrush == NULL);
		m_hBrush = ::CreatePatternBrush((HBITMAP)(*(CImage*)pImage));
		return (m_hBrush ? this : NULL);
	}

	IBrush* CreateSolidBrush(int nColorRef)
	{
		assert(m_hBrush == NULL);
		m_hBrush = ::CreateSolidBrush(nColorRef);
		return (m_hBrush ? this : NULL);
	}

	BOOL DeleteObject()
	{
		assert(m_hBrush != NULL);
		BOOL bRet = ::DeleteObject((HGDIOBJ)m_hBrush);
		if(bRet) m_hBrush = NULL;
		return bRet;
	}

	UI_HANDLE Detach()
	{
		HBRUSH hBrush = m_hBrush;
		m_hBrush = NULL;
		return hBrush;
	}

    UI_HANDLE GetUIHandle() const
    {
        return m_hBrush;
    }

	bool IsNull() const
	{
		return (m_hBrush == NULL);
	}

	virtual void Release()
	{
		delete this;
	}

protected:
	HBRUSH m_hBrush;
};

typedef CBrushT<true>	CBrush;
typedef CBrushT<false>	CBrushHandle;

NAMESPACE_END(xgdi)
