#include "qimageshow.h"
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QFile>
#include "./common/Util.h"
#include "./whiteboard/QWhiteBoard.h"

QImageShow::QImageShow(QWidget *parent)
    : QWidget(parent),m_zoomRatio(1.0)
	, m_viewWhiteboard(NULL)
	, m_userId(0)
	, m_courseId(0)	
	, m_wbCtrl(WB_CTRL_NONE)
{
	setFocusPolicy(Qt::ClickFocus);    
}

QImageShow::~QImageShow()
{
    closeFile();
}

bool QImageShow::openFile(QString filePath)
{	
    if (filePath.isEmpty()||!QFile::exists(filePath))
    {
        return false;
    }
	
	closeFile();
	
	bool br = m_imgFile.load(filePath);
	if(!br)
	{
		return false;
	}

	m_fileName = filePath;
	m_fileOpen = true;	

	initZoomRadio();	
	update();

	return true;
}

void QImageShow::closeFile()
{    
	m_imgFile.detach();
    m_fileName = "";
    m_fileOpen = false;
	m_zoomRatio = 1.0;
}

void QImageShow::resizeEvent(QResizeEvent *event)
{
	initZoomRadio();
	setWhiteboardViewBack();
	QWidget::resizeEvent(event);
}

void QImageShow::paintEvent(QPaintEvent *event)
{
	QPainter    painter(this);
	QRect       rect= this->rect();

	painter.fillRect(rect,QColor(242, 243, 248));
	
	if (!m_fileOpen)
	{
		return;
	}

	if(m_viewWhiteboard)
	{
		return;
	}

	if(m_imgFile.isNull())
	{
		return;
	}

	/*
	rect.setLeft(rect.left()+4);
	rect.setTop(rect.top()+4);

	rect.setRight(rect.right()-4);
	rect.setBottom(rect.bottom()-4);
	*/

	float width = m_imgFile.width() * m_zoomRatio;//(float)rect.width() * m_zoomRatio;
	float height = m_imgFile.height() * m_zoomRatio;//(float)rect.height() * m_zoomRatio;

	QSize sizePixmap((int)width,(int)height);
	QPixmap imgPixmap = QPixmap::fromImage(m_imgFile.scaled(sizePixmap,Qt::KeepAspectRatio));
	QRect imageRect = QRect(0,0,imgPixmap.width(),imgPixmap.height());
	QRect drawRect = calcImagePaintRect(rect,imgPixmap.width(),imgPixmap.height());

	painter.drawPixmap(drawRect,imgPixmap,imageRect);
}


QRect QImageShow::calcImagePaintRect(QRect& rectImage,int imageW, int imageH)
{
	//QRect rectPaint(rectImage.left()+1,rectImage.top()+1,rectImage.width()-2,rectImage.height()-2);
	QRect rectPaint = rectImage;
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

		int nShowWidth = rectPaint.width() > imageW ? rectPaint.width() : imageW;
		int nShowHeight = rectPaint.height() > imageH ? rectPaint.height() : imageH;

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

void QImageShow::zoomIn()
{
	m_zoomRatio += 0.2;
	if(m_zoomRatio >= 4.0)
	{
		m_zoomRatio = 4.0;
	}

	setWhiteboardViewBack();
	update();	
}

void QImageShow::zoomOut()
{
	m_zoomRatio -= 0.2;
	if(m_zoomRatio <= 0.2)
	{
		m_zoomRatio = 0.2;
	}

	setWhiteboardViewBack();
	update();
}

void QImageShow::initZoomRadio()
{
	QRect rectWidget = this->rect();
	QSize sizeImage(m_imgFile.width(),m_imgFile.height());
	
	/*
	if (m_imgFile.width()  <= (sizeImage.width() - 4) &&
		m_imgFile.height()  <= (sizeImage.height() - 4))
	{
		m_zoomRatio = 1.0;
	}
	

	float raidoWidth =  (float)(rectWidget.width() - 4) / (float)m_imgFile.width();
	float raidoHeight = (float)(rectWidget.height() - 4) / (float)m_imgFile.height();
	*/

	float raidoWidth =  (float)(rectWidget.width()) / (float)m_imgFile.width();
	float raidoHeight = (float)(rectWidget.height()) / (float)m_imgFile.height();

	char szRadio[128]={0};
	if(raidoWidth<raidoHeight)
	{
		sprintf(szRadio,"%0.2f",raidoWidth);
	}
	else
	{
		sprintf(szRadio,"%0.2f",raidoHeight);
	}

	m_zoomRatio = (float)atof(szRadio);
}

int QImageShow::calcFileId()
{
	int nId = 0;
	char szFile[512] = { 0 };
	Util::QStringToAnsi(m_fileName,szFile,sizeof(szFile) - 1);

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

void QImageShow::createWhiteboardView()
{
	if (!m_fileOpen)
	{
		return;
	}

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

void QImageShow::setWhiteboardViewBack()
{
	if(NULL == m_viewWhiteboard)
	{
		return;
	}

	QRect rectWidget= this->rect();

	float width = m_imgFile.width() * m_zoomRatio;
	float height = m_imgFile.height() * m_zoomRatio;

	QSize sizePixmap((int)width,(int)height);
	QPixmap imgPixmap = QPixmap::fromImage(m_imgFile.scaled(sizePixmap,Qt::KeepAspectRatio));
	QRect imageRect = QRect(0,0,imgPixmap.width(),imgPixmap.height());
	/*
	rectWidget.setLeft(rectWidget.left()+4);
	rectWidget.setTop(rectWidget.top()+4);
	rectWidget.setRight(rectWidget.right()-4);
	rectWidget.setBottom(rectWidget.bottom()-4);
	*/
	QRect rcView = calcImagePaintRect(rectWidget,imgPixmap.width(),imgPixmap.height());

	m_viewWhiteboard->setSizeAndSceneRect(rcView); 	
	m_viewWhiteboard->setBackPixmap(&imgPixmap);
	m_viewWhiteboard->show(); 
}

bool QImageShow::eventFilter(QObject * obj, QEvent * event)
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