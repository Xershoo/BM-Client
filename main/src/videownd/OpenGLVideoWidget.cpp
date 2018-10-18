#include "OpenGLVideoWidget.h"
#include <QPainter>
#include <QTimerEvent>
#include "common/macros.h"

#define SAFE_DELETE_BUF_IMAGE(i,b) if(i){delete i;i=NULL;};if(b){free(b);b=NULL;};

OpenGLVideoWidget::OpenGLVideoWidget(QWidget* parent,int videoType) :QOpenGLWidget(parent)
	,m_bufImage(NULL)
	,m_drawRect(false)
	,m_refreshTimerId(0)
	,m_renderImage(NULL)
	,m_paintTimerId(0)
	,m_videoWidth(0)
	,m_videoHeight(0)
	,m_isYUVData(0)
	,m_buffsize(0)
	,m_videoBuf(NULL)
	,m_autoShowBack(true)	
{
	loadBackImage(videoType);
	setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}

OpenGLVideoWidget::~OpenGLVideoWidget()
{
	QMutexLocker autoLockerRender(&m_mutexBufImage);
	SAFE_DELETE_BUF_IMAGE(m_bufImage,m_videoBuf);

	QMutexLocker autoLockerBuffer(&m_mutexRenderImage);
	SAFE_DELETE(m_renderImage);
}

void OpenGLVideoWidget::loadBackImage(int videoType)
{
	QString imageFile;
	switch (videoType)
	{
	case VIDEO_STUDENT:
		{
			imageFile = QString(":/res/res/image/default/student_video_back.png");
		}
		break;
	case VIDEO_TEACHER:
		{
			imageFile = QString(":/res/res/image/default/teacher_video_back.png");
		}
		break;
	case VIDEO_SET:
		{
			imageFile = QString(":/res/res/image/default/setting_video_back.png");
		}
		break;
	case VIDEO_COURSEWARE:
		{
			imageFile = QString(":/res/res/image/default/courseware_video_back.png");
		}
		break;
	default:
		{
			imageFile = QString(":/res/res/image/default/teacher_video_back.png");
		}
		break;
	}

	m_backImage.load(imageFile,"PNG");
}

void OpenGLVideoWidget::initializeGL()
{
	makeCurrent();
	initializeOpenGLFunctions();

	QColor mBgColor = QColor::fromRgb(255, 255, 255);
	float r = ((float)mBgColor.darker().red())/255.0f;
	float g = ((float)mBgColor.darker().green())/255.0f;
	float b = ((float)mBgColor.darker().blue())/255.0f;

	glClearColor(r,g,b,1.0f);
}

void OpenGLVideoWidget::resizeGL(int width, int height)
{
	makeCurrent();
	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, width, -height, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);

	updateScene();
}

void OpenGLVideoWidget::updateScene()
{
	if (!this->isVisible()) 
	{
		return;
	}
	
	QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);
}

void OpenGLVideoWidget::paintGL()
{
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	glPushMatrix();
	glRasterPos2i(0, 0);
	glPixelZoom(1, -1);

	QMutexLocker autoLocker(&m_mutexRenderImage);
	getRenderImage();
	if(NULL != m_renderImage && !m_renderImage->isNull())
	{
		glDrawPixels(m_renderImage->width(), m_renderImage->height(), GL_RGBA, GL_UNSIGNED_BYTE, m_renderImage->bits());
	}
	autoLocker.unlock();

	glPopMatrix();

	// end
	glFlush();	
}

void OpenGLVideoWidget::getRenderImage()
{	
	if(!this->isVisible())
	{
		return ;
	}
		
	QRect rectShow = this->rect();	
	
	if(NULL != m_bufImage)
	{
		SAFE_DELETE(m_renderImage);

		QMutexLocker autoLocker(&m_mutexBufImage);
		//xiewb 2018.10.17 width and height are added 1 to solve the problem of black side.
		int imgWidth = rectShow.width() + 1;
		int imgHeight = rectShow.height() + 1;
		QImage imageScaled = m_bufImage->scaled(QSize(imgWidth,imgHeight),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
		if(imageScaled.isNull())
		{
			return;
		}

		m_renderImage = new QImage(imageScaled);
		
		SAFE_DELETE_BUF_IMAGE(m_bufImage,m_videoBuf);
		autoLocker.unlock();

		if(m_refreshTimerId !=0)
		{
			killTimer(m_refreshTimerId);
			m_refreshTimerId = 0;
		}

		m_refreshTimerId = this->startTimer(3000,Qt::PreciseTimer);
	}
	else if(!m_backImage.isNull())
	{	
		if(NULL == m_renderImage)
		{
			m_renderImage = new QImage(rectShow.width(),rectShow.height(),QImage::Format_RGBA8888);
		
			QPainter painter(m_renderImage);
			QRect imageRect = calcImagePaintRect(rectShow,m_backImage.width(),m_backImage.height());
			QBrush backBrush(QColor(247,247,247,255));
		
			painter.fillRect(rectShow,backBrush);
			painter.drawImage(imageRect,m_backImage);
		}
	}
	else
	{
		return ;
	}
	
	if(m_drawRect)
	{
		QPainter painter(m_renderImage);
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

	return;
}

void OpenGLVideoWidget::showVideoBuffer(int videoWidth,int videoHeight,bool isYUVData,unsigned int buffsize,unsigned char* videoBuf)
{
	if(videoWidth <= 0 || videoHeight <= 0 || buffsize <=0 || NULL == videoBuf)
	{
		return;
	}
	
	QMutexLocker autoLocker(&m_mutexBufImage);

	if(isSameVideoBuffer(videoWidth,videoHeight,isYUVData,buffsize,videoBuf))
	{
		if(!m_autoShowBack&&m_refreshTimerId !=0)
		{
			killTimer(m_refreshTimerId);
			m_refreshTimerId = 0;
		}

		return;
	}
	
	SAFE_DELETE_BUF_IMAGE(m_bufImage,m_videoBuf);
	m_videoBuf = (unsigned char*)malloc(buffsize);
	if(NULL == m_videoBuf)
	{
		return;
	}

	memset(m_videoBuf,NULL,buffsize);
	memcpy(m_videoBuf,videoBuf,buffsize);

	QImage bufImage(m_videoBuf,videoWidth,videoHeight,QImage::Format_RGB888);	
	if(bufImage.isNull())
	{
		return;
	}
	
	if(!isYUVData)
	{
		QMatrix rotateMatrix;
		rotateMatrix.rotate(180);

		QImage tempImage = bufImage.transformed(rotateMatrix,Qt::SmoothTransformation);
		bufImage = tempImage.mirrored(true,false);
	}
	
	m_bufImage = new QImage(bufImage);
	updateScene();
}

QRect OpenGLVideoWidget::calcImagePaintRect(QRect& rectImage,int imageW, int imageH)
{	
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

void OpenGLVideoWidget::timerEvent(QTimerEvent *timerEvent)
{
	if(NULL == timerEvent)
	{
		return;
	}

	int timerId = timerEvent->timerId();
	if(timerId == m_refreshTimerId)
	{		
		QMutexLocker autoLocker(&m_mutexRenderImage);
		SAFE_DELETE(m_renderImage);

		this->killTimer(m_refreshTimerId);
		m_refreshTimerId = 0;

		updateScene();

		return;
	}

	if(timerId == m_paintTimerId)
	{
		if(!this->isVisible())
		{
			return;
		}

		updateScene();

		return;
	}
}

bool OpenGLVideoWidget::isSameVideoBuffer(int videoWidth,int videoHeight,bool isYUVData,unsigned int buffsize,unsigned char* videoBuf)
{
	if (m_isYUVData == isYUVData && 
		m_buffsize == buffsize && 
		m_videoWidth == videoWidth && 
		m_videoHeight == videoHeight &&
		NULL != m_videoBuf &&
		memcmp(videoBuf,m_videoBuf,buffsize) == 0)
	{
		return true; 
	}

	m_isYUVData = isYUVData ;
	m_buffsize = buffsize ;
	m_videoWidth = videoWidth ;
	m_videoHeight = videoHeight ;

	return false;
}

void OpenGLVideoWidget::repaint(bool delBufImage /* = false */)
{
	QMutexLocker autoLocker(&m_mutexRenderImage);
	SAFE_DELETE(m_renderImage);

	if(delBufImage && m_bufImage)
	{
		QMutexLocker autoLocker(&m_mutexBufImage);
		SAFE_DELETE_BUF_IMAGE(m_bufImage,m_videoBuf);
	}

	updateScene();
}