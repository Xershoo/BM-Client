#pragma once
#include "xctrl.h"
#include "xgdi/xgdidef.h"
#include "transwindow.h"
#include "cssparser.h"

NAMESPACE_BEGIN(xctrl)

class CStatic
    : public CWindowImpl<CStatic, CWindow, CControlWinTraits>
    , public TransChildWindow<CStatic>
    , public XCssParser
{
public:
    CStatic()
    {
        m_pFont             = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));
        m_crFont            = (COLORREF)xskin::QueryUIValue(SKINID(TextColor));
        m_crFontDisabled    = (COLORREF)xskin::QueryUIValue(SKINID(TextColorDisabled));
		m_crFontMouseOn		= m_crFont; 
        m_pResLoader        = xskin::CResLoader::inst();
		m_pImageSet			= NULL;
		m_bHighlighted		= FALSE;
		m_bGlowText			= false;
		m_nGlowColor		= 0;
		m_nTextLeftInter	= 0;
    }

    ~CStatic()
    {
    }

	virtual xgdi::IFont* GetTextFont()
	{
		return m_pFont;
	}

    virtual BOOL ParseCss(IProperty* pProp)
    {
        if(pProp == NULL) return FALSE;

		XStyleParser stylePraser(pProp->GetProperty(_T("style")));
		stylePraser.AddStyle(_T("SS_SIMPLE"), SS_SIMPLE);
		stylePraser.AddStyle(_T("SS_CENTERIMAGE"),   SS_CENTERIMAGE);
		stylePraser.AddStyle(_T("SS_RIGHT"),   SS_RIGHT);
		stylePraser.AddStyle(_T("SS_CENTER"),   SS_CENTER);
		stylePraser.AddStyle(_T("BS_PUSHLIKE"),   BS_PUSHLIKE);
		m_dwStyle = stylePraser.dwStyle;
		if(IsWindow()){
			ModifyStyle(0,m_dwStyle,0);
		}


        LPCTSTR proper;
        proper = pProp->GetProperty(_T("font"));
        if(proper)
        {
            m_pFont = m_pResLoader->LoadFont(proper);
        }

        proper = pProp->GetProperty(_T("font-color"));
        if(proper)
        {
            CTextUtil::TextToColor(proper, m_crFont);
			m_crFontMouseOn = m_crFont;
        }

		proper = pProp->GetProperty(_T("left-inter"));
		if(proper)
		{
			CTextUtil::TextToInt(proper, m_nTextLeftInter);			
		}

		proper = pProp->GetProperty(_T("font-color_mouseon"));
        if(proper)
        {
            CTextUtil::TextToColor(proper, m_crFontMouseOn);
        }

		

        proper = pProp->GetProperty(_T("font-color-disabled"));
        if(proper)
        {
            CTextUtil::TextToColor(proper, m_crFontDisabled);
        }

		if(proper = pProp->GetProperty(_T("image")))
		{
			m_pImageSet = m_pResLoader->LoadIconSet(proper);

		}

		if(proper = pProp->GetProperty(_T("image-inner")))
		{
			CTextUtil::TextToRect(proper, m_rcBkInner);
		}

		proper = pProp->GetProperty(_T("glowtext"));
		if(proper)
		{
			m_bGlowText = true;
		}
		proper = pProp->GetProperty(_T("glowcolor"));
		if(proper)
		{
			CTextUtil::TextToGdiPlusColor(proper, m_nGlowColor);
		}

        return XCssParser::ParseCss(pProp);
    }

public:
    DECLARE_WND_CLASS(_T("wtlStatic"));

    BEGIN_MSG_MAP(CStatic)
        MESSAGE_HANDLER(WM_CREATE,      OnCreate)
        MESSAGE_HANDLER(WM_PAINT,       OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND,  OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SETTEXT,     OnSetText)
        MESSAGE_HANDLER(WM_MOUSEMOVE,   OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONUP,   OnLButtonUp)
		MESSAGE_HANDLER(WM_KILLFOCUS,	OnKillFocus)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		ModifyStyle(0, m_dwStyle, 0);
        return 0;
    }

	void GetTextRect(CRect & rc)
	{
		ATL::CString strWindowText;
		GetWindowText(strWindowText);
		CPaintDC dc(m_hWnd);
		CClientRect rcTemp(m_hWnd);
		xgdi::ICanvas* pCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
		pCanvas->SetSize(rcTemp.Size());
		pCanvas->Attach(dc.m_hDC);
		pCanvas->DrawText(m_pFont, strWindowText, &rc, DT_CALCRECT);
		pCanvas->Detach();
		pCanvas->Release();
	}

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CUpdateRect rcUpdate(m_hWnd);
        CPaintDC dc(m_hWnd);
        CClientRect rcClient(m_hWnd);

        xgdi::ICanvas* pCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
        pCanvas->SetSize(rcClient.Size());
        pCanvas->Attach(dc.m_hDC);

        ParentDrawBkgrd(pCanvas, rcUpdate);

        ::SendMessage(::GetParent(m_hWnd), WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd);

        ATL::CString strWindowText;
        GetWindowText(strWindowText);
        pCanvas->SetBkMode(TRANSPARENT);

		if(m_pImageSet)
		{
			int nIndex = 0;
			if(m_bHighlighted)
				nIndex = 1;
			if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
			{
				pCanvas->DrawIcon(pIcon, rcClient, m_rcBkInner, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}
		}

        pCanvas->SetTextColor(IsWindowEnabled() ? m_crFont : m_crFontDisabled);
		if(m_bHighlighted)
			pCanvas->SetTextColor(m_crFontMouseOn);

        int nDIFlag = 0;
        DWORD dwStyle = GetStyle();
        switch(dwStyle & 0x000F)
        {
        case SS_LEFT: nDIFlag = DT_LEFT; break;
        case SS_RIGHT: nDIFlag = DT_RIGHT; break;
        case SS_CENTER: nDIFlag = DT_CENTER; break;
        case SS_SIMPLE: nDIFlag = DT_LEFT | DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_END_ELLIPSIS /*| DT_PATH_ELLIPSIS*/; break;
        default:
            break;
        }
        nDIFlag |= 0
            | ((dwStyle & SS_RIGHTJUST) ? DT_RIGHT : 0)
            | ((dwStyle & SS_CENTERIMAGE) ? DT_VCENTER : 0)
			| DT_NOPREFIX
            ;

		rcClient.left += m_nTextLeftInter;
		if(!m_bGlowText)
			pCanvas->DrawText(m_pFont, strWindowText, &rcClient, nDIFlag);
		else
			pCanvas->DrawTextGlow(m_pFont, strWindowText, &rcClient, nDIFlag, m_nGlowColor);

        //m_pStatic->Paint(pCanvas, rcClient, rcUpdate);
		rcClient.left = 0;
        pCanvas->Present(dc.m_hDC, rcClient, 0);
        pCanvas->Detach();
        pCanvas->Release();

        return 0;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT nRet = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

        Invalidate(FALSE);
        return 0;
    }

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bHighlighted = FALSE;
		Invalidate(TRUE);
		return 0;
	}

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(GetStyle() & BS_PUSHLIKE)
        {
            CClientRect rcClient(m_hWnd);
            CPoint pt = lParam;
            if(rcClient.PtInRect(pt))
            {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
                if(!m_bHighlighted)
                {
                    m_bHighlighted = TRUE;
                    Invalidate(FALSE);
                }
                ::SetCapture(m_hWnd);

            }
            else
            {
                if(m_bHighlighted)
                {
                    m_bHighlighted = FALSE;
                    Invalidate(FALSE);
                }
                ::ReleaseCapture();
            }
        }
        else
        {
            bHandled = FALSE;
        }

        return 0;
    }

    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(GetStyle() & BS_PUSHLIKE)
        {
            int id = ::GetDlgCtrlID(m_hWnd);
			m_bHighlighted = FALSE;
            ::PostMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELPARAM(id, BN_CLICKED), (LPARAM)m_hWnd);
        }
        else
        {
            bHandled = FALSE;
        }

        return 0;
    }

	void SetColor(COLORREF color)
	{
		m_crFont = color;
	}

protected:
    xgdi::IFont*        m_pFont;

    COLORREF            m_crFont;
	COLORREF			m_crFontMouseOn;
    COLORREF            m_crFontDisabled;

    BOOL                m_bHighlighted;
	bool				m_bGlowText;
	DWORD				m_nGlowColor;
	int					m_nTextLeftInter;
	CRect                   m_rcBkInner;
	xgdi::IIconSet*         m_pImageSet;

};

NAMESPACE_END(xctrl)