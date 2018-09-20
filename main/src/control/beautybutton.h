#ifndef BEAUTYBUTTON_H
#define BEAUTYBUTTON_H

#include <QPushButton>

class BeautyButton : public QPushButton
{
    Q_OBJECT

signals:
    void levelChanged(int);
public:
    BeautyButton(QWidget *parent);
    ~BeautyButton();

    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);
private:
    int m_level;
    int m_count;
};

#endif // BEAUTYBUTTON_H
