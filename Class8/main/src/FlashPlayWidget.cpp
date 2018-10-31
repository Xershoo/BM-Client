#include "FlashPlayWidget.h"
#include "courseware\courseware.h"
#include "common/Util.h"
#include <QtCore/QCoreApplication>
#include "./whiteboard/QWhiteBoard.h"

QFlashPlayWidget::QFlashPlayWidget(QWidget * parent) : QAxWidget(parent)	
	, m_viewWhiteboard(NULL)
	, m_userId(0)
	, m_courseId(0)	
	, m_wbCtrl(WB_CTRL_NONE)
{	
}

QFlashPlayWidget::~QFlashPlayWidget()
{

}

bool QFlashPlayWidget::play(const QString& file)
{
	bool br = false;
	br = this->setControl(QString::fromUtf8("{d27cdb6e-ae6d-11cf-96b8-444553540000}"));
	if(!br)
	{
		return false;
	}

	this->dynamicCall("LoadMovie(long,string)",0,file);
	m_file = file;

	return true;
}

long QFlashPlayWidget::getAllFrame()
{
	return 0;
}

long QFlashPlayWidget::getCurFrame()
{
	return 0;
}

void QFlashPlayWidget::gotoFrame(long frame)
{

}

void QFlashPlayWidget::pause(bool pause)
{

}

void QFlashPlayWidget::stop()
{

}

void QFlashPlayWidget::createWhiteboardView()
{
	__int64 idOwner = (__int64)calcFileId(); 
	m_viewWhiteboard = new QWhiteBoardView(this,false);
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	m_viewWhiteboard->setUserId(m_userId);
	m_viewWhiteboard->setPageId(0);
	m_viewWhiteboard->setCourseId(m_courseId);
	m_viewWhiteboard->setOwnerId(idOwner);
	m_viewWhiteboard->show();
	m_viewWhiteboard->installEventFilter(this);
	m_viewWhiteboard->setEnable((WBCtrl)m_wbCtrl);
	m_viewWhiteboard->setColor(WB_COLOR_BLUE);
	m_viewWhiteboard->setMode(WB_MODE_CURVE);
	m_viewWhiteboard->setTextSize(WB_SIZE_SMALL);
}

int QFlashPlayWidget::calcFileId()
{
	int nId = 0;
	char szFile[512] = { 0 };
	Util::QStringToAnsi(m_file,szFile,sizeof(szFile));

	if(strlen(szFile) <= 0)
	{
		return 0;
	}

	char * pszFileName = strrchr(szFile,'\\');
	if(NULL != pszFileName)
	{
		nId = (int)Util::crc32((unsigned char*)pszFileName,strlen(pszFileName));
	}

	return nId;
}

void QFlashPlayWidget::setWhiteboardViewBack()
{
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	QRect rectWidget= this->rect();
	m_viewWhiteboard->setSizeAndSceneRect(rectWidget); 	
	m_viewWhiteboard->setBackColor(QColor(255,255,255,0));
	m_viewWhiteboard->show();
	m_viewWhiteboard->update();
}

void QFlashPlayWidget::resizeEvent(QResizeEvent *event)
{	
	setWhiteboardViewBack();
	QAxWidget::resizeEvent(event);
}

bool QFlashPlayWidget::eventFilter(QObject * obj, QEvent * event)
{
	if(obj != m_viewWhiteboard)
	{
		return false;
	}

	if(event->type() == QEvent::KeyPress)
	{
		keyPressEvent((QKeyEvent*)event);
		m_viewWhiteboard->setFocus();
	}

	return false;
}