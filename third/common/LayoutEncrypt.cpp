#include "stdafx.h"
#include "LayoutEncrypt.h"
#include "rc4.h"

#define ENCRYPT_KEY		"%^AVEG)(*JKLKLJ*& 981iorj!#@%ASDfoijpaf#$%89uasdf^&(()_|}KLIO"

BOOL CLayoutEncrypt::EncodeFile(LPCTSTR szSource, LPCTSTR szTarget)
{
	if (!szSource || !szTarget)
		return FALSE;

	HANDLE hTargetFile = INVALID_HANDLE_VALUE;
	PVOID pReadBuffer = NULL;
	DWORD dwFileSize = 0;
	PUCHAR pResultData = NULL;
	BOOL bEncodeOK = FALSE;

	do 
	{
		// ȡ��Դ�ļ�����
		if (!GetFileData(szSource, &pReadBuffer, &dwFileSize))
			break;

		// ����Ŀ���ڴ�
		pResultData = (PUCHAR)malloc(dwFileSize);
		if (!pResultData)
		{
			ATLTRACE(_T("Allocate %d Bytes Memory for Encrypt Error\r\n"), dwFileSize);
			break;
		}

		// ������������
		INT nResultLen = 0;
		memset(pResultData, 0, dwFileSize);
		if (RC4((PUCHAR)pReadBuffer, dwFileSize, (PUCHAR)ENCRYPT_KEY, strlen(ENCRYPT_KEY), pResultData, &nResultLen) == 0)
		{
			ATLTRACE(_T("RC4 Encrypt with Size %d Error\r\n"), dwFileSize);
			break;
		}

		// ��Ŀ���ļ������������һ��
		hTargetFile = ::CreateFile(szTarget, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hTargetFile == INVALID_HANDLE_VALUE)
		{
			Sleep(100);
			hTargetFile = ::CreateFile(szTarget, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		}

		// Ŀ���ļ��޷��򿪣�����
		if (hTargetFile == INVALID_HANDLE_VALUE)
		{
			ATLTRACE(_T("Open Target File %s Error: %d\r\n"), szTarget, ::GetLastError());
			break;
		}

		// ������д���ļ�
		if (!::WriteFile(hTargetFile, pResultData, nResultLen, &dwFileSize, NULL))
		{
			ATLTRACE(_T("WriteFile %s Error: %d\r\n"), szTarget, ::GetLastError());
			break;
		}

		bEncodeOK = TRUE;
	}
	while (FALSE);

	if (pReadBuffer)
	{
		free(pReadBuffer);
	}
	if (pResultData)
	{
		free(pResultData);
	}
	if (hTargetFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hTargetFile);
	}

	return bEncodeOK;
}

BOOL CLayoutEncrypt::DecodeFile(LPCTSTR szPath, wstring& strContent)
{
	if (!szPath)
		return FALSE;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	PVOID pReadBuffer = NULL;
	DWORD dwFileSize = 0;
	PUCHAR pResultData = NULL;
	PWCHAR pXMLString = NULL;
	BOOL bDecodeOK = FALSE;

	do 
	{
		// ȡ��Դ�ļ�����
		if (!GetFileData(szPath, &pReadBuffer, &dwFileSize))
			break;

		// ����Ŀ���ڴ�
		pResultData = (PUCHAR)malloc(dwFileSize+1);
		if (!pResultData)
		{
			ATLTRACE(_T("Allocate %d Bytes Memory for Decrypt Error\r\n"), dwFileSize);
			break;
		}

		// ������������
		INT nResultLen = 0;
		memset(pResultData, 0, dwFileSize+1);
		if (RC4((PUCHAR)pReadBuffer, dwFileSize, (PUCHAR)ENCRYPT_KEY, strlen(ENCRYPT_KEY), pResultData, &nResultLen) == 0)
		{
			ATLTRACE(_T("RC4 Decrypt with Size %d Error\r\n"), dwFileSize);
			break;
		}

		// ����ת��
		pXMLString = (PWCHAR)malloc(2*(dwFileSize+1));
		memset(pXMLString, 0, 2*(dwFileSize+1));
		M2W((PCCH)pResultData, nResultLen, pXMLString, dwFileSize);

		strContent = pXMLString;
		bDecodeOK = TRUE;
	}
	while (FALSE);

	if (pReadBuffer)
	{
		free(pReadBuffer);
	}
	if (pResultData)
	{
		free(pResultData);
	}
	if (pXMLString)
	{
		free(pXMLString);
	}

	return bDecodeOK;
}

BOOL CLayoutEncrypt::DecodeMem(const unsigned char* buf, size_t len, wstring& strContent)
{
	if (NULL == buf || 0 ==len )
		return FALSE;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	const unsigned char* pReadBuffer = buf;
	DWORD dwFileSize = len;
	PUCHAR pResultData = NULL;
	PWCHAR pXMLString = NULL;
	BOOL bDecodeOK = FALSE;

	do 
	{
		// ����Ŀ���ڴ�
		pResultData = (PUCHAR)malloc(dwFileSize+1);
		if (!pResultData)
		{
			ATLTRACE(_T("Allocate %d Bytes Memory for Decrypt Error\r\n"), dwFileSize);
			break;
		}

		// ������������
		INT nResultLen = 0;
		memset(pResultData, 0, dwFileSize+1);
		if (RC4(pReadBuffer, dwFileSize, (PUCHAR)ENCRYPT_KEY, strlen(ENCRYPT_KEY), pResultData, &nResultLen) == 0)
		{
			ATLTRACE(_T("RC4 Decrypt with Size %d Error\r\n"), dwFileSize);
			break;
		}

		// ����ת��
		pXMLString = (PWCHAR)malloc(2*(dwFileSize+1));
		memset(pXMLString, 0, 2*(dwFileSize+1));
		M2W((PCCH)pResultData, nResultLen, pXMLString, dwFileSize);

		strContent = pXMLString;
		bDecodeOK = TRUE;
	}
	while (FALSE);

	if (pResultData)
	{
		free(pResultData);
	}
	if (pXMLString)
	{
		free(pXMLString);
	}

	return bDecodeOK;
}

BOOL CLayoutEncrypt::GetFileData(LPCTSTR szFilePath, PVOID* ppBuffer, DWORD* pdwSize)
{
	if (!szFilePath || !ppBuffer || !pdwSize)
		return FALSE;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bGetDataOK = FALSE;

	do 
	{
		// ���ļ������������һ��
		hFile = ::CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Sleep(100);
			hFile = ::CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		}

		// Դ�ļ��޷��򿪣�����
		if (hFile == INVALID_HANDLE_VALUE)
		{
			ATLTRACE(_T("Open File %s Error: %d\r\n"), szFilePath, ::GetLastError());
			break;
		}

		// ȡ���ļ���С�������ڴ�
		*pdwSize = ::GetFileSize(hFile, NULL);
		*ppBuffer = (PUCHAR)malloc(*pdwSize);	
		if (!*ppBuffer)
		{
			ATLTRACE(_T("Allocate %d Bytes Memory Error\r\n"), *pdwSize);
			break;
		}

		// ��ȡ�ļ��е�����
		memset(*ppBuffer, 0, *pdwSize);
		if (!::ReadFile(hFile, *ppBuffer, *pdwSize, pdwSize, NULL))
		{
			ATLTRACE(_T("ReadFile %s Error: %d\r\n"), szFilePath, ::GetLastError());
			break;
		}

		bGetDataOK = TRUE;
	}
	while (FALSE);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile);
	}

	return bGetDataOK;
}
