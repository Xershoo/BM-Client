//**********************************************************************
//	Copyright ��c�� 2016,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�MediaPlayerShowWnd.h
//	�汾�ţ�1.0
//	���ߣ� 
//	ʱ�䣺 
//	˵��������Ƶ�μ����Ŵ��ڷ�װ���ඨ��ͷ�ļ�
//  ʹ��˵����
//	�޸ļ�¼��
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