// HttpUploadFile.cpp: implementation of the CHttpUploadFile class.
//
// �޸ļ�¼��
// 041221 + ʹ��α���ӷ�ʽ��ʱ��ȡ������������û��������룬��platform��
// Mal����  һ���޸ĺ���ô�FlashGet��ȡ�ķ�ʽ��
//        + �����ϴ����ȵ���ʾ֧�֡�
//        + ���ӿ�ͨ���������Ƶ�LOG��
//        ~ ����Requestͷ���ĸ����ֶεĴ�Сд�����ϱ�׼����ڴ�Сд���е�
//          server����ʱ��������⡣
//        ~ �����ϴ�0�ֽڴ�С�ļ�ʱ�����BUG��
//
// 051107 ~ �������ļ����ض�û����Ч������
// Mal	  ? ��֪���������ʹ�ð�ͷ�д��ļ��ķ�ʽ��������������ܻΌ�ܽϴ�
//			�����󣬵��´���ʧ��
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpUploadFile.h"
#include "HttpDownloadUIProxy.h"

#include <Afxinet.h>

using namespace CHTTPUPLOADFILE;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Log����
#define HTTP_LOG
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpUploadFile::CHttpUploadFile()
{
	m_strBoundary = GenerateRandomReadableString();
	m_hFile = NULL;
	ResetFormData();
	m_strFileToUpload = "";
	m_dwFileSizeLimit = 1000 * 1024 * 1024;
	m_dwTotalWriteSize = 0;
	m_dwCurrentWriteSize = 0;
	m_bUploading = FALSE;
	ZeroMemory(m_byarBuf, c_dwBufferSize);
}

CHttpUploadFile::~CHttpUploadFile()
{

}

/// ���Form����
void CHttpUploadFile::ResetFormData()
{
	CancelDownload();
	m_bUploading = FALSE;
	m_strFileToUpload = "";
	m_strForm = "";
	m_strBoundary = GenerateRandomReadableString();
	m_dwTotalWriteSize = 0;
	m_dwCurrentWriteSize = 0;
}

/// ��Form���һ������ֵ
void CHttpUploadFile::AddFormData(const CString &strFieldName, const CString &strValue)
{
	CString strFormData;
	strFormData.Format(c_szFormDataFormat, m_strBoundary, strFieldName, strValue);

	m_strForm += strFormData;
}

/// �ϴ��ļ�
BOOL CHttpUploadFile::UploadFile(const CString &strField, 
								 const CString &strURL, 
								 const CString &strFile)
{
	if (strURL.IsEmpty() || strFile.IsEmpty())
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (FALSE == m_strFileToUpload.IsEmpty())
	{
		ASSERT(FALSE);
		// ***ÿһ���ϴ���֮�����ResetFormData���ϴ�
	
	}
	
	CString strShortFileName = strFile;
	int nSep = strShortFileName.ReverseFind(_T('\\'));
	strShortFileName = strShortFileName.Mid(nSep + 1);
	
	CString strFileHeader;
	strFileHeader.Format(c_szFormFileHeaderFormat, 
							m_strBoundary, 
							strField,
							strShortFileName);
	m_strForm += strFileHeader;

	m_strFileToUpload = strFile;

	SetAutoResume(FALSE);
	m_DownloadFileInfo.bForceDownloadWithResumeError = TRUE;
	m_DownloadFileInfo.dwRequestResumeBytes = 0;

	m_bUsePostVerb = TRUE;
	m_bUploading = FALSE;

	//modify by maisong at 2008-03-11
/*#ifndef _STANDALONE_USE_
	// ��ʹ��IE�������ã�ʹ��FlashGet��������
	SetUseIEProxy(FALSE); 
#endif*/

	return DownloadBase(strURL, GenerateTempFileName());
}

/// ��ʱȷ��Proxy�Ƿ���Ҫ��֤�ķ�����ֱ�Ӵ�Platform��������
BOOL CHttpUploadFile::ProbeProxyAuthentication(CString &strProxyUserName, 
											   CString &strProxyPassword)
{
	// original codes
	HINTERNET hOpen = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hReq = NULL;
	
	INTERNET_PORT dwPort;
	DWORD  dwFlags = INTERNET_FLAG_DONT_CACHE;
	DWORD dwError;
	DWORD  dwSize, dwCode;
	TCHAR szAccept[] = "*/*";
	LPSTR AcceptTypes[2]={0}; 
	AcceptTypes[0]=szAccept;
	
	CString strURL = _T("http://221.228.195.57:14080/eduup/up/log_file");
	CString strServerName;
	CString strObject;
	DWORD dwServiceType;

	if (!AfxParseURL((LPCTSTR)strURL, dwServiceType, strServerName, strObject, dwPort) )
	{
		return FALSE;
	}

    
	//ʹ����������ò��ܽ�����߳�:
	hOpen = ::InternetOpen (NULL,							// app name
							 INTERNET_OPEN_TYPE_PRECONFIG,	// access type
							 NULL,							// proxy server
							 INTERNET_INVALID_PORT_NUMBER,	// proxy port
							 0								// flags
							);



	// Initialize wininet.
	if (!hOpen)
	{
		return FALSE;
	}

	if ( !(hConnect = ::InternetConnect ( hOpen, strServerName , dwPort, "",  "", INTERNET_SERVICE_HTTP, 0  , 0) ) )
	{
		if (hOpen)
			::InternetCloseHandle (hOpen);
		return FALSE;
	}


	if ( !(hReq = ::HttpOpenRequest (hConnect, "GET", strObject, HTTP_VERSION, "", 
									(LPCTSTR*) AcceptTypes, dwFlags ,0 )))
	{
		if (hConnect)
			::InternetCloseHandle (hConnect);
		if (hOpen)
			::InternetCloseHandle (hOpen);
		return FALSE;
	}


	if (!::HttpSendRequest (hReq, NULL, 0, NULL, 0) )
	{
		dwError = ::GetLastError () ;
		if ( dwError == ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED)
		{
			//This works but there is no way to iterate through the client certs
			DWORD dwCert = 0;
			::InternetSetOption(hReq, INTERNET_OPTION_SECURITY_SELECT_CLIENT_CERT, 
							  &dwCert, sizeof(dwCert));
            ::HttpSendRequest (hReq, NULL, 0, NULL, 0);
		}
	}

	dwSize = sizeof (DWORD) ;  
    if(!::HttpQueryInfo (hReq, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwCode, &dwSize, NULL))
	{
		if (hReq)
			::InternetCloseHandle (hReq);
		if (hConnect)
			::InternetCloseHandle (hConnect);
		if (hOpen)
			::InternetCloseHandle (hOpen);
		return FALSE;
	}

	if(dwCode == HTTP_STATUS_PROXY_AUTH_REQ ||
		dwCode == HTTP_STATUS_DENIED)
	{
		
		if ( ::InternetErrorDlg (GetDesktopWindow(),
			hReq, ERROR_INTERNET_INCORRECT_PASSWORD,
			FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
			FLAGS_ERROR_UI_FLAGS_GENERATE_DATA |
			FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
			NULL) == ERROR_INTERNET_FORCE_RETRY)
		{

			DWORD dwLen = 64;
			::InternetQueryOption(hReq, INTERNET_OPTION_PROXY_USERNAME, strProxyUserName.GetBuffer(dwLen), &dwLen);
			strProxyUserName.ReleaseBuffer();
			
			dwLen = 64;
			::InternetQueryOption(hReq, INTERNET_OPTION_PROXY_PASSWORD, strProxyPassword.GetBuffer(dwLen), &dwLen);
			strProxyPassword.ReleaseBuffer();							
		}
		else
		{
			if (hReq)
				::InternetCloseHandle (hReq);
			if (hConnect)
				::InternetCloseHandle (hConnect);
			if (hOpen)
				::InternetCloseHandle (hOpen);
			return FALSE;
		}
	}

	if (hReq)
		::InternetCloseHandle (hReq);
	if (hConnect)
		::InternetCloseHandle (hConnect);
	if (hOpen)
		::InternetCloseHandle (hOpen);

	return TRUE;	
}


/// ���ص�HttpSendRequest����
/** �ִη����ļ���Ϣ
 *  
 */
UINT CHttpUploadFile::HttpSendRequest()
{
	ASSERT(m_hHttpFile != NULL);

	m_bUploading = TRUE;

	TRACE(_T("------> ASYNC INET INFO: ::HttpSendRequest(...) From CHttpUploadFile\n"));
	
	// ����ָ�����Ϣ��Header
	CString strBoundaryInfo;
	strBoundaryInfo.Format(c_szFormRootFormat, m_strBoundary); //= _T("Content-type: multipart/form-data, boundary=?????\r\n");
	HttpSetExternalHeader(strBoundaryInfo);

	if (FALSE == OpenFile())
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}

	UINT uRet = BeginRequest();
	if (DOWNLOAD_RESULT_CONTINUE != uRet)
	{
		return uRet;
	}

	uRet = WriteRequest();
	if (DOWNLOAD_RESULT_CONTINUE != uRet)
	{
		return uRet;
	}

	uRet = EndRequest();
	if (DOWNLOAD_RESULT_CONTINUE != uRet)
	{
		return uRet;
	}
	
	m_bUploading = FALSE;

	return DOWNLOAD_RESULT_CONTINUE;
}

/// ���ļ�
BOOL CHttpUploadFile::OpenFile()
{
	ASSERT(NULL == m_hFile); // �ļ��Ѵ򿪣�
	ASSERT(FALSE == m_strFileToUpload.IsEmpty()); // �ļ���Ϊ�գ�

	m_hFile = ::CreateFile(m_strFileToUpload, GENERIC_READ, FILE_SHARE_READ,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
     
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		HTTP_LOG("CreateFile Failed\n");
		return FALSE;
	}

	DWORD dwFileSize = 0;
	dwFileSize = ::GetFileSize(m_hFile, NULL);

	if (INVALID_FILE_SIZE == dwFileSize || m_dwFileSizeLimit < dwFileSize)
	{
		ASSERT(FALSE);
		// �ļ�̫��
		// ***ע��m_dwFileSizeLimit����ʼ��Ϊ0��ʹ��CHttpUploadFileʱ���
		// ***��ʹ��SetFileSizeLimit�����ļ���С����
	}

	return TRUE;
}

/// ����ļ��Ѵ򿪣��ر��ļ����
inline void CHttpUploadFile::CloseFileIfOpened()
{
	if (NULL != m_hFile)
	{
		::CloseHandle(m_hFile);
	}

	m_hFile = NULL;
}

/// ��ʼ��дRequest
UINT CHttpUploadFile::BeginRequest()
{
	ASSERT(NULL != m_hFile);

	DWORD dwFileSize = ::GetFileSize(m_hFile, NULL);
	if (INVALID_FILE_SIZE == dwFileSize)
	{
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}
	
	INTERNET_BUFFERS bufferIn;
	ZeroMemory(&bufferIn, sizeof(INTERNET_BUFFERS));
	bufferIn.dwStructSize = sizeof(INTERNET_BUFFERS);
	bufferIn.dwBufferTotal =	m_strForm.GetLength() 
								+ dwFileSize
								+ GetFormCompletionLength();

	m_dwTotalWriteSize = bufferIn.dwBufferTotal; // ����Ŀ��д���С
	m_dwCurrentWriteSize = 0;

	if (FALSE == ::HttpSendRequestEx(m_hHttpFile, 
									 &bufferIn, 
									 0, 
									 HSR_ASYNC | HSR_INITIATE, 
									 0))
	{
		UINT uRet = IsAsyncIOSucceeded();
		
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			HTTP_LOG("SendRequest Failed(0x%x). %d\n", uRet, __LINE__);
			return uRet;
		}
	}
	else
	{
		// ͬ��IO���
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// ��дRequest
UINT CHttpUploadFile::WriteRequest()
{
	DWORD	dwReadSize = 0;
	DWORD	dwWrittenSize = 0;
	
	CString strTmp = m_strForm;

	// ��дForm
	do
	{
		ReadString(m_byarBuf, strTmp, &dwReadSize);

		UINT uRet = InternetWriteFile(m_byarBuf, dwReadSize);
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			return uRet;
		}

		OnProgress(dwReadSize);
				
	} while (dwReadSize == c_dwBufferSize);
    	

	// д�ļ�
    do
	{
		if (FALSE == ::ReadFile(m_hFile, m_byarBuf, c_dwBufferSize, &dwReadSize, NULL))
		{
			CloseFileIfOpened();
			return DOWNLOAD_RESULT_FAILED;
		}

		UINT uRet = InternetWriteFile(m_byarBuf, dwReadSize);
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			return uRet;
		}

		OnProgress(dwReadSize);

	} while (dwReadSize == c_dwBufferSize);

	CloseFileIfOpened();

	return DOWNLOAD_RESULT_CONTINUE;
}


/// ������дRequest
UINT CHttpUploadFile::EndRequest()
{
	DWORD	dwReadSize = 0;
	DWORD	dwWrittenSize = 0;

	CString strTmp = GetFormCompletion();
	CString strSrc;
	BYTE *pSrc = NULL;
	
	do 
	{
		ReadString(m_byarBuf, strTmp, &dwReadSize);
		
		UINT uRet = InternetWriteFile(m_byarBuf, dwReadSize);
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			HTTP_LOG("InternetWriteFile Failed(0x%x). %d\n", uRet, __LINE__);
			return uRet;
		}

		OnProgress(dwReadSize);
		
	} while(dwReadSize == c_dwBufferSize);


	if(FALSE == HttpEndRequest(m_hHttpFile, NULL, 0, 0))
	{
		UINT uRet = IsAsyncIOSucceeded();
		
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			HTTP_LOG("EndRequest Failed(0x%x). %d\n", uRet, __LINE__);
			return uRet;
		}
	}
	else
	{
		// ��ͬ�����		
	}
	
	return DOWNLOAD_RESULT_CONTINUE;
}

/// ��HttpFileд��
UINT CHttpUploadFile::InternetWriteFile(BYTE *byarBuf, DWORD dwBufLen)
{
	if (0 == dwBufLen)
	{
		return DOWNLOAD_RESULT_CONTINUE;
	}

	ASSERT(AfxIsValidAddress(byarBuf, dwBufLen));

	DWORD dwWrittenSize = 0;
	if (FALSE == ::InternetWriteFile(m_hHttpFile, byarBuf, dwBufLen, &dwWrittenSize))
	{
		UINT uRet = IsAsyncIOSucceeded();
		
		if (DOWNLOAD_RESULT_CONTINUE != uRet)
		{
			HTTP_LOG("InternetWriteFile Failed(0x%x). %d\n", uRet, __LINE__);
			return uRet;
		}
	}
	else
	{
		// ��ͬ�����		
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// �첽IO��ͨ�óɰܼ������
/** �ɹ�����DOWNLOAD_RESULT_CONTINUE������������׳��쳣
 *  
 */
UINT CHttpUploadFile::IsAsyncIOSucceeded()
{
	DWORD dwError = ::GetLastError();

	if (ERROR_IO_PENDING != dwError)
	{
		// ���첽���Ҳ��ɹ�
		ASSERT(FALSE);
		HTTP_LOG("AsyncIO Error(0x%x). %d\n", dwError, __LINE__);
		m_bUploading = FALSE;
		CloseFileIfOpened();
		AfxThrowInternetException(NULL, dwError);
	}
	
	HANDLE hEvt;
	if (CThreadModel::QueryEvent(hEvt))
	{
		// �û��ж�
		HTTP_LOG("AsyncIO UserBreak.\n");
		m_bUploading = FALSE;
		CloseFileIfOpened();
		return DOWNLOAD_RESULT_USER_ABORT;
	}
	
	if (hEvt == m_hEvtRequestComplete)
	{
		if (m_dwError != ERROR_SUCCESS) 
		{
			// ��ɵ����ɹ�
			HTTP_LOG("AsyncIO Error(0x%x). %d\n", m_dwError, __LINE__);
			m_bUploading = FALSE;
			CloseFileIfOpened();
			AfxThrowInternetException(NULL, m_dwError);
		}
	}
	else
	{
		// û�����
		HTTP_LOG("AsyncIO Not Complete. %d\n", __LINE__);
		m_bUploading = FALSE;
		CloseFileIfOpened();
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// ���ַ������ݸ��Ƶ���������ע�����в��������ݶ��ᱻ�ı䣡��
/** ����ַ������ȴ��ڻ��峤�ȣ����ƻ��峤�����ݺ��ַ����ض�
 *  ����ַ������Ȳ����ڻ��峤�ȣ����ƻ��峤�����ݺ��ַ����ÿ�
 *  ���ص�dwReadSize��ʵ�ʶ������ֽ�����Ҳ��byarBuf�е�ʵ����Ч����
 */
void CHttpUploadFile::ReadString(BYTE *byarBuf, CString &strTmp, DWORD *pdwReadSize)
{
	ASSERT(AfxIsValidAddress(byarBuf, c_dwBufferSize));
	ASSERT(NULL != pdwReadSize);

	CString strSrc;
	BYTE	*pSrc = NULL;

	if ((DWORD)strTmp.GetLength() > c_dwBufferSize)
	{
		strSrc = strTmp.Left(c_dwBufferSize);
		pSrc = (BYTE*) strSrc.GetBuffer(strSrc.GetLength());
		
		CopyMemory(byarBuf, pSrc, c_dwBufferSize);
		
		*pdwReadSize = c_dwBufferSize;
		
		strSrc.ReleaseBuffer();
		strTmp = strTmp.Mid(c_dwBufferSize);
	}
	else
	{
		int nLen = strTmp.GetLength();
		pSrc = (BYTE*) strTmp.GetBuffer(nLen);
		
		CopyMemory(byarBuf, pSrc, nLen);
		
		*pdwReadSize = nLen;
		
		strTmp.ReleaseBuffer();
		strTmp = "";
	}
}

/// ��������Ŀɶ��ַ�����������Ϊ�ָ�������ʱ�ļ�����Ĭ��Ϊ6�ֽڳ�
CString CHttpUploadFile::GenerateRandomReadableString(int nLength/* = 6 */)
{
	static const char c_szBoundaryCharSet[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0";

	CString strBoundary = "";
	::srand((unsigned int)::time(NULL));
	
	for (int i = 0; i < nLength; ++i)
	{
		int n = int(62 * ( float(::rand()) / RAND_MAX ));
		strBoundary += c_szBoundaryCharSet[n];
	}

	return strBoundary;
}

/// ��ȡForm������ǵĳ���
int CHttpUploadFile::GetFormCompletionLength()
{
	return GetFormCompletion().GetLength();
}

/// ��ȡForm�Ľ������
CString CHttpUploadFile::GetFormCompletion()
{
	CString strFmt;
	strFmt.Format(c_szFormCompletionFormat, m_strBoundary);

	return strFmt;
}

/// �����ļ���С����
void CHttpUploadFile::SetFileSizeLimit(DWORD dwSize)
{
	m_dwFileSizeLimit = dwSize;
}

// ������ʱĿ¼�µ�����ļ���
CString	CHttpUploadFile::GenerateTempFileName()
{
	CString strPath = GetSystemTempPath();

	CString strFileName = GenerateRandomReadableString(8);

	strPath += strFileName;

	return strPath;
}


/// ��ȡϵͳ��ʱ�ļ���·��
/** ��98�£����ܳ���TEMP��TMP���������������ڵ�����𣿣�
 *  ������֣���MSDN˵�������ص�ǰĿ¼��
 *
 *  ����еõ�����ʱ�ļ��г��ȴ���MAX_PATH���������
 *  ��ͼ����c:\temp������޷�����֮����ֻ�÷���C:\��
 */
CString CHttpUploadFile::GetSystemTempPath()
{
	TCHAR szDstPath[MAX_PATH];
	
	DWORD dwLen = ::GetTempPath(MAX_PATH, szDstPath);

	if (dwLen > MAX_PATH)
	{
		CString strPath = _T("c:\\temp\\");
		
		DWORD dwAttrib = ::GetFileAttributes(strPath);

		if ((-1 == dwAttrib) || 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			if (FALSE == ::CreateDirectory(strPath, NULL))
			{
				return CString("c:\\"); // ʵ��û�취������£��������;
			}
		}

		return strPath;		
	}
	else
	{
		return CString(szDstPath);
	}
}

/// �ж��ϴ�����
void CHttpUploadFile::CancelUpload()
{
	m_bUploading = FALSE;
	CancelDownload();
}

/// ��ӳ�ϴ����ȣ�����Ϊ��ǰ����
void CHttpUploadFile::OnProgress(DWORD dwIncrement)
{
	if (FALSE == m_bUploading)
	{
		return;
	}

	if (0 == m_dwTotalWriteSize)
	{
		return;
	}
	
	ASSERT((m_dwTotalWriteSize - m_dwCurrentWriteSize - dwIncrement) < m_dwTotalWriteSize);

	m_dwCurrentWriteSize += dwIncrement;

	if (m_hwndUIProxy == NULL)
	{
		return;
	}

	DWORD pBuffer[2];
	LPVOID pMem = NULL;

	int nSize = sizeof(DWORD) * 2;
	pBuffer[0] = m_dwCurrentWriteSize;
	pBuffer[1] = m_dwTotalWriteSize;
		
	if (!CreateHeapData(pBuffer, nSize, pMem))
	{
		return;
	}

	::PostMessage(m_hwndUIProxy, WM_DOWNLOAD_PROGRESS, (WPARAM)this, (LPARAM)pMem);
}





#undef HTTP_LOG