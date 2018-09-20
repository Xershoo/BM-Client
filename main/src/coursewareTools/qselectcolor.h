#ifndef QSELECTCOLOR_H
#define QSELECTCOLOR_H

#include "control/c8popupwindow.h"
#include "ui_qselectcolor.h"

class QSelectColor : public C8PopupWindow
{
    Q_OBJECT

public:
    QSelectColor(QWidget *parent = 0);
    ~QSelectColor();
    void SetColor(int nColor);

protected slots:
    void redClicked();
    void yellowClicked();
    void blueClicked();
    void greenClicked();
    void blackClicked();

    void doRedToggled(bool);
    void doYellowToggled(bool);
    void doBlueToggled(bool);
    void doGreenToggled(bool);
    void doWhiteToggled(bool);

signals:
    void set_now_color(int);
private:
    Ui::QSelectColor ui;
protected:
    int     m_nColor;
};

#endif // QSELECTCOLOR_H
