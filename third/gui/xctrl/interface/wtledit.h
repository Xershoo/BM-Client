#pragma once
#include "cssparser.h"
#include "xctrl.h"
#include "skinloader.h"
#include "xgdi/xgdidef.h"
#include "transwindow.h"

NAMESPACE_BEGIN(xctrl)

class CEdit
    : public CWindowImpl<CEdit, WTL::CEdit>
    , public TransChildWindow<CEdit>
	, public XCssParser
{
public:
    BEGIN_MSG_MAP(CEdit)
		MESSAGE_HANDLER(OCM_CTLCOLOREDIT, OnCtrlColor)
		MESSAGE_HANDLER(WM_PASTE,	OnPaste)
    END_MSG_MAP()

	CEdit()
	{
		m_clrText = RGB(0,0,0);
		m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));
		m_pResLoader        = xskin::CResLoader::inst();
		m_pBgBrush = NULL;
	}

	virtual ~CEdit()
	{
		if(m_pBgBrush)
		{
			::DeleteObject(m_pBgBrush);
			m_pBgBrush = NULL;
		}
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		if(LPCTSTR lpszStyle = pProp->GetProperty(_T("style")))
        {
            XStyleParser stylePraser(lpszStyle);
    		stylePraser.AddStyle(_T("ES_NUMBER"),	 ES_NUMBER);
	    	stylePraser.AddStyle(_T("ES_PASSWORD"),  ES_PASSWORD);
		    stylePraser.AddStyle(_T("ES_MULTILINE"), ES_MULTILINE);
		    stylePraser.AddStyle(_T("ES_READONLY"),  ES_READONLY);

		    m_dwStyle = stylePraser.dwStyle;
		    if(IsWindow()){
			    ModifyStyle(0,m_dwStyle,0);
		    }
        }
		LPCTSTR lpclrText = pProp->GetProperty(_T("font-color"));
		if(lpclrText)
		{
			CTextUtil::TextToColor(lpclrText ,m_clrText);			
		}

		lpclrText = pProp->GetProperty(_T("backgroup-color"));
		if(lpclrText)
		{
			CTextUtil::TextToColor(lpclrText ,m_clBgColor);
			m_pBgBrush = ::CreateSolidBrush(m_clBgColor);
		}

		LPCTSTR lpFontText =pProp->GetProperty(_T("font"));
		if (lpFontText)
		{
			m_pFont = m_pResLoader->LoadFont(lpFontText);
			
			SetFont(m_pFont->GetUIHandle());
		}
		return XCssParser::ParseCss(pProp);
	}

	void SetTextColor(COLORREF clrText)
	{
		m_clrText = clrText;
	}

	COLORREF GetTextColor()
	{
		return m_clrText;
	}
	
	LRESULT OnCtrlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
//		
		::SetTextColor((HDC)wParam,m_clrText);				
		return 0;
	}

	LRESULT GetBrush()
	{
		return (LRESULT)m_pBgBrush;
	}


	LRESULT OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwStyle = GetWindowLong(GWL_STYLE);
		if(dwStyle & ES_PASSWORD)
			return 0;
		if ( (dwStyle & ES_NUMBER) == 0 )
		{
			bHandled = FALSE;
			return 0;
		}

		if ( !::IsClipboardFormatAvailable( CF_UNICODETEXT ) )
		{
			return 0;
		}

		if ( !::OpenClipboard( m_hWnd ) )
		{
			return 0;
		}

		BOOL bIsNum = TRUE;
		ATL::CString strTmp;
		HGLOBAL hMem = ::GetClipboardData( CF_UNICODETEXT );

		if ( NULL != hMem )
		{
			strTmp = (LPTSTR)GlobalLock( hMem );
			INT nLength = strTmp.GetLength();
			for ( INT nIndex = 0; nIndex < nLength; ++nIndex )
			{
				UINT nCharTmp = strTmp.GetAt( nIndex );
				if ( nCharTmp < '0' || nCharTmp > '9' )
				{
					strTmp = strTmp.Left(nIndex);
					bIsNum = FALSE;
					break;
				}
			}

			GlobalUnlock( hMem );
		}

		CloseClipboard();

		if ( !bIsNum )
		{
			ReplaceSel(strTmp);

			// 模拟输入一个字符，系统会弹出提示
			SendMessage(WM_CHAR, 'O', 0);
			return 0;
		}

		bHandled = FALSE;
		return 0;
	}

protected:
	COLORREF				m_clrText;
	COLORREF				m_clBgColor;
	xgdi::IFont*            m_pFont;
	HBRUSH					m_pBgBrush;
};

NAMESPACE_END(xctrl)