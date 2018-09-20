#pragma once
#include "xgdi/xgdi.h"
#include "xgdi/xgdidef.h"
#include "transwindow.h"
#include "xobject.h"
#include "cssparser.h"

NAMESPACE_BEGIN(xctrl)

class CPicture
    : public CWindowImpl<CPicture, CWindow, CControlWinTraits>
	, public xctrl::TransChildWindow<CPicture>
    , public XCssParser
{
public:
    CPicture(LPCTSTR lpszImageFile = NULL)
        : m_bHighlighted(FALSE),
		  m_bGray(FALSE)
		, m_pPicture(NULL)
    {
		m_pResLoader        = xskin::CResLoader::inst();
        if(lpszImageFile)
        {
            m_pPicture = m_pResLoader->LoadImage(lpszImageFile);
        }

		m_rectDraw = xgdi::rectOriginalImage;
		m_nSystleEx = 0;
		m_nPlayTimes = 0;
    }

    virtual ~CPicture()
    {
    }

	void SetDrawRect(const RECT& rectDraw)
	{
		m_rectDraw = rectDraw;
		Invalidate(FALSE);
	}

    enum TIMER_ID
    {
        TIMER_UPDATE_FRAME       = 0x100,

        TIMER_MAX
    };

    virtual BOOL ParseCss(IProperty* pProp)
    {
        LPCTSTR proper;
        if(proper = pProp->GetProperty(_T("image")))
        {
            SetPicture(proper);
        }

		XStyleParser stylePraser(pProp->GetProperty(_T("style")));
		stylePraser.AddStyle(_T("SS_CENTERIMAGE"), SS_CENTERIMAGE);
		stylePraser.AddStyle(_T("BS_PUSHLIKE"),   BS_PUSHLIKE);
		stylePraser.AddStyle(_T("SS_SIMPLE"),	  SS_SIMPLE);
		m_dwStyle = stylePraser.dwStyle;
		if(IsWindow()){
			ModifyStyle(0,m_dwStyle,0);
		}

		ATL::CString strExStyle(pProp->GetProperty(_T("image_style_ex")));
		if(strExStyle.Find(_T("DIF_ALPHA")) >= 0)
			m_nSystleEx |= xgdi::DIF_ALPHA;
		if(strExStyle.Find(_T("DIF_NORMAL")) >= 0)
			m_nSystleEx |= xgdi::DIF_NORMAL;
		if(strExStyle.Find(_T("DIF_TRANSPARENT")) >= 0)
			m_nSystleEx |= xgdi::DIF_TRANSPARENT;
		if(strExStyle.Find(_T("DIF_STRETCH")) >= 0)
			m_nSystleEx |= xgdi::DIF_STRETCH;
		if(strExStyle.Find(_T("DIF_ALIGNLEFT")) >= 0)
			m_nSystleEx |= xgdi::DIF_ALIGNLEFT;
		if(strExStyle.Find(_T("DIF_ALIGNRIGHT")) >= 0)
			m_nSystleEx |= xgdi::DIF_ALIGNRIGHT;
		if(strExStyle.Find(_T("DIF_ALIGNTOP")) >= 0)
			m_nSystleEx |= xgdi::DIF_ALIGNTOP;
		if(strExStyle.Find(_T("DIF_ALIGNBOTTOM")) >= 0)
			m_nSystleEx |= xgdi::DIF_ALIGNBOTTOM;
		

        return XCssParser::ParseCss(pProp);
    }

	void SetImageGray(bool bGray)
	{
		m_bGray = bGray;
	}

public:
    DECLARE_WND_CLASS_EX(_T("wtlPicture"), CS_PARENTDC, COLOR_WINDOW);

    BEGIN_MSG_MAP(CPicture)
        MESSAGE_HANDLER(WM_CREATE,      OnCreate)
        MESSAGE_HANDLER(WM_PAINT,       OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND,  OnEraseBkgnd)
        MESSAGE_HANDLER(WM_TIMER,       OnTimer)
        MESSAGE_HANDLER(WM_MOUSEMOVE,   OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONUP,   OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ATL::CString strWindowText;
        GetWindowText(strWindowText);

        //m_pPicture = m_pResLoader->LoadImage(strWindowText);

        if(GetStyle() & BS_GROUPBOX)
        {
            EnableWindow(FALSE);
        }

        return 0;
    }

    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		CUpdateRect rcDirty(m_hWnd);
        CPaintDC dc(m_hWnd);
        CClientRect rcClient(m_hWnd);
        xgdi::ICanvas* pCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj((GetStyle() & BS_GROUPBOX) ? xgdi::IID_UICanvasHandle : xgdi::IID_UICanvas);
        pCanvas->SetSize(rcClient.Size());
        pCanvas->Attach(dc.m_hDC);

		ParentDrawBkgrd(pCanvas, rcDirty);

		// 判断m_rectDraw以绘制一部分图像 [3/30/2010 何春龙]
		RECT rectDraw = xgdi::rectOriginalImage;
		if (m_rectDraw != xgdi::rectOriginalImage)
		{
			rectDraw = m_rectDraw;
		}

        if(GetStyle() & SS_CENTERIMAGE)
        {
			if(m_nSystleEx == 0)
				m_nSystleEx = xgdi::DIF_ALPHA | xgdi::DIF_NORMAL | xgdi::DIF_TRANSPARENT;
			if ( m_bGray )
			{
				pCanvas->DrawImage(m_pPicture, rcClient, rectDraw, m_nSystleEx | xgdi::DIF_GRAY);
			}
			else
			{
				pCanvas->DrawImage(m_pPicture, rcClient, rectDraw,m_nSystleEx);
			}
            
        }
        else
        {
			if(m_nSystleEx == 0)
				m_nSystleEx =xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT ;
			if ( m_bGray )
			{
				pCanvas->DrawImage(m_pPicture, rcClient, rectDraw,m_nSystleEx | xgdi::DIF_GRAY);
			}
			else
			{
				pCanvas->DrawImage(m_pPicture, rcClient, rectDraw, m_nSystleEx);
			}
			
        }
        pCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
        pCanvas->Detach();
        pCanvas->Release();

        return 0;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        LRESULT lResult = NULL;

        switch(wParam)
        {
            case TIMER_UPDATE_FRAME:
                if(!RenderNext())
                {
                    KillTimer(TIMER_UPDATE_FRAME);
                }
				KillTimer(TIMER_UPDATE_FRAME);
				if(m_pPicture != NULL)
				{	
					m_nPlayTimes--;
					if(m_nPlayTimes == 0)
					{
						//说明不需要播放了，停止时间
						break;
					}
					if(UINT nElapse = m_pPicture->GetFrameElapse())
					{
						SetTimer(TIMER_UPDATE_FRAME, nElapse, NULL);
					}
				}
                break;

            default:
                lResult = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
                break;
        }

        return 0;
    }

    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(GetStyle() & BS_PUSHLIKE)
        {
            CClientRect rcClient(m_hWnd);
            CPoint pt = lParam;
			CPoint wndPt(pt);
			::ClientToScreen(m_hWnd, &wndPt);
            if(rcClient.PtInRect(pt) && m_hWnd == ::WindowFromPoint(wndPt))
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
				m_bDown = false;
                ::ReleaseCapture();
            }
        }
        else
        {
            bHandled = FALSE;
        }

        return 0;
    }

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bDown = true;
		return 0;
	}

    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		if(m_bDown && (GetStyle() & BS_PUSHLIKE))
        {
            int id = ::GetDlgCtrlID(m_hWnd);
            ::PostMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELPARAM(id, BN_CLICKED), (LPARAM)m_hWnd);
        }
        else
        {
            bHandled = FALSE;
        }

		m_bDown = false;
        return 0;
    }

    BOOL SetPicture(LPCTSTR lpszPic)
    {
		if(lpszPic == NULL)
		{
			if(m_strPicName.GetLength() != 0)
			{
				m_pResLoader->UnloadImage(m_strPicName);
				m_pPicture = NULL;
				m_strPicName.Empty();
			}
		}
		else if(m_strPicName != lpszPic)
		{
			if(m_strPicName.GetLength() != 0)
			{
				m_pResLoader->UnloadImage(m_strPicName);
				m_pPicture = NULL;
			}
			m_strPicName = lpszPic;
			if(m_strPicName.GetLength() > 0)
				m_pPicture = m_pResLoader->LoadImage(lpszPic);
		}

        Invalidate(FALSE);

		return TRUE;
    }

	void SetImageGray(BOOL bGray)
	{
		m_bGray = bGray;
	}

    BOOL TriggerOn(int nTimes = 0)  //nTimes是指gif图片播放次数,为0表示不限制
    {
		UINT nElapse = 0;
		m_nPlayTimes = 0;
        if(m_pPicture != NULL && m_pPicture->GetFrameCount() > 1)
		{	 
			if(nTimes > 0)
				m_nPlayTimes = nTimes * m_pPicture->GetFrameCount();
			if(nElapse = m_pPicture->GetFrameElapse())
			{
				SetTimer(TIMER_UPDATE_FRAME, nElapse, NULL);
			}
		}
        return TRUE;
    }

    inline BOOL TriggerOff()
    {
        KillTimer(TIMER_UPDATE_FRAME);
		m_nPlayTimes = 0;

        return TRUE;
    }

    inline BOOL RenderNext()
    {
		if(NULL == m_pPicture)
			return FALSE;
        m_pPicture->NextFrame(-1);
        Invalidate();
        return TRUE;
    }

	inline BOOL RenderFrame(int nIndex)
	{
		if(NULL == m_pPicture)
			return FALSE;
		m_pPicture->NextFrame(nIndex);
		Invalidate();
		return TRUE;
	}

	const SIZE GetImageSize()
	{
		SIZE szImage = { 0 };
		if(NULL == m_pPicture)
		{
			return szImage;
		}

		return m_pPicture->GetSize();
	}
protected:
    xgdi::IImage*				m_pPicture;

    BOOL                        m_bHighlighted;
	xskin::IResLoader*			m_pResLoader;

	// 添加变量以支持只画一部分图像 [3/30/2010 何春龙]
	CRect						m_rectDraw;

	BOOL						m_bGray;	// 为True时，图像为灰度显示
	DWORD						m_nSystleEx;
	WTL::CString				m_strPicName;
	bool						m_bDown;
	int							m_nPlayTimes;
};

NAMESPACE_END(xctrl)