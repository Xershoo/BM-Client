// GTBugReport.h : main header file for the GTBUGREPORT application
//

#if !defined(AFX_GTBUGREPORT_H__C735CD33_B2BB_41FF_BF2A_81BF1493F8FB__INCLUDED_)
#define AFX_GTBUGREPORT_H__C735CD33_B2BB_41FF_BF2A_81BF1493F8FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp:
// See GTBugReport.cpp for the implementation of this class
//
#include "GTBugReportDlg.h"
#include "HttpUploadFile.h"
#include "HttpDownloadUIInterface.h"


class CGTBugReportApp : public CWinApp,
						public CHttpDownloadUIInterface
{
public:
	void OnConnecting(LPCTSTR lpszServerAddr);
	void OnConnected();
	void OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes);
	void OnProgress(DWORD dwProgress, DWORD dwProgressMax,LPCTSTR lpszSaveFile);
	void OnDownloadComplete(DWORD nErrorCode, LPCTSTR lpszSaveFile);
	void OnRedirected(LPCTSTR lpszRedirectedAddr);
public:
	CGTBugReportApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGTBugReportApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CGTBugReportDlg*	m_pBugReportDlg;
	CHttpUploadFile*	m_pHttpUpload;
	CString				m_strFileName;
	CString				m_strEXE;
	BOOL				m_bSending;
protected:
	DWORD	GetFileVersion();
public:
	//{{AFX_MSG(CGTBugReportApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTBUGREPORT_H__C735CD33_B2BB_41FF_BF2A_81BF1493F8FB__INCLUDED_)
