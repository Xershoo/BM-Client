//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardCurveItem.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中曲线item的类定义头文件
//  接口：
//      1.startDraw         曲线开始绘制
//      2.drawing           鼠标移动中的绘制
//      3.endDraw           曲线结束
//      4.setHoverColor     设置鼠标移上去的颜色
//  修改说明：
//**********************************************************************

#ifndef WHITEBOARD_CURVE_ITEM
#define WHITEBOARD_CURVE_ITEM

#include <QtWidgets/QGraphicsPathItem>
#include <QtGui/QPen>
#include "WhiteBoardData.h"

class QWhiteBoardCurveItem : public QObject,
    public QGraphicsPathItem,
    public WhiteBoardItem
{
    Q_OBJECT
public:
    enum { Type = UserType + WB_MODE_CURVE};
    int type() const Q_DECL_OVERRIDE 
    { 
        return Type; 
    };

    QWhiteBoardCurveItem(QGraphicsItem* parent = NULL);

    virtual void startDraw(QGraphicsSceneMouseEvent * event); 
    virtual void drawing(QGraphicsSceneMouseEvent * event);
    virtual void endDraw(QGraphicsSceneMouseEvent * event);

    virtual void setHoverColor(QColor& clr);
    virtual void setPenColor(QColor& clr);
    virtual void getItemString(std::string& strItem);

    virtual void setPaintElement(WBFreeCurve& wbCurve);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);

protected:
    QPainterPath m_lastPath;
    QPen         m_penHover;
    QPen         m_penNormal;
    QColor       m_clrHover;
};

#endif