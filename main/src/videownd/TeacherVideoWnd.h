//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�TeacherVideoWnd.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.15
//	˵������ʦRTMP�����Ŵ��ڷ�װ���ඨ��ͷ�ļ�
//  ʹ��˵����
//      TeacherVideoWnd * videoWnd = new TeacherVideoWnd(parent);
//      videoWnd->resize(400,300);
//      videoWnd->show();
//      videoWnd->setUrl(url);
//      videoWnd->start();
//      videoWnd->setMultiShow(true);
//      videoWnd->setMainVideo(2);
//	�޸ļ�¼��
//      
//**********************************************************************
#ifndef TEACHER_VIDEO_WND_H
#define TEACHER_VIDEO_WND_H

#include "UserVideoWnd.h"
#include "MultiChannelVideoShow.h"

class TeacherVideoWnd : public MultiChannelVideoShow,
	public UserVideoWnd
{
    Q_OBJECT
public:
    TeacherVideoWnd(QWidget * parent);

	virtual void  setStudentVideoStream(CRTMPStream* rtmpStream,bool set);
protected:
    //virtual void paintEvent(QPaintEvent * event);
	virtual void timerEvent(QTimerEvent * event);

	void showRtmpVideoBuf(const RtmpVideoBuf& videoData);
	
protected:
    virtual QRect getVideoShowRect();
    virtual int   getMainVideoIndex();
	
protected:
	//int			m_refreshTimerId[MULTI_SHOW_CHANNEL_NUM];
	//int			m_paintTimerId;
};

#endif