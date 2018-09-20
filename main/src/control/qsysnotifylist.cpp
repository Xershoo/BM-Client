#include "qsysnotifylist.h"

SysNotifyList::SysNotifyList(QWidget *parent) 
      : QListWidget(parent)
{
    connect(this, SIGNAL(itemPressed(QListWidgetItem *)), this, SLOT(itemSelected(QListWidgetItem *)));
}

SysNotifyList::~SysNotifyList()
{

}

void SysNotifyList::itemSelected(QListWidgetItem *item)
{
    QSysNotifyItem *listItemWidget = qobject_cast<QSysNotifyItem*>(itemWidget(item));
    if (listItemWidget)
    {
        bool find = false;
        for (auto it = m_selectedFiles.begin(); it != m_selectedFiles.end(); it++)
        {
            
        }
    }
}