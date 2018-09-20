
#pragma once

#include "xgdi/xgdidef.h"
#include "transwindow.h"
#include "xctrl.h"
#include <atlcrack.h>
#include "cssparser.h"

NAMESPACE_BEGIN(xctrl)

class CProgress  : public CWindowImpl<CProgress, CWindow, CControlWinTraits>
	, public TransChildWindow<CProgress>
	, public XCssParser
{

	typedef ATL::CString CString;
public:


	BEGIN_MSG_MAP(CSetSlider)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		//MSG_WM_MOUSEMOVE(OnMouseMove)
		//MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_SIZE(OnSize)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//MESSAGE_HANDLER(WM_LBUTTONUP,   OnLButtonUp)
		//MESSAGE_HANDLER(WM_LBUTTONDOWN,   OnLButtonDown)
	END_MSG_MAP()

	CProgress();
	~CProgress();

	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	

	void OnSetFocus(CWindow wndOld);
	void OnKillFocus(CWindow wndFocus);

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual BOOL ParseCss(IProperty* pProp);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();

	void SetPos(int nPos);
	void SetRange(int nMax);
	void SetPosNoMoveThumb(int nPos);
	int	 GetPos();
private:
	void CalcBtnIconPos();
	void InitImage();
	void Paint(CPoint pt);
private:
	HBRUSH m_hBrush;
	BOOL m_bTracking;
	bool					m_bVert;
	xgdi::IIconSet*         m_pBackImageSet;	
	CRect					m_rcBackInner;
	xgdi::IIconSet*         m_pBtnIconImageSet;	
	CRect					m_rcIconInner;
	CRect					m_rcButtonIcon;
	int						m_nButtonPos;
	int						m_nSliderTotalLen;
	int						m_nSetPos;
	int						m_nSetRealTimePos;
	long					m_lRealpx;
	bool					m_bDownCapture;
	int						m_nButtonStateIndex;
	CRect					m_rcSliderBack;

	xgdi::IImage*			m_pSliderBackgroudImag;
};

NAMESPACE_END(xctrl)
