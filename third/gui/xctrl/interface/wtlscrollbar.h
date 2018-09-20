#pragma once
#include "xctrl.h"
#include "transwindow.h"
#include "../../xlayout/interface/ILayout.h"
#include "cssparser.h"

NAMESPACE_BEGIN(xctrl)

enum SCROLLPOS
{
    SP_LEFTBTN              = 0,
    SP_THUMB                ,
    SP_RIGHTBTN             ,
    SP_BOTTOM               ,

    SP_MAX
};

//滚动条可以设置高度宽度，设置高度为其他高度时，更改的位置为 xml中的图片大小，如mainwnd.xml 中的vscroll-iconset="image\vscroll_back.png@16x14" 
//case SP_LEFTBTN: m_sizeItem.SetSize(16, 16); break; 改成case SP_LEFTBTN: m_sizeItem.SetSize(16, 14); break;
//ScrollBar.h 中case SP_LEFTBTN: m_sizeItem.SetSize(16, 16); break; 改为case SP_LEFTBTN: m_sizeItem.SetSize(16, 14); break;
//即可
class CScrollItemHandler
    : implement IItemHandler
{
public:
    CScrollItemHandler(int nPos, const RECT& rcInner, xgdi::IIconSet* pIconSet)
        : m_nScrollPos(nPos), m_rcInner(rcInner), m_pIconSet(pIconSet)
    {
        switch(nPos)
        {
        case SP_LEFTBTN: m_sizeItem.SetSize(16, 16); break;
        case SP_THUMB: break;
        case SP_RIGHTBTN: m_sizeItem.SetSize(16, 16); break;
        case SP_BOTTOM: break;
        default:
            break;
        }
    }

    virtual ~CScrollItemHandler()
    {
    }

    virtual void Release()
    {
        delete this;
    }

    // From IWindowHandler
    virtual LRESULT OnMessage(__in UINT, __in WPARAM, __in LPARAM, __inout BOOL&)
    {
        return 0;
    }

    // From IItemHandler
    virtual const SIZE& CalcItemSize(__in ICanvas*)
    {
        switch(m_nScrollPos)
        {
        case SP_LEFTBTN:
        case SP_RIGHTBTN:
            if(m_pIconSet) m_sizeItem = m_pIconSet->GetIconSize();
            break;

        case SP_THUMB:
        case SP_BOTTOM:
        default:
            break;
        }
        return m_sizeItem;
    }
    virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
    {
        BOOL bDisabled          = nFlag & DIF_DISABLED;
        BOOL bHighlighted       = nFlag & DIF_HIGHLIGHTED;
        BOOL bSelected          = nFlag & DIF_SELECTED;

        int nIndex;
        if(bDisabled)
        {
            nIndex = 3;
        }
        else if(bSelected)
        {
            nIndex = 1;
        }
        else if(bHighlighted)
        {
            nIndex = 2;
        }
        else
        {
            nIndex = 0;
        }

        if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(m_nScrollPos * 4 + nIndex))
        {
            pCanvas->DrawIcon(pIcon, rcItem, m_rcInner, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
        }
    }
    virtual LPCTSTR GetItemText() const
    {
        return NULL;
    }
    virtual void SetItemText(__in LPCTSTR)
    {

    }

protected:
    int                 m_nScrollPos;
    CSize               m_sizeItem;
    CRect               m_rcInner;
    xgdi::IIconSet*     m_pIconSet;
};

class CHScrollItemHandler
    : public CScrollItemHandler
{
public:
    CHScrollItemHandler(int nPos, const RECT& rcInner, xgdi::IIconSet* pIconSet)
        : CScrollItemHandler(nPos, rcInner, pIconSet)
    {
    }

    // From IWindowHandler
    virtual IWindowHandler* Clone()
    {
        return new CHScrollItemHandler(m_nScrollPos, m_rcInner, m_pIconSet);
    }
};

class CVScrollItemHandler
    : public CScrollItemHandler
{
public:
    CVScrollItemHandler(int nPos, const RECT& rcInner, xgdi::IIconSet* pIconSet)
        : CScrollItemHandler(nPos, rcInner, pIconSet)
    {
    }

    // From IWindowHandler
    virtual IWindowHandler* Clone()
    {
        return new CVScrollItemHandler(m_nScrollPos, m_rcInner, m_pIconSet);
    }
};


class CScrollBar
	: public CWindowImpl< CScrollBar, CWindow, CControlWinTraits >
    , public TransChildWindow<CScrollBar>
    , public XCssParser
{
public:
	CScrollBar()
        : m_extHandler(this), m_hWndBuddy(NULL), m_rcISInner(4, 4, 4, 4), m_pLayoutObjIn(NULL)
    {
        m_spScroll = (IScrollBar*)CreateGTUIBaseObj(IID_ScrollBar);
		m_pBkImage = NULL;
        m_pResLoader = xskin::CResLoader::inst();
    }

    virtual BOOL ParseCss(IProperty* pProp)
    {
		XStyleParser stylePraser(pProp->GetProperty(_T("style")));
		m_dwStyle = stylePraser.dwStyle;
		if(IsWindow()){
			ModifyStyle(0,m_dwStyle,0);
		}

        LPCTSTR proper;
        if(proper = pProp->GetProperty(_T("inner")))
        {
            CTextUtil::TextToRect(proper, m_rcISInner);
        }

		if(proper = pProp->GetProperty(_T("bkg-image")))
		{
			xgdi::IImage *m_pBkImage = m_pResLoader->LoadImage(proper);
			m_spScroll->SetBkImage(m_pBkImage);
		}

        if(proper = pProp->GetProperty(_T("iconset")))
        {
            m_pIconSet = m_pResLoader->LoadIconSet(proper);
            
            if(GetStyle() & SB_VERT)
            {
                m_spScroll->SetItemHandler(&CVScrollItemHandler(SP_LEFTBTN, m_rcISInner, m_pIconSet), 0);
                m_spScroll->SetItemHandler(&CVScrollItemHandler(SP_THUMB, m_rcISInner, m_pIconSet), 1);
                m_spScroll->SetItemHandler(&CVScrollItemHandler(SP_RIGHTBTN, m_rcISInner, m_pIconSet), 2);
                m_spScroll->SetItemHandler(&CVScrollItemHandler(SP_BOTTOM, m_rcISInner, m_pIconSet), 3);
            }
            else
            {
                m_spScroll->SetItemHandler(&CHScrollItemHandler(SP_LEFTBTN, m_rcISInner, m_pIconSet), 0);
                m_spScroll->SetItemHandler(&CHScrollItemHandler(SP_THUMB, m_rcISInner, m_pIconSet), 1);
                m_spScroll->SetItemHandler(&CHScrollItemHandler(SP_RIGHTBTN, m_rcISInner, m_pIconSet), 2);
                m_spScroll->SetItemHandler(&CHScrollItemHandler(SP_BOTTOM, m_rcISInner, m_pIconSet), 3);
            }
        }

        return XCssParser::ParseCss(pProp);
    }

public:
	DECLARE_WND_CLASS(_T("wtlScrollBar"))

	BEGIN_MSG_MAP(CScrollBar)
		MESSAGE_HANDLER(WM_CREATE,                  OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,                 OnDestroy)
		MESSAGE_HANDLER(WM_PAINT,                   OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND,              OnEraseBkgnd)
		MESSAGE_HANDLER(SBM_SETSCROLLINFO,          OnSetScroll)
        MESSAGE_HANDLER(SBM_GETSCROLLINFO,          OnGetScroll)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,             OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP,               OnLButtonUp)
        MESSAGE_HANDLER(WM_SIZE,                    OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE,               OnMouseMove)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		ModifyStyle(0,m_dwStyle,0);

        m_spScroll->SetExtHandler(&m_extHandler, ((GetStyle() & SB_VERT) ? SB_VERT : SB_HORZ));
        m_spScroll->Attach(m_hWnd);

        SCROLLINFO si;
        ZeroMemory(&si, sizeof(SCROLLINFO));
        si.cbSize       = sizeof(SCROLLINFO);
        si.fMask        = SIF_ALL;
        si.nMax         = 100;
        si.nMin         = 0;
        si.nPage        = 100;
        si.nPos         = 0;
        si.nTrackPos    = 0;
        m_spScroll->SetScrollInfo(&si);

        return 0;
    }

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
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
        m_spScroll->Paint(pCanvas, rcClient, rcUpdate);

        pCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
        pCanvas->Detach();
        pCanvas->Release();
        return 0;
    }

    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

	LRESULT OnSetScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_spScroll->SetScrollInfo((LPSCROLLINFO)lParam);
        return 0;
    }

    LRESULT OnGetScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_spScroll->GetScrollInfo((LPSCROLLINFO)lParam);
        return 0;
    }

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spScroll->OnMessage(uMsg, wParam, lParam, bHandled);
    }

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spScroll->OnMessage(uMsg, wParam, lParam, bHandled);
    }

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spScroll->OnMessage(uMsg, wParam, lParam, bHandled);
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SCROLLINFO si;
        si.fMask = 0;
        m_spScroll->SetScrollInfo(&si);

        return 0;
    }
    
public:
	void BindBuddy(HWND hWndBuddy)
    {
        m_hWndBuddy = hWndBuddy;
    }

    IScrollBar* GetScrollBarPtr()
    {
        return m_spScroll;            
    }

    int GetScrollInfo(LPSCROLLINFO lpScrollInfo)
    {
        return m_spScroll->GetScrollInfo(lpScrollInfo);
    }

    void SetExtHandler(IWindowHandler* pHandler)
    {
        return m_spScroll->SetExtHandler(pHandler, ((GetStyle() & SB_VERT) ? SB_VERT : SB_HORZ));
    }

    void SetLayoutObj(xlayout::ILayoutObj* pLayoutObj)
    {
        m_pLayoutObjIn = pLayoutObj;
    }

    int SetScrollInfo(LPSCROLLINFO lpScrollInfo)
    {
        return m_spScroll->SetScrollInfo(lpScrollInfo);
    }

private:
    class CExtHandler
        : implement IWindowHandler
    {
    public:
        CExtHandler(CScrollBar* pScroll)
        {
            m_pScroll = pScroll;
        }

        virtual void Release()
        {
            delete this;
        }

        virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            if(m_pScroll)
            {
                if(uMsg == WM_SHOWWINDOW && m_pScroll->m_pLayoutObjIn)
                {
                    m_pScroll->m_pLayoutObjIn->Show((lParam ? true : false), true);
                }

                if(m_pScroll->m_hWndBuddy)
                {
                    if(uMsg == WM_HSCROLL || uMsg == WM_VSCROLL)
                    {
                        switch(wParam & 0x0000FFFF)
                        {
                        case SB_THUMBTRACK:
                            {
                                SCROLLINFO si;
                                si.cbSize	= sizeof(SCROLLINFO);
                                si.fMask	= SIF_ALL;
                                m_pScroll->GetScrollInfo(&si);
                                si.nPos     = si.nTrackPos;
                                m_pScroll->SetScrollInfo(&si);
                                break;
                            }
                        default:
                            break;
                        }
                    }

                    ::PostMessage(m_pScroll->m_hWndBuddy, uMsg, wParam, lParam);
                }
                else
                {
                }
                return 0;
            }
            
            bHandled = FALSE;
            return 0;
        }

        virtual IWindowHandler* Clone()
        {
            return new CExtHandler(m_pScroll);
        }

    private:
        CScrollBar* m_pScroll;
    };

private:
    xlayout::ILayoutObj*            m_pLayoutObjIn;
    CExtHandler                     m_extHandler;
    XObjPtr<xctrl::IScrollBar>      m_spScroll;
	HWND			                m_hWndBuddy;

    CRect                           m_rcISInner;
    xgdi::IIconSet*                 m_pIconSet;
};

NAMESPACE_END(xctrl)