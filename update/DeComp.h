#pragma once
#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

typedef int (*PFN_ExtractToMem)(const char*, const char*, void**, int*,const char*);	
typedef int (*PFN_FreeBuffers)(void*);

class CDeComp
{
public:

	CDeComp(void)
		: m_pfnExtractToMem(NULL)
		, m_pfnFreeBuffers(NULL)
		, m_hCom(NULL)
		, m_strTheme(_T("default"))
	{
		m_hCom = ::LoadLibrary(L"ExtractZip.dll");
		if ( !m_hCom )
		{
			LPVOID lpMsgBuf;
			::FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				::GetLastError(),
				0, // Default language
				(LPTSTR) &lpMsgBuf,
			 0,
			 NULL 
			 );
			::MessageBox(NULL, (LPTSTR)lpMsgBuf, L"error", MB_OK);
			// Free the buffer.
			::LocalFree( lpMsgBuf );
			exit(1);
		}

		m_pfnExtractToMem =  (PFN_ExtractToMem) ::GetProcAddress(m_hCom, "ExtractToMem" );
		m_pfnFreeBuffers = (PFN_FreeBuffers) :: GetProcAddress(m_hCom, "FreeBuffers");

		TCHAR szExePath[MAX_PATH] = {0};    
		::GetModuleFileName(NULL, szExePath, MAX_PATH); 

		m_strExePath = szExePath;
		m_strExePath = m_strExePath.Left(m_strExePath.ReverseFind(_T('\\')) + 1);  

	}

	virtual ~CDeComp(void)
	{
		if(m_hCom) ::FreeLibrary(m_hCom);
	}
	
	static CDeComp* Inst()
	{
		static CDeComp decomp;
		return &decomp;
	}

	int ExtractToMem(LPCTSTR pSourceZip, LPCTSTR pSourceFile, void** buffer, int* len)
	{ 
		WTL::CString strGTR = m_strExePath  + GetGTRPath(pSourceZip);
		const char szPwd[]="BMClass@2018";
		
		int nRet = ExtractToMemGTR(strGTR, pSourceFile, buffer, len,szPwd);
		if(nRet != 1 && m_strTheme != _T("default"))
		{
			strGTR = m_strExePath + GetGTRPath(pSourceZip, true);
			nRet = ExtractToMemGTR(strGTR, pSourceFile, buffer, len,szPwd);
		}
		return nRet;
	}
	int FreeBuffer(void* buf){ return m_pfnFreeBuffers != NULL ? m_pfnFreeBuffers(buf) : 0 ;}
	inline void SetTheme(LPCTSTR theme){ m_strTheme = theme;}
private:
	HMODULE m_hCom;
	PFN_ExtractToMem m_pfnExtractToMem;
	PFN_FreeBuffers  m_pfnFreeBuffers;

	int ExtractToMemGTR(LPCTSTR pSourceZip, LPCTSTR pSourceFile, void** buffer, int* len,const char * pszPwd = NULL)
	{
		ATLASSERT(pSourceZip);
		ATLASSERT(pSourceFile);
		ATLASSERT(buffer);
		ATLASSERT(len);
#ifdef UNICODE 
		if(NULL == m_pfnExtractToMem) return 0;
		int chlen = ::WideCharToMultiByte(CP_ACP, 0, pSourceZip, wcslen(pSourceZip), NULL, 0, NULL, 0);
		char* zip = new char[chlen+1];
		ZeroMemory(zip, chlen+1);
		::WideCharToMultiByte(CP_ACP, 0, pSourceZip, wcslen(pSourceZip), zip, chlen, NULL, 0);

		chlen = ::WideCharToMultiByte(CP_ACP, 0, pSourceFile, wcslen(pSourceFile), NULL, 0, NULL, 0);
		char* name = new char[chlen+1];
		ZeroMemory(name, chlen+1);
		::WideCharToMultiByte(CP_ACP, 0, pSourceFile, wcslen(pSourceFile), name, chlen, NULL, 0);

		int nRet = m_pfnExtractToMem(zip, name, buffer, len,pszPwd);
		delete[] zip;
		delete[] name;
		return nRet;
#else
		return m_pfnExtractToMem != NULL 
			? m_pfnExtractToMem(pSourceZip, pSourceFile, buffer, len,pszPwd)
			: 0
			;
#endif
	}

	LPCTSTR GetGTRPath(LPCTSTR file, bool bDefault = false)
	{
		m_strGTRPath.Format(_T("..\\themes\\%s\\"), bDefault ? _T("default") : m_strTheme);
		if(0 == _tcscmp(file, Image))
		{
			m_strGTRPath = m_strGTRPath + _T("image.gt");
			return m_strGTRPath;
		}
		if(0 == _tcscmp(file, Faces))
			return _T("..\\data\\faces.gt");
		if(0 == _tcscmp(file, Style))
			return _T("..\\data\\style.gt");
		if(0 == _tcscmp(file, Lantern))
			return _T("..\\data\\lantern.gt");
		if(0 == _tcscmp(file, Avatar))
			return _T("..\\data\\avatar.gt");
		if(0 == _tcscmp(file, Language))
			return _T("..\\language\\chs.gt");
		return file;
	}
private:
	WTL::CString m_strTheme;
	WTL::CString m_strLanguage;
	WTL::CString m_strGTRPath;
	WTL::CString m_strExePath;
public:
	static LPCTSTR Image;
	static LPCTSTR Language;
	static LPCTSTR Style;
	static LPCTSTR Faces;
	static LPCTSTR Lantern;
	static LPCTSTR Avatar;
};

