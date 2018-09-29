//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardScene.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板场景的类实现源文件
//
//  修改说明：
//**********************************************************************

#include "QWhiteBoardScene.h"
#include <QtCore/QList>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QTextCursor>
#include "QWhiteBoardLarserItem.h"
#include "QWhiteBoardCurveItem.h"
#include "QWhiteBoardTextItem.h"
#include "QWhiteBoard.h"
#include "biz/interface/data.h"
#include "macros.h"

QWhiteBoardScene::QWhiteBoardScene(QObject *parent /* = NULL */)
    :QGraphicsScene(parent)
    ,m_lastItem(NULL)
    ,m_ctrlPaint(WB_CTRL_NONE)
    ,m_backColor(0,128,0,255)
    ,m_itemId(0)
    ,m_userId(0)
{
    m_modePaint = WB_MODE_NONE;
    m_penColor = WB_COLOR_WHITE;
    m_sizeText = WB_SIZE_MID;

    m_itemLarser = new QWhiteBoardLarserItem;
    m_itemLarser->setColor(QColor(255,128,255,255));
    m_itemLarser->setWidth(15);
    m_itemLarser->setId(0);
    
    addItem(m_itemLarser);

//	setBackgroundBrush(QBrush(m_backColor));

    qRegisterMetaType<std::string>("stdString");
}

void QWhiteBoardScene::undo(__int64 userId,bool setNotify /* = true */)
{
    if(m_ctrlPaint == WB_CTRL_NONE)
    {
        return;
    }

    QList<QGraphicsItem *> listItem = items();
    if(listItem.empty())
    {
        return;
    };

    for(int i= 0 ;i<listItem.count();i++)
    {
        QGraphicsItem* item = listItem.at(i);
        int t = item->type() - QGraphicsItem::UserType;

        if (t == WB_MODE_LARSER)
        {
            continue;
        }
        
        WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);
        if (NULL == wbItem)
        {
            continue;
        }

        if(m_ctrlPaint == WB_CTRL_SELF)
        {
            if(wbItem->getUserId() != userId)
            {
                continue;
            }
        }

        this->removeItem(item);
		this->delItemParam(userId,wbItem->getId());
        if(setNotify)
        {
            WBErase      wbEraser;
            string       itemPaint;
            wbEraser.nUserId = wbItem->getUserId();
            wbEraser.nPaintId = wbItem->getId();
            wbEraser.ToString(itemPaint);

            emit changeItem(biz::PT_ERASER,0,itemPaint);            
        }

        break;        
    }
          
    update();
}

void QWhiteBoardScene::clearAll(__int64 userId,bool setNotify /* = true */)
{
    if (m_ctrlPaint ==  WB_CTRL_NONE )
    {
        return;
    }

    QList<QGraphicsItem *> listItem = items();
    if(listItem.empty())
    {
        return;
    };

    for(int i= 0 ;i<listItem.count();i++)
    {
        QGraphicsItem* item = listItem.at(i);
        int t = item->type() - QGraphicsItem::UserType;

        if(t == WB_MODE_LARSER)
        {
            continue;
        }

        WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);
        if (NULL == wbItem)
        {
            continue;
        }

        if(m_ctrlPaint == WB_CTRL_SELF)
        {
            if(wbItem->getUserId() != userId)
            {
                continue;
            }
        }

        this->removeItem(item);
		this->delItemParam(userId,wbItem->getId());
    }

    if(setNotify)
    {
        string strPaint;
        emit changeItem(biz::PT_CLEAN,0,strPaint);
    }

    update();
}

void QWhiteBoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_lastItem = NULL;

    do 
    {
        if(m_ctrlPaint == WB_CTRL_NONE)
        {
            break;
        }

        if(event->button() != Qt::LeftButton)
        {
            break;
        }

        switch(m_modePaint)
        {
        case WB_MODE_NONE:
        case WB_MODE_ERASER:
            {

            }
            break;
        case WB_MODE_LARSER:
            {
                m_itemLarser->startDraw(event);
                m_itemLarser->setUserId(m_userId);

                emit changeCursor(WB_MODE_LARSER);

                std::string  itemPaint;
                m_itemLarser->getItemString(itemPaint);
                emit changeItem(biz::PT_LASERPOINTER,m_itemLarser->getId(),itemPaint);
            }

            break;

        case WB_MODE_CURVE:
            {
                QWhiteBoardCurveItem* curveItem = new QWhiteBoardCurveItem();

                if(NULL == curveItem)
                {
                    break;
                }

                curveItem->setId(getItemId());
                curveItem->setUserId(m_userId);
                curveItem->startDraw(event);
                curveItem->setPenColor(getColor());

                m_lastItem = curveItem;
                addItem(curveItem);

                return;
            }

            break;
        case WB_MODE_TEXT:
            {				
				QWhiteBoardView* view = dynamic_cast<QWhiteBoardView*>(parent());
				if(NULL == view)
				{
					break;
				}

				view->showTextEdit(event->scenePos());

				break;
				/*
                QWhiteBoardTextItem* textItem = new QWhiteBoardTextItem(QString(""));
                if(NULL == textItem)
                {
                    break;
                }

                addItem(textItem);
				
                textItem->setUserId(m_userId);
                textItem->setId(getItemId());
                textItem->setZValue(1000.0);
                textItem->startDraw(event);
                textItem->setFontSize(m_sizeText);                
                textItem->setDefaultTextColor(getColor());
				textItem->setFocus();

                connect(textItem,SIGNAL(editComplete(QWhiteBoardTextItem*)),this,SLOT(editCompleteEvent(QWhiteBoardTextItem*)));
				*/				
            }

            break;
        }

    } while (false);

    QGraphicsScene::mousePressEvent(event);
}

void QWhiteBoardScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    do 
    {
        if(m_ctrlPaint == WB_CTRL_NONE)
        {
            break;
        }
        
        switch(m_modePaint)
        {
        case WB_MODE_NONE:
        case WB_MODE_TEXT:
            {

            }
            break;
        case WB_MODE_LARSER:
            {
                m_itemLarser->drawing(event);

                std::string  itemPaint;
                m_itemLarser->getItemString(itemPaint);
                emit changeItem(biz::PT_LASERPOINTER,m_itemLarser->getId(),itemPaint);
            }
            return;
        case WB_MODE_ERASER:
            break;

        case WB_MODE_CURVE:
            {
                if(NULL == m_lastItem)
                {
                    break;
                }

                m_lastItem->drawing(event);
                return;
            }

            break;
        }

    } while (false);

    QGraphicsScene::mouseMoveEvent(event);
}

void QWhiteBoardScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    do 
    {
        if(m_ctrlPaint == WB_CTRL_NONE)
        {
            break;
        }

        if(event->button() != Qt::LeftButton)
        {
            break;
        }

        switch(m_modePaint)
        {
        case WB_MODE_NONE:
        case WB_MODE_TEXT:
            {

            }
            break;
        case WB_MODE_LARSER:
            {
                m_itemLarser->endDraw(event);
                emit changeCursor(WB_MODE_NONE);

                std::string  itemPaint;                
                m_itemLarser->getItemString(itemPaint);
                emit changeItem(biz::PT_LASERPOINTER,m_itemLarser->getId(),itemPaint);
            }

            return;
        case WB_MODE_ERASER:
            {
                QTransform tf;
                QGraphicsItem* item = itemAt(event->scenePos(),tf);
                if(NULL == item)
                {
                    break;
                }

                int t = item->type() - QGraphicsItem::UserType;
                if(t == WB_MODE_CURVE || t == WB_MODE_TEXT)
                {
                    std::string  itemPaint;
                    WBErase      wbEraser;
                    WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);

                    if(NULL == wbItem)
                    {
                        break;
                    }

                    if(m_ctrlPaint == WB_CTRL_SELF && wbItem->getUserId() != m_userId)
                    {
                        break;
                    }

                    wbEraser.nUserId = wbItem->getUserId();
                    wbEraser.nPaintId = wbItem->getId();
                    wbEraser.ToString(itemPaint);
                    emit changeItem(biz::PT_ERASER,0,itemPaint);

					this->delItemParam(wbItem->getUserId(),wbItem->getId());
                }                
                else
                {
                    break;
                }
                
                removeItem(item);
                update();
            }

            return;

        case WB_MODE_CURVE:
            {
                if(NULL == m_lastItem)
                {
                    break;
                }

                m_lastItem->endDraw(event);

                std::string  itemPaint;
                m_lastItem->getItemString(itemPaint);
                emit changeItem(biz::PT_PEN,m_lastItem->getId(),itemPaint);
				addItemParam(m_userId,biz::PT_PEN,m_lastItem->getId(),itemPaint);
                m_lastItem = NULL;
                return;
            }

            break;
        }

    } while (false);

    QGraphicsScene::mouseReleaseEvent(event);
}

void QWhiteBoardScene::setMode(WBMode mode)
{
	if(m_ctrlPaint ==  WB_CTRL_NONE)
	{
		return;
	}

    m_modePaint = mode;

    switch(mode)
    {
    case WB_MODE_NONE:
    case WB_MODE_TEXT:
    case WB_MODE_CURVE:
    case WB_MODE_ERASER:
        {
            emit changeCursor(mode);
        }
        break;
    }
}

QColor QWhiteBoardScene::getColor()
{
    QColor clr;
    switch(m_penColor)
    {
    case WB_COLOR_RED:
        {
            clr.setRgb(255,0,0);
        }
        break;
    case WB_COLOR_YELLOW:
        {
            clr.setRgb(255,255,0);
        }
        break;
    case WB_COLOR_BLUE:
        {
            clr.setRgb(0,0,255);
        }
        break;
    case WB_COLOR_WHITE:
        {
            clr.setRgb(255,255,255);
        }
        break;
    case WB_COLOR_GREEN:
        {
            clr.setRgb(0,255,0);
        }
        break;
    }
    
    return clr;
}

void QWhiteBoardScene::setBackColor(QColor& clr)
{
    m_backColor = clr;
//	setBackgroundBrush(QBrush(m_backColor));
}

void QWhiteBoardScene::editCompleteEvent(QWhiteBoardTextItem* itemText)
{
    QTextCursor cursorText = itemText->textCursor();
    cursorText.clearSelection();
    itemText->setTextCursor(cursorText);

    if (itemText->toPlainText().isEmpty()) 
    {
        removeItem(itemText);
        itemText->deleteLater();

        return;
    }

    std::string  itemPaint;
    itemText->getItemString(itemPaint);
    emit changeItem(biz::PT_TXT,itemText->getId(),itemPaint);
	addItemParam(m_userId,biz::PT_TXT,itemText->getId(),itemPaint);

    return;
}

int QWhiteBoardScene::getItemId()
{
    return m_itemId++;
}

void QWhiteBoardScene::handleItemEvent(__int64 uid,int type,int id,string& content)
{
	if(uid == m_userId && m_itemId <= id)
	{
		m_itemId = (id + 1);
	}

	if (NULL !=findItem(uid,id) && type != biz::PT_ERASER && 
		type != biz::PT_LASERPOINTER &&
		type != biz::PT_CLEAN)
	{
		return;
	}

    switch(type)
    {
    case biz::PT_PEN:
        {
            if(content.empty())
            {
                return;
            }

            QWhiteBoardCurveItem* curveItem = new QWhiteBoardCurveItem();
            if(NULL == curveItem)
            {
                break;
            }

			addItem(curveItem);

            WBFreeCurve wbCurve;
            wbCurve.FromString(content);
           
            curveItem->setId(id);
            curveItem->setUserId(uid);
            curveItem->setPaintElement(wbCurve);

			addItemParam(uid,type,id,content);
        }
        break;

    case biz::PT_TXT:
        {
            if(content.empty())
            {
                return;
            }

            WBText wbText;
            memset(&wbText,NULL,sizeof(wbText));
            wbText.FromString(content);

            QWhiteBoardTextItem * textItem = new QWhiteBoardTextItem(QString(""));
            if(NULL == textItem)
            {
                return;
            }

            addItem(textItem);

            textItem->setUserId(uid);
            textItem->setId(id);
            textItem->setZValue(1000.0);
            textItem->setItemElement(wbText);

			addItemParam(uid,type,id,content);
            
        }
        break;
    case biz::PT_LASERPOINTER:
        {
            if(content.empty())
            {
                return;
            }

            WBLarser wbLarser;
            wbLarser.FromString(content);

            m_itemLarser->setItemElement(wbLarser);
        }
        break;
    case biz::PT_ERASER:
        {
            if(content.empty())
            {
                return;
            }

            WBErase  wbEraser;
            wbEraser.FromString(content);

            //......
            delSceneItem(wbEraser.nUserId,wbEraser.nPaintId);
			delItemParam(wbEraser.nUserId,wbEraser.nPaintId);
        }
        break;
    case biz::PT_CLEAN:
        {
            QList<QGraphicsItem *> listItem = items();
            if(listItem.empty())
            {
                return;
            };

            for(int i= 0 ;i<listItem.count();i++)
            {
                QGraphicsItem* item = listItem.at(i);
                int t = item->type() - QGraphicsItem::UserType;

                if(t == WB_MODE_LARSER)
                {
                    continue;
                }

                WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);
                if (NULL == wbItem)
                {
                    continue;
                }
                
                if(wbItem->getUserId() != uid )
                {
                    continue;
                }

                this->removeItem(item);
				this->delItemParam(uid,wbItem->getId());
            }
        }
        break;
    case biz::PT_UNDO:
        {

        }
        break;
    default:
        break;
    }

    update();

    return;
}

void QWhiteBoardScene::delSceneItem(__int64 userId,int itemId)
{
    QList<QGraphicsItem *> listItem = items();
    if(listItem.empty())
    {
        return;
    };

    for(int i= 0 ;i<listItem.count();i++)
    {
        QGraphicsItem* item = listItem.at(i);
        int t = item->type() - QGraphicsItem::UserType;

        if (t == WB_MODE_LARSER)
        {
            continue;
        }

        WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);
        if (NULL == wbItem)
        {
            continue;
        }

        if(wbItem->getUserId() != userId || wbItem->getId() != itemId)
        {
            continue;            
        }

        this->removeItem(item);
        break;
    }
}

void QWhiteBoardScene::addTextItem(QString& text,QPointF pos)
{
	if(text.isEmpty())
	{
		return;
	}

	QWhiteBoardTextItem* textItem = new QWhiteBoardTextItem(QString(""));
	if(NULL == textItem)
	{
		return;
	}

	addItem(textItem);

	textItem->setUserId(m_userId);
	textItem->setId(getItemId());
	textItem->setZValue(1000.0);	
	textItem->setFontSize(m_sizeText);                
	textItem->setDefaultTextColor(getColor());
	textItem->setPlainText(text);
	textItem->setPos(pos.x(),pos.y());

	QSize sizeInit = textItem->calcInitSize();
	textItem->setTextWidth(sizeInit.width());

	std::string  itemPaint;
	textItem->getItemString(itemPaint);
	
	emit changeItem(biz::PT_TXT,textItem->getId(),itemPaint);
	addItemParam(m_userId,biz::PT_TXT,textItem->getId(),itemPaint);
}

QGraphicsItem* QWhiteBoardScene::findItem(__int64 userId,int itemId)
{
	QList<QGraphicsItem *> listItem = items();
	if(listItem.empty())
	{
		return NULL;
	};

	for(int i= 0 ;i<listItem.count();i++)
	{
		QGraphicsItem* item = listItem.at(i);
		int t = item->type() - QGraphicsItem::UserType;

		if (t == WB_MODE_LARSER)
		{
			continue;
		}

		WhiteBoardItem* wbItem = dynamic_cast<WhiteBoardItem*>(item);
		if (NULL == wbItem)
		{
			continue;
		}

		if(wbItem->getUserId() == userId && wbItem->getId() == itemId)
		{			
			return item;
		}
	}

	return NULL;
}

void QWhiteBoardScene::reset()
{
	this->clear();
	m_listItemParam.clear();

	update();
}

void QWhiteBoardScene::getItemParamList(QWbItemParamList& listParam)
{
	listParam.clear();
	if(m_listItemParam.empty()){
		return;
	}

	for(int i=0;i<m_listItemParam.size();i++){
		WBITEMPARAM param = m_listItemParam.at(i);
		listParam.append(param);
	}
}

void QWhiteBoardScene::setItemParamList(const QWbItemParamList& listParam)
{
	if(listParam.empty()){
		return;
	}

	for(int i=0;i<listParam.size();i++){
		WBITEMPARAM param = listParam.at(i);
		this->handleItemEvent(param.uid,param.type,param.id,param.content);
	}
}

void QWhiteBoardScene::addItemParam(__int64 uid,int type,int id,string& content)
{
	WBITEMPARAM param(uid,type,id,content);
	m_listItemParam.append(param);
}

void QWhiteBoardScene::delItemParam(__int64 userId,int id)
{
	if(m_listItemParam.empty()){
		return;
	}

	for(int i=0;i<m_listItemParam.size();i++){
		WBITEMPARAM param = m_listItemParam.at(i);
		if(param.uid == userId && param.id == id){
			m_listItemParam.removeOne(param);
			return;
		}
	}
}