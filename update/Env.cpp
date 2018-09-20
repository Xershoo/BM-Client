#include "StdAfx.h"
#include "Env.h"
#include "rc4.h"
#include "Markup.h"
#include "resource.h"
#include "DeComp.h"
#include <WinVer.h>
#include "CLFile.h"
#include "StringHelper.h"

#pragma  comment(lib, "version.lib")
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

WTL::CString CEnv::strExePath = _T("");
WTL::CString CEnv::m_strAppFileName = _T("");
WTL::CString CEnv::strTempPath = _T("");
WTL::CString CEnv::strGtPath = _T("");
WTL::CString CEnv::strGtVersionPath = _T("");
int CEnv::nLanguageId_ = 0;
CLog CEnv::m_oLog;
ChannelGroups CEnv::m_ChannelGroups;
std::vector<WTL::CString> CEnv::m_vecAuthType;

BYTE CEnv::chMainVer = 0;
BYTE CEnv::chSubVer = 0;
BYTE CEnv::chExVer = 0;
DWORD CEnv::chBuildNO = 0;
BYTE CEnv::szMac[] = "\0";

WTL::CString CEnv::GetUSerConfigPath(UINT64 u64UserID)
{
    
	WTL::CString strTmp;
	strTmp.Format(_T("%sUsers\\%I64u\\"),strGtPath, u64UserID);


    int nRet = SHCreateDirectoryEx(NULL, strTmp, NULL);
    if (nRet != ERROR_SUCCESS &&
        nRet != ERROR_FILE_EXISTS && 
        nRet != ERROR_ALREADY_EXISTS)
    {
        return _T("");
    }

    return strTmp;
}

WTL::CString CEnv::GetConfigPath()
{
	WTL::CString strPath;
	strPath.Format(_T("%s\\..\\config"), strExePath);
	return strPath;
}

WTL::CString CEnv::GetSoftConfigPath()
{
    WTL::CString strPath = _T("");
    TCHAR tczPath[MAX_PATH] = {0};

    if(strGtPath.IsEmpty())
    {
        ::GetModuleFileName(NULL,tczPath,MAX_PATH);  
        WTL::CString tmpdir = tczPath;  
		strPath = tmpdir.Left(tmpdir.ReverseFind(_T('\\'))); 
		tmpdir = strPath;  
		strPath = tmpdir.Left(tmpdir.ReverseFind(_T('\\')) + 1); 
    }
    else
    {
        strPath = strGtPath;
    }

    int nRet = SHCreateDirectoryEx(NULL, strPath, NULL);
    if (nRet != ERROR_SUCCESS &&
        nRet != ERROR_FILE_EXISTS && 
        nRet != ERROR_ALREADY_EXISTS)
    {
        return _T("");
    }
    return strPath;
}

WTL::CString CEnv::GetLanternDataPath()
{
    return strExePath + _T("lantern\\");
}

WTL::CString CEnv::GetResString(UINT nID)
{
    WTL::CString strTmp;
    strTmp.LoadString(nID);
    return strTmp;
}

WTL::CString CEnv::GetGTVersion()
{
    // 版本号是静态变量，只取一次即可
    static WTL::CString strGTVersion;

    // 如果已经取得，无需再打开文件读
    if (strGTVersion.GetLength())
        return strGTVersion;

	TCHAR szFileName[MAX_PATH] = { 0};
	::GetModuleFileName(NULL, szFileName, MAX_PATH - 1);
	DWORD dwLen = ::GetFileVersionInfoSize(szFileName, NULL);
	if(dwLen > 0)
	{
		BYTE* pBuffer = new BYTE[dwLen];
		if(::GetFileVersionInfo(szFileName, NULL, dwLen, pBuffer))
		{
			VS_FIXEDFILEINFO* lpFileInfo = NULL;
			UINT nLen = 0;
			if(::VerQueryValue(pBuffer, TEXT("\\"), (void**)&lpFileInfo, &nLen))
			{
				DWORD dwNumber = 65536;
				chMainVer = lpFileInfo->dwFileVersionMS / dwNumber;
				chSubVer = lpFileInfo->dwFileVersionMS % dwNumber;
				chExVer = lpFileInfo->dwFileVersionLS / dwNumber;
				chBuildNO = lpFileInfo->dwFileVersionLS % dwNumber;
				strGTVersion.Format(_T("%d.%d.%d.%d"), chMainVer,chSubVer, chExVer,chBuildNO);
			}
		}
		delete []pBuffer;
	}

    return strGTVersion;
}

WTL::CString CEnv::GetUserPicCachePath()
{
    WTL::CString strPicCachePath = GetUSerConfigPath() + _T("Images\\");

    // 判断路径是否存在
    if (!::PathFileExists(strPicCachePath))
    {
        // 创建目录
        INT nRet = SHCreateDirectoryEx(NULL, strPicCachePath, NULL);
        if(nRet != ERROR_SUCCESS && nRet != ERROR_FILE_EXISTS && nRet != ERROR_ALREADY_EXISTS)
        {
            return _T("");
        }
    }

    return strPicCachePath;
}

ATL::CString CEnv::GetAllPicCachePath()
{
	ATL::CString strTmp;
	strTmp.Format(_T("%sUsers\\Images\\"),strGtPath);


	SHCreateDirectoryEx(NULL, strTmp, NULL);
	return strTmp;
}

void CEnv::InitExePath()
{
    TCHAR szExePath[MAX_PATH] = {0};    
    ::GetModuleFileName(NULL, szExePath, MAX_PATH); 

    strExePath = szExePath;
	m_strAppFileName = szExePath;
    strExePath = strExePath.Left(strExePath.ReverseFind(_T('\\')) + 1);  
	GetMacAddress(szMac);

	TCHAR tczPath[MAX_PATH] = {0};
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, tczPath);
	strGtPath.Format(L"%s\\BMClass\\", tczPath);

	WTL::CString strPath = strExePath;
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath = strPath.Left(strPath.ReverseFind(_T('\\')));
	strPath = strPath.Mid(strPath.ReverseFind(_T('\\')) + 1);
	strGtVersionPath = strPath;

	CLFile cFile;
	CStringA strLanguagePath(strGtPath);
	strLanguagePath.AppendFormat("language");
	if(cFile.OpenA(strLanguagePath, OPEN_EXISTING))
	{
		char buffer[1024];
		ZeroMemory(buffer, sizeof(buffer));
		DWORD nRead = 0;
		cFile.Read(1024, buffer, &nRead);
		if(nRead)
			nLanguageId_ = atoi(buffer);
	}
}

CEnv::CEnv(void)
{

}

CEnv::~CEnv(void)
{

}

BOOL CEnv::LoadOtherXmlData()
{
    return TRUE;
}

WTL::CString CEnv::GetTempPath()
{
	if(strTempPath.GetLength() == 0)
	{
		wchar_t temp_path[MAX_PATH + 1] =  {0};
		DWORD path_len = ::GetTempPath(MAX_PATH, temp_path);
		strTempPath = temp_path;
	}
	return strTempPath;
}

WTL::CString CEnv::GetKeyName(int nKeyValue)
{
    WTL::CString str = _T("");
    if(('0' <= nKeyValue && nKeyValue <= '9')  
        || ('A' <= nKeyValue && nKeyValue <= 'Z'))
    {
        wchar_t wcz[2];
        wcz[0] = nKeyValue;
        wcz[1] = 0;
        str = wcz;
        return str;
    }

    switch(nKeyValue)
    {
    case VK_RETURN:
        return _T("Enter");
    case VK_BACK:
        return _T("Backspace");
    case VK_TAB:
        return _T("Tab");
    case VK_PAUSE:
        return _T("Pause");
    case VK_CAPITAL:
        return _T("Caps Lock");
    case VK_ESCAPE:
        return _T("Esc");
    case VK_SPACE:
        return _T("Space");
    case VK_CONTROL:
        return _T("Ctrl");
    case VK_LCONTROL:
        return _T("Left Ctrl");
    case VK_RCONTROL:
        return _T("Right Ctrl");
    case VK_SHIFT:
        return _T("Shift");
    case VK_LSHIFT:
        return _T("Left Shift");
    case VK_RSHIFT:
        return _T("Right Shift");
    case VK_MENU:
        return _T("Alt");
    case VK_LMENU:
        return _T("Left Alt");
    case VK_RMENU:
        return _T("Right Alt");
    case VK_PRIOR:
        return _T("Page Up");
    case VK_NEXT:
        return _T("Page Down");
    case VK_END:
        return _T("End");
    case VK_HOME:
        return _T("Home");
    case VK_LEFT:
        return _T("←");
    case VK_UP:
        return _T("↑");
    case VK_RIGHT:
        return _T("→");
    case VK_DOWN:
        return _T("↓");
    case VK_INSERT:
        return _T("Insert");
    case VK_DELETE:
        return _T("Delete");
    case VK_LWIN:
        return _T("Left Win");
    case VK_RWIN:
        return _T("Right Win");
    case VK_APPS:
        return _T("Apps");
    case VK_F1:
        return _T("F1");
    case VK_F2:
        return _T("F2");
    case VK_F3:
        return _T("F3");
    case VK_F4:
        return _T("F4");
    case VK_F5:
        return _T("F5");
    case VK_F6:
        return _T("F6");
    case VK_F7:
        return _T("F7");
    case VK_F8:
        return _T("F8");
    case VK_F9:
        return _T("F9");
    case VK_F10:
        return _T("F10");
    case VK_F11:
        return _T("F11");
    case VK_F12:
        return _T("F12");
    case VK_F13:
        return _T("F13");
    case VK_F14:
        return _T("F14");
    case VK_F15:
        return _T("F15");
    case VK_F16:
        return _T("F16");
    case VK_F17:
        return _T("F17");
    case VK_F18:
        return _T("F18");
    case VK_F19:
        return _T("F19");
    case VK_F20:
        return _T("F20");
    case VK_F21:
        return _T("F21");
    case VK_F22:
        return _T("F22");
    case VK_F23:
        return _T("F23");
    case VK_F24:
        return _T("F24");
    case VK_NUMPAD0:
        return _T("Num 0");
    case VK_NUMPAD1:
        return _T("Num 1");
    case VK_NUMPAD2:
        return _T("Num 2");
    case VK_NUMPAD3:
        return _T("Num 3");
    case VK_NUMPAD4:
        return _T("Num 4");
    case VK_NUMPAD5:
        return _T("Num 5");
    case VK_NUMPAD6:
        return _T("Num 6");
    case VK_NUMPAD7:
        return _T("Num 7");
    case VK_NUMPAD8:
        return _T("Num 8");
    case VK_NUMPAD9:
        return _T("Num 9");
    case VK_MULTIPLY:
        return _T("Num *");
    case VK_ADD:
        return _T("Num +");
    case VK_SEPARATOR:
        return _T("");
    case VK_SUBTRACT:
        return _T("Num -");
    case VK_DIVIDE:
        return _T("Num /");
    case VK_DECIMAL:
        return _T("Num .");
    case VK_NUMLOCK:
        return _T("Num Lock");
    case VK_SCROLL:
        return _T("Scroll Lock");
    case VK_OEM_3:
        return _T("`");
    case VK_OEM_MINUS:
        return _T("-");
    case VK_OEM_PLUS:
        return _T("=");
    case VK_OEM_4:
        return _T("[");
    case VK_OEM_6:
        return _T("]");
    case VK_OEM_5:
        return _T("\\");
    case VK_OEM_1:
        return _T(";");
    case VK_OEM_7:
        return _T("\"");
    case VK_OEM_COMMA:
        return _T("<");
    case VK_OEM_PERIOD:
        return _T(">");
    case VK_OEM_2:
        return _T("?");
    case VK_LBUTTON:
       // return GetResString(IDS_TXT_CONFIG_MOUSE_L);
    case VK_MBUTTON:
      //  return GetResString(IDS_TXT_CONFIG_MOUSE_M);
    case VK_RBUTTON:
      //  return GetResString(IDS_TXT_CONFIG_MOUSE_R);

    default:
        return _T("");
    }
}

BOOL CEnv::InitChannelGroupCombo(CComboBox * pComboBox)
{
    if ( NULL == pComboBox )
    {
        return FALSE;
    }

    pComboBox->ResetContent(); 
    int iIndex = 0;
    pComboBox->InsertString( iIndex++, m_ChannelGroups.m_strName );
    GroupIter iterBegin = m_ChannelGroups.m_vecGroups.begin(),
        iterEnd = m_ChannelGroups.m_vecGroups.end();
    for ( ++iterBegin; iterBegin != iterEnd; ++iIndex, ++iterBegin )
    {
        pComboBox->InsertString( iIndex, iterBegin->m_strName );
    }

    pComboBox->SetCurSel( 0 );

    return TRUE;
}

BOOL CEnv::ShowGroupItemCombo(CComboBox* pComboBox, int nGID)
{
    if ( NULL == pComboBox )
    {
        return FALSE;
    }

    pComboBox->ResetContent();

    int iIndex = 0;
    if ( 0 == nGID )
    {
        GroupIter iterGroupBegin = m_ChannelGroups.m_vecGroups.begin(),
            iterGroupEnd = m_ChannelGroups.m_vecGroups.end();
        for ( ; iterGroupBegin != iterGroupEnd; ++iIndex, ++iterGroupBegin )
        {
            pComboBox->InsertString( iIndex, iterGroupBegin->m_strName );
        }
    }
    if ( 3 == nGID )
    {
        
    }
    else
    {
        GroupItemIter iterBegin = m_ChannelGroups.m_vecGroups[ nGID ].m_vecItems.begin(),
            iterEnd = m_ChannelGroups.m_vecGroups[ nGID ].m_vecItems.end();
        for ( ; iterBegin != iterEnd; ++iterBegin, ++iIndex )
        {
            pComboBox->InsertString( iIndex, iterBegin->m_strName );
        }
    }

    pComboBox->SetCurSel( 0 );

    return TRUE;
}

BOOL CEnv::InitAuthCombo(CComboBox* pComboBox)
{
    if ( NULL == pComboBox )
    {
        return FALSE;
    }

    pComboBox->ResetContent();

    std::vector<WTL::CString>::iterator iterBegin = m_vecAuthType.begin(),
        iterEnd = m_vecAuthType.end();
    for ( int iIndex = 0; iterBegin != iterEnd; ++iterBegin, ++iIndex )
    {
        pComboBox->InsertString( iIndex, *iterBegin );
    }

    pComboBox->SetCurSel( 0 );

    return TRUE;
}

void CEnv::ReplaceEscapeChar(ATL::CString& strText)
{
	strText.Replace(_T("<"), _T("&lt;"));
	strText.Replace(_T(">"), _T("&gt;"));
	strText.Replace(_T("&"), _T("&amp;"));
	strText.Replace(_T("\""), _T("&quot;"));
}

void CEnv::ReplaceEscapeChar(WTL::CString& strText)
{
	strText.Replace(_T("<"), _T("&lt;"));
	strText.Replace(_T(">"), _T("&gt;"));
	strText.Replace(_T("&"), _T("&amp;"));
	strText.Replace(_T("\""), _T("&quot;"));
}

BOOL CEnv::IsOSHigherThanXP()
{
	static BOOL bIsXPHigher = FALSE;
	static BOOL bIsInited = FALSE;

	// 如果已经初始化则不再判断
	if (bIsInited)
		return bIsXPHigher;

	// 取得当前系统版本
	OSVERSIONINFO VersionInfo = {0};
	VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&VersionInfo);

	// 判断并返回
	bIsXPHigher = VersionInfo.dwMajorVersion > 5;
	bIsInited = TRUE;

	return bIsXPHigher;
}

BOOL CEnv::IsAllNumber(WTL::CString& strValue)
{
	BOOL bIsAllNumber = TRUE;

	INT nLength = strValue.GetLength();
	if (nLength == 0)
		return FALSE;

	for (INT n = 0; n < nLength; n++)
	{
		if (strValue[n] > _T('9') || strValue[n] < _T('0'))
		{
			bIsAllNumber = FALSE;
			break;
		}
	}

	return bIsAllNumber;
}

WTL::CString CEnv::GetAvatarPath()
{
	return strExePath + _T("avatar\\");
}

#ifdef _TEST_BETA
WTL::CString CEnv::GetBetaVersion()
{
	static WTL::CString strBetaVersion;
	static BOOL bInited = FALSE;

	// 只读取一次文件
	if (bInited)
		return strBetaVersion;

	// 打开文件
	HANDLE hBetaFile = ::CreateFile(CEnv::strExePath + _T("\\data\\beta"), GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	// 出错时重试一次
	if (hBetaFile == INVALID_HANDLE_VALUE)
	{
		Sleep(100);
		hBetaFile = ::CreateFile(CEnv::strExePath + _T("\\data\\beta"), GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	}

	// 打开成功则读取数据
	if (hBetaFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesRead = 0;
		CHAR szBuffer[256] = {0};
		::ReadFile(hBetaFile, szBuffer, 250, &dwBytesRead, NULL);
		::CloseHandle(hBetaFile);

		// 设置版本号
		strBetaVersion = szBuffer;
	}

	// 设置为已初始化
	bInited = TRUE;
	return strBetaVersion;
}

WTL::CString CEnv::GetMinVersion()
{
	// 默认小版本号为空
	static WTL::CString strMinVersion;
	static BOOL bInited = FALSE;

	// 只初始化一次
	if (bInited)
		return strMinVersion;

	// 从字符串分析小版本号
	WTL::CString strBetaVersion = GetBetaVersion();
	if (strBetaVersion.GetLength())
	{
		INT nPos = strBetaVersion.ReverseFind(_T('.'));
		if (nPos >= 0)
		{
			strMinVersion = strBetaVersion.Mid(nPos+1);

			// 不是数值或者为0则赋默认值
			if (!IsAllNumber(strMinVersion) || strMinVersion.Compare(_T("0")) == 0)
				strMinVersion = _T("");
		}
	}

	// 设置为已经初始化
	bInited = TRUE;
	return strMinVersion;
}
#endif
