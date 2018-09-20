#include "StringHelper.h"
#include "md5.h"
#include <time.h>
#include <process.h>
#include <TlHelp32.h>
#include <Iphlpapi.h>
#include <GdiPlus.h>
#include "languageX/languageX.h"

#include <windows.h>

#pragma comment(lib , "Iphlpapi.lib")

ATL::CString  GetTimerFromInt(int nTime)
{
	ATL::CString strText;
	int nSec = nTime % 60;
	int nMin = 0;
	int nHour = 0;
	if(nTime >= 3600)
	{
		nHour = nTime / 3600;
		nTime = nTime % 3600;
	}
	if(nTime >= 60)
	{
		nMin = nTime / 60;
	}
	if(nHour > 0)
		strText.Format(L"%d:%02d:%02d", nHour, nMin, nSec);
	else
		strText.Format(L"%02d:%02d", nMin, nSec);
	return strText;
}

time_t FileTimeToUnixTime(FILETIME ft)
{
	ULARGE_INTEGER	ul;
	ul.LowPart = ft.dwLowDateTime;
	ul.HighPart = ft.dwHighDateTime;
	return ((LONGLONG)(ul.QuadPart - 116444736000000000) / 10000000);
}
void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	if(pft == NULL)
		return;
	LONGLONG ll;
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
}
ATL::CString GetTimeString()
{
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);

	ATL::CString strResult;
	strResult.Format(_T("%04d-%02d-%02d.%02d:%02d:%02d"),timeNow.wYear, timeNow.wMonth, timeNow.wDay,
		timeNow.wHour, timeNow.wMinute, timeNow.wSecond);
	return strResult;
}

bool GetTime(time_t t, struct tm* tms)
{
	if(tms == NULL)
		return false;
	struct tm* tmp = localtime(&t);
	if(tmp)
		memcpy(tms, tmp, sizeof(tm));
	else
		memset(tms, 0, sizeof(tm));
	return true;
}

ATL::CString GetTimeStrFromSec(time_t t)
{
	struct tm tms;
	GetTime(t, &tms);
	ATL::CString strResult;
	strResult.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), tms.tm_year + 1900, tms.tm_mon + 1, tms.tm_mday,
		tms.tm_hour, tms.tm_min, tms.tm_sec);
	return strResult;
}

ATL::CString GetFileNameFromTime()
{
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);

	ATL::CString strResult;
	strResult.Format(_T("%04d%02d%02d_%02d%02d%02d"),timeNow.wYear, timeNow.wMonth, timeNow.wDay,
		timeNow.wHour, timeNow.wMinute, timeNow.wSecond);
	return strResult;
}

ATL::CString GetTimeNoDateFromSec(time_t t)
{
	struct tm tms;
	GetTime(t, &tms);
	ATL::CString strResult;
	strResult.Format(_T("%02d:%02d:%02d"), tms.tm_hour, tms.tm_min, tms.tm_sec);
	return strResult;
}

ATL::CString GetMsgTimeFromSec(time_t t)
{
	struct tm tms;
	GetTime(t, &tms);
	SYSTEMTIME timeNow;
	GetLocalTime(&timeNow);
	ATL::CString strResult;
	if(timeNow.wYear == tms.tm_year + 1900 && timeNow.wMonth == tms.tm_mon + 1 && timeNow.wDay == tms.tm_mday)
		strResult.Format(_T("%02d:%02d:%02d"), tms.tm_hour, tms.tm_min, tms.tm_sec);
	else
		strResult.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),tms.tm_year + 1900, tms.tm_mon + 1, tms.tm_mday,
		tms.tm_hour, tms.tm_min, tms.tm_sec);

	return strResult;
}

ATL::CStringA URLEncode(ATL::CStringA strSrc)
{
	CStringA ret;
	WCHAR uni_buf[16384];
	int uni_len = 16384;

	uni_len = ::MultiByteToWideChar(GetOEMCP(), 0, strSrc, strSrc.GetLength(), uni_buf, uni_len);

	for(int i=0; i<uni_len; i++)
	{
		//对每个unicode字符一一做转换
		unsigned char mb_buf[128];
		int mb_len;

		ZeroMemory(mb_buf, sizeof(mb_buf));
		mb_len = ::WideCharToMultiByte(CP_UTF8, 0, &uni_buf[i], 1, (char *)mb_buf, sizeof(mb_buf)-1, NULL, NULL);

		if(mb_len > 1)
		{
			for(int j=0; j<mb_len; j++)
			{
				CStringA sUTF8Char;
				sUTF8Char.Format("%%%2X", mb_buf[j]);
				ret += sUTF8Char;
			}
		}
		else
		{
			ret += (char *)mb_buf;
		}

	}
	return ret;

	return ret;
}

ATL::CStringA URLDecode(ATL::CStringA strUTF8)
{
	unsigned char mb_buf[16384] = {0};
	int mb_len = 0;

	for(int i=0 ;i<strUTF8.GetLength();i++)
	{
		unsigned int nDigest1 = 0,nDigest2 = 0;

		char c = strUTF8.GetAt(i);
		char t;
		if( c == '%')
		{

			i++;
			if( i >= strUTF8.GetLength())
				return "";
			else
			{
				t = strUTF8.GetAt(i);
				t = tolower(t);
				if(t>='0' && t<='9')
					nDigest1 = ( t-'0');
				else if( t>='a' && t<='z')
					nDigest1 = ( t-'a' )+10;

				nDigest1 = nDigest1<<4;
			}

			i++;
			if( i>= strUTF8.GetLength())
				return "";
			else
			{
				t = strUTF8.GetAt(i);
				t = tolower(t);
				if(t>='0' && t<='9')
					nDigest2 = ( t-'0');
				else if( t>='a' && t<='z')
					nDigest2 = ( t-'a' )+10;

				nDigest1 = nDigest2 + nDigest1;
				c = nDigest1;
				mb_buf[mb_len] = c;
				mb_len++;
			}

		}
		else
		{
			mb_buf[mb_len] = c;
			mb_len++;
		}
	}

#ifdef WIN32
	// 只有windows下才需要转成本地编码
	WCHAR uni_buf[16384];
	int uni_len = 16384;
	uni_len = ::MultiByteToWideChar(CP_UTF8, 0, (char *)mb_buf, mb_len, uni_buf, uni_len);

	ZeroMemory(mb_buf, sizeof(mb_buf));
	mb_len = WideCharToMultiByte(GetOEMCP(), 0, uni_buf, uni_len, (char *)mb_buf, sizeof(mb_buf)-1, NULL, NULL);
#endif
	return (char *)mb_buf;
}


bool String2Digest(const unsigned char* hash_str, unsigned char *pDigest, int nLen)
{
	if(hash_str == NULL || pDigest == NULL)
		return false;
	unsigned char *h = pDigest;
	char szTemp[16] = {0};

	for(int i = 0; i < nLen; i++)
	{
		strncpy(szTemp, (char*)hash_str + i * 2, 2);
		int ch;
		sscanf(szTemp, "%02X", &ch);
		h[i] = ch;
	}
	return true;
}

CStringA Digest2String(const unsigned char *pDigest, int nDigest)
{
	if(pDigest == NULL)
		return "";
	CStringA ret;
	char szTemp[8] = {0};
	for(int i=0; i<nDigest; i++)
	{
		_snprintf( szTemp, 8, "%02X", pDigest[i] );
		ret += szTemp;
	}
	return ret;
}

bool	Char2Digest(const char* pChar,int nCharLen, char* pDigest)
{
	if(pChar == NULL || nCharLen <= 0 || pDigest == NULL)
		return false;
	for (int i = 0; i < nCharLen; i++)
	{
		*pDigest = (*pChar >> 4);
		*pDigest++;
		*pDigest = *pChar & 0xF;
		pDigest++;		
		pChar++;
	}
	return true;
}
bool	Digest2Char(const char* pDigest, int nDigestLen, char* pChar)
{
	if(pDigest == NULL || nDigestLen <= 0 || pChar == NULL)
		return false;
	for (int i = 0; i < nDigestLen; i = i + 2)
	{
		*pChar = *pDigest << 4;
		pDigest++;	
		*pChar = *pChar | *pDigest;
		pDigest++;
		pChar++;
	}
	return true;
}

bool  CaleFileMd5(ATL::CString strFile, CStringA& strMd5)
{
	HANDLE hFile = ::CreateFile(strFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ATLASSERT(hFile);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwHigh;
		DWORD dwSize = ::GetFileSize(hFile, &dwHigh);
		if(dwSize == 0xFFFFFFFF || dwSize == 0)
			return false;
		const int nBuffLen = 10240;
		unsigned char buf[nBuffLen] = "\0";
		DWORD dwCaluSize = 0;
		CMD5 md5;
		md5.Init();
		DWORD dwRead = 0;
		while(dwCaluSize < dwSize)
		{
			if(::ReadFile(hFile, buf, nBuffLen, &dwRead, NULL) == FALSE)
				break;
			md5.Update(buf, dwRead);
			dwCaluSize += dwRead;

		}
		::CloseHandle(hFile);
		
		unsigned char hashmd5[16] = { 0};
		memcpy(hashmd5, md5.Finalize(), 16);
		
		strMd5 = Digest2String(hashmd5, 16);
		return true;
	}
	else
		return false;

}

ATL::CString CaleBufferMd5(ATL::CString& strBuffer)
{
	CMD5 md5;
	md5.Init();
	md5.Update((uchar*)(LPCTSTR)strBuffer, strBuffer.GetLength()*sizeof(TCHAR));
	unsigned char hashmd5[16] = { 0};
	memcpy(hashmd5, md5.Finalize(), 16);

	return ATL::CString(Digest2String(hashmd5, 16));
}

ATL::CString CaleBufferMd5(ATL::CStringA& strBuffer)
{
	CMD5 md5;
	md5.Init();
	md5.Update((uchar*)(LPCSTR)strBuffer, strBuffer.GetLength());
	unsigned char hashmd5[16] = { 0};
	memcpy(hashmd5, md5.Finalize(), 16);

	return ATL::CString(Digest2String(hashmd5, 16));
}

ATL::CString GetKeyNameById(int nKey, int nflag)
{
	wchar_t name[64] = {0};
	LPARAM lparam = 0;
	if (nKey != VK_SNAPSHOT) // Print Screen 特殊处理一下
		lparam = MapVirtualKey(nKey, 0) << 16;
	else
		lparam = 55 << 16;
	if (nKey != VK_RSHIFT)  // Right Shift 特殊处理一下
		lparam |= (nflag & 0x0F) << 24;
	GetKeyNameText(lparam, name, 64);
	return ATL::CString(name);
}

	/**
 * @brief [通过进程ID得到相应的进程]
 * 
 * [函数详细描述]
 * @n<b>函数名称</b>                    : GetProcByID
 * @n@param const DWORD      dwProcID   : [进程id]
 * @param   UNIQUE_PROCESS & procResult : [查询结果]
 * @return                                [查询是否成功]
 * @see                                   [参见函数]
 */
bool		 ProcessIsExist(int dwProcID)
{
    HANDLE            l_hSnapshot = NULL;
    PROCESSENTRY32    l_lppe;
    BOOL            l_bEntry;
    bool            l_bResult = false;    
    
    l_hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
    l_lppe.dwSize = sizeof(PROCESSENTRY32);
    l_bEntry = Process32First(l_hSnapshot,&l_lppe);

    while (TRUE == l_bEntry)
    {          
        if (l_lppe.th32ProcessID == dwProcID)
        {
            l_bResult = true;
            break;
        }
        l_bEntry = Process32Next(l_hSnapshot, &l_lppe);
    }
    
    CloseHandle(l_hSnapshot);
    return l_bResult;
}

ATL::CString GetExeName(LPCTSTR pFullPathName)
{
	ATL::CString strExeName;
	if(pFullPathName)
	{
		strExeName = pFullPathName;
		int nPos = strExeName.ReverseFind(_T('\\'));
		if(nPos >= 0)
			strExeName = strExeName.Mid(nPos + 1);
	}
	return strExeName;
}

CStringA UTF8_URL_2_MB(CStringA strSrc)
{
	CStringA ret;

	for(int i=0; i<strSrc.GetLength(); i++)
	{
		unsigned char c = strSrc.GetAt(i);
		if(c == 0x20)
			ret += "%20";
		else if(c < 0x80)
			ret += c;
		else
		{
			char buf[64] = {0};
			sprintf(buf, "%%%2X", c);
			ret += buf;
		}
	}

	return ret;

}


// http://www.utf-8.com/
// http://www.faqs.org/rfcs/rfc3629.html
CStringA MB_2_UTF8(ATL::CString strMultiByte)
{
	return SysWideToUTF8(strMultiByte);

}

ATL::CString UTF8_2_MB(CStringA strUTF8)
{
	return SysUTF8ToWide(strUTF8);
	
}

CStringA SysWideToUTF8(ATL::CString strMultiByte)
{
	int charcount = WideCharToMultiByte(CP_UTF8, 0, strMultiByte, strMultiByte.GetLength(),
		NULL, 0, NULL, NULL);
	if (charcount == 0)
		return "";

	std::string mb;
	mb.resize(charcount);
	WideCharToMultiByte(CP_UTF8, 0, strMultiByte, strMultiByte.GetLength(),
		&mb[0], charcount, NULL, NULL);
	return CStringA(mb.c_str());
}

ATL::CString SysUTF8ToWide(CStringA strUTF8)
{
	if(strUTF8.GetLength() == 0)
		return L"";
	
	int charcount = MultiByteToWideChar(CP_UTF8, 0,
		strUTF8, strUTF8.GetLength(), NULL, 0);
	if (charcount == 0)
		return L"";

	std::wstring wide;
	wide.resize(charcount);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, strUTF8.GetLength(), &wide[0], charcount);
	return wide.c_str();
}

enum eIpAddrType
{
	IPADDR_BAD = 0,               // 错误的地址，无法网络通讯
	IPADDR_LAN = 1,               // 局域网ip，或者是127.0.0.1
	IPADDR_PUBLIC = 2             // 正常外网ip
};

eIpAddrType NetCardInfo_GetIPType(DWORD ip)
{
	// 注： ip必须是网络字节序
	// 0.*							Bad
	// 169.254.*					Bad (APIPA)
	// 255.*				       	Bad (INADDR_NONE)

	// 10.*							LAN
	// 127.*						LAN (loop back)
	// 172.16.*	- 172.31.*			LAN
	// 192.168.*					LAN


	BYTE *b = (BYTE *)&ip;

	if(b[0] == 0 || b[0] == 255) return IPADDR_BAD;
	if(b[0] == 169 && b[1] == 254) return IPADDR_BAD;


	if(b[0] == 10 || b[0] == 127 )return IPADDR_LAN;
	if(b[0] == 172 && b[1] >= 16 && b[1] <=31) return IPADDR_LAN;
	if(b[0] == 192 && b[1] == 168) return IPADDR_LAN;

	return IPADDR_PUBLIC;

}

BOOL IsValidIpString(char *ip_str)
{
	if(ip_str == NULL)
		return FALSE;

	if(strlen(ip_str) == 0)
		return FALSE;

	if(strcmp(ip_str, "0.0.0.0") == 0)
		return FALSE;

	if(strcmp(ip_str, "127.0.0.1") == 0)
		return FALSE;

	return TRUE;
}



bool GetMacAddress(BYTE * szMAC )
{
	BYTE *mac = szMAC;
	bool in_nat = true;
	ZeroMemory(mac, 6);
	DWORD	dwDataSize	= 0;
	GetAdaptersInfo(0, &dwDataSize);
	if(dwDataSize == 0) return false;

	void *buf = malloc(dwDataSize);
	if (buf == NULL)	return false;

	IP_ADAPTER_INFO *IpAdaptersInfo	= (IP_ADAPTER_INFO *) buf;

	DWORD dwRetVal = GetAdaptersInfo(IpAdaptersInfo, &dwDataSize);

	if(dwRetVal != NO_ERROR)
	{
		free(buf);
		return false;
	}
	int total_interface = 0;

	for ( ; IpAdaptersInfo; IpAdaptersInfo = IpAdaptersInfo->Next)
	{
		total_interface ++;

		if ( IsValidIpString(IpAdaptersInfo->IpAddressList.IpAddress.String) == FALSE)
		{
			// ip 不合法就略过
			continue;
		}
		if ( IsValidIpString(IpAdaptersInfo->GatewayList.IpAddress.String) == FALSE)
		{
			// 没有网关的IP不合法
			continue;
		}
		if(IpAdaptersInfo->AddressLength != 6)
		{
			// 网卡物理地址长度不对的不合法
			continue;
		}
		DWORD current = inet_addr(IpAdaptersInfo->IpAddressList.IpAddress.String);
		if (current != 0)
		{
			//保证至少一个合法值

			// 忽略 vmware
			if(mac[0] == 0x00 && mac[1] == 0x50 && mac[2] == 0x56)
			{
			}
			else
			{
				memcpy(mac, IpAdaptersInfo->Address, 6);
			}
		}

		if(NetCardInfo_GetIPType(current) == IPADDR_PUBLIC)
		{
			memcpy(mac, IpAdaptersInfo->Address, 6);
			in_nat = false;
			return TRUE;
		}
	}
	free(buf);
	return TRUE;
}

bool SaveBitmapToFile(HBITMAP hBitmap  , LPCTSTR lpFileName)   //hBitmap   为刚才的屏幕位图句柄   lpFileName   为位图文件名   
{          
	if(hBitmap == NULL || lpFileName == NULL)
		return false;
	HDC		hDC; //设备描述表   
	int		iBits; //当前显示分辨率下每个像素所占字节数   
	WORD    wBitCount; //位图中每个像素所占字节数   
	DWORD   dwPaletteSize = 0; //定义调色板大小，   位图中像素字节大小   ，位图文件大小   ，   写入文件字节数   
	DWORD	dwBmBitsSize;  
	DWORD	dwDIBSize;
	DWORD	dwWritten;   
	BITMAP  Bitmap;				//位图属性结构
	BITMAPFILEHEADER    bmfHdr; //位图文件头结构
	BITMAPINFOHEADER    bi;		//位图信息头结构     
	LPBITMAPINFOHEADER	lpbi;	//指向位图信息头结构   
	HANDLE fh, hDib, hPal, hOldPal = NULL; //定义文件，分配内存句柄，调色板句柄  

	// 计算位图文件每个像素所占字节数   
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);   
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);   
	DeleteDC(hDC);  

	if(iBits <= 1)   
		wBitCount = 1;   
	else if(iBits <= 4)   
		wBitCount = 4;   
	else if(iBits <= 8)   
		wBitCount = 8;   
	else if(iBits <= 24)   
		wBitCount = 24;   
	else if(iBits <= 32)
		wBitCount = 32;

	// 计算调色板大小
	if(wBitCount <= 8)   
		dwPaletteSize = (1 << wBitCount) * sizeof(RGBQUAD);  

	// 设置位图信息头结构   
	GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&Bitmap);   
	bi.biSize = sizeof(BITMAPINFOHEADER);   
	bi.biWidth = Bitmap.bmWidth;   
	bi.biHeight = Bitmap.bmHeight;   
	bi.biPlanes = 1;   
	bi.biBitCount = wBitCount;   
	bi.biCompression = BI_RGB;   
	bi.biSizeImage = 0;   
	bi.biXPelsPerMeter = 0;   
	bi.biYPelsPerMeter = 0;   
	bi.biClrUsed = 0;   
	bi.biClrImportant = 0;  

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount+31)/32) * 4 * Bitmap.bmHeight;  

	// 为位图内容分配内存   
	hDib = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));   
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);   
	*lpbi = bi;  

	// 处理调色板         
	hPal = GetStockObject(DEFAULT_PALETTE);   
	if(hPal)   
	{   
		hDC = ::GetDC(NULL);   
		hOldPal = ::SelectPalette(hDC,   (HPALETTE)hPal,   FALSE);   
		RealizePalette(hDC);   
	}  

	// 获取该调色板下新的像素值   
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,   
		(LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize,   
		(LPBITMAPINFO)   
		lpbi, DIB_RGB_COLORS);  

	// 恢复调色板         
	if(hOldPal)   
	{   
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);   
		::ReleaseDC(NULL, hDC);   
	}  

	// 创建位图文件   
	fh = CreateFile(lpFileName, GENERIC_WRITE,     
		0, NULL, CREATE_ALWAYS,   
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);  

	if(fh == INVALID_HANDLE_VALUE)   
	{
		return FALSE;  
	}

	// 设置位图文件头   
	bmfHdr.bfType = 0x4D42;     //   "BM"   
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;       
	bmfHdr.bfSize = dwDIBSize;   
	bmfHdr.bfReserved1 = 0;   
	bmfHdr.bfReserved2 = 0;   
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;  

	// 写入位图文件头   
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);  

	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);  

	// 清除         
	GlobalUnlock(hDib);   
	GlobalFree(hDib);   
	CloseHandle(fh);   
	return true;   
}   


int GetEncoderClsid(LPCTSTR format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);
	int nReturn = -1;

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			nReturn = j;
			break;
		}    
	}

	free(pImageCodecInfo);
	return nReturn;
}


bool SaveBitmap2JPG(HBITMAP bitmap, LPCTSTR lpFileName)
{
	if(bitmap == NULL || lpFileName == NULL)
		return false;
	CLSID nClsid;
	GetEncoderClsid(TEXT("image/jpeg"), &nClsid);

	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromHBITMAP(bitmap, NULL);
	return (bmp->Save(lpFileName, &nClsid) == Gdiplus::Ok);
}

bool SaveBitmap2JPG(LPCWSTR lpBmpFile, LPCTSTR lpFileName)
{
	if(lpBmpFile == NULL || lpFileName == NULL)
		return false;
	CLSID nClsid;
	GetEncoderClsid(TEXT("image/jpeg"), &nClsid);

	Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(lpBmpFile);
	return (bmp->Save(lpFileName, &nClsid) == Gdiplus::Ok);
}

bool GetTextRectCalu(LPCTSTR pText,int nFontSize,HWND hWnd, CRect& rc)
{
	Gdiplus::Font font(LANGUAGE::GetString(L"IDS_FONT_TYPE_SIMSUN"), nFontSize);
	Gdiplus::RectF rcf_text(0.0, 0.0, 9999, 12);
	Gdiplus::Graphics g(hWnd);
	Gdiplus::RectF rcf_out(0.0,0.0,2.0,1.0);
	g.MeasureString(pText, -1, &font, rcf_text, &rcf_out);
	rc.right = rcf_out.GetRight();
	rc.bottom = rcf_out.GetBottom();
	return true;
}


bool IsNumeric(WTL::CString & strText)
{
	for (int i = 0; i < strText.GetLength(); i++)
	{
		TCHAR ch = strText.GetAt(i);
		if(ch > _T('9') || ch < _T('0'))
			return false;
	}
	return true;
}

DWORD GetFileSizeW(WCHAR* pwszFile)
{
	if(NULL == pwszFile || NULL == pwszFile[0])
	{
		return 0;
	}

	FILE* pFile = NULL;
	long  nSize;

	pFile = _wfopen(pwszFile, L"rb");
	if(pFile == NULL) 
	{		
		return 0;
	}

	fseek(pFile, 0, SEEK_END);
	nSize = ftell(pFile);
	fclose(pFile);

	return (DWORD)nSize;
}

#define ALLOC_UNIT    100

HRGN BitmapToRegion(HBITMAP hBmp,COLORREF cTransparentColor,COLORREF cTolerance)
{
	HRGN hRgn = NULL;
	HBITMAP hBmp32 = NULL;
	HDC hMemDC = NULL;
	HDC hDC = NULL;
	BITMAP bmp = { 0 };
	BITMAP bmp32 = { 0 };
	BITMAPINFOHEADER bmpHdr = { 0 };
	void * pBmpData = NULL;
	HBITMAP hOldBmp = NULL;
	HBITMAP hOldBmp2 = NULL;

	do 
	{		
		if(NULL==hBmp)
		{
			break;
		}
		hMemDC = CreateCompatibleDC(NULL);
		if(NULL==hMemDC)
		{
			break;
		}

		GetObject(hBmp, sizeof(bmp), &bmp);
		bmpHdr.biSize = sizeof(BITMAPINFOHEADER);
		bmpHdr.biWidth = bmp.bmWidth;
		bmpHdr.biHeight = bmp.bmHeight;
		bmpHdr.biPlanes = 1;
		bmpHdr.biBitCount = 32;
		bmpHdr.biCompression = BI_RGB;

		hBmp32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&bmpHdr, DIB_RGB_COLORS, &pBmpData, NULL, 0);
		if(NULL == hBmp32)
		{
			break;
		}

		hOldBmp = (HBITMAP)SelectObject(hMemDC, hBmp32);
		hDC = CreateCompatibleDC(hMemDC);
		if(NULL == hDC)
		{
			break;
		}

		// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
		GetObject(hBmp32, sizeof(BITMAP), &bmp32);
		while (bmp32.bmWidthBytes % 4)
			bmp32.bmWidthBytes++;

		// Copy the bitmap into the memory DC
		hOldBmp2 = (HBITMAP)SelectObject(hDC, hBmp);
		BitBlt(hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, hDC, 0, 0, SRCCOPY);

		DWORD maxRects = ALLOC_UNIT;
		HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
		RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
		pData->rdh.dwSize = sizeof(RGNDATAHEADER);
		pData->rdh.iType = RDH_RECTANGLES;
		pData->rdh.nCount = pData->rdh.nRgnSize = 0;
		SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

		// Keep on hand highest and lowest values for the "transparent" pixels
		BYTE lr = GetRValue(cTransparentColor);
		BYTE lg = GetGValue(cTransparentColor);
		BYTE lb = GetBValue(cTransparentColor);
		BYTE hr = min(0xff, lr + GetRValue(cTolerance));
		BYTE hg = min(0xff, lg + GetGValue(cTolerance));
		BYTE hb = min(0xff, lb + GetBValue(cTolerance));

		// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
		BYTE *p32 = (BYTE *)bmp32.bmBits + (bmp32.bmHeight - 1) * bmp32.bmWidthBytes;
		for (int y = 0; y < bmp.bmHeight; y++)
		{
			// Scan each bitmap pixel from left to right
			for (int x = 0; x < bmp.bmWidth; x++)
			{
				// Search for a continuous range of "non transparent pixels"
				int x0 = x;
				LONG *p = (LONG *)p32 + x;
				while (x < bmp.bmWidth)
				{
					BYTE b = GetRValue(*p);
					if (b >= lr && b <= hr)
					{
						b = GetGValue(*p);
						if (b >= lg && b <= hg)
						{
							b = GetBValue(*p);
							if (b >= lb && b <= hb)
								// This pixel is "transparent"
								break;
						}
					}
					p++;
					x++;
				}

				if (x > x0)
				{
					// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
					if (pData->rdh.nCount >= maxRects)
					{
						GlobalUnlock(hData);
						maxRects += ALLOC_UNIT;
						hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
						pData = (RGNDATA *)GlobalLock(hData);
					}
					RECT *pr = (RECT *)&pData->Buffer;
					SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
					if (x0 < pData->rdh.rcBound.left)
						pData->rdh.rcBound.left = x0;
					if (y < pData->rdh.rcBound.top)
						pData->rdh.rcBound.top = y;
					if (x > pData->rdh.rcBound.right)
						pData->rdh.rcBound.right = x;
					if (y+1 > pData->rdh.rcBound.bottom)
						pData->rdh.rcBound.bottom = y+1;
					pData->rdh.nCount++;

					// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
					// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
					if (pData->rdh.nCount == 2000)
					{
						HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
						if (hRgn)
						{
							CombineRgn(hRgn, hRgn, h, RGN_OR);
							DeleteObject(h);
						}
						else
							hRgn = h;
						pData->rdh.nCount = 0;
						SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
					}
				}
			}

			// Go to next row (remember, the bitmap is inverted vertically)
			p32 -= bmp32.bmWidthBytes;
		}

		// Create or extend the region with the remaining rectangles
		HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
		if (hRgn)
		{
			CombineRgn(hRgn, hRgn, h, RGN_OR);
			DeleteObject(h);
		}
		else
			hRgn = h;

		// Clean up
		GlobalFree(hData);
		SelectObject(hDC, hOldBmp2);
		SelectObject(hMemDC, hOldBmp);	
	} while(FALSE);

	if(NULL != hBmp32)
	{
		::DeleteObject(hBmp32);
		hBmp32 = NULL;
	}
	
	if(hMemDC){
		DeleteDC(hMemDC);
		hMemDC = NULL;
	}

	if(hDC){
		DeleteDC(hDC);
		hDC = NULL;
	}
	return hRgn;
}

/**************************************************************************** 
函数名称: str_to_hex 
函数功能: 字符串转换为十六进制 
输入参数: string 字符串 cbuf 十六进制 len 字符串的长度。 
输出参数: 无 
*****************************************************************************/   
int str_to_hex(char *string, unsigned char *cbuf, int len)  
{  
	BYTE high, low;  
	int idx, ii=0;  
	for (idx=0; idx<len; idx+=2)   
	{  
		high = string[idx];  
		low = string[idx+1];  

		if(high>='0' && high<='9')  
			high = high-'0';  
		else if(high>='A' && high<='F')  
			high = high - 'A' + 10;  
		else if(high>='a' && high<='f')  
			high = high - 'a' + 10;  
		else  
			return -1;  

		if(low>='0' && low<='9')  
			low = low-'0';  
		else if(low>='A' && low<='F')  
			low = low - 'A' + 10;  
		else if(low>='a' && low<='f')  
			low = low - 'a' + 10;  
		else  
			return -1;  

		cbuf[ii++] = high<<4 | low;  
	}  
	return 0;  
}  

/**************************************************************************** 
40.函数名称: hex_to_str 
41.函数功能: 十六进制转字符串 
42.输入参数: ptr 字符串 buf 十六进制 len 十六进制字符串的长度。 
43.输出参数: 无 
44.*****************************************************************************/   
void hex_to_str(char *ptr,unsigned char *buf,int len)  
{  
	for(int i = 0; i < len; i++)  
	{  
		sprintf(ptr, "%02x",buf[i]);  
		ptr += 2;  
	}  
} 

BOOL  DelFileW(LPCWSTR pwszFile)
{
	if(NULL == pwszFile || NULL == pwszFile[0])
	{
		return FALSE;
	}

	if(GetFileAttributesW(pwszFile) == INVALID_FILE_ATTRIBUTES)
	{
		return FALSE;
	};

	int		nLen = wcslen(pwszFile) + 2;	
	WCHAR*	wszFile = new WCHAR[nLen];
	memset(wszFile,NULL,sizeof(WCHAR)*nLen);
	memcpy(wszFile,pwszFile,(nLen - 2) * sizeof(WCHAR));

	SHFILEOPSTRUCTW FileOp={0};  

	FileOp.fFlags = FOF_NOCONFIRMATION; //不出现确认对话框 
	FileOp.pFrom = wszFile;  
	FileOp.pTo = NULL;					//一定要是NULL 
	FileOp.wFunc = FO_DELETE;			//删除操作

	BOOL br = ::SHFileOperationW(&FileOp) == 0 ? TRUE:FALSE;
	
	delete[] wszFile;
	return br;
}

BOOL  DelFileA(LPCSTR pszFile)
{
	if(NULL == pszFile || NULL == pszFile[0])
	{
		return FALSE;
	}

	if(GetFileAttributesA(pszFile) == INVALID_FILE_ATTRIBUTES)
	{
		return FALSE;
	};

	int		nLen = strlen(pszFile) + 2;	
	char*	szFile = new char[nLen];
	memset(szFile,NULL,sizeof(char)*nLen);
	memcpy(szFile,pszFile,(nLen - 2) * sizeof(char));

	SHFILEOPSTRUCTA FileOp={0};  

	FileOp.fFlags = FOF_NOCONFIRMATION; //不出现确认对话框 
	FileOp.pFrom = szFile;  
	FileOp.pTo = NULL;					//一定要是NULL 
	FileOp.wFunc = FO_DELETE;			//删除操作
	
	BOOL br = ::SHFileOperationA(&FileOp) == 0 ? TRUE:FALSE;
	
	delete[] szFile;
	return br;

}

ATL::CString GetDiskMaxFreeSpace(unsigned _int64 &nFreeSpace)
{
	ATL::CString strDiskName;

	int nDiskCount = 0;

	int DSLength = GetLogicalDriveStrings(0,NULL);    
	WCHAR* DStr = new WCHAR[DSLength]; 
	GetLogicalDriveStrings(DSLength,(LPTSTR)DStr);  
	
	int DType;  
	int si=0;  
	BOOL bResult = FALSE;  
	unsigned _int64 i64FreeBytesToCaller = 0;  
	unsigned _int64 i64TotalBytes = 0;  
	unsigned _int64 i64FreeBytes = 0;

	unsigned _int64 i64MaxSize = 0;

	for(int i=0;i<DSLength/4;++i)  
	{  
		WCHAR dir[4]={DStr[si],':','\\',0};  

		bResult = GetDiskFreeSpaceEx(dir,  
									 (PULARGE_INTEGER)&i64FreeBytesToCaller,  
									 (PULARGE_INTEGER)&i64TotalBytes,  
									 (PULARGE_INTEGER)&i64FreeBytes);  
		if (bResult)
		{
			PrintTraceW(L"%s---%I64d", dir, i64FreeBytesToCaller);
			if (i64MaxSize < i64FreeBytesToCaller/1024/1204)
			{
				i64MaxSize = i64FreeBytesToCaller/1024/1204;
				ATL::CString strDisk(dir);
				strDiskName.Format(L"%s", strDisk);
			}
		}
		si+=4;
	}
	nFreeSpace = i64MaxSize;
	
	return strDiskName;
}
