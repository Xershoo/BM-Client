#pragma once

#include "download_service.h"
#include <WinInet.h>
namespace PATCH
{

#define BLOCK_SIZE      8192
#define MAXFILESIZE     1073741824
#define SESSION_FLAG   INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT \
						| INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE

void __cdecl WorkerThread(void* p);
void ThrowInternetException(DWORD dwError = 0);

class CHttpDownloader
{
protected:
	int m_id;
	int m_offset;
	int m_size;
	int m_status;
	int m_error;

	TCHAR		m_url[INTERNET_MAX_URL_LENGTH];
	TCHAR		m_filename[MAX_PATH];
	TCHAR		m_szVerb[10];

	DownloadService*   m_mgr;
	

	DWORD Redirect();
	DWORD Connect(LPCTSTR url);

	void Read();
	void ReadHeader();

public:
	CHttpDownloader(int id, DownloadService* mgr);
	~CHttpDownloader();
	
	void Run();

	DWORD QueryFileSize(LPCTSTR url);
	void Download(LPCTSTR url, LPCTSTR fn, bool bBlocking,  int start=-1);

	int GetID() { return m_id; }
	int GetError() { return m_error; }
	unsigned int GetSize() { return m_size; }
	unsigned int GetOffset() { return m_offset; }
	const LPCTSTR GetFileName() { return m_filename; }
	const LPCTSTR GetUrl() { return m_url; }

private:;
	TCHAR		m_host[INTERNET_MAX_HOST_NAME_LENGTH];
	int			m_port;
	TCHAR		m_path[INTERNET_MAX_PATH_LENGTH];

	HINTERNET	m_hSession;
	HINTERNET	m_hConnect;
	HINTERNET	m_hRequest;

	BOOL		AnalysisUrl(LPCTSTR url);
	void		CloseInternetHandle(HINTERNET &hInternet);

};

};
