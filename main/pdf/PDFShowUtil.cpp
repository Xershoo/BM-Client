#include "PDFShowUtil.h"
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QMutexLocker>

#define PREVIEW_IMAGE_WIDTH		(80)
#define PREVIEW_IMAGE_HIEGHT	(60)

void pdfEventCallback(pdf_callback_param cbParam)
{
	if(cbParam.app == NULL)
	{
		return;
	}

	QPDFApp * appPDF =(QPDFApp*)(cbParam.app->userdata);
	if(NULL != appPDF)
	{
		appPDF->doPDFCallback(cbParam);
	}
}

//////////////////////////////////////////////////////////////////////////
//
QPDFApp::QPDFApp(QObject* parent /* = NULL */):QObject(parent)
{
	pdfapp_setcallback(pdfEventCallback);
}

QPDFApp::~QPDFApp()
{
	freePDFFileApp();
}

void QPDFApp::doPDFCallback(pdf_callback_param param)
{
	if(m_mapPDFFileApp.empty())
	{
		return;
	}

	QString file;
	for(PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.begin();itr != m_mapPDFFileApp.end();itr++)
	{
		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			continue;
		}

		if(pInfo->_app == param.app)
		{
			file = itr.key();
			break;
		}
	}

	if(file.isEmpty())
	{
		return;
	}

	if(param.event == PDFAPP_SET_CURSOR)
	{
		emit pdfWndSetCursor(file,param.data.cursor);
	}

	return;
}

int  QPDFApp::getPageCount(const QString& file)
{
	QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

	PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
	if(itr == m_mapPDFFileApp.end())
	{
		return 0;
	}

	LPPDFFILEINFO pInfo = *itr;
	if(NULL == pInfo)
	{
		return 0;
	}

	LPPDFAPP app = pInfo->_app;
	if(NULL == app)
	{
		return 0;
	}

	return app->pagecount;
}

int  QPDFApp::getCurPage(const QString& file)
{
	QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

	PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
	if(itr == m_mapPDFFileApp.end())
	{
		return -1;
	}

	LPPDFFILEINFO pInfo = *itr;
	if(NULL == pInfo)
	{
		return -1;
	}

	LPPDFAPP app = pInfo->_app;
	if(NULL == app)
	{
		return -1;
	}

	int page =  app->pageno;
	page--;

	return page;
}

bool QPDFApp::getCurImage(const QString& file,pdf_show_image* showImage)
{
	QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

	bool ret = false;

	do 
	{
		if(NULL == showImage)
		{
			break;
		}

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		pdf_show_param showParam = { 0 };
		ret = (bool)pdfapp_get_showparam(app,&showParam);
		if(!ret)
		{
			break;
		}

		*showImage = pdf_show_image(showParam.panx,
			showParam.pany,
			showParam.image_w,
			showParam.image_h,
			showParam.samples_l,
			showParam.samples);

		pdfapp_free_showparam(&showParam);
	}while(false);

	return ret;
}

LPPDFSHOWIMAGE QPDFApp::getPreviewImage(const QString& file,int nPage)
{
	QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

	PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
	if(itr == m_mapPDFFileApp.end())
	{
		return NULL;
	}

	LPPDFFILEINFO pInfo = *itr;
	if(NULL == pInfo)
	{
		return NULL;
	}

	if(nPage >= pInfo->_vec_prev_image.size())
	{
		return NULL;
	}

	return pInfo->_vec_prev_image.at(nPage);
}

bool QPDFApp::openFile(const QString& file,const QRect& rect)
{
	bool ret = false;
	LPPDFFILEINFO pInfo = NULL;

	emit pdfWndSetCursor(file,CURSOR_WAIT);

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		if(file.isEmpty())
		{
			break;
		}

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr != m_mapPDFFileApp.end())
		{
			ret = true;
			break;
		}

		LPPDFAPP app = new PDFAPP;
		if(NULL == app)
		{
			break;
		}

		memset(app,NULL,sizeof(PDFAPP));

		pInfo = new PDFFILEINFO;
		if(NULL == pInfo)
		{
			SAFE_DELETE(app);
			break;
		}

		pInfo->_file = file;
		pInfo->_app = app;

		ret = ::pdfapp_init(app);
		if(!ret)
		{
			break;
		}

		app->userdata = (void*)this;
		QDesktopWidget* desktopWidget = QApplication::desktop();    
		QRect screenRect = desktopWidget->screenGeometry();

		app->scrw = screenRect.width();
		app->scrh = screenRect.height();

		QByteArray fArray = QTextCodec::codecForLocale()->fromUnicode(file);
		ret = (bool)::pdfapp_open_with_file(app,(char*)fArray.data(),"",0);
		if(!ret)
		{
			break;
		}

        if(rect.width() > 0 && rect.height() > 0)
        {
            ::pdfapp_onresize(app,rect.width(),rect.height(),false);
        }
		
		//xiewb 2017/04/28
		//ret = savePreviewImageList(pInfo);
		//if(!ret)
		//{
		//	break;
		//}

		m_mapPDFFileApp.insert(file,pInfo);
		ret = true;
	} while (false);

	if(!ret)
	{
		SAFE_DELETE(pInfo);
	}

	emit pdfHandledEvent(file,PDF_OPEN,ret);
	emit pdfWndSetCursor(file,CURSOR_ARROW);

	return ret;
}

bool QPDFApp::nextPage(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_nextpage(app);
	} while (false);

	emit pdfHandledEvent(file,PDF_FLIP,ret);

	return ret;
}

bool QPDFApp::prevPage(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_prevpage(app);
	} while (false);

	emit pdfHandledEvent(file,PDF_FLIP,ret);

	return ret;
}

bool QPDFApp::homePage(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_homepage(app);
	} while (false);

	emit pdfHandledEvent(file,PDF_FLIP,ret);

	return ret;
}

bool QPDFApp::lastPage(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_lastpage(app);
	} while (false);

	emit pdfHandledEvent(file,PDF_FLIP,ret);

	return ret;
}

bool QPDFApp::gotoPage(const QString& file,int page)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		page++;
		ret = pdfapp_gotopage(app,page);
	} while (false);

	emit pdfHandledEvent(file,PDF_FLIP,ret);

	return ret;
}

bool QPDFApp::zoomIn(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_zoomin(app);

	} while (false);

	emit pdfHandledEvent(file,PDF_ZOOM,ret);

	return ret;
}

bool QPDFApp::zoomOut(const QString& file)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_zoomout(app);
	} while (false);

	emit pdfHandledEvent(file,PDF_ZOOM,ret);

	return ret;
}

bool QPDFApp::zoomPage(const QString& file,float radio)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		ret = pdfapp_zoom(app,radio);
	} while (false);
	
	emit pdfHandledEvent(file,PDF_ZOOM,ret);

	return ret;
}	

bool QPDFApp::resize(const QString& file,const QRect& rect)
{
	bool ret = false;

	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
		if(NULL == pInfo)
		{
			break;
		}

		LPPDFAPP app = pInfo->_app;
		if(NULL == app)
		{
			break;
		}

		pdfapp_onresize(app,rect.width(),rect.height(),false);

		ret = true;
	} while (false);

	emit pdfHandledEvent(file,PDF_RESIZE,ret);

	return ret;
}

void QPDFApp::closeFile(const QString& file)
{
	bool ret = false;
	
	do 
	{
		QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

		PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.find(file);
		if(itr == m_mapPDFFileApp.end())
		{
			break;
		}

		LPPDFFILEINFO pInfo = *itr;
        m_mapPDFFileApp.erase(itr);
        if(NULL == pInfo)
        {
            break;
        }

        LPPDFAPP app = pInfo->_app;
        if(NULL == app)
        {
            break;
        }

        pdfapp_close(app);
		SAFE_DELETE(pInfo);	

		ret = true;
	} while (false);
	
	emit pdfHandledEvent(file,PDF_CLOSE,ret);

	return ;
}

void QPDFApp::freePDFFileApp()
{
	QMutexLocker AutoLock(&m_mutexMapPDFFileApp);

	if(m_mapPDFFileApp.empty())
	{
		return;
	}

	for(PDFFileAPPMap::Iterator itr = m_mapPDFFileApp.begin();itr != m_mapPDFFileApp.end();itr++)
	{
		LPPDFFILEINFO pInfo = *itr;
		SAFE_DELETE(pInfo);
	}

	m_mapPDFFileApp.clear();
}

bool QPDFApp::savePreviewImageList(LPPDFFILEINFO pInfo)
{
	if(NULL == pInfo || NULL == pInfo->_app || 0 >= pInfo->_app->pagecount)
	{
		return false;
	}

	LPPDFAPP app = pInfo->_app;
    bool ret = true;

	pdfapp_homepage(app);

	do 
	{
		pdf_show_param showParam;
		ret = pdfapp_get_showparam(app,&showParam);
		if(!ret)
		{
			break;
		}

		LPPDFSHOWIMAGE pImage = new pdf_show_image(showParam.panx,
			showParam.pany,
			showParam.image_w,
			showParam.image_h,
			showParam.samples_l,
			showParam.samples);
		if(NULL == pImage)
		{
			pdfapp_free_showparam(&showParam);
			ret  = false;
			break;
		}

        QSize sizeImage(PREVIEW_IMAGE_WIDTH,PREVIEW_IMAGE_HIEGHT);
		
		QImage * prevImage = new QImage(pImage->_image->scaled(sizeImage,Qt::KeepAspectRatio));
       
		SAFE_DELETE(pImage->_image);
		SAFE_FREE(pImage->_data);
		pImage->_size = 0;
		pImage->_image = prevImage;

		pInfo->_vec_prev_image.push_back(pImage);
		pdfapp_free_showparam(&showParam);

	} while (pdfapp_nextpage(app));

	pdfapp_homepage(app);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
//
QPDFThread* QPDFThread::m_thrdPDFWork = NULL;

QPDFThread::QPDFThread(QObject* parent) : QThread(parent)
	,m_pdfApp(NULL)
{

}

QPDFThread::~QPDFThread()
{
	SAFE_DELETE(m_pdfApp);
}

void QPDFThread::run()
{
	m_pdfApp = new QPDFApp(NULL);
	if(NULL == m_pdfApp)
	{
		return;
	}

	this->exec();

	SAFE_DELETE(m_pdfApp);

	return;
}

QPDFApp* QPDFThread::getPDFApp()
{
	if(NULL == m_thrdPDFWork)
	{
		return NULL;
	}

	return m_thrdPDFWork->m_pdfApp;
}

void QPDFThread::createPDFThread()
{
	if(NULL != m_thrdPDFWork)
	{
		return;
	}

	m_thrdPDFWork = new QPDFThread(NULL);
	if(m_thrdPDFWork)
	{
		m_thrdPDFWork->start();
	}

	return;
}

void QPDFThread::destroyPDFThread()
{
	SAFE_DELETE(m_thrdPDFWork);
}