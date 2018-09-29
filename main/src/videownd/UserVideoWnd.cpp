//**********************************************************************
//	Copyright ��c�� 2015,����΢���Ƽ����޹�˾. All rights reserved.
//	�ļ����ƣ�UserVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.15
//	˵����RTMP�����Ŵ��ڷ�װ����ʵ���ļ�
//	�޸ļ�¼��
//      
//**********************************************************************
#include "UserVideoWnd.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include "common/log.h"

//UserVideoWnd::UserVideoWnd(QWidget * parent) : QWidget(parent)
UserVideoWnd::UserVideoWnd()
{
	this->setRtmpStream(&m_rtmpPlayer);	
	m_nPlayerId = 0;
	m_isDoStart = false;
}

void UserVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{	
	//showVideoBuffer(videoData.videoWidth,videoData.videoHeight,videoData.isYUVData,videoData.buffsize,videoData.videoBuff);
	return;
}

bool UserVideoWnd::start(void)
{
	LOG_INFO() << "�û���Ƶ���ڿ�ʼ����......"<<m_rtmpPlayer.getUrl().c_str()<<"\r\n";
    return m_rtmpPlayer.start();
}

bool UserVideoWnd::change(void)
{
    return m_rtmpPlayer.change();
}

void UserVideoWnd::stop(void)
{
	LOG_INFO() << "�û���Ƶ����ֹͣ����......"<<m_rtmpPlayer.getUrl().c_str()<<"\r\n";
	
	m_isDoStart = false;
    m_rtmpPlayer.stop();

	//repaint(true);
}
