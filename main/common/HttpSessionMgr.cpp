//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：HttpSessionMgr.h
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.7
//	说明：http（上传、下载）管理
//	修改记录：
//  备注：
//**********************************************************************

#include "HttpSessionMgr.h"
#include "HttpClient.h"
#include <process.h>
#include <QFileInfo>
#include <QtCore/QCoreApplication>
#include <QDebug>
#include "CL_Http_thread.h"
#include "macros.h"

#define MAX_HTTP_THREAD		(20)

int CHttpSessionMgr::m_nHttpID = 1;

CHttpSessionMgr* CHttpSessionMgr::m_instance = NULL;

CHttpSessionMgr::CHttpSessionMgr()
{
    m_HeadEvent = NULL;
    m_HeadThraed = NULL;
    m_nHttpThreadCount = 0;
    m_bISExit = false;
   
    m_nTimerID = this->startTimer(5000);
}

CHttpSessionMgr::~CHttpSessionMgr()
{
    if (m_nTimerID != 0)
    {
        this->killTimer(m_nTimerID);
        m_nTimerID = 0;
    }
    
    FreeAllHttpSession();
    m_bISExit = true;
    if (m_HeadThraed)
    {
        m_HeadThraed->stopthread();
        delete m_HeadThraed;
        m_HeadThraed = NULL;
        delete m_HeadEvent;
        m_HeadEvent = NULL;
    }
}

CHttpSessionMgr * CHttpSessionMgr::GetInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new CHttpSessionMgr();
    }

    return m_instance;
}

void CHttpSessionMgr::freeInstance()
{
    SAFE_DELETE(m_instance);
}

bool CHttpSessionMgr::isValid()
{
    return m_instance ? true:false;
}

void CHttpSessionMgr::timerEvent(QTimerEvent *event)
{
    if (m_nTimerID == 0)
    {
        return;
    }

    QMutexLocker AutoLock(&m_mutexHttpSession);
    
    for (HttpSessionMap::iterator itr = m_mapHttpSession.begin(); itr != m_mapHttpSession.end(); itr++)
    {
        if (m_nHttpThreadCount >= MAX_HTTP_THREAD)
        {
            break;
        }
        LPHTTPSESSION pSession = itr->second;
        if (NULL == pSession)
        {
            continue;
        }
        if (NULL == pSession->pThrd && HTTP_SESSION_WAIT == pSession->nstate)
        {
            CL_Http_Thread *httpThread = new CL_Http_Thread;
            httpThread->startthread(http_session_func,(void*)pSession);
            pSession->pThrd = (void*)httpThread;

            m_nHttpThreadCount++;
            pSession->nstate = HTTP_SESSION_PROGRESS;
        }
    }
    AutoLock.unlock();
}

int CHttpSessionMgr::HttpDownloadFile(LPCSTR url, LPCSTR file, int nFileType, int nRetryTimes, int timeout, LPVOID lpCbFunc/* = NULL*/, LPVOID lParam /* = NULL */)
{
    if (NULL == url || NULL == url[0] || NULL == file || NULL == file[0])
    {
        return -1;
    }

    LPHTTPSESSION pHttpSess = new _HttpSession();
    if(NULL == pHttpSess)
    {
        return -1;
    }
    m_nHttpID++;
    pHttpSess->id = m_nHttpID;
    pHttpSess->type = HTTP_SESSION_DOWNLOAD;
    pHttpSess->url = url;
    pHttpSess->file = file;
    pHttpSess->timeout = timeout;
    pHttpSess->nDoTime++;
    pHttpSess->nFileType = nFileType;
    pHttpSess->nRetryTimes = nRetryTimes;
    pHttpSess->nDataSize = 0;
    pHttpSess->nFileTotalSize = 0;
    pHttpSess->nFilesNowFileSize = GetFileSize(file);
    pHttpSess->lParam = lParam;
    pHttpSess->lpCbFunc = lpCbFunc;

    QMutexLocker AutoLock(&m_mutexHttpSession);
    m_mapListSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
    AutoLock.unlock();

    if (NULL == m_HeadThraed)
    {
        m_HeadThraed = new CL_Http_Head_Thread;
        m_HeadThraed->startthread(http_dowload_session_func, this);
        m_HeadEvent = new QEvent(QEvent::Type(HTTP_EVENT_HEAD));
    }
    else
    {
        QCoreApplication::postEvent(m_HeadThraed, m_HeadEvent);
    }

    return (int)pHttpSess->id;
}

int CHttpSessionMgr::HttpDownloadFile(LPCSTR url, LPCSTR file, int timeout, long long nTotalSize /* = 0 */, LPVOID lpCbFunc/* = NULL*/, LPVOID lParam /* = NULL */)
{
    if (NULL == url || NULL == url[0] || NULL == file || NULL == file[0])
    {
        return -1;
    }

    int nHttpId = getRunSession(url,file,HTTP_SESSION_DOWNLOAD,lpCbFunc,lParam);
	if( nHttpId >0 )
	{
		return nHttpId;
	}

    LPHTTPSESSION pHttpSess = new HTTPSESSION();
    if (NULL == pHttpSess)
    {
        return -1;
    }

    m_nHttpID++;
    pHttpSess->id = m_nHttpID;
    pHttpSess->type = HTTP_SESSION_DOWNLOAD;
    pHttpSess->url = url;
    pHttpSess->file = file;
    pHttpSess->nDoTime++;
    pHttpSess->timeout = timeout;	
    pHttpSess->nDataSize = 0;
    pHttpSess->nFileTotalSize = 0;
    pHttpSess->nFilesNowFileSize = GetFileSize(file);
    pHttpSess->lParam = lParam;
    pHttpSess->lpCbFunc = lpCbFunc;

    if(m_nHttpThreadCount < MAX_HTTP_THREAD)
    {
        CL_Http_Thread *HttpThread = new CL_Http_Thread;

        pHttpSess->pThrd = (void*)HttpThread;
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();

        HttpThread->startthread(http_session_func,pHttpSess);
        m_nHttpThreadCount++;
        pHttpSess->nstate = HTTP_SESSION_PROGRESS;        
    }
    else
    {
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();
    }

    return (int)pHttpSess->id;
}

int CHttpSessionMgr::HttpUploadFile(LPCSTR url, LPCSTR file, LPCSTR form, int timeout, LPCSTR sfname /* = NULL */, LPVOID lpCbFunc/* = NULL*/, LPVOID lParam /* = NULL */)
{
    if (NULL == url || NULL == url[0] || NULL == file || NULL == file[0])
    {
        return -1;
    }

	int nHttpId = getRunSession(url,file,HTTP_SESSION_UPLOAD,lpCbFunc,lParam);
	if( nHttpId >0 )
	{
		return nHttpId;
	}

	LPHTTPSESSION pHttpSess = new HTTPSESSION();
    if (NULL == pHttpSess)
    {
        return -1;
    }
    m_nHttpID++;
    pHttpSess->id = m_nHttpID;
    pHttpSess->type = HTTP_SESSION_UPLOAD;
    pHttpSess->url = url;
    pHttpSess->file = file;
    pHttpSess->timeout = timeout;
    pHttpSess->nDoTime = 1;
    pHttpSess->nDataSize = 0;
    pHttpSess->lParam = lParam;
    pHttpSess->nFileTotalSize  = GetFileSize(file);
    pHttpSess->nFilesNowFileSize = 0;
    pHttpSess->lpCbFunc = lpCbFunc;
    if(NULL != form)
    {
        pHttpSess->reof = form;
    }

    if(NULL != sfname)
    {
        pHttpSess->sname = sfname;
    }

    if(m_nHttpThreadCount < MAX_HTTP_THREAD)
    {
        CL_Http_Thread *HttpThread = new CL_Http_Thread;

        pHttpSess->pThrd = (void*)HttpThread;
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();

        HttpThread->startthread(http_session_func, pHttpSess);
        m_nHttpThreadCount++;
        pHttpSess->nstate = HTTP_SESSION_PROGRESS;
    }
    else
    {
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();
    }

    return (int)pHttpSess->id;
}

int CHttpSessionMgr::DoHttpRequest(int type, LPCSTR url, LPCSTR request, int timeout, LPVOID lpCbFunc/* = NULL*/, LPVOID lParam /* = NULL */)
{
    if (NULL == url || NULL == url[0])
    {
        return -1;
    }

	int nHttpId = getRunSession(url,request,type,lpCbFunc,lParam);
	if( nHttpId >0 )
	{
		return nHttpId;
	}

	LPHTTPSESSION pHttpSess = new HTTPSESSION();
    if (NULL == pHttpSess)
    {
        return -1;
    }
    m_nHttpID++;
    pHttpSess->id = m_nHttpID;
    pHttpSess->type = type;
    pHttpSess->url = url;
    pHttpSess->nDoTime = 1;
    pHttpSess->lParam = lParam;
    pHttpSess->timeout = timeout;
    pHttpSess->lpCbFunc = lpCbFunc;
    if(NULL != request)
    {
        pHttpSess->reof = request;
    }

    if(m_nHttpThreadCount < MAX_HTTP_THREAD)
    {
        CL_Http_Thread *HttpThread = new CL_Http_Thread;

        pHttpSess->pThrd = (void*)HttpThread;
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();

        HttpThread->startthread(http_session_func, pHttpSess);
        m_nHttpThreadCount++;
        pHttpSess->nstate = HTTP_SESSION_PROGRESS;
    }
    else
    {
        QMutexLocker AutoLock(&m_mutexHttpSession);
        m_mapHttpSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
        AutoLock.unlock();
    }
    
    return (int)pHttpSess->id;
}

bool CHttpSessionMgr::RedoSession(LPHTTPSESSION pSession)
{
    if (NULL == pSession)
    {
        return false;
    }
    if(m_nHttpThreadCount < MAX_HTTP_THREAD)
    {
        CL_Http_Thread *HttpThread = new CL_Http_Thread;
        HttpThread->startthread(http_session_func, pSession);
        pSession->pThrd = (void*)HttpThread;
        pSession->nDoTime++;
        m_nHttpThreadCount++;
    }
    else
    {
        pSession->nstate = HTTP_SESSION_WAIT;
        pSession->pThrd = NULL;
    }

    return true;
}

LPHTTPSESSION CHttpSessionMgr::GetHttpSession(UINT id)
{
    QMutexLocker AutoLock(&m_mutexHttpSession);
    HttpSessionMap::iterator itr = m_mapHttpSession.find(id);
    if(itr == m_mapHttpSession.end())
    {
        AutoLock.unlock();
        return NULL;
    }

    AutoLock.unlock();
    return itr->second;
}

LPHTTPSESSION CHttpSessionMgr::GetHttpSession(LPCSTR url,LPCSTR file,int type)
{
    if(m_mapHttpSession.empty())
    {
        return NULL;
    }

    if(NULL == url || NULL==url[0] || NULL == file || NULL == file[0])
    {
        return NULL;
    }

    QMutexLocker AutoLock(&m_mutexHttpSession);
    HttpSessionMap::iterator itr = m_mapHttpSession.begin();
    for(;itr != m_mapHttpSession.end();itr++)
    {
        LPHTTPSESSION pSession = itr->second;
        if(NULL == pSession)
        {
            continue;
        }
        
        if (_stricmp(pSession->url.c_str(),url) == 0 &&
            _stricmp(pSession->file.c_str(),file) == 0 &&
            pSession->type == type)
        {
			m_mapHttpSession.erase(itr);
            return pSession;
        }
    }

    return NULL;
}

void CHttpSessionMgr::FreeAllHttpSession()
{
    QMutexLocker AutoLock(&m_mutexHttpSession);
    if (m_mapHttpSession.empty())
    {   
        return;
    }

    for (HttpSessionMap::iterator itr = m_mapHttpSession.begin(); itr != m_mapHttpSession.end(); itr++)
    {
        LPHTTPSESSION pSession = itr->second;
        if (NULL == pSession)
        {
            continue;
        }

        pSession->bCancle = true;

        if (NULL != pSession->pThrd)
        {
            CL_Http_Thread * HttpThread = (CL_Http_Thread*)pSession->pThrd;
            
            if(!HttpThread->isFinished())
            {
                HttpThread->stopthread();
                pSession->bDelSelf = true;
                
                continue;
            }

            delete HttpThread;
            HttpThread = NULL;
        }

        delete pSession;
        pSession = NULL;
    }

    m_mapHttpSession.clear();
}

void CHttpSessionMgr::HttpProgressCallBack(LPVOID lParam, long nTotal, long nNow, LPVOID userData /*= NULL*/)
{
    LPHTTPSESSION pHttpSess = (LPHTTPSESSION)lParam;

    if(NULL == pHttpSess || 0 ==  nNow || nTotal <= 0)
    {		
        return;
    }

    if(!CHttpSessionMgr::isValid())
    {
        return;
    }

    int nPos = 0;
    if (nNow > 1024 && nTotal > 1024)
    {
        nPos = (nNow/1024*100)/(nTotal/1024);
    }
    else
    {
        nPos = (nNow*100)/nTotal;
    }
     
    if (pHttpSess->nFileTotalSize == 0)
    {
        pHttpSess->nFileTotalSize = pHttpSess->nFilesNowFileSize + nTotal;
        /*
        if ((scHttpEventCallBack)pHttpSess->lpCbFunc)
        {
            ((scHttpEventCallBack)pHttpSess->lpCbFunc)(HTTP_EVENT_PROGRESS, pHttpSess->lParam, true, (LPVOID)pHttpSess);
        }
        */
        emit httpEvent(HTTP_EVENT_PROGRESS, (unsigned int)(pHttpSess->lParam), false, (unsigned int)pHttpSess);
    }

    if (nPos < 100 && nPos > (pHttpSess->prgpet + 8) && pHttpSess->nstate != HTTP_SESSION_END)
    {
        pHttpSess->prgpet = (UINT)nPos;
        /*
        if ((scHttpEventCallBack)pHttpSess->lpCbFunc)
        {
            ((scHttpEventCallBack)pHttpSess->lpCbFunc)(HTTP_EVENT_PROGRESS, pHttpSess->lParam, false, (LPVOID)pHttpSess);
        }
        */

        emit httpEvent(HTTP_EVENT_PROGRESS, (unsigned int)(pHttpSess->lParam), false, (unsigned int)pHttpSess);
        
    }
}

long long CHttpSessionMgr::GetFileSize(LPCSTR lpFilePath)
{
    if (NULL == lpFilePath || NULL == lpFilePath[0])
    {
        return 0;
    }
    QFileInfo fi(lpFilePath);
    if (!fi.exists())
    {
        return 0;
    }

    return fi.size();//返回文件大小 byte
}

void CHttpSessionMgr::http_session_func(void *pUser)
{
    if (NULL == pUser || NULL == m_instance)
    {
        return;
    }

    LPHTTPSESSION pSession = (LPHTTPSESSION)pUser;
    m_instance->HttpSessionFunc(pSession);
}

bool CHttpSessionMgr::HttpSessionFunc(LPHTTPSESSION pSession)
{   
    if(NULL == pSession)
    {
        return false;
    }

    if(pSession->nDoTime > 1)
    {
        int nSleep = pSession->nDoTime;
        QThread::sleep(nSleep);
    }

    /*
    if ((scHttpEventCallBack)pSession->lpCbFunc)
    {
        ((scHttpEventCallBack)pSession->lpCbFunc)(HTTP_EVENT_BEGIN, pSession->lParam, false, (LPVOID)pSession);
    }
    */

    emit httpEvent(HTTP_EVENT_BEGIN, (unsigned int)(pSession->lParam), false, (unsigned int)pSession);

    CHttpClient httpClient;
    httpClient.SetParam(pSession);

    switch(pSession->type)
    {
    case HTTP_SESSION_GET:
        httpClient.Get(pSession->url,pSession->response,pSession->timeout);
        break;
    case HTTP_SESSION_POST:
        httpClient.Post(pSession->url,pSession->reof,pSession->response,pSession->timeout);
        break;
    case HTTP_SESSION_PUT:
        httpClient.Put(pSession->url,pSession->reof,pSession->response,pSession->timeout);
        break;
    case HTTP_SESSION_DELETE:
        httpClient.Delete(pSession->url,pSession->response,pSession->timeout);
        break;
    case HTTP_SESSION_UPLOAD:
        httpClient.Upload(pSession->url,pSession->file.c_str(),pSession->response,pSession->timeout,pSession->reof,pSession->sname.c_str());
        break;
    case HTTP_SESSION_DOWNLOAD:
        httpClient.Save(pSession->url,pSession->file.c_str(),pSession->timeout, pSession->nFileTotalSize);
        break;
    }
        
    if(pSession->bDelSelf || !CHttpSessionMgr::isValid())
    {
        delete pSession;
        return false;
    }

    pSession->rcode = httpClient.GetResponseCode();
    pSession->error = httpClient.GetErrorString();
    pSession->nstate = HTTP_SESSION_END;

    /*
    if ((scHttpEventCallBack)pSession->lpCbFunc)
    {
        ((scHttpEventCallBack)pSession->lpCbFunc)(HTTP_EVENT_END, pSession->lParam, false, (LPVOID)pSession);
    }
    */
	
	AddHttpThreadCount();
    
	emit httpEvent(HTTP_EVENT_END, (unsigned int)(pSession->lParam), false, (unsigned int)pSession);
    
    return true;
}

void CHttpSessionMgr::http_dowload_session_func(void *pUser, QEvent *e)
{
    if (NULL == pUser && NULL == e)
    {
        return;
    }
    if (HTTP_EVENT_HEAD == e->type())
    {
        CHttpSessionMgr *pThis = (CHttpSessionMgr *)pUser;
        pThis->HttpDowloadSessionFunc();
    }
}

bool CHttpSessionMgr::HttpDowloadSessionFunc()
{
    if (0 == m_mapListSession.size())
    {
        return false;
    }

    LPHTTPSESSION pSess = NULL;
    QMutexLocker AutoLock(&m_mutexHttpSession);
    while (m_mapListSession.size() > 0)
    {
        pSess = m_mapListSession[0];
        if (NULL == pSess)
        {
            continue;
        }
        if (pSess->type == HTTP_SESSION_DOWNLOAD)
        {
            if ((scHttpEventCallBack)pSess->lpCbFunc)
            {
                ((scHttpEventCallBack)pSess->lpCbFunc)(HTTP_EVENT_BEGIN, pSess->lParam, false, (LPVOID)pSess);
            }

            CHttpClient httpClient;
            httpClient.SetParam(pSess);
            do 
            {
                if(httpClient.Save(pSess->url,pSess->file.c_str(),pSess->timeout))
                    break;
                if (m_bISExit)
                {
                    break;
                }
                pSess->nRetryTimes--;
            } while (pSess->nRetryTimes);
            pSess->rcode = httpClient.GetResponseCode();
            pSess->error = httpClient.GetErrorString();

            if ((scHttpEventCallBack)pSess->lpCbFunc)
            {
                ((scHttpEventCallBack)pSess->lpCbFunc)(HTTP_EVENT_END, pSess->lParam, false, (LPVOID)pSess);
            }
        }
    }
    AutoLock.unlock();
    return true;
}

bool CHttpSessionMgr::isExsitSession(LPHTTPSESSION pSession)
{
	if(NULL == pSession)
	{
		return false;
	}

	if (m_mapHttpSession.empty())
	{	
		return false;
	}

	QMutexLocker AutoLock(&m_mutexHttpSession);	
	for (HttpSessionMap::iterator itr = m_mapHttpSession.begin(); itr != m_mapHttpSession.end(); itr++)
	{
		LPHTTPSESSION pSess = itr->second;
		if (pSess == pSession)
		{
			return true;
		}
	}

	return false;
}

int CHttpSessionMgr::getRunSession(LPCSTR url,LPCSTR file,int type,LPVOID lpCbFunc, LPVOID lParam)
{
	LPHTTPSESSION pHttpSess =  GetHttpSession(url,file,type);
	if(NULL == pHttpSess)
	{
		return 0;
	}

	CL_Http_Thread* httpThread = static_cast<CL_Http_Thread*>(pHttpSess->pThrd);
	if(NULL != httpThread && !httpThread->isFinished())
	{
		pHttpSess->lParam = lParam;
		pHttpSess->lpCbFunc = lpCbFunc;
		pHttpSess->bDelSelf = false;

		QMutexLocker AutoLock(&m_mutexHttpSession);
		m_mapListSession.insert(HttpSessionMap::value_type(pHttpSess->id,pHttpSess));
		
		return pHttpSess->id;
	}

	SAFE_DELETE(pHttpSess->pThrd);
	SAFE_DELETE(pHttpSess);
	
	return 0;
}
