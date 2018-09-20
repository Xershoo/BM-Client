#include "QClassWebDialog.h"
#include "common/Env.h"
#include <QtGlobal>
#include <QWebFrame>
#include <QDesktopServices>
#include <QWebSettings>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include "./session/classsession.h"
#include "lobbydialog.h"
#include "common/Config.h"

QClassWebDialog::QClassWebDialog(QString url,QString title,QWidget *parent /* = 0 */)
	: C8CommonWindow(parent)
{
	ui.setupUi(this);
	
	setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);    

	connect(ui.gifIconPushButton_close, SIGNAL(clicked()), this, SLOT(onClose()));
	connect(ui.gifIconPushButton_minSize, SIGNAL(clicked()), this, SLOT(onMinSize()));
	connect(ui.gifIconPushButton_maxSize, SIGNAL(clicked()), this, SLOT(onMaxSize()));

	QString iconPath = Env::currentThemeResPath();
	ui.gifIconPushButton_minSize->setIconPath(iconPath + "gificon_minSize_normal.gif", iconPath + "gificon_minSize_hover.gif", iconPath + "gificon_minSize_pressed.gif");
	ui.gifIconPushButton_close->setIconPath(iconPath + "gificon_close_normal.gif", iconPath + "gificon_close_hover.gif", iconPath + "gificon_close_pressed.gif");	
	ui.gifIconPushButton_maxSize->setIconPath(iconPath + "gificon_maxSize_normal.gif", iconPath + "gificon_maxSize_hover.gif", iconPath + "gificon_maxSize_pressed.gif");
    ui.label_title->setText(title);

	ui.webView->setContextMenuPolicy(Qt::NoContextMenu);	

	connect(ui.webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addObjectToJs()));

	QNetworkDiskCache* netDiskCache = new QNetworkDiskCache(this);    
	QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);     
	netDiskCache->setCacheDirectory(location);    
	ui.webView->page()->networkAccessManager()->setCache(netDiskCache);

	openWebPage(url);

    CWebDlgMgr::getInstance()->addWebDlg(this);
}

QClassWebDialog::~QClassWebDialog()
{
    CWebDlgMgr::getInstance()->delWebDlg(this);
}

void QClassWebDialog::setTitleBarRect()
{
	QPoint pt = ui.label_title->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.toolWidget->size());
}

void QClassWebDialog::onClose()
{
	QDialog::close();    
}

void QClassWebDialog::onMinSize()
{
	QDialog::showMinimized();
}

void QClassWebDialog::onMaxSize()
{
    QString iconPath=Env::currentThemeResPath();
    QString iconNoraml;
    QString iconHover;
    QString iconPress;

    if (this->isMaximized())
    {
        iconNoraml = iconPath + "gificon_maxSize_normal.gif";
        iconHover = iconPath + "gificon_maxSize_hover.gif";
        iconPress = iconPath + "gificon_maxSize_pressed.gif";
        QDialog::showNormal();
    }
    else
    {
        iconNoraml = iconPath + "gificon_showNormal_normal.gif";
        iconHover = iconPath + "gificon_showNormal_hover.gif";
        iconPress = iconPath + "gificon_showNormal_pressed.gif";
        QDialog::showMaximized();
    }

	ui.gifIconPushButton_maxSize->setIconPath(iconNoraml,iconHover,iconPress);
}

void QClassWebDialog::openWebPage(QString strUrl)
{
    if(strUrl.isEmpty())
    {
        return;
    }

	QNetworkRequest webRequest;    
	webRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	webRequest.setUrl(QUrl(strUrl));
	ui.webView->load(webRequest);
}

void QClassWebDialog::addObjectToJs()
{
	ui.webView->page()->mainFrame()->addToJavaScriptWindowObject("CoursePage", this);
}

void QClassWebDialog::enterClass(QString courseID, QString classId)
{
	LobbyDialog * dlgLobby = LobbyDialog::getInstance();
	if(NULL == dlgLobby)
	{
		return;
	}

	dlgLobby->enterClass(courseID,classId);
}

QClassWebView* QClassWebDialog::getWebView()
{
    return ui.webView;
}

//////////////////////////////////////////////////////////////////////////
//
CWebDlgMgr* CWebDlgMgr::m_instance = NULL;

CWebDlgMgr* CWebDlgMgr::getInstance()
{
    if(NULL== m_instance)
    {
        m_instance = new CWebDlgMgr();
    }

    return m_instance;
}

void CWebDlgMgr::freeInstance()
{
    if(NULL != m_instance)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

void CWebDlgMgr::addWebDlg(QClassWebDialog* webDlg)
{
    if(NULL == webDlg)
    {
        return;
    }

    int index = findWebDlg(webDlg);
    if(index >= 0)
    {
        return;
    }
    
    m_vecWebDlg.push_back(webDlg);    
}

void CWebDlgMgr::delWebDlg(QClassWebDialog* webDlg)
{
    if(NULL == webDlg)
    {
        return;
    }

    int index = findWebDlg(webDlg);
    if(index < 0)
    {
        return;
    }

    m_vecWebDlg.removeAt(index);
}

int CWebDlgMgr::findWebDlg(QClassWebDialog* webDlg)
{
    if(m_vecWebDlg.empty() || NULL == webDlg)
    {
        return -1;
    }

    for(int i=0;i<m_vecWebDlg.size();i++)
    {
        QClassWebDialog* dlg=m_vecWebDlg.at(i);
        if(dlg == webDlg)
        {
            return i;
        }
    }

    return -1;
}

void CWebDlgMgr::closeAll()
{
    if(m_vecWebDlg.empty())
    {
        return ;
    }

    for(int i=0;i<m_vecWebDlg.size();i++)
    {
        QClassWebDialog* dlg=m_vecWebDlg.at(i);
        if(NULL == dlg)
        {
            continue;
        }

        dlg->close();
    }

    m_vecWebDlg.clear();
}