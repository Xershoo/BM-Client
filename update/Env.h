#pragma once

#include "Log.h"
#include <atlcoll.h>
#include <vector>

class GroupItem
{
public:
    GroupItem() : m_iGID( 0 ), m_strName( L"" ){}
    int m_iGID;
    WTL::CString m_strName;
};

class ChannelGroup : public GroupItem
{
public:
    std::vector<GroupItem> m_vecItems;
};

class ChannelGroups : public GroupItem
{
public:
    std::vector<ChannelGroup> m_vecGroups;
};

typedef std::vector<GroupItem>::iterator GroupItemIter;
typedef std::vector<ChannelGroup>::iterator GroupIter;

class CEnv
{
public:
	CEnv(void);
	virtual ~CEnv(void);

public:
	static WTL::CString strExePath;
	static WTL::CString m_strAppFileName; //
	static CLog         m_oLog;
	static WTL::CString strTempPath;
	static BYTE chMainVer;
	static BYTE chSubVer;
	static BYTE chExVer;
	static DWORD chBuildNO;
	static BYTE	szMac[6];
	static WTL::CString	strGtPath; //GT的安装目录
	static WTL::CString strGtVersionPath; //版本号，不是EXE的版本号，而是文件夹的版本号
	static int	nLanguageId_;
private:
    static ChannelGroups                m_ChannelGroups;
    static std::vector<WTL::CString>    m_vecAuthType;
	
public:
	static WTL::CString GetLanternDataPath();
	//用户登录后以其USERID获取配置文件夹，该文件夹可能有其配置相关的文件，聊天记录等内容。为0时表情默认的当前用户
	static WTL::CString GetUSerConfigPath(UINT64 u64UserID = 0);
	static WTL::CString GetSoftConfigPath();
	static WTL::CString GetUserPicCachePath();
	static ATL::CString GetAllPicCachePath();
	static WTL::CString GetResString(UINT nID);
	static WTL::CString GetGTVersion();
	static WTL::CString GetAvatarPath();
	static void InitExePath();
	static WTL::CString GetConfigPath();
	static WTL::CString GetTempPath();

#ifdef _TEST_BETA
	static WTL::CString GetBetaVersion();		// 返回beta版完整版本号
	static WTL::CString GetMinVersion();		// 返回用于显示的小版本号
#endif

	// 替换HTML转义字符
	static void ReplaceEscapeChar(ATL::CString&);
	static void ReplaceEscapeChar(WTL::CString&);

	static BOOL LoadOtherXmlData();
    static WTL::CString GetKeyName(int nKeyValue);

    static BOOL InitChannelGroupCombo(CComboBox* pComboBox);
    static BOOL ShowGroupItemCombo(CComboBox* pComboBox, int nGID = 0);
    static BOOL InitAuthCombo(CComboBox* pComboBox);

	// 判断是否XP或更高版本 [4/28/2010 何春龙]
	static BOOL IsOSHigherThanXP();

	// 判断是否全是数字 [5/4/2010 何春龙]
	static BOOL IsAllNumber(WTL::CString&);
};

#ifdef CHECK_LOG
#define Check_Log(a,b)		{CEnv::m_oLog.logW(a, b);}
#else
#define Check_Log(a, b)		{}
#endif
