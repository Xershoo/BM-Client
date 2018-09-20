//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardLarserItem.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中激光笔的类定义头文件
//  接口：
//      1.startDraw         曲线开始绘制
//      2.drawing           鼠标移动中的绘制
//      3.endDraw           曲线结束
//      4.setHoverColor     设置鼠标移上去的颜色
//  修改说明：
//**********************************************************************

#ifndef WHITEBOARD_LARSER_ITEM
#define WHITEBOARD_LARSER_ITEM

#include <QtWidgets/QGraphicsEllipseItem>
#include "WhiteBoardData.h"

class QWhiteBoardLarserItem : public QObject,
    public QGraphicsEllipseItem,
    public WhiteBoardItem
{
    Q_OBJECT
public:
    enum { Type = UserType + WB_MODE_LARSER };
    int type() const Q_DECL_OVERRIDE 
    { 
        return Type; 
    };

    QWhiteBoardLarserItem(QGraphicsItem* parent = NULL);

    virtual void startDraw(QGraphicsSceneMouseEvent * event); 
    virtual void drawing(QGraphicsSceneMouseEvent * event);
    virtual void endDraw(QGraphicsSceneMouseEvent * event);

    virtual void setHoverColor(QColor& clr){ };
    virtual void getItemString(std::string& strItem);

    virtual void setItemElement(WBLarser& wbLarser);

public:
    void setColor(QColor& clr);
    void setWidth(qreal w);

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);

};

#endif