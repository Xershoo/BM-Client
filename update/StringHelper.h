#ifndef _STRING_HELPER_H_
#define _STRING_HELPER_H_
#pragma once

#include <vector>
#include <list>

time_t FileTimeToUnixTime(FILETIME ft);
void UnixTimeToFileTime(time_t t, LPFILETIME pft);
ATL::CString GetTimeString();
ATL::CString GetFileNameFromTime();
ATL::CString GetTimeStrFromSec(time_t t);
//聊天消息时间，如果是当天的消息不显示日期只显示小时分秒，如果不是当天的消息，显示日期+小时分钞
ATL::CString GetMsgTimeFromSec(time_t t);

ATL::CStringA URLEncode(ATL::CStringA strSrc);
ATL::CStringA URLDecode(ATL::CStringA strSrc);
CStringA Digest2String(const unsigned char *pDigest, int nDigest);
bool    String2Digest(const unsigned char* hash_str, unsigned char *pDigest, int nLen = 16);
bool	Char2Digest(const char* pChar,int nCharLen, char* pDigest);
bool	Digest2Char(const char* pDigest, int nDigestLen, char* pChar);
bool  CaleFileMd5(ATL::CString strFile, CStringA& strMd5);
ATL::CString CaleBufferMd5(ATL::CString& strBuffer);
ATL::CString GetKeyNameById(int nKey, int nflag);
bool		 ProcessIsExist(int nPic);
ATL::CString GetExeName(LPCTSTR pFullPathName);
// UTF-8 Url编码
CStringA MB_2_UTF8(ATL::CString strMultiByte);
ATL::CString UTF8_2_MB(CStringA strUTF8);

CStringA SysWideToUTF8(ATL::CString strMultiByte);
ATL::CString SysUTF8ToWide(CStringA strUTF8);
bool GetMacAddress(BYTE * szMAC );

bool SaveBitmapToFile(HBITMAP bitmap, LPCTSTR lpFileName);

//
bool SaveBitmap2JPG(HBITMAP bitmap, LPCTSTR lpFileName);
bool GetTextRectCalu(LPCTSTR pText,int nFontSize,HWND hWnd, CRect& rc);
bool IsNumeric(WTL::CString & strText);

ATL::CString  GetTimerFromInt(int nTime);

#endif