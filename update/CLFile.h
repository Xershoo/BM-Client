#ifndef _CLFile_H
#define _CLFile_H

#include <windows.h>


class CLFile
{
public:
	CLFile();
	virtual ~CLFile();

	bool OpenW(const wchar_t *pFileName, DWORD dwCreationDistribution = OPEN_ALWAYS, DWORD dwShareMode = FILE_SHARE_READ, DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE, bool bForceOpen = false);
	bool OpenA(const char *pFileName, DWORD dwCreationDistribution = OPEN_ALWAYS, DWORD dwShareMode = FILE_SHARE_READ, DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE, bool bForceOpen = false);
	bool Close();

	static HANDLE CreateFile(const TCHAR * pFileName, LONGLONG fileSize, bool bUnicodeFile);
	static HANDLE CreateFile(const TCHAR * pFileName, bool bUnicodeFile);
	bool   CloseFileHandle(HANDLE hFile);

	bool Read(unsigned int nPointer, unsigned int nLength,  void *pReadBuf, unsigned long *pBytesRead);
	bool Write(unsigned int nPointer, unsigned int nLength, const void *pWriteBuf, unsigned long *pBytesWrite);

	bool Read(unsigned int nLength,  void *pReadBuf, unsigned long *pBytesRead);
	bool Write(unsigned int nLength, const void *pWriteBuf, unsigned long *pBytesWrite);
	
	
	static bool DeleteFile(const TCHAR* pFilePath);

	static bool DeleteDirectory(const TCHAR* pDeletePath);

	bool SetFilePointer(unsigned int nPointer);
	DWORD GetFileSize();

	LONGLONG GetLargeFileSize();

	static bool IsDirectory(const TCHAR* strFileOrDirectoryFullPath);			//true denote the file is a Directory, false denote not;
	static bool FileIsExist(const TCHAR* fileName);								//true denote the file exist, or false denote not exist;
	static bool GetDirectoryOfFile(const TCHAR* strFileFullPath, TCHAR* strDirectory);

	static bool CreateDirectoryByPath(const TCHAR* strDiectoryFullPath);
	
	bool GetFileTimeInfo(LPFILETIME lpCreationTime,   LPFILETIME lpLastAccessTime,  LPFILETIME lpLastWriteTime );
	bool SetFileTimeInfo(LPFILETIME lpCreationTime,   LPFILETIME lpLastAccessTime,  LPFILETIME lpLastWriteTime );
	
	bool SetEndOfTheFile();
	void Flush();

	bool IsUnicodeFile();

private:
	CLFile(const CLFile&);
	CLFile& operator=(const CLFile&);
	
private:
	HANDLE m_hFile;

};
#endif