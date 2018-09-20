///////////////////////////////////////////////////////////////////////////////
//
//  Module: zlibcpp.cpp
//
//    Desc: See zlibcpp.h
//
// Copyright (c) 2003 Michael Carruth
//
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "zlibcpp.h"
//#include "utility.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// CZLib::CZLib
//
// 
//
CZLib::CZLib()
{
   m_zf = 0;
}


//-----------------------------------------------------------------------------
// CZLib::~CZLib
//
// Close open zip file
//
CZLib::~CZLib()
{
   if (m_zf)
      Close();
}

FILETIME CZLib::GetLastWriteFileTime(CString sFile)
{
   FILETIME          ftLocal = {0};
   HANDLE            hFind;
   WIN32_FIND_DATA   ff32;
   hFind = FindFirstFile(sFile, &ff32);
   if (INVALID_HANDLE_VALUE != hFind)
   {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
      FindClose(hFind);        
   }
   return ftLocal;
}




//-----------------------------------------------------------------------------
// CZLib::Open
//
// Create or open zip file
//
BOOL CZLib::Open(CString f_file, int f_nAppend)
{
   m_zf = zipOpen(f_file, f_nAppend);
   return (m_zf != NULL);
}


//-----------------------------------------------------------------------------
// CZLib::Close
//
// Close open zip file
//
void CZLib::Close()
{
   if (m_zf)
      zipClose(m_zf, NULL);

   m_zf = 0;
}


//-----------------------------------------------------------------------------
// CZLib::AddFile
//
// Adds a file to the zip archive
//
BOOL CZLib::AddFile(CString f_file)
{
   BOOL bReturn = FALSE;

   // Open file being added
   HANDLE hFile = NULL;
   hFile = CreateFile(f_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if (hFile)
   {
      // Get file creation date
      FILETIME       ft = GetLastWriteFileTime(f_file);
      zip_fileinfo   zi = {0};

      FileTimeToDosDateTime(
         &ft,                       // last write FILETIME
         ((LPWORD)&zi.dosDate)+1,   // dos date
         ((LPWORD)&zi.dosDate)+0);  // dos time

      // Trim path off file name
      CString sFileName = f_file.Mid(f_file.ReverseFind(_T('\\')) + 1);

      // Start a new file in Zip
      if (ZIP_OK == zipOpenNewFileInZip(m_zf, 
                                        sFileName, 
                                        &zi, 
                                        NULL, 
                                        0, 
                                        NULL, 
                                        0, 
                                        NULL, 
                                        Z_DEFLATED, 
                                        Z_BEST_COMPRESSION))
      {
         // Write file to Zip in 4 KB chunks 
         const DWORD BUFFSIZE    = 4096;
         BYTE buffer[BUFFSIZE];
         DWORD dwBytesRead = 0;
		 DWORD dwTotalRead = 0;
		 DWORD dwFileSize = ::GetFileSize(hFile,NULL);
		 if(INVALID_FILE_SIZE != dwFileSize)
		 {
			 while (ReadFile(hFile, &buffer, BUFFSIZE, &dwBytesRead, NULL))
			 {
				 dwTotalRead += dwBytesRead;
				 if(ZIP_OK != zipWriteInFileInZip(m_zf, buffer, dwBytesRead))
					 break;
				 if(dwTotalRead >= dwFileSize)
				 {
					bReturn = TRUE;
					break;
				 }
			 }
		 }
         bReturn &= (ZIP_OK == zipCloseFileInZip(m_zf));
      }
      
      bReturn &= CloseHandle(hFile);
   }
   return bReturn;
}



CString GetExePath()
{
	TCHAR lpPath[MAX_PATH];
	memset(lpPath,NULL,MAX_PATH);

	::GetModuleFileName(NULL,(LPTSTR)&lpPath, MAX_PATH);

	CString strPath = lpPath;
	int i = strPath.ReverseFind('\\');
	strPath = strPath.Left(i);
	return strPath;
}



CString MakeFullPath(const CString & strPath,const CString & strFilename)
{
	CString strRet;

	strRet = strPath;
	if(strRet.Right(1) != '\\') strRet += "\\";

	strRet += strFilename;

	return strRet;
}



CString GetReportComment(const TCHAR* pUserID)
{
	if(pUserID == NULL)
		return _T("");
	TCHAR chFileName[MAX_PATH];
	memset(chFileName,0,MAX_PATH);
	DWORD dwLen = ::GetModuleFileName(NULL,chFileName,MAX_PATH);
	CString strName = chFileName;
	CString strComment;
	int nPos = strName.ReverseFind(_T('\\'));
	if(nPos < 0)
		nPos = strName.ReverseFind(_T('/'));
	if(nPos >= 0)
		strName = strName.Mid(nPos + 1);
	DWORD dwVervsion = GetFileVersion();
	strComment.Format(_T("\"dmp_%s\" %d %s"), strName, dwVervsion, pUserID);
	return strComment;
}

DWORD GetFileVersion()
{
	DWORD dwVer = 0;
	BYTE chMainVer = 0,chSubVer = 0,chExVer = 0;
	DWORD dwBuildNO = 0;
	TCHAR chFileName[MAX_PATH];
	memset(chFileName,0,MAX_PATH);
	DWORD dwLen = ::GetModuleFileName(NULL,chFileName,MAX_PATH);
	CString strFullPath = chFileName;

	if(dwLen > 0 && dwLen < MAX_PATH)	
	{
		chFileName[dwLen] = '\0';
		dwLen = ::GetFileVersionInfoSize(strFullPath,NULL);
		if(dwLen != 0)
		{
			BYTE* pBuffer = new BYTE[dwLen];
			if(pBuffer)
			{
				if(::GetFileVersionInfo(strFullPath,NULL,dwLen,pBuffer))
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
