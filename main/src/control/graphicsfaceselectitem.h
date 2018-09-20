#ifndef GRAPHICSFACESELECTITEM_H
#define GRAPHICSFACESELECTITEM_H

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsPixmapItem>

class FaceSelectWindowScene;
class GraphicsFaceSelectItem : public QGraphicsPixmapItem
{
public:
    GraphicsFaceSelectItem();
    ~GraphicsFaceSelectItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setFacePath(const QString &facePath);
    QString getFacePath() const;

    void setParent(FaceSelectWindowScene *parent);
    FaceSelectWindowScene *getParent();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    
private:
    QString m_facePath;
    FaceSelectWindowScene *m_parent;
};


#endif // GRAPHICSFACESELECTITEM_H
