//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：PDFWnd.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.11.23
//	说明：PDF文档显示窗口封装类的定义头文件
//
//	修改记录：    
//  1.2015/12/20 增加生成PDF页面的预览图片
//  2.2016/01/04 修改PDF打开、翻页等为异步
//**********************************************************************
#ifndef PDF_SHOW_WIDGET_H
#define PDF_SHOW_WIDGET_H

#include <QtWidgets\QWidget>
#include <QtCore\QVector>
#include <common/varname.h>

class QWhiteBoardView;

typedef QVector<QWhiteBoardView*>   QWhiteBoardViewList;

class QPDFWnd : public QWidget
{
	enum
	{
		PDF_FILE_CLOSE = 0,
		PDF_FILE_OPENING,
		PDF_FILE_OPEN,
		PDF_FILE_CLOSE_DEL,
	};

    Q_OBJECT
public:
    QPDFWnd(QWidget* parent);
    ~QPDFWnd();

    bool openFile(QString& file);

    bool nextPage();
    bool prevPage();
    bool gotoPage(int page);
	bool homePage();
	bool lastPage();

	bool zoomIn();
	bool zoomOut();
	bool zoomPage(float radio);

    void closeFile(bool needDelete = true);
    
    int  getPageCount();
	int  getCurPage();

	QImage* getPagePreviewImage(int page);
public:
    inline void    setUserId(__int64 userId);
    inline __int64 getUserId();

    inline void    setCourseId(__int64 cid);
    inline __int64 getCourseId();

    inline void    setWbCtrl(int nCtrl);
    inline int     getWbCtrl();

    inline QWhiteBoardView* getCurWhiteBoardView();

	inline bool isFileOpen()
	{
		return (m_fileOpen == PDF_FILE_CLOSE || m_fileOpen == PDF_FILE_CLOSE_DEL) ? false : true;
	};
public:
    inline QString getFile()
    {
        return m_fileName;
    };

public slots:
    void gotoPage(const QString& file,int page);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void wheelEvent(QWheelEvent *);

    virtual bool eventFilter(QObject * obj, QEvent * event);

protected:
    void  initWhiteBoardViewList();
    void  setWhiteBoardViewShow();
	void  setPreviewImage();

    int   calcFileId();
	void  initPDFApp();

signals:
    void addPdfPreviewPage(const QString& file,const QImage& image,int index);    
	void showPdfPage(const QString& file,int index);

	void doOpenFile(const QString& file,const QRect& rect);
	void doNextPage(const QString& file);
	void doPrevPage(const QString& file);
	void doHomePage(const QString& file);
	void doLastPage(const QString& file);
	void doGotoPage(const QString& file,int page);
		 
	void doZoomIn(const QString& file);
	void doZoomOut(const QString& file);
	void doZoomPage(const QString& file,float radio);
		 
	void doResize(const QString& file,const QRect& rect);		 
	void doCloseFile(const QString& file);

    void doPdfCtrl(const QString& file, int nCtrl);

protected slots:
	void pdfHandledEvent(const QString& file,int ctl,bool ret);
	void pdfWndSetCursor(const QString& file,int cursor);

protected:
    int			m_fileOpen;
    QString     m_fileName;

    QWhiteBoardView*    m_curWhiteBoardView;    
    QWhiteBoardViewList m_lstWhiteBoardView;
    
    __int64     m_userId;
    __int64     m_courseId;
    int         m_wbCtrl;
	UINT		m_numOperation;
};

void QPDFWnd::setUserId(__int64 userId)
{
    m_userId = userId;
}

__int64 QPDFWnd::getUserId()
{
    return m_userId;
}

void QPDFWnd::setCourseId(__int64 cid)
{
    m_courseId = cid;
}

__int64 QPDFWnd::getCourseId()
{
    return m_courseId;
}

void QPDFWnd::setWbCtrl(int nCtrl)
{
    m_wbCtrl = nCtrl;
}

int QPDFWnd::getWbCtrl()
{
    return m_wbCtrl;
}

QWhiteBoardView* QPDFWnd::getCurWhiteBoardView()
{
    return m_curWhiteBoardView;
}

#endif