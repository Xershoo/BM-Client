#include "c8commonwindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QGraphicsDropShadowEffect>
C8CommonWindow::C8CommonWindow(QWidget *parent /* = 0 */,bool shadow /* = true */)
    : QDialog(parent)
{
    m_moveable = false;
    m_dragPosition.setX(0);
    m_dragPosition.setY(0);

    setWindowFlags(Qt::FramelessWindowHint);
    setShadow(shadow);
}

void C8CommonWindow::setShadow(bool shadow)
{
	if(!shadow)
	{
		return;
	}

	setAttribute(Qt::WA_TranslucentBackground);
	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);
}

C8CommonWindow::~C8CommonWindow()
{

}

void C8CommonWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_titlRect.isEmpty())
    {
        setTitleBarRect();
    }
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        if (m_titlRect.contains(event->pos()))
        {
            m_moveable = true;
        }
        event->accept();
    }
}

void C8CommonWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_moveable)
    {
        move(event->globalPos() - m_dragPosition);
    }
    event->accept();
}

void C8CommonWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveable = false;
    event->accept();
}

void C8CommonWindow::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
    QPainterPath path;
    QRectF rect = widget->rect();
    path.addRoundRect(rect, roundX, roundY);
    QPolygon polygon= path.toFillPolygon().toPolygon();
    QRegion region(polygon);
    widget->setMask(region);
}

/*
void C8CommonWindow::paintEvent(QPaintEvent *event)
{
    // ÒõÓ°ÊµÏÖ
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}
*/