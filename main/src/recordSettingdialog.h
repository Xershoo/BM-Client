#ifndef ADDWHITEBOARDDIALOG_H
#define ADDWHITEBOARDDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_recordSettingdialog.h"
#include "../varname.h"


class recordSettingdialog : public C8CommonWindow
{
    Q_OBJECT

public:
    recordSettingdialog(QString strPath,QWidget *parent = 0);
    ~recordSettingdialog();

	static QString getRecordPath();
    virtual int exec();
protected:
    void setTitleBarRect() override;

	QString getDefaultPath();
protected slots:
    void closeBtnClicked();
    void okBtnClicked();
    void changeClicked();

private:
    Ui::recordSettingdialog ui;
	static QString m_strPath;
};


#endif // ADDWHITEBOARDDIALOG_H
