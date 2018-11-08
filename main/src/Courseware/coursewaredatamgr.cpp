//**********************************************************************
//	Copyright （c） 2015-2020. All rights reserved.
//	文件名称：coursewaredatamgr.cpp
//	版本号：1.0
//	作者：潘良
//	时间：2015.12.2
//	说明：
//	修改记录：
//  备注：
//**********************************************************************
#include "coursewaredatamgr.h"
#include "common/macros.h"
#include "CoursewarePanel.h"
#include "./common/Util.h"
#include "./ClassRoomDialog.h"
#include "./session/classsession.h"

CoursewareDataMgr*  CoursewareDataMgr::  m_instance = NULL;
CoursewareDataMgr::CoursewareDataMgr()
{ 
    m_CoursewarePanel = new QCoursewarePannel();
    m_NowFileName = "";
    m_nFontSize = WB_SIZE_SMALL;
    m_nColorType = WB_COLOR_RED;

	connect(CoursewareTaskMgr::getInstance(), SIGNAL(deal_courseware(QString, int, int)), this, SLOT(onCoursewareStateChange(QString, int, int)));
	connect(m_CoursewarePanel, SIGNAL(setMediaPlayerPro(unsigned int,string&)), this, SLOT(onMediaPlayPosChange(unsigned int,string&)));
	connect(m_CoursewarePanel, SIGNAL(setShowPage(int)),this,SLOT(onPDFPageChange(int)));
	connect(WhiteBoardDataMgr::getInstance(), SIGNAL(modifyname_wb(QString, QString)), this, SLOT(onWhiteboardModifyName(QString, QString)));
	connect(CoursewareTaskMgr::getInstance(), SIGNAL(set_pos(QString,int)), this, SLOT(onCoursewareSetPos(QString, int)));

	CBizCallBack * pCallback = ::getBizCallBack();
	if(pCallback)
	{
		connect(pCallback, SIGNAL(WhiteBoardEvent(biz::SLWhiteBoardEvent)),this,SLOT(onWhiteBoardEvent(biz::SLWhiteBoardEvent)));
		connect(pCallback, SIGNAL(WhiteBoardOpt(biz::SLWhiteBoardOpt)), this, SLOT(onWhiteBoardOpt(biz::SLWhiteBoardOpt)));
		connect(pCallback, SIGNAL(CursewaveListEvent(biz::SLCursewaveListOpt)), this, SLOT(onCursewaveListEvent(biz::SLCursewaveListOpt)));
		connect(pCallback, SIGNAL(ShowTypeChanged(biz::SLClassRoomShowInfo)), this, SLOT(onShowTypeChanged(biz::SLClassRoomShowInfo)));
	}
}

CoursewareDataMgr::~CoursewareDataMgr()
{
    ReleaseData();
}

CoursewareDataMgr *CoursewareDataMgr::GetInstance()
{
    if (NULL == m_instance)
    {
        m_instance = new CoursewareDataMgr;
    }
    return m_instance;
}

void CoursewareDataMgr::freeInstance()
{
    SAFE_DELETE(m_instance);
}

bool CoursewareDataMgr::isValid()
{
    return m_instance ? true : false;
}

void CoursewareDataMgr::ReleaseData()
{
    if (m_CoursewarePanel)
    {
        delete m_CoursewarePanel;
        m_CoursewarePanel = NULL;
    }

	Util::releaseArray(m_vecWBAndCourseView);
	disconnect(CoursewareTaskMgr::getInstance(), NULL,this,NULL);
	disconnect(WhiteBoardDataMgr::getInstance(), NULL,this,NULL);
	
	CBizCallBack * pCallback = ::getBizCallBack();
	if(pCallback)
	{
		disconnect(pCallback, NULL,this,NULL);
	}
}

void CoursewareDataMgr::setNowFileName(QString fileName)
{
    m_LastFileName = m_NowFileName;
    m_NowFileName = fileName;
}

void CoursewareDataMgr::SetEnable(WBCtrl enable)
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->SetEnable(m_NowFileName, enable);
        }
        break;

	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->SetEnable(wstring((wchar_t*)(m_NowFileName).unicode()).data(), enable);
        }
        break;
    }
}

void CoursewareDataMgr::SetTextSize(WBSize tSize)
{
    m_nFontSize = tSize;
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->SetTextSize(m_NowFileName, tSize);
        }
        break;
	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->SetTextSize(wstring((wchar_t*)(m_NowFileName).unicode()).data(), tSize);
        }
        break;
    }
}

void CoursewareDataMgr::SetColor(WBColor clr)
{
    m_nColorType = clr;
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->SetColor(m_NowFileName, clr);
        }
        break;
	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->SetColor(wstring((wchar_t*)(m_NowFileName).unicode()).data(), clr);
        }
        break;
    }
}

void CoursewareDataMgr::SetMode(WBMode mode)
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->SetMode(m_NowFileName, mode);
        }
        break;
	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->SetMode(wstring((wchar_t*)(m_NowFileName).unicode()).data(), mode);
        }
        break;
    }
}

void CoursewareDataMgr::UnDo()
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->UnDo(m_NowFileName);
        }
        break;
	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->UnDo(wstring((wchar_t*)(m_NowFileName).unicode()).data());
        }
        break;
    }
}

void CoursewareDataMgr::Clear()
{
    int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    switch (nType)
    {
    case COURSEWARE_PDF:
    case COURSEWARE_DOC:
    case COURSEWARE_EXCLE:
    case COURSEWARE_PPT:
	case COURSEWARE_IMG:
	case COURSEWARE_VIDEO:
	case COURSEWARE_FLASH:
        {
            m_CoursewarePanel->Clear(m_NowFileName);
        }
        break;
	case COURSEWARE_WHITEBOARD:
    case COURSEWARE_UNKNOWN:
        {
            WhiteBoardDataMgr::getInstance()->Clear(wstring((wchar_t*)(m_NowFileName).unicode()).data());
        }
        break;
    }
}

void CoursewareDataMgr::ZoomIn()
{    
	m_CoursewarePanel->ZoomIn(m_NowFileName);    
}

void CoursewareDataMgr::ZoomOut()
{ 
    m_CoursewarePanel->ZoomOut(m_NowFileName);
}

void CoursewareDataMgr::GotoPage(int nType, int nPage)
{
	int cType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
    if (COURSEWARE_UNKNOWN == cType || COURSEWARE_WHITEBOARD == cType)
    {
        return;
    }
    m_CoursewarePanel->GotoPage(m_NowFileName, nType, nPage);
}

int CoursewareDataMgr::GetTotalPage()
{
    if (m_NowFileName.isEmpty() || 0 == m_NowFileName.length())
    {
        return 0;
    }
    return m_CoursewarePanel->GetTotalPage(m_NowFileName);
}

bool CoursewareDataMgr::MediaCtrl(int nCtrl, int nSeek /* = 0 */)
{
    if (m_NowFileName.isEmpty() || 0 == m_NowFileName.length())
    {
        return false;
    }
    return m_CoursewarePanel->MediaCtrl(m_NowFileName, nCtrl, nSeek);
}

unsigned int CoursewareDataMgr::getMediaState()
{
    return m_CoursewarePanel->GetMediaState(m_NowFileName);
}

void CoursewareDataMgr::onWhiteBoardEvent(biz::SLWhiteBoardEvent wbEvent)
{
	LPWHITEBOARDDATA pWhiteboardData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(wbEvent._nPageId);
	if(NULL != pWhiteboardData && NULL != pWhiteboardData->m_pWhiteBoard)
	{
		return;
	}

	QWhiteBoardView * pWhiteboardView =  WhiteBoardDataMgr::getInstance()->AddWhiteboard(wbEvent._nPageId,QString(""),false);
	if(NULL == pWhiteboardView)
	{
		return;
	}
		
	pWhiteboardData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(wbEvent._nPageId);
	if(NULL == pWhiteboardData)
	{
		return;
	}

	doWhiteboardShow(pWhiteboardData,false);

	pWhiteboardView->handleWhiteBoardEvent(wbEvent);
}

void CoursewareDataMgr::onWhiteBoardOpt(biz::SLWhiteBoardOpt info)
{
	if (NULL == WhiteBoardDataMgr::getInstance())
	{
		return;
	}

	switch (info._nOpt)
	{
	case biz::EBoardOpt_Create:
		{
			WhiteBoardDataMgr::getInstance()->AddWhiteboard(info._nPageId, QString::fromWCharArray(info._szwbName),false);
			openCoursewareORWb(QString::fromWCharArray(info._szwbName),false);
			
			/*
			LPWHITEBOARDDATA pData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(info._nPageId);
			doWhiteboardShow(pData);
			*/
		}
		break;

	case biz::EBoardOpt_Delete:
		{
			QString wbName = QString::fromWCharArray(info._szwbName);
			delViewWidget(wbName);
		}
		break;

	case biz::EBoardOpt_ModifyName:
		{
			WhiteBoardDataMgr::getInstance()->RenameWhiteboard(info._nPageId, info._szwbName);
		}
		break;
	}
}

void CoursewareDataMgr::onCursewaveListEvent(biz::SLCursewaveListOpt info)
{
	if (ClassSeeion::GetInst()->_nClassRoomId != info._nRoomId || NULL == info._szFileName || NULL == info._szFileName[0])
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
			int nError = 0;
			if (!CoursewareTaskMgr::getInstance()->AddCourseByNet(info._szFileName, &nError))
			{
				wchar_t szError[MAX_PATH] = {0};
				GetErrorInfo(szError, nError);			

				break;
			}
			else
			{
				if(nError == COURSEWARE_ERR_REPEAT)
				{
					PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(info._szFileName);
					if(pData)
					{
						QString qstrFileName=QString::fromWCharArray(pData->m_szName);
						if(m_NowFileName.compare(qstrFileName,Qt::CaseInsensitive)!=0)
						{
							openCoursewareORWb(qstrFileName);
						}
					}
				}
			}
		}
		break;

	case biz::ECursewaveOpt_CLOSE:
	case biz::ECursewaveOpt_DEL:
		{			
			PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNetUrlEx(info._szFileName);
			if (pData && !ClassSeeion::GetInst()->IsTeacher())
			{
				doCoursewareEvent(info._nOpt, QString::fromWCharArray(pData->m_szName));
				delete pData;
				pData = NULL;
			}
		}
		break;
	}
}

void CoursewareDataMgr::onShowTypeChanged(biz::SLClassRoomShowInfo info)
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
			setNowFileName(QString::fromWCharArray(pData->m_szName));
			CoursewareTaskMgr::getInstance()->SetFilePage(pData->m_nCoursewareID,info._nShowPage);

			if (CW_FILE_DOWNING == pData->m_nDownState || CW_FILE_WAIT_DOWN == pData->m_nDownState)
			{
				doCoursewareCtrl(QString::fromWCharArray(pData->m_szName),  info._nShowPage, QString::fromWCharArray(info._szShowName));
			}
			else if (COURSEWARE_STATE_OK == pData->m_nState && CW_FILE_DOWN_END == pData->m_nDownState)
			{
				doCoursewareCtrl(QString::fromWCharArray(pData->m_szName), info._nShowPage, QString::fromWCharArray(info._szShowName));
			}

			pData->m_bISOpen = true;
			delete pData;
			pData = NULL;
		}
	}
	else if (biz::eShowType_Whiteboard == info._nShowType && info._nShowPage > 0)
	{
		LPWHITEBOARDDATA pData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByID(info._nShowPage);
		QString wbname = QString::fromWCharArray(info._szShowName);
		
		if (pData)
		{
			WhiteBoardDataMgr::getInstance()->OpenWhiteboardByID(info._nShowPage);
			doWhiteboardShow(pData);
		}
		else
		{			
			WhiteBoardDataMgr::getInstance()->AddWhiteboard(info._nShowPage, wbname,false);
			openCoursewareORWb(wbname);
		}

		/*
		if (!ClassSeeion::GetInst()->IsTeacher())
		{
			openCoursewareORWb(wbname);
		}
		*/
	}
}

void CoursewareDataMgr::doCoursewareEvent(int nType, QString fileName)
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
			delViewWidget(fileName);
		}
		break;
	default:
		break;
	}
}

LPWBAndCourseView CoursewareDataMgr::GetViewWidget(QString viewName)
{
	if (viewName.isEmpty())
	{
		return NULL;
	}

	QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecWBAndCourseView.size(); i++)
	{
		if (viewName == m_vecWBAndCourseView[i]->m_name)
		{
			return m_vecWBAndCourseView[i];			
		}
	}

	return NULL;
}

LPWBAndCourseView CoursewareDataMgr::GetViewWidget(QWidget* widgetView)
{
	if (NULL==widgetView)
	{
		return NULL;
	}

	QMutexLocker AutoLock(&m_mutex);
	for (size_t i = 0; i < m_vecWBAndCourseView.size(); i++)
	{
		if (widgetView == m_vecWBAndCourseView[i]->m_widget)
		{
			return m_vecWBAndCourseView[i];			
		}
	}

	return NULL;
}

bool CoursewareDataMgr::delViewWidget(QString viewName)
{
	if (viewName.isEmpty())
	{
		return false;
	}

	PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(viewName).unicode()).data());	
	SAFE_DELETE(pData);
	
	QMutexLocker AutoLock(&m_mutex);
	if(m_vecWBAndCourseView.empty())
	{
		return false;
	}

	size_t i = 0;
	for (; i < m_vecWBAndCourseView.size(); i++)
	{
		if (viewName == m_vecWBAndCourseView[i]->m_name)
		{
			break;
		}
	}

	if(i >= m_vecWBAndCourseView.size())
	{
		return false;
	}

	LPWBAndCourseView pView = m_vecWBAndCourseView[i];
	m_vecWBAndCourseView.erase(m_vecWBAndCourseView.begin()+i);
	SAFE_DELETE(pView);

	deleteCoursewareORWb(viewName);
	
	return true;
}

bool CoursewareDataMgr::deleteCoursewareORWb(QString fileName)
{
	if (fileName.isEmpty())
	{
		return false; 
	}

	int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
	if (COURSEWARE_UNKNOWN == nType || COURSEWARE_WHITEBOARD == nType)
	{
		WhiteBoardDataMgr::getInstance()->DelWhiteboardByName(wstring((wchar_t*)(fileName).unicode()).data());
		setNowFileName("");

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
	m_CoursewarePanel->CloseCoursewareFile(QString::fromWCharArray(pData->m_szFilePath));
	setNowFileName("");

	CoursewareTaskMgr::getInstance()->DeleteCourseByName(pData->m_szName);
	
	if (ClassSeeion::GetInst()->IsTeacher())
	{
		biz::SLCursewaveListOpt scwOpt;
		scwOpt._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
		scwOpt._nUserId = ClassSeeion::GetInst()->_nUserId;
		scwOpt._nOpt = biz::ECursewaveOpt_DEL;
		wcscpy(scwOpt._szFileName, pData->m_szNetFileName);
		
		biz::GetBizInterface()->SendCursewaveListEvent(scwOpt);
	}

	SAFE_DELETE(pData);

	return true;
}

bool CoursewareDataMgr::openCoursewareORWb(QString fileName,bool onlyOpen /* = false */)
{
	if (fileName.isEmpty())
	{
		return  false;
	}

	QString filePath;
	QString lastFile = m_LastFileName;

	int lastType = GetCoursewareFileType(wstring((wchar_t*)(lastFile).unicode()).data());
	int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());

	// xiewb 2016.12.03
	/*
	if (!lastFile.isEmpty() && (COURSEWARE_VIDEO == lastType || COURSEWARE_AUDIO == lastType) && 
		(lastFile != fileName) && (COURSEWARE_VIDEO == nType || COURSEWARE_AUDIO == nType))
	{
		PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(lastFile).unicode()).data());
		if (pData)
		{
			m_CoursewarePanel->PauseCoursewareShow(QString::fromWCharArray(pData->m_szFilePath), true);
			SAFE_DELETE(pData);
		}
	}
	*/
	
	//video and audio courseware handle
	if(COURSEWARE_VIDEO == nType || COURSEWARE_AUDIO == nType)   //xiewb 2017.07.11
	{
		PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
		QString qstrFile;
		if(pData)
		{
			qstrFile = QString::fromWCharArray(pData->m_szFilePath);
			SAFE_DELETE(pData);
		}
		
		m_CoursewarePanel->PauseAllMediaCourseware(qstrFile);
	}
	else if (!lastFile.isEmpty() && COURSEWARE_VIDEO == lastType&& lastFile != fileName)
	{
		PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(lastFile).unicode()).data());
		if (pData)
		{
			m_CoursewarePanel->PauseCoursewareShow(QString::fromWCharArray(pData->m_szFilePath), true);
			SAFE_DELETE(pData);
		}
	}

	if (COURSEWARE_UNKNOWN == nType || COURSEWARE_WHITEBOARD == nType)
	{
		LPWHITEBOARDDATA pData = WhiteBoardDataMgr::getInstance()->GetWhiteboardByName(wstring((wchar_t*)(fileName).unicode()).data());
		doWhiteboardShow(pData);
		WhiteBoardDataMgr::getInstance()->OpenWhiteboardByName(wstring((wchar_t*)(fileName).unicode()).data());
		return true;
	}
	
	PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
	if (NULL==pData)
	{
		return false;
	}
	filePath = QString::fromWCharArray(pData->m_szFilePath);
	CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
	if (COURSEWARE_STATE_OK == pData->m_nState)
	{
		if (!ClassSeeion::GetInst()->_bBeginedClass)
		{
			if(ClassRoomDialog::isValid())
			{
				//2018.10.25 xiewb 
				ClassRoomDialog::getInstance()->showMainState(QString::fromWCharArray(pData->m_szName), 100);
			}

			return true;
		}

		if (m_CoursewarePanel->OpenCoursewareFile(QString::fromWCharArray(pData->m_szFilePath), true,onlyOpen))
		{	
			if(pData->m_nPage > 0)
			{
				m_CoursewarePanel->GotoPage(QString::fromWCharArray(pData->m_szFilePath),CTRL_SEEK_PAGE,pData->m_nPage);
				CoursewareTaskMgr::getInstance()->SetFilePage(pData->m_nCoursewareID, 0);
			}

			doCoursewareShow(pData);
			CoursewareTaskMgr::getInstance()->OpenCourseware(pData->m_szFilePath);					
		}
	}
	else
	{
		//课件未完成加载
		setTextPage(fileName, 1);

		if(ClassRoomDialog::isValid())
		{
			//2018.10.25 xiewb add for later
			ClassRoomDialog::getInstance()->showMainState(fileName, pData->m_nDealPerc);
		}
	}

	SAFE_DELETE(pData);

	return true;
}

bool CoursewareDataMgr::setTextPage(QString fileName, int nPage)
{
	LPWBAndCourseView pView = GetViewWidget(fileName);
	if (NULL==pView||nPage<=0)
	{
		return false;
	}

	pView->m_npage = nPage;

	return true;
}

void CoursewareDataMgr::doCoursewareCtrl(QString filename, int nPage, QString md5name)
{
	if (filename.isEmpty())
	{
		return;
	}

	if (nPage >= 0 && m_NowFileName.compare(filename,Qt::CaseInsensitive)==0)
	{
		m_CoursewarePanel->SetCoursewareShow(biz::eShowType_Cursewave, md5name, nPage);		
		PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(filename).unicode()).data());
		if (pData)
		{
			doCoursewareShow(pData);
		}

		SAFE_DELETE(pData);

		return;
	}

	setNowFileName(filename);	
	openCoursewareORWb(filename);

	if (nPage > 0)
	{
		GotoPage(CTRL_SEEK_PAGE, nPage);
	}
}

bool CoursewareDataMgr::closeCourseware(QString fileName)
{
	if (fileName.isEmpty())
	{
		return false; 
	}

	int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
	if (COURSEWARE_UNKNOWN == nType || COURSEWARE_WHITEBOARD == nType)
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
	m_CoursewarePanel->CloseCoursewareFile(QString::fromWCharArray(pData->m_szFilePath),false);
	setNowFileName("");
	
	CoursewareTaskMgr::getInstance()->CloseCourseware(pData->m_szName);	

	SAFE_DELETE(pData);

	return true;
}

void CoursewareDataMgr::onWhiteboardModifyName(QString oldName, QString newName)
{
	if (oldName.isEmpty() || newName.isEmpty())
	{
		return;
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

void CoursewareDataMgr::onCoursewareSetPos(QString fileName, int nPercent)
{
	if (fileName.isEmpty())
	{
		return;
	}

	if (m_NowFileName.compare(fileName,Qt::CaseInsensitive)==0) 
	{
		//2018.10.25 xiewb add for later
		ClassRoomDialog::getInstance()->showMainState(fileName, nPercent);
	}
}


void CoursewareDataMgr::onCoursewareStateChange(QString fileName, int nState, int nError)
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

		}
		break;

	case ACTION_UPLOAD_END:
		{
			if (!ClassSeeion::GetInst()->IsTeacher())
			{
				break;
			}
		
			PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
			if (NULL == pData)
			{
				break;
			}
			
			if (pData->m_nCoursewareType == COURSEWARE_IMG ||
				pData->m_nCoursewareType == COURSEWARE_AUDIO ||
				pData->m_nCoursewareType == COURSEWARE_VIDEO ||
				pData->m_nCoursewareType == COURSEWARE_PDF ||
				pData->m_nCoursewareType == COURSEWARE_FLASH)
			{
				CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
				if (m_NowFileName.compare(fileName,Qt::CaseInsensitive)==0)
				{			
					//xiewb 2017.05.18
					openCoursewareORWb(fileName,false);

					if(pData->m_nCoursewareType == COURSEWARE_AUDIO ||	pData->m_nCoursewareType == COURSEWARE_VIDEO)
					{
						m_CoursewarePanel->PauseCoursewareShow(QString::fromWCharArray(pData->m_szFilePath), true);						
					}
				}
				else
				{
					if (m_CoursewarePanel->OpenCoursewareFile(QString::fromWCharArray(pData->m_szFilePath), true,true))
					{
						doCoursewareShow(pData,false);
					}
				}
			}			
    
			SAFE_DELETE(pData);
			//ClassRoomDialog UI 设置
		}
		break;

	case ACTION_UPLOAD_FAILED:
		{
			if(ClassRoomDialog::isValid())
			{
				int showState = (nError == COURSEWARE_ERR_TRANSMEDIO)?MAIN_SHOW_TYPE_TRANS_FAILED:MAIN_SHOW_TYPE_UPLOAD_FAILED;
				//2018.10.25 xiewb add for later
				ClassRoomDialog::getInstance()->showMainState(fileName, showState);
			}
		}
		break;

	case ACTION_DOWN_BENGIN:
		{			
		}
		break;

	case ACTION_DOWN_END:
		{	
			PCOURSEWAREDATA pData = CoursewareTaskMgr::getInstance()->GetCoursewareByNameEx(wstring((wchar_t*)(fileName).unicode()).data());
			if (NULL == pData)
			{
				break;
			}
    
			CoursewareTaskMgr::getInstance()->SetFileOpen(pData->m_nCoursewareID, true);
			if (m_NowFileName.compare(fileName,Qt::CaseInsensitive)==0)
			{		
				//xiewb 2017.05.18
				openCoursewareORWb(fileName,false);

				if(pData->m_nCoursewareType == COURSEWARE_AUDIO ||	pData->m_nCoursewareType == COURSEWARE_VIDEO)
				{
					m_CoursewarePanel->PauseCoursewareShow(QString::fromWCharArray(pData->m_szFilePath), true);
				}
			}
			else
			{
				if (m_CoursewarePanel->OpenCoursewareFile(QString::fromWCharArray(pData->m_szFilePath), true,true))
				{
					doCoursewareShow(pData,false);
				}
			}
    
			SAFE_DELETE(pData);
		}

		break;

	case ACTION_DOWN_FAILED:
		{
			if(ClassRoomDialog::isValid())
			{	
				//2018.10.25 xiewb add for later
				ClassRoomDialog::getInstance()->showMainState(fileName, MAIN_SHOW_TYPE_DOWN_FAILED);
			}          
		}
		break;

	default:
		break;
	}
}

bool CoursewareDataMgr::doCoursewareShow(PCOURSEWAREDATA pData,bool setNowShow /* = true */)
{	
	if(NULL == pData)
	{		
		return false;
	}

	LPCOURSEWARESHOW pShow = m_CoursewarePanel->GetCoursewareShowByFileName(QString::fromWCharArray(pData->m_szFilePath));
	if(NULL == pShow)
	{		
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
		case COURSEWARE_FLASH:
			{
				pView->m_widget = pShow->_show._flash;
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
	}
	
	if(setNowShow)
	{
		setNowFileName(pView->m_name);
		
		if(ClassRoomDialog::isValid())	
		{
			//2018.10.25 xiewb add for later
			ClassRoomDialog::getInstance()->showMainView(pView->m_widget);
		}

		setCoursewareShowBar(pView,pShow->_type);
	}
	
	return true;
}

bool CoursewareDataMgr::doWhiteboardShow(LPWHITEBOARDDATA pData,bool setNowShow /* = true */)
{	
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
	}
	
	if(!setNowShow)
	{
		return false;
	}
	
	setNowFileName(pView->m_name);
	
	if(ClassRoomDialog::isValid())	
	{
		//2018.10.25 xiewb add for later
		ClassRoomDialog::getInstance()->showMainView(pView->m_widget);
		ClassRoomDialog::getInstance()->setCoursewareTool(COURSEWARE_WHITEBOARD,0, 0, 0); 
	}

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

void CoursewareDataMgr::onMediaPlayPosChange(unsigned int nPos,string&fileName)
{
	if(NULL == m_CoursewarePanel)
	{
		return;
	}

	LPCOURSEWARESHOW pShow = m_CoursewarePanel->GetCurrentCoursewareShow();	
	if(NULL == pShow)
	{
		return;
	}

	QString qstrFile;
	Util::AnsiToQString(fileName.c_str(),fileName.length(),qstrFile);

	LPCOURSEWARESHOW pSelShow = m_CoursewarePanel->GetCoursewareShowByFileName(qstrFile);
	if(pSelShow != pShow)
	{
		return;
	}
	
	if(pShow->_type != COURSEWARE_VIDEO && pShow->_type != COURSEWARE_AUDIO){
		return;
	}

	if(NULL==pShow->_show._media ){
		return;
	}

	if(ClassRoomDialog::isValid())
	{
		//2018.10.25 xiewb add for later
		int all = pShow->_show._media->getTotalPlayTime();
		int state = pShow->_show._media->getState();

		ClassRoomDialog::getInstance()->setCoursewareTool(pShow->_type,nPos,all,state,(QWidget*)pShow->_show._media);
	}
}

void CoursewareDataMgr::onPDFPageChange(int nPos)
{
	if(NULL == m_CoursewarePanel)
	{
		return;
	}

	LPCOURSEWARESHOW pShow = m_CoursewarePanel->GetCurrentCoursewareShow();
	if(NULL == pShow)
	{
		return;
	}

	if (pShow->_type != COURSEWARE_PDF && pShow->_type != COURSEWARE_DOC &&
		pShow->_type != COURSEWARE_EXCLE && pShow->_type != COURSEWARE_EXCLE){
		return;
	}

	if(ClassRoomDialog::isValid())
	{
		//2018.10.25 xiewb add for later
		ClassRoomDialog::getInstance()->setPageShowText(nPos,pShow->_show._pdf->getPageCount());
	}
}

int CoursewareDataMgr::getCurShowType()
{
	if(m_NowFileName.isEmpty()){
		return -1;
	}

	int nType = GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data());
	return nType;
}

void CoursewareDataMgr::setCoursewareShowBar(LPWBAndCourseView pView,int nType)
{
	if(NULL==pView){
		return;
	}

	switch(nType)
	{
	case COURSEWARE_AUDIO:
	case COURSEWARE_VIDEO:
		{
			if(!ClassRoomDialog::isValid()) 
			{
				break;
			}

			LPCOURSEWARESHOW pShow = m_CoursewarePanel->GetCoursewareShowByShowWidget(pView->m_widget);
			if(NULL==pShow){
				break;
			}

			int curPos = 0;
			int length = 0;
			int state = 0;

			do 
			{
				if(NULL==pShow){
					break;
				}

				if(NULL == pShow->_show._media){
					break;
				}

				curPos = pShow->_show._media->getCurPlayTime();
				length = pShow->_show._media->getTotalPlayTime();
				state = pShow->_show._media->getState();

				if (ClassSeeion::GetInst()->IsTeacher())
				{
					biz::SLClassRoomShowInfo sShowInfo;
					sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
					sShowInfo._nShowType = biz::eShowType_Cursewave;
					WCHAR szFile[MAX_PATH+1] = {0};
					GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), szFile, MAX_PATH);
					wcscpy(sShowInfo._szShowName, szFile);
					sShowInfo._nShowPage = pShow->_show._media->getState();						
					if(sShowInfo._nShowPage ==CMediaFilePlayer::PAUSE )
					{
						int nNowTime = pShow->_show._media->getCurPlayTime(true);
						sShowInfo._nShowPage |= (nNowTime << 4);
					}
					biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
				}
				else
				{
					//xiewb .2016.12.03
					bool bTeaExsit = true;
					biz::IClassRoomInfo * classInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
					biz::SLUserInfo teaInfo = classInfo->GetTeacherInfo();
					biz::SLUserInfo teaDetailInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(teaInfo.nUserId,&bTeaExsit);
					if(!bTeaExsit)
					{
						pShow->_show._media->pause(true);
					}
				}
			} while (false);

			ClassRoomDialog::getInstance()->setCoursewareTool(nType,curPos,length,state,pView->m_widget);
		}
		break;
	case COURSEWARE_PDF:
	case COURSEWARE_EXCLE:
	case COURSEWARE_DOC:
	case COURSEWARE_PPT:
		{
			if(!ClassRoomDialog::isValid()) 
			{
				break;
			}

			LPCOURSEWARESHOW pShow = m_CoursewarePanel->GetCoursewareShowByShowWidget(pView->m_widget);
			int curPage = 1;
			int allPage = 1;

			do 
			{
				if(NULL==pShow){
					break;
				}

				if(NULL == pShow->_show._pdf){
					break;
				}

				curPage = pShow->_show._pdf->getCurPage()+1;
				allPage = pShow->_show._pdf->getPageCount();				
			} while (false);

			ClassRoomDialog::getInstance()->setCoursewareTool(nType,curPage,allPage,0,pView->m_widget);
		}
		break;
	default:
		if(ClassRoomDialog::isValid())
		{
			ClassRoomDialog::getInstance()->setCoursewareTool(nType,0,0,0,pView->m_widget);
		}	
		break;
	}

	return;
}