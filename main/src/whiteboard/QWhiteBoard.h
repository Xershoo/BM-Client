//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoard.h
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板显示view的类定义头文件
//  接口：
//      1.setEnable         设置白板是否可以添加各种白板节点
//      2.setColor          设置白板的曲线和文字的颜色
//      3.setTextSize       设置文字字体的大小
//      4.setMode           设置白板item的类型
//      5.undo              删除最后一个节点
//      6.clear             清楚所有item
//  修改说明：
//**********************************************************************

#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QTextEdit>
#include "WhiteBoardData.h"
#include "QWhiteBoardScene.h"
#include "biz/interface/data.h"

class QWhiteBoardEdit : public QTextEdit
{
	Q_OBJECT
public:
	explicit QWhiteBoardEdit(QWidget *parent = 0);
	explicit QWhiteBoardEdit(const QString &text, QWidget *parent = 0);
	virtual ~QWhiteBoardEdit();

protected:
	virtual void focusOutEvent(QFocusEvent *event);
};

class QWhiteBoardView : public QGraphicsView
{
    Q_OBJECT
public:
    QWhiteBoardView(QWidget* parent = NULL,bool autoFill = true);
    ~QWhiteBoardView();

public:
    inline void setEnable(WBCtrl enable);
    inline void setColor(WBColor clr);
    inline void setTextSize(WBSize tSize);
    inline void setMode(WBMode mode);

    inline void setUserId(__int64 id);
    inline void setCourseId(__int64 id);
    inline void setOwnerId(__int64 id);
    inline void setPageId(int id);

    inline __int64 getUserId();
    inline __int64 getCourseId();
    inline __int64 getOwnerId();
    inline int getPageId();

    void resizeKeepRadio();
	void setSizeAndSceneRect(const QRect& rect);

public:
	//add for multiple page pdf-whiteboard xiewb 2018.09.27
	void reset();
	void getItemParamList(QWbItemParamList& listParam);
	void setItemParamList(const QWbItemParamList& listParam);

public:
    void undo();
    void clear();

    void setBackColor(QColor clr);
    void setBackPixmap(const QPixmap* backPixmap);

public:
	void showTextEdit(QPointF pos);
	void showTextItem(QPointF pos,QString& text);

protected slots:
    void sceneChangeCursor(WBMode mode);
    void sceneChangeItem(int type,int id,const string& content);
    void handleWhiteBoardEvent(biz::SLWhiteBoardEvent event);
    
public slots:
    void handleWhiteBoardEventBySelf(biz::SLWhiteBoardEvent event);

protected:
     virtual void resizeEvent(QResizeEvent *);
     virtual void drawBackground(QPainter *painter, const QRectF &rect);

	 virtual void initTextEdit();
protected:
    QWhiteBoardScene *  m_sceneWhiteBoard;
    bool                m_setSceneRect;
	bool				m_autoSize;

    __int64             m_userId;
    __int64             m_courseId;
    __int64             m_ownerId;
    int                 m_pageId;
    QColor              m_clrBack;
    QPixmap*             m_pixBack;

	QWhiteBoardEdit *   m_txtEdit;
};

void QWhiteBoardView::setEnable(WBCtrl enable)
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->setEnable(enable);
}

void QWhiteBoardView::setColor(WBColor clr)
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->setColor(clr);
}

void QWhiteBoardView::setTextSize(WBSize tSize)
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->setTextSize(tSize);
}

void QWhiteBoardView::setMode(WBMode mode)
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->setMode(mode);
}

void QWhiteBoardView::setUserId(__int64 id)
{
    m_userId = id;
    
    if(NULL != m_sceneWhiteBoard)
    {
        m_sceneWhiteBoard->setUserId(id);
    }
}

void QWhiteBoardView::setCourseId(__int64 id)
{
    m_courseId = id;
}

void QWhiteBoardView::setOwnerId(__int64 id)
{
    m_ownerId = id;
}

void QWhiteBoardView::setPageId(int id)
{
    m_pageId = id;
}

__int64 QWhiteBoardView::getUserId()
{
    return m_userId;
}

__int64 QWhiteBoardView::getCourseId()
{
    return m_courseId;
}

__int64 QWhiteBoardView::getOwnerId()
{
    return m_ownerId;
}

int QWhiteBoardView::getPageId()
{
    return m_pageId;
}

#endif