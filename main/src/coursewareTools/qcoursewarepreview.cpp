#include "qcoursewarepreview.h"

QCoursewarePreview::QCoursewarePreview(QWidget *parent)
    : C8PopupWindow(parent, "")
{
    ui.setupUi(this);
}

QCoursewarePreview::~QCoursewarePreview()
{

}

void QCoursewarePreview::ShowPreview(QString filePath, int npage)
{
    ui.widget_preview->selPDFPage(filePath, npage);
}

void QCoursewarePreview::RemovePreview(QString filePath)
{
    ui.widget_preview->delPDFPreview(filePath);
}

QListWidget* QCoursewarePreview::GetListWidget()
{
    return ui.widget_preview;
}