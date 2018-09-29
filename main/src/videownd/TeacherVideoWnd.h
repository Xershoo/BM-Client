//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：TeacherVideoWnd.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.15
//	说明：老师RTMP流播放窗口封装的类定义头文件
//  使用说明：
//      TeacherVideoWnd * videoWnd = new TeacherVideoWnd(parent);
//      videoWnd->resize(400,300);
//      videoWnd->show();
//      videoWnd->setUrl(url);
//      videoWnd->start();
//      videoWnd->setMultiShow(true);
//      videoWnd->setMainVideo(2);
//	修改记录：
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