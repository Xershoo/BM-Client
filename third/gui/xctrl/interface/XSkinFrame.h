#pragma once

#include "atlctrls.h"
#include "atlctrlw.h"
#include "atlstr.h"
#include "xgdi/xstretch.h"

NAMESPACE_BEGIN(xctrl)

template<class FrameWindow>
class XSkinFrame
{
public:
	XSkinFrame()
	{
		_rcCaption	= NULL_RECT;
		_szCorner	= NULL_SIZE;
		_szMini		= NULL_SIZE;
		_szMax		= NULL_SIZE;

		_isFixSize	= FALSE;
	}
	virtual ~XSkinFrame(){
		_cornerRgn.DestroyRgns();
	}
protected:
	BEGIN_MSG_MAP(XSkinFrame)
        MESSAGE_HANDLER(WM_NCHITTEST,					OnNcHitTest)
		MESSAGE_HANDLER(WM_ERASEBKGND,					OnEraseBkgnd)
		MESSAGE_HANDLER(WM_GETMINMAXINFO,				OnGetMinMaxInfo)
		MESSAGE_HANDLER(WM_SIZE,						OnSize)
		MESSAGE_HANDLER(WM_PAINT,						OnPaint)
		//MESSAGE_HANDLER(WM_NCCALCSIZE,					OnNcCalcSize)

		MESSAGE_HANDLER(WM_NCLBUTTONDBLCLK,				OnNcDbClick)
    END_MSG_MAP()


	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		/*CPaintDC dc(((FrameWindow*)this)->m_hWnd);
		RECT rect;
		((FrameWindow*)this)->GetClientRect(&rect);
		dc.FillSolidRect(&rect, RGB(0,0,0));*/
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;
		if((BOOL)wParam && lpncsp)
		{
			DWORD dwStyle = ((FrameWindow*)this)->GetStyle();
			if(dwStyle & WS_THICKFRAME)
			{
				lpncsp->rgrc[2] = lpncsp->rgrc[1];
				lpncsp->rgrc[1] = lpncsp->rgrc[0];
			}
		}
		return 0;
	}
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}
#define CORNER_SIZE 4
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CRect rcW;
		((FrameWindow*)this)->GetClientRect(rcW);
		((FrameWindow*)this)->ClientToScreen(rcW);

		CPoint point = lParam;
		
		CRect rcCaption = rcW;
		rcCaption.left	 = _rcCaption.left < 0 ?	rcW.right + _rcCaption.left		: rcW.left + _rcCaption.left;
		rcCaption.top	 = _rcCaption.top	< 0  ?	rcW.bottom + _rcCaption.top		: rcW.top + _rcCaption.top;
		rcCaption.right	 = _rcCaption.left	< 0 ?	rcW.right + _rcCaption.right	: rcW.right + _rcCaption.right;
		rcCaption.bottom = _rcCaption.bottom < 0 ?	rcW.bottom + _rcCaption.bottom	: rcW.top + _rcCaption.bottom;

		// 如果窗口大小固定，只判断CAPTION [3/24/2010 何春龙]
		if (_isFixSize)
		{
			if(rcCaption.PtInRect(point)){
				return HTCAPTION;
			}
			return HTCLIENT;
		}

		CRect rcLeftTop	  = rcW;
		rcLeftTop.bottom  = rcLeftTop.top + CORNER_SIZE;
		rcLeftTop.right  = rcLeftTop.left + CORNER_SIZE;
		if(rcLeftTop.PtInRect(point)){
			return HTTOPLEFT;
		}

		CRect rcLeftBot	  = rcW;
		rcLeftBot.top	 = rcLeftBot.bottom - 4 * CORNER_SIZE;
		rcLeftBot.right  = rcLeftTop.left	+ CORNER_SIZE;
		if(rcLeftBot.PtInRect(point)){
			return HTBOTTOMLEFT;
		}

		CRect rcRightBot = rcW;
		rcRightBot.top	 = rcRightBot.bottom - 4 * CORNER_SIZE;
		rcRightBot.left  = rcRightBot.right  - 4 * CORNER_SIZE;
		if(rcRightBot.PtInRect(point)){
			return HTBOTTOMRIGHT;
		}

		CRect rcRightTop = rcW;
		rcRightTop.bottom	 = rcRightTop.top + CORNER_SIZE;
		rcRightTop.left		  = rcRightTop.right  - CORNER_SIZE;
		if(rcRightTop.PtInRect(point)){
			return HTTOPRIGHT;
		}

		CRect rcLeft = rcW;
		rcLeft.top		+= CORNER_SIZE;
		rcLeft.bottom   -= CORNER_SIZE;
		rcLeft.right	= rcLeft.left + CORNER_SIZE;
		if(rcLeft.PtInRect(point)){
				return HTLEFT;
		}

		CRect rcTop		= rcW;
		rcTop.left		+= CORNER_SIZE;
		rcTop.right		-= CORNER_SIZE;
		rcTop.bottom	= rcTop.top + CORNER_SIZE;
		if(rcTop.PtInRect(point)){
				return HTTOP;
		}

		CRect rcRight		= rcW;
		rcRight.top			+= CORNER_SIZE;
		rcRight.bottom		-= CORNER_SIZE;
		rcRight.left		= rcRight.right - CORNER_SIZE;
		if(rcRight.PtInRect(point)){
				return HTRIGHT;
		}

		CRect rcBottom		= rcW;
		rcBottom.left		+= CORNER_SIZE;
		rcBottom.right		-= CORNER_SIZE;
		rcBottom.top		= rcBottom.bottom - CORNER_SIZE;
		if(rcBottom.PtInRect(point)){
				return HTBOTTOM;
		}

		if(rcCaption.PtInRect(point)){
			return HTCAPTION;
		}
		return HTCLIENT;
	}
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UpdateRegion();
		bHandled = FALSE;
		return 0;
	}
	LRESULT OnNcDbClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 尺寸固定时不响应标题栏双击事件 [4/14/2010 何春龙]
		if (!_isFixSize)
			bHandled = FALSE;
		return 0;
	}
	HRGN GetWindowRgn()
	{
		CRect rcClient,rcWindow;
		((FrameWindow*)this)->GetClientRect(&rcClient);
		HRGN hWndRgn = XGDIUtil::CombineRgn(_stretch,_cornerRgn, rcClient);
		((FrameWindow*)this)->ClientToScreen(&rcClient);
		((FrameWindow*)this)->GetWindowRect(&rcWindow);
		
		::OffsetRgn(hWndRgn, rcClient.left-rcWindow.left, rcClient.top - rcWindow.top);

		return hWndRgn;
	}
	void UpdateRegion()
	{
		HRGN hRgn = GetWindowRgn();
		((FrameWindow*)this)->SetWindowRgn(hRgn, TRUE);
		::DeleteObject(hRgn);
	}

	HWND CreateEx(LPRECT lpRect = NULL,DWORD dwStyle = NULL,HWND hParent = NULL)
	{		 
		if(dwStyle & DS_MODALFRAME) //不可左右上下拉动变大小
			dwStyle |= WS_POPUP|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU|WS_CLIPCHILDREN;
		else
			dwStyle |= WS_POPUP|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU|WS_THICKFRAME|WS_CLIPCHILDREN;
		return ((FrameWindow*)this)->Create(hParent, lpRect, NULL,  dwStyle);
	}
	LRESULT OnGetMinMaxInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		// TODO: Add your message handler code here and/or call default
		if(!_szMax.cx || !_szMax.cy)
		{
				RECT rWorkArea;
				if(!SystemParametersInfo(SPI_GETWORKAREA,sizeof(RECT),&rWorkArea,0))
				{
					rWorkArea.left = rWorkArea.top = 0;
					rWorkArea.right = ::GetSystemMetrics(SM_CXSCREEN);
					rWorkArea.bottom = ::GetSystemMetrics(SM_CYSCREEN);
				}
				int borderw = GetSystemMetrics(SM_CXFRAME); 
				int borderh = GetSystemMetrics(SM_CXFRAME);
				rWorkArea.left		-= borderw;
				rWorkArea.right		+= borderw;
				rWorkArea.top		-= borderh;
				rWorkArea.bottom	+= borderh;
				
				lpMMI->ptMaxPosition.x	= rWorkArea.left;
				lpMMI->ptMaxPosition.y	= rWorkArea.top;
				lpMMI->ptMaxSize.x		= rWorkArea.right - rWorkArea.left;
				lpMMI->ptMaxSize.y		= rWorkArea.bottom - rWorkArea.top;
		}else{
				lpMMI->ptMaxSize.x =	_szMax.cx;
				lpMMI->ptMaxSize.y =	_szMax.cy;
		}

		lpMMI->ptMinTrackSize.x = _szMini.cx;
		lpMMI->ptMinTrackSize.y = _szMini.cy;
		return 0;
	}
public:
	void SetCaption(int left, int top, int right, int bottom)
	{
			_rcCaption.left = left;
			_rcCaption.top  = top;
			_rcCaption.right = right;
			_rcCaption.bottom = bottom;
	}
	void SetCornerRegion(int x, int y)
	{
			_szCorner.cx = x;
			_szCorner.cy = y;
	}
	void SetCornerRegion(xgdi::IImage* pImage, 
						RECT innerRect, 
						COLORREF clrKey = RGB(255,0,255))
	{
			 if(pImage == NULL) return;
			_cornerRgn.DestroyRgns();
			_stretch.rcOuter	= xgdi::CImgStreRect(pImage, CRect(0,0,0,0));
			_stretch.rcInner	= xgdi::CImgStreRect(pImage, &innerRect);
			_cornerRgn			= XGDIUtil::CreateStretchRgn(_stretch, pImage, clrKey);
	}
	void SetMiniSize(int cx, int cy)
	{
			_szMini.cx = cx;
			_szMini.cy = cy;
	}
	void SetMaxSize(int cx, int cy)
	{
			_szMax.cx = cx;
			_szMax.cy = cy;
	}

	void SetFixSize(BOOL bIsFixSize)
	{
			_isFixSize = bIsFixSize;
			if (_isFixSize)
			{
				((FrameWindow*)this)->ModifyStyle(WS_MAXIMIZEBOX, 0, 0);
				((FrameWindow*)this)->ModifyStyle(WS_MINIMIZEBOX, 0, 0);
				((FrameWindow*)this)->ModifyStyle(WS_SYSMENU, 0, 0);
			}
			else
			{
				((FrameWindow*)this)->ModifyStyle(0, WS_MAXIMIZEBOX, 0);
				((FrameWindow*)this)->ModifyStyle(0, WS_MINIMIZEBOX, 0);
				((FrameWindow*)this)->ModifyStyle(0, WS_SYSMENU, 0);
			}
	}

protected:
	CRect		_rcCaption;
	CSize		_szCorner;	
	CSize		_szMini;
	CSize		_szMax;
	xgdi::StretchRgn	_cornerRgn;
	xgdi::StretchImg	_stretch;

	BOOL		_isFixSize;
};

NAMESPACE_END(xctrl)