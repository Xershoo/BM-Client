#ifndef QSYSNOTIFYLIST_H
#define QSYSNOTIFYLIST_H

#include <QListWidget>
#include <QVector>
#include "qsysnotifyitem.h"

class SysNotifyList : public QListWidget
{
    Q_OBJECT

public:
    SysNotifyList(QWidget *parent);
    ~SysNotifyList();

    const QVector<QSysNotifyItem *>& getDownloadItems() {return m_selectedFiles;}
protected slots:
        void itemSelected(QListWidgetItem *);
private:
    QVector<QSysNotifyItem *> m_selectedFiles;

};


#endif
