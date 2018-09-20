//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardLarserItem.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中激光笔的类实现源文件
//
//  修改说明：
//**********************************************************************

#include "QWhiteBoardLarserItem.h"
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGUI/QPen>
#include "WhiteBoardData.h"


QWhiteBoardLarserItem::QWhiteBoardLarserItem(QGraphicsItem* parent /* = NULL */) : 
    QGraphicsEllipseItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable,false);
    setFlag(QGraphicsItem::ItemIsMovable);

    this->setRect(0,0,10,10);
    this->hide();
}

void QWhiteBoardLarserItem::startDraw(QGraphicsSceneMouseEvent * event)
{
    if(NULL != event)
    {
        QPointF fPos = event->scenePos();
        this->setPos(fPos);
    }

    this->show();
}

void QWhiteBoardLarserItem::drawing(QGraphicsSceneMouseEvent * event)
{
    if(NULL == event)
    {
        return;
    }
    QPointF fPos = event->scenePos();
    this->setPos(fPos);
}

void QWhiteBoardLarserItem::endDraw(QGraphicsSceneMouseEvent * event)
{
    if(NULL != event)
    {
        QPointF fPos = event->scenePos();
        this->setPos(fPos);
    }

    this->hide();
}

void QWhiteBoardLarserItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QStyleOptionGraphicsItem styleItem;
    styleItem.initFrom(widget);
    styleItem.state = QStyle::State_None;

    return QGraphicsEllipseItem::paint(painter, &styleItem, widget);
}

void QWhiteBoardLarserItem::setColor(QColor& clr)
{
    setPen(QPen(clr,2));
}

void QWhiteBoardLarserItem::setWidth(qreal w)
{
    QRectF r = rect();
    r.setWidth(w);
    r.setHeight(w);

    setRect(r.x(),r.y(),r.width(),r.height());
}

void QWhiteBoardLarserItem::getItemString(std::string& strItem)
{
	QRectF rectScene = this->scene()->sceneRect();
    QRectF rectItem = this->rect();
    WBLarser wbLarser;
   
    wbLarser.rcLarser.X = (float)(rectItem.x() - rectScene.left()) / rectScene.width();
    wbLarser.rcLarser.Y = (float)(rectItem.y() - rectScene.top()) / rectScene.width();
    wbLarser.rcLarser.Width = rectItem.width();
    wbLarser.rcLarser.Height = rectItem.height();
    wbLarser.bShow = this->isVisible();

    wbLarser.ToString(strItem);
}

void QWhiteBoardLarserItem::setItemElement(WBLarser& wbLarser)
{
	QRectF rectScene = this->scene()->sceneRect();
    QRectF rectItem;

	qreal xPos = (qreal)wbLarser.rcLarser.X * rectScene.width() + rectScene.left();
	qreal yPos = (qreal)wbLarser.rcLarser.Y * rectScene.width() + rectScene.top();

    rectItem.setX((qreal)xPos);
    rectItem.setY((qreal)yPos);
    rectItem.setWidth((qreal)wbLarser.rcLarser.Width);
    rectItem.setHeight((qreal)wbLarser.rcLarser.Height);

    this->setRect(rectItem);

    if(wbLarser.bShow)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}
