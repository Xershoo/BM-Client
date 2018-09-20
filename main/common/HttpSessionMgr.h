//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�HttpSessionMgr.cpp
//	�汾�ţ�1.0
//	���ߣ�����
//	ʱ�䣺2015.12.7
//	˵����http���ϴ������أ�����
//	�޸ļ�¼��
//  ��ע��
//**********************************************************************

#ifndef HTTPSESSIONMGR1_H
#define HTTPSESSIONMGR1_H

#include "varname.h"
#include <QObject>
#include <QMutex>
#include <string>
#include <map>
#include <process.h>
#include <event.h>
#include <QEvent>
#include "CL_Http_thread.h"


#define HTTP_EVENT_CLOSE QEvent::User + 1028
#define HTTP_EVENT_HEAD  QEvent::User + 1029

using namespace std;

enum
{
    HTTP_SESSION_SUCCESS = 200,
    HTTP_SESSION_BAD_PARAM = 400,
    HTTP_AUTHORIZE_FAILED = 401,
    INTERNAL_SERVER_ERROR = 500,
    UPLOAD_REQ_NOT_MULTIPL_PART = 40001,
    UNSUPPORTED_IMG_TYPE = 40002,
    TOKEN_NOT_EXSIT_OR_EXPIRE = 40101,
};

enum
{
    HTTP_SESSION_GET = 0,
    HTTP_SESSION_POST,
    HTTP_SESSION_PUT,
    HTTP_SESSION_DELETE,
    HTTP_SESSION_DOWNLOAD,
    HTTP_SESSION_UPLOAD
};

enum
{
    HTTP_SESSION_WAIT = 0,
    HTTP_SESSION_BEGIN,
    HTTP_SESSION_PROGRESS,
    HTTP_SESSION_END
};

enum eDowloadFileType
{
    eDownloadFile_ChatImage = 1,
};

enum HttpEventType
{
    HTTP_EVENT_BEGIN = 0,
    HTTP_EVENT_PROGRESS,
    HTTP_EVENT_END,
};

typedef struct _HttpSession 
{
public:
    _HttpSession()
    {
        memset(this,NULL,sizeof(_HttpSession));
    }

    UINT			id;
    int				timeout;
    int				type;
    string			url;
    string			response;
    string			file;
    string			reof;			
    string			error;
    string			sname;
    long			rcode;
    UINT			prgpet;
    void            *pThrd;
    int				nstate;
    int				nFileType;
    int				nDoTime;
    int				nRetryTimes;	//���Դ���
    long long		nFileTotalSize;	//�ļ��ܴ�С
    long long		nFilesNowFileSize;//��ǰ�ļ��Ĵ�С(���ڶϵ�����)
    long long		nDataSize;		//����/���������ܴ�С
    bool			bCancle;
    bool            bDelSelf;
    LPVOID			lParam;
    LPVOID          lpCbFunc;
}HTTPSESSION,*LPHTTPSESSION;

typedef void (*scHttpEventCallBack)(int nEventType, void *lpUser, bool bIsFirst, void *Param);

class CHttpClient;

class CHttpSessionMgr : public QObject
{
    Q_OBJECT
public:
    CHttpSessionMgr();

    virtual ~CHttpSessionMgr();


public:

    typedef map<UINT,LPHTTPSESSION>		HttpSessionMap;

public:
    static CHttpSessionMgr* GetInstance();
    static void freeInstance();
    static bool isValid();

    virtual int HttpDownloadFile(LPCSTR url, LPCSTR file, int timeout, long long nTotalSize = 0, LPVOID lpCbFunc = NULL, LPVOID lParam = NULL);;
    virtual int HttpDownloadFile(LPCSTR url, LPCSTR file, int nFileType, int nRetryTimes, int timeout, LPVOID lpCbFunc = NULL, LPVOID lParam = NULL);
    virtual int HttpUploadFile(LPCSTR url, LPCSTR file, LPCSTR form, int timeout, LPCSTR sfname = NULL, LPVOID lpCbFunc = NULL, LPVOID lParam = NULL);
    virtual int DoHttpRequest(int type, LPCSTR url, LPCSTR request, int timeout, LPVOID lpCbFunc = NULL, LPVOID lParam = NULL);

    virtual bool RedoSession(LPHTTPSESSION pSession);

    LPHTTPSESSION GetHttpSession(UINT id);
    LPHTTPSESSION GetHttpSession(LPCSTR url,LPCSTR file,int type);

	bool isExsitSession(LPHTTPSESSION pSession);

    void FreeAllHttpSession();
    void AddHttpThreadCount()
    {
        if (m_nHttpThreadCount > 0)
        {
            m_nHttpThreadCount--;
        }
    }

signals:
    void httpEvent(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param);

protected:
   
    void timerEvent(QTimerEvent *event); 

    void HttpProgressCallBack(LPVOID lParam, long nTotal, long nNow, LPVOID userData = NULL);

protected:

    long long GetFileSize(LPCSTR lpFilePath);
	int	getRunSession(LPCSTR url,LPCSTR file,int type,LPVOID lpCbFunc, LPVOID lParam);
private:
    static void http_dowload_session_func(void *pUser, QEvent *e);

    bool HttpDowloadSessionFunc();

    static void http_session_func(void *pUser);

    bool HttpSessionFunc(LPHTTPSESSION pSession);

public:
    QMutex          m_mutexHttpSession;

    //����ͼƬ����
    HttpSessionMap  m_mapListSession;

protected:
    HttpSessionMap  m_mapHttpSession;
    UINT            m_nHttpThreadCount;
    int             m_nTimerID;

    CL_Http_Head_Thread  *m_HeadThraed;
    QEvent          *m_HeadEvent;

    friend  class   CHttpClient;

    static int      m_nHttpID;

    bool            m_bISExit;

private:
    static CHttpSessionMgr* m_instance;
};

#endif // HTTPSESSIONMGR1_H
