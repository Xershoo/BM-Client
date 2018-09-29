//**********************************************************************
//	Copyright ��c�� 2015,����΢���Ƽ����޹�˾. All rights reserved.
//	�ļ����ƣ�UserPublishVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.16
//	˵����RTMP������Ԥ�����ڷ�װ���ඨ��ͷ�ļ�
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef USER_PUBLISHE_VIDEO_WND
#define USER_PUBLISHE_VIDEO_WND

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtWidgets/QWidget>
#include "RtmpPublisher.h"
#include "VideoShowBase.h"

#include "OpenGLVideoWidget.h"
/*
class UserPubliserVideoWnd : public QWidget,
	public CRTMPStreamShow,
    public VideoShowBase */
class UserPubliserVideoWnd : public OpenGLVideoWidget,
	public CRTMPStreamShow
{
    Q_OBJECT
public:
    UserPubliserVideoWnd(QWidget * parent);
	
public:
    virtual bool setVideoIndex(int nIndex,bool start = true);
	virtual void closeVideo();

    virtual void showBar(bool show = true);
public:
    inline bool isStart();
    inline int  getVideoIndex();

    inline CRTMPPublisher* getPublisher();
	
	inline void setPaintCircle(bool paintCircle)
	{
		m_paintCircle = paintCircle;
	};

	virtual void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
protected:
//	virtual void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);

protected:
	virtual void setupUi();

protected:
    CRTMPPublisher*     m_rtmpPublisher;
    int                 m_indexVideo;
    bool                m_isStarted;
	
    QMutex              m_mutexVideoBuf;
    RtmpVideoBuf        m_showVideoBuf;	

//	int					m_refreshTimerId;

	int					m_paintTimerId;
	bool				m_paintCircle;

    QWidget*            m_widgetToolbar;
};

bool UserPubliserVideoWnd::isStart()
{
    if(NULL == m_rtmpPublisher)
    {
        return false;
    }

    return m_rtmpPublisher->isStart();
}

int UserPubliserVideoWnd::getVideoIndex()
{
    return m_indexVideo;
}

CRTMPPublisher* UserPubliserVideoWnd::getPublisher()
{
    return m_rtmpPublisher;
}

#endif