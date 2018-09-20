#include "UserPubliserVideoWnd.h"
#include <QtCore/QMutexLocker>
#include <QtGui/QBrush>
#include <QtGui/QImage>
#include <QtGui/QMatrix>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSizePolicy>
#include "MediaPublishMgr.h"


UserPubliserVideoWnd::UserPubliserVideoWnd(QWidget* parent):QWidget(parent),VideoShowBase(VideoType_Tea)
{
	m_widgetToolbar = NULL;
    m_indexVideo = -1;
    m_rtmpPublisher = NULL;
    m_isStarted = false;
	m_refreshTimerId = 0;
	m_paintTimerId = this->startTimer(TIME_UPDATE_VIDEO,Qt::PreciseTimer);
	
    //setupUi();
}

void UserPubliserVideoWnd::setupUi()
{
	QVBoxLayout* vboxLayout = new QVBoxLayout(this);
	vboxLayout->setSpacing(0);    
	vboxLayout->setContentsMargins(1, 1, 1, 0);
	QSpacerItem* verSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	vboxLayout->addItem(verSpacer);

	m_widgetToolbar = new QWidget(this);    
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);    
	m_widgetToolbar->setSizePolicy(sizePolicy);
	m_widgetToolbar->setMinimumSize(QSize(0, 32));
	m_widgetToolbar->setMaximumSize(QSize(16777215, 32));
	vboxLayout->addWidget(m_widgetToolbar);

	m_widgetToolbar->setStyleSheet(QString("background-color: rgba(249, 249, 249, 204)"));

	QHBoxLayout* hboxLayout = new QHBoxLayout(m_widgetToolbar);
	QLabel* labelName = new QLabel(m_widgetToolbar);
	sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
	sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
	labelName->setSizePolicy(sizePolicy);
	labelName->setMinimumSize(QSize(200,23));
	labelName->setMaximumSize(QSize(200,23));
	labelName->setAlignment(Qt::AlignHCenter);
	labelName->setStyleSheet(QString("font-family: \"Microsoft YaHei\";font-size: 13px;color: rgb(28, 118, 250)"));
	labelName->setAttribute(Qt::WA_TranslucentBackground,true);

	hboxLayout->addWidget(labelName,0,Qt::AlignHCenter);
	labelName->setText(QString(QObject::tr("userVideoName")));

	m_widgetToolbar->hide();
}

bool UserPubliserVideoWnd::setVideoIndex(int nIndex,bool start /* = true */)
{
    if(m_indexVideo == nIndex)
    {
        return true;
    }

	setRtmpStream(NULL);

    if (NULL != m_rtmpPublisher 
       && m_rtmpPublisher->isStart() 
       && !m_isStarted)
    {	
        m_rtmpPublisher->stop();
        m_rtmpPublisher = NULL;
    }

    m_isStarted = false;
    m_rtmpPublisher = CMediaPublishMgr::getInstance()->getRtmpPublisherByCamera(nIndex);
    if(NULL == m_rtmpPublisher)
    {
        return false;
    }

	setRtmpStream(m_rtmpPublisher);

    if(m_rtmpPublisher->isStart() || !start)
    {
		m_indexVideo = nIndex;
        m_isStarted = true;
	
        return true;
    }

    if(m_rtmpPublisher->getUrl().empty())
    {
        m_rtmpPublisher->setUrl("rtmp://");
        m_rtmpPublisher->setPublish(false);
    }
	
    bool br = m_rtmpPublisher->start();
    if(br)
    {
        m_indexVideo = nIndex;
    }

    return br;
}

void UserPubliserVideoWnd::showRtmpVideoBuf(const RtmpVideoBuf& videoData)
{
    QMutexLocker autoLock(&m_mutexVideoBuf);
	copyAndTransVideoData(m_showVideoBuf,videoData);
}

void UserPubliserVideoWnd::paintEvent(QPaintEvent * event)
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

    return;
}

void UserPubliserVideoWnd::closeVideo()
{
    if (NULL == m_rtmpPublisher )
	{
		return;
	}

	setRtmpStream(NULL);
	if(!m_rtmpPublisher->isStart())
	{
		m_rtmpPublisher = NULL;
		return;
	}

	if(!m_isStarted)
	{
		m_rtmpPublisher->stop();
	}
		
    m_rtmpPublisher = NULL;	
    return;
}

void UserPubliserVideoWnd::timerEvent(QTimerEvent * event)
{
	if(NULL == event)
	{
		return;
	}

	int timerId = event->timerId();
	if(timerId == m_refreshTimerId)
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

		QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();

		return;
	}
	
	return;
}

void UserPubliserVideoWnd::showBar(bool show /* = true */)
{
    if(NULL == m_widgetToolbar)
    {
        return;
    }

    if(show)
    {
        m_widgetToolbar->show();
    }
    else
    {
        m_widgetToolbar->hide();
    }
}