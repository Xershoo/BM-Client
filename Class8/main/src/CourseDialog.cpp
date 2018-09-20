#include "CourseDialog.h"
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
#include "lang.h"
#include "util.h"

CourseDialog::CourseDialog(QWidget *parent /* = 0 */)
	 : C8CommonWindow(parent)
{
	ui.setupUi(this);
	setWindowRoundCorner(ui.courseDlg_mainWidget,1,1);

	setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);    

	connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(onClose()));

	ui.CourseView->setContextMenuPolicy(Qt::NoContextMenu);

	connect(ui.CourseView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addObjectToJs()));

	QNetworkDiskCache* netDiskCache = new QNetworkDiskCache(this);    
	QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);     
	netDiskCache->setCacheDirectory(location);    
	ui.CourseView->page()->networkAccessManager()->setCache(netDiskCache);
}

void CourseDialog::setTitleBarRect()
{
	QPoint pt = ui.label_title->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.courseDlg_titleBar->size());
}

void CourseDialog::onClose()
{
	QDialog::close();
}

void CourseDialog::openCoursePage()
{
    QString regKey(QObject::tr("product"));
    char    szKey[MAX_PATH] = {0};
    QString strLang;
    Util::QStringToChar(regKey,szKey,MAX_PATH);
    int langId = LangSet::loadLangSet(szKey);
    if(langId == LangSet::LANG_ENG)
    {
        strLang=QString("eng");
    }
    else
    {
        strLang=QString("chn");
    }

    QString strUrl = QString(Config::getConfig()->m_urlCourseSelf.c_str()).arg(ClassSeeion::GetInst()->_nUserId).arg(strLang);
	
	QNetworkRequest webRequest;    
	webRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	webRequest.setUrl(QUrl(strUrl));
	ui.CourseView->load(webRequest);
}

void CourseDialog::addObjectToJs()
{
	ui.CourseView->page()->mainFrame()->addToJavaScriptWindowObject("CoursePage", this);
}

void CourseDialog::enterClass(QString courseID, QString classId)
{
	LobbyDialog * dlgLobby = LobbyDialog::getInstance();
	if(NULL == dlgLobby)
	{
		return;
	}

	dlgLobby->enterClass(courseID,classId);
}

void CourseDialog::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
	QPainterPath path;
	QRectF rect = widget->rect();
	path.addRoundRect(rect, roundX, roundY);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	widget->setMask(region);
}
