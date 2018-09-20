//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�TeacherVideoWnd.cpp
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.14
//	˵������ʦRTMP�����Ŵ��ڷ�װ����ʵ��Դ�ļ�
//
//	�޸ļ�¼��
//      
//**********************************************************************
#include "TeacherVideoWnd.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
#include <QtCore/QCoreApplication>

TeacherVideoWnd::TeacherVideoWnd(QWidget * parent) : UserVideoWnd(parent)    
{
    m_rtmpPlayer.setStudent(false);
    m_rtmpPlayer.setPlayAudio(true);

	m_paintTimerId = startTimer(TIME_UPDATE_VIDEO,Qt::PreciseTimer);
	memset(m_refreshTimerId,0,sizeof(int) * MULTI_SHOW_CHANNEL_NUM);
}

void TeacherVideoWnd::paintEvent(QPaintEvent * event)
{
	if(!this->isVisible())
	{
		return;
	}

    QPainter    painter(this);
    QRect       rectWnd = this->rect();

    //���Ʊ���ͼƬ���׵�
    QBrush backBrush(QColor(247,247,247,255));
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

QRect TeacherVideoWnd::getVideoShowRect()
{
    return this->rect();
}

int TeacherVideoWnd::getMainVideoIndex()
{
    return m_rtmpPlayer.getMainVideo();
}

void TeacherVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	MultiChannelVideoShow::recvTeacherVideoData(videoData);
}

void TeacherVideoWnd::setStudentVideoStream(CRTMPStream* rtmpStream,bool set)
{
	MultiChannelVideoShow::setStudentVideoStream(rtmpStream,set);
    QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();
}

void TeacherVideoWnd::timerEvent(QTimerEvent * event)
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

		QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();
		
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

