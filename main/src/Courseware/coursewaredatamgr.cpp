//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
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
#include "./whiteboard/WhileBoardDataMgr.h"
#include "CoursewarePanel.h"

CoursewareDataMgr*  CoursewareDataMgr::  m_instance = NULL;
CoursewareDataMgr::CoursewareDataMgr()
{ 
    m_CoursewarePanel = new QCoursewarePannel();
    m_NowFileName = "";
    m_nFontSize = WB_SIZE_SMALL;
    m_nColorType = WB_COLOR_RED;
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
        {
            m_CoursewarePanel->SetEnable(m_NowFileName, enable);
        }
        break;

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
        {
            m_CoursewarePanel->SetTextSize(m_NowFileName, tSize);
        }
        break;

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
        {
            m_CoursewarePanel->SetColor(m_NowFileName, clr);
        }
        break;

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
        {
            m_CoursewarePanel->SetMode(m_NowFileName, mode);
        }
        break;

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
        {
            m_CoursewarePanel->UnDo(m_NowFileName);
        }
        break;

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
        {
            m_CoursewarePanel->Clear(m_NowFileName);
        }
        break;

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
    if (COURSEWARE_UNKNOWN == GetCoursewareFileType(wstring((wchar_t*)(m_NowFileName).unicode()).data()))
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