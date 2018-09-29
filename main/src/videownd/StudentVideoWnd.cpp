//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：StudentVideoWnd.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.14
//	说明：学生RTMP流播放窗口封装的类实现源文件
//
//	修改记录：
//      
//**********************************************************************
#include "StudentVideoWnd.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSizePolicy>
#include <QtCore/QCoreApplication>
#include <QtGui/QMovie>


StudentVideoWnd::StudentVideoWnd(QWidget * parent) : OpenGLVideoWidget(parent,VIDEO_STUDENT)//,VideoShowBase(VideoType_Stu)
	,m_btnStuSpeak(NULL)
	,m_labStuSpeak(NULL)
{   
    m_rtmpPlayer.setStudent(true);

//  xiewb 2018.09.25
//	m_refreshTimerId = 0;
//	m_paintTimerId = this->startTimer(TIME_UPDATE_VIDEO,Qt::PreciseTimer);

	m_videoIndex = 0;
	m_showHand = USER_HAND_NO;

//	m_btnSpeakState = BTN_SPEAK_NORAML;
#ifdef STUDENT_VIDEO_HAS_BAR
	setupUI();
#endif
}

void StudentVideoWnd::setupUI()
{
	m_btnStuSpeak = new QPushButton(this);
	m_btnStuSpeak->setGeometry(340,262,17,17);	

	m_btnStuSpeak->setStyleSheet(QString("QPushButton{ width: 17px;height: 17px;background-color: none;image: url(:/res/res/image/default/btnStuSpeak_normal.png);}"
		"QPushButton:hover { background-color: none;image: url(:/res/res/image/default/btnStuSpeak_hover.png);}"
		"QPushButton:pressed { background-color: none;image: url(:/res/res/image/default/btnStuSpeak_pressed.png);}"
		"QPushButton:disabled {	background-color: none;image: url(:/res/res/image/default/btnStuSpeak_disabled.png);}"));

	m_btnStuSpeak->hide();

	m_labStuSpeak = new ClickableLabel(this);
	m_labStuSpeak->setGeometry(345,260,19,18);
	m_labStuSpeak->setContentsMargins(0,0,0,0);
	m_labStuSpeak->setStyleSheet(QString("background-color:transparent;"));
	QMovie* speakMovie = new QMovie(QString(":/res/res/image/default/label_speakState"));
	m_labStuSpeak->setMovie(speakMovie);
	speakMovie->start();
	m_labStuSpeak->hide();

	connect(m_btnStuSpeak,SIGNAL(clicked()),this,SLOT(onSpeakBtnClicked()));
	connect(m_labStuSpeak,SIGNAL(clicked()),this,SLOT(onSpeakLabClicked()));
}

void StudentVideoWnd::setUserSpeak(bool speak)
{
	if(m_userName.isEmpty())
	{
		if(m_btnStuSpeak)
		{
			m_btnStuSpeak->hide();
		}

		if(m_labStuSpeak)
		{
			m_labStuSpeak->hide();
		}
	}
	else
	{
		if(m_btnStuSpeak)
		{
			speak ? m_btnStuSpeak->hide() : m_btnStuSpeak->show();
		}

		if(m_labStuSpeak)
		{
			speak ? m_labStuSpeak->show() : m_labStuSpeak->hide();
		}
	}
}

/*
void StudentVideoWnd::paintEvent(QPaintEvent * event)
{
	if(!this->isVisible())
	{
		return;
	}

    QPainter    painter(this);
    QRect       rectWnd = this->rect();
    
    bool br = paintVideoBuff(&painter,rectWnd,m_showVideoBuf,&m_mutexVideoBuf);
	if(br)
	{
		killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
	}
	else
	{
		if(m_refreshTimerId <= 0)
		{
			m_refreshTimerId = this->startTimer(TIME_KEEP_VIDEO);
		}
	}

#ifdef STUDENT_VIDEO_HAS_BAR
	paintToolbar(painter,rectWnd);
#endif

#ifdef STUDENT_VIDEO_DRAW_HAND
    paintHandStatu(painter,rectWnd);
#endif
    return;
}
*/

void StudentVideoWnd::getRenderImage()
{
	OpenGLVideoWidget::getRenderImage();

	if(NULL==m_renderImage || m_renderImage->isNull()){
		return;
	}

	QPainter painter(m_renderImage);
	QRect    rectWnd = this->rect();

#ifdef STUDENT_VIDEO_HAS_BAR
	paintToolbar(painter,rectWnd);
#endif

#ifdef STUDENT_VIDEO_DRAW_HAND
	paintHandStatu(painter,rectWnd);
#endif
}

void StudentVideoWnd::paintHandStatu(QPainter& painter, QRect& rectWnd)
{
    QImage imageHand;
    QString imageHandFile;

    switch(m_showHand)
    {   
    case USER_HAND_ASK:
        imageHandFile = QString(":/res/res/image/default/student_video_hand.png");
        break;
    case USER_HAND_SPK:
        imageHandFile = QString(":/res/res/image/default/student_video_speak.png");
        break;
    case USER_HAND_NO:
    default:
        return;
    }

    if(imageHand.load(imageHandFile,"PNG"))
    {
        painter.drawImage(0,0,imageHand.scaled(QSize(rectWnd.width(),rectWnd.height()),Qt::KeepAspectRatio),0,0);
    };
}

void StudentVideoWnd::paintToolbar(QPainter& painter,QRect& rectWnd)
{
	QRect rectBar(rectWnd.left() + 1,rectWnd.bottom() - 28,
		rectWnd.width()-3,28);
	
	//背景
	painter.fillRect(rectBar.left(),rectBar.top(),rectBar.width(),rectBar.height(),QColor(32,32,32,192));

	//序号
	QString strImageIndex = QString(":/res/res/image/default/stuVideoIndex_%1.png").arg(m_videoIndex+1);
	QImage imageIndex;
	if(imageIndex.load(strImageIndex,"PNG"))
	{
		int xPos = rectBar.left() + 10;
		int yPos = rectBar.top() + (rectBar.height() - imageIndex.height()) / 2;
		painter.drawImage(xPos,yPos,imageIndex,0,0);
	};

	//名字
	if(!m_userName.isEmpty())
	{
		QFont fontName(QString("Microsoft YaHei"),10);
		painter.setFont(fontName);
		painter.setPen(QColor(255,255,255));
		painter.drawText(rectBar,m_userName,QTextOption(Qt::AlignCenter|Qt::AlignVCenter));
	}

	//举手
	if(m_showHand == USER_HAND_ASK)
	{
		QImage imageHand;
		QString imageHandFile = QString(":/res/res/image/default/label_handsUp.png");

		if(imageHand.load(imageHandFile,"PNG"))
		{
			int xPos = rectBar.right() - 60;
			int yPos = rectBar.top() + (rectBar.height() - imageHand.height()) / 2;
			painter.drawImage(xPos,yPos,imageHand,0,0);
		};
	}

	/*
	//喇叭
	if(m_showSound)
	{
		//gif动画
	}
	else
	{
		
		QImage imageSpeak;
		QString imageSpeakFile;

		switch(m_btnSpeakState)
		{
		case BTN_SPEAK_NORAML:
			imageSpeakFile = QString(":/res/res/image/default/btnStuSpeak_normal.png");
			break;
		case BTN_SPEAK_HOVER:
			imageSpeakFile = QString(":/res/res/image/default/btnStuSpeak_hover.png");
			break;
		case BTN_SPEAK_PRESS:
			imageSpeakFile = QString(":/res/res/image/default/btnStuSpeak_pressed.png");
			break;
		}

		if(imageSpeak.load(imageSpeakFile,"PNG"))
		{
			int xPos = rectBar.right() - imageSpeak.width() - 14;
			int yPos = rectBar.top() + (rectBar.height() - imageSpeak.height()) / 2;
			painter.drawImage(xPos,yPos,imageSpeak,0,0);
		};
	}*/
}

void StudentVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
    OpenGLVideoWidget::showVideoBuffer(videoData.videoWidth,videoData.videoHeight,videoData.isYUVData,videoData.buffsize,videoData.videoBuff);	
}

/*
void StudentVideoWnd::timerEvent(QTimerEvent * event)
{
	if(NULL == event)
	{
		return;
	}

	int timerId = event->timerId();
	if( timerId== m_refreshTimerId)
	{		
		QMutexLocker autoLock(&m_mutexVideoBuf);
		m_showVideoBuf.zeroBuffer();
	
		this->killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;
		
		return;
	}

	if(timerId == m_paintTimerId)
	{
		if(!this->isVisible())
		{
			return;
		}

		//this->update();
		QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);

		return;
	}

	return;
}
*/
void StudentVideoWnd::onSpeakBtnClicked()
{
    if(m_btnStuSpeak)
    {
        m_btnStuSpeak->hide();
    }

    if(m_labStuSpeak)
    {
        m_labStuSpeak->show();
    }

	m_showHand = USER_HAND_SPK;

	emit stuSpeakBtnClicked(this->m_nPlayerId,true);
}

void StudentVideoWnd::onSpeakLabClicked()
{
    if(m_btnStuSpeak)
    {
        m_btnStuSpeak->show();
    }

    if(m_labStuSpeak)
    {
        m_labStuSpeak->hide();
    }

	emit stuSpeakBtnClicked(this->m_nPlayerId,false);
}

void StudentVideoWnd::mouseDoubleClickEvent(QMouseEvent * event)
{

}