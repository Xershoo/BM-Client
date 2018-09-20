#ifndef QSYSNOTIFYITEM_H
#define QSYSNOTIFYITEM_H

#include <QWidget>
#include "ui_qsysnotifyitem.h"

class QSysNotifyItem : public QWidget
{
    Q_OBJECT

public:
    QSysNotifyItem(QWidget *parent, QString strTitle, QString strContent, QString strUrl, int nMsgType, bool bISChecked = false);
    ~QSysNotifyItem();

protected slots:
    void doSeeNotify(QString);

private:
    void setState();

private:
    Ui::QSysNotifyItem ui;

protected:
    QString m_strTitle;
    QString m_strContent;
    QString m_strUrl;
    int     m_nMsgType;
    bool    m_bISChecked;
};

#endif // QSYSNOTIFYITEM_H
