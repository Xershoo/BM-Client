
#include "stdafx.h"
#include "EC_HttpGet.h"
#include <process.h>
#include <ATLBASE.H>
#include <stdio.h>
#pragma comment(lib, "wininet.lib")

extern int		_nCompleteSize;

namespace PATCH
{

	enum URL_ERROR_CODE
	{
		NET_ERROR = 1000,
		FILE_DONE,
		RET_ERROR,
		ERR_UNKNOWN,
		ERR_GET_ADDR,
		ERR_URL_PARSE,
		ERR_REQ_DENIED,
		ERR_CREATE_FILE,
		ERR_OVERFLOW,
	};

/////////////////////////////////////////////////////////////////////////////
// exception handling

void ThrowInternetException(DWORD dwError /* = 0 */)
{
	if (dwError == 0)
		dwError = ::GetLastError();

	
}

/////////////////////////////////////////////////////////////////////////////

CHttpDownloader::CHttpDownloader(int id, DownloadService* mgr) 
	: m_id(id), m_status(0)
{
	m_mgr = mgr;
	m_hConnect = NULL;
	m_hRequest = NULL;	
	m_hSession = InternetOpen(TEXT("LiveUpdate Application"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!m_hSession)
		return;

	/// Set time out
	DWORD dwTimeout = 60000;
	InternetSetOptionEx(m_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeout, sizeof(dwTimeout), 0);
	dwTimeout = 90000;
	InternetSetOptionEx(m_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &dwTimeout, sizeof(dwTimeout), 0);
}

CHttpDownloader::~CHttpDownloader()
{
	CloseInternetHandle(m_hRequest);
	CloseInternetHandle(m_hConnect);
	CloseInternetHandle(m_hSession);
}

void CHttpDownloader::CloseInternetHandle(HINTERNET &hInternet)
{
	if(hInternet)
	{
		InternetCloseHandle(hInternet);
		hInternet = NULL;
	}
}

void __cdecl WorkerThread(void* p)
{
	CHttpDownloader* worker = (CHttpDownloader*)p;
	worker->Run();
}

void CHttpDownloader::Run()
{
	try
	{
		DWORD dwRet = Connect(m_url);

		if (dwRet==HTTP_STATUS_MOVED || dwRet==HTTP_STATUS_REDIRECT || dwRet==HTTP_STATUS_REDIRECT_METHOD)
		{
			dwRet = Redirect();
		}

		if (dwRet > HTTP_STATUS_PARTIAL_CONTENT)
		{
			m_error = dwRet;
			m_status = NET_ERROR;
		}
		else
		{
			if(m_offset<=0)
			{
				ReadHeader();
			}
			Read();
			m_status = FILE_DONE;
		}
	}
	catch (int code){
		m_status = RET_ERROR;
		m_error = code;
	}
	catch (...)
	{
		m_status = RET_ERROR;
		m_error = ERR_UNKNOWN;
	}

	if(m_mgr)
	{		

		if(m_status == RET_ERROR || m_status == NET_ERROR)
		{
			m_mgr->OnClientStop(this, false);
		}
		else
		{
			m_mgr->OnClientStop(this, true);
		}
	}
	delete this;
}

DWORD CHttpDownloader::Redirect()
{
	TCHAR strNewLocation[INTERNET_MAX_URL_LENGTH];
	DWORD infoSize = INTERNET_MAX_URL_LENGTH;
	if (HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation, &infoSize, 0))
	{
		strNewLocation[infoSize] = 0;
 	}

	TCHAR* cFind = _tcsstr(strNewLocation, TEXT("Location: "));
	if(!cFind)
		throw ERR_GET_ADDR;

	TCHAR* strLocation = cFind + _tcslen(TEXT("Location: "));
	cFind = _tcsstr(strLocation, TEXT("\n"));
	if(cFind)
		*cFind = 0;

	CloseInternetHandle(m_hRequest);
	CloseInternetHandle(m_hConnect);

	_tcscpy(m_url, strLocation);
	return Connect(m_url);
}

DWORD CHttpDownloader::QueryFileSize(LPCTSTR url)
{
	_tcscpy(m_url, url);
	_tcscpy(m_szVerb, TEXT("HEAD"));
	if (Connect(m_url) > HTTP_STATUS_PARTIAL_CONTENT)
	{
		return 0;
	}

	DWORD dwFileSize = 0;
	DWORD dwSize = sizeof(DWORD);
	if (!HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwFileSize, &dwSize, 0))
	{
		return 0;
	}

	return dwFileSize;
}

void CHttpDownloader::Download(LPCTSTR url, LPCTSTR fn,  bool bBlocking, int start)
{
	_tcscpy(m_url, url);
	_tcscpy(m_filename, fn);
	_tcscpy(m_szVerb, TEXT("GET"));
	m_offset = start;
	if(bBlocking)
	{
		Run();
	}
	else
	{
		_beginthread(WorkerThread, 0, this);
	}
}

DWORD CHttpDownloader::Connect(LPCTSTR url)
{
	const TCHAR szHeaders[] = TEXT("Accept: */*\r\nUser-Agent: Mozilla/4.0\r\n");

	if( !AnalysisUrl(url) )
		throw ERR_URL_PARSE;

	if (!m_hSession)
		ThrowInternetException(ERROR_INVALID_HANDLE);

	m_hConnect = InternetConnect(m_hSession, m_host, m_port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (!m_hConnect)
		ThrowInternetException();

	m_hRequest = HttpOpenRequest(m_hConnect, m_szVerb, m_path, NULL, NULL, NULL, SESSION_FLAG, 1);
	if (!m_hRequest)
		throw ERR_REQ_DENIED;

	HttpAddRequestHeaders(m_hRequest, szHeaders, szHeaders?_tcslen(szHeaders):0, HTTP_ADDREQ_FLAG_ADD);
	if(m_offset>0)
	{
		TCHAR range[MAX_PATH];
		_stprintf(range, TEXT("RANGE: bytes= %d -"), m_offset);
		HttpAddRequestHeaders(m_hRequest, range, _tcslen(range), HTTP_ADDREQ_FLAG_ADD);
	}
	else
	{
		m_offset = 0;
	}

	BOOL bRet = HttpSendRequest(m_hRequest, NULL, 0, NULL, 0);
	if (!bRet)
		ThrowInternetException();

	DWORD dwRet;
	TCHAR szBuf[80];
	DWORD infoSize = sizeof(szBuf)/sizeof(szBuf[0]);
	if (HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE, szBuf, &infoSize, 0))
	{
		szBuf[infoSize] = 0;
		dwRet = _ttol(szBuf);
	}
	return dwRet;
}

void CHttpDownloader::ReadHeader()
{
	TCHAR szBuf[32];
	DWORD infoSize = sizeof(szBuf);
	if (HttpQueryInfo(m_hRequest, HTTP_QUERY_CONTENT_LENGTH, szBuf, &infoSize, 0))
	{
		szBuf[infoSize] = 0;
		m_size = _ttol((TCHAR*)szBuf);
	}
	else
		m_size = 0;

	SetFileAttributes(m_filename, FILE_ATTRIBUTE_NORMAL);
	FILE* fp = _tfopen(m_filename, TEXT("wb"));
	if(!fp)
		throw ERR_CREATE_FILE;
	if(m_size>0){
		fseek(fp, m_size-1, SEEK_SET);
		fputc(0, fp);
	}
	
	fclose(fp);
}

void CHttpDownloader::Read()
{
	FILE* fp = _tfopen(m_filename, TEXT("rb+"));
	if(!fp)
		throw ERR_CREATE_FILE;
	unsigned int maxsize = fseek(fp,0, SEEK_END);
	maxsize = ftell(fp);
	if(maxsize==0)
		maxsize = MAXFILESIZE;

	fseek(fp, m_offset, SEEK_SET );

	char sz[4097];
	DWORD nRead = 1, total = 0;

	while(nRead > 0) 
	{
		if (!InternetReadFile(m_hRequest, sz, 4096, &nRead))
			ThrowInternetException();
		if((unsigned int)(m_offset+nRead)>maxsize)
		{
			fclose(fp);
			throw ERR_OVERFLOW;
		}
		if(nRead>0)
		{
			fwrite(sz, 1, nRead, fp);
			total += nRead;
			m_offset += nRead;
			_nCompleteSize += nRead;
			if(m_offset >= maxsize)
			{				
				total = 0;
				break;
			}
		}
	}
	fclose(fp);
}

BOOL CHttpDownloader::AnalysisUrl(LPCTSTR url)
{
	URL_COMPONENTS urlCom;
	memset(&urlCom, 0, sizeof(URL_COMPONENTS));
	urlCom.dwStructSize = sizeof(URL_COMPONENTS);
	const DWORD BUF_LEN = INTERNET_MAX_URL_LENGTH;

	TCHAR host[BUF_LEN+1];
	urlCom.lpszHostName = host;
	urlCom.dwHostNameLength = BUF_LEN;

	TCHAR path[BUF_LEN+1];
	urlCom.lpszUrlPath = path;
	urlCom.dwUrlPathLength = BUF_LEN;

	TCHAR extra[BUF_LEN+1];
	urlCom.lpszExtraInfo = extra;
	urlCom.dwExtraInfoLength = BUF_LEN;

	if(!InternetCrackUrl(url, 0, ICU_ESCAPE, &urlCom))
		return FALSE;
	
	m_port = urlCom.nPort;
	_tcscpy(m_host, host);
	_tcscpy(m_path, path);
	return TRUE;
}

}//namespace
