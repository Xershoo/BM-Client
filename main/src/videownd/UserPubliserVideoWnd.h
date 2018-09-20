//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：UserPublishVideoWnd.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.16
//	说明：RTMP流推送预览窗口封装的类定义头文件
//	修改记录：
//      
//**********************************************************************

#ifndef USER_PUBLISHE_VIDEO_WND
#define USER_PUBLISHE_VIDEO_WND

#include <QtWidgets/QWidget>
#include "RtmpPublisher.h"
#include "VideoShowBase.h"

class UserPubliserVideoWnd : public QWidget,
	public CRTMPStreamShow,
    public VideoShowBase
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

	virtual void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
protected:
    virtual void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);

protected:
	virtual void setupUi();
protected:
    CRTMPPublisher*     m_rtmpPublisher;
    int                 m_indexVideo;
    bool                m_isStarted;
	
    QMutex              m_mutexVideoBuf;
    RtmpVideoBuf        m_showVideoBuf;	

	int					m_refreshTimerId;
	int					m_paintTimerId;

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