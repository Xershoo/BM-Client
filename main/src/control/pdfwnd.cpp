//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：PDFWnd.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：PDF文档显示窗口封装类的实现源文件
//
//	修改记录：    
//  1.2015/12/20 增加生成PDF页面的预览图片
//**********************************************************************
#include "PdfWnd.h"
#include <QtGui/QPainter>
#include <QtCore/QTextCodec>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QtEvents>
#include <Windows.h>
#include "pdf/PDFShowUtil.h"
#include "WhiteBoard/QWhiteBoard.h"
#include "Env.h"
#include "Util.h"


QPDFWnd::QPDFWnd(QWidget* parent):QWidget(parent)
    ,m_fileOpen(PDF_FILE_CLOSE)
    ,m_fileName("")
    ,m_userId(0)
    ,m_courseId(0)
    ,m_curWhiteBoardView(NULL)
	,m_wbCtrl(WB_CTRL_NONE)
	,m_numOperation(0)
{
	initPDFApp();
    setFocusPolicy(Qt::ClickFocus);    
}

QPDFWnd::~QPDFWnd()
{
    closeFile();
}

bool QPDFWnd::openFile(QString& file)
{	
	if(file.isEmpty())
	{
		return false;
	}
	
	m_fileName = file;
	m_fileOpen = PDF_FILE_OPENING;
    QRect rect = this->rect();

	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doOpenFile(m_fileName,rect);

	return true;
}

bool QPDFWnd::nextPage()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doNextPage(m_fileName);
	return true;
}

bool QPDFWnd::prevPage()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doPrevPage(m_fileName);
	return true;
}

bool QPDFWnd::gotoPage(int page)
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doGotoPage(m_fileName,page);
	return true;
}

bool QPDFWnd::homePage()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doHomePage(m_fileName);
	return true;
}

bool QPDFWnd::lastPage()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doLastPage(m_fileName);
	return true;
}


void QPDFWnd::gotoPage(const QString& file,int page){

	if(m_fileName.compare(file,Qt::CaseInsensitive) != 0)
	{
		return;
	}

	gotoPage(page);
}

bool QPDFWnd::zoomIn()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doZoomIn(m_fileName);
	return true;
}

bool QPDFWnd::zoomOut()
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doZoomOut(m_fileName);
	return true;
}

bool QPDFWnd::zoomPage(float radio)
{
	::InterlockedIncrement((unsigned int*)&m_numOperation);
	emit doZoomPage(m_fileName,radio);
	return true;
}

void QPDFWnd::closeFile(bool needDelete /* = true */)
{
	if(!isFileOpen())
	{
		return;
	}

	UINT numOpera = _InterlockedExchange((long*)&m_numOperation,m_numOperation);
	if(numOpera <= 0)		
	{
		m_fileOpen = PDF_FILE_CLOSE;

		QPDFApp* appPDF = QPDFThread::getPDFApp();
		if(NULL != appPDF)
		{
			appPDF->closeFile(m_fileName);
		}
				
		m_fileName = QString("");
		m_lstWhiteBoardView.clear();

		if(needDelete)
		{
			this->disconnect(NULL,NULL,NULL,NULL);
			QWidget::close();
			delete this;
		}

		return;
	}
	
	m_fileOpen = needDelete ? PDF_FILE_CLOSE_DEL : PDF_FILE_CLOSE;
	emit doCloseFile(m_fileName);
	
	m_fileName = QString("");
	m_lstWhiteBoardView.clear();
}

int QPDFWnd::getPageCount()
{
	if(!isFileOpen() )
	{
		return 0;
	}

	QPDFApp* app = QPDFThread::getPDFApp();
	if(NULL == app)
	{
		return 0;
	}

	return app->getPageCount(m_fileName);
}

int QPDFWnd::getCurPage()
{
	if(!isFileOpen() )
	{
		return -1;
	}

	QPDFApp* app = QPDFThread::getPDFApp();
	if(NULL == app)
	{
		return -1;
	}

	return app->getCurPage(m_fileName);
}

QImage* QPDFWnd::getPagePreviewImage(int page)
{
	if(!isFileOpen() )
	{
		return NULL;
	}

	QPDFApp* app = QPDFThread::getPDFApp();
	if(NULL == app)
	{
		return NULL;
	}

	LPPDFSHOWIMAGE imageShow = app->getPreviewImage(m_fileName,page);
	if(NULL == imageShow)
	{
		return NULL;
	}

	return imageShow->_image;
}

void QPDFWnd::resizeEvent(QResizeEvent * event)
{
    if (!isFileOpen())
    {
        return;
    }
    
	QRect rcShow = this->rect();
    if(rcShow.width()&&rcShow.height()>0)
    {
		::InterlockedIncrement((unsigned int*)&m_numOperation);
		emit doResize(m_fileName,rcShow);
    }

	return;
}

void QPDFWnd::paintEvent(QPaintEvent * event)
{
    QPainter    painter(this);
    QRect       rect= this->rect();
        
    painter.fillRect(rect,QColor(178,178,178));

    if (!isFileOpen())
    {
        return;
    }

    if(m_curWhiteBoardView)
    {
        return;
    }
    
	QPDFApp* app = QPDFThread::getPDFApp();
	if(NULL == app)
	{
		return ;
	}

	pdf_show_image showImage;
	bool br = app->getCurImage(m_fileName,&showImage);
	if(!br)
	{
		return;
	}

	if(showImage._image == NULL || showImage._image->isNull())
	{
		return;
	}
    
    QPixmap pdfPixmap = QPixmap::fromImage(showImage._image->scaled(size(),Qt::KeepAspectRatio));
    painter.drawPixmap(showImage._xPos,showImage._yPos,showImage._width,showImage._height,pdfPixmap);    
}

void QPDFWnd::keyPressEvent(QKeyEvent * keyEvent)
{
    switch(keyEvent->key())
    {
    case Qt::Key_Right:
    case Qt::Key_Down:
        {
            nextPage();
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Up:
        {
            prevPage();
        }
		break;
	case Qt::Key_Home:
		{
			homePage();
		}
        break;
	case Qt::Key_End:
		{
			lastPage();
		}
		break;
    }
}

void QPDFWnd::wheelEvent(QWheelEvent * wheelEvent)
{
    if (!isFileOpen())
    {
        wheelEvent->ignore();
        return;
    }

    int numDegres = wheelEvent->delta() / 8;
    int numSteps = numDegres / 15 ; 

    if(numSteps > 0)
    {
        zoomOut();
    }
    else
    {
        zoomIn();
    }

    wheelEvent->accept();
}

void QPDFWnd::pdfWndSetCursor(const QString& file,int cursor)
{
    if(file.compare(m_fileName,Qt::CaseInsensitive) != 0)
	{
		return;
	}

    if(!isFileOpen())
    {
        return;
    }

    switch(cursor)
    {            
	case CURSOR_ARROW:
		{
			setCursor(QCursor(Qt::ArrowCursor));
		}
		break;
	case CURSOR_HAND:
		{
			setCursor(QCursor(Qt::OpenHandCursor));
		}
		break;
	case CURSOR_WAIT:
		{
			setCursor(QCursor(Qt::WaitCursor));
		}
		break;
	default:
		{
			setCursor(QCursor(Qt::ArrowCursor));
		}
		break;
	}
        
}

void QPDFWnd::pdfHandledEvent(const QString& file,int ctl,bool ret)
{
	if(m_fileName.compare(file,Qt::CaseInsensitive) != 0)
	{
		return;
	}

	if(m_fileOpen == PDF_FILE_CLOSE_DEL)
	{
		this->disconnect(NULL,NULL,NULL,NULL);
		QWidget::close();
		delete this;

		return;	
	}

	switch(ctl)
	{
	case PDF_OPEN:
		{
			if(!ret)
			{
				break;
			}

			m_fileOpen = PDF_FILE_OPEN;
			
			initWhiteBoardViewList();						
			setPreviewImage();
			resizeEvent(NULL);
		}

		break;
	case PDF_FLIP:
	case PDF_ZOOM:
	case PDF_RESIZE:
		{
			setWhiteBoardViewShow();
			
			QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();

			emit showPdfPage(m_fileName,getCurPage());
			
			if(ctl == PDF_FLIP)
            {
				emit doPdfCtrl(m_fileName, ctl);
			}
		}

		break;
	case PDF_CLOSE:
		{
			if(!ret)
			{
				break;
			}

			QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);//update();
		}

		break;
	}

	qApp->processEvents();
	::InterlockedDecrement((unsigned int*)&m_numOperation);
}

void QPDFWnd::setPreviewImage()
{
    if (!isFileOpen())
    {
        return;
    }

	QPDFApp* app = QPDFThread::getPDFApp();
	if(NULL == app)
	{
		return ;
	}

	int count = app->getPageCount(m_fileName);
	for(int i=0;i<count;i++)
	{
		LPPDFSHOWIMAGE pImage = app->getPreviewImage(m_fileName,i);
		if(NULL == pImage)
		{
			break;
		}

		emit addPdfPreviewPage(m_fileName,*pImage->_image,i);
	}
}

void QPDFWnd::initWhiteBoardViewList()
{
	if(!isFileOpen())
	{
		return;
	}

	int count =getPageCount();

    if( count <= 0)
    {
        return;
    }
    __int64 idOwner = (__int64)calcFileId(); 
    for(int i= 0;i<count;i++)
    {
        QWhiteBoardView * wbView = new QWhiteBoardView(this,false);
        wbView->setUserId(m_userId);
        wbView->setPageId(i);
        wbView->setCourseId(m_courseId);
        wbView->setOwnerId(idOwner);
        wbView->hide();
        wbView->installEventFilter(this);
        wbView->setEnable((WBCtrl)m_wbCtrl);
        wbView->setColor(WB_COLOR_BLUE);
        wbView->setMode(WB_MODE_CURVE);
        wbView->setTextSize(WB_SIZE_SMALL);

        m_lstWhiteBoardView.push_back(wbView);
    }
}

int QPDFWnd::calcFileId()
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

void QPDFWnd::initPDFApp()
{
	QPDFApp* appPDF = QPDFThread::getPDFApp();
	if(NULL != appPDF)
	{
		connect(this,SIGNAL(doOpenFile(const QString&,const QRect&)),appPDF,SLOT(openFile(const QString&,const QRect&)));
		connect(this,SIGNAL(doNextPage(const QString&)),appPDF,SLOT(nextPage(const QString&)));
		connect(this,SIGNAL(doPrevPage(const QString&)),appPDF,SLOT(prevPage(const QString&)));
		connect(this,SIGNAL(doHomePage(const QString&)),appPDF,SLOT(homePage(const QString&)));
		connect(this,SIGNAL(doLastPage(const QString&)),appPDF,SLOT(lastPage(const QString&)));
		connect(this,SIGNAL(doGotoPage(const QString&, int)),appPDF,SLOT(gotoPage(const QString&, int)));

		connect(this,SIGNAL(doZoomIn(const QString&)),appPDF,  SLOT(zoomIn(const QString&)));
		connect(this,SIGNAL(doZoomOut(const QString&)),appPDF, SLOT(zoomOut(const QString&)));
		connect(this,SIGNAL(doZoomPage(const QString&)),appPDF,SLOT(zoomPage(const QString&)));

		connect(this,SIGNAL(doResize(const QString&,const QRect&)),appPDF,SLOT(resize(const QString&,const QRect&)));
		connect(this,SIGNAL(doCloseFile(const QString&)),appPDF,SLOT(closeFile(const QString&)));

		connect(appPDF,SIGNAL(pdfWndSetCursor(const QString&,int)),this,SLOT(pdfWndSetCursor(const QString&,int)));
		connect(appPDF,SIGNAL(pdfHandledEvent(const QString&,int,bool)),this,SLOT(pdfHandledEvent(const QString&,int,bool)));
	}
}

void QPDFWnd::setWhiteBoardViewShow()
{
    if(m_lstWhiteBoardView.empty())
    {
        return;
    }

    int curPage = getCurPage();
    for(int i=0;i<m_lstWhiteBoardView.size();i++)
    {
        QWhiteBoardView * wbView = m_lstWhiteBoardView.at(i);
        if(NULL == wbView)
        {
            continue;
        }

        if(curPage != i)
        {
            wbView->hide();
			wbView->setBackPixmap(NULL);
            continue;
        }

        m_curWhiteBoardView = wbView;
        
		QPDFApp* app = QPDFThread::getPDFApp();
		if(NULL == app)
		{
			continue ;
		}

		pdf_show_image showImage;
		bool br = app->getCurImage(m_fileName,&showImage);
		if(!br)
		{
			continue;
		}

		if(showImage._image == NULL || showImage._image->isNull())
		{
			continue;
		}

        QRect   rcView(showImage._xPos,showImage._yPos,showImage._width,showImage._height);

        wbView->setSizeAndSceneRect(rcView);        
        wbView->show();
        wbView->setBackPixmap(&QPixmap::fromImage(*showImage._image));
		
		//xiewb 2018.07.24 如果不设置焦点将无法快捷翻页
		wbView->setFocus();
    }
}

bool QPDFWnd::eventFilter(QObject * obj, QEvent * event)
{
    if(obj != m_curWhiteBoardView)
    {
        return false;
    }

    if(event->type() == QEvent::KeyPress)
    {
        keyPressEvent((QKeyEvent*)event);
        m_curWhiteBoardView->setFocus();
    }

    return false;
}