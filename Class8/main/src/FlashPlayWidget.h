#ifndef FLASH_PLAY_WIDGET
#define FLASH_PLAY_WIDGET

#include <ActiveQt/QAxWidget>

class QWhiteBoardView;

class QFlashPlayWidget:public QAxWidget
{
	Q_OBJECT
public:
	QFlashPlayWidget(QWidget* parent);
	~QFlashPlayWidget();

	bool play(const QString& file);
	long getCurFrame();
	long getAllFrame();
	void gotoFrame(long frame);
	void pause(bool pause);
	void stop();
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
	int calcFileId();
	void setWhiteboardViewBack();

	void resizeEvent(QResizeEvent *event);
	bool eventFilter(QObject * obj, QEvent * event);

protected:
	__int64     m_userId;
	__int64     m_courseId;
	int         m_wbCtrl;

	QString		m_file;
	QWhiteBoardView * m_viewWhiteboard;
};

void QFlashPlayWidget::setUserId(__int64 userId)
{
	m_userId = userId;
}

__int64 QFlashPlayWidget::getUserId()
{
	return m_userId;
}

void QFlashPlayWidget::setCourseId(__int64 cid)
{
	m_courseId = cid;
}

__int64 QFlashPlayWidget::getCourseId()
{
	return m_courseId;
}

void QFlashPlayWidget::setWbCtrl(int nCtrl)
{
	m_wbCtrl = nCtrl;
}

int QFlashPlayWidget::getWbCtrl()
{
	return m_wbCtrl;
}

QWhiteBoardView* QFlashPlayWidget::getWhiteBoardView()
{
	return m_viewWhiteboard;
}

#endif