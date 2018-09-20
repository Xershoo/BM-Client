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
	static QString	strGtPath; //GT的安装目录
	static QString strGtVersionPath; //版本号，不是EXE的版本号，而是文件夹的版本号
	static int	nLanguageId_;
	
public:
	//用户登录后以其USERID获取配置文件夹，该文件夹可能有其配置相关的文件，聊天记录等内容。为0时表情默认的当前用户
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
