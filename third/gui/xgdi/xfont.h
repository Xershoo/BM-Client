#pragma once
#include "xgdi.h"

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CFontT :
	implement IFont
{
public:
	CFontT(HFONT hFont = NULL)
	{
		m_hFont = hFont;
	}

	~CFontT()
	{
		if(t_bManaged && m_hFont)
		{
			DeleteObject();
		}
	}

	CFontT<t_bManaged> operator =(HFONT hFont)
	{
		Attach(hFont);
		return *this;
	}

	operator HFONT() const
	{
		return m_hFont;
	}

	BOOL Attach(const UI_HANDLE& hFont)
	{
		if(t_bManaged && m_hFont != NULL && m_hFont != hFont)
			::DeleteObject(m_hFont);
		m_hFont = hFont;

        return TRUE;
	}

    IFont* CreateSimpleFont(LPCTSTR lpszFaceName, const SIZE& sizeFont, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = FALSE, LPCTSTR lpszFile = NULL)
    {
        assert(lpszFaceName);

        if(lpszFile) ::AddFontResource(lpszFile);

        m_sizeFont = sizeFont;

        m_hFont = ::CreateFont(
            m_sizeFont.cy, m_sizeFont.cx,
            0, 0, (bBold ? FW_BOLD : FW_NORMAL),
            bItalic, bUnderline, FALSE,
            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, VARIABLE_PITCH, lpszFaceName
            );

        return (m_hFont ? this : NULL);
    }

	BOOL DeleteObject()
	{
		assert(m_hFont != NULL);
		BOOL bRet = ::DeleteObject((HGDIOBJ)m_hFont);
		if(bRet) m_hFont = NULL;
		return bRet;
	}

	UI_HANDLE Detach()
	{
		HFONT hFont = m_hFont;
		m_hFont = NULL;
		return hFont;
	}

    UI_HANDLE GetUIHandle() const
    {
        return m_hFont;
    }

	bool IsNull() const
	{
		return (m_hFont == NULL);
	}

	virtual void Release()
	{
		delete this;
	}

protected:
	HFONT           m_hFont;

    CSize           m_sizeFont;
};

typedef CFontT<true>	CFont;
typedef CFontT<false>	CFontHandle;

NAMESPACE_END(xgdi)
