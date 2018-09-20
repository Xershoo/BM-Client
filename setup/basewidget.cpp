#include "basewidget.h"
#include <QMouseEvent>
#include <qdebug.h>
BaseWidget::BaseWidget(QWidget *parent) 
	: QDialog(parent)
{
    m_moveable = false;
    m_dragPosition.setX(0);
    m_dragPosition.setY(0);
	m_moveWidget = this;
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
}

void BaseWidget::setSetUp(SetUp *setup)
{
    this->m_setup = setup;
}

void BaseWidget::mousePressEvent(QMouseEvent *event)
{
    if (m_titlRect.isEmpty())
    {
        setTitleBarRect();
    }
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() -  m_moveWidget->frameGeometry().topLeft();
        if (m_titlRect.contains(event->pos()))
        {
            m_moveable = true;
        }
        event->accept();
    }
}

void BaseWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_moveable)
    {
		qDebug()<<m_moveWidget;
        m_moveWidget->move(event->globalPos() - m_dragPosition);
    }
    event->accept();
}

void BaseWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveable = false;
    event->accept();
}

void BaseWidget::setMoveWidget(QWidget* widget)
{
	m_moveWidget = widget;
}

BaseWidget::~BaseWidget()
{
}

void BaseWidget::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
	QPainterPath path;
	QRectF rect = widget->rect();
	path.addRoundRect(rect, roundX, roundY);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	widget->setMask(region);
}