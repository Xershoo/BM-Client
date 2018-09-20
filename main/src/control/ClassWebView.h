#ifndef CLASS_WEB_VIEW_H
#define CLASS_WEB_VIEW_H

#include <QWebView>
#include <QWebPage>

class QClassWebPage : public QWebPage
{
    Q_OBJECT
public:
    explicit QClassWebPage(QObject *parent = 0);
    ~QClassWebPage();

protected:  
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type); 
};

class QClassWebView : public QWebView
{
	Q_OBJECT
public:
	explicit QClassWebView(QWidget* parent = 0);
	virtual ~QClassWebView();

protected:
	virtual QWebView *createWindow(QWebPage::WebWindowType type);
};

#endif