#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_GTBUGREPORTDLG_H__06610FB2_8BF1_4B33_A4F1_8DA347A115CA__INCLUDED_)
#define AFX_GTBUGREPORTDLG_H__06610FB2_8BF1_4B33_A4F1_8DA347A115CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GTBugReportDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportDlg dialog
#define WM_THREAD_MESSAGE WM_USER + 120

class CGTBugReportDlg : public CDialog
{
// Construction
private:
	CString m_strBugInfo;
	CImageList	m_ImageList;
	HCURSOR		m_hHander;
	HCURSOR		m_hArrow;
public:
	CGTBugReportDlg(CWnd* pParent = NULL);   // standard constructor
	void SetBugReportInfo(CString strInfo);
// Dialog Data
	//{{AFX_DATA(CGTBugReportDlg)
	enum { IDD = IDD_BUGREPORT_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGTBugReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGTBugReportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRestart();
	afx_msg void OnBnClickedBtnExit();
private:
public:
	CEdit m_wndBugInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTBUGREPORTDLG_H__06610FB2_8BF1_4B33_A4F1_8DA347A115CA__INCLUDED_)
