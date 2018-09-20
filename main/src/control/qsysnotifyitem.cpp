#include "qsysnotifyitem.h"
#include "common/Env.h"
#include "common/Config.h"

QSysNotifyItem::QSysNotifyItem(QWidget *parent, QString strTitle, QString strContent, QString strUrl, int nMsgType, bool bISChecked)
    : QWidget(parent), 
    m_strTitle(strTitle),
    m_strContent(strContent),
    m_strUrl(strUrl),
    m_nMsgType(nMsgType),
    m_bISChecked(bISChecked)
{
    ui.setupUi(this);
    connect(ui.label_info,SIGNAL(linkActivated(QString)),this,SLOT(doSeeNotify(QString)));  
    setState();
}

QSysNotifyItem::~QSysNotifyItem()
{

}

void QSysNotifyItem::doSeeNotify(QString strurl)
{

    m_bISChecked = true;
    setState();
    this->update();
}

void QSysNotifyItem::setState()
{
    QString strContent, iconPath, strType;
    switch (m_nMsgType)
    {
    case 1:
        strType = QString(tr("SystemNotify")) + QString(" ");
        break;

    default:
        strType = "";
        break;
    }
    if (m_bISChecked)
    {
        strContent = "<a style='color: rgb(60, 60, 60); text-decoration:none;' href = " 
            + QString(Config::getConfig()->m_urlForgetPage.c_str()) 
            + "> " +strType + m_strTitle + "</a>";

        iconPath = QString("%1%2").arg(Env::currentThemeResPath()).arg("sys_notify_open.png");
      
    }
    else
    {
        strContent = "<a style='color: rgb(25, 118, 210); text-decoration:none;' href = " 
            + QString(Config::getConfig()->m_urlForgetPage.c_str()) 
            + "> " + strType + m_strTitle + "</a>";

        iconPath = QString("%1%2").arg(Env::currentThemeResPath()).arg("sys_notify_notopen.png");
    }
    ui.label_info->setText(strContent);
    QIcon iconPic;
    iconPic.addFile(iconPath, QSize(), QIcon::Normal, QIcon::On);
    iconPic.addFile(iconPath, QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_pic->setIcon(iconPic);
}