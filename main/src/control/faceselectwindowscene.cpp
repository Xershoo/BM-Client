#include "faceselectwindowscene.h"
#include "graphicsfaceselectitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QtDebug>

void FaceSelectWindowScene::hoverEnterFaceItem(QPointF pos, QSizeF size, QString facePath)
{
    emit sg_hoverEnterFace(pos, size, facePath);
}

void FaceSelectWindowScene::HoverLeaveFaceItem()
{
    emit sg_leaveFace();
}
void FaceSelectWindowScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->scenePos();
    QList<QGraphicsItem *> subItems = items(pos);
    for (auto it = subItems.begin(); it != subItems.end(); it++)
    {
        GraphicsFaceSelectItem* item = dynamic_cast<GraphicsFaceSelectItem*>(*it);
        if (item)
        {
            QString path = item->getFacePath();
            emit sg_selectFace(path);
        }
    }
}