//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�QWhiteBoardCurveItem.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.9
//	˵������װ�˰װ�������item���ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.startDraw         ���߿�ʼ����
//      2.drawing           ����ƶ��еĻ���
//      3.endDraw           ���߽���
//      4.setHoverColor     �����������ȥ����ɫ
//  �޸�˵����
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