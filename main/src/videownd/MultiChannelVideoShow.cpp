#include "MultiChannelVideoShow.h"
#include "RtmpPlayer.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include "common/macros.h"

void SingleStudenVideoShow::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	if(NULL != m_multiShow)
	{
		m_multiShow->recvStudentVideoData(videoData);
	}
}

//////////////////////////////////////////////////////////////////////////
MultiChannelVideoShow::MultiChannelVideoShow(QWidget * parent,int videoType) : QWidget(parent)
	,m_setStudentVideo(false)
	,m_setMultiShow(false)
	,m_showStudentVideo(NULL)
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++)
	{
		int videoType = (i == MULTI_SHOW_CHANNEL_NUM-1) ? OpenGLVideoWidget::VIDEO_STUDENT:OpenGLVideoWidget::VIDEO_TEACHER;
		m_videoWidget[i]=new OpenGLVideoWidget(parent,videoType);
		m_videoWidget[i]->setVisible(false);
		m_videoWidget[i]->setGeometry(0,0,0,0);
// 		m_multiBufImage[i] = NULL;
// 		m_multiVideoBuf[i] = NULL;
	}

	m_showStudentVideo = new SingleStudenVideoShow(this);
}

MultiChannelVideoShow::~MultiChannelVideoShow()
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++)
	{
		SAFE_DELETE(m_videoWidget[i]);
// 		QMutexLocker autoLocker(&m_multiMutexImage[i]);
// 		SAFE_DELETE(m_multiBufImage[i]);
// 		SAFE_FREE(m_multiVideoBuf[i]);
	}

	SAFE_DELETE(m_showStudentVideo);
}

void MultiChannelVideoShow::recvTeacherVideoData(const RtmpVideoBuf& videoData)
{   
    int nIndex = videoData.videoIndex;
    if(nIndex >=(MULTI_SHOW_CHANNEL_NUM-1) || nIndex < 0)
    {
        return;
    }

	/*
	QMutexLocker autoLock(&m_mutexVideoBuf[nIndex]);
	copyAndTransVideoData(m_showVideoBuf[nIndex],videoData);
	*/

	showVideoData(nIndex,videoData);
}

void MultiChannelVideoShow::recvStudentVideoData(const RtmpVideoBuf& videoData)
{
    int nIndex = MULTI_SHOW_CHANNEL_NUM-1;

	/*
    QMutexLocker autoLock(&m_mutexVideoBuf[nIndex]);
	copyAndTransVideoData(m_showVideoBuf[nIndex],videoData);
	*/

	showVideoData(nIndex,videoData);
}

void MultiChannelVideoShow::setMultiChannelShow(bool multiShow)
{
    m_setMultiShow = multiShow;	
	setVideoWidgetGeometry();
}

void MultiChannelVideoShow::setStudentVideoStream(CRTMPStream* rtmpStream,bool set)
{
	if(NULL == m_showStudentVideo)
	{
		return;
	}
	
	if(set&&rtmpStream)
	{
		m_showStudentVideo->setRtmpStream(rtmpStream);
	}
	else
	{
		m_showStudentVideo->setRtmpStream(NULL);
	}
	
    m_setStudentVideo = set;
	setVideoWidgetGeometry();
}

QRect MultiChannelVideoShow::calcVideoRect(int nIndex)
{
    QRect rectWnd = getVideoShowRect();
	QRect rectVideo(0,0,0,0);

	if(rectWnd.width() <= 0 || rectWnd.height()<=0){
		return rectVideo;
	}

    if(nIndex >= MULTI_SHOW_CHANNEL_NUM)
    {
        return rectVideo;
    }

    //多机位模式
    if(!m_setMultiShow)
    {
        if(m_setStudentVideo)       //有提问的学生
        {
            if( nIndex == MULTI_SHOW_CHANNEL_NUM-1)        //提问学生在左边
            {
                rectVideo = rectWnd;
                rectVideo.setLeft(rectWnd.left() + rectWnd.width()/2);

                return rectVideo;
            }
            else if(nIndex == getMainVideoIndex())     //主视频在右边
            {
                rectVideo = rectWnd;
                rectVideo.setRight(rectWnd.left() + rectWnd.width()/2);

                return rectVideo;
            }
            else //其他不显示
            {
                return rectVideo;
            }
        }
        else
        {
            if(nIndex == getMainVideoIndex())          //主视频铺满
            {
                return rectWnd;
            }
            else                                               //其他不显示      
            {
                return rectVideo;
            }
        }
    }

    if(nIndex == getMainVideoIndex())     //主视频在上边
    {
        rectVideo = rectWnd;
        rectVideo.setLeft(rectWnd.left() + rectWnd.width()/6);
        rectVideo.setRight(rectWnd.right() - rectWnd.width()/6);
        rectVideo.setBottom(rectWnd.bottom() - rectWnd.height()/3);

        return rectVideo;
    }

    if(m_setStudentVideo)
    {
        rectVideo = rectWnd;
        rectVideo.setTop(rectWnd.bottom() - rectWnd.height()/3);

        if(nIndex == 3)
        {
            rectVideo.setRight(rectVideo.left() + rectWnd.width()/3);
            return rectVideo;
        }

        if( nIndex == 0 || (nIndex == 1 && getMainVideoIndex() == 0))
        {
            rectVideo.setLeft(rectWnd.left() + rectWnd.width()/3);
            rectVideo.setRight(rectVideo.left() + rectWnd.width()/3);
        }
        else
        {
            rectVideo.setLeft(rectWnd.right() - rectWnd.width()/3);
        }
    }
    else
    {
        if(nIndex == MULTI_SHOW_CHANNEL_NUM-1)             //没有提问视频
        {
            return rectVideo;
        }

        rectVideo = rectWnd;
        rectVideo.setTop(rectWnd.bottom() - rectWnd.height()/3);

        if( nIndex == 0 || (nIndex == 1 && getMainVideoIndex() == 0))     //左边视频
        {
            rectVideo.setLeft(rectWnd.left() + rectWnd.width()/6);
            rectVideo.setRight(rectVideo.left() + rectWnd.width()/3);
        }
        else                                                                    //右边视频                
        {   
            rectVideo.setRight(rectWnd.right() - rectWnd.width()/6);
            rectVideo.setLeft(rectVideo.right() - rectWnd.width()/3);
        }
    }

    return rectVideo;
}

/*
bool MultiChannelVideoShow::drawVideo(QPainter* painter,int nIndex)
{
    if (NULL == painter || nIndex >= MULTI_SHOW_CHANNEL_NUM)
    {
        return true;
    }

    QRect rectVideo = calcVideoRect(nIndex);
    rectVideo = QRect(rectVideo.left()+1,rectVideo.top()+1,rectVideo.width()-2,rectVideo.height()-2);

    if(rectVideo.width() <= 0 || rectVideo.height() <= 0)
    {
        return true;
    }
	
	bool br = paintVideoBuff(painter,rectVideo,m_showVideoBuf[nIndex],&m_mutexVideoBuf[nIndex]);
	
	return br;
}
*/

/*
void MultiChannelVideoShow::getRenderImage()
{
	if(!this->isVisible())
	{
		return ;
	}

	QRect		rectShow = this->rect();
	QPainter*	imagePainter = NULL;

	if(NULL == m_renderImage)
	{
		m_renderImage = new QImage(rectShow.width(),rectShow.height(),QImage::Format_RGB32);
	}

	QPainter painter(m_renderImage);
	QRect imageRect = calcImagePaintRect(rectShow,m_backImage.width(),m_backImage.height());
	QBrush backBrush(QColor(247,247,247,255));

	painter.fillRect(rectShow,backBrush);
	painter.drawImage(imageRect,m_backImage);
	
	//paint video image
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++){
		QRect rectVideo = calcVideoRect(i);
		rectVideo = QRect(rectVideo.left()+1,rectVideo.top()+1,rectVideo.width()-2,rectVideo.height()-2);

		if(rectVideo.width() <= 0 || rectVideo.height() <= 0)
		{
			continue;
		}

		QMutexLocker autoLocker(&m_multiMutexImage[i]);
		if(NULL==m_multiBufImage[i] || m_multiBufImage[i]->isNull()){
			continue;
		}

		QImage imageScaled = m_multiBufImage[i]->scaled(QSize(rectVideo.width(),rectVideo.height()),Qt::KeepAspectRatio,Qt::SmoothTransformation);
		if(imageScaled.isNull())
		{
			continue;
		}
		
		calcImageDrawRect(rectVideo,imageScaled.width(),imageScaled.height());
		painter.drawImage(rectVideo,imageScaled.convertToFormat(QImage::Format_RGB32));
	}

	//paint rect
	if(m_drawRect)
	{
		QBrush rectBrush(QColor(211,255,255,255));
		QPen rectPen(rectBrush,2.0);
		QRect rectPrev ;

		rectPrev.setLeft(rectShow.left() + rectShow.width() / 4);
		rectPrev.setTop(rectShow.top() + rectShow.height() / 4);
		rectPrev.setWidth(rectShow.width()/2);
		rectPrev.setHeight(rectShow.height()/2);

		painter.setPen(rectPen);
		painter.drawRect(rectPrev);
	}

	if(m_refreshTimerId !=0)
	{
		killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
	}

	m_refreshTimerId = this->startTimer(3000,Qt::PreciseTimer);
}
*/

void MultiChannelVideoShow::showVideoData(int nIndex,const RtmpVideoBuf& videoData)
{
	if(videoData.videoWidth <= 0 || videoData.videoHeight <= 0 || videoData.buffsize <=0 || NULL == videoData.videoBuff)
	{
		return;
	}

	if (NULL == m_videoWidget[nIndex]||
		m_videoWidget[nIndex]->width() <= 0||
		m_videoWidget[nIndex]->height() <= 0 ||
		!m_videoWidget[nIndex]->isVisible()){
		return;
	}

	m_videoWidget[nIndex]->showVideoBuffer(videoData.videoWidth
		,videoData.videoHeight
		,videoData.isYUVData
		,videoData.buffsize
		,videoData.videoBuff);
/*
	QMutexLocker autoLocker(&m_multiMutexImage[nIndex]);
	SAFE_DELETE(m_multiBufImage[nIndex]);
	SAFE_FREE(m_multiVideoBuf[nIndex]);

	m_multiVideoBuf[nIndex] = (unsigned char*)malloc(videoData.buffsize);
	if(NULL == m_multiVideoBuf[nIndex])
	{
		return;
	}

	memset(m_multiVideoBuf[nIndex],NULL,videoData.buffsize);
	memcpy(m_multiVideoBuf[nIndex],videoData.videoBuff,videoData.buffsize);

	QImage bufImage(m_multiVideoBuf[nIndex],videoData.videoWidth,videoData.videoHeight,QImage::Format_RGB888);	
	if(bufImage.isNull())
	{
		return;
	}

	if(!videoData.isYUVData)
	{
		QMatrix rotateMatrix;
		rotateMatrix.rotate(180);

		QImage tempImage = bufImage.transformed(rotateMatrix,Qt::SmoothTransformation);
		bufImage = tempImage.mirrored(true,false);
	}

	if(m_refreshTimerId !=0)
	{
		killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
	}
	
	m_multiBufImage[nIndex] = new QImage(bufImage);
	
	updateScene();
*/
}


void MultiChannelVideoShow::calcImageDrawRect(QRect& rectDraw,int imageW, int imageH)
{
	int nShowWidth = rectDraw.width();
	int nShowHeight = rectDraw.height();

	if(imageW <= rectDraw.width() && imageH <= rectDraw.height())
	{
		nShowWidth = imageW;
		nShowHeight = imageH;
	}
	else
	{
		float fltRate = ((imageW * 1.0) / imageH);
		float fltShow = ((rectDraw.width() * 1.0) / rectDraw.height());

		if(fltShow > fltRate)
		{
			nShowWidth = nShowHeight * fltRate;
		}
		else
		{
			nShowHeight = nShowWidth / fltRate;
		}
	}

	rectDraw.setX(rectDraw.x() + (rectDraw.width() - nShowWidth)/2);
	rectDraw.setY(rectDraw.y() + (rectDraw.height() - nShowHeight)/2);

	rectDraw.setWidth(nShowWidth);
	rectDraw.setHeight(nShowHeight);
}

void MultiChannelVideoShow::setVideoWidgetGeometry()
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++){
		if(NULL == m_videoWidget[i]) {
			continue;
		}

		QRect rectVideo = calcVideoRect(i);
		if(rectVideo.width()<=0||rectVideo.height()<=0){
			m_videoWidget[i]->hide();
		}else {
			m_videoWidget[i]->show();
			m_videoWidget[i]->setGeometry(rectVideo);
		}
	}
}

void MultiChannelVideoShow::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	setVideoWidgetGeometry();
}

void MultiChannelVideoShow::paintEvent(QPaintEvent *event)
{
	if(!this->isVisible())
	{
		return;
	}

	QPainter    painter(this);
	QRect       rectWnd = this->rect();
	QImage		backImage;
	QString		imageFile = QString(":/res/res/image/default/teacher_video_back.png");
	QBrush		backBrush(QColor(247,247,247,255));

	painter.fillRect(rectWnd,backBrush);
	backImage.load(imageFile);

	if(!backImage.isNull())
	{
		QRect imageRect=rectWnd;
		calcImageDrawRect(imageRect,backImage.width(),backImage.height());
		painter.drawImage(imageRect,backImage);
	}
}