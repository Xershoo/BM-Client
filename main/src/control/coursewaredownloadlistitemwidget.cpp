#include "coursewaredownloadlistitemwidget.h"

CoursewareDownloadListItemWidget::CoursewareDownloadListItemWidget(QWidget *parent, QString fileName, int fileType, DWORD fileSize, int downloadPercent)
    : QWidget(parent)
{
    ui.setupUi(this);
    
    ui.label_icon->setObjectName(QString("coursewareDownloadListIcon_%1").arg(fileType));
    ui.label_fileName->setText(fileName);
    float size = (fileSize / 1024.0) / 1024.0;
    
    ui.label_fileSize->setText(QString::number(size, 'f', 1) + "M");

    ui.label_finished->setText("downloadFinished");
    ui.label_finished->hide();
    
    ui.progressBar->setValue(downloadPercent);
}

CoursewareDownloadListItemWidget::~CoursewareDownloadListItemWidget()
{

}

QString CoursewareDownloadListItemWidget::getContent()
{
    QString strContent = ui.label_fileName->text();
    return strContent;
}

void CoursewareDownloadListItemWidget::setPercent(int downloadPercent)
{
    if (downloadPercent < 0)
    {
        ui.label_finished->setText("downloadFailed");
        return;
    }
    ui.progressBar->setValue(downloadPercent);
}

void CoursewareDownloadListItemWidget::setfileSize(DWORD fileSize)
{
    float size = (fileSize / 1024.0) / 1024.0;

    ui.label_fileSize->setText(QString::number(size, 'f', 1) + "M");
}