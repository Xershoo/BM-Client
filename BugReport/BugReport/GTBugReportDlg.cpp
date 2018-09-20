// GTBugReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GTBugReport.h"
#include "GTBugReportDlg.h"
#include <stdio.h>
#include <stdlib.h>
#include "common/Config.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportDlg dialog


CGTBugReportDlg::CGTBugReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGTBugReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGTBugReportDlg)
	//}}AFX_DATA_INIT
	m_strBugInfo = _T("");
	m_hHander = NULL;
	m_hArrow = NULL;
}


void CGTBugReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGTBugReportDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_BUGINFO, m_wndBugInfo);
}


BEGIN_MESSAGE_MAP(CGTBugReportDlg, CDialog)
	//{{AFX_MSG_MAP(CGTBugReportDlg)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RESTART, OnBnClickedBtnRestart)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBnClickedBtnExit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportDlg message handlers
void CGTBugReportDlg::SetBugReportInfo(CString strInfo)
{
	m_strBugInfo = strInfo;
}

void CGTBugReportDlg::OnBnClickedBtnRestart()
{
	CWinApp* pApp = AfxGetApp();
	if(pApp)
		pApp->PostThreadMessage(WM_THREAD_MESSAGE,0,0);
}

void CGTBugReportDlg::OnBnClickedBtnExit()
{
	CWinApp* pApp = AfxGetApp();
	if(pApp)
		pApp->PostThreadMessage(WM_THREAD_MESSAGE,1,0);
}

void CGTBugReportDlg::OnClose() 
{
	CWinApp* pApp = AfxGetApp();
	if(pApp)
		pApp->PostThreadMessage(WM_THREAD_MESSAGE,1,0);
}

void CGTBugReportDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient,rc;
	GetClientRect(&rcClient);

	CString strText;
	CFont* pFont = NULL;
	LOGFONT LogFont;
	CFont font;
	CWnd* pWnd = GetDlgItem(IDC_STATIC_LOCATE);
	if(pWnd)
	{
		pWnd->GetWindowRect(&rc);
		pWnd->GetWindowText(strText);
		ScreenToClient(&rc);
		dc.Draw3dRect(&rc,RGB(231,230,227),RGB(231,230,227));
		rc.DeflateRect(1,1);
		dc.FillSolidRect(&rc,RGB(255,204,0));

		if(m_ImageList.GetSafeHandle())
		{
			int nX = rc.left + 12;
			int nY = (rc.Height() - 40) / 2 + rc.top;
			m_ImageList.Draw(&dc,0,CPoint(nX,nY),ILD_TRANSPARENT);
		}

		rc.left += 60;
		pFont = pWnd->GetFont();
		if(pFont)
		{
			pFont->GetLogFont(&LogFont);
			CString strFont;
			strFont.LoadString(IDS_FONT);
			LogFont.lfWeight = FW_BOLD;
			LogFont.lfHeight = -MulDiv(12, GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY), 72);
			font.DeleteObject();
			if(font.CreateFontIndirect(&LogFont))
			{
				CFont* pF = (CFont*)dc.SelectObject(&font);
				COLORREF crOld = dc.SetTextColor(RGB(48,48,48));
				int nBKMode = dc.SetBkMode(TRANSPARENT);
				dc.DrawText(strText,&rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
				dc.SetBkMode(nBKMode);
				dc.SetBkColor(crOld);
				dc.SelectObject(pF);
			}
		}

		

	}

	pWnd = GetDlgItem(IDC_STATIC_COPY_REPORT);
	if(pWnd)
	{
		pWnd->GetWindowText(strText);
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		pFont = pWnd->GetFont();
		if(pFont)
		{
			pFont->GetLogFont(&LogFont);
			LogFont.lfUnderline = TRUE;
			font.DeleteObject();
			if(font.CreateFontIndirect(&LogFont))
			{
				CFont* pF = (CFont*)dc.SelectObject(&font);
				COLORREF crOld = dc.SetTextColor(RGB(0,0,255));
				int nBKMode = dc.SetBkMode(TRANSPARENT);
				dc.DrawText(strText,&rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
				dc.SetBkMode(nBKMode);
				dc.SetBkColor(crOld);
				dc.SelectObject(pF);
			}
		}
	}
	
	pWnd = GetDlgItem(IDC_STATIC_GOTO_BBS);
	if(pWnd)
	{
		pWnd->GetWindowText(strText);
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		pFont = pWnd->GetFont();
		if(pFont)
		{
			pFont->GetLogFont(&LogFont);
			LogFont.lfUnderline = TRUE;
			font.DeleteObject();
			if(font.CreateFontIndirect(&LogFont))
			{
				CFont* pF = (CFont*)dc.SelectObject(&font);
				COLORREF crOld = dc.SetTextColor(RGB(0,0,255));
				int nBKMode = dc.SetBkMode(TRANSPARENT);
				dc.DrawText(strText,&rc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
				dc.SetBkMode(nBKMode);
				dc.SetBkColor(crOld);
				dc.SelectObject(pF);
			}
		}
	}	

}

BOOL CGTBugReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(hIcon,TRUE);

	m_hHander = AfxGetApp()->LoadCursor(IDC_HANDER);
	m_hArrow = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));

	CBitmap objBitmap;
	if(objBitmap.LoadBitmap(IDB_PROMPT))
	{
		if(m_ImageList.Create(40,40,ILC_COLOR24|ILC_MASK,1,1))
			m_ImageList.Add(&objBitmap,RGB(255,0,255));
	}

	int nLength = m_strBugInfo.GetLength();
	m_wndBugInfo.SetLimitText(nLength + 10);
	m_wndBugInfo.SetWindowText(m_strBugInfo);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGTBugReportDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);

	CWnd* pWnd1 = GetDlgItem(IDC_STATIC_COPY_REPORT);
	CWnd* pWnd2 = GetDlgItem(IDC_STATIC_GOTO_BBS);
	if(pWnd1 && pWnd2)
	{
		CRect rc1,rc2;
		pWnd1->GetWindowRect(&rc1);
		pWnd2->GetWindowRect(&rc2);
		ScreenToClient(&rc1);
		ScreenToClient(&rc2);

		if(rc1.PtInRect(point) || rc2.PtInRect(point))
		{
			SetCapture();
			SetCursor(m_hHander);
		}
		else
		{
			ReleaseCapture();
			SetCursor(m_hArrow);
		}
	}
}

void CGTBugReportDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CDialog::OnLButtonDown(nFlags, point);

	CWnd* pWnd1 = GetDlgItem(IDC_STATIC_COPY_REPORT);
	CWnd* pWnd2 = GetDlgItem(IDC_STATIC_GOTO_BBS);
	if(pWnd1 && pWnd2)
	{
		CRect rc1,rc2;
		pWnd1->GetWindowRect(&rc1);
		pWnd2->GetWindowRect(&rc2);
		ScreenToClient(&rc1);
		ScreenToClient(&rc2);
		
		if(rc1.PtInRect(point))
		{
			int nStart = 0;
			int nEnd = -1;
            m_wndBugInfo.GetSel(nStart,nEnd);
            m_wndBugInfo.SetSel(0, -1);
			m_wndBugInfo.Copy();
			m_wndBugInfo.SetSel(nStart,nEnd);
		}
		else if(rc2.PtInRect(point))
		{
			//ÂÛÌ³µØÖ·
			::ShellExecute(NULL,"open",Config::getConfig()->m_urlBugBBS.c_str(),NULL,NULL,SW_SHOW);
		}
	}
}

HBRUSH CGTBugReportDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd && pWnd->GetDlgCtrlID() == IDC_EDIT_BUGINFO)
		pDC->SetBkColor(RGB(255,255,255));
	return hbr;
}
