#ifndef QCOURSEWAREPREVIEW_H
#define QCOURSEWAREPREVIEW_H

#include "control/c8popupwindow.h"
#include "ui_qcoursewarepreview.h"

class QCoursewarePreview : public C8PopupWindow
{
    Q_OBJECT

public:
    QCoursewarePreview(QWidget *parent = 0);
    ~QCoursewarePreview();

    QListWidget *GetListWidget();

    void ShowPreview(QString filePath, int npage);
    void RemovePreview(QString filePath);

private:
    Ui::QCoursewarePreview ui;
};

#endif // QCOURSEWAREPREVIEW_H
