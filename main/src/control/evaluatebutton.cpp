#include "evaluatebutton.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

EvaluateButton::EvaluateButton(QWidget *parent)
    : m_star(5), m_hoverStar(5), m_starCount(5), QPushButton(parent)
{
    setMouseTracking(true);
    m_star = m_hoverStar;
    m_starPixmapSize = QSize(15, 15);
    m_starSize = QSize(20, 20);
}

EvaluateButton::~EvaluateButton()
{

}

void EvaluateButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QPoint pt;
    pt.setY(0);
    for (int i = 0; i != m_starCount; ++i)
    {        
        pt.setX(i * m_starSize.width());
        p.drawPixmap(pt, QPixmap(":/res/res/image/default/star_bk.png"), QRect(QPoint(0, 0), m_starPixmapSize));
    }

    QString starPixmapPath = QString(":/res/res/image/default/star_%1.png").arg(m_hoverStar);
    for (int i = 0; i < m_hoverStar; ++i)
    {
        pt.setX(i * m_starSize.width());
        p.drawPixmap(pt, QPixmap(starPixmapPath), QRect(QPoint(0, 0), m_starPixmapSize));
    }     
}

void EvaluateButton::mouseMoveEvent(QMouseEvent *e)
{
    int star = e->pos().x() / m_starSize.width() + 1;
    if (m_hoverStar != star)
    {
        m_hoverStar = star;
        emit evaluateHover(m_hoverStar);
        update();
    }    
}

void EvaluateButton::mouseReleaseEvent(QMouseEvent *e)
{
    m_star = e->pos().x() / m_starSize.width() + 1;
    m_hoverStar = m_star;
    emit evaluateSelected(m_star);
}

void EvaluateButton::leaveEvent(QEvent *e)
{
    m_hoverStar = m_star;
}