//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�UserVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.14
//	˵����RTMP�����Ŵ��ڷ�װ���ඨ��ͷ�ļ�
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef USER_VIDEO_WND
#define USER_VIDEO_WND

#include <QtWidgets/QWidget>
#include "RtmpPlayer.h"

class UserVideoWnd : public QWidget,
	public CRTMPStreamShow
{
    Q_OBJECT
public:
    UserVideoWnd(QWidget * parent);

public:
    virtual bool start(void);
    virtual bool change(void);
    virtual void stop(void);

public:
    inline bool isStart();

    inline void setUserId(__int64 nUserId);
    inline __int64 getUserId();

	inline void setPlayerId(__int64 nUserId);
	inline __int64 getPlayerId();

    inline void setUrl(const char* url);
    inline string getUrl();

    inline void setMainVideo(int nMainVideo);
    inline int getMainVideo();

    inline CRTMPPlayer* getPlayer();

	virtual void showRtmpVideoBuf(const RtmpVideoBuf& videoData);

protected:
    CRTMPPlayer    m_rtmpPlayer;
	__int64		   m_nPlayerId;
};

bool UserVideoWnd::isStart()
{
    return m_rtmpPlayer.isStart();
}

void UserVideoWnd::setPlayerId(__int64 nUserId)
{
    m_rtmpPlayer.setUserId(nUserId);
}

__int64 UserVideoWnd::getPlayerId()
{
    return m_rtmpPlayer.getUserId();
}

void UserVideoWnd::setUserId(__int64 nUserId)
{
	m_nPlayerId = nUserId;
}

__int64 UserVideoWnd::getUserId()
{
	return m_nPlayerId;
}

void UserVideoWnd::setUrl(const char* url)
{
    m_rtmpPlayer.setUrl(url);
}

string UserVideoWnd::getUrl()
{
    return m_rtmpPlayer.getUrl();
}

void UserVideoWnd::setMainVideo(int nMainVideo)
{
    m_rtmpPlayer.setMainVideo(nMainVideo);
	m_rtmpPlayer.change();
}

int UserVideoWnd::getMainVideo()
{
    return m_rtmpPlayer.getMainVideo();
}

CRTMPPlayer* UserVideoWnd::getPlayer()
{
    return &m_rtmpPlayer;
}

#endif