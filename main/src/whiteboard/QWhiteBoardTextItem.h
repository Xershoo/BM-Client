//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardTextItem.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中文字Item类定义头文件
//  接口：
//      1.startDraw         曲线开始绘制
//      2.drawing           鼠标移动中的绘制
//      3.endDraw           曲线结束
//      4.setHoverColor     设置鼠标移上去的颜色
//      5.setFontSize       设置文字字体的大小
//  修改说明：
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