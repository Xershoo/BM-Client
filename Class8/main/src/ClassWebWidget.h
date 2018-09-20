#ifndef CLASS_WEB_WIDGET_H
#define CLASS_WEB_WIDGET_H

#include <QtWidgets/QWidget>
#include "ClassWebBrowser.h"

class QClassWebWidget : public QWidget,public IClassCallback
{
	Q_OBJECT
public:
	QClassWebWidget(QWidget* parent=NULL);
	~QClassWebWidget();

	void openUrl(QString& strUrl);
	void setCallback(IClassCallback* pCallBack);

signals:
	void enterClassroom(QString courseID, QString classId);
	void playClassroom(QString courseID, QString classId);
	void webPageloadFinished();

public:
	virtual void onEnterClassRoom(__int64 nClassId,__int64 nCourseId);
	virtual void onPlayClassRoom(__int64 nClassId,__int64 nCourseId);
	virtual void onDocumentComplete();

protected:
	void resizeEvent(QResizeEvent *event);
	void timerEvent(QTimerEvent *event);
	
protected:
	void initWebBrowser();
	void unitWebBrowser();
	
protected:
	CClassWebBrowser*   m_webBrowser;
	BOOL			    m_initWebBrowser;
	int					m_showTimer;
};

#endif