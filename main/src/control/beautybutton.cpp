#include "beautybutton.h"
#include <QMouseEvent>
#include <QPainter>

BeautyButton::BeautyButton(QWidget *parent)
    : m_level(3), m_count(5), QPushButton(parent)
{

}

BeautyButton::~BeautyButton()
{

}

void BeautyButton::mousePressEvent(QMouseEvent *e)
{
    QSize btnSize = size();
    m_level = e->pos().x() / ((btnSize.width() * 1.0) / (m_count)) + 1;
    emit levelChanged(m_level);
}

void BeautyButton::paintEvent(QPaintEvent *e)
{

    int w = (size().width() * 1.0) / m_count;
    int h = 10;
    QPainter p(this);
    
    p.fillRect(rect(), QBrush(QColor(255, 255, 255)));

    QRect rcPart = rect();
    rcPart.setTop(5);
    rcPart.setHeight(10);
    for (int i = 0; i < m_count; i++)
    {
        rcPart.setLeft(i * w);
        rcPart.setWidth(w - 1);
        if (i < m_level)
        {
            if (i == 0)
            {
                QPainterPath path;
                path.arcMoveTo(rcPart.x(), rcPart.y(), 5, 5, 180);
                path.arcTo(rcPart.x() + 1, rcPart.y(), 5, 5, 180, -90);
                path.arcTo(rcPart.x()+rcPart.width() - 1, rcPart.y(), 1, 1, 90, 0);
                path.arcTo(rcPart.x()+rcPart.width() - 1, rcPart.y()+h - 1 , 1, 1, 0, 0);
                path.arcTo(rcPart.x() + 1, rcPart.y()+h-5, 5, 5, 270, -90);
                path.closeSubpath();
                
                p.setPen(QPen(QColor(83, 199, 147)));
                p.setBrush(QColor(83, 199, 147));
                p.fillPath(path, QBrush(QColor(83, 199, 147)));
                p.drawPath(path);
            }
            else
            {
                p.fillRect(rcPart, QBrush(QColor(83, 199, 147)));
            }            
        }
        else
        {
            if (i == (m_count - 1))
            {
                QPainterPath path;
                path.arcMoveTo(rcPart.x(), rcPart.y(), 5, 5, 180);
                path.arcTo(rcPart.x(), rcPart.y(), 1, 1, 180, 0);
                path.arcTo(rcPart.x()+rcPart.width() - 5, rcPart.y(), 5, 5, 90, -90);
                path.arcTo(rcPart.x()+rcPart.width() - 5, rcPart.y()+h - 6 , 5, 5, 0, -90);
                path.arcTo(rcPart.x(), rcPart.y()+h - 1, 1, 1, 270, 0);
                path.closeSubpath();

                p.setPen(QPen(QColor(237, 238, 242)));
                p.setBrush(QColor(237, 238, 242));
                p.fillPath(path, QBrush(QColor(237, 238, 242)));
                p.drawPath(path);
            }
            else
            {
                p.fillRect(rcPart, QBrush(QColor(237, 238, 242)));
            }            
        }
        
    }
    rcPart = rect();
    rcPart.setLeft(m_level * w - 6);
    rcPart.setTop(3);
    rcPart.setHeight(14);
    rcPart.setWidth(5);

    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundRect(rcPart, 60);
    p.setPen(QPen(QColor(55, 175, 121)));
    p.setBrush(QColor(55, 175, 121));
    p.fillPath(path, QBrush(QColor(55, 175, 121)));
    p.drawPath(path);
}
