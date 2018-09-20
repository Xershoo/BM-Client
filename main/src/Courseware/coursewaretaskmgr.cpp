//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaredata.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：课件管理
//	修改记录：
//  备注：
//**********************************************************************

#include "coursewaretaskmgr.h"
#include <QMutexLocker>
#include <QString>
#include <stdio.h>
#include <fstream>
#include <QVariant>
#include "common/Config.h"
#include "./../token/UploadTokenMgr.h"
#include "courseware.h"
#include "coursewaretask.h"
#include "./session/classsession.h"
#include "././common/Util.h"
#include "HttpSessionMgr.h"
#include "common/macros.h"


unsigned int CoursewareTaskMgr::m_nCoursewareID = 0;
CoursewareTaskMgr* CoursewareTaskMgr::m_instance = NULL;

CoursewareTaskMgr* CoursewareTaskMgr::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new CoursewareTaskMgr(NULL);
    }

    return m_instance;
}

void CoursewareTaskMgr::freeInstance()
{
    SAFE_DELETE(m_instance);
}

bool CoursewareTaskMgr::isValidate()
{
    return m_instance ? true : false;
}

CoursewareTaskMgr::CoursewareTaskMgr(QObject *parent)
    : QObject(parent)
{
    m_nMgrMax = 1;
    m_nTimerID = 0;

    InitData();
}

CoursewareTaskMgr::~CoursewareTaskMgr()
{
    UninitData();
}

void CoursewareTaskMgr::InitData()
{
    StartTimerTask();
}

void CoursewareTaskMgr::UninitData()
{
    EndTimerTask();
    ClearAll();
}

bool CoursewareTaskMgr::StartTimerTask()
{
    if (0 == m_nTimerID)
    {
        m_nTimerID = this->startTimer(COURSEWARE_TASK_TIMER);
    }
    return true;
}

bool CoursewareTaskMgr::EndTimerTask()
{
    if (m_nTimerID > 0)
    {
        this->killTimer(m_nTimerID);
        m_nTimerID = 0;
    }
    return true;
}

bool CoursewareTaskMgr::AddCourseBySource(LPCWSTR pszSourceFile, int *nError)
{
    if (NULL == pszSourceFile)
    {
        return false;
    }

    QMutexLocker AutoLock(&m_DataMutex);
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcscmp(pszSourceFile, m_vecCoursewareData[i]->m_szSourcePath))
        {
            //文件已存在列表中            
            *nError = COURSEWARE_ERR_REPEAT;
            return false;
        }
    }

    PCOURSEWAREDATA pData = new COURSEWAREDATA;

    pData->m_nFileType = GetCoursewareFileType(pszSourceFile);
    wcscpy_s(pData->m_szSourcePath, pszSourceFile);
    LPWSTR pName = NULL;
    pName = (LPWSTR)(wcsrchr(pszSourceFile,L'/'));
    if (NULL == pName)
    {
        pName = (LPWSTR)pszSourceFile;
    }
    else
    {
        pName++;
    }
    wcscpy_s(pData->m_szName, pName);
    
    if (NULL != GetCoursewareByName(pName))
    {
        //文件已存在列表中
        delete pData;
       
        *nError = COURSEWARE_ERR_REPEAT;
        return false;
    }

    m_nCoursewareID++;
    pData->m_nCoursewareID = m_nCoursewareID;

    LPWSTR pszType = NULL;
    pszType = (LPWSTR)(wcsrchr(pData->m_szName, L'.'));
    wchar_t szType[10] = {0};
    wcsncpy(szType, pszType, wcslen(pszType));

    QString strFile = QString::fromWCharArray(pData->m_szSourcePath);
    QString strMd5;
    Util::CaleFileMd5(strFile, strMd5);
    wchar_t szMd5[MAX_PATH] = {0};
    wcscpy(szMd5, wstring((wchar_t*)(strMd5).unicode()).data());
    switch (pData->m_nFileType)
    {
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
    case COURSEWARE_DOC:
        {
            if (Config::getConfig()->m_tranCourseWareLocal)
            {
                GetCoursewareTransFile(szMd5, pData->m_szFilePath, MAX_PATH);
            }
            else
            {
                GetCoursewareTransFile(szMd5, pData->m_szFilePath, szType, MAX_PATH);
            }
        }
        break;

    case COURSEWARE_VIDEO:
    case COURSEWARE_AUDIO:
        {
            GetCoursewareTransFileMedia(szMd5, pData->m_szFilePath, szType, MAX_PATH);
        }
        break;

    default:
        {
            GetCoursewareTransFile(szMd5, pData->m_szFilePath, szType, MAX_PATH);
        }
        break;
    }

    m_vecCoursewareData.push_back(pData);

    if (m_nMgrMax > m_vecMgrTask.size())
    {
		addTask(pData,STATE_TASK_UPLOAD);
    }
    else
    {
        int nID = pData->m_nCoursewareID;
        m_vecWaitUploadTask.push_back(nID);
    }
    
    AutoLock.unlock();

    emit add_courseware(QString::fromWCharArray(pData->m_szName), pData->m_nFileSize, 0);
	
    *nError = COURSEWARE_ERR_OK;
    return true;
}

bool CoursewareTaskMgr::AddCourseByNet(LPCWSTR pszNetFile, int *nError)
{
    if (NULL == pszNetFile)
    {
        return false;
    }

    QMutexLocker AutoLock(&m_DataMutex);

    for (size_t i = 0;i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcscmp(pszNetFile, m_vecCoursewareData[i]->m_szNetFileName))
        {   
            if (COURSEWARE_TYPE_TRANSONSERVER == m_vecCoursewareData[i]->m_nCoursewareType)
            {
                break;
            }
            AutoLock.unlock();
            *nError = COURSEWARE_ERR_REPEAT;

            emit add_courseware(QString::fromWCharArray(m_vecCoursewareData[i]->m_szName),
                m_vecCoursewareData[i]->m_nFileSize, 0);

            return false;
        }
    }

    PCOURSEWAREDATA pData = new COURSEWAREDATA;

    wchar_t szDownUrl[MAX_URL_LENGTH] = {0};
    wchar_t szUrl[MAX_PATH] = {0};
    wchar_t szName[MAX_PATH] = {0};
    wchar_t szTransName[MAX_PATH] = {0};
    wchar_t szFilePath[MAX_PATH] = {0};
    wchar_t szType[10] = {0};
    
    if (!GetFileNameByNetUrl(pszNetFile, szUrl, szName))
    {
        //net url不正确
        AutoLock.unlock();
        *nError = COURSEWARE_ERR_URL;
        return false;
    }
    GetFileTransName(szUrl, szTransName);

    LPWSTR pszTemp = (LPWSTR)(wcsrchr(szName, L'.'));
    if (NULL != pszTemp)
    {
        wcscpy(szType, pszTemp);
    }
    int nType = GetCoursewareFileType(szName);
    if (COURSEWARE_PPT == nType || COURSEWARE_EXCLE == nType || COURSEWARE_DOC == nType)
    {
        wcscpy(szType, L".pdf");
    }
    else if (COURSEWARE_VIDEO == nType)
    {
        wcscpy(szType, L".mp4");
    }
    GetHttpDownUrl(szDownUrl, MAX_URL_LENGTH-1);
    wcscat_s(szDownUrl, L"/");
    wcscat_s(szDownUrl, szUrl);
    GetCoursewareTransFile(szTransName, szFilePath, szType, MAX_PATH);

    wcscpy(pData->m_szName, szName);
    wcscpy(pData->m_szDownUrl, szDownUrl);
    wcscpy(pData->m_szFilePath, szFilePath);
    wcscpy(pData->m_szNetFileName, pszNetFile);
    pData->m_nFileSize = GetCoursewareFileSize(szFilePath);
    pData->m_nState = COURSEWARE_STATE_DOWN;
    pData->m_nFileType = nType;

    bool bISHave = false;
    int  nCID = 0;
    PCOURSEWAREDATA pTemp = GetCoursewareByName(szName);
    if (pTemp)
    {
        if (IsExistFile(szFilePath))
        {
            WCHAR szName[MAX_PATH] = {0};
            wcscpy(szName, pData->m_szName);
            //SetPosEvent(pTemp->m_nCoursewareID, 100);
            SetPos(pTemp->m_nCoursewareID, 100);
            pTemp->m_nState = COURSEWARE_STATE_OK;
            pTemp->m_nDownState = CW_FILE_DOWN_END;
            wcscpy(pTemp->m_szFilePath, szFilePath);
            wcscpy(pTemp->m_szNetFileName, pszNetFile);
            delete pData;
            pData = NULL;
            AutoLock.unlock();
            *nError = COURSEWARE_ERR_REPEAT;
            emit deal_courseware(QString::fromWCharArray(szName), ACTION_DOWN_END, *nError);
            return true;
        }
        bISHave = true;
        
        //预下载文件，在文件名后+“.tld”
        QString strTemp = QString("%1.tld").arg(QString::fromWCharArray(pData->m_szFilePath));
        wchar_t szDowndFile[MAX_PATH] = {0};
        wcscpy(szDowndFile, wstring((wchar_t*)(strTemp).unicode()).data());
        wcscpy(pTemp->m_szFilePath, szDowndFile);
        wcscpy(pTemp->m_szNetFileName, pData->m_szNetFileName);
        wcscpy(pTemp->m_szDownUrl, pData->m_szDownUrl);
        nCID = pTemp->m_nCoursewareID;
    }
    if (bISHave)
    {
        delete pData;
        pData = NULL;
        pData = pTemp;
    }
    else
    {
        m_nCoursewareID++;
        pData->m_nCoursewareID = m_nCoursewareID;
        nCID = m_nCoursewareID;
        wchar_t szDowndFile[MAX_PATH] = {0};
        QString strTemp = QString("%1.tld").arg(QString::fromWCharArray(pData->m_szFilePath));
        wcscpy(szDowndFile, wstring((wchar_t*)(strTemp).unicode()).data());
        wcscpy(pData->m_szFilePath, szDowndFile);
        pData->m_nCoursewareType = COURSEWARE_TYPE_OTHER;        
        m_vecCoursewareData.push_back(pData);
    }

    bool bIsFind = false;

	PTASKMGR pTaskInfo = GetTask(nCID);
	if(pTaskInfo)
	{
		((CoursewareTask *)pTaskInfo->m_pData)->StartDownTask();
		pTaskInfo->m_nStateTask = STATE_TASK_ALL;
		bIsFind = true;
	}
    
    if (!bIsFind)
    {
        if (m_nMgrMax > m_vecMgrTask.size())
        {
			addTask(pData,STATE_TASK_DOWN);            
        }
        else
        {
            int nID = nCID;
            m_vecWaitDownTask.push_back(nID);
        }
    }

    AutoLock.unlock();
    emit add_courseware(QString::fromWCharArray(pData->m_szName), pData->m_nFileSize, 0);
    
    *nError = COURSEWARE_ERR_OK;
    return true;
}

bool CoursewareTaskMgr::AddCourseBySky(LPCWSTR pszFileName, LPCWSTR pszFileUrl, int *nError)
{
    if (NULL == pszFileName || NULL == pszFileName[0] || NULL == pszFileUrl || NULL == pszFileUrl[0])
    {
        return false;
    }

    QMutexLocker AutoLock(&m_DataMutex);
    PCOURSEWAREDATA pTemp = GetCoursewareByName(pszFileName);
    if (pTemp)
    {
        //文件已存在列表中
        AutoLock.unlock();
        *nError = COURSEWARE_ERR_REPEAT;
        return false;
    }

    PCOURSEWAREDATA pData = new COURSEWAREDATA;

    wcscpy(pData->m_szName, pszFileName);

    m_nCoursewareID++;
    pData->m_nCoursewareID = m_nCoursewareID;
    pData->m_nFileType = GetCoursewareFileType(pszFileName);

    wchar_t szDownUrl[MAX_URL_LENGTH] = {0};
    wchar_t szTransName[MAX_PATH] = {0};
    wchar_t szType[5] = {0};

    GetHttpDownUrl(szDownUrl, MAX_URL_LENGTH-1);
    wcscat_s(szDownUrl, L"/");
    wcscat_s(szDownUrl, pszFileUrl);
    wcscpy(pData->m_szDownUrl, szDownUrl);

    GetFileTransName(pszFileUrl, szTransName);
    wchar_t *pszTemp = (wchar_t*)(wcsrchr(pszFileUrl, L'.'));
    if (NULL != pszTemp)
    {
        wcsncpy(szType, pszTemp, wcslen(pszTemp));
    }
    GetCoursewareTransFile(szTransName, pData->m_szFilePath, szType, MAX_PATH);
    
    if (wcslen(pData->m_szFilePath) <= 4)
    {
        delete pData;
        pData = NULL;
        AutoLock.unlock();
        *nError = COURSEWARE_ERR_PATH;
        return false;
    }
    memset(szTransName, 0, sizeof(szTransName));
    memset(szDownUrl, 0, sizeof(szDownUrl));

    QString strTemp = QString("%1.tdl").arg(QString::fromWCharArray(pData->m_szFilePath));
    wcscpy(szTransName, wstring((wchar_t*)(strTemp).unicode()).data());
    pData->m_nFileSize = GetCoursewareFileSize(szTransName);
    wcscpy(pData->m_szFilePath, szTransName);
    
    strTemp = QString("%1:%2").arg(QString::fromWCharArray(pszFileUrl)).arg(QString::fromWCharArray(pszFileName));
    wcscpy(pData->m_szNetFileName, wstring((wchar_t*)(strTemp).unicode()).data());
    pData->m_nCoursewareType = COURSEWARE_TYPE_SKYFILE;

    m_vecCoursewareData.push_back(pData);
    
    bool bIsFind = false;
	PTASKMGR pTaskInfo = GetTask(pData->m_nCoursewareID);
	if(pTaskInfo)
	{
		((CoursewareTask *)pTaskInfo->m_pData)->StartDownTask();		
		bIsFind = true;
	}

    if (!bIsFind)
    {
        if (m_nMgrMax > m_vecMgrTask.size())
        {
			addTask(pData,STATE_TASK_DOWN);            
        }
        else
        {
            int nID = pData->m_nCoursewareID;
            m_vecWaitDownTask.push_back(nID);
        }
    }

    biz::SLCursewaveListOpt scwOpt;
    scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
    scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
    scwOpt._nOpt = biz::ECursewaveOpt_ADD;
    scwOpt._nType = GetCoursewareFileType(pszFileName);
    QString strUrl = QString::fromWCharArray(szDownUrl);
    wcscpy(scwOpt._szFileName, reinterpret_cast<const wchar_t *>(strTemp.utf16()));
    biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);

    AutoLock.unlock();
    emit add_courseware(QString::fromWCharArray(pData->m_szName), pData->m_nFileSize, 0);
    *nError = COURSEWARE_ERR_OK;
    return true;
}

bool CoursewareTaskMgr::DeleteCourseByName(LPCWSTR pszName)
{
    if (NULL == pszName || 0 == m_vecCoursewareData.size())
    {
        return false;
    }

    int nCoursewareID = 0;
    WCHAR szName[MAX_PATH] = {0};

	DeleteTask(nCoursewareID);

    QMutexLocker AutoLock(&m_DataMutex);
    wcscpy(szName, pszName);

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcscmp(pszName, m_vecCoursewareData[i]->m_szName))
        {
            PCOURSEWAREDATA pData = m_vecCoursewareData[i];
            if (pData->m_bISOpen)
            {
                CloseCourseware(i, false);
            }
            
			m_vecCoursewareData.erase(m_vecCoursewareData.begin()+i);
            nCoursewareID = pData->m_nCoursewareID;
            
			delete pData;
            pData = NULL;
            
            break;
        }
    }   
    
    for (size_t i = 0; i < m_vecWaitDownTask.size(); i++)
    {
        if (nCoursewareID == m_vecWaitDownTask[i])
        {
            m_vecWaitDownTask.erase(m_vecWaitDownTask.begin()+i);
            break;
        }
    }

    for (size_t i = 0; i < m_vecWaitUploadTask.size(); i++)
    {
        if (nCoursewareID == m_vecWaitUploadTask[i])
        {
            m_vecWaitUploadTask.erase(m_vecWaitUploadTask.begin()+i);
            break;
        }
    }

    AutoLock.unlock();
    
	emit del_courseware(QString::fromWCharArray(szName));
    return false;
}

bool CoursewareTaskMgr::OpenCourseware(LPCWSTR pszFilePath)
{
    if (NULL == pszFilePath || 0 == m_vecCoursewareData.size())
    {
        return false;
    }

    QMutexLocker AutoLock(&m_DataMutex);

    PCOURSEWAREDATA pData = GetCoursewareByFilePath(pszFilePath);
    if (NULL == pData)
    {
        return false;
    }
        
    pData->m_bISOpen = true;
    
    AutoLock.unlock();

    emit mainshowchanged(pData->m_nCoursewareID, biz::eShowType_Cursewave,0);        
    return true;
    
}

bool CoursewareTaskMgr::CloseCourseware(LPCWSTR pszFileName,bool sendMsg /* = true */)
{
    if (NULL == pszFileName || 0 == m_vecCoursewareData.size())
    {
        return false;
    }

    PCOURSEWAREDATA pData = GetCoursewareByName(pszFileName);
    if (pData)
    {
        pData->m_bISOpen = false;
        
		DeleteTask(pData->m_nCoursewareID);
        
		if (ClassSeeion::GetInst()->IsTeacher() && sendMsg)
        {
            biz::SLCursewaveListOpt scwOpt;
            scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
            scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
            scwOpt._nOpt = biz::ECursewaveOpt_CLOSE;
            wchar_t* pszFileName = NULL;

            pszFileName = (wchar_t*)(wcsrchr(pData->m_szFilePath, '\\'));
            if (pszFileName)
            {
                pszFileName++;
            }
            QString str = QString::fromWCharArray(pData->m_szNetFileName);
            wcscpy(scwOpt._szFileName, reinterpret_cast<const wchar_t *>(str.utf16()));
            biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);
        }

		return true;
    }
    return false;
}

bool CoursewareTaskMgr::CloseCourseware(int nIndex,bool sendMsg /* = true */)
{
	if (nIndex < 0 || nIndex >= m_vecCoursewareData.size())
	{
		return false;
	}

	PCOURSEWAREDATA pData = m_vecCoursewareData.at(nIndex);
	if(NULL == pData)
	{
		return false;
	}

	pData->m_bISOpen = false;
	
	DeleteTask(pData->m_nCoursewareID);
	
	if (ClassSeeion::GetInst()->IsTeacher() && sendMsg)
	{
		biz::SLCursewaveListOpt scwOpt;
		scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
		scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
		scwOpt._nOpt = biz::ECursewaveOpt_CLOSE;
		wchar_t* pszFileName = NULL;

		pszFileName = (wchar_t*)(wcsrchr(pData->m_szFilePath, '\\'));
		if (pszFileName)
		{
			pszFileName++;
		}
		QString str = QString::fromWCharArray(pData->m_szNetFileName);
		wcscpy(scwOpt._szFileName, reinterpret_cast<const wchar_t *>(str.utf16()));
		biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);
	}

	return true;
}

void CoursewareTaskMgr::SetFileOpen(int nCoursewareID, bool bISOpen)
{
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (m_vecCoursewareData[i]->m_nCoursewareID == nCoursewareID)
        {
            m_vecCoursewareData[i]->m_bISOpen = bISOpen;
            break;
        }
    }
    
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByID(int nCoursewareID)
{
    if (0 == m_vecCoursewareData.size())
    {
        return NULL;
    }
    
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (nCoursewareID == m_vecCoursewareData[i]->m_nCoursewareID)
        {
            return m_vecCoursewareData[i];
        }
    }
    return NULL;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByIdEx(int nCoursewareID)
{
    QMutexLocker AutoLock(&m_DataMutex);
    if (0 == m_vecCoursewareData.size())
    {
        return NULL;
    }

    PCOURSEWAREDATA pData = NULL;
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (nCoursewareID == m_vecCoursewareData[i]->m_nCoursewareID)
        {
            pData = new COURSEWAREDATA();
            memcpy(pData ,m_vecCoursewareData.at(i),sizeof(COURSEWAREDATA));
            break;
        }
    }
    return pData;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByHttp(int nHttpID)
{
    if (0 == m_vecCoursewareData.size())
    {
        return NULL;
    }

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (nHttpID == m_vecCoursewareData[i]->m_nHttpID)
        {
            return m_vecCoursewareData[i];
        }
    }
    return NULL;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByName(LPCWSTR pszName)
{
    if (NULL == pszName || 0 == m_vecCoursewareData.size())
    {
        return NULL;
    }
        
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcsicmp(pszName, m_vecCoursewareData[i]->m_szName))
        {
            return m_vecCoursewareData[i];
        }
    }
    return NULL;
}


PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByNameEx(LPCWSTR pszName)
{
    QMutexLocker autoLock(&m_DataMutex);

    if (NULL == pszName || 0 == m_vecCoursewareData.size())
    {
        return NULL;
    }

    PCOURSEWAREDATA pData = NULL;
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcsicmp(pszName, m_vecCoursewareData[i]->m_szName))
        {
            pData = new COURSEWAREDATA();
            memcpy(pData ,m_vecCoursewareData.at(i),sizeof(COURSEWAREDATA));
            break;
        }
    }
    return pData;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByFilePath(LPCWSTR pszFilePath)
{
    if (NULL == pszFilePath || 0 == m_vecCoursewareData.size())
    {
        return NULL;
    }

    wchar_t szPath[MAX_PATH] = {0};
    QString strTemp = QString("%1.tld").arg(QString::fromWCharArray(pszFilePath));
    wcscpy(szPath, wstring((wchar_t*)(strTemp).unicode()).data());
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcsicmp(pszFilePath, m_vecCoursewareData[i]->m_szFilePath) ||
            0 == wcsicmp(szPath, m_vecCoursewareData[i]->m_szFilePath))
        {
            return m_vecCoursewareData[i];
        }
    }
    return NULL;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByFilePathEx(LPCWSTR pszFilePath)
{
    QMutexLocker autoLock(&m_DataMutex);
    if (NULL == pszFilePath || 0 == m_vecCoursewareData.size())
    {
        return NULL;
    }

    PCOURSEWAREDATA pData = NULL;
    wchar_t szPath[MAX_PATH] = {0};
    QString strTemp = QString("%1.tld").arg(QString::fromWCharArray(pszFilePath));
    wcscpy(szPath, wstring((wchar_t*)(strTemp).unicode()).data());
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcsicmp(pszFilePath, m_vecCoursewareData[i]->m_szFilePath) ||
            0 == wcsicmp(szPath, m_vecCoursewareData[i]->m_szFilePath))
        {
            pData = new COURSEWAREDATA();
            memcpy(pData ,m_vecCoursewareData.at(i),sizeof(COURSEWAREDATA));
            break;
        }
    }
    return pData;
}

PCOURSEWAREDATA CoursewareTaskMgr::GetCoursewareByNetUrlEx(LPCWSTR pszNetUrl)
{
    QMutexLocker autoLock(&m_DataMutex);
    if (NULL == pszNetUrl || NULL == pszNetUrl[0])
    {
        return NULL;
    }
    
    PCOURSEWAREDATA pData = NULL;

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (0 == wcscmp(pszNetUrl, m_vecCoursewareData[i]->m_szNetFileName))
        {
            pData = new COURSEWAREDATA();
            memcpy(pData ,m_vecCoursewareData.at(i),sizeof(COURSEWAREDATA));
            break;
        }
    }
    return pData;
}

void CoursewareTaskMgr::ClearAll()
{
    QMutexLocker AutoLock(&m_DataMutex);
    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (m_vecCoursewareData[i]->m_bISOpen)
        {
            CloseCourseware(i,false);
        }
    }

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        PCOURSEWAREDATA pData = m_vecCoursewareData[i];
        SAFE_DELETE(pData);
    }

    m_vecCoursewareData.clear();
    m_vecWaitDownTask.clear();
    m_vecWaitUploadTask.clear();
    
    AutoLock.unlock();

	clearAllTask();
}

int CoursewareTaskMgr::GetCoursewareCount()
{
    return m_vecCoursewareData.size();
}

int CoursewareTaskMgr::GetWaitDownFileSize()
{
    return 0;
}

void CoursewareTaskMgr::SetAllNotOpen()
{
    if (0 == m_vecCoursewareData.size())
    {
        return;
    }

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        m_vecCoursewareData[i]->m_bISOpen = false;
    }
}

bool CoursewareTaskMgr::GetFileNameByNetUrl(LPCWSTR pszOldUrl, LPWSTR pszNewUrl, LPWSTR pszName)
{
    if (NULL == pszOldUrl || NULL == pszOldUrl[0])
    {
        return false;
    }

    wchar_t pszUrl[MAX_PATH] = {0};
    wcscpy(pszUrl, pszOldUrl);

    wcstok(pszUrl, L":");
    if (0 == _wcsicmp(pszOldUrl, pszUrl) && wcslen(pszOldUrl) == wcslen(pszUrl))
    {
        wcsncpy(pszName, pszUrl, wcslen(pszUrl));
        return false;
    }
    wcsncpy(pszName, pszOldUrl +wcslen(pszUrl)+1, wcslen(pszOldUrl)-wcslen(pszUrl) - 1);
    wcsncpy(pszNewUrl, pszOldUrl, wcslen(pszOldUrl) - wcslen(pszName)-1);
    return true;
}

bool CoursewareTaskMgr::GetFileTransName(LPCWSTR pszFilePath, LPWSTR pszTransName)
{
    if (NULL == pszFilePath || NULL == pszFilePath[0])
    {
        return false;
    }

    wchar_t szFile[MAX_PATH] = {0};
    wcscpy(szFile, pszFilePath);
    wcstok(szFile, L"/");
    wchar_t *pszTemp = NULL;
    wchar_t szName[MAX_PATH] = {0};
    wcsncpy(szName, szFile, wcslen(szFile));
    while(pszTemp = wcstok(NULL, L"/"))
    {
        wcsncpy(szName, pszTemp, wcslen(pszTemp));
    }
    wcstok(szName, L".");
    wcsncpy(pszTransName, szName, wcslen(szName));
    return true;
}

bool CoursewareTaskMgr::DeleteTask(int nCoursewareID,bool stopTask /* =true*/)
{
	QMutexLocker autoLock(&m_taskMutex);
    for (size_t i = 0; i < m_vecMgrTask.size(); i++)
    {
        PTASKMGR pTaskInfo = m_vecMgrTask.at(i);
        if(NULL == pTaskInfo || nCoursewareID != pTaskInfo->m_nCoursewareID)        
        {
            continue;
        }
        
        m_vecMgrTask.erase(m_vecMgrTask.begin()+i);
        DeleteTask(pTaskInfo,stopTask);

        return true;        
    }

    return false;
}

bool CoursewareTaskMgr::DeleteTask(PTASKMGR pTaskInfo,bool stopTask /* = true */)
{
    if(NULL == pTaskInfo)
    {
        return false;
    }

    CoursewareTask* pTask = static_cast<CoursewareTask*>(pTaskInfo->m_pData);
    if(pTask && stopTask)
    {
        if (STATE_TASK_ALL == pTaskInfo->m_nStateTask || STATE_TASK_DOWN == pTaskInfo->m_nStateTask)
        {
            pTask->StopDownTask();
        }
        else if (STATE_TASK_ALL == pTaskInfo->m_nStateTask || STATE_TASK_UPLOAD == pTaskInfo->m_nStateTask)
        {
            pTask->StopUploadTask();
        }
    }

    SAFE_DELETE(pTaskInfo);
    return true;
}

PTASKMGR CoursewareTaskMgr::GetTask(int nCoursewareID)
{
	QMutexLocker autoLock(&m_taskMutex);
    if (0 == m_vecMgrTask.size())
    {
        return NULL;
    }
    for (size_t i = 0; i < m_vecMgrTask.size(); i++)
    {
        if (nCoursewareID == m_vecMgrTask[i]->m_nCoursewareID)
        {
            return m_vecMgrTask[i];
        }
    }

    return NULL;
}

void CoursewareTaskMgr::timerEvent(QTimerEvent *event)
{
    if (0 == m_nTimerID)
    {
        return;
    }
    if (m_nTimerID == event->timerId())
    {
        if (m_vecMgrTask.size() < m_nMgrMax)
        {
            doUploadTimer();
            doDownTimer();
        }
    }
}

bool CoursewareTaskMgr::doUploadTimer()
{
    if (m_vecWaitUploadTask.size() <= 0)
    {
		return false;
	}
	
	QMutexLocker AutoLock(&m_DataMutex);
	
	for (size_t i = 0; i < m_vecWaitUploadTask.size(); i++)
	{
		int nCoursewareID = m_vecWaitUploadTask[i];
		PCOURSEWAREDATA pData = GetCoursewareByID(nCoursewareID);

		addTask(pData,STATE_TASK_UPLOAD);

		m_vecWaitUploadTask.erase(m_vecWaitUploadTask.begin()+i);
		
		return true;
	}	

    return false;
}

bool CoursewareTaskMgr::doDownTimer()
{
    if (m_vecWaitDownTask.size() <= 0)
    {
		return false;
	}
	QMutexLocker AutoLock(&m_DataMutex);
	for (size_t i = 0; i < m_vecWaitDownTask.size(); i++)
	{
		int nCoursewareID = m_vecWaitDownTask[i];
		PTASKMGR pTask = GetTask(nCoursewareID);
		if (pTask)
		{
			((CoursewareTask *)(pTask->m_pData))->StartDownTask();
			pTask->m_nStateTask = STATE_TASK_ALL;
		}
		else
		{
			PCOURSEWAREDATA pData = GetCoursewareByID(nCoursewareID);
			addTask(pData,STATE_TASK_DOWN);                
		}

		m_vecWaitDownTask.erase(m_vecWaitDownTask.begin()+i);

		return true;
	}
	
	return false;
}

void CoursewareTaskMgr::TaskEventCallBack(int nEventType, void *lpUser, int nState, void *Param, int nError)
{
    CoursewareTaskMgr *pThis = (CoursewareTaskMgr*)lpUser;
    COURSEWAREDATA    *pData = (COURSEWAREDATA*)Param;
    if (NULL == pThis || NULL == pData)
    {
        return;
    }

    PCOURSEWAREDATA pTemp = pThis->GetCoursewareByID(pData->m_nCoursewareID);
    if (pTemp)
    {
        *pTemp = *pData;
    }

    switch (nEventType)
    {
    case TASK_EVENT_TYPE_UPLOAD:
        {
            pThis->UploadEvent(pTemp->m_nCoursewareID, nState, nError);
        }
        break;

    case TASK_EVENT_TYPE_DOWN:
        {
            pThis->DownEvent(pTemp->m_nCoursewareID, nState, nError);
        }
        break;

    case TASK_EVENT_TYPE_AGAIN:
        {
            pThis->AgainEvent(pTemp->m_nCoursewareID);
        }
        break;
        
    default:
        break;
    }
}

void CoursewareTaskMgr::TaskProCallBack(void *lpUser, unsigned int nCoursewareID, int nPerc, long long nFileSize, void *Param)
{
    CoursewareTaskMgr *pThis = (CoursewareTaskMgr*)lpUser;
    if (NULL == pThis)
    {
        return;
    }

    if (nFileSize > 0)
    {
        pThis->SetFilesize(nCoursewareID, nFileSize);
    }
    else
    {
        pThis->SetPos(nCoursewareID, nPerc);
    }
}


void CoursewareTaskMgr::UploadEvent(int nCoursewareID, int nState, int nError)
{
    PCOURSEWAREDATA pData = GetCoursewareByID(nCoursewareID);
    if (NULL==pData)
    {
		return;
	}

	if(ACTION_UPLOAD_END == nState || ACTION_UPLOAD_FAILED == nState)
    {
		QString file = QString::fromWCharArray(pData->m_szName);
		DeleteTask(pData->m_nCoursewareID);        
        
        if(ACTION_UPLOAD_FAILED == nState)
        {
            DeleteData(nCoursewareID);
        }

        if (pData->m_nFileType == COURSEWARE_AUDIO || 
            pData->m_nFileType == COURSEWARE_VIDEO || 
            pData->m_nFileType == COURSEWARE_IMG)
        {
            emit set_pos(file, nState == ACTION_UPLOAD_END ? 100 : -1);
        }

		emit deal_courseware(file, nState, nError);
		return;
	}
}

void CoursewareTaskMgr::DownEvent(int nCoursewareID, int nState, int nError)
{
	PCOURSEWAREDATA pData = GetCoursewareByID(nCoursewareID);
	if (NULL==pData)
	{
		return;
	}

	if(ACTION_DOWN_END == nState || ACTION_DOWN_FAILED == nState)
	{
		QString file = QString::fromWCharArray(pData->m_szName);
		DeleteTask(pData->m_nCoursewareID);
        
        if(ACTION_DOWN_FAILED == nState)
        {
            DeleteData(nCoursewareID);
        }

		emit set_pos(file, nState == ACTION_DOWN_END ? 100 : -1);
		emit deal_courseware(file, nState, nError);

		return;
	}
   
}

void CoursewareTaskMgr::AgainEvent(int nCoursewareID)
{
    PCOURSEWAREDATA pTemp = GetCoursewareByID(nCoursewareID);
    if (pTemp)
    {
		m_vecWaitDownTask.push_back(pTemp->m_nCoursewareID);
        DeleteTask(pTemp->m_nCoursewareID);        
    }
}

void CoursewareTaskMgr::SetPos(int nCoursewareID, int nPrec)
{
    PCOURSEWAREDATA pTemp = GetCoursewareByID(nCoursewareID);
    if (pTemp && nPrec > 0 && nPrec > pTemp->m_nDealPerc)
    {
        pTemp->m_nDealPerc = nPrec;
        emit set_pos(QString::fromWCharArray(pTemp->m_szName), nPrec);
    }
}

void CoursewareTaskMgr::SetFilesize(int nCoursewareID, long long nFileSize)
{
    PCOURSEWAREDATA pTemp = GetCoursewareByID(nCoursewareID);
    if (pTemp && nFileSize > 0)
    {
        pTemp->m_nFileSize = nFileSize;
        emit set_filesize(QString::fromWCharArray(pTemp->m_szName), nFileSize);
    }
}

void CoursewareTaskMgr::clearAllTask()
{
	QMutexLocker autoLock(&m_taskMutex);
	for (auto it = m_vecMgrTask.begin(); it != m_vecMgrTask.end(); it++)
	{
		PTASKMGR pTaskInfo = *it;
		DeleteTask(pTaskInfo);
	}

	m_vecMgrTask.clear();
}

void CoursewareTaskMgr::addTask(PCOURSEWAREDATA pData,int state)
{
	if(NULL == pData)
	{
		return;
	}

	PTASKMGR pTask = new TASKMGR;
	if(NULL == pTask)
	{
		return;
	}

	CoursewareTask *task = new CoursewareTask(pData);
	if(NULL == task)
	{
		SAFE_DELETE(pTask);
		return;
	}

	QMutexLocker autoLock(&m_taskMutex);

	pTask->m_nCoursewareID = pData->m_nCoursewareID;	
	pTask->m_pData = (void *)task;
	pTask->m_nStateTask = state;
	
	m_vecMgrTask.push_back(pTask);

	task->SetTaskCallBack(TaskEventCallBack, TaskProCallBack, (void*)this);
	if(state == STATE_TASK_DOWN)
	{
		task->StartDownTask();
	}
	else
	{
		task->StartUploadTask();
	}
}

void CoursewareTaskMgr::DeleteData(int nCoursewareID)
{
    QMutexLocker AutoLock(&m_DataMutex);
    if (0 == m_vecCoursewareData.size())
    {
        return;
    }

    for (size_t i = 0; i < m_vecCoursewareData.size(); i++)
    {
        if (nCoursewareID == m_vecCoursewareData[i]->m_nCoursewareID)
        {
            PCOURSEWAREDATA pData = m_vecCoursewareData[i];
            if (pData->m_bISOpen)
            {
                CloseCourseware(i, false);
            }

            m_vecCoursewareData.erase(m_vecCoursewareData.begin()+i);
            
            delete pData;
            pData = NULL;

            break;
        }
    }

    return;
}