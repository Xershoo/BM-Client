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
	static WTL::CString	strGtPath; //GT�İ�װĿ¼
	static WTL::CString strGtVersionPath; //�汾�ţ�����EXE�İ汾�ţ������ļ��еİ汾��
	static int	nLanguageId_;
private:
    static ChannelGroups                m_ChannelGroups;
    static std::vector<WTL::CString>    m_vecAuthType;
	
public:
	static WTL::CString GetLanternDataPath();
	//�û���¼������USERID��ȡ�����ļ��У����ļ��п�������������ص��ļ��������¼�����ݡ�Ϊ0ʱ����Ĭ�ϵĵ�ǰ�û�
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
	static WTL::CString GetBetaVersion();		// ����beta�������汾��
	static WTL::CString GetMinVersion();		// ����������ʾ��С�汾��
#endif

	// �滻HTMLת���ַ�
	static void ReplaceEscapeChar(ATL::CString&);
	static void ReplaceEscapeChar(WTL::CString&);

	static BOOL LoadOtherXmlData();
    static WTL::CString GetKeyName(int nKeyValue);

    static BOOL InitChannelGroupCombo(CComboBox* pComboBox);
    static BOOL ShowGroupItemCombo(CComboBox* pComboBox, int nGID = 0);
    static BOOL InitAuthCombo(CComboBox* pComboBox);

	// �ж��Ƿ�XP����߰汾 [4/28/2010 �δ���]
	static BOOL IsOSHigherThanXP();

	// �ж��Ƿ�ȫ������ [5/4/2010 �δ���]
	static BOOL IsAllNumber(WTL::CString&);
};

#ifdef CHECK_LOG
#define Check_Log(a,b)		{CEnv::m_oLog.logW(a, b);}
#else
#define Check_Log(a, b)		{}
#endif
