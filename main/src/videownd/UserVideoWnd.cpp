//**********************************************************************
//	Copyright （c） 2015,北京微恩科技有限公司. All rights reserved.
//	文件名称：UserVideoWnd.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.15
//	说明：RTMP流播放窗口封装的类实现文件
//	修改记录：
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
	LOG_INFO() << "用户视频窗口开始启动......"<<m_rtmpPlayer.getUrl().c_str()<<"\r\n";
    return m_rtmpPlayer.start();
}

bool UserVideoWnd::change(void)
{
    return m_rtmpPlayer.change();
}

void UserVideoWnd::stop(void)
{
	LOG_INFO() << "用户视频窗口停止播放......"<<m_rtmpPlayer.getUrl().c_str()<<"\r\n";
	
	m_isDoStart = false;
    m_rtmpPlayer.stop();

	//repaint(true);
}
