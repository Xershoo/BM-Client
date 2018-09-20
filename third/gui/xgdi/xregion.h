#pragma once
#include "xgdi.h"

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CRegionT
	: implement IRegion
{
public:
	CRegionT(int x1, int y1, int x2, int y2, int round = 0)
	{
		m_hRgn = NULL;
		if(round == 0)
        {
            CreateRectRgn(x1, y1, x2, y2);
        }
        else
        {
            CreateRoundRectRgn(x1, y1, x2, y2, round, round);
        }
	}

    CRegionT(const RECT& rcRect, int round = 0)
    {
        m_hRgn = NULL;
        if(round == 0)
        {
            CreateRectRgn(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);
        }
        else
        {
            CreateRoundRectRgn(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, round, round);
        }
    }

	CRegionT(HRGN hRgn = NULL)
	{
		m_hRgn = hRgn;
	}

	~CRegionT()
	{
		if(t_bManaged && m_hRgn)
		{
			DeleteObject();
		}
	}

	CRegionT<t_bManaged> operator =(HRGN hRgn)
	{
		Attach(hRgn);
		return *this;
	}

	operator HRGN() const
	{
		return m_hRgn;
	}

	BOOL Attach(const UI_HANDLE& hRgn)
	{
		if(t_bManaged && m_hRgn != NULL && m_hRgn != hRgn)
			::DeleteObject(m_hRgn);
		m_hRgn = hRgn;

        return (m_hRgn == NULL) ? FALSE : TRUE;
	}

	IRegion* CreateRectRgn(const RECT& rcRect)
	{
		assert(m_hRgn == NULL);
		m_hRgn = ::CreateRectRgn(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom);
		return (m_hRgn ? this : NULL);
	}

    IRegion* CreateRoundRectRgn(const RECT& rcRect, int width, int height)
    {
        assert(m_hRgn == NULL);
        m_hRgn = ::CreateRoundRectRgn(rcRect.left, rcRect.top, rcRect.right, rcRect.bottom, width, height);
        return (m_hRgn ? this : NULL);
    }

    IRegion* CreateRectRgn(int x1, int y1, int x2, int y2)
    {
        assert(m_hRgn == NULL);
        m_hRgn = ::CreateRectRgn(x1, y1, x2, y2);
        return (m_hRgn ? this : NULL);
    }

	IRegion* CreateRoundRectRgn(int x1, int y1, int x2, int y2, int width, int height)
	{
		assert(m_hRgn == NULL);
		m_hRgn = ::CreateRoundRectRgn(x1, y1, x2 + 1, y2 + 1, width, height);
		return (m_hRgn ? this : NULL);
	}

	BOOL DeleteObject()
	{
		assert(m_hRgn != NULL);
		BOOL bRet = ::DeleteObject((HGDIOBJ)m_hRgn);
		if(bRet) m_hRgn = NULL;
		return bRet;
	}

	UI_HANDLE Detach()
	{
		HRGN hRgn = m_hRgn;
		m_hRgn = NULL;
		return hRgn;
	}

	bool IsNull() const
	{
		return (m_hRgn == NULL);
	}

    UI_HANDLE GetUIHandle() const
    {
        return m_hRgn;
    }

	virtual void Release()
	{
		delete this;
	}

protected:
	HRGN m_hRgn;
};

typedef CRegionT<true>	CRegion;
typedef CRegionT<false>	CRegionHandle;

NAMESPACE_END(xgdi)