
#include "PlayerSlider.h"
#include <QMouseEvent>

PlayerSlider::PlayerSlider(QWidget *parent /* = 0 */)
    :QSlider(Qt::Horizontal, parent)
{
    
}

void PlayerSlider::mousePressEvent(QMouseEvent *ev)
{
    if (NULL == ev)
    {
        return;
    }
    

    if (ev->button() & Qt::LeftButton)
    {
        QSlider::mousePressEvent(ev);
        double pos = ev->pos().x() / (double)width();
        int nValue = pos * (maximum() - minimum()) + minimum();
    }
}

void PlayerSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    if (NULL == ev)
    {
        return;
    }

    if(ev->button()==Qt::LeftButton)
    {
        QSlider::mousePressEvent(ev);
        double pos = ev->pos().x() / (double)width();
        int nValue = pos * (maximum() - minimum()) + minimum();
        emit drag_now_value(nValue);
    }
}
