#ifndef PDF_SHOW_UTIL_H
#define PDF_SHOW_UITL_H

#include <QtWidgets/QWidget>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtGui/QImage>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QMutex>
#include "macros.h"
#include "../PDF/XPdfView/MPdfAPP.h"

enum pdf_ctrl
{
	PDF_NONE = 0,
	PDF_OPEN ,
	PDF_FLIP,
	PDF_ZOOM,
	PDF_RESIZE,
	PDF_CLOSE,

	PDF_FREE,
};

typedef struct pdf_show_image
{
public:
	pdf_show_image()
	{
		_xPos = 0;
		_yPos = 0;
		_width = 0;
		_height = 0;
		_size = 0;

		_data = NULL;
		_image = NULL;
	}

	pdf_show_image(int x,int y,int w,int h,int l,unsigned char * data)
	{
		_xPos = x;
		_yPos = y;
		_width = w;
		_height = h;
		_size = l;

		if(_size > 0 )
		{
			_data = (unsigned char *)malloc(_size);
			memset(_data,0,_size);

			memcpy(_data,data,_size);

			_image = new QImage(_data,_width,_height,_width * 4,QImage::Format_ARGB32);
		}
		else
		{
			_data = NULL;
			_image = NULL;
		}
	}

	~pdf_show_image()
	{
		_xPos = 0;
		_yPos = 0;
		_width = 0;
		_height = 0 ;
		_size = 0;

		SAFE_DELETE(_image);
		SAFE_FREE(_data);
	}

	pdf_show_image& operator=(const pdf_show_image& a)
	{
		_xPos = a._xPos;
		_yPos = a._yPos;
		_width = a._width;
		_height = a._height;
		_size = a._size;

		if(_size > 0 )
		{
			_data = (unsigned char *)malloc(_size);
			memset(_data,0,_size);

			memcpy(_data,a._data,_size);

			_image = new QImage(_data,_width,_height,_width * 4,QImage::Format_ARGB32);
		}
		else
		{
			_data = NULL;
			_image = NULL;
		}

		return *this;
	}
public:
	int _xPos;
	int _yPos;
	int _width;
	int _height;

	int _size;
	unsigned char * _data;
	QImage* _image;

}PDFSHOWIMAGE,*LPPDFSHOWIMAGE;

typedef QVector<LPPDFSHOWIMAGE> PDFSHOWIMAGELIST;

typedef struct _pdf_file_info
{
public:
	_pdf_file_info()
	{
		_app = NULL;
	}

	~_pdf_file_info()
	{
		SAFE_DELETE(_app);
		
		for(int i=0; i<_vec_prev_image.size();i++)
		{
			LPPDFSHOWIMAGE pImage = _vec_prev_image.at(i);
			SAFE_DELETE(pImage);
		}
	}

	QString				_file;
	LPPDFAPP			_app;
	PDFSHOWIMAGELIST	_vec_prev_image;
}PDFFILEINFO,*LPPDFFILEINFO;

typedef QMap<QString,LPPDFFILEINFO> PDFFileAPPMap;

class QPDFApp : public QObject
{
	Q_OBJECT
public:
	explicit QPDFApp(QObject* parent = NULL);
	~QPDFApp();

public:
	int  getPageCount(const QString& file);
	int  getCurPage(const QString& file);
	bool getCurImage(const QString& file,pdf_show_image* showImage);
	
	LPPDFSHOWIMAGE getPreviewImage(const QString& file,int nPage);
public slots:
	bool openFile(const QString& file,const QRect& rect);
	bool nextPage(const QString& file);
	bool prevPage(const QString& file);
	bool homePage(const QString& file);
	bool lastPage(const QString& file);
	bool gotoPage(const QString& file,int page);

	bool zoomIn(const QString& file);
	bool zoomOut(const QString& file);
	bool zoomPage(const QString& file,float radio);

	bool resize(const QString& file,const QRect& rect);

	void closeFile(const QString& file);
public:
	void doPDFCallback(pdf_callback_param param);

signals:
	void pdfWndSetCursor(const QString& file,int cursor);
	void pdfHandledEvent(const QString& file,int ctl,bool ret);

protected:
	void freePDFFileApp();

	bool savePreviewImageList(LPPDFFILEINFO pInfo);

private:
	QMutex			m_mutexMapPDFFileApp;
	PDFFileAPPMap	m_mapPDFFileApp;
};

class QPDFThread : public QThread
{
	Q_OBJECT
public:
	QPDFThread(QObject* parent);
	~QPDFThread();

public:
	static QPDFApp* getPDFApp();;

public:
	static void createPDFThread();
	static void destroyPDFThread();

protected:
	virtual void run();

private:
	QPDFApp*	m_pdfApp;

	static QPDFThread * m_thrdPDFWork;
};

#endif