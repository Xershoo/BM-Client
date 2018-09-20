#ifndef COURSEWAREDOWNLOADLISTWINDOW_H
#define COURSEWAREDOWNLOADLISTWINDOW_H

#include "control/c8popupwindow.h"
#include "ui_coursewaredownloadlistwindow.h"
#include "varname.h"

class CoursewareDownloadListWindow : public C8PopupWindow
{
    Q_OBJECT

public:
    CoursewareDownloadListWindow(QWidget *parent = 0);
    ~CoursewareDownloadListWindow();
    
    void addItem();
    void addItem(QString fileName, int fileType, long long fileSize, int downloadPercent);
    void delItem(QString fileName);
    void setFileSize(QString fileName, long long fileSize);
    void setPercent(QString fileName, int downloadPercent);

public slots:
    void AddCourseware(QString, long long, int);
    void SetFileSize(QString, long long);
    void SetPercent(QString, int);
    void DelCourseware(QString);

private:
    Ui::CoursewareDownloadListWindow ui;
};

#endif // COURSEWAREDOWNLOADLISTWINDOW_H
