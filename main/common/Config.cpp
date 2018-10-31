#include "config.h"
#include <windows.h>
#include "zlib/contrib/minizip/unzip.h"

Config* Config::m_instance = NULL;

Config * Config::getConfig()
{
    if(NULL == m_instance)
    {
        m_instance = new Config();
    }

    return m_instance;
}

void Config::freeConfig()
{
    if(NULL==m_instance)
    {
		return;
	}
        
	delete m_instance;
    m_instance = NULL;
}

Config::Config():m_tranCourseWareLocal(false)
{
    loadConfig();
}

Config::~Config()
{

}

void Config::loadConfig()
{
    // Get Config file
    char szFileName[MAX_PATH]= { 0 };
    char * pszConfigBuf = NULL;
    int    lenConfigBuf = 0;;

    ::GetModuleFileNameA(NULL,szFileName,MAX_PATH);

    *(strrchr(szFileName,'\\')) = NULL;
    *(strrchr(szFileName,'\\')) = NULL;

    strcat(szFileName,"\\Config\\Config.dat");

    int ret = unzipConfigFile(szFileName,"config.ini",(void**)&pszConfigBuf,&lenConfigBuf,"BMClass@2018");
    if(ret != 0)
    {
        return;
    }
        
    readConfigBuf(pszConfigBuf,lenConfigBuf);

    if(pszConfigBuf)
    {
        free(pszConfigBuf);
        pszConfigBuf = NULL;
    }
    
    return;
}

int Config::unzipConfigFile(const char* zipFile, const char* srcFile, void** unzipBuffer, int* unzipLength,const char* pszPwd /*= NULL*/)
{	
	unzFile uf = NULL;
    int     nr = UNZ_OK;

    int     lenUnzip = 0;
    char*   bufUnzip = NULL;

    do 
    {   

        uf = ::unzOpen(zipFile);
        if (uf == NULL)
        {
            nr = -1;
            break;
        };

        nr = ::unzLocateFile(uf, srcFile, 0);
        if (nr!= UNZ_OK)
        {
            nr = -2;
            break;
        };

        nr = ::unzOpenCurrentFilePassword(uf, pszPwd);
        if (nr!= UNZ_OK)
        {
            nr = -2;
            break;
        };

        unz_file_info unzInfo = { 0 };
        ::unzGetCurrentFileInfo( uf, &unzInfo, NULL, 0, NULL, 0, NULL, 0 );
        lenUnzip = unzInfo.uncompressed_size;
        if(lenUnzip <= 0)
        {
            nr = E_FAIL;
            break;
        };

        bufUnzip = (char*)malloc(lenUnzip+1);
        if(NULL == bufUnzip)
        {
            nr = E_OUTOFMEMORY;
            break;
        }

        memset(bufUnzip,NULL,lenUnzip+1);

        lenUnzip = ::unzReadCurrentFile( uf, (void*)bufUnzip, lenUnzip);
        if(lenUnzip <= 0)
        {
            free(bufUnzip);
            bufUnzip = NULL;
            lenUnzip = 0;

            nr = E_FAIL;
        };
    } while (false);
    
    if(NULL!=uf)
    {
        ::unzCloseCurrentFile( uf );	
	    ::unzClose( uf );
    };

    if(nr==UNZ_OK)
    {
        *unzipBuffer = bufUnzip;
        *unzipLength = lenUnzip;
    };
	return nr;
}

void Config::readConfigBuf(char* bufConfig,int bufLength)
{
    if (NULL==bufConfig||bufLength<=0)
    {
        return;
    }

    char szTempFile[1024] = { 0 };
    char szTempPath[1024] = { 0 };
    bool br = false;
    
    do 
    {    
        ::GetTempPathA(sizeof(szTempPath),szTempPath);
        strcpy_s(szTempFile,szTempPath);
        strcat_s(szTempFile,"config_class8.ini");

        try
        {
            FILE* batFile = fopen(szTempFile,"w+");
            if(NULL == batFile)
            {
                break;
            }
            
            fwrite(bufConfig,sizeof(char),bufLength,batFile);

            fclose(batFile);
        }
        catch(...)
        {
            break;
        }

        br = true;

    } while (false);

    if(!br)
    {
        return;
    }

    char szTemp[1024]={0};
    GetPrivateProfileStringA("login_serv","ser_ip","",szTemp,1024,szTempFile);    
    m_loginServIp = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("login_serv","port","",szTemp,1024,szTempFile);
    m_loginServPort = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("web_link","site_url","",szTemp,1024,szTempFile);
    m_urlWebSite = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("web_link","forget_page_url","",szTemp,1024,szTempFile);
    m_urlForgetPage = szTemp;

	memset(szTemp,NULL,sizeof(szTemp));
	GetPrivateProfileStringA("web_link","register_page_url","",szTemp,1024,szTempFile);
	m_urlRegister = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("web_link","forget_pwd_url","",szTemp,1024,szTempFile);
    m_urlForgetPwd = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("web_link","notify_msg_url","",szTemp,1024,szTempFile);
    m_urlNotifyMsg = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("class_hall","home_url","",szTemp,1024,szTempFile);
    m_urlClassHome = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("class_hall","class_url","",szTemp,1024,szTempFile);
    m_urlCourseSelf = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("class_hall","user_url","",szTemp,1024,szTempFile);
    m_urlUserHome = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("coureware","upload_url","",szTemp,1024,szTempFile);
    m_urlCoursewareUpload = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("coureware","down_url","",szTemp,1024,szTempFile);
    m_urlCoursewareDown = szTemp;

	memset(szTemp,NULL,sizeof(szTemp));
	GetPrivateProfileStringA("coureware","list_url","",szTemp,1024,szTempFile);
	m_urlCourseClassList = szTemp;

    m_tranCourseWareLocal = (bool)GetPrivateProfileIntA("coureware","local_tran",0,szTempFile);

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("head_image","down_url","",szTemp,1024,szTempFile);
    m_urlUserHeadImage = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("chat_image","upload_url","",szTemp,1024,szTempFile);
    m_urlChatImageUpload = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("chat_image","down_url","",szTemp,1024,szTempFile);
    m_urlChatImageDown = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("bug_report","upload_url","",szTemp,1024,szTempFile);
    m_urlBugReport = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("bug_report","bbs_url","",szTemp,1024,szTempFile);
    m_urlBugBBS = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("update","protocol_url","",szTemp,1024,szTempFile);
    m_urlUpdateProtocol = szTemp;

    memset(szTemp,NULL,sizeof(szTemp));
    GetPrivateProfileStringA("update","mainfest_url","",szTemp,1024,szTempFile);
    m_urlUpdateManifest = szTemp;

    ::DeleteFileA(szTempFile);
}