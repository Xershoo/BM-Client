

#include "StdAfx.h"
#include "Progress.h"
#include "skinloader.h"
#include "Message.h"

NAMESPACE_BEGIN(xctrl)

	CProgress::CProgress()
{
	m_pResLoader        = xskin::CResLoader::inst();
	m_pBackImageSet = NULL;
	m_pBtnIconImageSet = NULL;
	m_bTracking = FALSE;
	m_bVert = true;
	m_nButtonPos = 0;
	m_nSliderTotalLen = 100;
	m_nSetRealTimePos = 0;
	m_nSetPos = 0;
	m_lRealpx = 0;
	m_bDownCapture = false;
	m_nButtonStateIndex = 0;
}
CProgress::~CProgress()
{
}



LRESULT CProgress::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt = lParam;
	int nId = ::GetDlgCtrlID(m_hWnd);
	if(m_rcSliderBack.PtInRect(pt) || m_bDownCapture)
		::PostMessage(::GetParent(m_hWnd), WM_EDU_CHANGE_PROGRESSPOS, (WPARAM)m_nSetRealTimePos, (LPARAM)m_hWnd);
	m_bDownCapture = false;
	if(m_nButtonStateIndex != 0)
	{
		m_nButtonStateIndex = 0;
		InvalidateRect(m_rcButtonIcon);
	}
	return 0;
}

LRESULT CProgress::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPoint pt = lParam;
	if(m_rcButtonIcon.PtInRect(pt))
	{
		m_bDownCapture = true;
		m_nButtonStateIndex = 2;
		InvalidateRect(m_rcButtonIcon);
	}
	else if(m_rcSliderBack.PtInRect(pt))
	{
		CRect rc;
		GetClientRect(rc);
		float nFloat = 0.0;
		if(m_bVert)
		{
			int nHeight = m_rcButtonIcon.Height();
			nFloat = 1.0 - (float)pt.y / rc.bottom;
			m_rcButtonIcon.bottom = nHeight / 2 + pt.y;
			m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
			if(m_rcButtonIcon.bottom >= rc.bottom)
			{
				m_rcButtonIcon.bottom = rc.bottom;
				m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
				nFloat = 0.0;
			}
			if(m_rcButtonIcon.top <= 0)
			{
				m_rcButtonIcon.top = 0;
				m_rcButtonIcon.bottom = nHeight;
				nFloat = 1.0;
			}
			m_lRealpx = rc.bottom * nFloat;
		}
		else
		{
			int nWidth = m_rcButtonIcon.Width();
			nFloat = (float)(pt.x + nWidth/2) / rc.right;
			if (pt.x + nWidth/2 > m_rcSliderBack.Width())
				pt.x = m_rcSliderBack.Width();
			m_rcButtonIcon.left = pt.x - nWidth/2;
			m_rcButtonIcon.right = pt.x + nWidth/2;
			if (m_rcButtonIcon.left < rc.left)
			{
				m_rcButtonIcon.left = rc.left;
				m_rcButtonIcon.right = m_rcButtonIcon.left + nWidth;
				nFloat = 0.0;
			}
			if (m_rcButtonIcon.right > rc.right)
			{
				m_rcButtonIcon.left = rc.right-nWidth;
				m_rcButtonIcon.right = rc.right;
			}
			
			if(nFloat > 1.0)
				nFloat = 1.0;
			m_lRealpx = pt.x + nWidth/2;
		}
		m_nSetPos = m_nSliderTotalLen * nFloat;
		m_nSetRealTimePos = m_nSetPos;
		Invalidate();
	}

	return	 0;
}

void CProgress::OnSetFocus(CWindow wndOld)
{

}

void CProgress::OnKillFocus(CWindow wndFocus)
{
	
}

int  CProgress::OnCreate(LPCREATESTRUCT lpCreateStruct)
{	
	InitImage();
	SetMsgHandled(FALSE);
	return 0;
}

void CProgress::OnDestroy()
{

}

void CProgress::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tmt = {sizeof(TRACKMOUSEEVENT),
			TME_LEAVE, m_hWnd, 0};//跟踪鼠标离开消息
		if ( _TrackMouseEvent(&tmt) )//开始跟踪
			m_bTracking = TRUE;
	}

	if(m_bDownCapture)
	{
		float nFloat = 0;
		CRect rcUpdate(m_rcButtonIcon);
		if(m_bVert)
		{
			if(point.y < m_rcSliderBack.top || point.y > m_rcSliderBack.bottom)
				return;
			CRect rcOld(m_rcButtonIcon);
			nFloat = 1.0 - (float)point.y / m_rcSliderBack.bottom;
			m_lRealpx = point.y - m_rcSliderBack.top;
			int nHeigth = m_rcButtonIcon.Height();
			m_rcButtonIcon.top = point.y - nHeigth / 2;
			m_rcButtonIcon.bottom = m_rcButtonIcon.top + nHeigth;
			if(m_rcButtonIcon.bottom > m_rcSliderBack.bottom)
			{
				m_rcButtonIcon.top = m_rcSliderBack.bottom - nHeigth;
				m_rcButtonIcon.bottom = m_rcSliderBack.bottom;
				nFloat = 0.0;
			}
			if(m_rcButtonIcon.top < m_rcSliderBack.top)
			{
				m_rcButtonIcon.top = m_rcSliderBack.top;
				m_rcButtonIcon.bottom = m_rcButtonIcon.top + nHeigth;
				nFloat = 1.0;
			}
			m_nSetPos = m_nSliderTotalLen * nFloat;
			m_nSetRealTimePos = m_nSetPos;
			if(m_rcButtonIcon.bottom > rcOld.bottom)
			{
				rcUpdate.bottom = m_rcButtonIcon.bottom;
				rcUpdate.top = rcOld.top;
			}
			else
			{
				rcUpdate.bottom = rcOld.bottom;
				rcUpdate.top = m_rcButtonIcon.top;
			}
			/*int nHeight = m_rcButtonIcon.Height();
			m_rcButtonIcon.bottom = nHeight / 2 + point.y;
			m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
			if(m_rcButtonIcon.bottom > m_rcSliderBack.bottom)
			{
				m_rcButtonIcon.bottom = m_rcSliderBack.bottom;
				m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
			}
			if(m_rcButtonIcon.top < 0)
			{
				m_rcButtonIcon.top = 0;
				m_rcButtonIcon.bottom = nHeight;
			}*/
			//nFloat = 1.0 - (float)pt.y / rc.bottom;
			m_lRealpx = m_rcSliderBack.bottom - point.y;
		}
		else
		{
			if (point.x < m_rcSliderBack.left || point.x > m_rcSliderBack.right)
				return;
			CRect rcOld(m_rcButtonIcon);
			rcOld.left = 0;
			int nWidth = m_rcButtonIcon.Width();
			nFloat = (float)(point.x - m_rcSliderBack.left + nWidth/2)/m_rcSliderBack.Width();
			if(nFloat > 1.0)
				nFloat = 1.0;
			m_lRealpx = point.x - m_rcSliderBack.left + nWidth/2;
			m_rcButtonIcon.left = point.x - nWidth / 2;
			m_rcButtonIcon.right = m_rcButtonIcon.left + nWidth;
			if ( m_rcButtonIcon.right > m_rcSliderBack.right)
			{
				m_rcButtonIcon.left = m_rcSliderBack.right - nWidth;
				m_rcButtonIcon.right = m_rcSliderBack.right;
			}
			if (m_rcButtonIcon.left < m_rcSliderBack.left)
			{
				m_rcButtonIcon.left = m_rcSliderBack.left;
				m_rcButtonIcon.right = m_rcButtonIcon.left + nWidth;
				nFloat = 0.0;
			}
			m_nSetPos = m_nSliderTotalLen * nFloat;
			m_nSetRealTimePos = m_nSetPos;
			if (m_rcButtonIcon.right > rcOld.right)
			{
				rcUpdate.left = rcOld.left;
				rcUpdate.right = m_rcButtonIcon.right;
			}
			else
			{
				rcUpdate.right = rcOld.right;
				rcUpdate.left = m_rcButtonIcon.left;
			}
		}

		InvalidateRect(rcUpdate); 
		m_nSetPos = m_nSliderTotalLen * nFloat;
		m_nSetRealTimePos = m_nSetPos;
		::PostMessage(::GetParent(m_hWnd), WM_EDU_CHANGE_PROGRESSPOS, (WPARAM)m_nSetRealTimePos, (LPARAM)m_hWnd);

	}
	else
	{
		if(m_rcButtonIcon.PtInRect(point))
		{
			m_nButtonStateIndex = 1;
			InvalidateRect(m_rcButtonIcon);
		}
	}
}

void CProgress::OnMouseLeave()
{
	m_bTracking = FALSE;
	m_bDownCapture = false;
	if(m_nButtonStateIndex != 0)
	{
		m_nButtonStateIndex = 0;
		InvalidateRect(m_rcButtonIcon);
		int nId = ::GetDlgCtrlID(m_hWnd);
		::PostMessage(::GetParent(m_hWnd), WM_EDU_CHANGE_PROGRESSPOS, (WPARAM)m_nSetRealTimePos, (LPARAM)m_hWnd);
	}
}

void CProgress::Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
{
	// normal button
	int nIndex = 0;

	if(m_pSliderBackgroudImag)
	{
		CRect rcBack(0,0, 0,0);
		pCanvas->DrawImage(m_pSliderBackgroudImag, rcItem,  rcBack, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
	}

	if(m_pBackImageSet)
	{		
		if(xgdi::IIcon* pIcon = m_pBackImageSet->GetIcon(nIndex))
		{			
			CRect rcTemp = m_rcSliderBack;
			
			pCanvas->DrawIcon(pIcon, m_rcSliderBack,m_rcBackInner,   xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			if(m_nSliderTotalLen > 0 && m_nSetRealTimePos > 0)
			{
				float nFloat = (float)m_nSetRealTimePos / m_nSliderTotalLen;
				if(!m_bVert)
				{
					//rcTemp.right = rcItem.right * nFloat + rcItem.left;
					rcTemp.right = m_lRealpx + rcTemp.left;
					rcTemp.top = m_rcSliderBack.top;
					rcTemp.bottom = m_rcSliderBack.bottom;
				}
				else
				{
					//rcTemp.top = rcTemp.bottom - rcItem.bottom * nFloat;
					rcTemp.top = rcTemp.bottom - m_lRealpx;
					rcTemp.left = m_rcSliderBack.left;
					rcTemp.right = m_rcSliderBack.right;
				}
				nIndex += 1;
				pIcon = m_pBackImageSet->GetIcon(nIndex);

				pCanvas->DrawIcon(pIcon, rcTemp,m_rcBackInner,   xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}			
		}
	}

	if(m_pBtnIconImageSet)
	{
		if(xgdi::IIcon* pIcon = m_pBtnIconImageSet->GetIcon(m_nButtonStateIndex))
		{
			pCanvas->DrawIcon(pIcon, m_rcButtonIcon,m_rcIconInner, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
		}
	}
	::PostMessage(::GetParent(m_hWnd), WM_EDU_CHANGE_PROGRESSPOS, (WPARAM)m_nSetRealTimePos, (LPARAM)m_hWnd);

}

LRESULT CProgress::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	CUpdateRect rcUpdate(m_hWnd);
	CRect rcClient;
	GetClientRect(&rcClient);

	CPaintDC dc(m_hWnd);
	xgdi::XGDIObject<xgdi::ICanvas> spCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
	spCanvas->SetSize(rcClient.Size());
	spCanvas->Attach(dc.m_hDC);
	ParentDrawBkgrd(spCanvas, rcUpdate);

	this->Draw(spCanvas, rcClient, rcUpdate, 0);

	spCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
	spCanvas->Detach();

	return 0;
}

void CProgress::InitImage()
{
	m_pSliderBackgroudImag = (xgdi::IImage*)xskin::QueryUIObject(SKINID(progressbar.ProgressBackground)); 
	m_pBackImageSet = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(progressbar.ProgressDlgSet));  
	m_pBtnIconImageSet = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(progressbar.ProgressVertIcon)); //加载拖动按钮
	int nVer = xskin::QueryUIValue(SKINID(progressbar.IsVertical));
	m_bVert = nVer==1?true:false;

	if(m_pBtnIconImageSet)
	{
		if(m_bVert)
		{
			m_rcButtonIcon.left = 0;
			m_rcButtonIcon.right = 30;
			m_rcButtonIcon.top = m_rcSliderBack.Height();
			m_rcButtonIcon.bottom = m_rcSliderBack.Height()+10;
		}
		else
		{
			m_rcButtonIcon.right = 0;
			m_rcButtonIcon.bottom = 30;
			m_rcButtonIcon.left = 0;
		}
	}
	//m_bVert = False;

	CRect rcInner(0,0,0,0);
	CTextUtil::TextToRect(L"0,0,0,0", rcInner);
	m_rcBackInner = rcInner;

	CTextUtil::TextToRect(L"0,0,0,0", rcInner);
	m_rcIconInner = rcInner;

}

BOOL CProgress::ParseCss(IProperty* pProp)
{
	if(pProp == NULL) return FALSE;

	LPCTSTR proper;

	proper = pProp->GetProperty(_T("font"));
	if(proper)
	{

	}
	if(proper = pProp->GetProperty(_T("backimage")))
	{		
		m_pBackImageSet = m_pResLoader->LoadIconSet(proper);
	}

	if(proper = pProp->GetProperty(_T("iconimage")))
	{		
		m_pBtnIconImageSet = m_pResLoader->LoadIconSet(proper);
		if(m_pBtnIconImageSet)
		{
			m_rcButtonIcon.right = m_rcButtonIcon.left + m_pBtnIconImageSet->GetIconSize().cx;
			m_rcButtonIcon.bottom = m_rcButtonIcon.top + m_pBtnIconImageSet->GetIconSize().cy;
		}
	}

	if(proper = pProp->GetProperty(_T("slidertype")))
	{		
		if(_ttoi(proper) > 0)
			m_bVert = true;
	}

	CRect rcInner(4, 4, 4, 4);
	if(proper = pProp->GetProperty(_T("slider-inner")))
	{
		CTextUtil::TextToRect(proper, rcInner);
		m_rcBackInner = rcInner;
	}

	if(proper = pProp->GetProperty(_T("button-inner")))
	{
		CTextUtil::TextToRect(proper, rcInner);
		m_rcIconInner = rcInner;
	}

	return XCssParser::ParseCss(pProp);
}

void CProgress::OnSize(UINT nType, CSize size)
{
	CRect rc;
	GetClientRect(rc);
	m_rcSliderBack = rc;
	if(m_pBackImageSet)
	{
		if(!m_bVert)
		{
			
		}
		else
		{
			if(m_rcSliderBack.Width() > m_pBackImageSet->GetIconSize().cx)
			{
				int nH = m_rcSliderBack.Width() - m_pBackImageSet->GetIconSize().cx;
				m_rcSliderBack.right = m_rcSliderBack.right - nH / 2;
				m_rcSliderBack.left = m_rcSliderBack.right - m_pBackImageSet->GetIconSize().cx;
			}

		}
	}

	CalcBtnIconPos();
}

void CProgress::SetPos(int nPos)
{
	if (nPos < 0)
		return;
	if(nPos >= 100)
		m_nSetPos = 100;
	else
		m_nSetPos = nPos;
	m_nSetRealTimePos = m_nSetPos;
	if (m_bVert)
	{
		m_lRealpx = (long)((float)m_nSetPos/m_nSliderTotalLen * (float)m_rcSliderBack.Height());
	}
	else
	{
		m_lRealpx = (long)((float)m_nSetPos/m_nSliderTotalLen * (float)m_rcSliderBack.Width());
	}
	CalcBtnIconPos();
	InvalidateRect(m_rcButtonIcon);

}

int	 CProgress::GetPos()
{
	return m_nSetPos;
}

void CProgress::CalcBtnIconPos()
{
	if(m_nSliderTotalLen > 0)
	{
		float nFloat = 1.0 - (float)m_nSetPos / m_nSliderTotalLen;
		if(m_bVert)
		{
			int nHeigth = m_rcButtonIcon.Height();
			m_rcButtonIcon.top = m_rcSliderBack.Height() * nFloat - nHeigth / 2;
			m_rcButtonIcon.bottom = m_rcButtonIcon.top + nHeigth;
			if(m_rcButtonIcon.bottom > m_rcSliderBack.bottom)
			{
				m_rcButtonIcon.top = m_rcSliderBack.bottom - nHeigth;
				m_rcButtonIcon.bottom = m_rcSliderBack.bottom;
			}
			if(m_rcButtonIcon.top < m_rcSliderBack.top)
			{
				m_rcButtonIcon.top = m_rcSliderBack.top;
				m_rcButtonIcon.bottom = m_rcButtonIcon.top + nHeigth;
			}	
		}
		else
		{
			int nWidth = m_rcButtonIcon.Width();
			m_rcButtonIcon.left = m_lRealpx - nWidth/2;
			m_rcButtonIcon.right = m_lRealpx + nWidth/2;
		}
		CPoint pt;
		
		if (m_bVert)
		{
			if (m_lRealpx <= 0)
			{
				m_lRealpx = 1;
				pt.SetPoint(5,m_lRealpx);
			}
			else
				pt.SetPoint(5,m_rcSliderBack.Height()-m_lRealpx);
		}
		else{
			if (m_lRealpx >= m_rcSliderBack.Width())
			{
				m_lRealpx = m_rcSliderBack.Width()-1;
				pt.SetPoint(m_lRealpx,5);
			}
			else
				pt.SetPoint(m_lRealpx, 5);
		}

		Paint(MAKELPARAM(pt.x,pt.y));
	}
}

void CProgress::Paint(CPoint pt)
{
	if(m_rcButtonIcon.PtInRect(pt))
	{
		m_bDownCapture = true;
		m_nButtonStateIndex = 2;
		InvalidateRect(m_rcButtonIcon);
	}
	else if(m_rcSliderBack.PtInRect(pt))
	{
		CRect rc;
		GetClientRect(rc);
		float nFloat = 0.0;
		if(m_bVert)
		{
			int nHeight = m_rcButtonIcon.Height();
			nFloat = 1.0 - (float)pt.y / rc.bottom;
			m_rcButtonIcon.bottom = nHeight / 2 + pt.y;
			m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
			if(m_rcButtonIcon.bottom >= rc.bottom)
			{
				m_rcButtonIcon.bottom = rc.bottom;
				m_rcButtonIcon.top = m_rcButtonIcon.bottom - nHeight;
				nFloat = 0.0;
			}
			if(m_rcButtonIcon.top <= 0)
			{
				m_rcButtonIcon.top = 0;
				m_rcButtonIcon.bottom = nHeight;
				nFloat = 1.0;
			}
			m_lRealpx = rc.bottom * nFloat;
		}
		else
		{
			int nWidth = m_rcButtonIcon.Width();
			nFloat = (float)(pt.x + nWidth/2) / rc.right;
			if (pt.x + nWidth/2 > m_rcSliderBack.Width())
				pt.x = m_rcSliderBack.Width();
			m_rcButtonIcon.left = pt.x - nWidth/2;
			m_rcButtonIcon.right = pt.x + nWidth/2;
			if (m_rcButtonIcon.left < rc.left)
			{
				m_rcButtonIcon.left = rc.left;
				m_rcButtonIcon.right = m_rcButtonIcon.left + nWidth;
				nFloat = 0.0;
			}
			if (m_rcButtonIcon.right > rc.right)
			{
				m_rcButtonIcon.left = rc.right-nWidth;
				m_rcButtonIcon.right = rc.right;
			}

			if(nFloat > 1.0)
				nFloat = 1.0;
			m_lRealpx = pt.x + nWidth/2;
		}
		m_nSetPos = m_nSliderTotalLen * nFloat;
		m_nSetRealTimePos = m_nSetPos;
		Invalidate();
	}
}
void CProgress::SetRange(int nMax)
{
	if(nMax < 0)
		return;
	m_nSliderTotalLen = nMax;
}
void CProgress::SetPosNoMoveThumb(int nPos)
{
	if(nPos >= 0)
		m_nSetRealTimePos = nPos;
	CRect rc;
	GetClientRect(rc);
	if(m_bVert)
	{
		rc.left = m_rcButtonIcon.left;
		rc.right = m_rcButtonIcon.right;
		rc.top = m_rcButtonIcon.top + m_rcButtonIcon.Height() / 2;
	}
	else
	{
		rc.top = m_rcButtonIcon.top;
		rc.bottom = m_rcButtonIcon.bottom;
		rc.right = m_rcButtonIcon.right + m_rcButtonIcon.Width() /  2;
	}
	InvalidateRect(rc);
	//Invalidate();
}

NAMESPACE_END(xctrl)