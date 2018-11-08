#ifndef OPENGL_VIDEO_WIDGET
#define OPENGL_VIDEO_WIDGET

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtGui/QImage>

class OpenGLVideoWidget: public QOpenGLWidget,
	protected QOpenGLFunctions
{
	Q_OBJECT

public:
	enum VIDEO_TYPE
	{
		VIDEO_STUDENT,
		VIDEO_TEACHER,
		VIDEO_SET,
		VIDEO_COURSEWARE
	};

public:
	OpenGLVideoWidget(QWidget* parent,int videoType);
	virtual ~OpenGLVideoWidget();

	inline void setDrawVideoRect(bool drawRect)
	{
		m_drawRect = drawRect;
	};

	inline void setAutoShowBack(bool autoShowBack)
	{
		m_autoShowBack = autoShowBack;
	};

	inline void setVideoKeepRadio(bool keepRadio){
		m_videoKeepRadio = keepRadio;
	};

	void repaint(bool delBufImage = false);
public:
	virtual void showVideoBuffer(int videoWidth,int videoHeight,bool isYUVData,unsigned int buffsize,unsigned char*  videoBuf);

protected:
	virtual void initializeGL();						// OpenGL initialization
	virtual void paintGL();								// OpenGL Rendering
	virtual void resizeGL(int width, int height);       // Widget Resize Event
	
protected:
	virtual void timerEvent(QTimerEvent *timerEvent);

protected:
	virtual void 	getRenderImage();
	virtual void	drawRenderImage();
	virtual void	updateScene();

	virtual void	loadBackImage(int videoType);
	virtual QRect	calcImagePaintRect(QRect& rectImage,int imageW, int imageH);

	virtual bool	isSameVideoBuffer(int videoWidth,int videoHeight,bool isYUVData,unsigned int buffsize,unsigned char*  videoBuf);

	virtual void	drawVideoOnImage();
	virtual void	drawNoVideoImage();
	virtual void	drawLocationRect();
protected:
	QMutex	m_mutexRenderImage;
	QImage* m_renderImage;

	QMutex	m_mutexBufImage;
	QImage* m_bufImage;
	QImage  m_backImage;
	unsigned char*  m_videoBuf;

	bool	m_drawRect;

	int		m_refreshTimerId;
	int		m_paintTimerId;

	int		m_videoWidth;
	int		m_videoHeight;
	bool	m_isYUVData;
	UINT	m_buffsize;
	bool	m_autoShowBack;
	bool	m_videoKeepRadio;
};

#endif