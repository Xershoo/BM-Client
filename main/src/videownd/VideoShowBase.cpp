//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：VideoShowBase.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.17
//	说明：视频显示数据转换和绘制基类定义实现源文件
//	修改记录：
//      
//**********************************************************************
#include "VideoShowBase.h"
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QMatrix>


static void myQImageCleanupFunc(void* imageBuf)
{
	unsigned char* buf = (unsigned char *)imageBuf;
	if(NULL == imageBuf)
	{
		return;
	}

	free(buf);
	buf = NULL;
}

VideoShowBase::VideoShowBase(int nVideoType)
{
    QImage imageBack;
    QString imageFile;
    
    switch (nVideoType)
    {
    case VideoType_Stu:
        {
            imageFile = QString(":/res/res/image/default/student_video_back.png");
        }
        break;
    case VideoType_Tea:
        {
            imageFile = QString(":/res/res/image/default/teacher_video_back.png");
        }
        break;
    case VideoType_Setting:
        {
            imageFile = QString(":/res/res/image/default/setting_video_back.png");
        }
        break;
    case VideoType_Courseware:
        {
            imageFile = QString(":/res/res/image/default/courseware_video_back.png");
        }
        break;

    default:
        {
            imageFile = QString(":/res/res/image/default/student_video_back.png");
        }
        break;
    }

    if(imageBack.load(imageFile,"PNG"))
    {
        m_backPixmap = QPixmap::fromImage(imageBack);
    }

	m_drawRect = false;
}

VideoShowBase::~VideoShowBase()
{
	
}

void VideoShowBase::setDrawVideoRect(bool drawRect)
{
	m_drawRect = drawRect;
}

bool VideoShowBase::copyAndTransVideoData(RtmpVideoBuf& videoShowDec,const RtmpVideoBuf& videoBufSrc)
{
    if (NULL == videoBufSrc.videoBuff || 
        0 >= videoBufSrc.buffsize || 
        0 >= videoBufSrc.videoWidth || 
        0 >= videoBufSrc.videoHeight)
    {
        return false;
    }

	videoShowDec.copy(videoBufSrc);
	    
	// 图像数据处理
    for(int m = 0 ; m < videoShowDec.videoWidth/2 ; m++)      
    {
        for(int n = 0 ; n < videoShowDec.videoHeight;n++)
        {
            int leftPos = (n * videoShowDec.videoWidth + m)*3;
            int rightPos = ((n+1) * videoShowDec.videoWidth - (m + 1))*3;

            unsigned char * bufSrc = videoShowDec.videoBuff + leftPos;
            unsigned char * bufDec = videoBufSrc.videoBuff + (videoBufSrc.isYUVData ?  leftPos:rightPos);

            *(bufSrc) = *(bufDec+2);
            *(bufSrc+1) = *(bufDec+1);
            *(bufSrc+2) = *(bufDec);

            bufSrc = videoShowDec.videoBuff + rightPos;
            bufDec = videoBufSrc.videoBuff + (videoBufSrc.isYUVData ? rightPos:leftPos);

            *(bufSrc) = *(bufDec+2);
            *(bufSrc+1) = *(bufDec+1);
            *(bufSrc+2) = *(bufDec);
        }
    }
    	
    return true;
}

bool VideoShowBase::paintVideoBuff(QPainter * painter,QRect& rectShow,RtmpVideoBuf& videoBuf,QMutex* bufMutext)
{	
    QBrush  backBrush;
    QPixmap imagePixmap;
    QRect   imageRect;
    QRect   drawRect;

    bool    showVideo = false;
    
    do 
    {
		QMutexLocker autoLock(bufMutext);

        if (NULL == videoBuf.videoBuff 
            || 0 >= videoBuf.buffsize 
            || 0 >= videoBuf.videoWidth 
            || 0 >= videoBuf.videoHeight)
        {
            break;
        }
				
		unsigned char * drawBuf = (unsigned char*)malloc(videoBuf.buffsize);
		memcpy(drawBuf,videoBuf.videoBuff,sizeof(unsigned char)*videoBuf.buffsize);

        // 绘制视频，黑底        
		QImage imageVideo(/*videoBuf.videoBuff*/drawBuf,videoBuf.videoWidth,
            videoBuf.videoHeight,QImage::Format_RGB888,myQImageCleanupFunc,drawBuf);
		
		autoLock.unlock();

        if(imageVideo.isNull())
        {
            break;
        }
		
        backBrush = QBrush(QColor(192,192,192,255));
        imagePixmap = QPixmap::fromImage(imageVideo.scaled(QSize(rectShow.width(),rectShow.height()),Qt::IgnoreAspectRatio));
				
        if(!videoBuf.isYUVData)
        {
            QMatrix rotateMatrix;
            rotateMatrix.rotate(180);

            imagePixmap = imagePixmap.transformed(rotateMatrix,Qt::SmoothTransformation);
        }

        showVideo = videoBuf.isDraw ? false : true;        
		videoBuf.isDraw = true;
    } while (false);
    
    if(imagePixmap.isNull())
    {
		if(m_backPixmap.isNull())
		{
			return false;
		}

		backBrush = QBrush(QColor(247,247,247,255));
#ifndef VIDEO_SHOW_SCALE_BACK
        imagePixmap = m_backPixmap;
#else
		imagePixmap = m_backPixmap.scaled(QSize(rectShow.width(),rectShow.height()));
#endif
    }
  
    painter->fillRect(rectShow,backBrush);
    imageRect = QRect(0,0,imagePixmap.width(),imagePixmap.height());
    drawRect = calcImagePaintRect(rectShow,imagePixmap.width(),imagePixmap.height());
	
    painter->drawPixmap(drawRect,imagePixmap,imageRect);

	if(m_drawRect && !imagePixmap.isNull())  //draw rect
	{
		QBrush rectBrush(QColor(211,255,255,255));
		QPen rectPen(rectBrush,2.0);
		QRect rectPrev ;

		rectPrev.setLeft(rectShow.left() + rectShow.width() / 4);
		rectPrev.setTop(rectShow.top() + rectShow.height() / 4);
		rectPrev.setWidth(rectShow.width()/2);
		rectPrev.setHeight(rectShow.height()/2);

		painter->setPen(rectPen);
		painter->drawRect(rectPrev);
	}

	return showVideo;
}

QRect VideoShowBase::calcImagePaintRect(QRect& rectImage,int imageW, int imageH)
{
    QRect rectPaint(rectImage.left()+1,rectImage.top()+1,rectImage.width()-2,rectImage.height()-2);

    if(imageW <= rectPaint.width() && imageH <= rectPaint.height())
    {
        rectPaint.setX(rectPaint.x() + (rectPaint.width() - imageW)/2);
        rectPaint.setY(rectPaint.y() + (rectPaint.height() - imageH)/2);

        rectPaint.setWidth(imageW);
        rectPaint.setHeight(imageH);
    }
    else
    {
        float fltRate = ((imageW * 1.0) / imageH);
        float fltShow = ((rectPaint.width() * 1.0) / rectPaint.height());

        int nShowWidth = rectPaint.width();
        int nShowHeight = rectPaint.height();

        if(fltShow > fltRate)
        {
            nShowWidth = nShowHeight * fltRate;
        }
        else
        {
            nShowHeight = nShowWidth / fltRate;
        }

        rectPaint.setX(rectPaint.x() + (rectPaint.width() - nShowWidth)/2);
        rectPaint.setY(rectPaint.y() + (rectPaint.height() - nShowHeight)/2);

        rectPaint.setWidth(nShowWidth);
        rectPaint.setHeight(nShowHeight);
    }

    return rectPaint;
}