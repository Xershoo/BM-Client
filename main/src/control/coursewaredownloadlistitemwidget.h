#ifndef COURSEWAREDOWNLOADLISTITEMWIDGET_H
#define COURSEWAREDOWNLOADLISTITEMWIDGET_H

#include <QWidget>
#include "ui_coursewaredownloadlistitemwidget.h"
#include "varname.h"

class CoursewareDownloadListItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CoursewareDownloadListItemWidget(QWidget *parent, QString fileName, int fileType, DWORD fileSize, int downloadPercent);
    ~CoursewareDownloadListItemWidget();

    QString getContent();
    void setPercent(int downloadPercent);
    void setfileSize(DWORD fileSize);

private:
    Ui::CoursewareDownloadListItemWidget ui;

};

#endif // COURSEWAREDOWNLOADLISTITEMWIDGET_H
