//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：RtmpPlayer.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2016.01.27
//	说明：多媒体文件播放封装的类实现源文件
//  接口：
//
//	修改记录：
//
//**********************************************************************
#include "FilePlayer.h"
#include <QtCore/QEvent>
#include <math.h>
#include "./common/Util.h"

CMediaFilePlayer::CMediaFilePlayer(QWidget* parent):OpenGLVideoWidget(parent,VIDEO_COURSEWARE)
	,m_state(STOP)
	,m_fileName("")
	,m_totalPlayTime(0)
{
    m_bShow = false;
    m_idTimePlay = 0;
}

CMediaFilePlayer::~CMediaFilePlayer()
{
	stop();
}

bool CMediaFilePlayer::play(const string& file,bool onlyOpen /* = false */)
{
	bool ret = true;
	
	if(stricmp(file.c_str(),m_fileName.c_str())==0)
	{	
		if(onlyOpen)
		{
			return true;
		}

		if(m_state == STOP)
		{   
			ret = ::AVP_PlayFile(m_fileName.c_str(),(HWNDHANDLE)this,NULL,this);
		}
		else if( m_state == PAUSE)
		{
			ret = ::AVP_PauseFile(m_fileName.c_str(),false);
		}
	}
	else
	{
		if(::bIsExistVideoStream(file.c_str()))
		{
			return false;
		}

		if(!onlyOpen)
		{
			ret = ::AVP_PlayFile(file.c_str(),(HWNDHANDLE)this,NULL,this);
		}
		else
		{
			m_fileName = file;
			return true;
		}
	}

	if(!ret)
	{
		return false;
	}

	m_totalPlayTime = AVP_GetFileDuration(file.c_str());
	m_fileName = file;
	m_state = PLAY;
    
	if (0 == m_idTimePlay)
    {
        m_idTimePlay = this->startTimer(1000,Qt::CoarseTimer);
    }
	/*
    if(onlyOpen)
    {
        ::AVP_PauseFile(m_fileName.c_str(),true);
        m_state = PAUSE;
    }
	*/
	return true;
}

bool CMediaFilePlayer::pause(bool isPause)
{
	if (isPause)
	{
		if (0 != m_idTimePlay)
		{
			this->killTimer(m_idTimePlay);
			m_idTimePlay = 0;
		}
	}
	else 
	{
		if (0 == m_idTimePlay)
		{
			m_idTimePlay = this->startTimer(1000, Qt::CoarseTimer);
		}		
	}

	if(m_state == STOP)
	{		
		if(!::AVP_PlayFile(m_fileName.c_str(),(HWNDHANDLE)this,NULL,this))
		{
			return false;
		}

		m_totalPlayTime = AVP_GetFileDuration(m_fileName.c_str());

		m_state = PLAY;
		return true;
	}

	if( m_state == (isPause ? PAUSE : PLAY))
	{
		return true;
	}

	if(!isPause)
    {
		::playFileSwitch(m_fileName.c_str());
	}
	else
	{
		::playFileSwitch("");
	}

	bool ret = ::AVP_PauseFile(m_fileName.c_str(),isPause);
	if(!ret)
	{
		return false;
	}
		
	m_state = isPause ? PAUSE : PLAY;

	return true;
}

bool CMediaFilePlayer::stop()
{
	if(m_state == STOP)
	{
		return false;
	}

	bool bRes = ::AVP_StopFile(m_fileName.c_str(),this);	
    if (0 != m_idTimePlay)
    {
        this->killTimer(m_idTimePlay);
        m_idTimePlay = 0;
    }

	::playFileSwitch("");

	m_state = STOP;	
	m_idTimePlay = 0;
	
	return bRes;
}

bool CMediaFilePlayer::seek(unsigned int pos,bool force /* = false */)
{
	bool br = true;
	if(m_state == STOP)
	{        
		br = play(m_fileName);
	}
	else if(m_state == PAUSE)
	{
		br = pause(false);
	}

	if(!br)
	{
		return false;
	}
	
	unsigned int curPlay = getCurPlayTime();
	if(curPlay == pos)
	{
		return true;
	}

	if(::abs((int)(curPlay - pos)) <= 2000 && !force)
	{
		return true;
	}

	Util::PrintTrace("CMediaFilePlayer::seek %d",pos);

	return ::AVP_SeekFile(m_fileName.c_str(),pos);
}

unsigned int CMediaFilePlayer::getTotalPlayTime()
{
	if (0==m_totalPlayTime)
	{
		m_totalPlayTime =  ::AVP_GetFileDuration(m_fileName.c_str());
	}

	return m_totalPlayTime;
}

unsigned int CMediaFilePlayer::getCurPlayTime(bool video /* = false */)
{
	if(m_fileName.empty())
	{
		return 0;
	}

	return ::getFileCurPlayTime(m_fileName.c_str());
}

void CMediaFilePlayer::ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
{
// 	CMediaFilePlayer* userHandle = (CMediaFilePlayer*)ShowHandle;
// 	if(userHandle != this)
// 	{
// 		return;
// 	}
// 
// 	if(NULL == pData || 0 >= nSize || 0 >= nVideoW || 0 >= nVideoH)
// 	{
// 		return;
// 	}
// 
// 	showVideoBuffer(nVideoW,nVideoH,true,nSize,pData);
//
//	RtmpVideoBuf videoData(0,pData,nSize,nVideoW,nVideoH,true);
// 	for(int i= 0; i<nVideoH*nVideoW;i++)
// 	{
// 		unsigned char * bufSrc = videoData.videoBuff + i*3;
// 		unsigned char * bufDec = pData + i*3;;
// 
// 		*(bufSrc) = *(bufDec+2);
// 		*(bufSrc+1) = *(bufDec+1);
// 		*(bufSrc+2) = *(bufDec);
// 	}

	return;
}

void CMediaFilePlayer::timerEvent(QTimerEvent *tEvent)
{
	if(NULL == tEvent)
	{
		return;
	}

	int tid = tEvent->timerId();
	if(tid == m_idTimePlay)
	{
		if(m_state != PLAY)
		{
			return;
		}

		unsigned int curPlay = getCurPlayTime();
		unsigned int totPlay = getTotalPlayTime();
		if((curPlay/1000) == (totPlay/1000))
		{
			stop();
		}
		
		emit playPosChange(curPlay,m_fileName);

		return;
	}

	OpenGLVideoWidget::timerEvent(tEvent);
}

/*未使用的接口*/
void CMediaFilePlayer::pauseShow(bool bShow)
{
    if (m_bShow == bShow)
    {
        return;
    }

    if (bShow)
    {
        switch (m_state)
        {		
        case PLAY:
            {
				::playFileSwitch(m_fileName.c_str());
				if (0 == m_idTimePlay)
                {
                    m_idTimePlay = this->startTimer(1000, Qt::CoarseTimer);
                }
            }
            break;

        case STOP:
            {
                ::playFileSwitch(m_fileName.c_str());
                ::AVP_PauseFile(m_fileName.c_str(), true);
            }
            break;

        default:
            break;
        }
    }
    else
    {		
        ::playFileSwitch("");
		
		if (0 != m_idTimePlay)
        {
            this->killTimer(m_idTimePlay);
            m_idTimePlay = 0;
        }
    }

    m_bShow = bShow;
}

bool CMediaFilePlayer::switchPlayFile(const string& curPlayFile)
{
	if(curPlayFile.empty())
	{
		return false;
	}

	return ::playFileSwitch(curPlayFile.c_str());
}

bool CMediaFilePlayer::playSwitch(const string& file)
{
	if(stricmp(file.c_str(),m_fileName.c_str())==0)
	{
		bool ret = true;
		if(m_state == STOP)
		{	
			ret = ::playFileSwitch(m_fileName.c_str());
		}
		else if( m_state == PAUSE)
		{
			ret = ::AVP_PauseFile(m_fileName.c_str(),false);
		}

		if(ret)
		{
			m_state = PLAY;
		}

		return ret;
	}

	if(::bIsExistVideoStream(file.c_str()))
	{
		return false;
	}

	bool ret = ::playFileSwitch(file.c_str());
	if(!ret)
	{
		return false;
	}

	m_totalPlayTime = AVP_GetFileDuration(file.c_str());
	m_fileName = file;
	m_state = PLAY;

	if (0 == m_idTimePlay)
	{
		m_idTimePlay = this->startTimer(1000,Qt::CoarseTimer);
	}

	return true;
}
/************************************************************************/