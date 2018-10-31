#pragma once
#include <string>

using namespace std;

class Config
{
public:
    static Config *getConfig();
    static void freeConfig();

protected:
    Config();
    ~Config();

    void loadConfig();
public:
    string  m_loginServIp;
    string  m_loginServPort;

    string  m_urlWebSite;
    string  m_urlForgetPage;
    string  m_urlForgetPwd;
    string  m_urlNotifyMsg;
	string  m_urlRegister;

    string  m_urlClassHome;
    string  m_urlCourseSelf;
    string  m_urlUserHome;

    string  m_urlCoursewareUpload;
    string  m_urlCoursewareDown;
	string	m_urlCourseClassList;
    bool    m_tranCourseWareLocal;

    string  m_urlUserHeadImage;

    string  m_urlChatImageUpload;
    string  m_urlChatImageDown;

    string  m_urlBugReport;
    string  m_urlBugBBS;
    
    string  m_urlUpdateProtocol;
    string  m_urlUpdateManifest;

protected:
    static Config* m_instance;

protected:
    int unzipConfigFile(const char* zipFile, const char* srcFile, void** unzipBuffer, int* unzipLength,const char* pszPwd /*= NULL*/);
    void readConfigBuf(char* bufConfig,int bufLength);
};