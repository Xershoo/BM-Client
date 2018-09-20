//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：VideoShowBase.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.17
//	说明：视频显示数据转换和绘制基类定义头文件
//	修改记录：
//      
//**********************************************************************

#ifndef VIDEO_SHOW_BASE_H
#define VIDEO_SHOW_BASE_H

#include <QtGui/QPainter>
#include <QtCore/QRect>
#include <QtCore/QMutex>
#include <QtGui/QPixmap>
#include "RtmpStream.h"

//视频类型，方便设置缺省图片
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