// HttpUploadFile.cpp: implementation of the CHttpUploadFile class.
//
// 修改记录：
// 041221 + 使用伪连接方式临时获取代理服务器的用户名与密码，待platform进
// Mal　　  一步修改后改用从FlashGet获取的方式。
//        + 增加上传进度的显示支持。
//        + 增加可通过编译宏控制的LOG。
//        ~ 修正Request头部的个别字段的大小写不符合标准造成在大小写敏感的
//          server处理时出错的问题。
//        ~ 修正上传0字节大小文件时出错的BUG。
//
// 051107 ~ 修正了文件名截短没有生效的问题
// Mal	  ? 已知问题存在于使用包头夹带文件的方式，代理服务器可能会抗拒较大
//			的请求，导致传送失败
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

// Log设置
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

/// 清空Form内容
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

/// 向Form添加一个域及其值
void CHttpUploadFile::AddFormData(const CString &strFieldName, const CString &strValue)
{
	CString strFormData;
	strFormData.Format(c_szFormDataFormat, m_strBoundary, strFieldName, strValue);

	m_strForm += strFormData;
}

/// 上传文件
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
		// ***每一次上传完之后必须ResetFormData再上传
	
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
	// 不使用IE代理设置，使用FlashGet代理设置
	SetUseIEProxy(FALSE); 
#endif*/

	return DownloadBase(strURL, GenerateTempFileName());
}

/// 临时确认Proxy是否需要验证的方法，直接从Platform拷过来的
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

    
	//使用浏览器设置才能进入该线程:
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


/// 重载的HttpSendRequest方法
/** 分次发送文件信息
 *  
 */
UINT CHttpUploadFile::HttpSendRequest()
{
	ASSERT(m_hHttpFile != NULL);

	m_bUploading = TRUE;

	TRACE(_T("------> ASYNC INET INFO: ::HttpSendRequest(...) From CHttpUploadFile\n"));
	
	// 插入分隔符信息到Header
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

/// 打开文件
BOOL CHttpUploadFile::OpenFile()
{
	ASSERT(NULL == m_hFile); // 文件已打开？
	ASSERT(FALSE == m_strFileToUpload.IsEmpty()); // 文件名为空？

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
		// 文件太大
		// ***注意m_dwFileSizeLimit被初始化为0，使用CHttpUploadFile时务必
		// ***先使用SetFileSizeLimit设置文件大小限制
	}

	return TRUE;
}

/// 如果文件已打开，关闭文件句柄
inline void CHttpUploadFile::CloseFileIfOpened()
{
	if (NULL != m_hFile)
	{
		::CloseHandle(m_hFile);
	}

	m_hFile = NULL;
}

/// 开始书写Request
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

	m_dwTotalWriteSize = bufferIn.dwBufferTotal; // 设置目标写入大小
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
		// 同步IO完成
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// 书写Request
UINT CHttpUploadFile::WriteRequest()
{
	DWORD	dwReadSize = 0;
	DWORD	dwWrittenSize = 0;
	
	CString strTmp = m_strForm;

	// 书写Form
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
    	

	// 写文件
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


/// 结束书写Request
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
		// 已同步完成		
	}
	
	return DOWNLOAD_RESULT_CONTINUE;
}

/// 向HttpFile写入
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
		// 已同步完成		
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// 异步IO的通用成败检查流程
/** 成功返回DOWNLOAD_RESULT_CONTINUE，否则出错，或抛出异常
 *  
 */
UINT CHttpUploadFile::IsAsyncIOSucceeded()
{
	DWORD dwError = ::GetLastError();

	if (ERROR_IO_PENDING != dwError)
	{
		// 非异步，且不成功
		ASSERT(FALSE);
		HTTP_LOG("AsyncIO Error(0x%x). %d\n", dwError, __LINE__);
		m_bUploading = FALSE;
		CloseFileIfOpened();
		AfxThrowInternetException(NULL, dwError);
	}
	
	HANDLE hEvt;
	if (CThreadModel::QueryEvent(hEvt))
	{
		// 用户中断
		HTTP_LOG("AsyncIO UserBreak.\n");
		m_bUploading = FALSE;
		CloseFileIfOpened();
		return DOWNLOAD_RESULT_USER_ABORT;
	}
	
	if (hEvt == m_hEvtRequestComplete)
	{
		if (m_dwError != ERROR_SUCCESS) 
		{
			// 完成但不成功
			HTTP_LOG("AsyncIO Error(0x%x). %d\n", m_dwError, __LINE__);
			m_bUploading = FALSE;
			CloseFileIfOpened();
			AfxThrowInternetException(NULL, m_dwError);
		}
	}
	else
	{
		// 没有完成
		HTTP_LOG("AsyncIO Not Complete. %d\n", __LINE__);
		m_bUploading = FALSE;
		CloseFileIfOpened();
		ASSERT(FALSE);
		return DOWNLOAD_RESULT_FAILED;
	}

	return DOWNLOAD_RESULT_CONTINUE;
}

/// 把字符串数据复制到缓冲区（注意所有参数的内容都会被改变！）
/** 如果字符串长度大于缓冲长度，复制缓冲长度数据后将字符串截短
 *  如果字符串长度不大于缓冲长度，复制缓冲长度数据后将字符串置空
 *  返回的dwReadSize是实际读到的字节数，也即byarBuf中的实际有效长度
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

/// 生成随机的可读字符串，用于作为分隔符及临时文件名，默认为6字节长
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

/// 获取Form结束标记的长度
int CHttpUploadFile::GetFormCompletionLength()
{
	return GetFormCompletion().GetLength();
}

/// 获取Form的结束标记
CString CHttpUploadFile::GetFormCompletion()
{
	CString strFmt;
	strFmt.Format(c_szFormCompletionFormat, m_strBoundary);

	return strFmt;
}

/// 设置文件大小上限
void CHttpUploadFile::SetFileSizeLimit(DWORD dwSize)
{
	m_dwFileSizeLimit = dwSize;
}

// 生成临时目录下的随机文件名
CString	CHttpUploadFile::GenerateTempFileName()
{
	CString strPath = GetSystemTempPath();

	CString strFileName = GenerateRandomReadableString(8);

	strPath += strFileName;

	return strPath;
}


/// 获取系统临时文件夹路径
/** 在98下，可能出现TEMP跟TMP环境变量都不存在的情况吗？！
 *  如果出现，按MSDN说，将返回当前目录。
 *
 *  如果有得到的临时文件夹长度大于MAX_PATH的情况，将
 *  试图创建c:\temp，如果无法创建之，就只好返回C:\了
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
				return CString("c:\\"); // 实在没办法的情况下，返回这个;
			}
		}

		return strPath;		
	}
	else
	{
		return CString(szDstPath);
	}
}

/// 中断上传动作
void CHttpUploadFile::CancelUpload()
{
	m_bUploading = FALSE;
	CancelDownload();
}

/// 反映上传进度，参数为当前增量
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