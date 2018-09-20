//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：TeacherPublishVideoWnd.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.15
//	说明：老师RTMP视频流发布预览窗口封装的类实现源文件
//  使用说明：
//
//	修改记录：
//      
//**********************************************************************
#include "TeacherPublishVideoWnd.h"
#include "MediaPublishMgr.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QtEvents>
#include <QtCore/QCoreApplication>
#include "biz/BizInterface.h"
#include "session/classsession.h"

TeacherPublishVideoWnd::TeacherPublishVideoWnd(QWidget * parent) : QWidget(parent)
{
    CMediaPublishMgr * mgrMediaPublish = CMediaPublishMgr::getInstance();
    if(NULL != mgrMediaPublish)
    {
		connect(mgrMediaPublish,SIGNAL(updatedPublishSeat(int)),this,SLOT(onUpdatePublishSeat(int)));
    }
	
	memset(m_refreshTimerId,0,sizeof(int) * MULTI_SHOW_CHANNEL_NUM);
	m_paintTimerId = startTimer(TIME_UPDATE_VIDEO,Qt::PreciseTimer);
	
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM - 1;i++)
	{
		m_showTeaVideo[i]=new SingleTeacherVideoShow(this,i);		
	}
}

TeacherPublishVideoWnd::~TeacherPublishVideoWnd()
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM - 1;i++)
	{
		if(m_showTeaVideo[i])
		{
			delete m_showTeaVideo[i];
			m_showTeaVideo[i] = NULL;
		}
	}
}

void TeacherPublishVideoWnd::paintEvent(QPaintEvent * event)
{
	if(!this->isVisible())
	{
		return;
	}

    QPainter    painter(this);
    QRect       rectWnd = this->rect();

    //绘制背景图片，白底
    QBrush backBrush(QColor(218,218,218,255));
    painter.fillRect(rectWnd,backBrush);

    for (int i= 0; i<MULTI_SHOW_CHANNEL_NUM;i++)
    {   
        bool br = drawVideo(&painter,i);
		if(br)
		{
			killTimer(m_refreshTimerId[i]);
			m_refreshTimerId[i] = 0;
		}
		else
		{
			if(m_refreshTimerId[i] <= 0)
			{
				m_refreshTimerId[i] = this->startTimer(TIME_KEEP_VIDEO);
			}
		}
    }
}

void TeacherPublishVideoWnd::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(!m_setMultiShow)
    {
        return;
    }

    if(NULL == event)
    {
        return;
    }

    if(event->button() != Qt::LeftButton)
    {
        return;
    }

    QPoint pos = event->pos();
    for(int i= 0; i<MULTI_SHOW_CHANNEL_NUM-1;i++)
    {   
        QRect rectVideo = calcVideoRect(i);
        if(rectVideo.contains(pos))
        {
            m_setMultiShow = false;
            CMediaPublishMgr::getInstance()->setMainSeat(i);
			
			biz::GetBizInterface()->SwitchTeacherVideo(ClassSeeion::GetInst()->_nClassRoomId,
				ClassSeeion::GetInst()->_nUserId,i);
				
			//update();
			QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);

			return;
        }
    }
}

QRect TeacherPublishVideoWnd::getVideoShowRect()
{
    return this->rect();
}

int TeacherPublishVideoWnd::getMainVideoIndex()
{
    return CMediaPublishMgr::getInstance()->getMainSeat();
}

void TeacherPublishVideoWnd::setStudentVideoStream(CRTMPStream* rtmpStream,bool set)
{
	MultiChannelVideoShow::setStudentVideoStream(rtmpStream,set);
    //update();
	QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);
}

void TeacherPublishVideoWnd::timerEvent(QTimerEvent * event)
{
	if(NULL == event)
	{
		return;
	}
	int timerId = event->timerId();
	if(timerId == m_paintTimerId)
	{
		if(!this->isVisible())
		{
			return;
		}

		//update();
		QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);

		return;
	}

	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++)
	{
		if(timerId == m_refreshTimerId[i])
		{			
			QMutexLocker autoLock(&m_mutexVideoBuf[i]);
			m_showVideoBuf[i].zeroBuffer();

			this->killTimer(m_refreshTimerId[i]);
			m_refreshTimerId[i] = 0;

			return;
		}		
	}

	return;
}

void TeacherPublishVideoWnd::onUpdatePublishSeat(int seatIndex)
{
	if(seatIndex <0 || seatIndex >= MULTI_SHOW_CHANNEL_NUM - 1)
	{
		return;
	}

	if(!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	CMediaPublishMgr * mgrMediaPublish = CMediaPublishMgr::getInstance();
	if(NULL == mgrMediaPublish)
	{
		return;
	}

	SingleTeacherVideoShow* videoShow = m_showTeaVideo[seatIndex];
	if(NULL == videoShow)
	{
		return;
	}
		
	LPPUBLISHSEATINFO seatInfo = mgrMediaPublish->getPublishSeatInfo(seatIndex);
	if(NULL == seatInfo || NULL == seatInfo->_rtmp)
	{
		return;
	}

	videoShow->setRtmpStream(seatInfo->_rtmp);
	
}

//////////////////////////////////////////////////////////////////////////
//
void SingleTeacherVideoShow::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	if(NULL == m_wndTeaPubVideo)
	{
		return;
	}

	RtmpVideoBuf teaVideoBuf = videoData;
	teaVideoBuf.videoIndex = m_videoIndex;
	m_wndTeaPubVideo->recvTeacherVideoData(teaVideoBuf);
}
