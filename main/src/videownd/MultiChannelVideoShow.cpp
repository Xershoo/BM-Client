#include "MultiChannelVideoShow.h"
#include "RtmpPlayer.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPainter>

void SingleStudenVideoShow::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
	if(NULL != m_multiShow)
	{
		m_multiShow->recvStudentVideoData(videoData);
	}
}

//////////////////////////////////////////////////////////////////////////
MultiChannelVideoShow::MultiChannelVideoShow() : m_setStudentVideo(false)
	,m_setMultiShow(false)
	,VideoShowBase(VideoType_Tea)
	,m_showStudentVideo(NULL)
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++)
	{
		m_multImgPixmap[i] = NULL;
	}

	m_showStudentVideo = new SingleStudenVideoShow(this);
}

MultiChannelVideoShow::~MultiChannelVideoShow()
{
	for(int i=0;i<MULTI_SHOW_CHANNEL_NUM;i++)
	{
		if(NULL != m_multImgPixmap)
		{
			delete m_multImgPixmap[i];
			m_multImgPixmap[i] = NULL;
		}
	}

	if(NULL != m_showStudentVideo)
	{
		delete m_showStudentVideo;
		m_showStudentVideo = NULL;
	}
}

void MultiChannelVideoShow::recvTeacherVideoData(const RtmpVideoBuf& videoData)
{   
    int nIndex = videoData.videoIndex;
    if(nIndex >=(MULTI_SHOW_CHANNEL_NUM-1) || nIndex < 0)
    {
        return;
    }

    QMutexLocker autoLock(&m_mutexVideoBuf[nIndex]);
    copyAndTransVideoData(m_showVideoBuf[nIndex],videoData);	
}

void MultiChannelVideoShow::recvStudentVideoData(const RtmpVideoBuf& videoData)
{
    int nIndex = MULTI_SHOW_CHANNEL_NUM-1;

    QMutexLocker autoLock(&m_mutexVideoBuf[nIndex]);
	copyAndTransVideoData(m_showVideoBuf[nIndex],videoData);	
}

void MultiChannelVideoShow::setMultiChannelShow(bool multiShow)
{
    m_setMultiShow = multiShow;	
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
}

QRect MultiChannelVideoShow::calcVideoRect(int nIndex)
{
    QRect rectWnd = getVideoShowRect();
    QRect rectVideo(0,0,0,0);

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