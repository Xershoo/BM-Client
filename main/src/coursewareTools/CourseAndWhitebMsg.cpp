//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：CourseAndWhitebMsg.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.25
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#include <QString>
#include "CourseAndWhitebMsg.h"
#include "CL8_EnumDefs.h"
#include "./session/classsession.h"
#include "../whiteboard/WhileBoardDataMgr.h"
#include "../Courseware/CourseWareTaskMgr.h"

QCourseAndWhitebMsg * QCourseAndWhitebMsg::m_instance = NULL;

QCourseAndWhitebMsg* QCourseAndWhitebMsg::getInstance()
{
    if (NULL == m_instance)
    {
        m_instance = new QCourseAndWhitebMsg();
    }
    return m_instance;
}

void QCourseAndWhitebMsg::freeInstance()
{
    if (NULL == m_instance)
    {
        return;
    }

    delete m_instance;
    m_instance = NULL;
}

QCourseAndWhitebMsg::QCourseAndWhitebMsg()
{
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
		connect(pCallback, SIGNAL(WhiteBoardEvent(biz::SLWhiteBoardEvent)),this,SLOT(doWhiteBoardEvent(biz::SLWhiteBoardEvent)));
        connect(pCallback, SIGNAL(WhiteBoardOpt(biz::SLWhiteBoardOpt)), this, SLOT(doWhiteBoardOpt(biz::SLWhiteBoardOpt)));
        connect(pCallback, SIGNAL(CursewaveListEvent(biz::SLCursewaveListOpt)), this, SLOT(doCursewaveListEvent(biz::SLCursewaveListOpt)));
        connect(pCallback, SIGNAL(ShowTypeChanged(biz::SLClassRoomShowInfo)), this, SLOT(doShowTypeChanged(biz::SLClassRoomShowInfo)));
    }
}

void QCourseAndWhitebMsg::doCursewaveListEvent(biz::SLCursewaveListOpt info)
{
    if (ClassSeeion::GetInst()->_nClassRoomId != info._nRoomId ||
        NULL == info._szFileName || NULL == info._szFileName[0])
    {
        return;
    }

    if (!CoursewareTaskMgr::isValidate())
    {
        return;
    }

    switch (info._nOpt)
    {
    case biz::ECursewaveOpt_ADD:
        {
            qDebug("Receive network messages(courseware events) : Add Courseware(%s)", QString::fromWCharArray(info._szFileName));
            int nError = 0;
            if (!CoursewareTaskMgr::getInstance()->AddCourseByNet(info._szFileName, &nError))
            {
                wchar_t szError[MAX_PATH] = {0};
                GetErrorInfo(szError, nError);
                qDebug("Add Courseware By Net : error(%d - %s) file(%s)", nError, QString::fromWCharArray(szError), QString::fromWCharArray(info._szFileName));
            }
        }
        break;

    case biz::ECursewaveOpt_CLOSE:
        {
            qDebug("Receive network messages(courseware events) : Close Courseware(%s)", QString::fromWCharArray(info._szFileName));
            PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(info._szFileName);
            if (pData && !ClassSeeion::GetInst()->IsTeacher())
            {
                emit courseware_event(ECursewaveOpt_CLOSE, QString::fromWCharArray(pData->m_szName));
                delete pData;
                pData = NULL;
            }
        }
        break;

    case biz::ECursewaveOpt_DEL:
        {
            qDebug("Receive network messages(courseware events) : Delete Courseware(%s)", QString::fromWCharArray(info._szFileName));
            PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(info._szFileName);
            if (pData && !ClassSeeion::GetInst()->IsTeacher())
            {
                emit courseware_event(ECursewaveOpt_DEL, QString::fromWCharArray(pData->m_szName));
                delete pData;
                pData = NULL;
            }
        }
        break;
    }
}

void QCourseAndWhitebMsg::doShowTypeChanged(biz::SLClassRoomShowInfo info)
{
    biz::IClassRoomInfo* pInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(info._nRoomId);
    if(NULL == pInfo)
    {
        return;
    }
    
    biz::SLClassRoomBaseInfo sBaseInfo = pInfo->GetRoomBaseInfo();
    if(sBaseInfo._nClassState != biz::EClassroomstate_Doing)
    {
        return;
    }
    if (biz::eShowType_Cursewave == info._nShowType)
    {
        WCHAR wszFileName[MAX_PATH] = {0};
        WCHAR wszFileType[10] = {0};
        WCHAR wszMD5[MAX_PATH] = {0};
        WCHAR *pwszType = NULL;
        wcscpy_s(wszFileName, info._szShowName);
        pwszType = (WCHAR*)wcschr(wszFileName, '.');
        if (pwszType)
        {
            wcsncpy(wszFileType, pwszType, wcslen(pwszType));
            wcsncpy(wszMD5, wszFileName, wcslen(wszFileName) - wcslen(pwszType));
        }
        GetCoursewareTransFile(wszMD5, wszFileName, wszFileType, MAX_PATH);

        PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByFilePathEx(wszFileName);
        if (pData)
        {
            if (CW_FILE_DOWNING == pData->m_nDownState || CW_FILE_WAIT_DOWN == pData->m_nDownState)
            {
                emit courseware_ctrl(QString::fromWCharArray(pData->m_szName),  -1, NULL);
            }
            else if (COURSEWARE_STATE_OK == pData->m_nState && CW_FILE_DOWN_END == pData->m_nDownState)
            {
                emit courseware_ctrl(QString::fromWCharArray(pData->m_szName), info._nShowPage, QString::fromWCharArray(info._szShowName));
            }
            pData->m_bISOpen = true;
            delete pData;
            pData = NULL;
        }
    }
    else if (biz::eShowType_Whiteboard == info._nShowType && info._nShowPage > 0)
    {
        LPWHITEBOARDDATA pData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(info._nShowPage);
        if (pData)
        {
            WhiteBoardDataMgr::getInstance()->OpenWhiteboardByID(info._nShowPage);
        }
        else
        {
            QString wbname = QString::fromWCharArray(info._szShowName);
            WhiteBoardDataMgr::getInstance()->AddWhiteboard(info._nShowPage, wbname,false);
        }
        emit whiteboard_ctrl(QString::fromWCharArray(info._szShowName), info._nShowPage);
    }
}

void QCourseAndWhitebMsg::doWhiteBoardOpt(biz::SLWhiteBoardOpt info)
{
    if (NULL == WhiteBoardDataMgr::getInstance())
    {
        return;
    }
    switch (info._nOpt)
    {
    case biz::EBoardOpt_Create:
        {
            qDebug("Receive network messages(whiteboard events) : Add Whiteboard(%s)", QString::fromWCharArray(info._szwbName));
            WhiteBoardDataMgr::getInstance()->AddWhiteboard(info._nPageId, QString::fromWCharArray(info._szwbName),false);
        }
        break;

    case biz::EBoardOpt_Delete:
        {
            qDebug("Receive network messages(whiteboard events) : Delete Whiteboard(%s)", QString::fromWCharArray(info._szwbName));
            emit whiteboard_event(biz::EBoardOpt_Delete, QString::fromWCharArray(info._szwbName));
        }
        break;

    case biz::EBoardOpt_ModifyName:
        {
            qDebug("Receive network messages(whiteboard events) : ModifyName Whiteboard(%s)", QString::fromWCharArray(info._szwbName));
            WhiteBoardDataMgr::getInstance()->RenameWhiteboard(info._nPageId, info._szwbName);
        }
        break;
    }
}

void QCourseAndWhitebMsg::doWhiteBoardEvent(biz::SLWhiteBoardEvent wbEvent)
{
    QWhiteBoardView* pWhiteboard = WhiteBoardDataMgr::getInstance()->AddWhiteboard(wbEvent._nPageId,QString(""),false);
    if (pWhiteboard)
    {
        pWhiteboard->handleWhiteBoardEventBySelf(wbEvent);
    }
}