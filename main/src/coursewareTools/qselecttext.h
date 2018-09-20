#ifndef QSELECTTEXT_H
#define QSELECTTEXT_H

#include "control/c8popupwindow.h"
#include "ui_qselecttext.h"

class QSelectText : public C8PopupWindow
{
    Q_OBJECT

public:
    QSelectText(QWidget *parent = 0);
    ~QSelectText();
    void SetFontSize(int nSize);

protected slots:
    void bigClicked();
    void middleClicked();
    void smallClicked();

private:
    Ui::QSelectText ui;

protected:
    int     m_nSize;
};

#endif // QSELECTTEXT_H
