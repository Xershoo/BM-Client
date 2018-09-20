#include "msgdlg.h"
#include <QPalette>
#include <QPainter>
#include <QtMath>
#include <QGraphicsDropShadowEffect>
MsgDlg::MsgDlg(QWidget *parent)
	: BaseWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowRoundCorner(this,2,2);
	setMoveWidget(this);
	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);

	connect(ui.close_Btn,SIGNAL(clicked()),this,SLOT(on_close_Btn_clicked()));
	connect(ui.msg_know_Btn,SIGNAL(clicked()),this,SLOT(on_close_Btn_clicked()));
}

MsgDlg::~MsgDlg()
{

}

void MsgDlg::setTitleBarRect(){
	QPoint pt = ui.msgDlg_titleBat_widget->mapTo(this, QPoint(0, 0));
	m_titlRect = QRect(pt, ui.msgDlg_titleBat_widget->size());
}

void MsgDlg::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
	QPainterPath path;
	QRectF rect = widget->rect();
	path.addRoundRect(rect, roundX, roundY);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	widget->setMask(region);
}

void MsgDlg::on_close_Btn_clicked()
{
	close();
}