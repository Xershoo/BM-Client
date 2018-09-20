//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoardScene.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板场景的类定义头文件
//  接口：
//      1.setEnable         设置白板是否可以添加各种白板节点
//      2.setColor          设置白板的曲线和文字的颜色
//      3.setTextSize       设置文字字体的大小
//      4.setMode           设置白板item的类型
//      5.undo              删除最后一个节点
//      6.clearAll          清楚所有item
//      7.setBackColor      设置场景的背景颜色
//  修改说明：
//**********************************************************************

#ifndef WHITEBOARD_SCENE_H
#define WHITEBOARD_SCENE_H

#include <QtWidgets/QGraphicsScene>
#include <string>
#include "WhiteBoardData.h"

using namespace  std;

class QWhiteBoardLarserItem;
class QWhiteBoardCurveItem;
class QWhiteBoardTextItem;

class QWhiteBoardScene : public QGraphicsScene
{
    Q_OBJECT
public:
    QWhiteBoardScene(QObject *parent = NULL);

public:    
    inline void setColor(WBColor clr);
    inline void setTextSize(WBSize tSize);
    inline void setEnable(WBCtrl ctl);

    inline void setUserId(__int64 userId)
    {
        m_userId = userId;       
    }

    inline __int64 getUserId()
    {
        return m_userId;
    }
public:
    void setMode(WBMode mode);
    void undo(__int64 userId,bool setNotify = true);
    void clearAll(__int64 userId,bool setNotify = true);

    void setBackColor(QColor& clr);

	void addTextItem(QString& text,QPointF pos);

	QColor getColor();
public:
    void handleItemEvent(__int64 uid,int type,int id,string& content);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:
    void   delSceneItem(__int64 userId,int itemId);    
    int    getItemId();

protected slots:
    void editCompleteEvent(QWhiteBoardTextItem* itemText);

signals:
    void changeCursor(WBMode mode);
    void changeItem(int type,int id,const string& content);

protected:
    WBSize  m_sizeText;
    WBMode  m_modePaint;
    WBColor m_penColor;
    WBCtrl  m_ctrlPaint;

    int     m_itemId;
    QColor  m_backColor;
    
    QWhiteBoardLarserItem*  m_itemLarser;
    WhiteBoardItem*         m_lastItem;

    __int64                 m_userId;
};

void QWhiteBoardScene::setColor(WBColor clr)
{
    m_penColor = clr;
}

void QWhiteBoardScene::setTextSize(WBSize tSize)
{
    m_sizeText = tSize;
}

void QWhiteBoardScene::setEnable(WBCtrl ctl)
{
    m_ctrlPaint = ctl;
}

#endif