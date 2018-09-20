#include "qloginbttokendialog.h"
#include "setdebugnew.h"


QLoginBtTokenDialog* QLoginBtTokenDialog::getInstance()
{
    static QLoginBtTokenDialog inst;
    return &inst;
}

QLoginBtTokenDialog::QLoginBtTokenDialog(QWidget *parent)
    : C8CommonWindow(parent)
{
    ui.setupUi(this);
}

QLoginBtTokenDialog::~QLoginBtTokenDialog()
{

}

void QLoginBtTokenDialog::setTitleBarRect()
{
   
}