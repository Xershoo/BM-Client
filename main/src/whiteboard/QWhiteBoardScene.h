//**********************************************************************
//	Copyright ��c�� 2018,�㽭��â�������޹�˾. All rights reserved.
//	�ļ����ƣ�QWhiteBoardScene.h
//	�汾�ţ�1.0
//	���ߣ�л�ı�
//	ʱ�䣺2015.12.9
//	˵������װ�˰װ峡�����ඨ��ͷ�ļ�
//  �ӿڣ�
//      1.setEnable         ���ðװ��Ƿ������Ӹ��ְװ�ڵ�
//      2.setColor          ���ðװ�����ߺ����ֵ���ɫ
//      3.setTextSize       ������������Ĵ�С
//      4.setMode           ���ðװ�item������
//      5.undo              ɾ�����һ���ڵ�
//      6.clearAll          �������item
//      7.setBackColor      ���ó����ı�����ɫ
//  �޸�˵����
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

typedef struct _whiteboard_item_param{
	_whiteboard_item_param():uid(0)
		,type(0)
		,id(0)
		,content("")
	{
	};

	_whiteboard_item_param(__int64 _uid,int _type,int _id,string _content):uid(_uid)
		,type(_type)
		,id(_id)
		,content(_content)
	{
	};

	_whiteboard_item_param& operator=(const _whiteboard_item_param& other){
		uid = other.uid;
		id = other.id;
		type = other.type;
		content = other.content;
		return *this;
	};

	bool operator==(const _whiteboard_item_param& a){
		if (uid == a.uid && 
			id == a.id && 
			type == a.type && 
			content.compare(a.content) == 0){
			return true;
		}
		return false;
	};

	__int64 uid;
	int		type;
	int		id;
	string	content;
}WBITEMPARAM,*LPWBITEMPARAM;

typedef QList<WBITEMPARAM>	QWbItemParamList;

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
	//add for multiple page pdf-whiteboard xiewb 2018.09.27
	void reset();
	void getItemParamList(QWbItemParamList& listParam);
	void setItemParamList(const QWbItemParamList& listParam);
protected:
	void addItemParam(__int64 uid,int type,int id,string& content);
	void delItemParam(__int64 userId,int id);
public:
    void handleItemEvent(__int64 uid,int type,int id,string& content);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:
    void   delSceneItem(__int64 userId,int itemId);    
    int    getItemId();

	QGraphicsItem *	findItem(__int64 userId,int itemId);
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
	QWbItemParamList		m_listItemParam;
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