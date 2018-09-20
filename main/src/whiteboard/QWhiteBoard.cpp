//**********************************************************************
//	Copyright （c） 2018,浙江邦芒数据有限公司. All rights reserved.
//	文件名称：QWhiteBoard.cpp
//	版本号：1.0
//	作者：谢文兵
//	时间：2015.12.9
//	说明：封装了白板显示view的类定义实现文件
//  修改说明：
//**********************************************************************

#include "QWhiteBoard.h"
#include <QtGui/QPalette>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QTextEdit>
#include "biz/interface/interface.h"
#include "BizInterface.h"
#include "Env.h"
#include <QPixmap>

#define DEFAULT_VIEW_WIDTH      (800)
#define DEFAULT_VIEW_HEIGHT     (600)

#define DEFAULT_EDIT_WIDTH		(200)
#define DEFAULT_EDIT_HEIGHT		(100)

#define MIN_EDIT_WIDTH			(12)
#define MIN_EDIT_HIGHT			(16)
//////////////////////////////////////////////////////////////////////////
///
QWhiteBoardEdit::QWhiteBoardEdit(QWidget *parent /* = 0 */) : QTextEdit(parent)
{
	return;
}

QWhiteBoardEdit::QWhiteBoardEdit(const QString &text, QWidget *parent /* = 0 */):QTextEdit(text,parent)
{

}

QWhiteBoardEdit::~QWhiteBoardEdit()
{

}

void QWhiteBoardEdit::focusOutEvent(QFocusEvent *event)
{
	this->hide();

	QWhiteBoardView* viewWhiteBoard = dynamic_cast<QWhiteBoardView*>(parent());
	if(NULL == viewWhiteBoard)
	{
		return;
	}

	QString text = this->toPlainText();
	if(text.isEmpty())
	{
		return;
	}

	QPointF pos = this->pos();
	viewWhiteBoard->showTextItem(pos,text);
}


//////////////////////////////////////////////////////////////////////////
QWhiteBoardView::QWhiteBoardView(QWidget* parent /* = NULL */,bool autoFill /* = true */)
    :QGraphicsView(parent)
    ,m_sceneWhiteBoard(NULL)
	,m_txtEdit(NULL)
    ,m_setSceneRect(false)
    ,m_userId(0)
    ,m_courseId(0)
    ,m_ownerId(0)
    ,m_pageId(0)
	,m_pixBack(NULL)
    ,m_clrBack(0,128,0,255)
{
    m_sceneWhiteBoard = new QWhiteBoardScene(this);
    
	setAttribute(Qt::WA_InputMethodEnabled,true);
    setScene(m_sceneWhiteBoard);

    setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(FullViewportUpdate);    
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff);

	m_autoSize = autoFill;

    if(parent)
    {
        if(autoFill)
        {
            QGridLayout * layout = new QGridLayout();
            layout->addWidget(this);
            parent->setLayout(layout);

            setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        }
    }
    /* 如果没有父窗口则不设置场景大小
    else
    {
        m_sceneWhiteBoard->setSceneRect(-DEFAULT_VIEW_WIDTH/2,
            -DEFAULT_VIEW_HEIGHT/2,
            DEFAULT_VIEW_WIDTH,
            DEFAULT_VIEW_HEIGHT);
        m_setSceneRect = true;
    }
    */

    setFocus();    
    connect(m_sceneWhiteBoard,SIGNAL(changeCursor(WBMode)),this,SLOT(sceneChangeCursor(WBMode)));
    connect(m_sceneWhiteBoard,SIGNAL(changeItem(int,int,const string&)),this,SLOT(sceneChangeItem(int,int,const string&)));

    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback,SIGNAL(WhiteBoardEvent(biz::SLWhiteBoardEvent)),this,SLOT(handleWhiteBoardEvent(biz::SLWhiteBoardEvent)));
    }

	initTextEdit();
}

QWhiteBoardView::~QWhiteBoardView()
{
	if(NULL!=m_pixBack) {
		delete m_pixBack;
		m_pixBack = NULL;
	}
}

void QWhiteBoardView::initTextEdit()
{
	if(NULL != m_txtEdit)
	{
		return;
	}

	m_txtEdit = new QWhiteBoardEdit(this);	
	m_txtEdit->resize(0,0);
	m_txtEdit->move(0,0);
	m_txtEdit->hide();
}

void QWhiteBoardView::showTextEdit(QPointF pos)
{
	if(NULL == m_txtEdit)
	{
		return;
	}

	QPoint	posView = this->mapFromScene(pos.x(),pos.y());
	QRect	rect = this->rect();
	int		width = rect.width() - posView.x() - 4;
	int		height = rect.height() - posView.y() - 2;

	width = width > DEFAULT_EDIT_WIDTH ? DEFAULT_EDIT_WIDTH:width;
	height = height > DEFAULT_EDIT_HEIGHT ? DEFAULT_EDIT_HEIGHT:height;

	if(width < MIN_EDIT_WIDTH || height < MIN_EDIT_HIGHT)
	{
		return;
	}
	
	//m_txtEdit->setTextBackgroundColor(m_clrBack);
	//m_txtEdit->setTextColor(m_sceneWhiteBoard->getColor());	
	m_txtEdit->move(posView.x(),posView.y());
	m_txtEdit->setText(QString(""));
	m_txtEdit->resize(width,height);
	m_txtEdit->show();
	m_txtEdit->setFocus();
}

void QWhiteBoardView::showTextItem(QPointF pos,QString& text)
{
	if(text.isEmpty())
	{
		return;
	}

	if(NULL == m_sceneWhiteBoard)
	{
		return;
	}

	QPointF posScene = this->mapToScene(pos.x(),pos.y());
	m_sceneWhiteBoard->addTextItem(text,posScene);
}

void QWhiteBoardView::undo()
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->undo(m_userId,true);
}

void QWhiteBoardView::clear()
{
    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->clearAll(m_userId,true);
}

void QWhiteBoardView::resizeEvent(QResizeEvent * event)
{
    QGraphicsView::resizeEvent(event);
    QSize sizeView = event->size();
	
	if(!m_setSceneRect && !m_autoSize)
	{
		return;
	}

    if(m_sceneWhiteBoard)
    {	
        if(m_setSceneRect)
        {
            QRectF  rectScene = m_sceneWhiteBoard->sceneRect();
            fitInView(rectScene, Qt::KeepAspectRatio);
        }
        else
        {
            m_sceneWhiteBoard->setSceneRect(-sizeView.width()/2,
                -sizeView.height()/2,
                sizeView.width(),
                sizeView.height());
         
            m_setSceneRect = true;
        }
    }
}

void QWhiteBoardView::sceneChangeCursor(WBMode mode)
{
    switch(mode)
    {
    case WB_MODE_NONE:
        {
			this->setCursor(QCursor(QPixmap(Env::currentThemeResPath() + "cursor_other.png")));
        }
        break;
    case WB_MODE_LARSER:
        {
			this->setCursor(QCursor(QPixmap(Env::currentThemeResPath() + "cursor_other.png")));
        }
        break;
    case WB_MODE_CURVE:
        {
			this->setCursor(QCursor(QPixmap(Env::currentThemeResPath() + "cursor_other.png")));
        }
        break;
    case WB_MODE_TEXT:
        {
			this->setCursor(QCursor(QPixmap(Env::currentThemeResPath() + "cursor_other.png")));
        }
        break;
    case WB_MODE_ERASER:
        {
			this->setCursor(QCursor(QPixmap(Env::currentThemeResPath() + "cursor_erase.png")));  
        }
        break;
    }
}

void QWhiteBoardView::sceneChangeItem(int type,int id,const string& content)
{
    biz::SLWhiteBoardEvent wbEvent;

    wbEvent._nPaintId = id;
    wbEvent._nPainType =  (biz::PainType)type;
    wbEvent._nRoomId = m_courseId;
    wbEvent._nPageId = m_pageId;
    wbEvent._nUserId = m_userId;
    wbEvent._nOwerUserId = m_ownerId;
    wbEvent._strParam = content;

    biz::GetBizInterface()->SendWhiteBoardEvent(wbEvent);
}

void QWhiteBoardView::handleWhiteBoardEvent(biz::SLWhiteBoardEvent event)
{
    if (event._nRoomId != m_courseId || 
        event._nOwerUserId != m_ownerId ||
        event._nPageId != m_pageId || 
        event._nUserId == m_userId)
    {
        return;
    }

    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }
    
    m_sceneWhiteBoard->handleItemEvent(event._nUserId,event._nPainType,event._nPaintId,event._strParam);
}

void QWhiteBoardView::handleWhiteBoardEventBySelf(biz::SLWhiteBoardEvent event)
{
    if (event._nRoomId != m_courseId || 
        event._nOwerUserId != m_ownerId ||
        event._nPageId != m_pageId)
    {
        return;
    }

    if(NULL == m_sceneWhiteBoard)
    {
        return;
    }

    m_sceneWhiteBoard->handleItemEvent(event._nUserId,event._nPainType,event._nPaintId,event._strParam);
}

void QWhiteBoardView::setBackColor(QColor clr)
{
    m_clrBack = clr;
}

void QWhiteBoardView::setBackPixmap(const QPixmap* backPixmap)
{
	if(NULL != m_pixBack) {
		delete m_pixBack;
		m_pixBack = NULL;
	}

	if(NULL==backPixmap){
		return;
	}

    m_pixBack = new QPixmap(*backPixmap);
}

void QWhiteBoardView::drawBackground(QPainter *painter, const QRectF &rect)
{
    if (NULL == painter)
    {
        return;
    }

    if(NULL == m_pixBack || m_pixBack->isNull())
    {
        QBrush brushBack(m_clrBack);
        painter->fillRect(rect,brushBack);

        return;
    }
    QRect rectDraw((int)rect.left(),(int)rect.top(),(int)rect.width(),(int)rect.height());
    painter->drawPixmap(rectDraw,*m_pixBack);

    return;
}

void QWhiteBoardView::resizeKeepRadio()
{
    QWidget * widgetParent = dynamic_cast<QWidget*>(this->parent());
    if(NULL == widgetParent)
    {
        return;
    }

    if(!m_setSceneRect || NULL == m_sceneWhiteBoard)
    {
        return;
    }

    QRect   rectParent = widgetParent->rect();
    QRectF  rectScene = m_sceneWhiteBoard->sceneRect();

    qreal   rp = (qreal)rectParent.width() / (qreal)rectParent.height();
    qreal   rs = rectScene.width() / rectScene.width();

    qreal   viewWidth = 0.0L;
    qreal   viewHeight = 0.0L;

    if(rs > rp)
    {
        viewWidth = rectParent.width();
        viewHeight = viewWidth / rs;
    }
    else
    {
        viewHeight = rectParent.height();
        viewWidth = viewHeight * rs;
    }

    this->setGeometry((int)(qreal)((rectParent.width() - viewWidth) / 2),
        (int)(qreal)((rectParent.height() - viewHeight) / 2),
        viewWidth,viewHeight);
}

void QWhiteBoardView::setSizeAndSceneRect(const QRect& rect)
{
	if(rect.width() <= 0 || rect.height() <= 0)
	{
		return;
	}

	this->setGeometry(rect);

	if(NULL == m_sceneWhiteBoard)
	{
		return;
	}

	if(m_setSceneRect)
	{
		return;
	}

	m_sceneWhiteBoard->setSceneRect(-rect.width()/2,
		-rect.height()/2,
		rect.width(),
		rect.height());

	m_setSceneRect = true;

	return;
}

