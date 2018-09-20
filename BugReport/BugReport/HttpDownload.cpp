 // -------------------------------------------------------------------------
//	�ļ���		��	HttpDownload.cpp
//	������		��	yuyu
//	����ʱ��	��	2004-2-13 10:00:00
//	��������	��	����Http Server���ļ�
//			
//	
//			֧�ֶϵ�����
//			֧�ֶ��ʧ������
//			֧���ض�������
//			֧��˫����, �ӳ�д��
//			֧�ֶ���ļ�ͬʱ����
//
//-------------------------------------------------------------------------
// �޸ļ�¼
//
// Mal 041122 ~ �޸�QueryInfo������ά��ԭ�Ĵ�Сд״̬���أ���������������
//              ��Ϊð�ŲŴ���
//            + ����QueryInfo�����ط������ɣ�֧�ַֽ��ȡString(Trim֧��)��
//              DWORD��BOOL��YES/NO����DecodedBuffer
//            ~ �޸�ʹ�÷�DLL�汾�ı���꣬��_FOR_AUTOUPDATE_��Ϊ_STANDALONE_USE_
//
// Mal 041221 ~ ���CHttpUploadFile����Ҫ��OnProgress������Ϊvirtual���Ա���Upload
//              ����ʾSendRequest�Ľ��ȡ�
//            + ���ӱ������Ƶ�LOG��
//
// Mal 050315 ~ ��̬��������Buffer�Ĵ�С������˵�server�������һ�����ݰ�
//				ʱ���û����FIN��ǽ��������ȵ���ʱ�����⡣
//			  ~ ����������Ŀ���ļ���С������ԣ����޷�����ļ���Сʱ���趨Ϊ-1
//				(0xFFFFFFFF)ԭ�趨Ϊ����dwInetFileSizeΪ�Σ�������dwCurrentPos��
//				�������ͼ����ʱ���ϴ˵ȱ�̬��server����dwFileSize����Խ��
//				0xFFFFFFFF���һ����Ŀ���ļ�С��ֵ��
//
// Mal 051103 + �Զ���HTTP���������֧�֣�����ȫ��Ӱ�켰ʵ��Ӱ��ģʽ��
//			  ~ ȥ����FileOperation�����������õ��ĺ������˹������������ָ����
//				���ھۡ�
//	
// Mal 051108 + ��FlashGet��Ĭ������������Ϊ�����޸ġ�ԭĬ��Ϊ����IE���ã��ָ�Ϊ��QQ
//				ʹ�ÿ�֧�ֵ���������ʱ������ʹ��FlashGet���������á�Ŀǰ���������
//				��FlashGet�������б���ѡ�����ϣ������ɿ�����ηָ
//

#include "stdafx.h"
#include "HttpDownload.h"

#include "wininet.h"
#include "WinInetUtilFunc.h"
#include "HttpDownloadUIProxy.h"

// Log����
#define HTTP_LOG

// ���httpdownload��������ַ���
		
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHttpDownload::CThisMapPtrToPtr CHttpDownload::m_thisMap;

DWORD	CHttpDownload::s_dwAccessType		= INTERNET_OPEN_TYPE_PRECONFIG;
CString	CHttpDownload::s_strProxyIP			= _T("");
WORD	CHttpDownload::s_wProxyPort			= 0;
BOOL	CHttpDownload::s_bProxyAuthentic	= FALSE;	
CString	CHttpDownload::s_strProxyUserName	= _T("");
CString	CHttpDownload::s_strProxyPassword	= _T("");

CHttpDownload::CHttpDownload()
{
	m_bAutoAddCookie = TRUE;
	m_hUIHeap = NULL;
	m_hwndUIProxy = NULL;
	m_pUIInterface = NULL;
	m_hInetSession = NULL;
	m_dwReceiveBufferLen = 32 * 1024;
	m_bSupportResume = TRUE;
	m_ReceiveBuffer = NULL;
	
	m_hHttpConnect = NULL;
	m_hHttpFile = NULL;
	m_hSaveFileHandle = INVALID_HANDLE_VALUE;
	m_bForceDownload = TRUE;
	m_nRetryTimes = 0;
	m_nMaxRetryTime = 3;

	m_bUseWriteBuffer = TRUE;
	m_dwWriteBufferOffset = NULL;
	m_pWriteBuffer = NULL;
	m_dwWriteBufferLen = m_dwReceiveBufferLen * 16;

	m_dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;

	m_bAutoResume = TRUE;

	m_chEventMask = EVENT_ALL;

	m_bUsePostVerb = FALSE;

	m_bIfModified = FALSE;
			
	m_dwError = 0;	

	m_strProxyIP = _T("");
	m_wProxyPort = 0;
	m_bProxyAuthentic = FALSE;
	m_strProxyUserName = _T("");
	m_strProxyPassword = _T("");
	
	m_bUseGlobalNetworkSettings = TRUE;

	m_bPopAuthenticDlg = TRUE;



	// ��ʼ��Event
	m_hEvtRequestComplete	=	::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEvtHandleCreated		=	::CreateEvent(NULL, TRUE, FALSE, NULL);

	// �����̼߳���
	m_hEvtWakeUpThread		=	::CreateEvent(NULL, TRUE, FALSE, NULL);

	// ��ʱ����
	m_hEvtUIExit			=	::CreateEvent(NULL, TRUE, FALSE, NULL);	

	CThreadModel::AddEvent(m_hEvtRequestComplete);
	CThreadModel::AddEvent(m_hEvtHandleCreated);
	CThreadModel::AddEvent(m_hEvtWakeUpThread);
	

	m_bReuseThread = FALSE;	

	memset(&m_buffersOut,0,sizeof(INTERNET_BUFFERS));
	memset(&m_tLastModified,0,sizeof(SYSTEMTIME));

	m_dwStatusCode = 0; //kongdong added
	
}


CHttpDownload::~CHttpDownload()
{
	CancelDownload();

	CloseHandle(m_hEvtUIExit);
}


void CHttpDownload::OnMessageError(LPCSTR lpszMsg)
{
	TRACE1("CHttpDownload::ERROR ---> %s\n", lpszMsg);
}

void CHttpDownload::SetDownloadTempDir(LPCTSTR strDir)
{
	m_strDownloadTempDir = strDir;
	if (m_strDownloadTempDir.Right(1) != "\\")
		m_strDownloadTempDir += "\\";
	
	if (!CheckDirectoryExist(m_strDownloadTempDir))
		CreateAllDirectory(m_strDownloadTempDir);

}


CString &CHttpDownload::GetDownloadTempDir()
{
	return m_strDownloadTempDir;
}

/// ���������͵ķ�ʽ
void CHttpDownload::SetRequestMethod(int nMethod)
{
	if (REQUEST_METHOD_GET == nMethod)
	{
		m_bUsePostVerb = FALSE;
	}
	else if (REQUEST_METHOD_POST == nMethod)
	{
		m_bUsePostVerb = TRUE;
	}
	else
	{
		ASSERT(FALSE);
	}
}


void CHttpDownload::CancelDownload()
{
	CThreadModel::StopThread();
	DeleteUIProxy();
}

// dwResumeBytes = 0�� ���ϵ�
//				 > 0:	ָ����ʼ���صĶϵ�λ��
UINT CHttpDownload::DownloadResume(LPCTSTR lpszDownloadUrl, LPCTSTR lpszSaveFile, DWORD dwResumeBytes, BOOL bForceDownloadWithResumeError /*= TRUE*/)
{
	if (!bForceDownloadWithResumeError)
	{	
		// check file size		
		if (dwResumeBytes != 0)
		{
			HANDLE hSaveFileHandle = ::CreateFile(m_DownloadFileInfo.m_strSavePath,           // 
					GENERIC_READ,			   // open for writing 
					FILE_SHARE_READ,           // share for reading 
					NULL,                      // no security 
					OPEN_EXISTING,             // existing file only 
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
					NULL);  

			if (hSaveFileHandle == INVALID_HANDLE_VALUE)
				return -1;
			
			DWORD dwFileSize = 0;
			GetFileCurrentPos(hSaveFileHandle, dwFileSize);

			::CloseHandle(hSaveFileHandle);	

			if (dwResumeBytes != dwFileSize)
				return -1;				
		}
	}
	m_DownloadFileInfo.bForceDownloadWithResumeError = bForceDownloadWithResumeError;
	m_DownloadFileInfo.dwRequestResumeBytes = dwResumeBytes;
	return DownloadBase(lpszDownloadUrl, lpszSaveFile);
}

UINT CHttpDownload::Download(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile, SYSTEMTIME *tFileTime /*= NULL*/)
{
	m_DownloadFileInfo.bForceDownloadWithResumeError = TRUE;
	m_DownloadFileInfo.dwRequestResumeBytes = 0;
	if(tFileTime != NULL)
	{	
		m_DownloadFileInfo.m_bRequestModified = TRUE;
		m_DownloadFileInfo.m_tIfModifiedSince = *tFileTime;
	}
	return DownloadBase(lpszDownloadUrl, lpszSaveFile);
}

UINT CHttpDownload::DownloadBase(LPCTSTR lpszDownloadUrl,LPCTSTR lpszSaveFile)
{
	ASSERT(lpszDownloadUrl != NULL && AfxIsValidString(lpszDownloadUrl));
	ASSERT(lpszSaveFile != NULL && AfxIsValidString(lpszSaveFile));

	// parse the  URL
	CString	strUserName;
	CString strPassword;
	CString strServer;
	CString strObject;
	DWORD	dwServerType;
	INTERNET_PORT	nPort;

	CancelDownload();
	
#ifdef _DEBUG
	if(!m_bReuseThread)
		ASSERT(m_hThread == NULL);
#endif

	if(AfxParseURLEx(lpszDownloadUrl, dwServerType, strServer, strObject, nPort, strUserName, strPassword, ICU_NO_ENCODE) == FALSE 
					|| dwServerType != AFX_INET_SERVICE_HTTP)
	{
		TRACE0("CHttpDownload::URL����http����\n");
		return 0;
	}

	//
	// check the savePath
	//////////////////////////////////////////////////////////////////////////	
	CString strSavePath =  lpszSaveFile;
	strSavePath.TrimLeft();
	strSavePath.TrimRight();
	
	if( strSavePath.IsEmpty())
		return 0;

	if (!CreateUIProxy()) 
		return 0;

	m_DownloadFileInfo.m_strDownloadURL = lpszDownloadUrl;
	m_DownloadFileInfo.m_strSavePath = lpszSaveFile;
	m_DownloadFileInfo.nErrorCode = ERRORCODE_NULL;
	m_DownloadFileInfo.dwDownloadedSize = 0;
	m_DownloadFileInfo.dwFileSize = 0;
	m_DownloadFileInfo.nStatus = DOWNLOAD_STATUS_PREPAREING;
		
	m_hSaveFileHandle = INVALID_HANDLE_VALUE; //hFile;
	
	if (m_bReuseThread && m_hThread != NULL)
	{
		return ::SetEvent(m_hEvtWakeUpThread); 	// �������߳�
	}
	else
	{
		return CThreadModel::StartThread();
	}
}


void CHttpDownload::BeforeThread()
{	
	CThreadModel::BeforeThread();
}

void CHttpDownload::AfterThread()
{
	// ���ﲻ��ɾ��
	//DeleteUIProxy();
	ReleaseReceiveBuffer();
 	CThreadModel::AfterThread();
}

void CHttpDownload::InitMemberData()
{
	::ResetEvent(m_hEvtRequestComplete);
	::ResetEvent(m_hEvtHandleCreated);
	
	m_nRetryTimes = 0;
	m_dwError = 0;	
	m_strResponseHeader.Empty();
	m_bIfModified = FALSE;
	m_strResponseFileName.Empty();
}

UINT CHttpDownload::Run()
{
resueThread:
	InitMemberData();
	UINT nResultCode = DOWNLOAD_RESULT_FAILED;
	while(nResultCode != DOWNLOAD_RESULT_OK && nResultCode != DOWNLOAD_RESULT_EXIT 	&& 
		  nResultCode != DOWNLOAD_RESULT_ABORT && nResultCode != DOWNLOAD_RESULT_USER_ABORT)
	{					
		nResultCode = InitDownload();
		if (nResultCode != DOWNLOAD_RESULT_CONTINUE)
			break;			
		nResultCode = DownloadRun();
		ExitDownload();
	}
	// �������ͷ��
	ClearRequestHeader();

	if (nResultCode == DOWNLOAD_RESULT_OK)
	{
		OnDownloadComplete(m_DownloadFileInfo.nErrorCode);
	}
	else
	{
		if (m_DownloadFileInfo.nErrorCode == ERRORCODE_NULL)
			m_DownloadFileInfo.nErrorCode = ERRORCODE_GENERAL;
		OnError(m_DownloadFileInfo.nErrorCode);
		OnDownloadComplete(m_DownloadFileInfo.nErrorCode);
	}

	// �����߳�
	if (m_bReuseThread)
	{
		HANDLE hEvt = NULL;
		if (!CThreadModel::QueryEvent(hEvt))
			if (hEvt == m_hEvtWakeUpThread)  // ���ü���
				goto resueThread;
	}

	return nResultCode;
}

UINT CHttpDownload::DownloadRun()
{
	BOOL	bForceResumeAtZero = FALSE; // ǿ�ƴ�0��ʼ���أ�����416����	

	UINT	nResultCode = 0;

	try
	{
INIT_HTTPFILE:

		OnConnecting();

		++m_nRetryTimes;
		HTTP_LOG("HTTP Retry Times: %d\n", m_nRetryTimes);

		if (m_nRetryTimes > m_nMaxRetryTime)
		{
			return DOWNLOAD_RESULT_EXIT;
		}
		
		nResultCode = OpenInternetFile();
		if (nResultCode != DOWNLOAD_RESULT_CONTINUE)
		{
			return nResultCode;
		}

		nResultCode = ApplyNetworkSettings();
		if (nResultCode != DOWNLOAD_RESULT_CONTINUE)
		{
			return nResultCode;
		}

		if (!OpenSaveFile())
			return DOWNLOAD_RESULT_FAILED;

		DWORD dwCurrentPos = 0;

		if (!GetFileCurrentPos(m_hSaveFileHandle, dwCurrentPos)) 
		{			
			CloseInternetFile();
			return DOWNLOAD_RESULT_FAILED;
		}

		// ����ϵ���ļ���С��һ��
		if ((m_DownloadFileInfo.dwRequestResumeBytes != 0) && (m_DownloadFileInfo.dwRequestResumeBytes != dwCurrentPos))
		{
			if (!m_DownloadFileInfo.bForceDownloadWithResumeError)  
			{
				// �ϵ����
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_FILESIZEERROR);
				CloseInternetFile();
				return DOWNLOAD_RESULT_FAILED;
			}
			else // �Զ������ϵ㣬 ��ͷ��ʼ����
			{
				dwCurrentPos = 0;			
			}
		}

		// �û�������ϵ�, �����ò��Զ�ȡ�ϵ㡣��ͷ��ʼ����
		if ((m_DownloadFileInfo.dwRequestResumeBytes == 0) && !m_bAutoResume)
		{
			dwCurrentPos = 0;
		}
		
		// �����ʹ��DownloadRequestModify�� ��ֻ����������
		if (m_DownloadFileInfo.m_bRequestModified)
			dwCurrentPos = 0;

		// �ϴ��յ�416�� ǿ�ƴ�0��ʼ����
		if(bForceResumeAtZero)
			dwCurrentPos = 0;

		// ����������ʼ��
		HttpPreSetFilePointer(dwCurrentPos);
		
		// ����Post����
		if (!m_strPostHeader.IsEmpty())
		{
			HttpSetExternalHeader(m_strPostHeader);
		}

		if (m_DownloadFileInfo.m_bRequestModified)
			HttpSetRequestModifiedHeader(m_DownloadFileInfo.m_tIfModifiedSince);
		
		// ������������
retry:
		HTTP_LOG("������������\n");
		nResultCode = HttpSendRequest();
		if (nResultCode != DOWNLOAD_RESULT_CONTINUE)
		{
			CloseInternetFile();
			return nResultCode;
		}
		
		HttpQueryResponseHeader();
		HttpHandleModifiedTime();

		///
		///  Get File Info
		///////////////////////////////////////////////
		CString strInetFileName;
		DWORD dwInetFileSize;
		SYSTEMTIME stInetFileLastModifiedTime;
	
		// ��ѯInternet�ļ���Ϣ
		QueryInternetFileInfo(strInetFileName, dwInetFileSize, stInetFileLastModifiedTime);
		m_DownloadFileInfo.dwFileSize = dwInetFileSize;
		m_strResponseFileName = strInetFileName;
		//////////////////////////////////

		OnConnected();
		
		// ��ѯ״̬��
		DWORD dwStatusCode = HttpQueryInfoStatusCode();

		//kongdong added,���������
		m_dwStatusCode = dwStatusCode;

		HTTP_LOG("HttpQueryInfoStatusCode: %d\n", dwStatusCode);

		switch(dwStatusCode) 
		{
		case HTTP_STATUS_DENIED :
		case HTTP_STATUS_PROXY_AUTH_REQ:
			{
				if (m_bPopAuthenticDlg)
				{
					if (!OnGetProxyUserPassword())
					{
						return DOWNLOAD_RESULT_ABORT;	
					}
				}
				else
				{
					return DOWNLOAD_RESULT_ABORT;	
				}

				goto retry;
			}
		case HTTP_STATUS_OK:							// 200
			{
				nResultCode = OnHttpStatusOK(dwCurrentPos);
				CloseSaveFile();
				CloseInternetFile();

				if (nResultCode == DOWNLOAD_RESULT_TRY) 
				{
					dwCurrentPos = 0;
					goto INIT_HTTPFILE;
				}
				else
				{
					return nResultCode;
				}
			}
		case HTTP_STATUS_NOT_MODIFIED:						// 304
			{
				// �ļ�û�иı䣬 ֱ�ӷ���
				SetStatusAndErrorCode(DOWNLOAD_STATUS_COMPLETE, ERRORCODE_NOMODIFIED);
				CloseSaveFile();
				CloseInternetFile();				
				return DOWNLOAD_RESULT_OK;
			}
			break;

		case HTTP_STATUS_REDIRECT:						// 302
		case HTTP_STATUS_REDIRECT_METHOD:				// 303
		case HTTP_STATUS_REDIRECT_KEEP_VERB:			// 307
			{
				nResultCode = OnHttpStatusRedirect(dwCurrentPos);
				CloseInternetFile();
				// reset 
				m_nRetryTimes = 0;  
				return nResultCode;

			}
		case HTTP_STATUS_PARTIAL_CONTENT:				// 206
			{
				nResultCode = OnHttpStatusPartialContent(dwCurrentPos);
				CloseSaveFile();
				CloseInternetFile();				

				if (nResultCode == DOWNLOAD_RESULT_TRY) 
				{
					goto INIT_HTTPFILE;
				}
				else
				{
					return nResultCode;
				}
				
			}
		case HTTP_STATUS_NOT_FOUND:	//404 ��������
			{
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_FILENOFOUND);
				CloseSaveFile();
				CloseInternetFile();
				return DOWNLOAD_RESULT_ABORT;
			}
		case HTTP_STATUS_SERVICE_UNAVAIL:	// 503
			{   // service unavailable, no Try!
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_SERVERUNAVAIL);
				CloseSaveFile();
				CloseInternetFile();
				return DOWNLOAD_RESULT_ABORT;
			}
		case 416: //HTTP 416 Request Range Not Satisfiable 
			{
				if (bForceResumeAtZero == FALSE)  // ��һ�γ���416
				{				
					SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);

					// retry from postion 0
					CloseSaveFile();
					CloseInternetFile();
				
					bForceResumeAtZero = TRUE;	// ǿ�ƴ�0��ʼ���أ�����
					goto INIT_HTTPFILE;	
				}
				else	// �ڶ�������ֻ��ʧ�ܡ���������ֹ��ѭ��������
				{
					SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);
					CloseSaveFile();
					CloseInternetFile();
					return DOWNLOAD_RESULT_ABORT;
				}
			}
		default:
			{
				// �����״̬�룬����DL_RESULT_FAIL����������
				nResultCode = OnHttpStatusOther(dwStatusCode);
				OnMessageError(_T(""));
				CloseInternetFile();
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);
				return nResultCode;
			}
		}
	}
	catch (CInternetException* pEx) 
	{
		CString strErrorMessage;
		pEx->GetErrorMessage(strErrorMessage.GetBuffer(CHAR_MAX + 1), CHAR_MAX + 1);
		strErrorMessage.ReleaseBuffer();
		TRACE(_T("------> ERROR : Catch CInternetException: %s\n"), strErrorMessage);
		HTTP_LOG("Caught CInternetException: %d(%s)\n", pEx->m_dwError, strErrorMessage);
		
		OnMessageError(strErrorMessage);

		if (pEx->m_dwError == 0x00002ee7)	// �޷���������������ʧ��
		{
			pEx->Delete();
			CloseInternetFile();
			SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_NETWORKFAILED);
			return 1;
		}
		pEx->Delete();
	}
	catch (CException* pEx) 
	{
		CString strErrorMessage;
		pEx->GetErrorMessage(strErrorMessage.GetBuffer(CHAR_MAX + 1), CHAR_MAX + 1);
		strErrorMessage.ReleaseBuffer();
		TRACE(_T("------> ERROR : Catch CException: %s\n"), strErrorMessage);
		HTTP_LOG("Caught CException: %s", strErrorMessage);
		OnMessageError(strErrorMessage);
		pEx->Delete();
		SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);
	}

	CloseInternetFile();
	return 1;
}



UINT CHttpDownload::InitDownload()
{
	try
	{
		// 1.�����Ự
		if (!OpenInternetSession()) 
		{
			TRACE0("CHttpDownload::�����Ựʧ��\n");
			return DOWNLOAD_RESULT_FAILED;
		}

		UINT nResultCode = 0;

		// 2.��������
		nResultCode = OpenInternetConnection();
		if (nResultCode != DOWNLOAD_RESULT_CONTINUE) 
		{
			return nResultCode;
		}

		return DOWNLOAD_RESULT_CONTINUE;
	}
	catch (CInternetException* pEx) 
	{
		CString strErrorMessage;
		pEx->GetErrorMessage(strErrorMessage.GetBuffer(CHAR_MAX + 1), CHAR_MAX + 1);
		strErrorMessage.ReleaseBuffer();
		TRACE(_T("CHttpDownload::------> ERROR : Catch CInternetException: %s\n"), strErrorMessage);
		pEx->Delete();
	}
	catch (CException* pEx) 
	{
		CString strErrorMessage;
		pEx->GetErrorMessage(strErrorMessage.GetBuffer(CHAR_MAX + 1), CHAR_MAX + 1);
		strErrorMessage.ReleaseBuffer();
		TRACE(_T("CHttpDownload:: ------> ERROR : Catch CException: %s\n"), strErrorMessage);		
		pEx->Delete();
	}
	
	return DOWNLOAD_RESULT_FAILED;
}


void CHttpDownload::ExitDownload()
{
	CloseSaveFile();

	CloseInternetFile();
	
	CloseInternetConnection();

	CloseInternetSession();
}



BOOL CHttpDownload::InitReceiveBuffer()
{
	if (m_ReceiveBuffer == NULL)
	{	
		m_ReceiveBuffer = new BYTE[m_dwReceiveBufferLen];
	}
	return  (m_ReceiveBuffer != NULL);
}


void CHttpDownload::ReleaseReceiveBuffer()
{
	delete [] m_ReceiveBuffer;
	m_ReceiveBuffer = NULL;
}


BOOL CHttpDownload::OpenInternetSession()
{
	if (m_hInetSession != NULL) 
	{
		return TRUE;
	}

	// ��ȡ�û�����
	CString strUserAgent;
	if (!GetUserAgent(strUserAgent)) 
	{
		return FALSE;
 	}
	
	TRACE(_T("------> ASYNC INET INFO: m_hInetSession = ::InternetOpen(...)\n"));
	
	m_hInetSession = ::InternetOpen(
		strUserAgent,								// lpszAgent
		m_bUseGlobalNetworkSettings?s_dwAccessType:m_dwAccessType,// dwAccessType
		NULL,										// lpszProxyName
		NULL,										// lpszProxyBypass
		INTERNET_FLAG_ASYNC);						// dwFlags
	
	if (m_hInetSession == NULL) 
	{
		AfxThrowInternetException(NULL);
	}
	
	// ����״̬�ص�
	INTERNET_STATUS_CALLBACK pRet = 
		::InternetSetStatusCallback(m_hInetSession, InternetStatusCallback);
	if (pRet == INTERNET_INVALID_STATUS_CALLBACK) 
	{
		AfxThrowInternetException(NULL);
	}
	m_thisMap.SetAt(this);

	// ����Internetѡ��
	if (!SetInternetOptions()) 
	{
		return FALSE;
	}

	return TRUE;
}


BOOL CHttpDownload::OpenSaveFile()
{
	// create file handle	
	m_hSaveFileHandle = CreateFile(m_DownloadFileInfo.m_strSavePath,           // 
                GENERIC_WRITE,              // open for writing 
                FILE_SHARE_READ,           // share for reading 
                NULL,                      // no security 
                OPEN_ALWAYS,             // existing file only 
                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,     // normal file 
                NULL);                     // no attr. template 
 
	if (m_hSaveFileHandle == INVALID_HANDLE_VALUE) 
	{ 
		SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_WRITEFAILED);
		TRACE0("CHttpDownload::�޷��򿪱����ļ�\n");
		return FALSE;
	}
	return TRUE;
}


void CHttpDownload::CloseSaveFile()
{
	// release write buffer
	if (m_pWriteBuffer != 0)
	{
		// flush to disk 
		FlushSaveFile();

		HANDLE hProcessHeap = ::GetProcessHeap();
		if (hProcessHeap)
		{
			::HeapFree(hProcessHeap, 0, m_pWriteBuffer);
		}
		m_pWriteBuffer = NULL;
	}
	if (m_hSaveFileHandle != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hSaveFileHandle);
		m_hSaveFileHandle = INVALID_HANDLE_VALUE;
	}
}


BOOL CHttpDownload::WriteSaveFile(LPVOID pBuffer, DWORD dwBytes)
{	
	// if fisrt in , create buffer;
	if (m_pWriteBuffer == 0 && m_bUseWriteBuffer)
	{
		// create buffer
		HANDLE hProcessHeap = ::GetProcessHeap();
		if (hProcessHeap)
		{		
			m_pWriteBuffer = ::HeapAlloc(hProcessHeap, 0, m_dwWriteBufferLen);
			if (m_pWriteBuffer == NULL)
				m_bUseWriteBuffer = FALSE;
		}
		else
			m_bUseWriteBuffer = FALSE;
	}

#ifdef _DEBUG
	if (m_bUseWriteBuffer == FALSE)
		TRACE("create heap memory error!\n");
#endif

	// no use buffer, just write to disk
	if (!m_bUseWriteBuffer)
	{
		DWORD dwWriteBytes = 0;

		int nResult = WriteFile(m_hSaveFileHandle, 
							pBuffer, 
							dwBytes, 
							&dwWriteBytes, 
			NULL);

		if (nResult == 0 || dwWriteBytes == 0 ||	// д��ʧ��
			dwBytes != dwWriteBytes					// û����ȫд��
			)
			return FALSE;
		
		return TRUE;
	}
	
	// buffer full. flush to file
	if (m_dwWriteBufferOffset + dwBytes > m_dwWriteBufferLen)
	{
		if (!FlushSaveFile())
			return FALSE;
	}
	
	// just save the buffer
	
	::CopyMemory( (BYTE*)m_pWriteBuffer + m_dwWriteBufferOffset, pBuffer, dwBytes);
	m_dwWriteBufferOffset += dwBytes;

	return TRUE;
}


BOOL CHttpDownload::FlushSaveFile()
{
	DWORD dwWriteBytes = 0;

	// no use buffer
	if (m_bUseWriteBuffer == NULL)
		return TRUE;

	// no data
	if (m_dwWriteBufferOffset == 0)
		return TRUE;

	int nResult = WriteFile(m_hSaveFileHandle, 
						m_pWriteBuffer, 
						m_dwWriteBufferOffset, 
						&dwWriteBytes, 
						NULL);						
		
	if (nResult == 0 || dwWriteBytes == 0 || 
			m_dwWriteBufferOffset != dwWriteBytes)
	{
		m_dwWriteBufferOffset = 0;
		return FALSE;
	}
	
	m_dwWriteBufferOffset = 0;

	TRACE("flush to disk\n");
	
	return TRUE;
}


void CHttpDownload::CloseInternetSession()
{
	if (m_hInetSession != NULL) 
	{
		TRACE(_T("------> ASYNC INET INFO: ::InternetSetStatusCallback(m_hInetSession, NULL)\n"));
		::InternetSetStatusCallback(m_hInetSession, NULL);
		m_thisMap.RemoveKey(this);
		TRACE(_T("------> ASYNC INET INFO: ::InternetCloseHandle(m_hInetSession)\n"));
		::InternetCloseHandle(m_hInetSession);
		m_hInetSession = NULL;
	}
}


UINT CHttpDownload::OpenInternetConnection()
{
	if (m_hInetSession == NULL) 
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}
	if (m_hHttpConnect != NULL) 
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}
	
	// ��ȡURL��Ϣ
	CString	strUserName;
	CString strPassword;
	CString strServer;
	CString strObject;
	DWORD	dwServerType;
	INTERNET_PORT	nPort;

	if(AfxParseURLEx(m_DownloadFileInfo.m_strDownloadURL, dwServerType, strServer, strObject, nPort, strUserName, strPassword, ICU_NO_ENCODE) == FALSE 
					|| dwServerType != AFX_INET_SERVICE_HTTP)
	{
		TRACE0("CHttpDownload::URL����http����\n");
		return DOWNLOAD_RESULT_FAILED;
	}

	m_strObject = strObject;
	m_strServer = strServer;
	
	m_hHttpConnect = ::InternetConnect(
		m_hInetSession,											// hInternet
		strServer,												// lpszServername
		nPort,													// nServerPort
		strUserName.IsEmpty() ? NULL : (LPCTSTR)strUserName,	// lpszUserName 
		strPassword.IsEmpty() ? NULL : (LPCTSTR)strPassword,	// lpszPassword
		INTERNET_SERVICE_HTTP,									// dwService
		0,														// dwFlags
		reinterpret_cast<DWORD_PTR>(this));						// dwContext

	if (m_hHttpConnect == NULL)
	{
		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_IO_PENDING) 
		{
			// �������Ľ��
			ASSERT(FALSE);
			AfxThrowInternetException(NULL, dwError);
		}
		
		HANDLE hEvt;
		if (CThreadModel::QueryEvent(hEvt))
		{
			return DOWNLOAD_RESULT_USER_ABORT;
		}
		if (hEvt == m_hEvtHandleCreated)
		{
			if (m_dwError != ERROR_SUCCESS) 
			{
				AfxThrowInternetException(NULL, m_dwError);
			}
			else
			{
				// Just keep go on
			}
		}
		else
		{
			// �������Ľ��
			ASSERT(FALSE);
			OnMessageError(_T(""));
			return DOWNLOAD_RESULT_FAILED;
		}	
	}
	::ResetEvent(m_hEvtHandleCreated);

	if (m_hHttpConnect == NULL) 
	{
		// �������Ľ��
		ASSERT(FALSE);
		OnMessageError(_T(""));
		return DOWNLOAD_RESULT_FAILED;
	}
	
	return DOWNLOAD_RESULT_CONTINUE;
}


UINT CHttpDownload::OpenInternetFile()
{
	if (m_hHttpConnect == NULL) 
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}
	if (m_hHttpFile != NULL) 
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}


	LPCTSTR ppstrAcceptTypes[2] = {_T("*/*"), NULL};
	
	TRACE(_T("------> ASYNC INET INFO: m_hHttpFile = ::HttpOpenRequest(...)\n"));
	

	CString strVerb;
	
	strVerb = (m_bUsePostVerb) ? _T("POST") : _T("GET");

	DWORD dwFlags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_PRAGMA_NOCACHE
		|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_AUTO_REDIRECT;
	if( !m_bAutoAddCookie )
		dwFlags |= INTERNET_FLAG_NO_COOKIES;
	m_hHttpFile = ::HttpOpenRequest(
		m_hHttpConnect,											// hConnect
		strVerb,												// lpszVerb
		m_strObject,											// lpszObjectName 
		_T("HTTP/1.1"),											// lpszVersion 
		m_strRef.IsEmpty() ? NULL : (LPCTSTR)m_strRef,			// lpszReferer
		ppstrAcceptTypes,										// lplpszAcceptTypes 
		dwFlags,                                                // dwFlags 
		reinterpret_cast<DWORD_PTR>(this));						// dwContext
	
	if (m_hHttpConnect == NULL)
	{
		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_IO_PENDING) 
		{
			// �������Ľ��
			ASSERT(FALSE);
			SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);
			AfxThrowInternetException(NULL, dwError);
		}

		HANDLE hEvt;
		if (CThreadModel::QueryEvent(hEvt))
		{
			return DOWNLOAD_RESULT_USER_ABORT;
		}
		if (hEvt == m_hEvtHandleCreated)
		{
			if (m_dwError != ERROR_SUCCESS) 
			{
				AfxThrowInternetException(NULL, m_dwError);
			}
			else
			{
				// Just keep go on
			}
		}
		else
		{
			// �������Ľ��
			ASSERT(FALSE);
			OnMessageError(_T(""));
			return DOWNLOAD_RESULT_FAILED;
		}
	}
	::ResetEvent(m_hEvtHandleCreated);


	if (m_hHttpFile == NULL) 
	{
		// �������Ľ��
		ASSERT(FALSE);
		OnMessageError(_T(""));
		SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_GENERAL);
		return DOWNLOAD_RESULT_FAILED;
	}
	
	return DOWNLOAD_RESULT_CONTINUE;
}


void CHttpDownload::CloseInternetFile()
{
	if (m_hHttpFile != NULL) 
	{
		TRACE(_T("------> ASYNC INET INFO: ::InternetCloseHandle(m_hHttpFile)\n"));
		::InternetCloseHandle(m_hHttpFile);
		m_hHttpFile = NULL;
	}
}


void CHttpDownload::CloseInternetConnection()
{
	if (m_hHttpConnect != NULL) 
	{
		TRACE(_T("------> ASYNC INET INFO: ::InternetCloseHandle(m_hHttpConnect)\n"));
		::InternetCloseHandle(m_hHttpConnect);
		m_hHttpConnect = NULL;
	}
}


UINT CHttpDownload::HttpSendRequest()
{
	ASSERT(m_hHttpFile != NULL);

	TRACE(_T("------> ASYNC INET INFO: ::HttpSendRequest(...)\n"));
	
	if (!::HttpSendRequest(m_hHttpFile, NULL, 0, NULL, 0))
	{
		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_IO_PENDING) 
		{
			// �������Ľ��
			HTTP_LOG("dwError: %d\n", dwError);
			ASSERT(FALSE);
			AfxThrowInternetException(NULL, dwError);
		}

		HANDLE hEvt;
		if (CThreadModel::QueryEvent(hEvt))
		{
			return DOWNLOAD_RESULT_USER_ABORT;
		}
		if (hEvt == m_hEvtRequestComplete)
		{
			if (m_dwError != ERROR_SUCCESS) 
			{
				HTTP_LOG("m_dwError: %d\n", m_dwError);
				AfxThrowInternetException(NULL, m_dwError);
			}
			else
			{
				// Just keep go on
			}
		}
		else
		{
			// �������Ľ��
			HTTP_LOG("�������Ľ��\n");
			ASSERT(FALSE);
			OnMessageError(_T(""));
			return DOWNLOAD_RESULT_FAILED;
		}

	}

	return DOWNLOAD_RESULT_CONTINUE;
}


void CHttpDownload::SetStatusAndErrorCode(DOWNLOAD_STATUS nStatus, DOWNLOAD_ERRORCODE nErrorCode)
{
	m_DownloadFileInfo.nStatus = nStatus;
	m_DownloadFileInfo.nErrorCode = nErrorCode;
}


void CHttpDownload::SetDownloadSize(int nSize)
{	
	m_DownloadFileInfo.dwDownloadedSize = nSize;
}


void CHttpDownload::SetFileSize(int nSize)
{	
	m_DownloadFileInfo.dwFileSize = nSize;
}


void CHttpDownload::HttpQueryResponseHeader()
{
	ASSERT(m_hHttpFile != NULL);

	if (!HttpQueryInfo(m_hHttpFile,
			HTTP_QUERY_RAW_HEADERS_CRLF ,
	        m_strResponseHeader))
	{
		AfxThrowInternetException(NULL);
	}			
}

void CHttpDownload::HttpHandleModifiedTime()
{
	ASSERT(m_hHttpFile != NULL);

	if (HttpQueryInfo(m_hHttpFile,
			HTTP_QUERY_LAST_MODIFIED | HTTP_QUERY_FLAG_SYSTEMTIME,
	        &m_tLastModified))
	{
		m_bIfModified = TRUE;
	}	
}

DWORD CHttpDownload::HttpQueryInfoStatusCode()
{
	ASSERT(m_hHttpFile != NULL);

	DWORD dwStatusCode;
	if (!::HttpQueryInfoStatusCode(m_hHttpFile, dwStatusCode)) 
	{
		AfxThrowInternetException(NULL);
	}
	return dwStatusCode;
}

void CHttpDownload::HttpSetExternalHeader(CString strHeader)
{
	ASSERT(m_hHttpFile != NULL);

	if (!::HttpAddRequestHeaders(m_hHttpFile, strHeader, 
		strHeader.GetLength(), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE))
	{
		AfxThrowInternetException(NULL);
	}
}

void CHttpDownload::HttpPreSetFilePointer(DWORD dwOffSet)
{
	ASSERT(m_hHttpFile != NULL);
	
	if (dwOffSet == 0) 
	{
		return;
	}

	CString strRangeHeader;
	strRangeHeader.Format(_T("Range: bytes=%d-"), dwOffSet);
	if (!::HttpAddRequestHeaders(m_hHttpFile, strRangeHeader, 
		strRangeHeader.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		AfxThrowInternetException(NULL);
	}
}

void CHttpDownload::HttpSetRequestModifiedHeader(SYSTEMTIME &pTime)
{
	ASSERT(m_hHttpFile != NULL);

	CString strTime;
	CTime tTime(pTime);	
	// weakday
	CString strWeak = tTime.Format("%A").Left(3);
	//day
	CString strDay = tTime.Format("%d");
	// month
	CString strMonth = tTime.Format("%B").Left(3);
	// year
	CString strYear = tTime.Format("%Y");
	// time
	CString strHour = tTime.Format("%H:%M:%S");
	

	strTime.Format("%s, %s %s %s %s GMT", strWeak, strDay, strMonth, strYear, strHour);

	CString strRangeHeader;
	strRangeHeader.Format(_T("If-Modified-Since: %s"), strTime);
	if (!::HttpAddRequestHeaders(m_hHttpFile, strRangeHeader, 
		strRangeHeader.GetLength(), HTTP_ADDREQ_FLAG_ADD))
	{
		AfxThrowInternetException(NULL);
	}	
}


BOOL CHttpDownload::GetUserAgent(CString& strUserAgent)
{
	strUserAgent = _T("Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
	return TRUE;
}

/// ��Internet�������ô�����Ϣ��DownloadRun�ڲ�ʹ�ã������⹫��
UINT CHttpDownload::ApplyNetworkSettings()
{


	DWORD dwAccessType = (m_bUseGlobalNetworkSettings)?s_dwAccessType:m_dwAccessType;
	if (INTERNET_OPEN_TYPE_PROXY == dwAccessType)
	{
		CString strProxyAddress;

		if (m_bUseGlobalNetworkSettings)
		{
			strProxyAddress.Format("%s:%u", s_strProxyIP, s_wProxyPort);
		}
		else
		{
			strProxyAddress.Format("%s:%u", m_strProxyIP, m_wProxyPort);
		}
		
		INTERNET_PROXY_INFO proxyInfo;
		proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_PROXY;
		proxyInfo.lpszProxy = strProxyAddress;
		proxyInfo.lpszProxyBypass = strProxyAddress;
		DWORD dwBufSize = sizeof(proxyInfo);
		
		if (FALSE == ::InternetSetOption(	m_hInetSession, 
											INTERNET_OPTION_PROXY, 
											&proxyInfo, 
											dwBufSize))
		{
			DWORD dwError = GetLastError();
			ASSERT(FALSE);
			HTTP_LOG("����Proxy��Ϣ����0x%x\n", dwError);
			return	DOWNLOAD_RESULT_FAILED;
		}
	}
	else if (INTERNET_OPEN_TYPE_DIRECT == dwAccessType)
	{
		CString strProxyAddress(_T(""));
		INTERNET_PROXY_INFO proxyInfo;
		proxyInfo.dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
		proxyInfo.lpszProxy = strProxyAddress;
		proxyInfo.lpszProxyBypass = strProxyAddress;
		DWORD dwBufSize = sizeof(proxyInfo);
		
		if (FALSE == ::InternetSetOption(	m_hInetSession, 
											INTERNET_OPTION_PROXY, 
											&proxyInfo, 
											dwBufSize))
		{
			DWORD dwError = GetLastError();
			ASSERT(FALSE);
			HTTP_LOG("����Proxy��Ϣ����0x%x\n", dwError);
			return	DOWNLOAD_RESULT_FAILED;
		}
		
		return DOWNLOAD_RESULT_CONTINUE;
	}

	// UserName
	CString &strProxyUserName = 
			(m_bUseGlobalNetworkSettings)?s_strProxyUserName:m_strProxyUserName;

	int nLen = strProxyUserName.GetLength();
	
	if (FALSE == ::InternetSetOption(	m_hHttpFile, 
										INTERNET_OPTION_PROXY_USERNAME, 
										(LPVOID)(strProxyUserName.GetBuffer(nLen)),
										nLen + 1))
	{
		DWORD dwError = GetLastError();
		strProxyUserName.ReleaseBuffer(nLen);
		ASSERT(FALSE);
		HTTP_LOG("����Proxy�û�������0x%x\n", dwError);
		return DOWNLOAD_RESULT_FAILED;
	}
	
	strProxyUserName.ReleaseBuffer(nLen);
	
	// Password
	CString &strProxyPassword = 
			(m_bUseGlobalNetworkSettings)?s_strProxyPassword:m_strProxyPassword;

	nLen = strProxyPassword.GetLength();
	
	if (FALSE == ::InternetSetOption(m_hHttpFile, 
									INTERNET_OPTION_PROXY_PASSWORD, 
									(LPVOID)(strProxyPassword.GetBuffer(nLen)),
									nLen + 1))
	{
		DWORD dwError = GetLastError();
		strProxyPassword.ReleaseBuffer(nLen);
		ASSERT(FALSE);
		HTTP_LOG("����Proxy�������0x%x\n", dwError);
		return DOWNLOAD_RESULT_FAILED;
	}
	
	strProxyPassword.ReleaseBuffer(nLen);


	return DOWNLOAD_RESULT_CONTINUE;
}

BOOL CHttpDownload::SetInternetOptions()
{
	ASSERT(m_hInetSession != NULL);
	
	// ���ó�ʱѡ��

	return TRUE;
}

#ifdef _DEBUG

void AFXAPI CHttpDownload::InternetStatusCallbackDebug(HINTERNET hInternet,
	DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
	UNUSED_ALWAYS(hInternet);
	TRACE(_T("Internet ctxt=%d: "), dwContext);

	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		TRACE(_T("Resolving name for %s\n"), (LPCTSTR)lpvStatusInformation);
		break;

	case INTERNET_STATUS_NAME_RESOLVED:
		TRACE(_T("Resolved name for %s!\n"), (LPCTSTR)lpvStatusInformation);
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		TRACE(_T("Connecting to socket address [%s]\n"), (LPCTSTR)lpvStatusInformation);
		break;

	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		TRACE(_T("Connected to socket address!\n"));
		break;

	case INTERNET_STATUS_SENDING_REQUEST:
		TRACE(_T("Sending request...\n"));
		break;

	case INTERNET_STATUS_REQUEST_SENT:
		TRACE(_T("Request sent!\n"));
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE:
		TRACE(_T("Receiving response...\n"));
		break;

	case INTERNET_STATUS_RESPONSE_RECEIVED:
		TRACE(_T("Response received!\n"));
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION:
		TRACE(_T("Closing connection 0x%08X\n"), hInternet);
		break;

	case INTERNET_STATUS_CONNECTION_CLOSED:
		TRACE(_T("Connection 0x%08X closed!\n"), hInternet);
		break;

	case INTERNET_STATUS_HANDLE_CREATED:
		TRACE(_T("Handle 0x%08X created\n"), LPINTERNET_ASYNC_RESULT(lpvStatusInformation)->dwResult);
		break;

	case INTERNET_STATUS_HANDLE_CLOSING:
		TRACE(_T("Handle 0x%08X closing!\n"), hInternet);
		break;

	case INTERNET_STATUS_REQUEST_COMPLETE:
		if (dwStatusInformationLength == sizeof(INTERNET_ASYNC_RESULT))
		{
			LPINTERNET_ASYNC_RESULT lpResult = LPINTERNET_ASYNC_RESULT(lpvStatusInformation);
			TRACE(_T("Request complete, dwResult = 0x%08X, dwError = 0x%08X\n"),
				lpResult->dwResult, lpResult->dwError);
		}
		else
			TRACE(_T("Request complete.\n"));
		break;

	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
	case INTERNET_STATUS_PREFETCH:
	case INTERNET_STATUS_REDIRECT:
	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
	case INTERNET_STATUS_STATE_CHANGE:
	default:
		TRACE(_T("Unknown status: %d\n"), dwInternetStatus);
		break;
	}

	return;
}
#endif // _DEBUG


void CHttpDownload::OnStatusCallback(HINTERNET hInternet, 
	DWORD dwInternetStatus,LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
		
	CString strMessage((char*)lpvStatusInformation, dwStatusInformationLength);
	HTTP_LOG("StatusCallback: %d(%s)\n", dwInternetStatus, strMessage);
	switch (dwInternetStatus)
	{
	case INTERNET_STATUS_RESOLVING_NAME:
		//OnInetResolvingName(hInternet, lpvStatusInformation, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_NAME_RESOLVED:
		//OnInetNameResolved(hInternet, lpvStatusInformation, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_CONNECTING_TO_SERVER:
		//OnInetConnectingToServer(hInternet, lpvStatusInformation, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER:
		//OnInetConnectedToServer(hInternet, lpvStatusInformation, dwStatusInformationLength);
		break;
	case INTERNET_STATUS_SENDING_REQUEST:
		//OnInetSendingRequest(hInternet);
		break;
	case INTERNET_STATUS_REQUEST_SENT:
		//OnInetRequestSent(hInternet, lpvStatusInformation);
		break;
	case INTERNET_STATUS_RECEIVING_RESPONSE:
		//OnInetReceivingResponce(hInternet);
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED:
		OnInetResponceReceived(hInternet, lpvStatusInformation);
		break;
	case INTERNET_STATUS_CLOSING_CONNECTION:
		//OnInetClosingConnection(hInternet);
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED:
		//OnInetConnectionClosed(hInternet);
		break;
	case INTERNET_STATUS_HANDLE_CREATED:
		OnInetHandleCreated(hInternet, LPINTERNET_ASYNC_RESULT(lpvStatusInformation));
		break;
	case INTERNET_STATUS_HANDLE_CLOSING:
	//	OnInetHandleClosing(hInternet);
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE:
		OnInetRequestComplete(hInternet, LPINTERNET_ASYNC_RESULT(lpvStatusInformation));
		break;
	case INTERNET_STATUS_REDIRECT:
	case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
	case INTERNET_STATUS_STATE_CHANGE:
	default:
		break;
	}
}


VOID AFXAPI CHttpDownload::InternetStatusCallback(
	HINTERNET hInternet, DWORD_PTR dwContext,
	DWORD dwInternetStatus, LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
#ifdef _DEBUG
		InternetStatusCallbackDebug(hInternet, dwContext,
			dwInternetStatus, lpvStatusInformation, dwStatusInformationLength);
#endif
		
	CHttpDownload * pThis = (CHttpDownload *)dwContext;
	if (m_thisMap.Lookup(pThis)) 
	{
		if (m_thisMap.LockMap())
		{		
			pThis->OnStatusCallback(hInternet, dwInternetStatus, 
				lpvStatusInformation, dwStatusInformationLength);

			m_thisMap.UnlockMap();
		}
	}
}


void CHttpDownload::OnInetHandleCreated(HINTERNET hInternet, 
	LPINTERNET_ASYNC_RESULT lpInetStatusResult)
{
	ASSERT(lpInetStatusResult != NULL);

	HINTERNET hInet = HINTERNET(lpInetStatusResult->dwResult);
	DWORD dwInetHandleType = AfxGetInternetHandleType(hInet);
	switch(dwInetHandleType) 
	{
	case INTERNET_HANDLE_TYPE_CONNECT_HTTP:
		CloseInternetConnection();
		m_hHttpConnect = hInet;
		break;
	case INTERNET_HANDLE_TYPE_HTTP_REQUEST:
		CloseInternetFile();
		m_hHttpFile = hInet;
		break;
	default:
		break;
	}

	// HANDLE�Ѵ����¼����첽���ƣ�
	::SetEvent(m_hEvtHandleCreated);
}


void CHttpDownload::OnInetRequestComplete(HINTERNET hInternet, 
	LPINTERNET_ASYNC_RESULT lpInetStatusResult)
{
	ASSERT(lpInetStatusResult != NULL);
	
	m_dwError = lpInetStatusResult->dwError;
	// ������������¼����첽���ƣ�
	::SetEvent(m_hEvtRequestComplete);
}


void CHttpDownload::OnInetResponceReceived(HINTERNET hInternet, 
	LPVOID lpvStatusInformation)
{
	DWORD *pdwBytes = (DWORD *)lpvStatusInformation;
	TRACE("Received %d bytes\n", *pdwBytes);
	
}


void CHttpDownload::OnInetClosingConnection(HINTERNET hInternet)
{
}


void CHttpDownload::OnInetConnectionClosed(HINTERNET hInternet)
{
	TRACE0("close conection!!\n");
	::SetEvent(m_hEvtRequestComplete);
}


void CHttpDownload::QueryInternetFileInfo(CString& strInetFileName, 
												DWORD& dwInetFileSize, 
												SYSTEMTIME& stInetFileLastModifiedTime)
{
	ASSERT(m_hHttpFile != NULL);

	// ��ѯ�ļ���	
	strInetFileName.Empty();

	CString strDisposition;
	if (HttpQueryInfo(m_hHttpFile,
			HTTP_QUERY_CONTENT_DISPOSITION ,
	        strDisposition))
	{
		// Attachment specifi...
		//
		/* {"attachment; filename="1454.zip""} */
		//     -- comment by yuyu

		int nLeft = strDisposition.Find('\"');
		int nRight = strDisposition.ReverseFind('\"');
		if (nLeft > 0 && nRight > nLeft)
		{
			strInetFileName = strDisposition.Mid(nLeft + 1, nRight - nLeft - 1);
		}
				
	}	
	else
	{	
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];
		TCHAR szFilePart[_MAX_FNAME];
		TCHAR szExtName[_MAX_EXT];
		_tsplitpath(m_DownloadFileInfo.m_strDownloadURL, szDrive, szDir, szFilePart, szExtName);

		strInetFileName = szFilePart;

		if (*szExtName != '\0')
		{			
			strInetFileName += szExtName; 
		}		
	}
	 
	 
	 
	// ��ѯ�ļ�����
	if (!HttpQueryInfo(m_hHttpFile,
		HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
        dwInetFileSize))
	{
		dwInetFileSize = (DWORD)-1;
		OnSupportResume(FALSE);
	}
	
	if (!m_bSupportResume) 
	{
		dwInetFileSize = (DWORD)-1;
	}
}

void CHttpDownload::OnSupportResume(BOOL bSupport)
{
	if (!bSupport && m_bSupportResume) 
	{
		TRACE0(_T("��������֧�ֶϵ��������"));
	}
}


UINT CHttpDownload::OnHttpStatusOK(DWORD dwCurrentPos)
{
	ASSERT(m_hHttpFile != NULL);

	if (dwCurrentPos != 0 && m_bForceDownload)  // ��֧�ֶϵ�����
	{
		dwCurrentPos = 0;
		OnSupportResume(FALSE);
	}
	
	if (dwCurrentPos == 0)
	{
		
		MoveFilePosition(0, m_DownloadFileInfo.dwFileSize);		

		return DownloadInternetFile();
	}

	return DOWNLOAD_RESULT_FAILED;	
}


BOOL CHttpDownload::GetFileCurrentPos(HANDLE hSaveFileHandle, DWORD &dwCurrentPos)
{
	DWORD dwSizeHigh;
	DWORD dwFileLength = GetFileSize (hSaveFileHandle, &dwSizeHigh) ; 
	// If we failed ... 
	if (dwFileLength == INVALID_FILE_SIZE) 
	{
		DWORD dwError = GetLastError();  

		return FALSE;
	}
	if (dwSizeHigh > 0)		// we don't support huge file
	{
		return FALSE;
	}
	dwCurrentPos = dwFileLength;
	return TRUE;
}


BOOL CHttpDownload::MoveFilePosition(DWORD dwCurrentPos, DWORD dwFileSize)
{
	if(dwCurrentPos > dwFileSize)
	{
		if (m_bForceDownload)
			dwCurrentPos = 0;
		else
			return FALSE;
	}

	
	SetFilePointer(m_hSaveFileHandle, dwCurrentPos, NULL, FILE_BEGIN);

	// truncate file
	SetEndOfFile(m_hSaveFileHandle);
	m_DownloadFileInfo.dwResumeBytes = m_DownloadFileInfo.dwDownloadedSize = dwCurrentPos;

	return TRUE;
}



UINT CHttpDownload::DownloadInternetFile()
{
	HRESULT hr = E_UNEXPECTED;
	DWORD dwBytesRead = 0;
	DWORD dwBytesLeft = 0;

	OnDownloadStart(m_DownloadFileInfo.dwResumeBytes, m_DownloadFileInfo.dwFileSize);

	if (!InitReceiveBuffer ())
		return DOWNLOAD_RESULT_FAILED;
	
	::ZeroMemory(&m_buffersOut, sizeof(INTERNET_BUFFERS));
	m_buffersOut.dwStructSize = sizeof(INTERNET_BUFFERS);
	m_buffersOut.lpvBuffer = m_ReceiveBuffer;			

	dwBytesLeft = m_DownloadFileInfo.dwFileSize;
	//OnMessageInfo(_T("��ʼ��������"));
	int		nResult = 0;
	DWORD	dwWriteBytes = 0;
	while (TRUE) 
	{			
		m_buffersOut.dwBufferLength = 
			(m_dwReceiveBufferLen > dwBytesLeft)?dwBytesLeft:m_dwReceiveBufferLen;

		if (!::InternetReadFileEx(m_hHttpFile, &m_buffersOut, 
			IRF_ASYNC, reinterpret_cast<DWORD_PTR>(this)))
		{
			DWORD dwError = ::GetLastError();
			if (dwError != ERROR_IO_PENDING) 
			{
				// �������Ľ��
				ASSERT(FALSE);
				AfxThrowInternetException(NULL, dwError);
			}

			// Query all exit event
			HANDLE hEvt;
			if (CThreadModel::QueryEvent(hEvt))
			{
				return DOWNLOAD_RESULT_USER_ABORT;
			}
			if (hEvt == m_hEvtRequestComplete)
			{
				if (m_dwError != ERROR_SUCCESS) 
				{
					AfxThrowInternetException(NULL, m_dwError);
				}
				else
				{
					// Just keep go on
				}
			}
			else
			{
				// �������Ľ��
				ASSERT(FALSE);
				OnMessageError(_T(""));
				return DOWNLOAD_RESULT_FAILED;
			}			
		}
		else  // ������ַ��첽����� Ҳ��Ҫ��ѯ�˳��¼� 2004.2.17��
		{
			// Only query thread abort event
			if (CThreadModel::QueryExitEvent())
				return DOWNLOAD_RESULT_USER_ABORT;
		}
	
		dwBytesRead = m_buffersOut.dwBufferLength;
		
		// û��Internet�ļ����Զ���
		if (dwBytesRead == 0) 
		{
			// force write buffer to file
			if (!FlushSaveFile())
			{			
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_WRITEFAILED);			
				return DOWNLOAD_RESULT_TRY;
			}

			// ����ļ���Сδ֪�� ��������ر�������Ϊ���سɹ�
			if (m_DownloadFileInfo.dwFileSize == -1 || 
				m_DownloadFileInfo.dwFileSize == m_DownloadFileInfo.dwDownloadedSize)
			{
				SetStatusAndErrorCode(DOWNLOAD_STATUS_COMPLETE);
				return DOWNLOAD_RESULT_OK;
			}
			else
			{
				SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_SERVERCLOSE);
				return DOWNLOAD_RESULT_TRY;
			}			
		}

		m_DownloadFileInfo.dwDownloadedSize += dwBytesRead;
		dwBytesLeft -= dwBytesRead;
		
		// write file							
		//nResult = WriteFile(m_hSaveFileHandle, m_buffersOut.lpvBuffer, dwBytesRead, &dwWriteBytes, NULL);					
		;

		//if (nResult == 0 || dwWriteBytes == 0)	//write error. may be disk full
		if (!WriteSaveFile(m_buffersOut.lpvBuffer, dwBytesRead))
		{			
			SetStatusAndErrorCode(DOWNLOAD_STATUS_FAILED, ERRORCODE_WRITEFAILED);			
			return DOWNLOAD_RESULT_TRY;
		}					
		
		OnProgress(dwBytesRead);
		
#ifdef _DEBUG
		// ���������ٶȣ� ����debug��Ч��
		Sleep(500);
#endif		
	}
}


UINT CHttpDownload::OnHttpStatusRedirect(DWORD dwCurrentPos)
{
	ASSERT(m_hHttpFile != NULL);

	CString strRedirectUrl;
	if (!HttpQueryInfo(m_hHttpFile, HTTP_QUERY_LOCATION, strRedirectUrl)) 
	{
		OnMessageError(_T("�ض���ʧ��"));
		return DOWNLOAD_RESULT_FAILED;
	}
	CString strMessage;
	strMessage.Format(_T("�ض��� %s"), strRedirectUrl);
	//OnMessageInfo(strMessage);

	OnRedirect(strRedirectUrl);
	return DOWNLOAD_RESULT_TRY;
}


UINT CHttpDownload::OnHttpStatusPartialContent(DWORD dwCurrentPos)
{
	ASSERT(m_hHttpFile != NULL);

	HRESULT hr = E_UNEXPECTED;
	if (dwCurrentPos == 0) 
	{
		// �����״̬�룬����DL_RESULT_FAIL����������
		OnMessageError(_T(""));
		return DOWNLOAD_RESULT_FAILED;
	}
	else
	{
		OnSupportResume(TRUE);
		
		CString strInetFileName;
		DWORD dwInetFileSize;
		SYSTEMTIME stInetFileLastModifiedTime;
	
		// ��ѯInternet�ļ���Ϣ
		QueryInternetFileInfo(strInetFileName, dwInetFileSize, stInetFileLastModifiedTime);
		if (-1 != dwInetFileSize)
		{
			m_DownloadFileInfo.dwFileSize = dwInetFileSize + dwCurrentPos;
		}
		else
		{
			m_DownloadFileInfo.dwFileSize = dwInetFileSize;
		}
		
		if (MoveFilePosition(dwCurrentPos, m_DownloadFileInfo.dwFileSize))
			return DownloadInternetFile();
		else
			return DOWNLOAD_RESULT_FAILED;
	}
}


UINT CHttpDownload::OnHttpStatusOther(DWORD dwStatusCode)
{
	static const DWORD arrStatusCodeFail[] =
	{
		HTTP_STATUS_BAD_REQUEST,         // 400
		HTTP_STATUS_DENIED,              // 401
		HTTP_STATUS_PAYMENT_REQ,         // 402
		HTTP_STATUS_NOT_FOUND,           // 404
		HTTP_STATUS_BAD_METHOD,          // 405
		HTTP_STATUS_NONE_ACCEPTABLE,     // 406
		HTTP_STATUS_PROXY_AUTH_REQ       // 407
	};

	for (UINT i = 0; i < sizeof(arrStatusCodeFail) / sizeof(DWORD); i++)
	{
		if (dwStatusCode == arrStatusCodeFail[i]) 
		{
			// 
			return DOWNLOAD_RESULT_FAILED;
		}
	}
	return DOWNLOAD_RESULT_FAILED;
}


void CHttpDownload::OnRedirect(LPCTSTR lpszRedirectUrl)
{
	ASSERT(lpszRedirectUrl != NULL && AfxIsValidString(lpszRedirectUrl));	

	m_DownloadFileInfo.m_strDownloadURL = lpszRedirectUrl;
	
	LPVOID pMem = NULL;
		
	if (NULL == m_hwndUIProxy)
	{
		return;
	}

	CString strRedirected = lpszRedirectUrl;

	int nSize = strRedirected.GetLength() + 1;
	if (FALSE == CreateHeapData(strRedirected.GetBuffer(nSize), nSize, pMem))
	{
		return;
	}
		
	strRedirected.ReleaseBuffer(nSize);

	::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_REDIRECTED, (WPARAM)this, (LPARAM)pMem);
}


BOOL CHttpDownload::CreateHeapData(const void *Data, DWORD dwSize, LPVOID &pMem)
{
	pMem = NULL;
	
	if ( m_hUIHeap == NULL)
		return	FALSE;
	
	if ((pMem = ::HeapAlloc(m_hUIHeap, 0, dwSize)) == NULL)
		return	FALSE;
	
	::CopyMemory(pMem, (CONST VOID *)Data, dwSize);	

	return TRUE;
}


void CHttpDownload::OnConnecting()
{
	LPVOID pMem = NULL;
		
	if (m_hwndUIProxy == NULL)
		return;

	if (m_chEventMask & EVENT_CONNECTING)
	{	
		int nSize = m_strServer.GetLength() + 1;
		if (!CreateHeapData(m_strServer.GetBuffer(nSize), nSize, pMem))
			return;
		
		m_strServer.ReleaseBuffer(nSize);

		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_CONNECTING, (WPARAM)this, (LPARAM)pMem);
	}	
}


void CHttpDownload::OnConnected()
{
	LPVOID pMem = NULL;
	
	if (m_hwndUIProxy == NULL)
		return;

	if (m_chEventMask & EVENT_CONNECTED)
	{
		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_CONNECTED, (WPARAM)this, (LPARAM)0);
	}
	
}


void CHttpDownload::OnDownloadStart(DWORD dwResumeBytes, DWORD dwFileBytes)
{
	LPVOID pMem = NULL;
	DWORD pBuffer[2];
	
	if (m_hwndUIProxy == NULL)
		return;
	
	if (m_chEventMask & EVENT_DOWNLOADSTART)
	{
		int nSize = sizeof(DWORD) * 2;
		pBuffer[0] = dwResumeBytes;
		pBuffer[1] = dwFileBytes;
		
		if (!CreateHeapData(pBuffer, nSize, pMem))
			return;
		
		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_START, (WPARAM)this, (LPARAM)pMem);	 
	}

}


void CHttpDownload::OnDownloadComplete(DWORD nErrorCode)
{
	LPVOID pMem = NULL;
	
	if (m_hwndUIProxy == NULL)
		return;

	if (m_chEventMask & EVENT_DOWNLOADCOMPLETE)
	{
		CString strBuffer;
		CString &strSaveFile = m_DownloadFileInfo.m_strSavePath;
		CString &strResponseFile = m_strResponseFileName;
		
		int nSize = sizeof(DWORD) +					// errorcode
					strSaveFile.GetLength() + 1;		// save file
					//+ strResponseFile.GetLength() + 1;				// ReponseFile

		LPTSTR pBuffer = strBuffer.GetBuffer(nSize);

		int nOffset = 0;
		::ZeroMemory(pBuffer, nSize);
		::CopyMemory(pBuffer, &nErrorCode, sizeof(DWORD));
		
		nOffset += sizeof(DWORD);
		::CopyMemory(pBuffer + nOffset, strSaveFile, strSaveFile.GetLength());

		//nOffset += strSaveFile.GetLength() + 1;
		//::CopyMemory(pBuffer + nOffset, strResponseFile, strResponseFile.GetLength());

		if (!CreateHeapData(pBuffer, nSize, pMem))
			return;
		
		strBuffer.ReleaseBuffer(nSize);

		TRACE("post download complete msg\n");
		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_COMPLETE, (WPARAM)this, (LPARAM)pMem);
	}
	
}


void CHttpDownload::OnError(DWORD nErrorCode)
{	
	if (m_hwndUIProxy == NULL)
		return;
	
	LPVOID pMem = NULL;

	if (m_chEventMask & EVENT_ERROR)
	{

		if (!CreateHeapData((const void *)&nErrorCode, sizeof(nErrorCode), pMem))
			return;
			
		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_ERROR, (WPARAM)this, (LPARAM)pMem);
	}

}


void CHttpDownload::OnProgress(DWORD dwBytesRead)
{
	ASSERT((m_DownloadFileInfo.dwDownloadedSize - m_DownloadFileInfo.dwResumeBytes) > 0);

	if (m_hwndUIProxy == NULL)
		return;

	if (m_chEventMask & EVENT_PROGRESS)
	{

		int nSize = 2 * sizeof(DWORD) + MAX_PATH + 1;
		BYTE pBuffer[ 2 * sizeof(DWORD) + MAX_PATH + 1 ];
		memset(pBuffer,0,nSize);
		LPVOID pMem = NULL;
        DWORD *pDWord =(DWORD *) pBuffer;
		pDWord[0] = m_DownloadFileInfo.dwDownloadedSize;
		pDWord[1] = m_DownloadFileInfo.dwFileSize;
		int nLen = m_DownloadFileInfo.m_strSavePath.GetLength();
		if( nLen > MAX_PATH)
			return;
		memcpy(pBuffer + sizeof(DWORD)*2,m_DownloadFileInfo.m_strSavePath.GetBuffer(nLen),nLen);
		m_DownloadFileInfo.m_strSavePath.ReleaseBuffer(nLen);
		
		if (!CreateHeapData(pBuffer, nSize, pMem))
			return;

		::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_PROGRESS, (WPARAM)this, (LPARAM)pMem);
	}

}


BOOL CHttpDownload::CreateUIProxy()
{
	ASSERT(m_hwndUIProxy == NULL);
	
	if (m_pUIInterface == NULL)
		return TRUE;	// Skip Ui interface
	
	CHttpDownloadUIProxy *pUIProxy = CHttpDownloadUIProxy::GetInstance();
	
	ASSERT(pUIProxy != NULL);

	if (pUIProxy == NULL) 
	{
		return FALSE;
	}
	
	pUIProxy->SetUIInterface((DWORD)this, m_pUIInterface);
	pUIProxy->SetUIExitEvent(m_hEvtUIExit);

	m_hwndUIProxy = pUIProxy->GetSafeHwnd();
	m_hUIHeap = pUIProxy->GetHeap();

	ASSERT(m_hwndUIProxy != NULL);
	ASSERT(m_hUIHeap != NULL);
	
	return TRUE;
}

// ��������Ҫ�����߳��ڵ��á� ��CancelDownload()��Download()�����ط�����
// ��������ʹ��SendMessage������PostMessage, ���ܱ�֤CHttpDownloadUIPRoxy֪��
// ĳ�����ؽ����� ���ܱ�֤ɾ��UIInterface����

void CHttpDownload::DeleteUIProxy()
{
	// �����������߳��ڵ���
	ASSERT(m_hThread == 0);

	if (m_hwndUIProxy != NULL)
	{	
		::SendMessage(m_hwndUIProxy, WM_DOWNLOAD_EXITINSTANCE, (WPARAM)this, 0);
		m_hwndUIProxy = NULL;
	}
}


void CHttpDownload::SetUIInterface(CHttpDownloadUIInterface *pUIInterface)
{
	ASSERT(m_pUIInterface == NULL);
	// ���ؿ�ʼ���޷�����UIInterface.
	ASSERT(m_hThread == NULL);

	m_pUIInterface = pUIInterface;
}


void CHttpDownload::SetUseIEProxy(BOOL bUseIEProxy)
{
	if (bUseIEProxy)
	{
		m_dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
	}	
	else
	{
		return; // if not using IE Proxy, just return
	}

	m_strProxyIP		= _T("");
	m_wProxyPort		= 0;
	m_bProxyAuthentic	= FALSE;
	m_strProxyUserName	= _T("");
	m_strProxyPassword	= _T("");	

	if (m_bUseGlobalNetworkSettings)
	{
		s_dwAccessType		= m_dwAccessType;
		s_strProxyIP		= _T("");
		s_wProxyPort		= 0;
		s_bProxyAuthentic	= FALSE;
		s_strProxyUserName	= _T("");
		s_strProxyPassword	= _T("");	
	}
}

void CHttpDownload::SetAutoResume(BOOL bAutoResume)
{
	m_bAutoResume = bAutoResume;
}

void CHttpDownload::SetEventMask(BYTE chEventMask)
{
	ASSERT(chEventMask <= EVENT_ALL);

	m_chEventMask = chEventMask;
}

BOOL CHttpDownload::GetRawResponseHeader(CString &strResponseHeader)
{
	ASSERT(!m_strResponseHeader.IsEmpty());

	strResponseHeader = m_strResponseHeader;

	return TRUE;
}

BOOL CHttpDownload::GetLastModifyTime(SYSTEMTIME &tTime)
{
	if (m_bIfModified)
	{
		tTime = m_tLastModified;
	}
	return m_bIfModified;
}

/** M�޸ļ�¼���ĳɷ����ַ�����ԭʼ��Сд���ã������ֽ����������URL
 *  �Ĵ�Сд���д���
 *  ���⣬ȷ������Ĳ����������ð�ţ����û�У����Զ�����ð��������
 */
BOOL CHttpDownload::QueryInfoBase(const char* szParamName, CString& strInfo)
{
	ASSERT(!m_strResponseHeader.IsEmpty());

	CString strParamName(szParamName);

	CString strOriginalCase = m_strResponseHeader;
	
	strOriginalCase.TrimLeft();
	strOriginalCase.TrimRight();
	
	strInfo = strOriginalCase;
	strInfo.MakeLower();
	strParamName.MakeLower();
	
	if (_T(':') != strParamName.GetAt(strParamName.GetLength()-1))
	{
		strParamName += ":"; //M Must end with this
	}
	
	int nPosS = strInfo.Find(strParamName);
	int nPosE = strInfo.Find('\r', nPosS + strParamName.GetLength());
	
	if(nPosE < 0)
	{
		nPosE = strInfo.GetLength();
	}

	if(-1 != nPosS)
	{
		nPosS += strParamName.GetLength();
		strInfo = strOriginalCase.Mid(nPosS, nPosE - nPosS); //M fix miss case error
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CHttpDownload::OnGetProxyUserPassword()
{
	DWORD dwErr;
	dwErr = InternetErrorDlg(	::GetDesktopWindow(),                     
								m_hHttpFile,                               
								ERROR_SUCCESS ,                         
								FLAGS_ERROR_UI_FILTER_FOR_ERRORS  |     
								FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
								FLAGS_ERROR_UI_FLAGS_GENERATE_DATA, 
								NULL
							);

	if (ERROR_INTERNET_FORCE_RETRY == dwErr)
	{
		// ���赽��Ա������
		DWORD dwLen = 0;
		CString strTmpUser, strTmpPass;
		LPTSTR szTmp = NULL;

		if (FALSE == ::InternetQueryOption(	m_hHttpFile, 
											INTERNET_OPTION_PROXY_USERNAME,
											NULL,
											&dwLen))
		{
			DWORD dwError = GetLastError();
			ASSERT(ERROR_INSUFFICIENT_BUFFER == dwError);
			
			szTmp = strTmpUser.GetBuffer(dwLen+1);
			
			if (FALSE == ::InternetQueryOption(	m_hHttpFile, 
												INTERNET_OPTION_PROXY_USERNAME,
												szTmp,
												&dwLen))
			{
				ASSERT(FALSE);
				return FALSE;
			}

			strTmpUser.ReleaseBuffer(dwLen);
		}
		else
		{
			ASSERT(FALSE);
		}

		dwLen = 0;
		if (FALSE == ::InternetQueryOption(	m_hHttpFile, 
											INTERNET_OPTION_PROXY_PASSWORD, 
											NULL,
											&dwLen))
		{
			DWORD dwError = GetLastError();
			ASSERT(ERROR_INSUFFICIENT_BUFFER == dwError);
			
			szTmp = strTmpPass.GetBuffer(dwLen+1);
			
			if (FALSE == ::InternetQueryOption(	m_hHttpFile, 
												INTERNET_OPTION_PROXY_PASSWORD,
												szTmp,
												&dwLen))
			{
				ASSERT(FALSE);
				return FALSE;
			}

			strTmpPass.ReleaseBuffer(dwLen);
		}
		else
		{
			ASSERT(FALSE);
		}

		m_strProxyUserName = strTmpUser;
		m_strProxyPassword = strTmpPass;

		if (m_bUseGlobalNetworkSettings)
		{
			s_strProxyUserName = m_strProxyUserName;
			s_strProxyPassword = m_strProxyPassword;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

void CHttpDownload::SetResueThread(BOOL bResue)
{
	m_bReuseThread = bResue;
}

BOOL CHttpDownload::GetResponseFileName(CString &strResponseFileName)
{
	if (m_strResponseFileName.IsEmpty())
		return FALSE;
	
	strResponseFileName = m_strResponseFileName;
	return TRUE;
}


/// ��HTTP�ظ�ͷ����ȡ�ַ�������ѡ���Ƿ�ȥ��ǰ��ո�
BOOL CHttpDownload::QueryInfo(	const CString &strParamName, 
								CString &strRet, 
								BOOL bTrim/* = TRUE*/)
{
	if (strParamName.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (FALSE == QueryInfoBase(strParamName, strRet))
	{
		strRet = "";
		return FALSE;
	}

	if (bTrim)
	{
		strRet.TrimLeft();
		strRet.TrimRight();
	}

	return TRUE;
}

/// ��HTTP�ظ�ͷ����ȡ�����ַ�������ת����DWORD
BOOL CHttpDownload::QueryInfo(const CString &strParamName, DWORD &dwRet)
{
	if (strParamName.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strBuf;
	TCHAR	*ptcEnd;

	if (FALSE == QueryInfoBase(strParamName, strBuf))
	{
		dwRet = 0;
		return FALSE;
	}

	strBuf.TrimLeft();
	strBuf.TrimRight();
	
	dwRet = (DWORD)_tcstoul(strBuf, &ptcEnd, 10);
	if (ULONG_MAX == dwRet || (LPCTSTR)strBuf == ptcEnd)
	{
		dwRet = 0;
		return FALSE;
	}

	return TRUE;
}

/// ��HTTP�ظ�ͷ����ȡ��YES/NO��ʾ���ı�����ֵ����ת��ΪBOOL
BOOL CHttpDownload::QueryInfo(const CString &strParamName, BOOL &bRet)
{
	if (strParamName.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strTmp;
	if (FALSE == QueryInfoBase(strParamName, strTmp))
	{
		return FALSE;
	}

	strTmp.TrimLeft();
	strTmp.TrimRight();
	strTmp.MakeUpper();

	if (0 == strTmp.Compare(_T("YES")))
	{
		bRet = TRUE;
	}
	else if (0 == strTmp.Compare(_T("NO")))
	{
		bRet = FALSE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/// ��HTTP�ظ�ͷ����ȡ�ı���ʽ��Buffer����ԭ��Byte���� 
BOOL CHttpDownload::QueryInfo(	const CString &strParamName,	
								BYTE *byarBuf, 
								DWORD dwBufLen)
{
	ASSERT(AfxIsValidAddress(byarBuf, dwBufLen));

	if (strParamName.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	CString strTmp;
	if (FALSE == QueryInfoBase(strParamName, strTmp))
	{
		return FALSE;
	}

	strTmp.TrimLeft();
	strTmp.TrimRight();

	if (DWORD(strTmp.GetLength()) != dwBufLen * 2)
	{
		return FALSE;
	}
	
	if (FALSE == EncodeBuffer(byarBuf, strTmp))
	{
		return FALSE;
	}

	return TRUE;
}

/// ���HTTP����ͷ�������û�������Ϣ��
void CHttpDownload::ClearRequestHeader()
{
	m_strPostHeader = "";
}

/// ��ȡHTTP����ͷ�������û�������Ϣ��
CString CHttpDownload::GetRawUserRequestHeader()
{
	return m_strPostHeader;
}

/// ��HTTP����ͷ�������������
/** Ŀǰ��û�м��ϸ�ʽ�жϣ�������ע��OnDownloadComplete�ķ���ֵ

 *  
 */
BOOL CHttpDownload::AddInfo(const CString &strContent)
{
	m_strPostHeader += strContent;
	return TRUE;
}

/// ��HTTP����ͷ������ַ���
BOOL CHttpDownload::AddInfo(const CString &strParamName, const CString &strValue)
{
	if (strParamName.IsEmpty() || strValue.IsEmpty())
	{
		// ��ʽ���󣬲�������ֵ������Ϊ��
		ASSERT(FALSE);
		return FALSE;
	}

	CString strNewInfo;
	strNewInfo.Format("%s: %s \r\n", strParamName, strValue);
	
	m_strPostHeader += strNewInfo;

	return TRUE;
}

/// ��HTTP����ͷ�����DWORD���Զ�ת���������ַ���
BOOL CHttpDownload::AddInfo(const CString &strParamName, DWORD	dwValue)
{
	if (strParamName.IsEmpty())
	{
		// ��ʽ���󣬲���������Ϊ��
		ASSERT(FALSE);
		return FALSE;
	}

	CString strNewInfo;
	strNewInfo.Format("%s: %d \r\n", strParamName, dwValue);

	m_strPostHeader += strNewInfo;

	return TRUE;
}

/// ��HTTP����ͷ��������飬�Զ�ת����16���������ı�
BOOL CHttpDownload::AddInfo(const CString &strParamName, const BYTE *byarBuf, int dwLength)
{
	ASSERT(AfxIsValidAddress(byarBuf, dwLength));
	
	if (strParamName.IsEmpty())
	{
		// ��ʽ���󣬲���������Ϊ��
		ASSERT(FALSE);
		return FALSE;
	}

	CString strBuf;
	if (FALSE == DecodeBuffer(strBuf, byarBuf, dwLength))
	{
		return FALSE;
	}

	CString strNewInfo;
	strNewInfo.Format("%s: %s \r\n", strParamName, strBuf);

	m_strPostHeader += strNewInfo;

	return FALSE;
}

/// ת���ı���ʽ�������Ƹ�ʽ
BOOL CHttpDownload::EncodeBuffer(BYTE *byarBuf, CString &strBuf)
{
	int nLength = strBuf.GetLength();
	
	strBuf.MakeUpper();
	CString strHexCmp = strBuf.SpanIncluding("0123456789ABCDEF");
	
	if (nLength != strHexCmp.GetLength()
		|| 0 == nLength 
		|| 0 != nLength%2)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	
	// �ַ������ȱ���պ�Ϊbyte�����С������
	ASSERT(AfxIsValidAddress(byarBuf, (UINT)(nLength/2)));

	for (int i = 0; i < nLength; i += 2)
	{
		TCHAR tcHi = 0;
		TCHAR tcLo = 0;

		tcHi = strBuf.GetAt(i);
		tcLo = strBuf.GetAt(i+1);

		tcHi = (tcHi >= _T('A')) ? (tcHi - 0x37) : (tcHi - _T('0'));
		tcLo = (tcLo >= _T('A')) ? (tcLo - 0x37) : (tcLo - _T('0'));
		
		byarBuf[i/2] = tcHi * 16 + tcLo;
	}

	return TRUE;
}

/// ת�������Ƹ�ʽ���ı���ʽ
BOOL CHttpDownload::DecodeBuffer(CString &strBuf, const BYTE *byarBuf, DWORD dwBufLen)
{
	ASSERT(0 != dwBufLen);
	ASSERT(AfxIsValidAddress(byarBuf, dwBufLen));

	LPTSTR pszBuf = strBuf.GetBuffer(dwBufLen * 2);

	for (DWORD i = 0; i < dwBufLen; ++i)
	{
		TCHAR tcHi = 0;
		TCHAR tcLo = 0;

		BYTE bySample = byarBuf[i];
		tcHi = bySample / 16;
		tcLo = bySample % 16;

		tcHi = (tcHi < 0xA) ? (tcHi + _T('0')) : (tcHi + 0x37);
		tcLo = (tcLo < 0xA) ? (tcLo + _T('0')) : (tcLo + 0x37);

		pszBuf[i*2]		= tcHi;
		pszBuf[i*2+1]	= tcLo;
	}

	strBuf.ReleaseBuffer();

	return TRUE;
}

BOOL CHttpDownload::CheckDirectoryExist(const CString& strFileName)
{
	DWORD dwAttri = ::GetFileAttributes(strFileName);

	if ((dwAttri == (-1)) || !(dwAttri & FILE_ATTRIBUTE_DIRECTORY))
		return FALSE;

	return TRUE;
}

BOOL CHttpDownload::CreateAllDirectory(const CString& strPath)
{	
	BOOL bRet = FALSE;

	for(int nPos = 0; nPos < strPath.GetLength(); ++nPos)
	{		
		nPos = strPath.Find('\\', nPos);
		if(nPos < 0)
		{
			break;
		}
		bRet = ::CreateDirectory(strPath.Left(nPos), NULL);		
	}

	return bRet;
}

/// ������������
BOOL CHttpDownload::SetNetwork(	BYTE byAccessType,
								LPCTSTR szProxyIP/* = NULL*/, 
								WORD wProxyPort/* = 0*/, 
								LPCTSTR szProxyUserName/* = NULL*/,
								LPCTSTR szProxyPassword/* = NULL*/ )
{
	m_strProxyIP		= _T("");
	m_wProxyPort		= 0;
	m_bProxyAuthentic	= FALSE;
	m_strProxyUserName	= _T("");
	m_strProxyPassword	= _T("");		

	switch (byAccessType) 
	{
	case PROXY_CUSTOM:
		{
			m_dwAccessType = INTERNET_OPEN_TYPE_PROXY;	
		}
		break;
	case PROXY_IE:
		{
			m_dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG;
		}
		break;
	default:
		{
			m_dwAccessType = INTERNET_OPEN_TYPE_DIRECT;
		}
	}

	if (PROXY_CUSTOM != byAccessType)
	{
		return TRUE;
	}

	if (NULL == szProxyIP || 0 == wProxyPort)
	{
		ASSERT(FALSE); // Both of them must be valid
		return FALSE;
	}

	m_strProxyIP = szProxyIP;
	m_wProxyPort = wProxyPort;

	if (NULL != szProxyUserName && NULL != szProxyPassword)
	{
		m_bProxyAuthentic = TRUE;
		m_strProxyUserName = szProxyUserName;
		m_strProxyPassword = szProxyPassword;		
	}

	if (m_bUseGlobalNetworkSettings)
	{
		s_dwAccessType		= m_dwAccessType;
		s_strProxyIP		= m_strProxyIP;
		s_wProxyPort		= m_wProxyPort;
		s_bProxyAuthentic	= m_bProxyAuthentic;
		s_strProxyUserName	= m_strProxyUserName;
		s_strProxyPassword	= m_strProxyPassword;	
	}

	return TRUE;
}

/// �Ƿ��������Ի���ѯ���û�������������û���������
void CHttpDownload::EnablePopupAuthenticDialog(BOOL bEnable/* = TRUE*/)
{
	m_bPopAuthenticDlg = bEnable;
}
	
/// �Ƿ�ʹ��ȫ�ִ�������
void CHttpDownload::EnableGlobalNetworkSettings(BOOL bEnable/* = TRUE*/)
{
	m_bUseGlobalNetworkSettings = bEnable;
}


//kongdong added
DWORD CHttpDownload::GetStatusCode()	//��ȡHTTP���صķ�����
{
	return m_dwStatusCode;
}

void CHttpDownload::SetMaxRetryTimes(int nMaxTimes) //����������Դ���,Ĭ��3��
{
	if (nMaxTimes < 1)
	{
		return;
	}
	else
	{
		m_nMaxRetryTime = nMaxTimes;
	}
}


void CHttpDownload::SetAutoAddCookieToHeadFlag( BOOL bChoice )
{
	m_bAutoAddCookie = bChoice;
}

 // ndef(_STANDALONE_USE_)

#undef HTTP_LOG