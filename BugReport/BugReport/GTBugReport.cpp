// GTBugReport.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GTBugReport.h"
#include <afxcoll.h>
#include "common/Config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp

BEGIN_MESSAGE_MAP(CGTBugReportApp, CWinApp)
	//{{AFX_MSG_MAP(CGTBugReportApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp construction
CGTBugReportApp::CGTBugReportApp()
{
	m_pBugReportDlg = NULL;
	m_pHttpUpload = NULL;
	m_bSending = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CGTBugReportApp object

CGTBugReportApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp initialization

BOOL CGTBugReportApp::InitInstance()
{
	if (!AfxSocketInit())
	{		
		return FALSE;
	}

    Config::getConfig();

	// Standard initialization
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CString strCommand = ::GetCommandLine();
	CString strEXE = "",strMD5 = "",strModuleName = "",strFileName = "",m_strEXE,strMiniDumpFile;
	DWORD dwClientVersion = 0;
	int nLength = 0;
	int nMode = 0;
	
	int nPos1 = 0;
	int nPos = strCommand.Find('\"',0);
	while(nPos != -1)
	{
		nPos1 = nPos + 1;
		nPos = strCommand.Find('\"',nPos1);
		if(nPos != -1)
			strEXE = strCommand.Mid(nPos1,nPos - nPos1);
		else
			break;
		
		nPos1 = nPos + 2;
		nPos = strCommand.Find(' ',nPos1);
		if(nPos != -1)
			nMode = atoi(strCommand.Mid(nPos1,nPos - nPos1));
		else
			break;

		nPos1 = nPos + 1;
		nPos = strCommand.Find(' ',nPos1);
		if(nPos != -1)
			strModuleName = strCommand.Mid(nPos1,nPos - nPos1);
		else
			break;

		nPos1 = nPos + 1;
		nPos = strCommand.Find(' ',nPos1);
		if(nPos != -1)
			dwClientVersion = atoi(strCommand.Mid(nPos1,nPos - nPos1));
		else
			break;

		nPos1 = nPos + 1;
		nPos = strCommand.Find(' ',nPos1);
		if(nPos != -1)
			strMD5 = strCommand.Mid(nPos1,nPos - nPos1);
		else
			break;

		nPos1 = nPos + 1;
		nPos = strCommand.Find(' ',nPos1);
		if(nPos != -1)
			nLength = atoi(strCommand.Mid(nPos1,nPos - nPos1));
		else
			break;

		nPos1 = nPos + 2;
		nPos = strCommand.Find('\"',nPos1);
		if(nPos != -1)
			strFileName = strCommand.Mid(nPos1,nPos - nPos1);
		else
			break;

		nPos1 = nPos + 3;
		nPos = strCommand.Find('\"',nPos1);
		if(nPos != -1)
			m_strEXE = strCommand.Mid(nPos1,nPos - nPos1);
		else
			break;

		nPos1 = nPos + 3;
		nPos = strCommand.Find('\"',nPos1);
		if(nPos != -1)
			strMiniDumpFile = strCommand.Mid(nPos1,nPos - nPos1);

		break;
	}

	if(strMD5 == "" || strModuleName == "" || dwClientVersion == 0 || strFileName == "" || nLength <= 0 || m_strEXE == "" || strMiniDumpFile == "")
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	BOOL bFlag = FALSE;
	CFile FF;
	CString strDebugInfo;
	if(FF.Open(strFileName,CFile::modeReadWrite|CFile::typeBinary))
	{
		if(nLength == (int)FF.GetLength())
		{
			BYTE* pBuffer = new BYTE[nLength + 1];//预留一个'\0'字符
			memset(pBuffer,0,nLength + 1);
			FF.Seek(0,CFile::begin);
			FF.Read(pBuffer,nLength);
			strDebugInfo = pBuffer;
			delete pBuffer;
			bFlag = TRUE;
		}
		FF.Close();
	}
	CFile::Remove(strFileName);

	if(!bFlag)
	{
		
		CFile::Remove(strMiniDumpFile);
		return FALSE;
	}

	//upload data to server
	m_pHttpUpload = new CHttpUploadFile;
	m_pHttpUpload->SetUIInterface(this);
	m_pHttpUpload->SetFileSizeLimit(1024 * 1024);
	m_pHttpUpload->ResetFormData();

	m_bSending = FALSE;
	//上传BUG信息 
    //http://bugreport.wanmei.com/gt/fileuploadservlet 
    //http://10.5.33.64:8080/eduup/up/log_file

    CString strUrl(Config::getConfig()->m_urlBugReport.c_str());
	UINT nCode = m_pHttpUpload->UploadFile("myfile",strUrl,strMiniDumpFile);
	if(nCode == 0) //error
	{
		delete m_pHttpUpload;
		m_pHttpUpload = NULL;
	}
	else
	{
		m_bSending = TRUE;
		m_strFileName = strMiniDumpFile;
	}	

	m_pBugReportDlg = new CGTBugReportDlg;
	m_pBugReportDlg->SetBugReportInfo(strDebugInfo);
	if(m_pBugReportDlg->Create(CGTBugReportDlg::IDD,NULL))
	{
		CString strTitle,s1;
		m_pBugReportDlg->GetWindowText(strTitle);
		s1.Format("%s %s",strModuleName,strTitle);
		m_pBugReportDlg->SetWindowText(s1);
		m_pMainWnd = m_pBugReportDlg;
		m_pBugReportDlg->CenterWindow(CWnd::GetDesktopWindow());
		m_pBugReportDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		delete m_pBugReportDlg;
		m_pBugReportDlg = NULL;
	}

	MSG msg;
	while(::GetMessage(&msg, 0, 0, 0))
	{
		if(msg.message == WM_THREAD_MESSAGE)
		{
			if(msg.wParam == 0 && !m_strEXE.IsEmpty())
				::ShellExecute(NULL,"open",m_strEXE,NULL,NULL,SW_SHOW);

			if(m_pBugReportDlg)
			{
				m_pBugReportDlg->ShowWindow(SW_HIDE);
				m_pBugReportDlg->DestroyWindow();
				delete m_pBugReportDlg;
				m_pBugReportDlg = NULL;
			}
	
			if(!m_bSending)
				::PostQuitMessage(0);
		}
		TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	if(m_pBugReportDlg)
	{
		if(::IsWindow(m_pBugReportDlg->GetSafeHwnd()))
			m_pBugReportDlg->DestroyWindow();
		delete m_pBugReportDlg;
		m_pBugReportDlg = NULL;
		m_pMainWnd = NULL;
	}

	if(m_pHttpUpload)
	{
		m_pHttpUpload->CancelUpload();
		delete m_pHttpUpload;
		m_pHttpUpload = NULL;
	}

    Config::freeConfig();

	return FALSE;
}

DWORD CGTBugReportApp::GetFileVersion()
{
	DWORD dwVer = 0;
	BYTE chMainVer = 0,chSubVer = 0,chExVer = 0;
	DWORD dwBuildNO = 0;
	TCHAR chFileName[1024];
	memset(chFileName,0,sizeof(chFileName));
	DWORD dwLen = ::GetModuleFileName(NULL,chFileName,1024);
	if(dwLen > 0 && dwLen < 1024)	
	{
		chFileName[dwLen] = '\0';
		dwLen = ::GetFileVersionInfoSize(chFileName,NULL);
		if(dwLen != 0)
		{
			BYTE* pBuffer = new BYTE[dwLen];
			if(pBuffer)
			{
				if(::GetFileVersionInfo(chFileName,NULL,dwLen,pBuffer))
				{
					VS_FIXEDFILEINFO* lpFileInfo = NULL;
					UINT nLen = 0;
					if(::VerQueryValue(pBuffer,TEXT("\\"),(void**)&lpFileInfo,&nLen))
					{
						DWORD dwNumber = 65536;
						chMainVer = (BYTE)(lpFileInfo->dwFileVersionMS / dwNumber);
						chSubVer = (BYTE)(lpFileInfo->dwFileVersionMS % dwNumber);
						
						chExVer = (BYTE)(lpFileInfo->dwFileVersionLS / dwNumber);
						dwBuildNO = lpFileInfo->dwFileVersionLS % dwNumber;

						dwVer = chMainVer * 100 + chSubVer;	
					}
				}
				delete []pBuffer;
			}
		}
	}
	return dwVer;
}

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp message handlers

int CGTBugReportApp::ExitInstance() 
{
	if(m_pBugReportDlg)
	{
		if(::IsWindow(m_pBugReportDlg->GetSafeHwnd()))
			m_pBugReportDlg->DestroyWindow();
		delete m_pBugReportDlg;
		m_pBugReportDlg = NULL;
		m_pMainWnd = NULL;
	}

	if(m_pHttpUpload)
	{
		m_pHttpUpload->CancelUpload();
		delete m_pHttpUpload;
		m_pHttpUpload = NULL;
	}

	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportApp message handlers
void CGTBugReportApp::OnConnecting(LPCTSTR lpszServerAddr)
{
}

void CGTBugReportApp::OnConnected()
{
}

void CGTBugReportApp::OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes)
{
}

void CGTBugReportApp::OnProgress(DWORD dwProgress, DWORD dwProgressMax,LPCTSTR lpszSaveFile)
{
}

void CGTBugReportApp::OnDownloadComplete(DWORD nErrorCode, LPCTSTR lpszSaveFile)
{
	m_bSending = FALSE;

	if(!m_strFileName.IsEmpty())
	{
		CFile::Remove(m_strFileName);
		m_strFileName = "";
	}

	if(m_pBugReportDlg == NULL)
		::PostQuitMessage(0);
}

void CGTBugReportApp::OnRedirected(LPCTSTR lpszRedirectedAddr)
{
}



