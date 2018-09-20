#include "coursewaredownloadlistwindow.h"
#include "control/coursewaredownloadlistitemwidget.h"
#include "Courseware/CourseWareTaskMgr.h"
#include "session/classsession.h"


CoursewareDownloadListWindow::CoursewareDownloadListWindow(QWidget *parent)
    : C8PopupWindow(parent, "CoursewareDownloadListWindow.png")
{
    ui.setupUi(this);

    connect(CoursewareTaskMgr::getInstance(), SIGNAL(add_courseware(QString, long long, int)), this, SLOT(AddCourseware(QString, long long, int)));
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(set_pos(QString, int)), this, SLOT(SetPercent(QString, int)));
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(set_filesize(QString, long long)), this, SLOT(SetFileSize(QString, long long)));
    connect(CoursewareTaskMgr::getInstance(), SIGNAL(del_courseware(QString)), this, SLOT(DelCourseware(QString)));
}

CoursewareDownloadListWindow::~CoursewareDownloadListWindow()
{

}

void CoursewareDownloadListWindow::addItem()
{
    for (int i = 0; i < 7; i++)
    {
        QString fileName = QString("fileName%1").arg(i);
        DWORD fileSize = 100110 + i * 100110;
        QListWidgetItem *item = new QListWidgetItem;
        CoursewareDownloadListItemWidget *itemWidget = new CoursewareDownloadListItemWidget(ui.listWidget_coursewareDownloadList, fileName, i, fileSize, 66);
        ui.listWidget_coursewareDownloadList->addItem(item);
        ui.listWidget_coursewareDownloadList->setItemWidget(item, itemWidget);
    }
    
}

void CoursewareDownloadListWindow::addItem(QString fileName, int fileType, long long fileSize, int downloadPercent)
{
    if (fileName.isEmpty())
    {
        return;
    }
    QListWidgetItem *item = new QListWidgetItem;
    CoursewareDownloadListItemWidget *itemWidget = new CoursewareDownloadListItemWidget(ui.listWidget_coursewareDownloadList, fileName, fileType, fileSize, downloadPercent);
    ui.listWidget_coursewareDownloadList->addItem(item);
    ui.listWidget_coursewareDownloadList->setItemWidget(item, itemWidget);
}

void CoursewareDownloadListWindow::delItem(QString fileName)
{
    if (fileName.isEmpty() || 0 == ui.listWidget_coursewareDownloadList->count())
    {
        return;
    }

    int nsize = ui.listWidget_coursewareDownloadList->count();
    for (int i = 0; i < nsize; i++)
    {
        QListWidgetItem *item = ui.listWidget_coursewareDownloadList->item(i);
        CoursewareDownloadListItemWidget *itemWidget = (CoursewareDownloadListItemWidget*)ui.listWidget_coursewareDownloadList->itemWidget(item);
        
        if (itemWidget)
        {
            QString str = itemWidget->getContent();
            if (fileName == str)
            {
                ui.listWidget_coursewareDownloadList->removeItemWidget(item);
                delete itemWidget;
                delete item;
                break;
            }
        }
        
    }
}

void CoursewareDownloadListWindow::setPercent(QString fileName, int downloadPercent)
{
    if (fileName.isEmpty() || 0 == ui.listWidget_coursewareDownloadList->count())
    {
        return;
    }

    int nsize = ui.listWidget_coursewareDownloadList->count();
    for (int i = 0; i < nsize; i++)
    {
        QListWidgetItem *item = ui.listWidget_coursewareDownloadList->item(i);
        CoursewareDownloadListItemWidget *itemWidget = (CoursewareDownloadListItemWidget*)ui.listWidget_coursewareDownloadList->itemWidget(item);

        if (itemWidget)
        {
            QString str = itemWidget->getContent();
            if (fileName == str)
            {
                if (downloadPercent < 0)
                {
                    //ÏÂÔØÊ§°Ü
                }
                else
                {
                    itemWidget->setPercent(downloadPercent);
                }
                break;
            }
        }
    }
}

void CoursewareDownloadListWindow::setFileSize(QString fileName, long long fileSize)
{
    if (fileName.isEmpty() || 0 == ui.listWidget_coursewareDownloadList->count())
    {
        return;
    }

    int nsize = ui.listWidget_coursewareDownloadList->count();
    for (int i = 0; i < nsize; i++)
    {
        QListWidgetItem *item = ui.listWidget_coursewareDownloadList->item(i);
        CoursewareDownloadListItemWidget *itemWidget = (CoursewareDownloadListItemWidget*)ui.listWidget_coursewareDownloadList->itemWidget(item);

        if (itemWidget)
        {
            QString str = itemWidget->getContent();
            if (fileName == str)
            {
                itemWidget->setfileSize(fileSize);
                break;
            }
        }
    }
}

void CoursewareDownloadListWindow::AddCourseware(QString wszFile, long long nFileSize, int nPos)
{
    if (NULL == wszFile || NULL == wszFile[0])
    {
        return;
    }
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    int nType = GetCoursewareFileType(wstring((wchar_t*)(wszFile).unicode()).data());
    addItem((wszFile), nType, nFileSize, nPos);
}

void CoursewareDownloadListWindow::SetPercent(QString wszFile, int nPos)
{
    if (NULL == wszFile || NULL == wszFile[0])
    {
        return;
    }
    setPercent((wszFile), nPos);
}

void CoursewareDownloadListWindow::SetFileSize(QString wszFile, long long nFileSize)
{
    if (NULL == wszFile || NULL == wszFile[0] || nFileSize <= 0)
    {
        return;
    }
    setFileSize((wszFile), nFileSize);
}

void CoursewareDownloadListWindow::DelCourseware(QString wszFile)
{
    if (NULL == wszFile || NULL == wszFile[0])
    {
        return;
    }
    delItem((wszFile));
}