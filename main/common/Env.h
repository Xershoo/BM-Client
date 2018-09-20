#pragma once

#include <atlcoll.h>
#include <vector>
#include <QString>

class Env
{
public:
	Env(void);
	virtual ~Env(void);

public:
	static QString strExePath;
	static QString m_strAppFileName; //
	static QString strTempPath;
	static BYTE chMainVer;
	static BYTE chSubVer;
	static BYTE chExVer;
	static DWORD chBuildNO;
	static BYTE	szMac[6];
	static QString	strGtPath; //GT�İ�װĿ¼
	static QString strGtVersionPath; //�汾�ţ�����EXE�İ汾�ţ������ļ��еİ汾��
	static int	nLanguageId_;
	
public:
	//�û���¼������USERID��ȡ�����ļ��У����ļ��п�������������ص��ļ��������¼�����ݡ�Ϊ0ʱ����Ĭ�ϵĵ�ǰ�û�
	static QString GetUSerConfigPath(UINT64 u64UserID = 0);
	static QString GetUserPicCachePath(UINT64 u64UserID);
	static QString GetAllPicCachePath();
	static QString GetGTVersion();
	static void InitExePath();
	static QString GetTempPath();
	static QString GetTempFilePathName(QString ext);

    static QString GetKeyName(Qt::Key key);
    static QString getExePath();
    static QString currentThemeResPath();
private:
    static bool m_init;
    
};

#ifdef CHECK_LOG
#define Check_Log(a,b)		{CEnv::m_oLog.logW(a, b);}
#else
#define Check_Log(a, b)		{}
#endif
