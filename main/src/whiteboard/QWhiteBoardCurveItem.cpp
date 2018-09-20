//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardCurveItem.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板中曲线item的类实现源文件
//  修改说明：
//**********************************************************************

#include "QWhiteBoardCurveItem.h"
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtGui/QPen>
#include <QtGui/QColor>
#include <QtWidgets/QGraphicsScene>
#include "WhiteBoardData.h"
#include "macros.h"

#define DEFAULT_CURVE_WIDTH     (3.0)
//////////////////////////////////////////////////////////////////////////
static int GetLineDirection(QPainterPath::Element& p1,QPainterPath::Element& p2)
{
    qreal x1 = (qreal)p1.x;
    qreal x2 = (qreal)p2.x;
    qreal y1 = (qreal)p1.y;
    qreal y2 = (qreal)p2.y;

    if( x1 == x2)
    {
        if(y1 == y2)
        {
            return LINE_DIRECTION_NONE;
        }
        else if( y1 > y2)
        {
            return LINE_DIRECTION_BOTTOM;
        }
        else 
        {
            return LINE_DIRECTION_TOP;
        }
    }
    else if( x1 > x2)
    {
        if(y1 == y2)
        {
            return LINE_DIRECTION_LEFT;
        }
        else if( y1 > y2)
        {
            return LINE_DIRECTION_LEFTBOTTOM;
        }
        else 
        {
            return LINE_DIRECTION_LEFTTOP;
        }
    }
    else
    {
        if(y1 == y2)
        {
            return LINE_DIRECTION_RIGHT;
        }
        else if( y1 > y2)
        {
            return LINE_DIRECTION_RIGHTBOTTOM;
        }
        else 
        {
            return LINE_DIRECTION_RIGHTTOP;
        }
    }
}

static bool IsDirectionCorner(int l1,int l2)
{
    if(l1 == l2)
    {
        return false;
    }

    switch(l1)
    {
    case LINE_DIRECTION_LEFT:
        {
            if(l2 == LINE_DIRECTION_LEFTTOP || l2 == LINE_DIRECTION_LEFTBOTTOM)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_LEFTTOP:
        {
            if(l2 == LINE_DIRECTION_LEFT || l2 == LINE_DIRECTION_TOP)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_TOP:
        {
            if(l2 == LINE_DIRECTION_LEFTTOP || l2 == LINE_DIRECTION_RIGHTTOP)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_RIGHTTOP:
        {
            if(l2 == LINE_DIRECTION_RIGHT || l2 == LINE_DIRECTION_TOP)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;

    case LINE_DIRECTION_RIGHT:
        {
            if(l2 == LINE_DIRECTION_RIGHTTOP || l2 == LINE_DIRECTION_RIGHTBOTTOM)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_RIGHTBOTTOM:
        {
            if(l2 == LINE_DIRECTION_RIGHT || l2 == LINE_DIRECTION_BOTTOM)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_BOTTOM:
        {
            if(l2 == LINE_DIRECTION_LEFTBOTTOM || l2 == LINE_DIRECTION_RIGHTBOTTOM)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    case LINE_DIRECTION_LEFTBOTTOM:
        {
            if(l2 == LINE_DIRECTION_LEFT || l2 == LINE_DIRECTION_BOTTOM)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        break;
    };

    return true;
}

QWhiteBoardCurveItem::QWhiteBoardCurveItem(QGraphicsItem* parent /* = NULL */) : 
    QGraphicsPathItem(parent),
    m_clrHover(0,128,192,255)
{   
    this->setAcceptHoverEvents(true);

    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsMovable,false);
}

void QWhiteBoardCurveItem::startDraw(QGraphicsSceneMouseEvent * event)
{    
    QPointF fPos = event->scenePos();    
    m_lastPath.moveTo(fPos);
}

void QWhiteBoardCurveItem::drawing(QGraphicsSceneMouseEvent * event)
{
    QPointF fPos = event->scenePos();
    m_lastPath.lineTo(fPos);
    setPath(m_lastPath);
}

void QWhiteBoardCurveItem::endDraw(QGraphicsSceneMouseEvent * event)
{   
    QPainterPath curPath = path();
    QPainterPath endPath;
    int count = curPath.elementCount();
    int pos = 0;

    if(count <= 30)
    {
        return;
    }

    for(int i = 0; i< count;i++)
    {
        QPainterPath::Element em = curPath.elementAt(i);
        bool putElement = true;

        do 
        {
            if( i == 0 || i == 1 || i == (count - 1))
            {
                break;
            }

            QPainterPath::Element em_l = endPath.elementAt(pos-1);

            if (qAbs(em.x - em_l.x ) >=3 || 
                qAbs(em.y - em_l.y ) >=3)
            {
                break;
            }

            QPainterPath::Element em_2 = curPath.elementAt(i-1);
            QPainterPath::Element em_l_2 = endPath.elementAt(pos-2);

            int ld1 = GetLineDirection(em_2,em);
            int ld2 = GetLineDirection(em_l_2,em_l);

            if(IsDirectionCorner(ld1,ld2))
            {
                break;
            }

            if( i%3 == 0)
            {
                break;
            }

            putElement = false;
        } while (false);

        if(!putElement)
        {
            continue;
        }

        if(pos != 0)
        {
            endPath.lineTo(em.x,em.y);
        }
        else
        {
            endPath.moveTo(em.x,em.y);
        }
        
        pos++;
    }

    setPath(endPath);
}

void QWhiteBoardCurveItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{  
    m_penNormal = pen();
}


void QWhiteBoardCurveItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    m_penHover.setColor(m_clrHover);
    m_penHover.setWidth(m_penNormal.width());

    setPen(m_penHover);
}


void QWhiteBoardCurveItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{   
    setPen(m_penNormal);
}

void QWhiteBoardCurveItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    QStyleOptionGraphicsItem styleItem;
    styleItem.initFrom(widget);
    styleItem.state = QStyle::State_None;

    return QGraphicsPathItem::paint(painter, &styleItem, widget);
}

void QWhiteBoardCurveItem::setHoverColor(QColor& clr)
{
    m_clrHover = clr;
}

void QWhiteBoardCurveItem::setPenColor(QColor& clr)
{
    QPen penPaint(QBrush(clr),DEFAULT_CURVE_WIDTH);
    this->setPen(penPaint);
}

void QWhiteBoardCurveItem::getItemString(std::string& strItem)
{
	QRectF rectScene = this->scene()->sceneRect();

    WBFreeCurve wbCurve;
    QPen penCurve = this->pen();
    QColor penColor = penCurve.color();
    
    wbCurve.dwColor = RGB(penColor.red(),penColor.green(),penColor.blue());
    wbCurve.nWidth = penCurve.width();
    
    QPainterPath curPath = path();    
    int count = curPath.elementCount();
    
    for(int i = 0; i< count;i++)
    {
        QPainterPath::Element em = curPath.elementAt(i);
        WBPointF pt;
        pt.X = (float)(em.x - rectScene.left())/rectScene.width();
        pt.Y = (float)(em.y - rectScene.top())/rectScene.width();
        wbCurve.vecPointCalc.push_back(pt);
    }

    wbCurve.ToString(strItem);
	
    return;
}

void QWhiteBoardCurveItem::setPaintElement(WBFreeCurve& wbCurve)
{
    QColor clrPaint(GetRValue(wbCurve.dwColor),
        GetGValue(wbCurve.dwColor),
        GetBValue(wbCurve.dwColor));

    if(wbCurve.vecPointCalc.empty())
    {
        return;
    }

	QRectF rectScene = this->scene()->sceneRect();

    QPainterPath curPath;    
    for(int i = 0; i< wbCurve.vecPointCalc.size();i++)
    {
        WBPointF pt = wbCurve.vecPointCalc.at(i);
		qreal xPos = (qreal)pt.X * rectScene.width() + rectScene.left();
		qreal yPos = (qreal)pt.Y * rectScene.width() + rectScene.top();
        if(i != 0)
        {
            curPath.lineTo((qreal)xPos,(qreal)yPos);
        }
        else
        {
            curPath.moveTo((qreal)xPos,(qreal)yPos);
        }
    }

    setPath(curPath);
    setPenColor(clrPaint);

    update();
}