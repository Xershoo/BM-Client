//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�QWhiteBoardTextItem.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.9
//	˵������װ�˰װ�������Item�ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.startDraw         ���߿�ʼ����
//      2.drawing           ����ƶ��еĻ���
//      3.endDraw           ���߽���
//      4.setHoverColor     �����������ȥ����ɫ
//      5.setFontSize       ������������Ĵ�С
//  �޸�˵����
//**********************************************************************

#ifndef WHITEBOAD_TEXT_ITEM
#define WHITEBOAD_TEXT_ITEM

#include <QtWidgets/QGraphicsTextItem>
#include "WhiteBoardData.h"

class QWhiteBoardTextItem : public QGraphicsTextItem,
    public WhiteBoardItem
{
    Q_OBJECT

public:
    enum { Type = UserType + WB_MODE_TEXT };
    int type() const Q_DECL_OVERRIDE 
    { 
        return Type; 
    };

    QWhiteBoardTextItem(QString& text,QGraphicsItem* parent = NULL);
    
    virtual void startDraw(QGraphicsSceneMouseEvent * event); 
    virtual void drawing(QGraphicsSceneMouseEvent * event);
    virtual void endDraw(QGraphicsSceneMouseEvent * event);

    virtual void getItemString(std::string& strItem);
    virtual void setHoverColor(QColor& clr);
    virtual void setFontSize(WBSize sizeFont);

    virtual void setItemElement(WBText wbText);

    QRectF boundingRect();
signals:
    void editComplete(QWhiteBoardTextItem *item);
    void selectedChange(QWhiteBoardTextItem *item);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    void focusOutEvent(QFocusEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	void inputMethodEvent(QInputMethodEvent *event);
	void keyPressEvent(QKeyEvent* event);
	
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */);

	QVariant inputMethodQuery(Qt::InputMethodQuery query) const Q_DECL_OVERRIDE;
public:
    QSize calcInitSize();

protected:
    QColor m_clrHover;
    QColor m_clrNormal;

    bool   m_endEdit;
    bool   m_initRect;
    WBSize m_sizeFont;
};

#endif