//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�TeacherPublishVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.15
//	˵������ʦRTMP��Ƶ������Ԥ�����ڷ�װ���ඨ��ͷ�ļ�
//  ʹ��˵����
//      TeacherPublishVideoWnd * videoWnd = new TeacherPublishVideoWnd(parent);
//      videoWnd->resize(400,300);
//      videoWnd->show();
//      videoWnd->setMultiShow(true);
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef TEACHER_PUBLISH_VIDEO_WND_H
#define TEACHER_PUBLISH_VIDEO_WND_H

#include <QtWidgets/QWidget>
#include "MultiChannelVideoShow.h"

class TeacherPublishVideoWnd;
class SingleTeacherVideoShow: public CRTMPStreamShow
{
public:
	explicit SingleTeacherVideoShow(TeacherPublishVideoWnd* wndTeaPubVideo,int nVideoIndex)
	{
		m_wndTeaPubVideo = wndTeaPubVideo;
		m_videoIndex = nVideoIndex;
	};

	void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
protected:
	TeacherPublishVideoWnd* m_wndTeaPubVideo;
	int						m_videoIndex;
};

class TeacherPublishVideoWnd : //public QWidget,
    public MultiChannelVideoShow
{
    Q_OBJECT
public:
    TeacherPublishVideoWnd(QWidget * parent);
	virtual ~TeacherPublishVideoWnd();

	virtual void  setStudentVideoStream(CRTMPStream* rtmpStream,bool set);
	virtual int   getMainVideoIndex();

protected:
    //virtual void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);

    virtual void mouseDoubleClickEvent(QMouseEvent * event);
		
protected slots:
	void onUpdatePublishSeat(int seatIndex);

protected:
    virtual QRect getVideoShowRect();

protected:
	//int			m_refreshTimerId[MULTI_SHOW_CHANNEL_NUM];
	//int			m_paintTimerId;

	SingleTeacherVideoShow*  m_showTeaVideo[MULTI_SHOW_CHANNEL_NUM-1];
};

#endif