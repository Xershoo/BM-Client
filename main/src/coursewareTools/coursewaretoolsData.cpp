//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：coursewaretoolsData.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.29
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#include "coursewaretoolsData.h"
#include "CourseAndWhitebMsg.h"
#include "session/classsession.h"
#include "common/Util.h"
#include "common/macros.h"
#include "coursewareTools.h"
#include "../whiteboard/WhileBoardDataMgr.h"
#include "../Courseware/CoursewareTaskMgr.h"
#include "../Courseware/CoursewarePanel.h"

CoursewareToolData::CoursewareToolData(QcoursewareTools* tools,QObject *parent /* = 0 */) : QObject(parent)
	,m_tools(tools)
{
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(deal_courseware(QString, int, int)), this, SLOT(dealCoursewareState(QString, int, int)));

    connect(WhiteBoardDataMgr::getInstance(), SIGNAL(mainshowchanged(int, int, int)), this, SLOT(MainShowChanged(int, int, int)));
    connect(WhiteBoardDataMgr::getInstance(), SIGNAL(modifyname_wb(QString, QString)), this, SLOT(doWhiteboardModifyName(QString, QString)));
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(mainshowchanged(int, int, int)), this, SLOT(MainShowChanged(int, int, int)));

    connect(QCourseAndWhitebMsg::getInstance(), SIGNAL(whiteboard_event(int, QString)), this, SLOT(doWhiteboardEvent(int, QString)));
    connect(QCourseAndWhitebMsg::getInstance(), SIGNAL(courseware_event(int, QString)), this, SLOT(doCoursewareEvent(int, QString)));
    connect(QCourseAndWhitebMsg::getInstance(), SIGNAL(courseware_ctrl(QString, int, QString)), this, SLOT(doCoursewareCtrl(QString, int, QString)));
    connect(QCourseAndWhitebMsg::getInstance(), SIGNAL(whiteboard_ctrl(QString, int)), this, SLOT(doWhiteboardCtrl(QString, int)));
}

CoursewareToolData::~CoursewareToolData()
{
    Util::releaseArray(m_vecWBAndCourseView);
    QCourseAndWhitebMsg::freeInstance();
}

void CoursewareToolData::doWhiteboardEvent(int nType, QString wbName)
{
    if (wbName.isEmpty())
    {
        return;
    }
    switch (nType)
    {
    case biz::EBoardOpt_Delete:
        {
            deleteCoursewareORWb(wbName);
			if(m_tools)
			{
				m_tools->DeleteListItem(wbName);
			}
          
        }
        break;
    }
}

void CoursewareToolData::doCoursewareCtrl(QString filename, int nPage, QString md5name)
{
    if (filename.isEmpty())
    {
        return;
    }

    if (nPage >= 0 && (CoursewareDataMgr::GetInstance()->m_NowFileName == filename))
    {
        CoursewareDataMgr::GetInstance()->m_CoursewarePanel->SetCoursewareShow(biz::eShowType_Cursewave, md5name, nPage);
        return;
    }

    if (m_tools)
    {
        m_tools->SetShowCoursewareList(filename);
    }
    else
    {
        CoursewareDataMgr::GetInstance()->setNowFileName(filename);
    }
        
    openCoursewareORWb(filename);

    if (nPage > 0)
    {
        CoursewareDataMgr::GetInstance()->GotoPage(CTRL_SEEK_PAGE, nPage);
    }
}

void CoursewareToolData::doWhiteboardCtrl(QString filename, int nID)
{
    CoursewareDataMgr::GetInstance()->setNowFileName(filename);
	if(m_tools)
	{
		m_tools->SetShowCoursewareList(filename);
        if (!ClassSeeion::GetInst()->IsTeacher())
        {
            openCoursewareORWb(filename);
        }
	}
}

void CoursewareToolData::doWhiteboardModifyName(QString oldName, QString newName)
{
    if (oldName.isEmpty() || newName.isEmpty()
        )
    {
        return;
    }

	if(m_tools)
	{
		m_tools->ModifyItemName(oldName, newName);
	}

    for (size_t i = 0; i < m_vecWBAndCourseView.size(); i++)
    {
        if (m_vecWBAndCourseView[i]->m_name == oldName)
        {
            m_vecWBAndCourseView[i]->m_name = newName;
            break;
        }
    }
}

void CoursewareToolData::doCoursewareEvent(int nType, QString fileName)
{
    if (fileName.isEmpty())
    {
        return;
    }
    switch (nType)
    {
    case biz::ECursewaveOpt_CLOSE:
        {
            closeCourseware(fileName);            
        }
        break;

    case biz::ECursewaveOpt_DEL:
        {
            deleteCoursewareORWb(fileName);            
        }
        break;
	default:
		break;
    }

	if(m_tools)
	{
		m_tools->DeleteListItem(fileName);
	}
}

void CoursewareToolData::MainShowChanged(int nID, int nType, int nState)
{
    if (nID <= 0)
    {
        return;
    }

    switch (nType)
    {
	case eShowType_Blank:
	case eShowType_NotBegin:
        {
			if(m_tools)
			{
				m_tools->ShowMainState("", MAIN_SHOW_TYPE_NO);
			}
        }
        break;

    case eShowType_Cursewave:
        {
            showCourseware(nID, nState);
        }
        break;

    case eShowType_Whiteboard:
        {
            showWhiteBoard(nID, nState);
        }
        break;
    }
}

bool CoursewareToolData::deleteCoursewareORWb(QString fileName)
{
    if (fileName.isEmpty())
    {
        return false; 
    }

    int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
    if (COURSEWARE_UNKNOWN == nType)
    {   
        
        WhiteBoardDataMgr::getInstance()->DelWhiteboardByName(wstring((wchar_t*)(fileName).unicode()).data());
        CoursewareDataMgr::GetInstance()->setNowFileName("");

        return true;
    }
    
    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
    if (NULL == pData)
    {
        pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(wstring((wchar_t*)(fileName).unicode()).data());
    }

    if (NULL == pData)
    {
        return false;
    }
   
    QString fielpath = QString::fromWCharArray(pData->m_szName);
    CoursewareDataMgr::GetInstance()->m_CoursewarePanel->CloseCoursewareFile(QString::fromWCharArray(pData->m_szFilePath));
    CoursewareTaskMgr::getInstance()->DeleteCourseByName(pData->m_szName);
    CoursewareDataMgr::GetInstance()->setNowFileName("");

    if (ClassSeeion::GetInst()->IsTeacher())
    {
        biz::SLCursewaveListOpt scwOpt;
        scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
        scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
        scwOpt._nOpt = biz::ECursewaveOpt_DEL;
        wcscpy(scwOpt._szFileName, pData->m_szNetFileName);
        biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);
    }
    
    delete pData;
    pData = NULL;

    return true;
}

bool CoursewareToolData::closeCourseware(QString fileName)
{
	if (fileName.isEmpty())
	{
		return false; 
	}

	int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
	if (COURSEWARE_UNKNOWN == nType)
	{
		return false;
	}
	
	PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
	if (NULL == pData)
	{
		pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(wstring((wchar_t*)(fileName).unicode()).data());
	}
	
	if (NULL == pData)
	{
		return false;
	}
	
	QString fielpath = QString::fromWCharArray(pData->m_szName);
	CoursewareDataMgr::GetInstance()->m_CoursewarePanel->CloseCoursewareFile(QString::fromWCharArray(pData->m_szFilePath),false);
	CoursewareTaskMgr::getInstance()->CloseCourseware(pData->m_szName);
	CoursewareDataMgr::GetInstance()->setNowFileName("");

    SAFE_DELETE(pData);

	return true;
}

void CoursewareToolData::dealCoursewareState(QString fileName, int nState, int nError)
{
    if (fileName.isEmpty())
    {
        return;
    }
    switch (nState)
    {
    case ACTION_PROGRESS:
        {

        }
        break;

    case ACTION_UPLOAD_BENGIN:
        {
            uploadEventBengin(fileName, nError);
        }
        break;

    case ACTION_UPLOAD_END:
        {
            uploadEventEnd(fileName, nError);
        }
        break;

    case ACTION_UPLOAD_FAILED:
        {
            uploadEventFailed(fileName, nError);
        }
        break;

    case ACTION_DOWN_BENGIN:
        {
            downEventBengin(fileName, nError);
        }
        break;

    case ACTION_DOWN_END:
        {
            downEventEnd(fileName, nError);
        }
        break;

    case ACTION_DOWN_FAILED:
        {
            downEventFailed(fileName, nError);            
        }
        break;

    default:
        break;
    }
}


bool CoursewareToolData::uploadEventBengin(QString fileName, int nError)
{
    qDebug("deal courseware events: (%s) bengin upload", fileName);

    return true;
}

bool CoursewareToolData::uploadEventEnd(QString fileName, int nError)
{
    qDebug("deal courseware events: (%s) end upload", fileName);
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return false;
    }
		
    if(m_tools)
    {
        m_tools->SetDownState(fileName, false);
    }


    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
    if (NULL == pData)
    {
        return true;
    }
    
    if (fileName == CoursewareDataMgr::GetInstance()->m_NowFileName)
    {
        if (pData->m_nCoursewareType == COURSEWARE_IMG ||
            pData->m_nCoursewareType == COURSEWARE_AUDIO ||
            pData->m_nCoursewareType == COURSEWARE_VIDEO)
        {
            CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
            openCoursewareORWb(fileName,true);
        }

        /*
        if (pData->m_nFileType != COURSEWARE_VIDEO && pData->m_nFileType != COURSEWARE_AUDIO)
        {
            CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
            openCoursewareORWb(fileName);
        }
        */
    }
    
    SAFE_DELETE(pData);

    return true;
}

bool CoursewareToolData::uploadEventFailed(QString fileName, int nError)
{
    wchar_t szError[MAX_PATH] = {0};
    GetErrorInfo(szError, nError);
    qDebug("deal courseware events: error(%d -- %s) file(%s)", nError, szError, fileName);

    if (ClassSeeion::GetInst()->IsTeacher())
    {
		if(m_tools)
		{
			m_tools->SetDownState(fileName, false);
		}
    }
    else
    {

    }

	if(m_tools)
    {
        int showState = nError == COURSEWARE_ERR_TRANSMEDIO?MAIN_SHOW_TYPE_TRANS_FAILED:MAIN_SHOW_TYPE_UPLOAD_FAILED;
		m_tools->ShowMainState(fileName, showState);
	}

    return true;
}

bool CoursewareToolData::downEventBengin(QString fileName, int nError)
{
    qDebug("deal courseware events: (%s) begin down", fileName);

    return true;
}

bool CoursewareToolData::downEventEnd(QString fileName, int nError)
{
    qDebug("deal courseware events: (%s) end down", fileName);
    
    if(m_tools)
    {
        m_tools->SetDownState(fileName, false);
    }

    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
    if (NULL == pData)
    {
        return false;
    }
    
    if (fileName == CoursewareDataMgr::GetInstance()->m_NowFileName)
    {
        CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
        openCoursewareORWb(fileName,true);

        /*
        if (pData->m_nFileType != COURSEWARE_VIDEO && pData->m_nFileType != COURSEWARE_AUDIO)
        {
            CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
            openCoursewareORWb(fileName);
        }
        */
    }
    
    SAFE_DELETE(pData);
	
    return true;
}

bool CoursewareToolData::downEventFailed(QString fileName, int nError)
{
    wchar_t szError[MAX_PATH] = {0};
    GetErrorInfo(szError, nError);
    qDebug("deal courseware events: error(%d -- %s) file(%s)", nError, szError, fileName);
    
	if (ClassSeeion::GetInst()->IsTeacher())
    {
		if(m_tools)
		{
			m_tools->SetDownState(fileName, false);
		}
    }
    else
    {

    }
	if(m_tools)
	{
		m_tools->ShowMainState(fileName, MAIN_SHOW_TYPE_DOWN_FALIED);
	}
    
    return true;
}

bool CoursewareToolData::setTextPage(QString fileName, int nPage)
{
    LPWBAndCourseView pView = GetViewWidget(fileName);
	if (NULL==pView||nPage<=0)
	{
		return false;
	}

    pView->m_npage = nPage;

	if(m_tools)
    {
		m_tools->setTextPage(nPage);
	}
    
    return true;
}

LPWBAndCourseView CoursewareToolData::GetViewWidget(QString ItemName)
{
    if (ItemName.isEmpty())
    {
        return false;
    }

    QMutexLocker AutoLock(&m_mutex);
    for (size_t i = 0; i < m_vecWBAndCourseView.size(); i++)
    {
        if (ItemName == m_vecWBAndCourseView[i]->m_name)
        {
            return m_vecWBAndCourseView[i];
            break;
        }
    }
    AutoLock.unlock();
    return NULL;
}

bool CoursewareToolData::delViewWidget(QString ItemName)
{
    if (ItemName.isEmpty())
    {
        return false;
    }

    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(ItemName).unicode()).data());
    if (pData)
    {
        if (COURSEWARE_PDF == GetCoursewareFileType(pData->m_szFilePath))
        {
            if(m_tools)
			{
				m_tools->DeletePreviewView(QString::fromWCharArray(pData->m_szFilePath));
			}
        }

        SAFE_DELETE(pData);
    }

    QMutexLocker AutoLock(&m_mutex);
    for (size_t i = 0; i < m_vecWBAndCourseView.size(); i++)
    {
        if (ItemName == m_vecWBAndCourseView[i]->m_name)
        {
            LPWBAndCourseView pView = m_vecWBAndCourseView[i];
            m_vecWBAndCourseView.erase(m_vecWBAndCourseView.begin()+i);
            deleteCoursewareORWb(ItemName);
            
            delete pView;
            pView = NULL;
            break;
        }
    }

    AutoLock.unlock();

    return true;
}

bool CoursewareToolData::openCoursewareORWb(QString fileName,bool onlyOpen /* = false */)
{
    if (fileName.isEmpty())
    {
        return  false;
    }

    QString filePath;
    QString lastFile = CoursewareDataMgr::GetInstance()->m_LastFileName;

    int  nType = GetCoursewareFileType(wstring((wchar_t*)(lastFile).unicode()).data());
    if (!lastFile.isEmpty() && (COURSEWARE_VIDEO == nType || COURSEWARE_AUDIO == nType) && (lastFile != fileName))
    {
        PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(lastFile).unicode()).data());
        if (pData)
        {
            CoursewareDataMgr::GetInstance()->m_CoursewarePanel->PauseCoursewareShow(QString::fromWCharArray(pData->m_szFilePath), true);
            SAFE_DELETE(pData);
        }
    }

    nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
    if (COURSEWARE_UNKNOWN == nType)
    {
        WhiteBoardDataMgr::getInstance()->OpenWhiteboardByName(wstring((wchar_t*)(fileName).unicode()).data());
        setTextPage(fileName, 1);
    }
    else
    {
        PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
        if (pData)
        {
            filePath = QString::fromWCharArray(pData->m_szFilePath);
            CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
            if (COURSEWARE_STATE_OK == pData->m_nState)
            {
                if (!ClassSeeion::GetInst()->_bBeginedClass)
                {
					if(m_tools)
					{
						m_tools->ShowMainState(QString::fromWCharArray(pData->m_szName), 100);
					}
                    return true;
                }
				
                if (CoursewareDataMgr::GetInstance()->m_CoursewarePanel->OpenCoursewareFile(QString::fromWCharArray(pData->m_szFilePath), true,onlyOpen))
                {
                    CoursewareTaskMgr::getInstance()->OpenCourseware(pData->m_szFilePath);
                }
				
            }
            else
            {
                //课件未完成加载
                setTextPage(fileName, 1);
                
				if(m_tools)
				{
					m_tools->ShowMainState(fileName, pData->m_nDealPerc);
				}
            }

            SAFE_DELETE(pData);
        }
    }

    LPWBAndCourseView pView = GetViewWidget(fileName);
    if (pView)
    {
		if(m_tools)
		{
			m_tools->ShowcsORwb(pView->m_widget, false);
		}

        bool isMedia = false;
        if (COURSEWARE_AUDIO == GetCoursewareFileType(wstring((wchar_t*)(filePath).unicode()).data()) ||
            COURSEWARE_VIDEO == GetCoursewareFileType(wstring((wchar_t*)(filePath).unicode()).data()))
        {
            isMedia = true;
        }
        
        LPCOURSEWARESHOW pshow = CoursewareDataMgr::GetInstance()->m_CoursewarePanel->GetCoursewareShow();        
        if (pshow )
        {
			int nNowTime = 0;
			int nTotleTime = 0;
			int nState = 0;
                        
            if (isMedia)
            {   
				if(pshow->_show._media)
				{
					nNowTime = pshow->_show._media->getCurPlayTime();
					nTotleTime = pshow->_show._media->getTotalPlayTime();
					nState = pshow->_show._media->getState();

                    if (ClassSeeion::GetInst()->IsTeacher())
                    {
                        biz::SLClassRoomShowInfo sShowInfo;
                        sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
                        sShowInfo._nShowType = biz::eShowType_Cursewave;
                        WCHAR szFile[MAX_PATH+1] = {0};
                        GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pshow->_fileName).unicode()).data(), szFile, MAX_PATH);
                        wcscpy(sShowInfo._szShowName, szFile);
                        sShowInfo._nShowPage = pshow->_show._media->getState();
                        biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
                    }
				}
            }
            
			if(m_tools)
			{
				m_tools->ShowMediaTool(isMedia, fileName, nNowTime, nTotleTime, nState);
			}			
        }
    }
    else
    {
		if(m_tools)
		{
			m_tools->ShowMediaTool(false, fileName, 0, 0, 0);
		}        
    }
    
    return true;
}

bool CoursewareToolData::showCourseware(int nID, int nState)
{
    PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByIdEx(nID);
    LPCOURSEWARESHOW pShow = CoursewareDataMgr::GetInstance()->m_CoursewarePanel->GetCoursewareShow();
    if(NULL == pData || NULL == pShow)
    {
        SAFE_DELETE(pData);
        return false;
    }

    bool bAddNew = false;
    LPWBAndCourseView pView = GetViewWidget(QString::fromWCharArray(pData->m_szName));
    if (NULL == pView)
    {
        bAddNew = true;
        pView = new WBAndCourseView;
        switch (pShow->_type)
        {
        case COURSEWARE_PDF:
            {
                pView->m_widget = pShow->_show._pdf;
				if(m_tools)
                {
					m_tools->ShowCoursePreview(QString::fromWCharArray(pData->m_szFilePath), pView->m_npage-1);
				}
            }
            break;

        case COURSEWARE_VIDEO:
        case COURSEWARE_AUDIO:
            {
                pView->m_widget = pShow->_show._media;
            }
            break;

        case COURSEWARE_IMG:
            {
                pView->m_widget = pShow->_show._image;
            }
            break;
        default:
            delete pView;
            return false;
        }
        pView->m_name = QString::fromWCharArray(pData->m_szName);
        pView->m_npage = 1;

        QMutexLocker AutoLock(&m_mutex);
        m_vecWBAndCourseView.push_back(pView);
        AutoLock.unlock();
    }
		
	if(m_tools)
	{
		m_tools->ShowcsORwb(pView->m_widget, bAddNew);
	}
       
    CoursewareDataMgr::GetInstance()->setNowFileName(pView->m_name);
    SAFE_DELETE(pData);

    return true;
}

bool CoursewareToolData::showWhiteBoard(int nID, int nState)
{
    LPWHITEBOARDDATA pData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(nID);
    if(NULL==pData)
    {
        return false;
    }

    bool bAddNew = false;
    LPWBAndCourseView pView = GetViewWidget(QString::fromWCharArray(pData->m_wszName));
    if (NULL == pView)
    {
        bAddNew = true;
        pView = new WBAndCourseView;
        pView->m_widget = pData->m_pWhiteBoard;
        pView->m_name = QString::fromWCharArray(pData->m_wszName);
        pView->m_npage = 1;
        QMutexLocker AutoLock(&m_mutex);
        m_vecWBAndCourseView.push_back(pView);
        AutoLock.unlock();
    }
    if(m_tools)
    {
        m_tools->ShowcsORwb(pView->m_widget, bAddNew);
    }

    CoursewareDataMgr::GetInstance()->setNowFileName(pView->m_name);

    if (ClassSeeion::GetInst()->IsTeacher())
    {
        biz::SLClassRoomShowInfo sShowInfo;
        sShowInfo._nShowPage = pData->m_nWhiteboardID;
        sShowInfo._nShowType = biz::eShowType_Whiteboard;
        if (NULL != pData->m_wszName)
        {
            wcscpy_s(sShowInfo._szShowName, pData->m_wszName);
        }

        sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
        biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
    }

    return true;
}
