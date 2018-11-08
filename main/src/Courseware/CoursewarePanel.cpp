//**********************************************************************
//	Copyright （c） 2015,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：CoursewarePanel.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2018.10.25
//	说明：
//	修改记录：
//  备注：
//**********************************************************************

#include "CoursewarePanel.h"
#include <QMutexLocker>
#include "courseware.h"
#include "session/classsession.h"
#include "common/Util.h"
#include "common/macros.h"
#include "classroomdialog.h"
#include "Courseware/coursewaretaskmgr.h"
#include <windows.h>

#define TIMES_DO_WHITEBOARD_EVENT (5000)

QCoursewarePannel::QCoursewarePannel()
{
    m_nSelIndex = 0;
    m_nTimer = 0;
    m_pdfPreview = NULL;
}

QCoursewarePannel::~QCoursewarePannel()
{
    if (m_nTimer != 0)
    {
        killTimer(m_nTimer);
        m_nTimer = 0;
    }
    CloseAllCoursewareFile();
    m_pdfPreview = NULL;
}

void QCoursewarePannel::setPreviewList(QListWidget *pwidget)
{
    if (pwidget)
    {
        m_pdfPreview = (QPDFPreviewList*)pwidget;
    }
}

bool QCoursewarePannel::OpenCoursewareFile(QString fileName, bool bCtrl,bool onlyOpen /* = false */)
{
    if (fileName.isEmpty())
    {
        return false;
    }
    
    OutputDebugStringW(L"Lock OpenCoursewareFile.\n");

    QMutexLocker AutoLock(&m_mutex);

    size_t i = 0;
    for (; i < m_vecCourShow.size(); i++)
    {
        LPCOURSEWARESHOW pShow = m_vecCourShow.at(i);
        if (NULL == pShow)
        {
            continue;
        }
        if(pShow->_fileName == fileName)
        {			
            break;
        }
    }

    if (i < m_vecCourShow.size())
    {        
        AutoLock.unlock();

		if(!onlyOpen)
		{
			SelCoursewareShow(i);
		
			if (COURSEWARE_PDF == m_vecCourShow[i]->_type&&NULL != m_vecCourShow[i]->_show._pdf)
			{
				GotoPage(fileName, CTRL_SEEK_PAGE, m_vecCourShow[i]->_show._pdf->getCurPage());
			}
		}

        return true;
    }

    int nType = GetCoursewareFileType(wstring((wchar_t*)(fileName).unicode()).data());
    if (COURSEWARE_PDF != nType && COURSEWARE_AUDIO != nType && 
		COURSEWARE_VIDEO != nType && COURSEWARE_IMG != nType &&
		COURSEWARE_FLASH != nType)
    {
        AutoLock.unlock();
        return false;
    }

    LPCOURSEWARESHOW pShow = new COURSEWARESHOW;
	
    pShow->_type = nType;    
    pShow->_fileName = fileName;
	pShow->_open = true;

    switch (nType)
    {
    case COURSEWARE_PDF:
        {
            QPDFWnd *pPDFShow = OpenPDFFile(fileName, pShow->_id, 0, bCtrl);
            if (NULL == pPDFShow)
            {
                AutoLock.unlock();
                return false;
            }
            pShow->_show._pdf = pPDFShow;
        }
        break;

    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
            MediaPlayerShowWnd *pMedia = OpenMediaFile(fileName,onlyOpen);			
            if (NULL == pMedia)
            {
                AutoLock.unlock();
                return false;
            }
            pShow->_show._media = pMedia;
        }
        break;

    case COURSEWARE_IMG:
        {
            QImageShow *pImage = OpenImageFile(fileName);
            if (NULL == pImage)
            {
                AutoLock.unlock();
                return false;
            }
            pShow->_show._image = pImage;
        }
        break;

	case COURSEWARE_FLASH:
		{
			QFlashPlayWidget *pFlash = OpenFlashFile(fileName);
			if (NULL == pFlash)
			{
				AutoLock.unlock();
				return false;
			}
			pShow->_show._flash = pFlash;
		}
		break;

    default: 
		pShow->_open = false;
        break;
    }
    m_vecCourShow.push_back(pShow);
	AutoLock.unlock();
	
	if(!onlyOpen)
	{
		SelCoursewareShow(m_vecCourShow.size()-1);
	}
	
    return true;
}

void QCoursewarePannel::CloseCoursewareFile(QString fileName,bool delCourseware /* = true */)
{
    if (fileName.isEmpty())
    {
        return;
    }

    OutputDebugStringW(L"Lock CloseCoursewareFile.\n");

    QMutexLocker AutoLock(&m_mutex);

    if (m_vecCourShow.empty())
    {
        AutoLock.unlock();
        return;
    }
    LPCOURSEWARESHOW pShow = NULL;
    size_t i =0;
    for (; i < m_vecCourShow.size(); i++)
    {
        pShow = m_vecCourShow.at(i);
        if (NULL == pShow)
        {
            continue;
        }

        if (pShow->_fileName == fileName)
        {
            break;
        }
        pShow = NULL;
    }
    if (NULL == pShow)
    {
        AutoLock.unlock();
        return;
    }

	if(delCourseware)
    {
		m_vecCourShow.erase(m_vecCourShow.begin()+i);
		DelCourseware(pShow);
	}
	else
	{
		CloseCourseware(pShow);
	}

    if (i == m_nSelIndex && m_vecCourShow.size() > 0)
    {
        m_nSelIndex = -1;
    }
    else if (i < m_nSelIndex && m_vecCourShow.size() > 0)
    {
        m_nSelIndex--;
    }

	SelCoursewareShow(m_nSelIndex);
    AutoLock.unlock();
}

void QCoursewarePannel::CloseAllCoursewareFile()
{
    OutputDebugStringW(L"Lock CloseAllCoursewareFile.\n");

    QMutexLocker AutoLock(&m_mutex);

    FreeAllShow();

    AutoLock.unlock();
}

bool QCoursewarePannel::GetShowCourseware(char &nType, QString &fileName, int &nPage)
{
    OutputDebugStringW(L"Lock GetShowCourseware....\n");

    QMutexLocker AutoLock(&m_mutex);

    if (m_vecCourShow.empty() || m_nSelIndex >= m_vecCourShow.size())
    {
        AutoLock.unlock();
        return false;
    }

    LPCOURSEWARESHOW pShow = m_vecCourShow.at(m_nSelIndex);
    if (NULL == pShow)
    {
        AutoLock.unlock();
        return false;
    }

    switch (pShow->_type)
    {
    case COURSEWARE_PDF:
        {
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
            nType = biz::eShowType_Cursewave;
            nPage = pShow->_show._pdf->getPageCount()+1;

            if (!fileName.isEmpty())
            {
                WCHAR wszFileName[MAX_PATH+1] = {0};
                GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszFileName, MAX_PATH);
                fileName = QString::fromWCharArray(wszFileName);
            }
        }
        break;

    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
			if(NULL == pShow->_show._media)
			{
				break;
			}

            nType = biz::eShowType_Cursewave;
            nPage = pShow->_show._media->getCurPlayTime();

            if(!fileName.isEmpty())
            {				
                WCHAR wszFileName[MAX_PATH + 1] = { 0 };
                GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszFileName, MAX_PATH);
                fileName = QString::fromWCharArray(wszFileName);
            }
        }
        break;
	case COURSEWARE_FLASH:
		{	
			nType = biz::eShowType_Cursewave;
			nPage = pShow->_show._flash->getCurFrame();
			if (!fileName.isEmpty())
			{
				WCHAR wszFileName[MAX_PATH+1] = {0};
				GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszFileName, MAX_PATH);
				fileName = QString::fromWCharArray(wszFileName);
			}
		}
		break;
    case COURSEWARE_IMG:
        {	
            nType = biz::eShowType_Cursewave;
            nPage = 0;
            if (!fileName.isEmpty())
            {
                WCHAR wszFileName[MAX_PATH+1] = {0};
                GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszFileName, MAX_PATH);
                fileName = QString::fromWCharArray(wszFileName);
            }
        }
        break;

    default:
        AutoLock.unlock();
        break;
    }

    AutoLock.unlock();
    return true;
}

void QCoursewarePannel::SetCoursewareShow(char nType, QString fileName, int nPage)
{
    if (fileName.isEmpty())
    {
        return;
    }

    WCHAR wszLocalFile[MAX_PATH+1] = {0};
    GetCoursewareDownloadFileName((wchar_t*)wstring((wchar_t*)(fileName).unicode()).data(), wszLocalFile, MAX_PATH);
    if (!IsExistFile(wszLocalFile))
    {
        return;
    }
    long long nFileSize = GetCoursewareFileSize(wszLocalFile);
    if (nFileSize <= 0)
    {
        return;
    }
    if (!OpenCoursewareFile(QString::fromWCharArray(wszLocalFile), false))
    {
        return;
    }

    LPCOURSEWARESHOW pShow = GetCoursewareShowByFileName(QString::fromWCharArray(wszLocalFile));
    if (NULL == pShow)
    {
        return;
    }

    switch (pShow->_type)
    {
    case COURSEWARE_PDF:
        {
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
            pShow->_show._pdf->gotoPage(nPage);
        }
        break;

    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
			if(NULL == pShow->_show._media)
			{
				break;
			}

			UINT nCtrl = nPage & 0xf;			
			switch (nCtrl)
			{
			case CMediaFilePlayer::PLAY:
				{   
					pShow->_show._media->pause(false);
				}
				break;

			case CMediaFilePlayer::PAUSE:
				{					
					pShow->_show._media->pause(true);

					UINT seekPos = (nPage >> 4);
					if(seekPos > 0)
					{
						pShow->_show._media->seekEx(seekPos);						
					}
				}
				break;

			case CMediaFilePlayer::STOP:
				{
					pShow->_show._media->stop();
				}
				break;
			case CMediaFilePlayer::SEEK:
			default:
				{
					int curPos = pShow->_show._media->getCurPlayTime();
					int seekPos = (nPage >> 4);		

					Util::PrintTrace("media file seek,file : %s, total : %d ,cur pos: %d ,seek pos: %d",pShow->_show._media->getFile().c_str(),
						pShow->_show._media->getTotalPlayTime(),pShow->_show._media->getCurPlayTime(),seekPos);

					if (abs(curPos-seekPos) > 200)
					{
						pShow->_show._media->seek(seekPos,true);

					}
				}
				break;			
            }
            
        }
        break;

    case COURSEWARE_IMG:
        {
			if(NULL == pShow->_show._image)
			{
				break;
			}
            pShow->_show._image->show();
        }
        break;

	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			pShow->_show._flash->gotoFrame(nPage);
			pShow->_show._flash->show();
		}
		break;
    }
}

void QCoursewarePannel::PauseAllMediaCourseware(QString fileNo)
{
	QMutexLocker AutoLock(&m_mutex);

	if (m_vecCourShow.empty() || m_nSelIndex >= m_vecCourShow.size())
	{
		AutoLock.unlock();
		return;
	}

	size_t i = 0;
	LPCOURSEWARESHOW pShow = NULL;
	
	for (; i < m_vecCourShow.size(); i++)
	{
		pShow = m_vecCourShow.at(i);
		if (NULL == pShow)
		{
			continue;
		}

		if(pShow->_type != COURSEWARE_AUDIO && pShow->_type != COURSEWARE_VIDEO)
		{
			continue;
		}

		if(pShow->_fileName == fileNo)
		{
			continue;
		}

		if(NULL == pShow->_show._media)
		{
			continue;
		}

		if( pShow->_show._media->getState() == CMediaFilePlayer::PAUSE ||
			pShow->_show._media->getState() == CMediaFilePlayer::STOP )
		{
			continue;
		}

		if(!pShow->_show._media->pause(true))
		{
			continue;
		}

		if (!ClassSeeion::GetInst()->IsTeacher())
		{
			continue;
		}
		
		int curPos = pShow->_show._media->getCurPlayTime(true);
		biz::SLClassRoomShowInfo sShowInfo;
		WCHAR szFile[MAX_PATH+1] = {0};

		sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
		sShowInfo._nShowType = biz::eShowType_Cursewave;
		GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), szFile, MAX_PATH);

		wcscpy(sShowInfo._szShowName, szFile);
		sShowInfo._nShowPage = CMediaFilePlayer::PAUSE;
		sShowInfo._nShowPage |= (curPos << 4);
		biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
	}

	AutoLock.unlock();
}

void QCoursewarePannel::PauseCoursewareShow(QString fileName, bool pause)
{
    OutputDebugStringW(L"Lock PauseCoursewareShow.\n");

    QMutexLocker AutoLock(&m_mutex);

    if (m_vecCourShow.empty() || m_nSelIndex >= m_vecCourShow.size())
    {
        AutoLock.unlock();
        return;
    }

    size_t i = 0;
    LPCOURSEWARESHOW pShow = NULL;
    for (; i < m_vecCourShow.size(); i++)
    {
        pShow = m_vecCourShow.at(i);
        if (NULL == pShow)
        {
            continue;
        }
        if(pShow->_fileName == fileName)
        {
            break;
        }
    }

    if (NULL == pShow)
    {
        AutoLock.unlock();
        return;
    }

    bool bResult = false;
	QWidget * widgetShow = NULL;
	UINT curPos = 0;
	UINT allPos = 0;
	UINT state = 0;

    switch(pShow->_type)
    {
    case COURSEWARE_PDF:
        {
          
        }
        break;

    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
			if(NULL == pShow->_show._media)
			{
				break;
			}
			
			if(pShow->_show._media->getState() == CMediaFilePlayer::PLAY)
			{
				pShow->_show._media->pause(true);
			}
			
			curPos = pShow->_show._media->getCurPlayTime(true);
			allPos = pShow->_show._media->getTotalPlayTime();
			state = pShow->_show._media->getState();

			widgetShow = pShow->_show._media;
            bResult = true;
        }
        break;

	case COURSEWARE_FLASH:
		{
			//...暂停
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			pShow->_show._flash->pause(true);
			
			bResult = true;
		}
		break;
    case COURSEWARE_IMG:
        {

        }
        break;
    }

    if (bResult)
    {
        if (ClassSeeion::GetInst()->IsTeacher())
        {
            biz::SLClassRoomShowInfo sShowInfo;
            sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
            sShowInfo._nShowType = biz::eShowType_Cursewave;
            WCHAR szFile[MAX_PATH+1] = {0};
            GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), szFile, MAX_PATH);
            wcscpy(sShowInfo._szShowName, szFile);
            sShowInfo._nShowPage = pause ? CMediaFilePlayer::PAUSE : CMediaFilePlayer::PLAY;
			if(pause)
			{
				sShowInfo._nShowPage |= (curPos << 4);
			}

            biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
        }
    }

    AutoLock.unlock();

	if(ClassRoomDialog::isValid() && bResult && widgetShow)
	{
		//xiewb 2018.10.25 add later
		ClassRoomDialog::getInstance()->setCoursewareTool(pShow->_type,curPos,allPos,state,widgetShow);
	}
}

LPCOURSEWARESHOW QCoursewarePannel::GetCurrentCoursewareShow()
{
    OutputDebugStringW(L"Lock GetCurrentCoursewareShow.\n");

    QMutexLocker AutoLock(&m_mutex);

    if(m_vecCourShow.empty() || m_nSelIndex >= m_vecCourShow.size() || m_nSelIndex < 0)
    {
        return NULL;
    }

    LPCOURSEWARESHOW pShow = m_vecCourShow.at(m_nSelIndex);
    
	return pShow;
}

LPCOURSEWARESHOW QCoursewarePannel::GetCoursewareShowByFileName(QString& fileName)
{
	if(fileName.isEmpty())
	{
		return NULL;
	}

	OutputDebugStringW(L"Lock GetCoursewareShowByFileName.\n");

	QMutexLocker AutoLock(&m_mutex);
	if(m_vecCourShow.empty())
	{
		return NULL;
	}

	LPCOURSEWARESHOW pShow = NULL;
	size_t i = 0;
	for (; i < m_vecCourShow.size(); i++)
	{
		pShow = m_vecCourShow.at(i);
		if (NULL == pShow)
		{
			continue;
		}

		if(pShow->_fileName.compare(fileName,Qt::CaseInsensitive) == 0)
		{
			return pShow;
		}
	}

	return NULL;
}

LPCOURSEWARESHOW QCoursewarePannel::GetCoursewareShowByShowWidget(QWidget* showWidget)
{
	if(NULL == showWidget)
	{
		return NULL;
	}
	
	OutputDebugStringW(L"Lock GetCoursewareShowByShowWidget.\n");

	QMutexLocker AutoLock(&m_mutex);
	if(m_vecCourShow.empty())
	{
		return NULL;
	}

	LPCOURSEWARESHOW pShow = NULL;
	size_t i = 0;
	for (; i < m_vecCourShow.size(); i++)
	{
		pShow = m_vecCourShow.at(i);
		if (NULL == pShow)
		{
			continue;
		}
		
		if(pShow->_show._pdf == showWidget)
		{
			return pShow;
		}
	}

	return NULL;
}

void QCoursewarePannel::FreeAllShow()
{
    if (m_vecCourShow.empty())
    {
        return;
    }

    for (size_t i = 0; i < m_vecCourShow.size(); i++)
    {
        LPCOURSEWARESHOW pShow = m_vecCourShow.at(i);
        DelCourseware(pShow);        
    }

    m_vecCourShow.clear();
}

void QCoursewarePannel::SelCoursewareShow(UINT nIndex)
{
    if(nIndex >= m_vecCourShow.size())
    {
        return;
    }

    LPCOURSEWARESHOW pShow = NULL;
    if(m_nSelIndex >= 0 && m_nSelIndex != nIndex && m_nSelIndex < m_vecCourShow.size())
    {
        pShow = m_vecCourShow.at(m_nSelIndex);
        ShowCourseware(pShow,false);
    }

    pShow = m_vecCourShow.at(nIndex);
	if(!pShow->_open)
	{
		switch (pShow->_type)
		{
		case COURSEWARE_PDF:
			{	
				if(NULL == pShow->_show._pdf)
				{
					break;
				}
				pShow->_show._pdf->openFile(pShow->_fileName);
			}
			break;

		case COURSEWARE_AUDIO:
		case COURSEWARE_VIDEO:
			{
				if(NULL == pShow->_show._media)
				{
					break;
				}
				std::string strfile;
				Util::QStringToString(pShow->_fileName, strfile);				
				pShow->_show._media->play(strfile);
			}
			break;

		case COURSEWARE_IMG:
			{	
				if(NULL == pShow->_show._image)
				{
					break;
				}

				pShow->_show._image->openFile(pShow->_fileName);
			}
			break;
		case COURSEWARE_FLASH:
			{	
				if(NULL == pShow->_show._flash)
				{
					break;
				}

				pShow->_show._flash->play(pShow->_fileName);
			}
			break;
		}
	}

    ShowCourseware(pShow,true);

    m_nSelIndex = nIndex;
}

void QCoursewarePannel::ShowCourseware(LPCOURSEWARESHOW pShow, bool bShow)
{
    if (NULL == pShow)
    {
        return;
    }

    biz::SLClassRoomShowInfo sShowInfo;
    sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;

    switch (pShow->_type)
    {
    case COURSEWARE_PDF:
        {
			if(NULL == pShow->_show._pdf)
			{
				break;
			}

			if(!bShow)
            {
				pShow->_show._pdf->hide();
				break;
			}

			pShow->_show._pdf->show();
            sShowInfo._nShowType = biz::eShowType_Cursewave;
            sShowInfo._nShowPage = pShow->_show._pdf->getCurPage();

            WCHAR wszUpFile[MAX_PATH+1] = {0};
            GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszUpFile, MAX_PATH);
            wcscpy(sShowInfo._szShowName, wszUpFile);
        }
        break;
        
    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
			if(NULL == pShow->_show._media)
			{
				break;
			}

			if(!bShow)
			{
				pShow->_show._media->hide();
				break;
			}

            pShow->_show._media->show();
			if(pShow->_show._media->getState() == CMediaFilePlayer::PLAY)
			{
				pShow->_show._media->pause(false);
			}

            sShowInfo._nShowType = biz::eShowType_Cursewave;
			sShowInfo._nShowPage = pShow->_show._media->getState();						
			if(sShowInfo._nShowPage ==CMediaFilePlayer::PAUSE )
			{
				UINT curPos = pShow->_show._media->getCurPlayTime(true);
				sShowInfo._nShowPage |= (curPos << 4);
			}

            WCHAR wszUpFile[MAX_PATH+1] = {0};
            GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszUpFile, MAX_PATH);
            wcscpy(sShowInfo._szShowName, wszUpFile);
        }
        break;

    case COURSEWARE_IMG:
        {
			if(NULL == pShow->_show._image)
			{
				break;
			}

			if(!bShow)
			{
				pShow->_show._image->hide();
				break;
			}

            pShow->_show._image->show();
            sShowInfo._nShowType = biz::eShowType_Cursewave;
            sShowInfo._nShowPage = 0;

            WCHAR wszUpFile[MAX_PATH+1] = {0};
            GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszUpFile, MAX_PATH);
            wcscpy(sShowInfo._szShowName, wszUpFile);
        }
        break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			if(!bShow)
			{
				pShow->_show._flash->hide();
				break;
			}

			pShow->_show._flash->show();
			sShowInfo._nShowType = biz::eShowType_Cursewave;
			sShowInfo._nShowPage = pShow->_show._flash->getCurFrame();

			WCHAR wszUpFile[MAX_PATH+1] = {0};
			GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszUpFile, MAX_PATH);
			wcscpy(sShowInfo._szShowName, wszUpFile);
		}
		break;

    default:
        return;
    }

    if (bShow && ClassSeeion::GetInst()->IsTeacher())
    {
        biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
    }
}

void QCoursewarePannel::DelCourseware(LPCOURSEWARESHOW pShow)
{
    if (NULL == pShow)
    {
        return;
    }

    switch (pShow->_type)
    {
    case COURSEWARE_PDF:
        {
			if(NULL == pShow->_show._pdf)
            {
				break;
			}
			
			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._pdf);
			}

			pShow->_show._pdf->closeFile(true);            
			pShow->_show._pdf = NULL;
        }
        break;

    case COURSEWARE_AUDIO:
    case COURSEWARE_VIDEO:
        {
			if(NULL == pShow->_show._media)
            {
				break;
			}
			
			if (CMediaFilePlayer::STOP != pShow->_show._media->getState())
			{
				pShow->_show._media->stop();
			}

			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._media);
			}
			
            pShow->_show._media->hide();            
			delete pShow->_show._media;
			pShow->_show._media = NULL;			
		}
        break;

    case COURSEWARE_IMG:
        {
			if(NULL == pShow->_show._image)
			{
				break;
			}

			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._image);
			}

			pShow->_show._image->closeFile();            
            delete pShow->_show._image;
			pShow->_show._image = NULL;
        }
        break;
	case COURSEWARE_FLASH:
        {
			if(NULL == pShow->_show._flash)
			{
				break;
			}
            
			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._flash);
			}

			pShow->_show._flash->stop();
            delete pShow->_show._flash;
			pShow->_show._flash = NULL;
        }
        break;
    }

    SAFE_DELETE(pShow);

    return;
}

void QCoursewarePannel::CloseCourseware(LPCOURSEWARESHOW pShow)
{
	if (NULL == pShow)
	{
		return;
	}

	pShow->_open = false;
	switch (pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}

			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._pdf);
			}

			pShow->_show._pdf->closeFile(false);			
		}
		break;

	case COURSEWARE_AUDIO:
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			if (CMediaFilePlayer::STOP != pShow->_show._media->getState())
			{                
				pShow->_show._media->stop();
			}

			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._media);
			}
		}
		break;

	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			/****************************************************************
             2018.10.25 xiewb add for later
			*****************************************************************/
			if(ClassRoomDialog::isValid())
			{
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._image);
			}            
			pShow->_show._image->closeFile();						
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			if(ClassRoomDialog::isValid())
			{
				// 2018.10.25 xiewb add later
				ClassRoomDialog::getInstance()->removeMainView(pShow->_show._flash);
			}            
			pShow->_show._flash->stop();						
		}
		break;

	default:
		return;
	}	
}

QPDFWnd *QCoursewarePannel::OpenPDFFile(QString fileName, int nID, long* lParam /* = 0 */, bool bCtrl /* = true */)
{
    if (fileName.isEmpty())
    {
        return NULL;
    }

    QPDFWnd *pShow = NULL;
    bool bOpen = false;

    do 
    {
        pShow = new QPDFWnd(NULL);
        if (NULL == pShow)
        {
            break;;
        }
		
		/****************************************************************/
        /*2018.10.25 xiewb add later
		/******************************************************************/
		if(ClassRoomDialog::isValid())
		{
			ClassRoomDialog::getInstance()->addMainView(pShow);
		}

		m_pdfPreview->addPDFPreview(pShow);
        bOpen = pShow->openFile(fileName);
        if(!bOpen)
        {
            break;
        }
        pShow->setUserId(ClassSeeion::GetInst()->_nUserId);
        pShow->setCourseId(ClassSeeion::GetInst()->_nClassRoomId);
        if (ClassSeeion::GetInst()->IsTeacher())
        {
            pShow->setWbCtrl(WB_CTRL_EVERY);
        }
        else
        {
            pShow->setWbCtrl(WB_CTRL_NONE);
        }
        connect(pShow, SIGNAL(doPdfCtrl(const QString&, int)), this, SLOT(doCoursewareCtrl(const QString&, int)));
        connect(pShow, SIGNAL(showPdfPage(const QString&, int)),this,SLOT(doShowPDFPage(const QString&,int)));
        return pShow;
    } while (false);

    if (pShow)
    {
        delete pShow;
        pShow = NULL;
    }
    
    return NULL;
}

void QCoursewarePannel::SetMode(QString fileName, WBMode mode)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->setMode(mode);
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setMode(mode);
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setMode(mode);
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setMode(mode);
			}
		}
		break;
	}    
}

void QCoursewarePannel::SetColor(QString fileName, WBColor clr)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->setColor(clr);
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setColor(clr);
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setColor(clr);
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setColor(clr);
			}
		}
		break;
	}    
}

void QCoursewarePannel::SetTextSize(QString fileName, WBSize tSize)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->setTextSize(tSize);
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setTextSize(tSize);
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setTextSize(tSize);
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setTextSize(tSize);
			}
		}
		break;
	}    
}

void QCoursewarePannel::SetEnable(QString fileName, WBCtrl enable)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->setEnable(enable);
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setEnable(enable);
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setEnable(enable);
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->setEnable(enable);
			}
		}
		break;
	}    
}

void QCoursewarePannel::UnDo(QString fileName)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->undo();
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->undo();
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->undo();
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->undo();
			}
		}
		break;
	}    
}

void QCoursewarePannel::Clear(QString fileName)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}
			QWhiteBoardView* pWhite = pShow->_show._pdf->getCurWhiteBoardView();
			if (pWhite)
			{
				pWhite->clear();
			}
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._image->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->clear();
			}
		}
		break;
	case COURSEWARE_VIDEO:
		{
			if(NULL == pShow->_show._media)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._media->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->clear();
			}
		}
		break;
	case COURSEWARE_FLASH:
		{
			if(NULL == pShow->_show._flash)
			{
				break;
			}

			QWhiteBoardView* pWhite = pShow->_show._flash->getWhiteBoardView();
			if (pWhite)
			{
				pWhite->clear();
			}
		}
		break;
	}    
}

void QCoursewarePannel::ZoomIn(QString fileName)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
	case COURSEWARE_DOC:
	case COURSEWARE_EXCLE:
	case COURSEWARE_PPT:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}

			pShow->_show._pdf->zoomIn();
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			pShow->_show._image->zoomIn();
		}
		break;
	default:
		break;
	}
}

void QCoursewarePannel::ZoomOut(QString fileName)
{
	LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
	if(NULL == pShow)
	{
		return;
	}

	switch(pShow->_type)
	{
	case COURSEWARE_PDF:
	case COURSEWARE_DOC:
	case COURSEWARE_EXCLE:
	case COURSEWARE_PPT:
		{
			if(NULL == pShow->_show._pdf)
			{
				break;
			}

			pShow->_show._pdf->zoomOut();
		}
		break;
	case COURSEWARE_IMG:
		{
			if(NULL == pShow->_show._image)
			{
				break;
			}

			pShow->_show._image->zoomOut();
		}
		break;
	default:
		break;
	}
}

void QCoursewarePannel::GotoPage(QString fileName, int nType, int nPage)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
    if (pShow)
    {
        switch (pShow->_type)
        {
        case COURSEWARE_PDF:
        case COURSEWARE_DOC:
        case COURSEWARE_EXCLE:
        case COURSEWARE_PPT:
            {
              
				switch (nType)
                {
                case CTRL_NEXT_PAGE:
                    {
                        if(NULL == pShow->_show._pdf)
                        {
                            break;
                        }
                        pShow->_show._pdf->nextPage();
                    }
                    break;

                case CTRL_PREV_PAGE:
                    {
                        if(NULL == pShow->_show._pdf)
                        {
                            break;
                        }
                        pShow->_show._pdf->prevPage();
                    }
                    break;

                case CTRL_LAST_PAGE:
                    {

                    }
                    break;

                case CTRL_HOME_PAGE:
                    {

                    }
                    break;

                case CTRL_SEEK_PAGE:
                    {
                        if(NULL == pShow->_show._pdf)
                        {
                            break;
                        }
                        if (nPage > 0)
                        {
                            pShow->_show._pdf->gotoPage(nPage);
                        }
                    }
                    break;
                }
            }
            break;
        case COURSEWARE_VIDEO:
        case COURSEWARE_AUDIO:
            {
                if (NULL == pShow->_show._media)
                {
                    break;
                }
                pShow->_show._media->seek(nPage);
				
				Util::PrintTrace("media file seek,file : %s, total : %d ,cur pos: %d ,seek pos: %d",pShow->_show._media->getFile().c_str(),
					pShow->_show._media->getTotalPlayTime(),pShow->_show._media->getCurPlayTime(),nPage);
            }
            break;
        }
    }
}

int QCoursewarePannel::GetTotalPage(QString fileName)
{
    LPCOURSEWARESHOW pShow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
    if (pShow && pShow->_show._pdf)
    {
        return pShow->_show._pdf->getPageCount();
    }
    return 0;
}

void QCoursewarePannel::doCoursewareCtrl(const QString &fileName, int nCtrl)
{
    if (fileName.isEmpty())
    {
        return;
    }

    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }

    OutputDebugStringW(L"Lock doCoursewareCtrl.\n");

    QMutexLocker AutoLock(&m_mutex);
    LPCOURSEWARESHOW pShow = NULL;
    size_t i = 0;
    for (; i < m_vecCourShow.size(); i++)
    {
        pShow = m_vecCourShow.at(i);
        if (NULL == pShow)
        {
            continue;
        }
        if(pShow->_fileName == fileName)
        {
            break;
        }
        pShow = NULL;
    }
    if (NULL == pShow)
    {
        AutoLock.unlock();
        return;
    }

    biz::SLClassRoomShowInfo sShowInfo;
    sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;

    switch (pShow->_type)
    {
    case COURSEWARE_PDF:
        {
			if(NULL == pShow->_show._pdf)
			{
				break;
			}

            sShowInfo._nShowType = biz::eShowType_Cursewave;
            sShowInfo._nShowPage = pShow->_show._pdf->getCurPage();
            WCHAR wszUpFile[MAX_PATH+1] = {0};
            GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pShow->_fileName).unicode()).data(), wszUpFile, MAX_PATH);
            wcscpy(sShowInfo._szShowName, wszUpFile);

			if(ClassRoomDialog::isValid())
			{
				//2018.10.25 xiewb add later
				//ClassRoomDialog::getInstance()->setCursewarePageShow(pShow);
			}
        }
        break;

    case COURSEWARE_IMG:
        {

        }
        break;

    default: 
        break;
    }
    AutoLock.unlock();
    biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
}

void QCoursewarePannel::doShowPDFPage(const QString& fileName, int nPage)
{
    if (fileName.isEmpty() || nPage < 0)
    {
        return;
    }
    emit setShowPage(nPage+1);
}

 MediaPlayerShowWnd* QCoursewarePannel::OpenMediaFile(QString fileName,bool onlyOpen /* = false */)
{	
    if (fileName.isEmpty())
    {
        return NULL;
    }

    MediaPlayerShowWnd* pShow = NULL;
    bool bopen = false;

    do 
    {        
        pShow = new MediaPlayerShowWnd(NULL);
        if (NULL == pShow)
        {            
            break;
        }

		/****************************************************************
         2018.10.25 xiewb add later
		*****************************************************************/
		if(ClassRoomDialog::isValid())
		{
			ClassRoomDialog::getInstance()->addMainView(pShow);
		}

        std::string strfile;
        Util::QStringToString(fileName, strfile);
        bopen = pShow->play(strfile,onlyOpen);
        if (!bopen)
        {            
            break;
        }

        connect((CMediaFilePlayer*)pShow, SIGNAL(playPosChange(unsigned int,string&)), this, SLOT(doMediaPlayProgress(unsigned int,string&)));

		pShow->setUserId(ClassSeeion::GetInst()->_nUserId);
		pShow->setCourseId(ClassSeeion::GetInst()->_nClassRoomId);
		if (ClassSeeion::GetInst()->IsTeacher())
		{
			pShow->setWbCtrl(WB_CTRL_EVERY);
		}
		else
		{
			pShow->setWbCtrl(WB_CTRL_NONE);
		}

		pShow->createWhiteboardView();

        return pShow;

    } while (false);

    if(NULL != pShow)
    {
        delete pShow;
        pShow = NULL;
    }

    return NULL;
}

bool QCoursewarePannel::MediaCtrl(QString fileName, int nCtrl, int nSeek)
{
    bool bResult = false;
    int nState = nCtrl;
    LPCOURSEWARESHOW pshow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
    if (pshow && (COURSEWARE_AUDIO == pshow->_type || COURSEWARE_VIDEO == pshow->_type) && pshow->_show._media)
    {
        switch (nCtrl)
        {
        case CMediaFilePlayer::PLAY:
            {
                bResult = pshow->_show._media->pause(false);

				/*
				bResult = true;
				*/
            }
            break;

        case CMediaFilePlayer::PAUSE:
            {
                bResult = pshow->_show._media->pause(true); 
				UINT curPos = pshow->_show._media->getCurPlayTime(true);				
				nState |= (curPos << 4);
				pshow->_show._media->seekEx(curPos);

                /*
				bResult = true;
				*/
            }
            break;

        case CMediaFilePlayer::STOP:
            {
                pshow->_show._media->stop();
                bResult = true;
            }
            break;

        case CMediaFilePlayer::SEEK: //表示seek
            {
				bResult = pshow->_show._media->seek(nSeek);	
				
				//xiewb 2018.11.19
				int total = pshow->_show._media->getTotalPlayTime();
				int maxOff = (total - nSeek) > 200 ? 200 : 0;
				nSeek += maxOff;
				
				nState |= (nSeek << 4);
            }
            break;
        }
    }

    if (bResult && ClassSeeion::GetInst()->IsTeacher() && nCtrl != CMediaFilePlayer::SEEK)  //SEEK不用同步，通过Player的定时器同步
    {
        biz::SLClassRoomShowInfo sShowInfo;
        sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
        sShowInfo._nShowType = biz::eShowType_Cursewave;
        WCHAR szFile[MAX_PATH+1] = {0};
        GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pshow->_fileName).unicode()).data(), szFile, MAX_PATH);
        wcscpy(sShowInfo._szShowName, szFile);
        sShowInfo._nShowPage = nState;
        biz::GetBizInterface()->ChangeMyShowType(sShowInfo);
    }

    return bResult;
}

unsigned int QCoursewarePannel::GetMediaState(QString fileName)
{
    LPCOURSEWARESHOW pshow = GetCurrentCoursewareShow(); //GetCoursewareShowByFileName(fileName);
    if (pshow && (COURSEWARE_AUDIO == pshow->_type || COURSEWARE_VIDEO == pshow->_type) && pshow->_show._media)
    {
        return pshow->_show._media->getState();
    }
    return 0;
}

void QCoursewarePannel::doMediaPlayProgress(unsigned int nPos,string& fileName)
{
    emit setMediaPlayerPro(nPos,fileName);
    
	if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    
    static int nSyncTime = 0; //同步标记(每5s发送一次音视频课件同步)
    nSyncTime++;

    if (0 != nSyncTime%2)
    {
        return;
    }

    LPCOURSEWARESHOW pshow = GetCurrentCoursewareShow();
	if(NULL == pshow || NULL == pshow->_show._media)
	{
		return;
	}

	if( pshow->_type != COURSEWARE_AUDIO && COURSEWARE_VIDEO != pshow->_type)
	{
		return;
	}
	
	string cur_file = pshow->_show._media->getFile();
	if(cur_file!=fileName)
	{
		return;
	}
	
    biz::SLClassRoomShowInfo sShowInfo;
    sShowInfo._nRoomId = ClassSeeion::GetInst()->_nClassRoomId;
    sShowInfo._nShowType = biz::eShowType_Cursewave;
    WCHAR szFile[MAX_PATH+1] = {0};
    GetCoursewareUploadFileName((wchar_t*)wstring((wchar_t*)(pshow->_fileName).unicode()).data(), szFile, MAX_PATH);
    wcscpy(sShowInfo._szShowName, szFile);

	unsigned int pstate = pshow->_show._media->getState();
	if(pstate == CMediaFilePlayer::PLAY)
    {
		sShowInfo._nShowPage = CMediaFilePlayer::SEEK;
	}
	else
	{
		sShowInfo._nShowPage = pstate;
	}
	
	//xiewb 2017.08.19
	int total = pshow->_show._media->getTotalPlayTime();
	int maxOff = (total - nPos) > 200 ? 200 : 0;
	nPos += maxOff;

	sShowInfo._nShowPage |= (nPos << 4);
    biz::GetBizInterface()->ChangeMyShowType(sShowInfo);

	Util::PrintTrace("chang show play pos,file :%s,length : %d,pos : %d",fileName.c_str(),total,nPos);
}

QImageShow* QCoursewarePannel::OpenImageFile(QString fileName)
{
    if (fileName.isEmpty())
    {
        return NULL;
    }

    QImageShow  *pImage = NULL;
    bool bOpen = false;
    do 
    {
        pImage = new QImageShow();
        if (NULL == pImage)
        {
            return NULL;
        }

		/****************************************************************
		 2018.10.25 xiewb add later
		*****************************************************************/
		if(ClassRoomDialog::isValid())
		{
			ClassRoomDialog::getInstance()->addMainView(pImage);
		}

        bOpen = pImage->openFile(fileName);
        if (!bOpen)
        {
            break;
        }

		pImage->setUserId(ClassSeeion::GetInst()->_nUserId);
		pImage->setCourseId(ClassSeeion::GetInst()->_nClassRoomId);
		if (ClassSeeion::GetInst()->IsTeacher())
		{
			pImage->setWbCtrl(WB_CTRL_EVERY);
		}
		else
		{
			pImage->setWbCtrl(WB_CTRL_NONE);
		}

		pImage->createWhiteboardView();

        return pImage;

    } while (false);
   
    SAFE_DELETE(pImage);    
    return NULL;
}

QFlashPlayWidget* QCoursewarePannel::OpenFlashFile(QString fileName)
{
    if (fileName.isEmpty())
    {
        return NULL;
    }

    QFlashPlayWidget  *pFlash = NULL;
    bool bOpen = false;
    do 
    {
        pFlash = new QFlashPlayWidget(ClassRoomDialog::getInstance());
        if (NULL == pFlash)
        {
            return NULL;
        }
				
		//2018.10.25 xiewb add for later
		if(ClassRoomDialog::isValid())
		{
			ClassRoomDialog::getInstance()->addMainView(pFlash);
		}

        bOpen = pFlash->play(fileName);
        if (!bOpen)
        {
            break;
        }

		pFlash->setUserId(ClassSeeion::GetInst()->_nUserId);
		pFlash->setCourseId(ClassSeeion::GetInst()->_nClassRoomId);
		if (ClassSeeion::GetInst()->IsTeacher())
		{
			pFlash->setWbCtrl(WB_CTRL_EVERY);
		}
		else
		{
			pFlash->setWbCtrl(WB_CTRL_NONE);
		}

		pFlash->createWhiteboardView();

        return pFlash;

    } while (false);
   
    SAFE_DELETE(pFlash);    
    return NULL;
}


