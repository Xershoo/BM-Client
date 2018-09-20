#ifndef ADDWHITEBOARDDIALOG_H
#define ADDWHITEBOARDDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_addwhiteboarddialog.h"
#include "../varname.h"


class addwhiteboarddialog : public C8CommonWindow
{
    Q_OBJECT

public:
    addwhiteboarddialog(unsigned int nID, QWidget *parent = 0);
    ~addwhiteboarddialog();

    inline QString getName()
    {
        return m_strName;
    };

protected:
    void setTitleBarRect() override;

protected slots:
    void closeBtnClicked();
    void okBtnClicked();


private:
    Ui::addwhiteboarddialog ui;

    QString     m_strName;
    int         m_nWhiteboardID;
};


#endif // ADDWHITEBOARDDIALOG_H
