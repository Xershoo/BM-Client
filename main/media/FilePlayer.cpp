//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�RtmpPlayer.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2016.01.27
//	˵������ý���ļ����ŷ�װ����ʵ��Դ�ļ�
//  �ӿڣ�
//
//	�޸ļ�¼��
//
//**********************************************************************
#include "FilePlayer.h"
#include <QtCore/QEvent>

CMediaFilePlayer::CMediaFilePlayer(QWidget* parent):QWidget(parent)
	,m_state(STOP)
	,m_fileName("")
	,m_totalPlayTime(0)
{
    m_bShow = false;
    m_idTimePlay = 0;
};

CMediaFilePlayer::~CMediaFilePlayer()
{
	stop();
}

bool CMediaFilePlayer::switchPlayFile(const string& curPlayFile)
{
	if(curPlayFile.empty())
	{
		return false;
	}

	return ::playFileSwitch(curPlayFile.c_str());
}

bool CMediaFilePlayer::play(const string& file,bool onlyOpen /* = false */)
{
	if(stricmp(file.c_str(),m_fileName.c_str())==0)
	{
		bool ret = true;
		if(m_state == STOP)
		{	
            ::playFileSwitch(m_fileName.c_str());
			ret = ::AVP_PlayFile(m_fileName.c_str(),(HWNDHANDLE)this,NULL,this);
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

	bool ret = ::AVP_PlayFile(file.c_str(),(HWNDHANDLE)this,NULL,this);
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

    if(onlyOpen)
    {
        ::AVP_PauseFile(m_fileName.c_str(),true);
        m_state = PAUSE;
    }

	return true;
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

bool CMediaFilePlayer::pause(bool isPause)
{
	if(m_state == STOP)
	{
        return play(m_fileName);
	}

	if( m_state == (isPause ? PAUSE : PLAY))
	{
		return true;
	}
    ::playFileSwitch(m_fileName.c_str());
	bool ret = ::AVP_PauseFile(m_fileName.c_str(),isPause);
	if(!ret)
	{
		return false;
	}
		
	m_state = isPause ? PAUSE : PLAY;

    if (PAUSE == m_state)
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

	m_state = STOP;	
	m_idTimePlay = 0;
	
	return bRes;
}

bool CMediaFilePlayer::seek(unsigned int pos)
{
	if(m_state == STOP)
	{        
		return play(m_fileName);
	}
	else if(m_state == PAUSE)
	{
		pause(false);
	}
	
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

unsigned int CMediaFilePlayer::getCurPlayTime()
{
	if(m_fileName.empty())
	{
		return 0;
	}

	return ::getFileCurPlayTime(m_fileName.c_str());
}

void CMediaFilePlayer::ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH)
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

	RtmpVideoBuf videoData(0,pData,nSize,nVideoW,nVideoH,true);

	//ͼ�����ݴ���
	for(int i= 0; i<nVideoH*nVideoW;i++)
	{
		unsigned char * bufSrc = videoData.videoBuff + i*3;
		unsigned char * bufDec = pData + i*3;;

		*(bufSrc) = *(bufDec+2);
		*(bufSrc+1) = *(bufDec+1);
		*(bufSrc+2) = *(bufDec);
	}

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
		if(m_state == STOP)
		{
			return;
		}

		unsigned int curPlay = getCurPlayTime();
		unsigned int totPlay = getTotalPlayTime();
		if(curPlay == totPlay)
		{
			stop();
		}

		emit playPosChange(curPlay);

		return;
	}
}


void CMediaFilePlayer::Show(bool bShow)
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
