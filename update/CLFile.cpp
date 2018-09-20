#include "CLFile.h"
#include <tchar.h>
#include <atlstr.h>
#include <ShlObj.h>
//#include "../global.h"

CLFile::CLFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
}


CLFile::~CLFile()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hFile);
}

bool CLFile::OpenW(const wchar_t *pFileName, 
				   DWORD dwCreationDistribution /* = OPEN_ALWAYS */, 
				   DWORD dwShareMode /* = FILE_SHARE_READ */, 
				   DWORD dwDesiredAccess /* = GENERIC_READ | GENERIC_WRITE */, 
				   bool bForceOpen /* = false */)
{
	if(pFileName == NULL)
		return false;
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(bForceOpen)
			::CloseHandle(m_hFile);
		else
			return false;
	}

	m_hFile = ::CreateFileW(pFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDistribution, 0, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD nSize = GetFileSize();
	unsigned char unicode_header[2] = {0xFF, 0xFE};
	if(nSize > 0)
	{
		unsigned char buffer[2] ;
		ZeroMemory(buffer, sizeof(buffer));
		::ReadFile(m_hFile, buffer, 2, &nSize, NULL);
		if(memcmp(buffer, unicode_header, 2) != 0)
		{
			SetFilePointer(0);
		}
		
	}
	else
	{
		::WriteFile(m_hFile,unicode_header , 2, &nSize, NULL);
	}
	
	return true;
}

bool CLFile::OpenA(const char *pFileName, 
				   DWORD dwCreationDistribution /* = OPEN_ALWAYS */, 
				   DWORD dwShareMode /* = FILE_SHARE_READ */, 
				   DWORD dwDesiredAccess /* = GENERIC_READ | GENERIC_WRITE */, 
				   bool bForceOpen /* = false */)
{
	if(pFileName == NULL)
		return false;
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		if(bForceOpen)
			::CloseHandle(m_hFile);
		else
			return false;
	}

	m_hFile = ::CreateFileA(pFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDistribution, 0, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	return true;
}


HANDLE CLFile::CreateFile(const TCHAR * pFileName, bool bUnicodeFile)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if(pFileName == NULL)
		return hFile;

	TCHAR pFilePath[MAX_PATH] = {0};
	GetDirectoryOfFile(pFileName, pFilePath);
	if(!IsDirectory(pFilePath))
		CreateDirectoryByPath(pFilePath);

	
	if(!FileIsExist(pFileName))
	{
		hFile = ::CreateFile(pFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile && bUnicodeFile)
		{			
			unsigned char unicode_header[2] = {0xFF, 0xFE};
			DWORD nSize = 0;
			::WriteFile(hFile,unicode_header , 2, &nSize, NULL);
		}
	}
	return hFile;
}

HANDLE CLFile::CreateFile(const TCHAR * pFileName, LONGLONG fileSize, bool bUnicodeFile)
{
	DWORD dwWrite;

	HANDLE hFile = INVALID_HANDLE_VALUE;
	if(pFileName == NULL || (hFile = CreateFile(pFileName, bUnicodeFile)) == INVALID_HANDLE_VALUE)
		return INVALID_HANDLE_VALUE;

	if(bUnicodeFile)
	{			
		fileSize += 2;
	}

	DWORD thePosition = ::SetFilePointer(hFile,fileSize - 1, 0, FILE_BEGIN);
	if(::WriteFile(hFile,"\0",1,&dwWrite,NULL) == FALSE)
	{
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return hFile;
}

bool CLFile::CloseFileHandle(HANDLE hFile)
{
	if(hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return true;
}

bool CLFile::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
	return true;
}

bool CLFile::Read(unsigned int nPointer, unsigned int nLength,  void *pReadBuf, unsigned long *pBytesRead)
{
	if(m_hFile == INVALID_HANDLE_VALUE || pBytesRead == NULL)
	{
		return false;
	}

	SetFilePointer(nPointer);

	return Read(nLength, pReadBuf, pBytesRead);
}

bool CLFile::Read(unsigned int nLength, void *pReadBuf, unsigned long *pBytesRead)
{
	if(m_hFile == INVALID_HANDLE_VALUE || pBytesRead == NULL)
	{
		return false;
	}
	return ::ReadFile(m_hFile, pReadBuf, nLength, pBytesRead, NULL) == TRUE;
}

bool CLFile::Write(unsigned int nPointer, unsigned int nLength, const void *pWriteBuf, unsigned long *pBytesWrite)
{
	if(m_hFile == INVALID_HANDLE_VALUE || pBytesWrite == NULL)
	{
		return false;
	}

	SetFilePointer(nPointer);

	return Write(nLength, pWriteBuf, pBytesWrite);
}



bool CLFile::Write(unsigned int nLength, const void *pWriteBuf, unsigned long *pBytesWrite)
{
	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		*pBytesWrite = 0;
		return false;
	}

	return ::WriteFile(m_hFile, pWriteBuf, nLength, pBytesWrite, NULL) == TRUE;
}


bool CLFile::SetFilePointer(unsigned int nPointer)
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	LONG nPointerHigh = 0;
	::SetFilePointer(m_hFile, nPointer, &nPointerHigh, FILE_BEGIN);

	return true;
}

DWORD CLFile::GetFileSize()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwHigh;
	DWORD dwSize = ::GetFileSize(m_hFile, &dwHigh);
	if(dwSize == 0xFFFFFFFF || dwSize == 0)
		return 0;
	else
	{
		/*if(m_bUnicodeFile)
			dwSize -= 2;*/
		return dwSize;
	}
}

LONGLONG CLFile::GetLargeFileSize()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return 0;
	
	DWORD dwHigh;
	DWORD dwLow = ::GetFileSize(m_hFile, &dwHigh);
	LONGLONG lCLFileSize = dwHigh;
	lCLFileSize = lCLFileSize << 32;
	lCLFileSize += dwLow;
	return lCLFileSize;
}

bool CLFile::DeleteFile(const TCHAR* pFilePath)
{	
	if(pFilePath == NULL)
		return false;
	return ::DeleteFile(pFilePath) == TRUE;
}

bool CLFile::IsDirectory(const TCHAR* strFileOrDirectoryFullPath)
{
	if(strFileOrDirectoryFullPath == NULL)
		return false;

	DWORD theFileAttribute = GetFileAttributes(strFileOrDirectoryFullPath);
	if(( theFileAttribute != -1 ) && (theFileAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0)
		return true;
	else 
		return false;
}

bool CLFile::FileIsExist(const TCHAR* fileName)
{
	if(fileName == NULL)
		return false;

	DWORD theFileAttribute = GetFileAttributes(fileName);
	if( theFileAttribute != -1 )
		return true;
	else 
		return false;
}

bool CLFile::CreateDirectoryByPath(const TCHAR* strDiectoryFullPath)
{
	int len = _tcslen(strDiectoryFullPath);
	if(strDiectoryFullPath == NULL || len == 0)
		return false;
	int nRet = SHCreateDirectoryEx(NULL, strDiectoryFullPath, NULL);
	if (nRet != ERROR_SUCCESS &&
		nRet != ERROR_FILE_EXISTS && 
		nRet != ERROR_ALREADY_EXISTS)
	{
		return false;
	}
	return true;
}

bool CLFile::GetDirectoryOfFile(const TCHAR* strFileFullPath, TCHAR* strDirectory)
{
	if(strFileFullPath == NULL || strDirectory == NULL)
		return false;
	TCHAR *p =(TCHAR*) strFileFullPath;
	int len = _tcslen(strFileFullPath);
	p = p + len;
	int i = 0;
	for(i = 0; i < len; i++)
	{
		if(*p != '\\' && *p != '/')
			p--;
		else
			break;
	}
//	while(p && *p != '\\')
//		p--;
	if(i >= len)
		return false;
	_tcsncpy(strDirectory, strFileFullPath, p-strFileFullPath+1);
	return true;
}

bool CLFile::DeleteDirectory(const TCHAR* pDeletePath)
{
	if(pDeletePath == NULL)
		return false;

	ATL::CString   s1,s2;     
	HANDLE   h   =   NULL;     
	WIN32_FIND_DATA   data;     

	if(pDeletePath[_tcslen(pDeletePath)]=='\\')
		s1=pDeletePath;
	else
	{
		s1=pDeletePath;
		s1 += _T("\\");     
	}
	s2   =   s1   + _T("*.*");     
	if((h=::FindFirstFile(s2,&data))!=0)     
		do     
		{     
			if(   (ATL::CString(data.cFileName) == _T("."))   ||   (ATL::CString(data.cFileName) == _T("..")  ) )     
				continue;     
			if   ((data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) )   //删除文件夹     
				DeleteDirectory((s1+ATL::CString(data.cFileName)));     
			s2   =   s1 + ATL::CString(data.cFileName);   //提取文件名     
			::DeleteFile(s2);   //删除文件     
		}while(::FindNextFile(h,&data));     
		
		::FindClose(h);         
		::RemoveDirectory(s1);   //删除文件夹     
		return   true;     

}

bool CLFile::GetFileTimeInfo(LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime )
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;
	return GetFileTime(m_hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime) == TRUE;
}

bool CLFile::SetFileTimeInfo(LPFILETIME lpCreationTime,   LPFILETIME lpLastAccessTime,  LPFILETIME lpLastWriteTime )
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;
	return ::SetFileTime(m_hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime) == TRUE;
}

bool CLFile::SetEndOfTheFile()
{
	if(m_hFile == INVALID_HANDLE_VALUE)
		return false;

	SetEndOfFile(m_hFile);
	return true;
}

void CLFile::Flush()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
		::FlushFileBuffers(m_hFile);
}