#include "qimageshow.h"
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QFile>

QImageShow::QImageShow(QWidget *parent)
    : QWidget(parent),m_zoomRatio(1.0)
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
	m_zoomRatio = 1.0;
	QWidget::resizeEvent(event);
}

void QImageShow::paintEvent(QPaintEvent *event)
{
	QPainter    painter(this);
	QRect       rect= this->rect();

	painter.fillRect(rect,QColor(178,178,178));
	
	if (!m_fileOpen)
	{
		return;
	}

	if(m_imgFile.isNull())
	{
		return;
	}

	rect.setLeft(rect.left()+4);
	rect.setTop(rect.top()+4);

	rect.setRight(rect.right()-4);
	rect.setBottom(rect.bottom()-4);

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

	update();
}

void QImageShow::zoomOut()
{
	m_zoomRatio -= 0.2;
	if(m_zoomRatio <= 0.2)
	{
		m_zoomRatio = 0.2;
	}
	update();
}