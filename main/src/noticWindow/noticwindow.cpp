#include "noticwindow.h"
#include <qDebug>
#include <QWebFrame>
#include <QDesktopServices>
#include <QWebSettings>
#include "common/Config.h"
#include "./session/classsession.h"
#include "./token/LoginTokenMgr.h"
#include "setdebugnew.h"

NoticWindow* NoticWindow::getInstance()
{
    static NoticWindow noticWindow;
    return &noticWindow;
}

NoticWindow::NoticWindow(QWidget *parent)
    : C8PopupWindow(parent, "")
{
    ui.setupUi(this);

    QString strSeeAll = ui.label_see_all->text();
    QString strContent = "<a style='color: rgb(23, 116, 200);' href = " 
        + QString(Config::getConfig()->m_urlWebSite.c_str()) 
        + "> " + strSeeAll + "</a>";

    ui.label_see_all->setText(strContent);
    m_bISClickedAllMsg = false;

    connect(ui.label_see_all,SIGNAL(linkActivated(QString)),this,SLOT(doLkSeeAll(QString)));  
    connect(CLoginTokenMgr::GetInstance(), SIGNAL(recvLoginToken(LPCWSTR)), this, SLOT(onRecvLoginToken(LPCWSTR)));
    hide();
}

NoticWindow::~NoticWindow()
{
    releaseWidget();
}

void NoticWindow::releaseWidget()
{
    int count = ui.listWidget_sysnotify->count();
    for (int i = 0; i < count; i++)
    {
        QListWidgetItem *item = ui.listWidget_sysnotify->item(i);
        delete ui.listWidget_sysnotify->itemWidget(item);
        ui.listWidget_sysnotify->removeItemWidget(item);
    }
}

void NoticWindow::closeBtnClicked()
{
    NoticWindow::close();
}

void NoticWindow::doLkSeeAll(QString strurl)
{
    m_bISClickedAllMsg = true;
    WCHAR szToken[MAX_PATH] = {0};
    if (CLoginTokenMgr::GetInstance()->GetLoginToken(ClassSeeion::GetInst()->_nUserId, szToken))
    {
        ShowSelfHome(szToken);
    }
    this->hide();
}

void NoticWindow::onRecvLoginToken(LPCWSTR pwszToken)
{
    ShowSelfHome(pwszToken);
}

void NoticWindow::ShowSelfHome(LPCWSTR wszToken)
{
    if (!m_bISClickedAllMsg)
    {
        return;
    }
    m_bISClickedAllMsg = false;
    if (NULL == wszToken || NULL == wszToken[0])
    {
        return;
    }
    QString url;
    QString strToken(QString::fromWCharArray(wszToken));
    url = QString(Config::getConfig()->m_urlNotifyMsg.c_str()).arg(ClassSeeion::GetInst()->_nUserId).arg(strToken);
    QDesktopServices::openUrl(QUrl(url));
    this->hide();
}

void NoticWindow::updateMsg(MessageListInfo infolist)
{
    if (ui.listWidget_sysnotify->count() > 0)
    {
        releaseWidget();
        ui.listWidget_sysnotify->clear();
    }
    if (infolist._msgList.size() > 0)
    {
        for (size_t i = 0; i < infolist._msgList.size(); i++)
        {
            QString strTitle = QString::fromWCharArray(infolist._msgList[i]._wszTitle);
            QString strUrl = QString::fromWCharArray(infolist._msgList[i]._wszUrl);
            QString strContent = QString::fromWCharArray(infolist._msgList[i]._wszContent);
            QListWidgetItem *listWidgetItem = new QListWidgetItem(ui.listWidget_sysnotify);
            QSysNotifyItem *item = new QSysNotifyItem(ui.listWidget_sysnotify, strTitle, strContent, strUrl, infolist._nMsgType);
            int nw = item->width();
            int nh = item->height();
            listWidgetItem->setSizeHint(QSize(item->width(), item->height()));
            ui.listWidget_sysnotify->addItem(listWidgetItem);
            ui.listWidget_sysnotify->setItemWidget(listWidgetItem, item);
        }
    }
}

int NoticWindow::GetCount()
{
    return ui.listWidget_sysnotify->count();
}