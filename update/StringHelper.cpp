#include "stdafx.h"
#include "StringHelper.h"
#include "md5.h"
#include <time.h>
#include <process.h>
#include <TlHelp32.h>
#include <Iphlpapi.h>
#include <GdiPlus.h>
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
		strText.Format(L"%d:%d:%d", nHour, nMin, nSec);
	else
		strText.Format(L"%d:%d", nMin, nSec);
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