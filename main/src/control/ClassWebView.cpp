#include "ClassWebView.h"
#include <QNetworkRequest>
#include <QWebFrame>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QList>
#include "common/Config.h"
#include "QClassWebDialog.h"

//////////////////////////////////////////////////////////////////////////
//JSESSIONID=
//QString g_strSessionId;
QList<QNetworkCookie> g_listCookie;

QClassWebPage::QClassWebPage(QObject *parent) :QWebPage(parent)  
{  
    QNetworkCookieJar*  cookieJar = new QNetworkCookieJar(this);
    QNetworkCookie cookie;

    foreach(cookie,g_listCookie)
    {
        cookieJar->insertCookie(cookie);
    }

    this->networkAccessManager()->setCookieJar(cookieJar);
}  

QClassWebPage::~QClassWebPage()
{

}

bool QClassWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)  
{     

    QString strUrl = request.url().toString();
    QString strWeb(Config::getConfig()->m_urlWebSite.c_str());

    if(strUrl.startsWith(strWeb))
    {
        QNetworkCookieJar*  cookieJar = this->networkAccessManager()->cookieJar();
        QList<QNetworkCookie> listCookie = cookieJar->cookiesForUrl(request.url());

        
        if(!listCookie.isEmpty())
        {
            g_listCookie.append(listCookie);
        }
    }

    return QWebPage::acceptNavigationRequest(frame, request, type);  
}


//////////////////////////////////////////////////////////////////////////
QClassWebView::QClassWebView(QWidget* parent /* = 0 */):QWebView(parent)
{
    QClassWebPage* newWebPage = new QClassWebPage(this);
    this->setPage(newWebPage);

    this->page()->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);
	this->page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows,true);
}

QClassWebView::~QClassWebView()
{

}

QWebView * QClassWebView::createWindow(QWebPage::WebWindowType type)
{
    QClassWebDialog * webDlg = new QClassWebDialog(QString(""),QString(QObject::tr("productName")),NULL);

	QClassWebView* webView = webDlg->getWebView();
	
    webDlg->setAttribute(Qt::WA_DeleteOnClose,true);

	if(type == QWebPage::WebModalDialog)
	{
		webDlg->setWindowModality(Qt::ApplicationModal);        
	}
    
    webDlg->show();    
		
	return webView;	
}