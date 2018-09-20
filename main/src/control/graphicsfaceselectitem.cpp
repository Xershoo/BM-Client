#include "graphicsfaceselectitem.h"
#include "faceselectwindowscene.h"
#include <QtDebug>
GraphicsFaceSelectItem::GraphicsFaceSelectItem()
{
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
}

GraphicsFaceSelectItem::~GraphicsFaceSelectItem()
{

}

QRectF GraphicsFaceSelectItem::boundingRect() const
{
    return QRectF(0, 0, 30, 30);   
}

void GraphicsFaceSelectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);
}

void GraphicsFaceSelectItem::setFacePath(const QString &facePath)
{
    setPixmap(QPixmap(facePath));
    m_facePath = facePath;
}

QString GraphicsFaceSelectItem::getFacePath() const
{
    return m_facePath;
}

void GraphicsFaceSelectItem::setParent(FaceSelectWindowScene *parent)
{
    m_parent = parent;
}

FaceSelectWindowScene *GraphicsFaceSelectItem::getParent()
{
    return m_parent;
}

void GraphicsFaceSelectItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_parent->hoverEnterFaceItem(scenePos(), boundingRect().size(), m_facePath);
}

void GraphicsFaceSelectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QPointF offsct = this->scenePos();
    QRectF rcBoundingRect = this->boundingRect();
    QPointF mousePosition = this->mapToScene(event->pos());
    QRectF rc(offsct, rcBoundingRect.size());
    if (!rc.contains(mousePosition))
    {
        m_parent->HoverLeaveFaceItem();
    }
}