#ifndef QLOGINBTTOKENDIALOG_H
#define QLOGINBTTOKENDIALOG_H

#include "c8commonwindow.h"
#include "ui_qloginbttokendialog.h"

class QLoginBtTokenDialog : public C8CommonWindow
{
    Q_OBJECT

public:
    static QLoginBtTokenDialog* getInstance();
    static void FreeInstan();
    ~QLoginBtTokenDialog();

private:
    QLoginBtTokenDialog(QWidget *parent = 0);

protected:
    virtual void setTitleBarRect() override;

private:
    Ui::QLoginBtTokenDialog ui;
};

#endif // QLOGINBTTOKENDIALOG_H
