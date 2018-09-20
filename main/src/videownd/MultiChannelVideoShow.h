#ifndef MULTI_CHANNEL_VIDEO_SHOW_H
#define MULTI_CHANNEL_VIDEO_SHOW_H

#include "VideoShowBase.h"
#define MULTI_SHOW_CHANNEL_NUM  (4)

class CRTMPPlayer;
class MultiChannelVideoShow;

class SingleStudenVideoShow: public CRTMPStreamShow
{
public:
	SingleStudenVideoShow(MultiChannelVideoShow* multiShow)
	{
		m_multiShow = multiShow;
	};
	
	void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
protected:
	MultiChannelVideoShow* m_multiShow;
};

class MultiChannelVideoShow : public VideoShowBase
{
public:
    MultiChannelVideoShow();
	virtual ~MultiChannelVideoShow();

    void setMultiChannelShow(bool multiShow);                           //�����Ƿ���λ��ʾ
    bool isMulitChannelShow() {return m_setMultiShow;}
    void setStudentVideoStream(CRTMPStream* rtmpStream,bool set);           //�����Ƿ�������ѧ����Ƶ

	virtual int   getMainVideoIndex() = 0;
public:
    virtual void recvTeacherVideoData(const RtmpVideoBuf& videoData);
    virtual void recvStudentVideoData(const RtmpVideoBuf& videoData);

protected:
    virtual QRect getVideoShowRect() = 0;    
    
protected:
    QRect calcVideoRect(int nIndex);
    bool  drawVideo(QPainter* painter,int nIndex);

protected:
    QMutex              m_mutexVideoBuf[MULTI_SHOW_CHANNEL_NUM];
    RtmpVideoBuf        m_showVideoBuf[MULTI_SHOW_CHANNEL_NUM];
	QPixmap	*			m_multImgPixmap[MULTI_SHOW_CHANNEL_NUM];

    bool                m_setMultiShow;
    bool                m_setStudentVideo;

	SingleStudenVideoShow  *  m_showStudentVideo;

	friend class SingleStudenVideoShow;
};
    
#endif


