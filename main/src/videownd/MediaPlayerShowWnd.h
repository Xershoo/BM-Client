//**********************************************************************
//	Copyright （c） 2016,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：MediaPlayerShowWnd.h
//	版本号：1.0
//	作者： 
//	时间： 
//	说明：音视频课件播放窗口封装的类定义头文件
//  使用说明：
//	修改记录：
//**********************************************************************

#pragma once

#include <QtWidgets/QWidget>
#include "VideoShowBase.h"
#include "FilePlayer.h"


class MediaPlayerShowWnd : public CMediaFilePlayer,
    public VideoShowBase
{
    Q_OBJECT
public:
    MediaPlayerShowWnd(QWidget * parent = 0);
    virtual ~MediaPlayerShowWnd();

protected:
    virtual void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);
protected slots:
    void showMediaData();

protected:    
    virtual void ShowVideo(void* ShowHandle,unsigned char* pData,unsigned int nSize,int nVideoW,int nVideoH);

protected:
    QMutex              m_mutexVideoBuf;
    RtmpVideoBuf        m_showVideoBuf;

	int					m_paintTimerId;
};