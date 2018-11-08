#ifndef MULTI_CHANNEL_VIDEO_SHOW_H
#define MULTI_CHANNEL_VIDEO_SHOW_H

//#include "VideoShowBase.h"
#include "RtmpStream.h"
#include "OpenGLVideoWidget.h"
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

class MultiChannelVideoShow : public QWidget//public VideoShowBase
{
	Q_OBJECT
public:
    MultiChannelVideoShow(QWidget * parent,int videoType);
	virtual ~MultiChannelVideoShow();

    void setMultiChannelShow(bool multiShow);                           //设置是否多机位显示
    bool isMulitChannelShow() {return m_setMultiShow;}
    void setStudentVideoStream(CRTMPStream* rtmpStream,bool set);           //设置是否有提问学生视频

	virtual int   getMainVideoIndex() = 0;
public:
    virtual void recvTeacherVideoData(const RtmpVideoBuf& videoData);
    virtual void recvStudentVideoData(const RtmpVideoBuf& videoData);

protected:
	virtual QRect getVideoShowRect() = 0; 
// 	virtual void  drawRenderImage();

	virtual void  showVideoData(int nIndex,const RtmpVideoBuf& videoData);

	virtual void  paintEvent(QPaintEvent *event);
	virtual void  resizeEvent(QResizeEvent* event);
protected:
    QRect calcVideoRect(int nIndex);
	void  setVideoWidgetGeometry();

	void  calcImageDrawRect(QRect& rectDraw,int imageW, int imageH);
//	bool  drawVideo(QPainter* painter,int nIndex);

protected:
    //QMutex              m_mutexVideoBuf[MULTI_SHOW_CHANNEL_NUM];
    //RtmpVideoBuf        m_showVideoBuf[MULTI_SHOW_CHANNEL_NUM];
	//QPixmap	*		  m_multImgPixmap[MULTI_SHOW_CHANNEL_NUM];
	//QImage*				m_multiBufImage[MULTI_SHOW_CHANNEL_NUM];
	//QMutex				m_multiMutexImage[MULTI_SHOW_CHANNEL_NUM];
	//unsigned char*		m_multiVideoBuf[MULTI_SHOW_CHANNEL_NUM];

	OpenGLVideoWidget*		m_videoWidget[MULTI_SHOW_CHANNEL_NUM];

    bool                m_setMultiShow;
    bool                m_setStudentVideo;

	SingleStudenVideoShow  *  m_showStudentVideo;

	friend class SingleStudenVideoShow;
};
    
#endif


