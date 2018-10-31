//**********************************************************************
//	Copyright （c） 2016,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaPlayerShowWnd.cpp
//	版本号：1.0
//	作者： 
//	时间： 
//	说明：音视频课件播放窗口封装的类实现源文件
//  使用说明：
//	修改记录：
//**********************************************************************

#include "MediaPlayerShowWnd.h"
#include "courseware\courseware.h"
#include "common/Util.h"
#include <QtCore/QCoreApplication>
#include "./whiteboard/QWhiteBoard.h"
#include "./common/macros.h"

MediaPlayerShowWnd::MediaPlayerShowWnd(QWidget * parent) : CMediaFilePlayer(parent)
	, m_isVideo(false)
	, m_viewWhiteboard(NULL)
	, m_userId(0)
	, m_courseId(0)	
	, m_wbCtrl(WB_CTRL_NONE)
{
    
}

MediaPlayerShowWnd::~MediaPlayerShowWnd()
{

};

void MediaPlayerShowWnd::ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
    CMediaFilePlayer* userHandle = (CMediaFilePlayer*)ShowHandle;
    if(userHandle != this)
    {
        return;
    }

    if(NULL == pData || 0 >= nSize || 0 >= nVideoW || 0 >= nVideoH)
    {
        return;
    }

	if(m_isVideo)
	{
		showVideoBuffer(nVideoW,nVideoH,true,nSize,pData);
	}
}

bool MediaPlayerShowWnd::play(const string& file,bool onlyOpen/* = false*/)
{
	WCHAR wszFile[MAX_PATH]={0};
	Util::AnsiToUnicode(file.c_str(),file.length(),wszFile,MAX_PATH);
	int ftype = ::GetCoursewareFileType(wszFile);

	switch(ftype)
	{
	case COURSEWARE_AUDIO:
		m_isVideo = false;
		break;
	case COURSEWARE_VIDEO:
		m_isVideo = true;
		break;
	default:
		return false;		
	}
	setAutoShowBack(false);
	setWhiteboardViewBack();

	return CMediaFilePlayer::play(file,onlyOpen);
}

void MediaPlayerShowWnd::createWhiteboardView()
{
	if (!m_isVideo)
	{
		return;
	}

	__int64 idOwner = (__int64)calcFileId(); 
	m_viewWhiteboard = new QWhiteBoardView(this,false);
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	m_viewWhiteboard->setUserId(m_userId);
	m_viewWhiteboard->setPageId(0);
	m_viewWhiteboard->setCourseId(m_courseId);
	m_viewWhiteboard->setOwnerId(idOwner);
	m_viewWhiteboard->show();
	m_viewWhiteboard->installEventFilter(this);
	m_viewWhiteboard->setEnable((WBCtrl)m_wbCtrl);
	m_viewWhiteboard->setColor(WB_COLOR_BLUE);
	m_viewWhiteboard->setMode(WB_MODE_CURVE);
	m_viewWhiteboard->setTextSize(WB_SIZE_SMALL);
}

int MediaPlayerShowWnd::calcFileId()
{
	int nId = 0;
	char szFile[512] = { 0 };
	strcpy_s(szFile,getFile().c_str());

	if(strlen(szFile) <= 0)
	{
		return 0;
	}

	char * pszFileName = strrchr(szFile,'\\');
	if(NULL != pszFileName)
	{
		nId = (int)Util::crc32((unsigned char*)pszFileName,strlen(pszFileName));
	}

	return nId;
}

void MediaPlayerShowWnd::setWhiteboardViewBack()
{
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	QRect rectWidget= this->rect();
	m_viewWhiteboard->setSizeAndSceneRect(rectWidget); 	
	m_viewWhiteboard->setBackColor(QColor(255,255,255,0));
	m_viewWhiteboard->show();
	m_viewWhiteboard->update();
}

void MediaPlayerShowWnd::resizeEvent(QResizeEvent *event)
{	
	SAFE_DELETE(m_renderImage);
	setWhiteboardViewBack();
	CMediaFilePlayer::resizeEvent(event);
}

bool MediaPlayerShowWnd::eventFilter(QObject * obj, QEvent * event)
{
	if(obj != m_viewWhiteboard)
	{
		return false;
	}

	if(event->type() == QEvent::KeyPress)
	{
		keyPressEvent((QKeyEvent*)event);
		m_viewWhiteboard->setFocus();
	}

	return false;
}

unsigned int MediaPlayerShowWnd::getCurPlayTime(bool video /* = false */)
{
	if(m_fileName.empty())
	{
		return 0;
	}

	if(video && m_isVideo)
	{
		return ::getFileStreamCurTime(m_fileName.c_str(),true);
	}

	return ::getFileCurPlayTime(m_fileName.c_str());
}


bool MediaPlayerShowWnd::seekEx(unsigned int pos)
{
	bool br = false;
	unsigned int curPlay = 0;	
	if(m_isVideo)
	{		
		br = ::AVP_SeekFileStream(m_fileName.c_str(),pos,true);
	}
	else
	{
		curPlay = getCurPlayTime();
		if(curPlay == pos)
		{
			return true;
		}

		br = ::AVP_SeekFile(m_fileName.c_str(),pos);
	}
	
	if(br)
	{
		updateScene();
	}

	return br;
}