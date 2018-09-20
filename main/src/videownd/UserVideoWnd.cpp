//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
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

UserVideoWnd::UserVideoWnd(QWidget * parent) : QWidget(parent)
{
	this->setRtmpStream(&m_rtmpPlayer);	
	m_nPlayerId = 0;
}

void UserVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	return;
}

bool UserVideoWnd::start(void)
{
    return m_rtmpPlayer.start();
}

bool UserVideoWnd::change(void)
{
    return m_rtmpPlayer.change();
}

void UserVideoWnd::stop(void)
{
    m_rtmpPlayer.stop();
}
