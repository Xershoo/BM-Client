#pragma once

#include "xgdi/xgdidef.h"
#include "transwindow.h"
#include "xctrl.h"
#include <wtltree.h>
#include <atlcrack.h>



NAMESPACE_BEGIN(xctrl)


class CWTLTreeCtrlEx
    : public CWTLTreeCtrl
{
public:
	DECLARE_WND_CLASS(_T("CWTLTreeCtrlEx"))

    BEGIN_MSG_MAP_EX(CWTLTreeCtrl)
		MSG_WM_KILLFOCUS	( OnKillFocus )		
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP( CWTLTreeCtrl )
	END_MSG_MAP()

	CWTLTreeCtrlEx()
	{
		m_pDropListBk = NULL;
		m_hComboboxHwnd = NULL;
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

		if(m_pDropListBk)
			spCanvas->DrawIcon(m_pDropListBk->GetIcon(0), rcClient, CRect(4,4,4,4), xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
		//rcClient.DeflateRect(m_rcBord);
		m_spTreeView->Paint(spCanvas, rcUpdate, rcUpdate);
		spCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
		spCanvas->Detach();

		return 0;
	}
	
	int  GetItemCount()
	{
		if(m_spTreeView)
			return m_spTreeView->GetVisibleCount();
		return 0;
	}

	void SetBkIconSet(xgdi::IIconSet* pIcon)
	{
		m_pDropListBk = pIcon;
	}

	void SetBordRect(LPRECT rc)
	{
		if(rc == NULL)
			return;
		m_rcBord = rc;
	}

	LPCTSTR GetCurrentItemText()
	{
		if(m_spTreeView == NULL)
			return NULL;
		ITreeItem* pNow = m_spTreeView->GetCurrentSel();
		if(pNow)
		{
			if(ITreeItemHandler* pSelectHandle = pNow->GetHandler())
			{					
				return pSelectHandle->GetItemText();
			}				
		}
		return NULL;
	}

	//得到列表每项的高度
	int GetItemHeight()
	{		
		if(m_spTreeView == NULL)
			return 0;
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return 0;
		ITreeItem* pFirstChild = m_spTreeView->GetFirstChild(proot);
		if(pFirstChild == NULL)
			return 0;
		CRect rc;
		pFirstChild->GetRect(rc);
		return rc.Height();

	}

	LPCTSTR GetNextItemText()
	{
		if(m_spTreeView == NULL)
			return NULL;
		ITreeItem* pNow = m_spTreeView->GetCurrentSel();
		ITreeItemHandler* pSelectHandle = NULL;
		if(pNow == NULL)
		{
			pNow = m_spTreeView->GetFirstChild(m_spTreeView->GetRootItem());
			if(pNow)
			{
				pSelectHandle = pNow->GetHandler();
				m_spTreeView->Select(pNow, TRUE);
				if(pSelectHandle)
				{					
					return pSelectHandle->GetItemText();
				}				
			}

			return NULL;

		}
		
		ITreeItem* pNext = m_spTreeView->GetNext(pNow);
		
		if(pNext)
		{
			pSelectHandle = pNext->GetHandler();
			m_spTreeView->Select(pNext, TRUE);
			if(pSelectHandle)
				return pSelectHandle->GetItemText();
		}		
		return NULL;
	}

	int GetCurSel()
	{
		if(m_spTreeView == NULL)
			return -1;
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return -1;
		 int i = 0;
		 for (ITreeItem *p = m_spTreeView->GetFirstChild(proot); p != NULL; p = m_spTreeView->GetNext(p)) 
		 {
            if(p->GetItemSelected())
				return i;
			i++;
        }

		return -1;
	}

	int SetCurSel(int nSelect)
	{
		if(nSelect < 0)
			return -1;
		if(m_spTreeView == NULL)
			return -1;
		
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return -1;

		int i = 0;

		ITreeItem *p = m_spTreeView->GetFirstChild(proot);
		bool bFind = false;
		for (; p != NULL; p = m_spTreeView->GetNext(p)) 
		{
			if(i == nSelect)
			{
				bFind = true;
				break;
			}
			i++;

		}
		if(bFind)
		{
			m_spTreeView->Select(p, TRUE);
			return i;
		}


		return -1;
			
	}

	int GetLBTextLen(int nSel)
	{
		if(nSel < 0)
			return 0;
		if(m_spTreeView == NULL)
			return -1;
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return -1;
		int i = 0;
		ITreeItem *p = m_spTreeView->GetFirstChild(proot);
		bool bFind = false;
		for (; p != NULL; p = m_spTreeView->GetNext(p)) 
		{
			if(i == nSel)
			{
				bFind = true;
				break;
			}
			i++;

		}
		if(bFind)
		{
			ITreeItemHandler* pSelectHandle  = p->GetHandler();
			if(pSelectHandle)
			{
				ATL::CString strText = pSelectHandle->GetItemText();
				return strText.GetLength();
			}
			else
				return 0;
		}
			
		return 0;
	}

	int SelectString(int nIndex, LPCTSTR pStr)
	{
		if(pStr == NULL)
			return -1;

		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return -1;
		int i = 0;
		ITreeItem *p = m_spTreeView->GetFirstChild(proot);
		while(i < nIndex && p)
		{
			i++;
			p = m_spTreeView->GetNext(p);
		}
		if(i < nIndex)
			return -1;
		while(p)
		{
			ATL::CString strText = p->GetHandler()->GetItemText();
			if(strText == pStr)
				return i;
			p = m_spTreeView->GetNext(p);
			i++;
		}
		return -1;
	}

	int GetLBText(int nIndex, LPTSTR lpszText) const
	{
		if(nIndex < 0 || lpszText == NULL)
			return 0;

		if(m_spTreeView == NULL)
			return 0;
		ITreeItem* proot = m_spTreeView->GetRootItem();
		if(proot == NULL)
			return 0;
		int i = 0;
		ITreeItem *p = m_spTreeView->GetFirstChild(proot);
		bool bFind = false;
		for (; p != NULL; p = m_spTreeView->GetNext(p)) 
		{
			if(i == nIndex)
			{
				bFind = true;
				break;
			}
			i++;

		}
		if(bFind)
		{
			ITreeItemHandler* pSelectHandle  = p->GetHandler();
			if(pSelectHandle)
			{
				ATL::CString strText = pSelectHandle->GetItemText();
				_tcscpy(lpszText, strText);
				return strText.GetLength();
			}
			else
				return 0;
		}

		return 0;
	}

	LPCTSTR GetPreItemText()
	{
		if(m_spTreeView == NULL)
			return NULL;

		ITreeItem* pNow = m_spTreeView->GetCurrentSel();
		ITreeItemHandler* pSelectHandle = NULL;
		if(pNow == NULL)
		{
			pNow = m_spTreeView->GetFirstChild(m_spTreeView->GetRootItem());
			if(pNow)
			{
				pSelectHandle = pNow->GetHandler();
				m_spTreeView->Select(pNow, TRUE);
				if(pSelectHandle)
				{					
					return pSelectHandle->GetItemText();
				}				
			}

			return NULL;

		}
		
		ITreeItem* pNext = m_spTreeView->GetPrev(pNow);
		
		if(pNext)
		{
			m_spTreeView->Select(pNext, TRUE);
			pSelectHandle = pNext->GetHandler();
			if(pSelectHandle)
				return pSelectHandle->GetItemText();
			
		}
		 
		return NULL;
	}

	void SetComboboxHwnd(HWND hWnd)
	{
		m_hComboboxHwnd = hWnd;
	}

	void	OnKillFocus(CWindow wndFocus)
	{
		if(m_hComboboxHwnd)
		{
			::SendMessage(m_hComboboxHwnd, WM_COMMAND, (WPARAM)m_hWnd, 0);
		}
		else
			ShowWindow(SW_HIDE);
		OutputDebugString(L"combodroplist OnKillFocus\r\n");
		//	GetParent().SetFocus();
	}

	protected:
		xgdi::IIconSet	*	m_pDropListBk;
		HWND				m_hComboboxHwnd;
};

NAMESPACE_END(xctrl)