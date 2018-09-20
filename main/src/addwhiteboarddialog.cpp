#include "addwhiteboarddialog.h"
#include "CL8_EnumDefs.h"
#include "common/Env.h"

addwhiteboarddialog::addwhiteboarddialog(unsigned int nID,QWidget *parent)
     : C8CommonWindow(parent)
{
    ui.setupUi(this);
    setWindowRoundCorner(ui.widget, 3, 3);
    connect(ui.pushButton_Cancel, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));
    connect(ui.pushButton_OK, SIGNAL(clicked()), this, SLOT(okBtnClicked()));
    QString strName;
    if (nID > nWhiteBoardViewWBIdBegin)
    {
        strName = QString("%1%2").arg(ui.lineEdit_whiteboardname->text()).arg(nID-nWhiteBoardViewWBIdBegin);
        m_nWhiteboardID = nID;
    }
    else
    {
        strName = QString("%1%2").arg(ui.lineEdit_whiteboardname->text()).arg(1);
        m_nWhiteboardID = 1;
    }
    ui.lineEdit_whiteboardname->setText(strName);
    m_strName = strName;
    QString iconPath = Env::currentThemeResPath();
}

addwhiteboarddialog::~addwhiteboarddialog()
{

}

void addwhiteboarddialog::setTitleBarRect()
{
    m_titlRect = ui.horizontalLayout_titleBar->contentsRect();
}

void addwhiteboarddialog::closeBtnClicked()
{
    C8CommonWindow::reject();
}

void addwhiteboarddialog::okBtnClicked()
{
    m_strName = ui.lineEdit_whiteboardname->text();    
    C8CommonWindow::accept();
}