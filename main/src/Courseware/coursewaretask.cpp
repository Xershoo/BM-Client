//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaretask.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：课件下载任务
//	修改记录：
//  备注：
//**********************************************************************

#include "coursewaretask.h"
#include <QString>
#include <QDebug>
#include <string.h>
#include "././common/HttpSessionMgr.h"
#include "././common/Util.h"
#include "jason/include/json.h"
#include "./session/classsession.h"
#include "../media/publish/libPublish/include/PublishInterface.h"
#include "../session/classsession.h"
#include "CoursewareTaskMgr.h"
#include "env.h"
#include <QFile>
#include "common/Config.h"

void CoursewareTask::HttpEventCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param)
{
	CoursewareTask *pTask = (CoursewareTask*)lpUser;
	LPHTTPSESSION pHttpS = (LPHTTPSESSION)Param;
	if (NULL == pTask || NULL == pHttpS || pTask!= this)
	{
		return;
	}

    if(!CoursewareTaskMgr::isValidate())
    {	
        return;
    }

    CoursewareTaskMgr* pTaskMgr = CoursewareTaskMgr::getInstance();
    if(NULL == pTaskMgr)
    {
	    return;
    }
		

    if(pTask->isStop() || pTask->m_doHttpEvent)
    {		
		pTask->m_doHttpEvent = true;
        return;
    }

	PTASKMGR pTaskInfo = pTaskMgr->GetTask(pTask->m_data.m_nCoursewareID);
	if(NULL == pTaskInfo || pTaskInfo->m_pData != (void*)pTask)
	{
		pTask->m_doHttpEvent = false;
		return;
	}
		
    switch (httpEventType)
    {
    case HTTP_EVENT_BEGIN:
        {
            pTask->HttpEventBengin(pHttpS->id, pHttpS->type);
			pTask->m_doHttpEvent = false;
        }
        break;

    case HTTP_EVENT_PROGRESS:
        {
            if (bIsFirst)
            {
                pTask->HttpEventProgress(pHttpS->id, pHttpS->prgpet, bIsFirst, pHttpS->nFileTotalSize);
            }
            else
            {
                pTask->HttpEventProgress(pHttpS->id, pHttpS->prgpet);
            }

			pTask->m_doHttpEvent = false;
        }
        break;

    case HTTP_EVENT_END:
        {
            pTask->HttpEventEnd((LPVOID)Param);
        }
        break;
    }
}

CoursewareTask::CoursewareTask(PCOURSEWAREDATA pData,QObject *parent /* = NULL */)
{
    if (pData)
    {
        m_data = *pData;
    }

    m_bISLocalTrans = Config::getConfig()->m_tranCourseWareLocal;

    connect(&m_UploadThread, SIGNAL(finished()), this, SLOT(doFinished()));
    connect(&m_DownThread, SIGNAL(finished()), this, SLOT(doFinished()));
    connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpEventCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);

    m_TaskFunc = NULL;
    m_TaskProFunc = NULL;
    m_pTaskUser = NULL;
    m_stop = false;
	m_doHttpEvent = false;
}

CoursewareTask::~CoursewareTask()
{
	this->disconnect(NULL,NULL,NULL,NULL);
}

void CoursewareTask::doFinished()
{
    if (!m_stop)
    {
		return;
	}
    
	stopHttpSession();

	m_TaskFunc = NULL;
    m_TaskProFunc = NULL;
    m_pTaskUser = NULL;    
	delete this;
	return;
}

bool CoursewareTask::StartUploadTask()
{
    m_stop = false;
    m_UploadThread.startthread(UploadFunc, this);
    return true;
}

bool CoursewareTask::StopUploadTask()
{
	disconnect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpEventCallBack(int, unsigned int, bool, unsigned int)));
	
	stopHttpSession();

	QThread::msleep(100);

    if (COURSEWARE_AUDIO == m_data.m_nFileType || COURSEWARE_VIDEO == m_data.m_nFileType)
    {
        //停止音视频转码
        StopTrancodeFile();
        SetEventCallback(NULL, NULL);
    }

    if(m_UploadThread.isFinished())
    {
        m_stop = true;
        delete this;    
    }
    else
    {
        m_UploadThread.stopthread();    
        m_stop = true;
    }

    return true;
}

bool CoursewareTask::StartDownTask()
{
    m_stop = false;
    m_DownThread.startthread(DownFunc, this);
    return true;
}

bool CoursewareTask::StopDownTask()
{ 
	disconnect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpEventCallBack(int, unsigned int, bool, unsigned int)));

	stopHttpSession();

	QThread::msleep(100);

    if(m_DownThread.isFinished())
    {
        m_stop = true;
        delete this;    
    }
    else
    {
        m_DownThread.stopthread();    
        m_stop = true;
    }

    return true;
}

void CoursewareTask::DownFunc(void *pUser)
{
    if (NULL == pUser)
    {
        return;
    }
    CoursewareTask *pThis = (CoursewareTask *)pUser;
    pThis->ExecDown();
}

bool CoursewareTask::ExecDown()
{
    int nState = ACTION_OTHER;
    int nError = 0;

    wchar_t szFile[MAX_PATH] = {0};
    wcsncpy(szFile, m_data.m_szFilePath, wcslen(m_data.m_szFilePath)-4);
    if (IsExistFile(szFile))
    {
        //文件已经存在
        wcscpy(m_data.m_szFilePath, szFile);
        m_data.m_nState = COURSEWARE_STATE_DOWN;
        //设置当前进度为100；
        SetPos(100);
        m_data.m_nState = COURSEWARE_STATE_OK;
        m_data.m_nDownState = CW_FILE_DOWN_END;
        
        nState = ACTION_DOWN_END;

        m_TaskFunc(TASK_EVENT_TYPE_DOWN, m_pTaskUser, nState, (void*)(&m_data), COURSEWARE_ERR_OK);
    }
    else
    {
        char url[MAX_URL_LENGTH+1] = {0};
        char path[MAX_PATH+1] = {0};
        
        Util::UnicodeToAnsi(m_data.m_szDownUrl, wcslen(m_data.m_szDownUrl), url, MAX_URL_LENGTH);
        Util::UnicodeToAnsi(m_data.m_szFilePath, wcslen(m_data.m_szFilePath), path, MAX_PATH);

        int nID = CHttpSessionMgr::GetInstance()->HttpDownloadFile(url, path, HTTP_DOWN_TIMEOUT, 0, (LPVOID)NULL, (LPVOID)this);
        if (-1 != nID)
        {
            m_data.m_nHttpID = nID;
            m_data.m_nState = COURSEWARE_STATE_DOWN; 
            m_data.m_nDownState = CW_FILE_DOWNING;
            nState = ACTION_DOWN_BENGIN;
        }
        else
        {
            m_data.m_nDownState = CW_FILE_WAIT_DOWN;
            nError = COURSEWARE_ERR_DOWNFAILED;
            nState = ACTION_DOWN_FAILED;
        }
        
        m_data.m_nDownTime++;
       
        if (ACTION_DOWN_FAILED == nState)
        {
            m_data.m_nState = COURSEWARE_STATE_FAILED;

            m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, nState, (void*)(&m_data), nError);
        }
    }

    return true;
}

void CoursewareTask::UploadFunc(void *pUser)
{
    if (NULL == pUser)
    {
        return;
    }
    CoursewareTask * pThis = (CoursewareTask *)pUser;
    pThis->ExecUpload();
}

bool CoursewareTask::ExecUpload()
{
    char szToken[MAX_PATH] = {0};
    int nError = 0;
    if (!GetUploadToken(szToken, ClassSeeion::GetInst()->_nUserId))
    {
        //获取token失败
        
        m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, ACTION_UPLOAD_FAILED, (void*)(&m_data), nError);
        return false;
    }
    
    int nState = ACTION_OTHER;
    switch (m_data.m_nFileType)
    {
    case COURSEWARE_UNKNOWN:
        break;

    case COURSEWARE_PPT:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
        {
            if (m_bISLocalTrans)
            {
                m_data.m_nTrans = COURSEWARE_TRANS_FAILED;
                //本地转换课件
                bool br = false;//(暂时不支持本地转换课件)
                m_data.m_nState = COURSEWARE_STATE_TRANS;
                m_data.m_nCoursewareType = COURSEWARE_TYPE_TRANSPDF;
                if (!br)
                {
                    nState = ACTION_UPLOAD_FAILED;
                    nError = COURSEWARE_ERR_UPLOADFAILED;
                    break;
                }
            }
            else
            {
                if (GetCoursewareFileType(m_data.m_szFilePath) != m_data.m_nFileType)
                {
                    WCHAR *pwszType = NULL;
                    pwszType = (WCHAR *)(wcsrchr(m_data.m_szSourcePath, L'.'));
                    if (pwszType)
                    {
                        WCHAR wszType[10] = {0};
                        WCHAR wszFileName[MAX_PATH + 1] = { 0 };
                        wcsncpy(wszType, pwszType, wcslen(pwszType));
                    }
                }
                if (!CopyFileToMD5Path(m_data.m_szFilePath, m_data.m_szSourcePath))
                {
                    nState = ACTION_UPLOAD_FAILED;
                    nError = COURSEWARE_ERR_UPLOADFAILED;
                    break;
                }
                m_data.m_nState = COURSEWARE_STATE_UPLOAD;
                m_data.m_nTrans = COURSEWARE_TRANS_FAILED;
                m_data.m_nFileSize = GetCoursewareFileSize(m_data.m_szFilePath);
                m_data.m_nCoursewareType = COURSEWARE_TYPE_TRANSONSERVER;
                
                if (UploadFile(m_data.m_szName, m_data.m_szFilePath, szToken))
                {
                    nState = ACTION_UPLOAD_BENGIN;
                    break;
                }
                nState = ACTION_UPLOAD_FAILED;
                nError = COURSEWARE_ERR_UPLOADFAILED;
                break;
            }
        }
        break;

    case COURSEWARE_VIDEO:
    case COURSEWARE_AUDIO:
        {
            char szError[MAX_PATH] = {0};
            //设置转换回调函数
            SetEventCallback(&TransMediaOverCallBack, this);
            char sourcepath[1024] = {0};
            char filepath[1024] = {0};
			char taskfile[1024] = {0};
			char *filename = NULL;
			
			//先拷贝到临时目录，且没有中文字符，防止非中文系统报错
			//xiewb 2016.6.16
			Util::UnicodeToAnsi(m_data.m_szFilePath, wcslen(m_data.m_szFilePath), filepath, sizeof(filepath)-1);
			Util::UnicodeToAnsi(m_data.m_szSourcePath, wcslen(m_data.m_szSourcePath), taskfile, sizeof(taskfile)-1);
			filename = strrchr(filepath,'\\');
			if(NULL== filename)
			{
				filename = strrchr(filepath,'/');
			}

			QString qstrTemp;
			QString qstrSrc = QString::fromWCharArray(m_data.m_szSourcePath);
			Util::AnsiToQString(filename,strlen(filename),qstrTemp);
			qstrTemp = Env::GetTempPath() + qstrTemp;
			QFile::copy(qstrSrc,qstrTemp);

			Util::QStringToAnsi(qstrTemp,sourcepath,sizeof(sourcepath)-1);
			/*			
			Util::UnicodeToAnsi(m_data.m_szSourcePath, wcslen(m_data.m_szSourcePath), sourcepath, MAX_PATH);
            Util::UnicodeToAnsi(m_data.m_szFilePath, wcslen(m_data.m_szFilePath), filepath, MAX_PATH);
			*/

            bool br = TrancodeFile(sourcepath, filepath, szError);
            m_data.m_nState = COURSEWARE_STATE_TRANS;
            m_data.m_nCoursewareType = COURSEWARE_TYPE_TRANSMEDIA;
            if (!br)
            {
                QString strErr(szError);
                if (strErr.isEmpty())
                {
                    qDebug("Error :音视频转码失败!!!");
                }
                else
                {
                    qDebug("Error : %s", szError);
                }
                StopTrancodeFile();
                nState = ACTION_UPLOAD_FAILED;
                DeleteCoursewareFile(m_data.m_szFilePath);
                nError = COURSEWARE_ERR_TRANSMEDIO;
                break;
            }
        }
        break;

    case COURSEWARE_PDF:
    case COURSEWARE_IMG:
        {
            if (!CopyFileToMD5Path(m_data.m_szFilePath, m_data.m_szSourcePath))
            {
                nState = ACTION_UPLOAD_FAILED;
                break;
            }
            m_data.m_nState = COURSEWARE_STATE_UPLOAD;
            m_data.m_nCoursewareType = COURSEWARE_TYPE_OTHER;
            m_data.m_nFileSize = GetCoursewareFileSize(m_data.m_szFilePath);

            if (UploadFile(m_data.m_szName, m_data.m_szFilePath, szToken))
            {
                nState = ACTION_UPLOAD_BENGIN;
                break;
            }
            nState = ACTION_UPLOAD_FAILED;
            nError = COURSEWARE_ERR_UPLOADFAILED;
            break;
        }
        break;
    }
    if (ACTION_UPLOAD_FAILED == nState)
    {
        m_data.m_nState = COURSEWARE_STATE_FAILED;
        
        m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, nState, (void*)(&m_data), nError);
    }

    return true;
}

bool CoursewareTask::UploadFile(LPCWSTR pszFileName, LPCWSTR pszFilePath, const char * pszToken)
{
    if (NULL == pszFileName || NULL == pszFileName[0] ||
        NULL == pszFilePath || NULL == pszFilePath[0] ||
        NULL == pszToken || NULL == pszToken[0])
    {
        return false;
    }

    QString strName = QString::fromWCharArray(pszFileName);
    char szForm[MAX_PATH] = {0};
    wchar_t szUrl[MAX_URL_LENGTH] = {0};
    char szName[MAX_PATH+1] = {0};
    
    strcpy_s(szName, strName.toUtf8());
    sprintf(szForm, "{\"uid\":\"%I64d\",\"t\":\"%s\",\"old\":\"%s\",\"POS\":\"2\"}", ClassSeeion::GetInst()->_nUserId, pszToken, szName);
    GetHttpUploadUrl(szUrl, MAX_URL_LENGTH-1);
    
    int nRes = -1;
    char url[MAX_URL_LENGTH+1] = {0};
    char filepath[MAX_PATH+1] = {0};
    Util::UnicodeToAnsi(szUrl, wcslen(szUrl), url, MAX_URL_LENGTH);
    Util::UnicodeToAnsi(pszFilePath, wcslen(pszFilePath), filepath, MAX_PATH);

    nRes = CHttpSessionMgr::GetInstance()->HttpUploadFile(url, filepath, szForm, HTTP_OUTTIME, 0, (LPVOID)NULL, (LPVOID)this);
    if (nRes >= 0)
    {
        m_data.m_nHttpID = nRes;
        wcscpy(m_data.m_szUploadUrl, szUrl);
        return true;
    }
    m_data.m_nState = COURSEWARE_STATE_FAILED;
    
    //m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, ACTION_UPLOAD_FAILED, (void*)(&m_data), COURSEWARE_ERR_UPLOADFAILED);
    return false;
}

void CoursewareTask::TransMediaOverCallBack(EventInfoType enmuType,void* dwUser,unsigned char* pBuf,unsigned int nBufSize,void* Param)
{
    if (enmuType != TRANCODE_FILE)
    {
        return;
    }
       
    CoursewareTask* pThis = (CoursewareTask*)dwUser;
    TrancodeFileParam *pTran = (TrancodeFileParam*)Param;
    if (pTran == NULL || NULL == pThis)
    {
        return;
    }

    if(pThis->isStop())
    {
        return;
    }

    if (pTran->nCurTrancodePos ==  pTran->nTrancodeLen )
    {
        pThis->TransMediaOver();
    }
    else
    {	//当前进度
        int nPos = 0;
        if(pTran->nTrancodeLen)
        {
            nPos = pTran->nCurTrancodePos * 100 / pTran->nTrancodeLen;
        }

        //设置当前进度
        pThis->SetPos(nPos);
    }
}

bool CoursewareTask::SetPos(int nPerc)
{
	if(nPerc <= 0)
	{
		return false;
	}
	
	switch (m_data.m_nState)
    {
    case COURSEWARE_STATE_TRANS:
        {
            int nPos = nPerc*5/10;
            if (nPos > m_data.m_nDealPerc + 5)
            {
                m_data.m_nDealPerc = nPerc*5/10;
            }
            else
            {
                return true;
            }
        }
        
		break;

    case COURSEWARE_STATE_UPLOAD:
        {
            switch (m_data.m_nCoursewareType)
            {
            case COURSEWARE_TYPE_TRANSMEDIA:
                {
                    m_data.m_nDealPerc = 50 + nPerc*5/10;
                }
                break;

            case COURSEWARE_TYPE_TRANSPDF:
            case COURSEWARE_TYPE_OTHER:
                {
                    m_data.m_nDealPerc = nPerc;
                }
                break;

            case COURSEWARE_TYPE_TRANSONSERVER:
                {
                    m_data.m_nDealPerc = nPerc*5/10;
                }
            }
        }
        break;

    case COURSEWARE_STATE_DOWN:
        {
            nPerc = m_data.m_nBreakPDown + (nPerc * (100-m_data.m_nBreakPDown)/100);
            if (ClassSeeion::GetInst()->IsTeacher())
            {
                switch (m_data.m_nCoursewareType)
                {
                case COURSEWARE_TYPE_OTHER:
                case COURSEWARE_TYPE_SKYFILE:
                    m_data.m_nDealPerc = nPerc;
                    break;
                case COURSEWARE_TYPE_TRANSONSERVER:
                    m_data.m_nDealPerc = 50 + nPerc * 5 / 10;
                    break;
                }
            }
            else
            {
                m_data.m_nDealPerc = nPerc;
            }
        }
        break;
    }
    
    m_TaskProFunc(m_pTaskUser, m_data.m_nCoursewareID, m_data.m_nDealPerc, 0, NULL);
    
    return true;
}

bool CoursewareTask::TransMediaOver()
{
    m_data.m_nState = COURSEWARE_STATE_UPLOAD;
    m_data.m_nFileSize = GetCoursewareFileSize(m_data.m_szFilePath);

    int nError = 0;
    char szToken[MAX_PATH] = {0};
    if (!GetUploadToken(szToken))
    {
        //获取token失败
        
        m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, ACTION_UPLOAD_FAILED, (void*)(&m_data), COURSEWARE_ERR_TOKEN);
        return false;
    }
    int nState = ACTION_OTHER;
    if (!UploadFile(m_data.m_szName, m_data.m_szFilePath, szToken))
    {
        m_data.m_nState = COURSEWARE_STATE_FAILED;
        nState = ACTION_UPLOAD_FAILED;
        nError = COURSEWARE_ERR_UPLOADFAILED;
    }
    else
    {
        nState = ACTION_UPLOAD_BENGIN;
    }
   
    m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, nState, (void*)(&m_data), nError);
    return true;
}

void CoursewareTask::HttpEventBengin(unsigned int nHttpID, int nState)
{
    if (m_data.m_nHttpID != nHttpID)
    {
        return;
    }
   
    switch (nState)
    {
    case HTTP_SESSION_DOWNLOAD:
        m_data.m_nState = COURSEWARE_STATE_DOWN;
        m_TaskFunc(TASK_EVENT_TYPE_DOWN, m_pTaskUser, ACTION_DOWN_BENGIN, (void*)(&m_data), COURSEWARE_ERR_OK);
        break;

    case HTTP_SESSION_UPLOAD:
        m_data.m_nState = COURSEWARE_STATE_UPLOAD;
        m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, ACTION_UPLOAD_BENGIN, (void*)(&m_data), COURSEWARE_ERR_OK);
        break;
    }

    if (COURSEWARE_VIDEO == m_data.m_nFileType || COURSEWARE_AUDIO == m_data.m_nFileType)
    {
        StopTrancodeFile();
        SetEventCallback(NULL, NULL);
    }
}

void CoursewareTask::HttpEventProgress(unsigned int nHttpID, int nPos, bool bisFirst /* = false */, long long nFileSize /* = 0 */)
{
    if (m_data.m_nHttpID != nHttpID)
    {
        return;
    }

    if (bisFirst)
    {
        m_data.m_nFileSize = nFileSize;
        m_TaskProFunc(m_pTaskUser, m_data.m_nCoursewareID, 0, m_data.m_nFileSize, NULL);
    }
	else
	{
		SetPos(nPos);
	}
}

void CoursewareTask::HttpEventEnd(LPVOID pHttpData)
{
    LPHTTPSESSION pHttp = (LPHTTPSESSION)pHttpData;
    if (NULL == pHttp)
    {
        return;
    }

    if (m_data.m_nHttpID != pHttp->id)
    {
        return;
    }
    int state = ACTION_OTHER;
    int nError = 0;

    switch (pHttp->type)
    {
    case HTTP_SESSION_UPLOAD:
        {
            string strKey;
            string strValue;
            int nCode;
            Json::Reader jReader;
            Json::Value jValue;
            jReader.parse(pHttp->response, jValue);
            Json::Value::Members jKeys = jValue.getMemberNames();
            for (Json::Value::Members::iterator iter = jKeys.begin(); iter != jKeys.end(); ++iter)
            {
                strKey = *iter;
                if (0 == strKey.compare("code"))
                {
                    nCode = jValue[strKey.c_str()].asInt();
                }
                if (0 == strKey.compare("url"))
                {
                    strValue = jValue[strKey.c_str()].asString();
                    Util::URLDecode(strValue.c_str(), strValue);
                }
            }

            //若response里code为40101，则代表upload token过期
            if (40101 == nCode)
            {
                m_data.m_nState = ACTION_UPLOAD_FAILED;
                m_data.m_nDealPerc = 0;
                state = ACTION_UPLOAD_FAILED;
                nError = COURSEWARE_ERR_TOKEN;
                break;
            }

            //若返回的code != 200 则代表上传失败
            if (pHttp->rcode != 200 || strValue == "")
            {
                m_data.m_nState = ACTION_UPLOAD_FAILED;
                m_data.m_nDealPerc = 0;

                state = ACTION_UPLOAD_FAILED;
                nError = COURSEWARE_ERR_UPLOADFAILED;
                break;
            }

            if (COURSEWARE_TYPE_TRANSONSERVER == m_data.m_nCoursewareType)
            {
                m_data.m_nDealPerc = 50;
            }
            else
            {
                m_data.m_nDealPerc = 100;
            }
            
            biz::SLCursewaveListOpt scwOpt;
            QString strTemp = QString("%1:%2").arg(strValue.c_str()).arg(QString::fromWCharArray(m_data.m_szName));
            wcscpy(m_data.m_szNetFileName, wstring((wchar_t*)(strTemp).unicode()).data());
            scwOpt._nType = GetCoursewareFileType(m_data.m_szName);
            scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
            scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
            scwOpt._nOpt = biz::ECursewaveOpt_ADD;
            QString strFile = QString::fromStdWString(m_data.m_szNetFileName);
            wcscpy(scwOpt._szFileName, reinterpret_cast<const wchar_t *>(strFile.utf16()));
            if (COURSEWARE_TRANS_FAILED != m_data.m_nTrans)
            {
                m_data.m_nState = COURSEWARE_STATE_DOWN;
                //SetPos(100);
                m_data.m_nState = COURSEWARE_STATE_OK;
                biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);
            }
            else
            {
                m_data.m_nState = COURSEWARE_STATE_DOWN;
            }
            state = ACTION_UPLOAD_END;
        }
        break;

    case HTTP_SESSION_DOWNLOAD:
        {
            if (200 != pHttp->rcode && 206 != pHttp->rcode)
            {
                if (416 == pHttp->rcode)   //range not satisfiable
                {
                    state = ACTION_DOWN_FAILED;
                }
                if (m_data.m_nDownTime < HTTP_DOWNFILE_REPEAT)
                {
                    //ExecDown();
                    state = ACTION_DOWN_BENGIN;
                    m_data.m_nBreakPDown = m_data.m_nDealPerc;

                    if(!CoursewareTaskMgr::isValidate())
                    {
                        return;
                    }

                    m_TaskFunc(TASK_EVENT_TYPE_AGAIN, m_pTaskUser, 0, (void*)(&m_data), 0);
                    return;
                }
                m_data.m_nState = COURSEWARE_STATE_FAILED;
                state = ACTION_DOWN_FAILED;
                nError = COURSEWARE_ERR_DOWNFAILED;
                break;
            }

            m_data.m_nState = COURSEWARE_STATE_OK;
            m_data.m_nDownState = CW_FILE_DOWN_END;

            wchar_t szFile[MAX_PATH] = {0};
            wcsncpy(szFile, m_data.m_szFilePath, wcslen(m_data.m_szFilePath)-4);
            RenameFile(m_data.m_szFilePath, szFile);
            wcscpy(m_data.m_szFilePath, szFile);
            state = ACTION_DOWN_END;
        }
        break;
    }

    if(!CoursewareTaskMgr::isValidate())
    {
        return;
    }

    if (HTTP_SESSION_DOWNLOAD == pHttp->type)
    {
        m_TaskFunc(TASK_EVENT_TYPE_DOWN, m_pTaskUser, state, (void*)(&m_data), nError);
    }
    else if (HTTP_SESSION_UPLOAD == pHttp->type)
    {
        m_TaskFunc(TASK_EVENT_TYPE_UPLOAD, m_pTaskUser, state, (void*)(&m_data), nError);
    }
}

void CoursewareTask::SetTaskCallBack(scTaskCallBack pTaskFunc, scTaskProCallBack pTaskProFunc, void* lpTaskUser)
{
    m_TaskFunc = pTaskFunc;
    m_TaskProFunc = pTaskProFunc;
    m_pTaskUser = lpTaskUser;
}

void CoursewareTask::stopHttpSession()
{
	LPHTTPSESSION pHttpSession = CHttpSessionMgr::GetInstance()->GetHttpSession(m_data.m_nHttpID);
	if(NULL == pHttpSession)
	{
		return;
	}

	pHttpSession->bCancle = true;
	pHttpSession->lpCbFunc = NULL;
}