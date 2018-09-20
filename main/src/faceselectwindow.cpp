#include "faceselectwindow.h"
#include "graphicsfaceselectitem.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QtDebug>
#include <QMovie>
#include <QScrollBar>

FaceSelectWindow::FaceSelectWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlags(Qt::Popup);
    connect(&m_scene, SIGNAL(sg_selectFace(QString)), this, SLOT(selectFace(QString)));
    connect(&m_scene, SIGNAL(sg_leaveFace), this, SLOT(sl_hoverLeaveFaceItem));
    connect(&m_scene, SIGNAL(sg_hoverEnterFace(QPointF, QSizeF, QString)), this, SLOT(sl_hoverEnterFaceItem(QPointF, QSizeF, QString)));
	m_labelFacePreview = new QLabel;
    m_labelFacePreview->setParent(ui.graphicsView);
    m_labelFacePreview->setVisible(false);
    m_labelFacePreview->installEventFilter(this);
    ui.graphicsView->installEventFilter(this);
    m_labelFacePreview->setStyleSheet("border-style: none;");
}

FaceSelectWindow::~FaceSelectWindow()
{
	delete m_labelFacePreview;
}

void FaceSelectWindow::selectFace(QString facePath)
{
	emit sg_selectface(facePath);
}

bool FaceSelectWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_labelFacePreview)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMovie *moive = m_labelFacePreview->movie();
            selectFace(moive->fileName());
            return true;
        }
        if (event->type() == QEvent::Wheel)
        {
            event->ignore();
            return false;
        }
    }
    if (obj == ui.graphicsView)
    {
        if (event->type() == QEvent::Wheel)
        {
            m_labelFacePreview->setVisible(false);
        }
    }
    return QWidget::eventFilter(obj, event);

}

void FaceSelectWindow::sl_hoverEnterFaceItem(QPointF pos, QSizeF size, QString facePath)
{
    QMovie *old = m_labelFacePreview->movie();
    if (old)
    {
        old->stop();
        delete old;
    }
    QPoint pt = pos.toPoint();
    int position = ui.graphicsView->verticalScrollBar()->sliderPosition();
    pt.setY(pt.y() - position);
	pt.setX(pt.x());

	
    m_labelFacePreview->setGeometry(QRect(pt, size.toSize()));
    m_labelFacePreview->setVisible(true);
    QMovie *movie = new QMovie(facePath);
    m_labelFacePreview->setMovie(movie);
    movie->start();
    m_labelFacePreview->show();	
}

void FaceSelectWindow::sl_hoverLeaveFaceItem()
{
    m_labelFacePreview->setVisible(false);
}

void FaceSelectWindow::selectFaceWindowBtnClicked()
{
    int itemWidth = 30;
    int itemHeight = 30;

    QRect rc = ui.graphicsView->rect();
    int itemLineCount = rc.width() / itemWidth;
    int itemHeightCount = rc.height() / itemHeight;

    for (int i = 0; i < 48; i++)
    {
        QString path = QString(":/face/res/face/account/%1.gif").arg(i + 1);
        GraphicsFaceSelectItem *item = new GraphicsFaceSelectItem;
        item->setParent(&m_scene);
        item->setFacePath(path);
        int x = (i % itemLineCount) * itemWidth;
        int line = i / itemLineCount;
        if (x < 0)
            x = 0.0;
        qreal y = (line) * itemHeight;
        item->setPos(x+2.5, y+2.5);
        m_scene.addItem(item);        
    }

    for (int i = 1; i < itemLineCount; i++)
    {
        QLine line(i*itemWidth, 0, i*itemWidth, rc.height());
        m_scene.addLine(line, QPen(QColor(211,228,240)));
    }

    for (int i = 1; i < itemHeightCount; i++)
    {
        QLine line(0, i*itemHeight, rc.width(), i*itemHeight);
        m_scene.addLine(line, QPen(QColor(211,228,240)));
    }

    QRectF sceneRc = m_scene.sceneRect();
	sceneRc.setWidth(sceneRc.width()-itemWidth);
    //m_scene.addLine(sceneRc, QPen(QColor(255, 0, 0)));
	ui.graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui.graphicsView->setScene(&m_scene);
}