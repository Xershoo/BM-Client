#pragma once
#include "xctrl.h"
#include "xuimisc.h"
#include "cssparser.h"
#include "wtlscrollbar.h"
#include "skinloader.h"

#define WTL_WIDTH_BORDER 2

NAMESPACE_BEGIN(xctrl)

//class CSysScrollBar
//    : implement IScrollBar
//{
//public:
//    CSysScrollBar(BOOL bVert) : m_bVert(bVert) {}
//
//    // From IObject
//    virtual void Release() {}
//
//    // From IUIObject
//    virtual BOOL Attach(const xgdi::UI_HANDLE& hWnd)
//    {
//        m_hWnd = hWnd;
//        return FALSE;
//    }
//    virtual xgdi::UI_HANDLE Detach()
//    {
//        HWND hWnd = m_hWnd;
//        m_hWnd = NULL;
//        return hWnd;
//    }
//    virtual UI_HANDLE GetUIHandle() const
//    {
//        return m_hWnd;
//    }
//
//    // From IWindowObject
//    virtual BOOL Paint(ICanvas* pCanvas, const RECT& rcCanvas, const RECT& rcUpdate) { return FALSE; }
//    virtual BOOL GetRect(RECT& rcRect) const { return FALSE; }
//    virtual void InvalidateRect(const RECT& rc) {};
//    virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { bHandled = FALSE; return 0; }
//    virtual void SetSite(xctrl::IWindowLessHost* site) {}
//    virtual BOOL ShowScrollBar(__in BOOL bShow) { return ::ShowScrollBar(m_hWnd, m_bVert ? SB_VERT : SB_HORZ, bShow); }
//    
//    // From IScrollBar
//    virtual int GetScrollInfo(LPSCROLLINFO lpScrollInfo) const
//    {
//        return ::GetScrollInfo(m_hWnd, m_bVert ? SB_VERT : SB_HORZ, lpScrollInfo);
//    }
//    virtual void SetExtHandler(IWindowHandler* pHandler, int nBar) {}
//    virtual void SetItemHandler(IItemHandler* pHandler, int nIndex) {}
//    virtual int SetScrollInfo(LPSCROLLINFO lpScrollInfo)
//    {
//        return ::SetScrollInfo(m_hWnd, m_bVert ? SB_VERT : SB_HORZ, lpScrollInfo, TRUE);
//    }
//    virtual BOOL SetScrollCapture(BOOL bCapture) { return FALSE; }
//
//private:
//    HWND m_hWnd;
//    BOOL m_bVert;
//};

class CWTLTreeCtrl
    : public CWindowImpl< CWTLTreeCtrl, CWindow >
    , public TransChildWindow<CScrollBar>
    , public xctrl::XCssParser
{
public:
    CWTLTreeCtrl()
        : m_rcVSInner(4, 4, 5, 5), m_rcHSInner(4, 4, 4, 4)
        //, m_sysVScroll(TRUE), m_sysHScroll(FALSE)
    {
        m_spTreeView = (xctrl::ITreeViewCtrl*)xctrl::CreateGTUIBaseObj(xctrl::IID_TreeViewCtrl);
        m_pResLoader = xskin::CResLoader::inst();
		m_bFillAll = TRUE;
		m_bTracking = false;
		m_bLButtonUpToParent = false;
    }

	BOOL SetVScrollPoint(int x, int y)
	{
		if(m_spTreeView)
			m_spTreeView->ShowScrollBar(SB_VERT, x, y);
		return TRUE;
	}

	BOOL SetVScrollIconSet( xgdi::IIconSet* pIconSet)
	{
		if(pIconSet && m_spTreeView)
		{
			m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(0, m_rcVSInner, pIconSet), 0);
			m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(1, m_rcVSInner, pIconSet), 1);
			m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(2, m_rcVSInner, pIconSet), 2);
			m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(3, m_rcVSInner, pIconSet), 3);
		}
		return TRUE;
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		xctrl::XStyleParser stylePraser(pProp->GetProperty(_T("style")));
		stylePraser.AddStyle(_T("TVS_FULLROWSELECT"), TVS_FULLROWSELECT);
		stylePraser.AddStyle(_T("TVS_HASBUTTONS"),    TVS_HASBUTTONS);
		m_dwStyle = stylePraser.dwStyle;
		if(IsWindow()){
			ModifyStyle(0,m_dwStyle,0);
		}


        LPCTSTR proper;
        if(proper = pProp->GetProperty(_T("vscroll")))
        {
            POINT pt;
            CTextUtil::TextToPoint(proper, pt);

            // pt.x : 0 disable, 1 always, 2 auto
            // pt.y : width
            if(pt.x && pt.y)
            {
                m_spTreeView->ShowScrollBar(SB_VERT, pt.x, pt.y);
            }
        }

		if(proper = pProp->GetProperty(_T("vs-inner")))
		{
			CTextUtil::TextToRect(proper, m_rcVSInner);
		}

		if(proper = pProp->GetProperty(_T("hs-inner")))
		{
			CTextUtil::TextToRect(proper, m_rcHSInner);
		}

        if(proper = pProp->GetProperty(_T("hscroll")))
        {
            POINT pt;
            CTextUtil::TextToPoint(proper, pt);

            // pt.x : 0 disable, 1 always, 2 auto
            // pt.y : width
            if(pt.x && pt.y)
            {
                m_spTreeView->ShowScrollBar(SB_HORZ, pt.x, pt.y);
            }
        }

		if(proper = pProp->GetProperty(_T("vs-bkgImage")))
		{
			xgdi::IImage *pBkImage = m_pResLoader->LoadImage(proper);
			if(m_spTreeView)
			{
				m_spTreeView->GetScrollBar(SB_VERT)->SetBkImage(pBkImage);
				m_spTreeView->GetScrollBar(SB_HORZ)->SetBkImage(pBkImage);
			}
		}

        if(proper = pProp->GetProperty(_T("vscroll-iconset")))
        {
            if(m_pResLoader)
            {
                xgdi::IIconSet* pIconSet = m_pResLoader->LoadIconSet(proper);
                if(m_spTreeView)
                {
                    m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(0, m_rcVSInner, pIconSet), 0);
                    m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(1, m_rcVSInner, pIconSet), 1);
                    m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(2, m_rcVSInner, pIconSet), 2);
                    m_spTreeView->GetScrollBar(SB_VERT)->SetItemHandler(&xctrl::CVScrollItemHandler(3, m_rcVSInner, pIconSet), 3);
                }
            }
        }

        if(proper = pProp->GetProperty(_T("hscroll-iconset")))
        {
            if(m_pResLoader)
            {
                xgdi::IIconSet* pIconSet = m_pResLoader->LoadIconSet(proper);
                if(m_spTreeView)
                {
                    m_spTreeView->GetScrollBar(SB_HORZ)->SetItemHandler(&xctrl::CHScrollItemHandler(0, m_rcHSInner, pIconSet), 0);
                    m_spTreeView->GetScrollBar(SB_HORZ)->SetItemHandler(&xctrl::CHScrollItemHandler(1, m_rcHSInner, pIconSet), 1);
                    m_spTreeView->GetScrollBar(SB_HORZ)->SetItemHandler(&xctrl::CHScrollItemHandler(2, m_rcHSInner, pIconSet), 2);
                    m_spTreeView->GetScrollBar(SB_HORZ)->SetItemHandler(&xctrl::CHScrollItemHandler(3, m_rcHSInner, pIconSet), 3);
                }
            }
        }

		if(proper = pProp->GetProperty(_T("fillallclient")))
		{
			int nTemp = 0;
			CTextUtil::TextToInt(proper, nTemp);
			m_bFillAll = nTemp == 0? FALSE: TRUE;
			if(m_spTreeView)
			{

				m_spTreeView->SetFillAllClient(m_bFillAll);
			}
		}
		if(proper = pProp->GetProperty(_T("clientbkcolor")))
		{
			COLORREF rc ;
			CTextUtil::TextToColor(proper, rc);
			if(m_spTreeView)
			{

				m_spTreeView->SetClientBkColor(rc);
			}
		}	

		if(proper = pProp->GetProperty(_T("bkgImage")))
		{
			if(m_spTreeView)
			{
				xgdi::IImage* pImage = m_pResLoader->LoadImage(proper);
				m_spTreeView->SetBkImage(pImage);
			}
		}

		

        return XCssParser::ParseCss(pProp);
    }

public:
    DECLARE_WND_CLASS(_T("wtlTreeView"))

    BEGIN_MSG_MAP(CWTLTreeCtrl)
        MESSAGE_HANDLER(WM_CREATE,          OnCreate)
        MESSAGE_HANDLER(WM_DESTROY,			OnDestroy)
		MESSAGE_HANDLER(WM_PAINT,			OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,		OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SIZE,			OnSize)
		MESSAGE_HANDLER(WM_NCCALCSIZE,      OnNcCalcSize)
		MESSAGE_HANDLER(WM_NCPAINT,         OnNcPaint)
		MESSAGE_HANDLER(WM_NCHITTEST,       OnNcHitTest)

		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,	OnMouseButton)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,		OnMouseButton)
		MESSAGE_HANDLER(WM_LBUTTONUP,		OnMouseButtonLUp)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK,	OnMouseButton)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,		OnMouseButton)
		MESSAGE_HANDLER(WM_RBUTTONUP,		OnMouseButton)
		MESSAGE_HANDLER(WM_MOUSEMOVE,		OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE,		OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER,		OnMouseHover)

		MESSAGE_HANDLER(WM_MOUSEWHEEL,		OnMouseWheel)

		MESSAGE_HANDLER(WM_HSCROLL,			OnHScroll)
		MESSAGE_HANDLER(WM_VSCROLL,			OnVScroll)
    END_MSG_MAP()

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		ModifyStyle(0,m_dwStyle,0);
        if(m_spTreeView)
        {
            m_spTreeView->Attach(m_hWnd);
			if(m_bFillAll == FALSE)
			{
				m_spTreeView->SetFillAllClient(m_bFillAll);
			}
            //m_spTreeView->SetScrollBar(SB_VERT, &m_sysVScroll);
            //m_spTreeView->SetScrollBar(SB_HORZ, &m_sysHScroll);
        }

        return 0;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		CUpdateRect rcUpdate(m_hWnd);

		CPaintDC dc(m_hWnd);
		CClientRect rcClient(m_hWnd);
		
		//dc.FillRect(&rcClient, color);

		XGDIObject<xgdi::ICanvas> spCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
		spCanvas->SetSize(rcClient.Size());
		spCanvas->Attach(dc.m_hDC);
		
        ParentDrawBkgrd(spCanvas, rcUpdate);
		rcClient.DeflateRect(m_rcBord);
		rcUpdate.DeflateRect(m_rcBord);
		m_spTreeView->Paint(spCanvas, rcClient, rcUpdate);
		spCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
		spCanvas->Detach();

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
        if(m_spTreeView)
        {
			RECT rc;
			::GetClientRect(m_hWnd, &rc);
			m_spTreeView->CalcScrollHost(rc/*CRect(CPoint(0, 0), CPoint(lParam))*/);
			m_spTreeView->Scroll(0, 0);
        }

		return 0;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		UINT_PTR nRet = 0;

		LPRECT lpRect = NULL;

		if(wParam)
		{
			LPNCCALCSIZE_PARAMS lpNcCalcSize = (LPNCCALCSIZE_PARAMS)lParam;
			lpRect = &lpNcCalcSize->rgrc[0];
			nRet = 0;
		}
		else
		{
			lpRect = (LPRECT)lParam;
			nRet = 0;
		}

#ifndef _USE_WTL_SCROLLBAR_
		bHandled = FALSE;
#else
		if(lpRect && (::GetWindowLong(m_hWnd, GWL_STYLE) & WS_BORDER))
		{
			lpRect->left    += WTL_WIDTH_BORDER;
			lpRect->top     += WTL_WIDTH_BORDER;
			lpRect->right   -= WTL_WIDTH_BORDER;
			lpRect->bottom  -= WTL_WIDTH_BORDER;
		}
#endif

		return nRet;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
#ifndef _USE_WTL_SCROLLBAR_
		bHandled = FALSE;
#else
		if(::GetWindowLong(m_hWnd, GWL_STYLE) & WS_BORDER)
		{
			if(HDC hDC = GetWindowDC())
			{
				XObjPtr<xgdi::ICanvas> spCanvas = (xgdi::ICanvas*)xgdi::CoCreateUIObject(xgdi::IID_UICanvasHandle);
				spCanvas->Attach(hDC);

				CWindowRect rcWindow(m_hWnd);
				rcWindow.MoveToXY(0, 0);
				spCanvas->FrameSolidRect(rcWindow, RGB(0,0,0));
				rcWindow.DeflateRect(1, 1, 1, 1);
				spCanvas->FrameSolidRect(rcWindow, RGB(255,255,255));

				spCanvas->Detach();

				ReleaseDC(hDC);
			}
		}
#endif
		return 0;
	}

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
#ifndef _USE_WTL_SCROLLBAR_
		bHandled = FALSE;
#else
		if(::GetWindowLong(m_hWnd, GWL_STYLE) & WS_BORDER)
		{
			CPoint pt = lParam;
			CWindowRect rcWindow(m_hWnd);
			rcWindow.DeflateRect(WTL_WIDTH_BORDER, WTL_WIDTH_BORDER, WTL_WIDTH_BORDER, WTL_WIDTH_BORDER);
			if(!rcWindow.PtInRect(pt))
			{
				return HTBORDER;
			}
		}
#endif

		return HTCLIENT;
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bTracking)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof( tme );
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 1;
			m_bTracking = ::_TrackMouseEvent( &tme ) ? true : false;
		}
		return OnMouseButton(uMsg, wParam, lParam, bHandled);
	}
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bTracking = false;
		return OnMouseButton(uMsg, wParam, lParam, bHandled);
	}
	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		OutputDebugString(_T("TME_HOVER is deal with\r\n"));
		return OnMouseButton(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMouseButton(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		return m_spTreeView->OnMessage(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMouseButtonLUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_spTreeView->OnMessage(uMsg, wParam, lParam, bHandled);
		if(m_bLButtonUpToParent)
		{
			OnPaint(0,0,0,bHandled);
			HWND hParent = GetParent();
			::PostMessage(hParent, WM_COMMAND, MAKEWPARAM(::GetDlgCtrlID(m_hWnd), BN_CLICKED), 0); 
		}
		return 0;
	}

	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		return m_spTreeView->OnMessage(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spTreeView->OnScroll(SB_HORZ, wParam & 0x0000FFFF);
	}

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spTreeView->OnScroll(SB_VERT, wParam & 0x0000FFFF);
	}

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
       return 0;
    }

	void SendLButtonUpToParent(bool bSendToParent)
	{
		m_bLButtonUpToParent = bSendToParent;
	}

	void SetItemAllShow(xctrl::ITreeItem* pItem, int nShowStyle)
	{
		m_spTreeView->SetItemAllShow(pItem, nShowStyle);
	}

public:
    inline xctrl::ITreeViewCtrl* GetWindowLessObj()
    {
        return (xctrl::ITreeViewCtrl*)m_spTreeView;
    }

protected:
    XObjPtr<xctrl::ITreeViewCtrl>           m_spTreeView;
    CRect                                   m_rcVSInner;
    CRect                                   m_rcHSInner;
	CRect									m_rcBord;
	BOOL									m_bFillAll;
	bool									m_bTracking;
	bool									m_bLButtonUpToParent; //右击鼠标弹掉通知父窗口，让其处理
    //CSysScrollBar                           m_sysVScroll;
    //CSysScrollBar                           m_sysHScroll;
};


NAMESPACE_END(xctrl)