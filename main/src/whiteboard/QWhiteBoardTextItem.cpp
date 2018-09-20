//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardTextItem.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中文字Item类定义头文件
//  接口：
//
//  修改说明：
//**********************************************************************

#include "QWhiteBoardTextItem.h"
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QGuiApplication>
#include <QInputMethod>

#include "macros.h"
#include "util.h"

#define TEXT_ITEM_DEFAULT_WIDTH         (200)
#define TEXT_ITEM_DEFAULT_HIGHT         (100)

QWhiteBoardTextItem::QWhiteBoardTextItem(QString& text,QGraphicsItem* parent /* = NULL */)
    : QGraphicsTextItem(text,parent),
    m_clrHover(0,128,192,255),
    m_endEdit(true),
    m_initRect(false),
    m_sizeFont(WB_SIZE_UNKNOW)
{
    setAcceptHoverEvents(true);
	
    setFlag(QGraphicsItem::ItemIsMovable,false);
    setFlag(QGraphicsItem::ItemIsSelectable);
    
    setFontSize(WB_SIZE_MID);
}

QVariant QWhiteBoardTextItem::itemChange(GraphicsItemChange change,
    const QVariant &value)
{
    if (change == QGraphicsItem::ItemSelectedHasChanged)
    {
        emit selectedChange(this);
    }

    return value;
}

void QWhiteBoardTextItem::focusOutEvent(QFocusEvent *event)
{
    setTextInteractionFlags(Qt::NoTextInteraction);
    m_endEdit = true;

    emit editComplete(this);

    QGraphicsTextItem::focusOutEvent(event);
}

void QWhiteBoardTextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    /*
    if (textInteractionFlags() == Qt::NoTextInteraction)
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        m_endEdit = false;
    }
    */
    QGraphicsTextItem::mouseDoubleClickEvent(event);    
}

void QWhiteBoardTextItem::setHoverColor(QColor& clr)
{
    m_clrHover = clr;
}

void QWhiteBoardTextItem::startDraw(QGraphicsSceneMouseEvent * event)
{    
    setPos(event->scenePos());
    
    QString text = toPlainText();

    if(text.isEmpty())
    {
        setTextInteractionFlags(Qt::TextEditorInteraction);
        m_endEdit = false;

        QSize sizeInit = calcInitSize();
        setTextWidth(sizeInit.width());
    }
    else
    {
        setTextInteractionFlags(Qt::NoTextInteraction);
    }
}

void QWhiteBoardTextItem::drawing(QGraphicsSceneMouseEvent * event)
{
    
}

void QWhiteBoardTextItem::endDraw(QGraphicsSceneMouseEvent * event)
{

}

void QWhiteBoardTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{    
    m_clrNormal = this->defaultTextColor();
}


void QWhiteBoardTextItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{   
    if(m_endEdit)
    {
        this->setDefaultTextColor(m_clrHover);
    }
}


void QWhiteBoardTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{   
    this->setDefaultTextColor(m_clrNormal);
}


void QWhiteBoardTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QStyleOptionGraphicsItem styleItem;
    styleItem.initFrom(widget);
    styleItem.state = QStyle::State_None;

    if(!m_endEdit)
    {
        QRectF rectBound = boundingRect();
        QRect  rectItem(0,0,(int)rectBound.width(),(int)rectBound.height());
        QPen   penRect(QBrush(QColor(0,0,128,255)),1,Qt::DashLine);

        painter->setPen(penRect);
        painter->drawRect(rectItem);
    }

    QGraphicsTextItem::paint(painter, &styleItem, widget);
}

QRectF QWhiteBoardTextItem::boundingRect()
{
    QRectF rectText = QGraphicsTextItem::boundingRect();
    QSize  sizeInit = calcInitSize();

    QRectF rectInit(0,0,sizeInit.width(),sizeInit.height());

    if(!m_initRect)
    {
        m_initRect = true;
        return rectInit;
    }

    if(m_endEdit)
    {
        return rectText;
    }

    if(rectInit.height() > (int)rectText.height())
    {
        return rectInit;
    }

    return rectText;    
}

QSize QWhiteBoardTextItem::calcInitSize()
{
    QSize initSize(TEXT_ITEM_DEFAULT_WIDTH,TEXT_ITEM_DEFAULT_HIGHT);
    QGraphicsScene * curScene = this->scene();
    QRectF rectScene = curScene->sceneRect();
    QPointF ptItem = this->scenePos();

    if(initSize.width() + ptItem.x() - rectScene.x() > rectScene.width())
    {
        initSize.setWidth(rectScene.width() - ptItem.x() + rectScene.x() - 2);
    }

    if(initSize.height() + ptItem.y() - rectScene.y() > rectScene.height())
    {
        initSize.setHeight(rectScene.height() - ptItem.y() + rectScene.y() - 2);
    }

    return initSize;
}

void QWhiteBoardTextItem::setFontSize(WBSize sizeFont)
{
    if(m_sizeFont == sizeFont)
    {
        return;
    }

    QFont textFont = font();

    switch(sizeFont)
    {
    case WB_SIZE_SMALL:
        {
            textFont.setWeight(QFont::Normal);
            textFont.setPointSize(12);
        }
        break;
    case WB_SIZE_MID:
        {
            textFont.setWeight(QFont::Medium);
            textFont.setPointSize(18);
        }
        break;
    case WB_SIZE_BIG:
        {
            textFont.setWeight(QFont::Bold);
            textFont.setPointSize(24);
        }
        break;
    }

    setFont(textFont);
    m_sizeFont = sizeFont;
}

void QWhiteBoardTextItem::getItemString(std::string& strItem)
{
	QRectF  rectScene = this->scene()->sceneRect();

    WBText wbText;
    memset(&wbText,NULL,sizeof(wbText));

    QColor textColor = this->defaultTextColor();
    QRectF textRect = this->boundingRect();
    QString textPlain = this->toPlainText();
    QFont   textFont = this->font();
    QString fontFace = textFont.family();
    QPointF ptItem = this->scenePos();

    wbText.yHeight = (float)m_sizeFont;
    wbText.rcText.X = (float)(ptItem.x() - rectScene.left()) / rectScene.width();
    wbText.rcText.Y = (float)(ptItem.y() - rectScene.top()) /  rectScene.width();
    wbText.rcText.Width = textRect.width();
    wbText.rcText.Height = textRect.height();
    wbText.dwColor = RGB(textColor.red(),textColor.green(),textColor.blue());
    Util::QStringToWString(textPlain,wbText.wstrText);
    fontFace.toWCharArray(wbText.wszFaceName);

    wbText.ToString(strItem);
}

void QWhiteBoardTextItem::setItemElement(WBText wbText)
{
    if(wbText.wstrText.empty())
    {
        return;
    }

	QRectF  rectScene = this->scene()->sceneRect();

    QColor clrPaint(GetRValue(wbText.dwColor),
        GetGValue(wbText.dwColor),
        GetBValue(wbText.dwColor));
        
    QFont textFont = this->font();
	qreal xPos = (qreal)wbText.rcText.X * rectScene.width() + rectScene.left();
	qreal yPos = (qreal)wbText.rcText.Y * rectScene.width() + rectScene.top();

    textFont.setFamily(QString::fromWCharArray(wbText.wszFaceName));
    setFont(textFont);
    setDefaultTextColor(clrPaint);
    setFontSize((WBSize)(int)wbText.yHeight);

    setPlainText(QString::fromWCharArray(wbText.wstrText.c_str()));
    setPos((qreal)xPos,(qreal)yPos);
    setTextWidth((qreal)wbText.rcText.Width);
}

void QWhiteBoardTextItem::inputMethodEvent(QInputMethodEvent *event)
{
	if(NULL == event)
	{
		return;
	}

	QGraphicsTextItem::inputMethodEvent(event);
}

QVariant QWhiteBoardTextItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
	if(query == Qt::ImEnabled)
	{
		QVariant var(true);		
		return var;
	}

	return QGraphicsTextItem::inputMethodQuery(query);
}

void QWhiteBoardTextItem::keyPressEvent(QKeyEvent* event)
{
	if(NULL == event)
	{
		return;
	}
	
	QGraphicsTextItem::keyPressEvent(event);
}