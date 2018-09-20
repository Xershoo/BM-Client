//**********************************************************************
//	Copyright ��c�� 2016,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaPlayerShowWnd.cpp
//	�汾�ţ�1.0
//	���ߣ� 
//	ʱ�䣺 
//	˵��������Ƶ�μ����Ŵ��ڷ�װ����ʵ��Դ�ļ�
//  ʹ��˵����
//	�޸ļ�¼��
//**********************************************************************

#include "MediaPlayerShowWnd.h"
#include <QtCore/QCoreApplication>

MediaPlayerShowWnd::MediaPlayerShowWnd(QWidget * parent) : CMediaFilePlayer(parent) 
    , VideoShowBase(VideoType_Courseware),m_paintTimerId(0)
{
    m_paintTimerId = this->startTimer(100);
}

void MediaPlayerShowWnd::showMediaData()
{   
    //update();
	QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);
}

void MediaPlayerShowWnd::timerEvent(QTimerEvent * event)
{
	if(NULL == event)
	{
		return;
	}

    CMediaFilePlayer::timerEvent(event);

	int timerId = event->timerId();
	if(timerId == m_paintTimerId)
	{
		if(!this->isVisible())
		{
			return;
		}

		showMediaData();
		return;
	}
}

MediaPlayerShowWnd::~MediaPlayerShowWnd()
{
    QMutexLocker autoLock(&m_mutexVideoBuf); 
    m_showVideoBuf.zeroBuffer();
}

void MediaPlayerShowWnd::paintEvent(QPaintEvent * event)
{
    QPainter    painter(this);
    QRect       rectWnd = this->rect();

    //���Ʊ���ͼƬ���׵�
    QBrush backBrush(QColor(247,2,2,2));
    painter.fillRect(rectWnd,backBrush);
    
    QRect rectVideo = this->rect();
    rectVideo = QRect(rectVideo.left()+1,rectVideo.top()+1,rectVideo.width()-2,rectVideo.height()-2);

    if(rectVideo.width() <= 0 || rectVideo.height() <= 0)
    {
        return;
    }

    paintVideoBuff(&painter,rectVideo,m_showVideoBuf,&m_mutexVideoBuf);
}

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

    RtmpVideoBuf videoData(0,pData,nSize,nVideoW,nVideoH,true);
    QMutexLocker autoLock(&m_mutexVideoBuf); 
    copyAndTransVideoData(m_showVideoBuf,videoData);
}
