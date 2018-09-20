#pragma	  once

#include "xctrl.h"
#include "xgdi/xgdi.h"
#include "xgdi/xgdiutil.h"
#include "xuimisc.h"


NAMESPACE_BEGIN(xctrl)


struct ChildDrawBkgrd
{
    xgdi::ICanvas* canvas;
    RECT		   rcScreen;  //screen rect
    RECT		   rcCanvas;
};

template <class TWindow>
class TransChildWindow
{
public:
    BOOL ParentDrawBkgrd(xgdi::ICanvas* pCanvas, RECT rect)
    {
        TWindow* pThis = (TWindow*)this;


        ChildDrawBkgrd cdb;
        cdb.canvas		= pCanvas;			
        cdb.rcCanvas	= rect;
        pThis->ClientToScreen(&rect);
        cdb.rcScreen	= rect;

        HWND hForwordWnd = ::GetParent(pThis->m_hWnd);
        return ::SendMessage(hForwordWnd, WM_CHILD_DRAW_BKGRD, (WPARAM)pThis->m_hWnd, (LPARAM)&cdb);;
    }
};

template<class TWindow>
class TransWindow
{
public:
    BEGIN_MSG_MAP(TransWindow)
        MESSAGE_HANDLER(WM_CHILD_DRAW_BKGRD,               OnDrawChildBkGrd)
    END_MSG_MAP()

    LRESULT OnDrawChildBkGrd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bResult)
    {
        ChildDrawBkgrd* pChildDraw = (ChildDrawBkgrd*)lParam;
        if(pChildDraw){
            xgdi::ICanvas* pCanvas = pChildDraw->canvas;
            RECT		   rect	   = pChildDraw->rcScreen; //screnn rect

            TWindow* pThis = (TWindow*)this; 
            pThis->ScreenToClient(&rect);

            return (HRESULT)pThis->DrawChildBkGrd(pCanvas, rect, pChildDraw->rcCanvas, wParam);				
        }
        return NULL;;
    }
    virtual HRESULT DrawChildBkGrd(xgdi::ICanvas* pCanvas, RECT rcSrc, RECT rcDest, int ctrlId, int nFlag = 0)
    {
        return NULL;
    }
};

template<class TWindow>
class ForwordTransWindow : public TransWindow<TWindow>
{
public:
	 BEGIN_MSG_MAP(TransWindow)
        MESSAGE_HANDLER(WM_CHILD_DRAW_BKGRD,               OnDrawChildBkGrd)
    END_MSG_MAP()

     LRESULT OnDrawChildBkGrd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bResult)
	 {
        TWindow* pThis = (TWindow*)this; 
		::SendMessage(pThis->GetParent(), uMsg, wParam, lParam);
		return 0;
    }
};

template<class TWindow>
class ImageTransWindow : public TransWindow<TWindow>
{
    BEGIN_MSG_MAP(ImageTransWindow)
        MESSAGE_HANDLER(WM_SIZE,               OnSize)
        CHAIN_MSG_MAP(TransWindow<TWindow>)
    END_MSG_MAP()

    ImageTransWindow()
    {
        m_pBkImage	= NULL;
        m_pBkCanvas	= NULL;
		m_bFromLoad = FALSE;
		m_szCanvas  = NULL_SIZE;
    }
    virtual ~ImageTransWindow()
    {
        if(m_pBkImage && m_bFromLoad){
            m_pBkImage->Release();
            m_pBkImage = NULL;
        }
        if(m_pBkCanvas){
            m_pBkCanvas->Release();
        }
    }
	void LoadBkgrdImage(LPCTSTR path, LPRECT outter, LPRECT inner)
	{		
		 if(m_pBkImage && m_bFromLoad){
            m_pBkImage->Release();
        }
		m_bFromLoad = FALSE; //xxxx
		xgdi::IImage* pImage = (xgdi::IImage*)xgdi::CoCreateUIObject(xgdi::IID_UIImage);
        pImage->LoadImage(path, CSize(0, 0));
		SetBkgrdImage(pImage, outter, inner );
		m_bFromLoad = TRUE;
	}

	void	SetBkgrdImage(xgdi::IImage* image, LPRECT outter, LPRECT inner)
	{
	
		if(m_pBkImage && m_bFromLoad){
			m_pBkImage->Release();
		}
		m_pBkImage = image;
		m_bFromLoad = FALSE;

		m_rcOutter = CImgStreRect(image, outter);
		m_rcInner  = CImgStreRect(image, inner);
	}
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        TWindow* pThis = (TWindow*)this;
        CRect rcClient;
        pThis->GetClientRect(&rcClient);		

        if(rcClient.Width() > 0 && rcClient.Height() > 0)
        {           
				int cx = rcClient.Width();
				int cy = rcClient.Height();
				if(cx > m_szCanvas.cx || cy > m_szCanvas.cy)
				{						
					m_szCanvas.cx = upper_count(cx,100) * 100;
					m_szCanvas.cy = upper_count(cy,100) * 100;
									
				}else if((cx + 100 < m_szCanvas.cx) || (cy + 100 < m_szCanvas.cy))
				{
					m_szCanvas.cx = upper_count(cx,100) * 100;
					m_szCanvas.cy = upper_count(cy,100) * 100;

				}else{
					return 0;
				}

				if(m_pBkCanvas) {
						m_pBkCanvas->Release();
				}
				m_pBkCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
				m_pBkCanvas->SetSize(m_szCanvas);
				HDC hDC = pThis->GetDC();
				m_pBkCanvas->Attach(hDC);
				pThis->ReleaseDC(hDC);

				if(m_pBkImage)
				{
					xgdi::DrawStretchImage(m_pBkCanvas, m_pBkImage, rcClient, m_rcOutter, m_rcInner,
						xgdi::DIF_STRETCH|xgdi::DIF_TRANSPARENT, xgdi::DEF_TRANS_COLOR);
				}			

				
							
        }
        bHandled = FALSE;
        return 0;
    }
    virtual HRESULT DrawChildBkGrd(xgdi::ICanvas* pCanvas, RECT rcSrc, RECT rcDest, int ctrlId, int nFlag = 0)
    {
        if(m_pBkCanvas && pCanvas){					
            m_pBkCanvas->Flush(pCanvas, rcDest, rcSrc, xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT | nFlag, xgdi::DEF_TRANS_COLOR);
			return TRUE;
        }
        return NULL;
    }

	virtual  ICanvas*  GetBkgrdCanvas()
	{
			return m_pBkCanvas;
	}

protected:
    xgdi::IImage*	m_pBkImage;		
    xgdi::ICanvas*	m_pBkCanvas;
	BOOL			m_bFromLoad;
	CSize			m_szCanvas;

    CRect			m_rcOutter;
    CRect			m_rcInner;
};


NAMESPACE_END(xctrl)