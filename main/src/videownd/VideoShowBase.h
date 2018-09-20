//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�VideoShowBase.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.17
//	˵������Ƶ��ʾ����ת���ͻ��ƻ��ඨ��ͷ�ļ�
//	�޸ļ�¼��
//      
//**********************************************************************

#ifndef VIDEO_SHOW_BASE_H
#define VIDEO_SHOW_BASE_H

#include <QtGui/QPainter>
#include <QtCore/QRect>
#include <QtCore/QMutex>
#include <QtGui/QPixmap>
#include "RtmpStream.h"

//��Ƶ���ͣ���������ȱʡͼƬ
#define  TIME_UPDATE_VIDEO   (40)
#define  TIME_KEEP_VIDEO	 (2000)

enum VideoType
{
    VideoType_Stu   = 0,
    VideoType_Tea,
    VideoType_Setting,
    VideoType_Courseware,
};

class VideoShowBase
{
public:
    VideoShowBase(int nVideoType = VideoType_Stu);
	virtual ~VideoShowBase();

	void setDrawVideoRect(bool drawRect);
protected:
    virtual bool copyAndTransVideoData(RtmpVideoBuf& videoBufDesc,const RtmpVideoBuf& videoBufSrc);
    virtual bool paintVideoBuff(QPainter * painter,QRect& rectShow,RtmpVideoBuf& videoBuf,QMutex* bufMutext);
    virtual QRect calcImagePaintRect(QRect& rectImage,int imageW, int imageH);

protected:
    QPixmap		m_backPixmap;		
	bool		m_drawRect;
};

#endif