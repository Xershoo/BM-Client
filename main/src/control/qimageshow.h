#ifndef QIMAGESHOW_H
#define QIMAGESHOW_H

#include <QWidget>
#include <QImage>
class QWhiteBoardView;

class QImageShow : public QWidget
{
    Q_OBJECT

public:
    QImageShow(QWidget *parent = 0);
    ~QImageShow();

    bool openFile(QString filePath);
    void closeFile();

	void zoomIn();
	void zoomOut();

public:
	inline void    setUserId(__int64 userId);
	inline __int64 getUserId();

	inline void    setCourseId(__int64 cid);
	inline __int64 getCourseId();

	inline void    setWbCtrl(int nCtrl);
	inline int     getWbCtrl();

	inline QWhiteBoardView* getWhiteBoardView();
	
	void createWhiteboardView();
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject * obj, QEvent * event);

protected:
	QRect calcImagePaintRect(QRect& rectImage,int imageW, int imageH);
	void initZoomRadio();

	int calcFileId();
	
	void setWhiteboardViewBack();
protected:
    bool        m_fileOpen;
    QString     m_fileName;
	QImage		m_imgFile;
	float		m_zoomRatio;

	__int64     m_userId;
	__int64     m_courseId;
	int         m_wbCtrl;

	QWhiteBoardView * m_viewWhiteboard;
};


void QImageShow::setUserId(__int64 userId)
{
	m_userId = userId;
}

__int64 QImageShow::getUserId()
{
	return m_userId;
}

void QImageShow::setCourseId(__int64 cid)
{
	m_courseId = cid;
}

__int64 QImageShow::getCourseId()
{
	return m_courseId;
}

void QImageShow::setWbCtrl(int nCtrl)
{
	m_wbCtrl = nCtrl;
}

int QImageShow::getWbCtrl()
{
	return m_wbCtrl;
}

QWhiteBoardView* QImageShow::getWhiteBoardView()
{
	return m_viewWhiteboard;
}

#endif // QIMAGESHOW_H
