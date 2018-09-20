//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardDataMgr.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.11
//	说明：
//  修改说明：
//**********************************************************************

#include "WhileBoardDataMgr.h"
#include <QMutexLocker>
#include "./addwhiteboarddialog.h"
#include "c8messagebox.h"
#include "QWhiteBoard.h"
#include "./session/classsession.h"
#include "biz/interface/interface.h"
#include "biz/interface/data.h"
#include "CL8_EnumDefs.h"
#include "session/classsession.h"
#include "biz/biz.h"
#include "classroomdialog.h"
#include "common/macros.h"

WhiteBoardDataMgr* WhiteBoardDataMgr::m_instance = NULL;

WhiteBoardDataMgr* WhiteBoardDataMgr::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new WhiteBoardDataMgr();
    }

    return m_instance;
}

void WhiteBoardDataMgr::freeInstance()
{
    SAFE_DELETE(m_instance);
}

WhiteBoardDataMgr::WhiteBoardDataMgr():m_nBaseID(nWhiteBoardViewWBIdBegin)
{

}

WhiteBoardDataMgr::~WhiteBoardDataMgr()
{
    ClearAll();
}

QWhiteBoardView* WhiteBoardDataMgr::AddWhiteboard(int nId,QString strName,bool sendMsg /* = true */)
{
    if ( nId <= nWhiteBoardViewWBIdBegin)
    {
        return NULL;
    }

    if(strName.isEmpty())
    {
        strName = QString(tr("WhiteboardName")) + QString("%1").arg(nId-nWhiteBoardViewWBIdBegin);
    }

    LPWHITEBOARDDATA pData = GetWhiteboardByID(nId);
    if (NULL != pData)
    {
        return pData->m_pWhiteBoard;
    }

    pData =new WHITEBOARDDATA;
    QWhiteBoardView *pwhiteboard = new QWhiteBoardView;    
	ClassRoomDialog::getInstance()->addMainView(pwhiteboard);
	pData->m_pWhiteBoard = pwhiteboard;

    pwhiteboard->setPageId(nId);
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        pwhiteboard->setEnable(WB_CTRL_EVERY);
    }
    else
    {
		biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);
		if(myInfo._nUserState & biz::eUserState_speak)
		{
			pwhiteboard->setEnable(WB_CTRL_SELF);
		}
		else
		{
			pwhiteboard->setEnable(WB_CTRL_NONE);
		}
    }

    pwhiteboard->setColor(WB_COLOR_WHITE);
    pwhiteboard->setMode(WB_MODE_CURVE);
    pwhiteboard->setTextSize(WB_SIZE_SMALL);
    pwhiteboard->setUserId(ClassSeeion::GetInst()->_nUserId);
    pwhiteboard->setCourseId(ClassSeeion::GetInst()->_nClassRoomId);
    
    if (ClassSeeion::GetInst()->IsTeacher() && sendMsg)
    {
        biz::GetBizInterface()->WhiteBoardOpt(biz::EBoardOpt_Create, ClassSeeion::GetInst()->_nClassRoomId, nId, wstring((wchar_t*)(strName).unicode()).data());
    }

    pData->m_nWhiteboardID = nId;
    wcscpy(pData->m_wszName, wstring((wchar_t*)(strName).unicode()).data());

    m_nBaseID = nId > m_nBaseID ? nId : m_nBaseID;

    QMutexLocker autoLock(&m_mutex);
    m_vecWhiteborad.push_back(pData);
    autoLock.unlock();
    
    OpenWhiteboardByID(nId,false);

    emit add_whiteboard(strName);

    return pwhiteboard;
}

int WhiteBoardDataMgr::getNewId()
{
    m_nBaseID++;
    return m_nBaseID;
}

bool WhiteBoardDataMgr::RenameWhiteboard(int nWhiteboardID, LPCWSTR pwszName)
{
    if (NULL == pwszName || NULL == pwszName[0] || nWhiteboardID >= 0)
    {
        return false;
    }

    LPWHITEBOARDDATA pData = GetWhiteboardByID(nWhiteboardID);
    if (NULL == pData)
    {
        return false;
    }
        
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        biz::GetBizInterface()->WhiteBoardOpt(biz::EBoardOpt_ModifyName, ClassSeeion::GetInst()->_nClassRoomId, nWhiteboardID, pwszName);
    }
    
    emit modifyname_wb(QString::fromWCharArray(pData->m_wszName), QString::fromWCharArray(pwszName));
    wcscpy(pData->m_wszName, pwszName);
    return true;    
}

bool WhiteBoardDataMgr::DelWhiteboardByID(int nWhiteboardID)
{
    if (0 == m_vecWhiteborad.size())
    {
        return false;
    }

    QMutexLocker autoLock(&m_mutex);

    size_t i=0;
    LPWHITEBOARDDATA pData = NULL;

    for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
    {
        pData = m_vecWhiteborad.at(i);
        if(NULL == pData)
        {
            continue;
        }

        if (nWhiteboardID == pData->m_nWhiteboardID)
        {    
            break;
        }

        pData = NULL;
    }

    if(NULL == pData)
    {
        return FALSE;
    }

    m_vecWhiteborad.erase(m_vecWhiteborad.begin()+i);
    
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        biz::GetBizInterface()->WhiteBoardOpt(biz::EBoardOpt_ModifyName, ClassSeeion::GetInst()->_nClassRoomId, pData->m_nWhiteboardID, pData->m_wszName);
    }
        
    SAFE_DELETE(pData->m_pWhiteBoard);
    SAFE_DELETE(pData); 

    return false;
}

bool WhiteBoardDataMgr::DelWhiteboardByName(LPCWSTR pwszName)
{
    if (0 == m_vecWhiteborad.size())
    {
        return false;
    }

    QMutexLocker autoLock(&m_mutex);
    
    size_t i=0;
    LPWHITEBOARDDATA pData = NULL;

    for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
    {
        pData = m_vecWhiteborad.at(i);
        if(NULL == pData)
        {
            continue;
        }

        if (0 == wcscmp(pwszName, pData->m_wszName))
        {    
            break;
        }

        pData = NULL;
    }

    if(NULL == pData)
    {
        return FALSE;
    }

    m_vecWhiteborad.erase(m_vecWhiteborad.begin()+i);

    if (ClassSeeion::GetInst()->IsTeacher())
    {
        biz::GetBizInterface()->WhiteBoardOpt(biz::EBoardOpt_ModifyName, ClassSeeion::GetInst()->_nClassRoomId, pData->m_nWhiteboardID, pwszName);
    }

    SAFE_DELETE(pData->m_pWhiteBoard);
    SAFE_DELETE(pData); 

    return false;
}

bool WhiteBoardDataMgr::CloseWhiteboardByID(int nWhiteboardID)
{
    if (0 == m_vecWhiteborad.size())
    {
        return false;
    }

    LPWHITEBOARDDATA pData = GetWhiteboardByID(nWhiteboardID);
    if (pData)
    {
        pData->m_pWhiteBoard->hide();
        return true;
    }

    return false;
}

bool WhiteBoardDataMgr::CloseWhiteboardByName(LPCWSTR pwszName)
{
    if (NULL == pwszName || 0 == m_vecWhiteborad.size())
    {
        return false;
    }

    LPWHITEBOARDDATA pData = GetWhiteboardByName(pwszName);
    if (pData)
    {
        pData->m_pWhiteBoard->hide();
        return true;
    }
    return false;
}

bool WhiteBoardDataMgr::OpenWhiteboardByID(int nWhiteboardID,bool sendMsg /* = true */)
{
    LPWHITEBOARDDATA pData = GetWhiteboardByID(nWhiteboardID);
    if(NULL == pData)
    {
        return false;
    }

    if (pData)
    {
        if(sendMsg)
        {
            emit mainshowchanged(nWhiteboardID, biz::eShowType_Whiteboard,0);
        }
        
        //打开白板
        pData->m_pWhiteBoard->show();
        return true;
    }

    return false;
}

bool WhiteBoardDataMgr::OpenWhiteboardByName(LPCWSTR pwszName)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return false;
    }

    LPWHITEBOARDDATA pData = GetWhiteboardByName(pwszName);
    if (pData)
    {
        //打开白板
        emit mainshowchanged(pData->m_nWhiteboardID, biz::eShowType_Whiteboard,0);
        pData->m_pWhiteBoard->show();
        return true;
    }

    return false;
}

LPWHITEBOARDDATA WhiteBoardDataMgr::GetWhiteboardByID(int nWhiteboardID)
{
    if (0 == m_vecWhiteborad.size())
    {
        return NULL;
    }

    QMutexLocker autoLock(&m_mutex);
    for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
    {
        if (nWhiteboardID == m_vecWhiteborad[i]->m_nWhiteboardID)
        {
            return m_vecWhiteborad[i];
        }
    }
    
    return NULL;
}

LPWHITEBOARDDATA WhiteBoardDataMgr::GetWhiteboardByName(LPCWSTR pwszName)
{
    if (0 == m_vecWhiteborad.size() || NULL == pwszName || NULL == pwszName[0])
    {
        return NULL;
    }

    QMutexLocker autoLock(&m_mutex);
    for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
    {
        if (0 == wcscmp(pwszName, m_vecWhiteborad[i]->m_wszName))
        {   
            return m_vecWhiteborad[i];
        }
    }
    
    return NULL;
}

void WhiteBoardDataMgr::ClearAll()
{
    if (0 == m_vecWhiteborad.size())
    {
        return;
    }

    QMutexLocker autoLock(&m_mutex);
    for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
    {
        LPWHITEBOARDDATA pData;
        pData = m_vecWhiteborad[i];
       
        SAFE_DELETE(pData->m_pWhiteBoard);
        SAFE_DELETE(pData);
    }

    m_vecWhiteborad.clear();    
}

void WhiteBoardDataMgr::SetColor(LPCWSTR pwszName, WBColor clr)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->setColor(clr);
    }
}

void WhiteBoardDataMgr::SetTextSize(LPCWSTR pwszName, WBSize tSize)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->setTextSize(tSize);
    }
}

void WhiteBoardDataMgr::SetEnable(LPCWSTR pwszName, WBCtrl enable)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->setEnable(enable);
    }
}

void WhiteBoardDataMgr::SetAllEnable(WBCtrl enable)
{
	if (0 == m_vecWhiteborad.size())
	{
		return ;
	}

	QMutexLocker autoLock(&m_mutex);
	for (size_t i = 0; i < m_vecWhiteborad.size(); i++)
	{
		LPWHITEBOARDDATA pWb = m_vecWhiteborad.at(i);
		if (pWb)
		{
			pWb->m_pWhiteBoard->setEnable(enable);
		}
	}
}


void WhiteBoardDataMgr::SetMode(LPCWSTR pwszName, WBMode mode)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->setMode(mode);
    }
}

void WhiteBoardDataMgr::UnDo(LPCWSTR pwszName)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->undo();
    }
}

void WhiteBoardDataMgr::Clear(LPCWSTR pwszName)
{
    if (NULL == pwszName || NULL == pwszName[0])
    {
        return;
    }

    LPWHITEBOARDDATA pWb = GetWhiteboardByName(pwszName);
    if (pWb)
    {
        pWb->m_pWhiteBoard->clear();
    }
}