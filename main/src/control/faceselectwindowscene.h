#ifndef FACESELECTWINDOWSCENE_H
#define FACESELECTWINDOWSCENE_H
#include <QGraphicsScene>

class FaceSelectWindowScene : public QGraphicsScene
{
    Q_OBJECT

public:
    void hoverEnterFaceItem(QPointF pos, QSizeF size, QString facePath);
    void HoverLeaveFaceItem();

signals:
    void sg_hoverEnterFace(QPointF pos, QSizeF size, QString facePath);
    void sg_leaveFace();
    void sg_selectFace(QString facePath);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};


#endif // FACESELECTWINDOWSCENE_H
