#ifndef ADDCOURSEWAREWINDOW_H
#define ADDCOURSEWAREWINDOW_H

#include "control/c8popupwindow.h"
#include "ui_addcoursewarewindow.h"
#include <QtWidgets/QDialog>

class AddCoursewareTipDlg : public QDialog
{
    Q_OBJECT

public:
    AddCoursewareTipDlg(QWidget* parent);
    virtual ~AddCoursewareTipDlg();

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    bool eventFilter(QObject * obj, QEvent * event);
};

class AddCoursewareWindow : public C8PopupWindow
{
    Q_OBJECT

public:
    AddCoursewareWindow(QWidget *parent = 0);
    ~AddCoursewareWindow();

protected slots:
    void addSkyFileBtnClicked();
    void addLoacalFileBtnClicked();
    void addWhiteBoardBtnClicked();
private:
    Ui::AddCoursewareWindow ui;
    static bool m_showAddTipDlg;
};

#endif // ADDCOURSEWAREWINDOW_H
