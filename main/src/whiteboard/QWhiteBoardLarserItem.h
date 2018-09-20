//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�QWhiteBoardLarserItem.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.9
//	˵������װ�˰װ��м���ʵ��ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.startDraw         ���߿�ʼ����
//      2.drawing           ����ƶ��еĻ���
//      3.endDraw           ���߽���
//      4.setHoverColor     �����������ȥ����ɫ
//  �޸�˵����
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