#include "UserSeatVideoWnd.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
#include <QtCore/QCoreApplication>
#include "MediaPublishMgr.h"

//UserSeatVideoWnd::UserSeatVideoWnd(QWidget* parent):QWidget(parent),VideoShowBase(VideoType_Tea)
UserSeatVideoWnd::UserSeatVideoWnd(QWidget* parent):OpenGLVideoWidget(parent,VIDEO_TEACHER)
{
	m_seat = -1;
	
	/*
	m_refreshTimerId = 0;
	m_paintTimerId = this->startTimer(TIME_UPDATE_VIDEO,Qt::PreciseTimer);
	*/

	CMediaPublishMgr* mgrMediaPublish = CMediaPublishMgr::getInstance();
	if (NULL!=mgrMediaPublish)
	{		
		connect(mgrMediaPublish,SIGNAL(updatedPublishSeat(int)),this,SLOT(onUpdatePublishSeat(int)));
	}
}

UserSeatVideoWnd::~UserSeatVideoWnd()
{
	CMediaPublishMgr* mgrMediaPublish = CMediaPublishMgr::getInstance();
	if (NULL!=mgrMediaPublish)
	{
		disconnect(mgrMediaPublish,NULL,this,NULL);
	}
}

bool UserSeatVideoWnd::setSeatIndex(int nIndex)
{
	if(m_seat == nIndex)
	{
		return true;
	}

	CMediaPublishMgr* mgrMediaPublish = CMediaPublishMgr::getInstance();
	if (NULL==mgrMediaPublish)
	{
		return false;
	}

	LPPUBLISHSEATINFO seatInfo = mgrMediaPublish->getPublishSeatInfo(nIndex);
	if(NULL != seatInfo && NULL != seatInfo->_rtmp)
	{
		this->setRtmpStream(seatInfo->_rtmp);
	}
    else
    {
        this->setRtmpStream(NULL);
    }

    m_seat = nIndex;

	return true;
}

void UserSeatVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	//QMutexLocker autoLock(&m_mutexVideoBuf);
	//copyAndTransVideoData(m_showVideoBuf,videoData);

	showVideoBuffer(videoData.videoWidth,videoData.videoHeight,videoData.isYUVData,videoData.buffsize,videoData.videoBuff);
}

/*
void UserSeatVideoWnd::paintEvent(QPaintEvent * event)
{
	if(!this->isVisible())
	{
		return;
	}

	QPainter    painter(this);
	QRect       rectWnd = this->rect();
	
	bool br = paintVideoBuff(&painter,rectWnd,m_showVideoBuf,&m_mutexVideoBuf);
	if(br)
	{
		killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
	}
	else
	{
		if(m_refreshTimerId <= 0)
		{
			m_refreshTimerId = this->startTimer(TIME_KEEP_VIDEO);
		}
	}

	return;
}
*/

void UserSeatVideoWnd::timerEvent(QTimerEvent * event)
{
	/*
	if(NULL == event)
	{
		return;
	}

	int timerId = event->timerId();
	if(timerId == m_refreshTimerId)
	{		
		QMutexLocker autoLock(&m_mutexVideoBuf);
		m_showVideoBuf.zeroBuffer();
		
		this->killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
		
		return;
	}

	if(timerId == m_paintTimerId)
	{
		if(!this->isVisible())
		{
			return;
		}

		QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();

		return;
	}
	*/
	return;
}

void UserSeatVideoWnd::onUpdatePublishSeat(int seatIndex)
{
	if(m_seat != seatIndex)
	{
		return;
	}
	
	CMediaPublishMgr* mgrMediaPublish = CMediaPublishMgr::getInstance();
	if (NULL==mgrMediaPublish)
	{
		return;
	}

	LPPUBLISHSEATINFO seatInfo = mgrMediaPublish->getPublishSeatInfo(m_seat);
	if(NULL != seatInfo && NULL != seatInfo->_rtmp)
	{
		this->setRtmpStream(seatInfo->_rtmp);
	}
}