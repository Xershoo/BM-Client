#pragma once
#include <xctrl.h>
#include <transwindow.h>
#include "ILayout.h"
#include "wtlscrollbar.h"

NAMESPACE_BEGIN(xctrl)

class CSliderButtonHandler
    : implement IItemHandler
{
public:
    CSliderButtonHandler(int nPos, const RECT& rcInner, xgdi::IIconSet* pIconSet)
        : m_nSliderPos(nPos), m_rcInner(rcInner), m_pIconSet(pIconSet), m_sizeItem(0)
    {
    }

    virtual ~CSliderButtonHandler()
    {
    }

    virtual void Release()
    {
        delete this;
    }

    // From IWindowHandler
    virtual IWindowHandler* Clone()
    {
        return new CSliderButtonHandler(m_nSliderPos, m_rcInner, m_pIconSet);
    }

    virtual LRESULT OnMessage(__in UINT, __in WPARAM, __in LPARAM, __inout BOOL&)
    {
        return 0;
    }

    // From IItemHandler
    virtual const SIZE& CalcItemSize(__in ICanvas*)
    {
        switch(m_nSliderPos)
        {
        case SP_LEFTBTN:
        case SP_RIGHTBTN:
            if(m_pIconSet) m_sizeItem = m_pIconSet->GetIconSize();
            break;

        case SP_THUMB:
			if(m_pIconSet) m_sizeItem = m_pIconSet->GetIconSize();
			break;
        default:
            break;
        }
        return m_sizeItem;
    }
    virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
    {
        if(NULL == m_pIconSet) return ;

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
            nIndex = 2;
        }
        else if(bHighlighted)
        {
            nIndex = 1;
        }
        else
        {
            nIndex = 0;
        }

        if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(m_nSliderPos *4 + nIndex))
        {
			pCanvas->DrawIcon(pIcon, rcItem, m_rcInner,xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
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
    int                 m_nSliderPos;
    CSize               m_sizeItem;
    CRect               m_rcInner;
    xgdi::IIconSet*     m_pIconSet;
};

class CSliderScrollHandler
    : implement IItemHandler
{
public:
    CSliderScrollHandler(IScrollBar* pScroll, const RECT& rcInner, xgdi::IIconSet* pIconSet)
        : m_pScroll(pScroll), m_rcInner(rcInner), m_pIconSet(pIconSet), m_sizeItem(0)
    {
    }

    virtual ~CSliderScrollHandler()
    {
    }

    virtual void Release()
    {
        delete this;
    }

    // From IWindowHandler
    virtual IWindowHandler* Clone()
    {
        return new CSliderScrollHandler(m_pScroll, m_rcInner, m_pIconSet);
    }
    virtual LRESULT OnMessage(__in UINT, __in WPARAM, __in LPARAM, __inout BOOL&)
    {
        return 0;
    }

    // From IItemHandler
    virtual const SIZE& CalcItemSize(__in ICanvas*)
    {
        return m_sizeItem;
    }
    virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
    {
        if(NULL == m_pIconSet) return ;

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

        if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(nIndex))
        {
			CRect rcTemp = rcItem;
			if(rcTemp.Height() > m_pIconSet->GetIconSize().cy)
			{
				int nH = rcTemp.Height() - m_pIconSet->GetIconSize().cy;
				rcTemp.bottom = rcTemp.bottom - nH / 2;
				rcTemp.top = rcTemp.bottom - m_pIconSet->GetIconSize().cy;
			}
            pCanvas->DrawIcon(pIcon, rcTemp, m_rcInner,xgdi::DIF_ALPHA |  xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
        }

        if(m_pScroll)
        {
            if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(nIndex + 4))
            {
                SCROLLINFO si;
                si.cbSize   = sizeof(SCROLLINFO);
                si.fMask    = SIF_ALL;
                m_pScroll->GetScrollInfo(&si);

                CRect rcDest(rcItem);
                if(si.nMax > si.nMin)
                {
                    rcDest.right = rcDest.left + si.nPos * rcDest.Width() / (si.nMax - si.nMin);
                }

				CRect rcTemp = rcItem;
				if(rcTemp.Height() > m_pIconSet->GetIconSize().cy)
				{
					int nH = rcTemp.Height() - m_pIconSet->GetIconSize().cy;
					rcTemp.bottom = rcTemp.bottom - nH / 2;
					rcTemp.top = rcTemp.bottom - m_pIconSet->GetIconSize().cy;
				}

                pCanvas->CreateClipOpAnd(&rcDest, 1);
                pCanvas->DrawIcon(pIcon, rcTemp, m_rcInner,xgdi::DIF_ALPHA |  xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
                pCanvas->ResetClip(NULL);
            }
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
    CSize               m_sizeItem;
    CRect               m_rcInner;
    xgdi::IIconSet*     m_pIconSet;
    IScrollBar*         m_pScroll;
};


class CSliderBar
	: public CWindowImpl< CSliderBar, CWindow, CControlWinTraits >
    , public TransChildWindow<CSliderBar>
    , public XCssParser
{
public:
	CSliderBar()
        : m_extHandler(this)
    {
        m_spSlider = (IScrollBar*)CreateGTUIBaseObj(IID_ScrollBar);
        m_pResLoader = xskin::CResLoader::inst();
    }

    virtual BOOL ParseCss(IProperty* pProp)
    {
        LPCTSTR proper;
        if(proper = pProp->GetProperty(_T("button-iconset")))
        {
            if(xgdi::IIconSet* pIconSet = m_pResLoader->LoadIconSet(proper))
            {
                CRect rcInner(4, 4, 4, 4);
                if(proper = pProp->GetProperty(_T("button-inner")))
                {
                    CTextUtil::TextToRect(proper, rcInner);
                }

                m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_LEFTBTN, rcInner, pIconSet), SP_LEFTBTN);
                m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_RIGHTBTN, rcInner, pIconSet), SP_RIGHTBTN);
                m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_THUMB, rcInner, pIconSet), SP_THUMB);
				m_spSlider->SetThumbLength(pIconSet->GetIconSize().cx);
            }
        }

		if(proper = pProp->GetProperty(_T("no-btns")))
		{
			if(_ttoi(proper) > 0)
			{
				 m_spSlider->SetItemHandler(NULL, SP_LEFTBTN);
				 m_spSlider->SetItemHandler(NULL, SP_RIGHTBTN);
			}
		}

        if(proper = pProp->GetProperty(_T("slider-iconset")))
        {
            if(xgdi::IIconSet* pIconSet = m_pResLoader->LoadIconSet(proper))
            {
                CRect rcInner(4, 4, 4, 4);
                if(proper = pProp->GetProperty(_T("slider-inner")))
                {
                    CTextUtil::TextToRect(proper, rcInner);
                }

                m_spSlider->SetItemHandler(&CSliderScrollHandler(m_spSlider, rcInner, pIconSet), SP_BOTTOM);
            }
        }
        return XCssParser::ParseCss(pProp);
    }

public:
	DECLARE_WND_CLASS(_T("wtlSliderBar"))

	BEGIN_MSG_MAP(CSliderBar)
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
        MESSAGE_HANDLER(WM_ENABLE,                  OnEnable)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_spSlider->SetExtHandler(&m_extHandler, SB_HORZ);
        m_spSlider->Attach(m_hWnd);

        CRect rcInner(4, 4, 4, 4);
        m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_LEFTBTN, rcInner, NULL), SP_LEFTBTN);
        m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_THUMB, rcInner, NULL), SP_THUMB);
        m_spSlider->SetItemHandler(&CSliderButtonHandler(SP_RIGHTBTN, rcInner, NULL), SP_RIGHTBTN);

        m_spSlider->SetItemHandler(&CSliderScrollHandler(m_spSlider, rcInner, NULL), SP_BOTTOM);

        SetRange(100);

        return 0;
    }

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_spSlider->Detach();

        return 0;
    }

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CUpdateRect rcUpdate(m_hWnd);
        CPaintDC dc(m_hWnd);
        CClientRect rcClient(m_hWnd);

        xgdi::ICanvas* pCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvasHandle);
        pCanvas->Attach(dc.m_hDC);
		ParentDrawBkgrd(pCanvas, rcUpdate);       
        m_spSlider->Paint(pCanvas, rcClient, rcUpdate);
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
        m_spSlider->SetScrollInfo((LPSCROLLINFO)lParam);
        return 0;
    }

    LRESULT OnGetScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        m_spSlider->GetScrollInfo((LPSCROLLINFO)lParam);
        return 0;
    }

	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spSlider->OnMessage(uMsg, wParam, lParam, bHandled);
    }

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return m_spSlider->OnMessage(uMsg, wParam, lParam, bHandled);
    }

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
		if(::GetCapture() == m_hWnd)
			return m_spSlider->OnMessage(uMsg, wParam, lParam, bHandled);
		else
			return 0;
    }

    LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        Invalidate(FALSE);
        
        return 0;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        SCROLLINFO si;
        si.fMask = 0;
        m_spSlider->SetScrollInfo(&si);

        return 0;
    }
    
public:
    void SetExtHandler(IWindowHandler* pHandler)
    {
        return m_spSlider->SetExtHandler(pHandler, SB_HORZ);
    }

    void SetItemHandler(IItemHandler* pHandler, int nFlag)
    {
        return m_spSlider->SetItemHandler(pHandler, nFlag);
    }

    int GetRange()
    {
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        m_spSlider->GetScrollInfo(&si);

        return si.nMax;
    }

    int GetPos()
    {
        SCROLLINFO si;
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        m_spSlider->GetScrollInfo(&si);

        return si.nPos;
    }

    void SetPos(int nPos)
    {
        SCROLLINFO si;
        si.cbSize   = sizeof(SCROLLINFO);
        si.fMask    = SIF_ALL;
        m_spSlider->GetScrollInfo(&si);
        si.nPos     = nPos;
        si.nPage    = 1;
        m_spSlider->SetScrollInfo(&si);
    }

	void SetPosNoMoveThumb(int nPos)
	{
		SCROLLINFO si;
		si.cbSize   = sizeof(SCROLLINFO);
		si.fMask    = SIF_ALL;
		m_spSlider->GetScrollInfo(&si);
		si.nPos     = nPos;
		si.nPage    = 1;
		m_spSlider->SetScrollInfoNotThnmb(&si);
	}

    void SetRange(int nMax)
    {
        SCROLLINFO si;
        si.cbSize   = sizeof(SCROLLINFO);
        si.fMask    = SIF_ALL;
        m_spSlider->GetScrollInfo(&si);
        si.nMax     = nMax;
        si.nMin     = 0;
        si.nPage    = 1;
        m_spSlider->SetScrollInfo(&si);
    }

private:
    void Scroll(int nOffset)
    {
        SCROLLINFO si;
        si.cbSize   = sizeof(SCROLLINFO);
        si.fMask    = SIF_POS;
        m_spSlider->GetScrollInfo(&si);
        si.nPos     += nOffset;
        m_spSlider->SetScrollInfo(&si);
    }

    void OnScroll(WPARAM wParam, LPARAM lParam)
    {
        switch(wParam & 0x0000FFFF)
        {
        case SB_LINELEFT: Scroll(-1); break;
        case SB_LINERIGHT: Scroll(1); break;
        case SB_PAGELEFT: SetPos(HIWORD(wParam)); break;
        case SB_PAGERIGHT: SetPos(HIWORD(wParam)); break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            {
                SCROLLINFO si;
                si.cbSize	= sizeof(SCROLLINFO);
                si.fMask	= SIF_ALL;
                m_spSlider->GetScrollInfo(&si);
                SetPos(si.nTrackPos);
                break;
            }
        case SB_LEFT: break;
        case SB_RIGHT: break;
        case SB_ENDSCROLL:
        default:
            break;
        }

        // Notify Parent
        ::PostMessage(::GetParent(m_hWnd), WM_HSCROLL, wParam, lParam);
    }

private:
    class CExtHandler
        : implement IWindowHandler
    {
    public:
        CExtHandler(CSliderBar* pSlider)
        {
            m_pSlider = pSlider;
        }

        virtual void Release()
        {
            delete this;
        }

        virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
        {
            if(m_pSlider)
            {
                switch(uMsg)
                {
                case WM_HSCROLL: m_pSlider->OnScroll(wParam, lParam); break;

                default:
                    break;
                }
            }
            else
            {
                bHandled = FALSE;
            }

            return 0;
        }

        virtual IWindowHandler* Clone()
        {
            return new CExtHandler(m_pSlider);
        }

    private:
        CSliderBar* m_pSlider;
    };

private:
    CExtHandler                     m_extHandler;
    XObjPtr<xctrl::IScrollBar>      m_spSlider;
};

NAMESPACE_END(xctrl)