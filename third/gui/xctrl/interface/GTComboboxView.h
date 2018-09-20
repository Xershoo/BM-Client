#pragma once

#include "xgdi/xgdidef.h"
#include "transwindow.h"
#include "xctrl.h"
#include "GTComboDropList.h"

NAMESPACE_BEGIN(xctrl)


class CGTComboBoxCtrlView  : public CWindowImpl<CGTComboBoxCtrlView, CWindow, CControlWinTraits>
	, public TransChildWindow<CGTComboBoxCtrlView>
	, public XCssParser
{
	typedef ATL::CString CString;
public:
	BEGIN_MSG_MAP(CGTComboBoxCtrlView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_KEYDOWN(OnChar)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDBClick)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_PAINT,                   OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,  OnEraseBkgnd)
		MESSAGE_HANDLER (MSG_COMBOBOX_CHANGESELECT, OnChangeSelect)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
	END_MSG_MAP()

	CGTComboBoxCtrlView()
	{
		m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));
		m_crFont            = (COLORREF)xskin::QueryUIValue(SKINID(TextColor));
		m_crFontDisabled    = (COLORREF)xskin::QueryUIValue(SKINID(TextColorDisabled));
		m_nEditLeft = xskin::QueryUIValue(SKINID(MyComboBox.EditNormalLeft));
		m_nEditRight = xskin::QueryUIValue(SKINID(MyComboBox.EditNormalRight));
		m_nEditBottom = xskin::QueryUIValue(SKINID(MyComboBox.EditNormalBottom));
		m_nEditTop = xskin::QueryUIValue(SKINID(MyComboBox.EditNormalTop));
		m_nEditHeight = xskin::QueryUIValue(SKINID(MyComboBox.EditHeight));

		m_nlistVScrollX = xskin::QueryUIValue(SKINID(MyComboBox.vscroll_point_x));
		m_nlistVScrollY = xskin::QueryUIValue(SKINID(MyComboBox.vscroll_point_y));;
		m_pListVScrollImageSet = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(MyComboBox.vscroll_image));
		m_pResLoader        = xskin::CResLoader::inst();
		m_bMouseOn = false;
		m_rcInner.bottom = 1;
		m_rcInner.left = 1;
		m_rcInner.top = 1;
		m_rcInner.right = 1;
		m_maxShowItemCount = 10;
		m_bTracking = FALSE;
	}
	~CGTComboBoxCtrlView()
	{
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

	int  OnCreate(LPCREATESTRUCT lpCreateStruct)
	{	
		m_editInput.Create( m_hWnd, rcDefault, NULL,  
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |WS_CLIPCHILDREN | ES_LEFT | ES_AUTOHSCROLL, 
			WS_EX_TRANSPARENT/*, ID_SEARCHBAR_EDIT*/ );

		// Parlace add for DropDownList style
		/*if(GetStyle() & CBS_DROPDOWNLIST)
		m_editInput.ShowWindow(SW_HIDE);
		else*/
		m_editInput.ShowWindow(SW_SHOW);
		//m_editInput.SendLBDownToParent(true);
		//m_editInput.SetForbidWheel(true);
		
		if(m_comboboxlist.Create(m_hWnd, rcDefault, NULL, WS_POPUP | WS_VISIBLE | TVS_FULLROWSELECT, 0)) 
		{ 
			if(m_pFont)
				m_comboboxlist.SetFont(m_pFont->GetUIHandle());
			m_comboboxlist.SetVScrollPoint(m_nlistVScrollX, m_nlistVScrollY);
			m_comboboxlist.SetVScrollIconSet(m_pListVScrollImageSet);
			m_comboboxlist.SetComboboxHwnd(m_hWnd);
			xctrl::ITreeViewCtrl* pFriendTreeView = m_comboboxlist.GetWindowLessObj();
			
			m_comboboxlist.ShowWindow(SW_HIDE);
		}

		UpdateLayout(TRUE);
		SetMsgHandled(FALSE);
		return 0;
	}
	
	void OnDestroy()
	{
		//m_comboboxlist.DestroyWindow();
	}
	void OnSize(UINT nType, CSize size)
	{
		UpdateLayout();
	}
	void UpdateLayout(BOOL bResizeBars = TRUE)
	{
		CRect rcClient;
		GetClientRect(&rcClient);

		CRect rcEdit(rcClient);
		rcEdit.left = rcClient.left + m_nEditLeft;
		rcEdit.right = rcClient.right - m_nEditRight;
		rcEdit.top = rcClient.top + m_nEditTop;
		rcEdit.bottom = rcClient.bottom - m_nEditBottom;
		if(rcEdit.Height() > m_nEditHeight)
		{
			rcEdit.top = rcClient.top + rcClient.Height() / 2 - m_nEditHeight / 2;
			rcEdit.bottom = rcEdit.top + m_nEditHeight;
		}

		m_editInput.SetWindowPos( NULL, &rcEdit, SWP_NOZORDER | SWP_NOACTIVATE );

		m_rcButtonDrop = rcClient;
		m_rcButtonDrop.left = m_rcButtonDrop.right - GetSystemMetrics(SM_CXHTHUMB);
	}
	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{


		xgdi::IFont* pFont= m_pFont;
		// normal button
		int nIndex = 0;
		// add disable status
		if(!this->IsWindowEnabled() && m_pImageSet->GetIconCount() > 3)
		{
			pCanvas->SetTextColor(m_crFontDisabled);
			nIndex = 3;
		}
		else
		{
			pCanvas->SetTextColor(m_crFont);
			if(m_bMouseOn)
			{
				nIndex = 1;
			}
			//pFont = m_pFontDisabled;
		}

		if(m_pImageSet)
		{
			if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
			{
				pCanvas->DrawIcon(pIcon, rcItem,m_rcInner, xgdi::DIF_ALPHA |  xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}
		}

		// Parlace add for DropDownList style
		/*
		if(GetStyle() & CBS_DROPDOWNLIST)
		{
			CRect rcClient;
			GetClientRect(&rcClient);
			rcClient.left += 5;
			rcClient.right -= (m_rcButtonDrop.Width() + 5);
			CString szTemp;
			m_editInput.GetWindowText(szTemp);
			pCanvas->DrawText(m_pFont, szTemp, &rcClient, DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
			//this->InvalidateRect(&rcClient);
		}
		*/
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		CUpdateRect rcUpdate(m_hWnd);
		CRect rcClient;
		GetClientRect(&rcClient);
	//	CClientRect rcClient(m_hWnd);

		CPaintDC dc(m_hWnd);

		xgdi::XGDIObject<xgdi::ICanvas> spCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
		spCanvas->SetSize(rcClient.Size());
		spCanvas->Attach(dc.m_hDC);

		ParentDrawBkgrd(spCanvas, rcUpdate);
		spCanvas->SetBkMode(TRANSPARENT);
		this->Draw(spCanvas, rcClient, rcUpdate, 0);

		spCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
		spCanvas->Detach();

		return 0;
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		if(pProp == NULL) return FALSE;

		LPCTSTR proper;

		proper = pProp->GetProperty(_T("font"));
		if(proper)
		{
			m_pFont = m_pResLoader->LoadFont(proper);
			m_editInput.SetFont(m_pFont->GetUIHandle(), FALSE);
		}

		if(proper = pProp->GetProperty(_T("image")))
		{
			m_pImageSet = m_pResLoader->LoadIconSet(proper);

		}

		if(proper = pProp->GetProperty(_T("droplistbk_image")))
		{
			xgdi::IIconSet* pImageSet = m_pResLoader->LoadIconSet(proper);
			m_comboboxlist.SetBkIconSet(pImageSet);
			m_comboboxlist.GetWindowLessObj()->SetFillAllClient(FALSE);
		}


		if(proper = pProp->GetProperty(_T("image-inner")))
		{
			CTextUtil::TextToRect(proper, m_rcInner);
		}

		if(proper = pProp->GetProperty(_T("dropbutoom_width")))
		{
			CTextUtil::TextToInt(proper, m_nEditRight);
		}

		if(proper = pProp->GetProperty(_T("drop_itemCount")))
		{
			CTextUtil::TextToInt(proper, m_maxShowItemCount);
		}

		return XCssParser::ParseCss(pProp);
	}

	void OnMouseMove(UINT nFlags, CPoint point)
	{
		
		if (!m_bTracking)
		{
			TRACKMOUSEEVENT tmt = {sizeof(TRACKMOUSEEVENT),
				TME_LEAVE, m_hWnd, 0};//跟踪鼠标离开消息
			if ( _TrackMouseEvent(&tmt) )//开始跟踪
				m_bTracking = TRUE;
		}
		
		
		if(!m_bMouseOn)
		{
			m_bMouseOn = true;
			Invalidate();
		}
	}

	void OnSetFocus(HWND hWnd)
	{
		m_editInput.SetFocus();
	}
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		HWND hFocus = (HWND)wParam;
		if(hFocus == m_comboboxlist.m_hWnd)
		{
			HWND hNowFocus = ::GetFocus();
			if(hNowFocus != GetParent())
			{
				m_comboboxlist.ShowWindow(SW_HIDE);		
				return 0;
			}
			POINT pt = {0};
			GetCursorPos(&pt);
			CRect rcWin;
			::GetWindowRect(m_hWnd, rcWin);
			rcWin.left = rcWin.right - m_rcButtonDrop.Width();
			if(!rcWin.PtInRect(pt))
			{
				m_comboboxlist.ShowWindow(SW_HIDE);		
				return 0;
			}
		}
		
		return 0;
	}
	void OnLButtonDBClick(UINT nFlags, CPoint point)
	{
		if((m_rcButtonDrop.PtInRect(point) || (HWND)nFlags == m_hWnd)
			&& (GetStyle() & CBS_DROPDOWNLIST))		// Parlace add for DropDownList style
		{
			if(m_comboboxlist.IsWindowVisible())
			{
				m_comboboxlist.ShowWindow(SW_HIDE);
			}
		}
	}
	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		OutputDebugString(L"combobox onlbuttondown\r\n");
		if((m_rcButtonDrop.PtInRect(point) || (HWND)nFlags == m_hWnd)
			&& (GetStyle() & CBS_DROPDOWNLIST))		// Parlace add for DropDownList style
		{
			if(m_comboboxlist.IsWindowVisible())
			{
				m_comboboxlist.ShowWindow(SW_HIDE);
			}
			else
			{
				updateWndRect();				
			}
		}
	}

	void updateWndRect() 
	{
		m_comboboxlist.ShowWindow(SW_SHOW);
		CRect rcWin;
		GetWindowRect(&rcWin);
		CRect rcDropList;
		m_comboboxlist.GetWindowRect(&rcDropList);
		int nCount = m_comboboxlist.GetItemCount();
		int nPerHeight = m_comboboxlist.GetItemHeight();
		if(nCount > m_maxShowItemCount)
			rcDropList.bottom = rcWin.bottom + m_maxShowItemCount*nPerHeight + 1;
		else
			rcDropList.bottom = rcWin.bottom + nCount*nPerHeight + 1;
		rcDropList.top = rcWin.bottom + 1;
		if(rcDropList.bottom == rcDropList.top)
			rcDropList.bottom = rcDropList.top + 2;
		rcDropList.left = rcWin.left;
		rcDropList.right = rcWin.right;

		m_comboboxlist.SetWindowPos( NULL, &rcDropList, SWP_NOZORDER | SWP_NOACTIVATE );
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
	}
	void OnMouseLeave()
	{
		m_bTracking = FALSE;
		/*if(m_bMouseOn)
		{
			m_bMouseOn = false;
			Invalidate();
		}*/
		POINT pt = {0};
		GetCursorPos(&pt);

		CRect rc;
		GetWindowRect(rc);
		bool bOnMouse = false;
		if(rc.PtInRect(pt))
		{
			bOnMouse = true;
		}
		if(m_bMouseOn != bOnMouse)
		{
			m_bMouseOn = bOnMouse;
			Invalidate();
		}

	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		CRect rectEdit;
		m_editInput.GetWindowRect(&rectEdit);
		if(rectEdit.PtInRect(pMsg->pt))
		{
			if(pMsg->message == WM_MOUSELEAVE)
				return TRUE;
			if(pMsg->message == WM_MOUSEMOVE)
			{
				OnMouseMove(0, pMsg->pt);
				return TRUE;
			}
			if(pMsg->message == WM_LBUTTONDOWN)
			{
				OnLButtonDown(0, pMsg->pt);
				return TRUE;
			}
		}
		return FALSE;
		//return __super::PreTranslateMessage(pMsg);
	}

	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if(nChar == VK_UP )
		{
			LPCTSTR pText = m_comboboxlist.GetPreItemText();
			if(pText)
			{
				m_editInput.SetWindowText(pText);
				SendSelectToParent();
			}
		}
		else if(nChar == VK_DOWN)
		{
			LPCTSTR pText = m_comboboxlist.GetNextItemText();
			if(pText)
			{
				m_editInput.SetWindowText(pText);
				SendSelectToParent();
			}
		}
	}

	LRESULT OnChangeSelect( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		m_comboboxlist.ShowWindow(SW_HIDE);
		ITreeItemHandler * pItem = (ITreeItemHandler*)wParam;
		ITreeItemHandler * pItemPre = (ITreeItemHandler*)lParam;
		if(pItem == NULL)
			return 0;
		if(pItem != pItemPre)
		{
			//选项没改变时，不发送消息
			WTL::CString strText = pItem->GetItemText();
			m_editInput.SetWindowText(strText);
			m_editInput.SetSel(0,0);
			m_editInput.SetFocus();
			
			SendSelectToParent();
		}
		//m_comboboxlist.Get

		if(GetStyle() & CBS_DROPDOWNLIST)
			Invalidate();	// Parlace add for DropDownList style
		return 0;
	}

	void SendSelectToParent()
	{
		WPARAM wSendParam = MAKEWPARAM(GetDlgCtrlID(), CBN_SELCHANGE);
		::PostMessage(GetParent(), WM_COMMAND, wSendParam, 0);
	}

	void ShowCommonMenu()
	{
	}
	void GetSize(CSize &szTab);

	int InsertString(ITreeItemHandler *pIHandler)
	{
		xctrl::ITreeViewCtrl* pFriendTreeView = m_comboboxlist.GetWindowLessObj();
		if (pFriendTreeView)
		{	
			pFriendTreeView->InsertItem(pIHandler, NULL, NULL);

		}
		return 0;
	}

	ITreeItem* GetItemByString(ATL::CString itemText)
	{
		xctrl::ITreeViewCtrl* pFriendTreeView = m_comboboxlist.GetWindowLessObj();
		ATLASSERT(pFriendTreeView);
		ITreeItem* pItem = pFriendTreeView->FindItem(itemText, NULL);
		if (pItem)
		{
			return pItem;
		}
		/*ITreeItem* pItem = pFriendTreeView->GetFirstChild(NULL);
		int pos = 0;
		while(pItem)
		{
			if (ATL::CString(pItem->GetHandler()->GetItemText()) == itemText)
			{
				return pItem;
			}
			pos++;
			pItem = pFriendTreeView->GetNextSibling(pItem);
		}*/
		return NULL;
	}

	int SelectString(int nIndex, LPCTSTR pStr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(m_editInput));
		if(m_comboboxlist.SelectString(nIndex, pStr) >= 0)
		{
			m_editInput.SetWindowText(pStr);
			m_editInput.SetSel(0,0);
			hideDroplist();

		}
		return 0;
	}

	void hideDroplist() 
	{
		m_editInput.SetFocus();
		m_editInput.SetSelAll();
	}

	void SetCulSelByItemData(int nData)
	{
		xctrl::ITreeViewCtrl* m_spTreeView = m_comboboxlist.GetWindowLessObj();
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return ;
		ITreeItem *p = m_spTreeView->GetFirstChild(proot);
		while(p)
		{			
			ITreeItemHandler* pItemData = p->GetHandler();
			if(pItemData && pItemData->GetItemData() == nData)
			{
				m_spTreeView->Select(p, TRUE);
				if(::IsWindow(m_editInput))
				{
					m_editInput.SetWindowText(pItemData->GetItemText());
					m_editInput.SetSel(0,0);
				}
				return;
			}
			p = m_spTreeView->GetNext(p);
		}
		
	}

	int	 InsertString(int nIndex, LPCTSTR pStr)
	{
		return 0;
	}
	
	int	 GetCount()
	{
		return m_comboboxlist.GetItemCount();
		return 0;
	}
	int  DeleteString(int nPos)
	{
		xctrl::ITreeViewCtrl* pFriendTreeView = m_comboboxlist.GetWindowLessObj();
		ATLASSERT(pFriendTreeView);
		ITreeItem* pItem = pFriendTreeView->GetFirstChild(NULL);
		while(pItem && nPos--)
		{
			pItem = pFriendTreeView->GetNextSibling(pItem);
		}
		if(pItem != NULL)
			return pFriendTreeView->DeleteItem(pItem);
		else
			return 0;
	}

	int  DeleteString(ATL::CString itemText)
	{
		xctrl::ITreeViewCtrl* pFriendTreeView = m_comboboxlist.GetWindowLessObj();
		ATLASSERT(pFriendTreeView);
		ITreeItem* pItem = pFriendTreeView->GetFirstChild(NULL);
		int pos = 0;
		while(pItem)
		{
			if (ATL::CString(pItem->GetHandler()->GetItemText()) == itemText)
			{
				return DeleteString(pos);
			}
			pos++;
			pItem = pFriendTreeView->GetNextSibling(pItem);
		}

		return 0;
	}

	void	 GetText(CString& str)
	{
		if(::IsWindow(m_editInput))
			m_editInput.GetWindowText(str);
	}
	void SetWindowText(LPCTSTR pText)
	{
		if(pText == NULL)
			return;
		if(::IsWindow(m_editInput))
		{
			m_editInput.SetWindowText(pText);
			m_editInput.SetSel(0,0);
		}
		if(GetStyle() & CBS_DROPDOWNLIST)
			Invalidate();	// Parlace add
	}
	void GetWindowText(CString& strText)
	{
		if(::IsWindow(m_editInput))
			m_editInput.GetWindowText(strText);
	}

	void SetFont(HFONT hFont, BOOL bRedraw = TRUE) throw()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(m_editInput));
		m_editInput.SetFont(hFont, bRedraw);
		m_comboboxlist.SetFont(hFont);
		::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(bRedraw, 0));
	}

	int GetCurSel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return m_comboboxlist.GetCurSel();
		
	}

	//
	int SetCurSel(int nSelect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		int n = m_comboboxlist.SetCurSel(nSelect);
		if(n >= 0 && ::IsWindow(m_editInput))
		{
			m_editInput.SetWindowText(m_comboboxlist.GetCurrentItemText());
			m_editInput.SetSel(0,0);
			SendSelectToParent();
		}
		
		if(GetStyle() & CBS_DROPDOWNLIST)
			Invalidate();	// Parlace add
		return n;
	}

	int GetLBTextLen(int nLen)
	{
		return m_comboboxlist.GetLBTextLen(nLen);
	}
	
	int GetLBText(int nIndex, LPTSTR lpszText) const
	{
		return m_comboboxlist.GetLBText(nIndex, lpszText);
	}

	//删除所有记录
	void ResetContent()
	{
		xctrl::ITreeViewCtrl* pTree = m_comboboxlist.GetWindowLessObj();
		if(pTree)
			pTree->Clear();
	}

private:
	HBRUSH m_hBrush;
	BOOL m_bTracking;
	bool m_bMouseOn;
	int  m_nMouseMove;
	int		m_nMoveOnIndex;
	int		m_nDownIndex;
	int		m_nEditHeight;

	bool	m_bMoveOnCbx;

	//	CComboBoxListView		m_comboboxlist;

	xgdi::IFont*            m_pFont;
	COLORREF                m_crFont;
	COLORREF                m_crFontDisabled;
	xgdi::IIconSet*         m_pImageSet;
	int						m_nEditLeft;
	int						m_nEditRight;
	int						m_nEditTop;
	int						m_nEditBottom;

	int						m_nlistVScrollX;
	int						m_nlistVScrollY;
	xgdi::IIconSet*         m_pListVScrollImageSet;
	CRect					m_rcButtonDrop;
	CRect					m_rcInner;	
	int						m_maxShowItemCount;
public:
	xlayout::CWTLayoutWindow<xctrl::CWTLTreeCtrlEx>  m_comboboxlist;
	CRichEditCtrl				m_editInput;
};

NAMESPACE_END(xctrl)