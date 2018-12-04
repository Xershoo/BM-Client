#include "c8commonwindow.h"
#include <QMouseEvent>
#include <QPainter>
#include <QtMath>
#include <QLayout>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QDesktopWidget>
#include "common/Util.h"
#include "common/QtAeroWin.h"

C8CommonWindow::C8CommonWindow(QWidget *parent /* = 0 */,int shadow /* = SHADOW_AERO */)
    : QDialog(parent),m_shadow(shadow)
{
    m_moveable = false;
    m_dragPosition.setX(0);
    m_dragPosition.setY(0);

    setWindowFlags(Qt::FramelessWindowHint);
    setShadow();
}

C8CommonWindow::~C8CommonWindow()
{

}

void C8CommonWindow::setShadow()
{
	if(m_shadow == SHADOW_NO){
		return;
	}

	bool isWinXPHigh = Util::IsXPHigher();
	if(!isWinXPHigh){
		setShadowByQt();
		m_shadow = SHADOW_QT;

		return;
	}

	if(m_shadow == SHADOW_AERO){

		if(setShadowByAero()){
			return;
		}
	}

	setShadowByQt();
	m_shadow = SHADOW_QT;

	return;
}

void C8CommonWindow::setShadowByQt()
{
	setAttribute(Qt::WA_TranslucentBackground);
	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);

	m_shadow = SHADOW_QT;
}

bool C8CommonWindow::setShadowByAero()
{
	if (!QtAeroWin::isCompositionEnabled()) {
		return false;
	}
	
	WId wid = this->winId();
	if(NULL==wid){
		return false;
	}

	HWND hwnd = (HWND)wid;
	if(!::IsWindow(hwnd)){
		return false;
	}

	LONG style = WS_POPUP|WS_BORDER|WS_CAPTION|WS_CLIPCHILDREN ;
	SetWindowLongPtr(hwnd, GWL_STYLE, style);
	
	

	if(!QtAeroWin::extendFrameIntoClientArea(this,5, -1, 5, 5))
	{
		return false;
	}

	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	m_shadow = SHADOW_AERO;

	return true;
	
	/*
	BOOL bEnable = false;
	::DwmIsCompositionEnabled(&bEnable);
	if(!bEnable){
		return false;
	}
	WId wid = this->winId();
	if(NULL==wid){
		return false;
	}

	HWND hwnd = (HWND)wid;
	if(!::IsWindow(hwnd)){
		return false;
	}

	LONG style = WS_POPUP|WS_BORDER|WS_CAPTION|WS_CLIPCHILDREN ;
	SetWindowLongPtr(hwnd, GWL_STYLE, style);

	DWMNCRENDERINGPOLICY ncrp = DWMNCRP_ENABLED;
	HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
	if(FAILED(hr)){
		return false;
	}
		
	const MARGINS shadow = {5, 5, 5, 5};
	hr = DwmExtendFrameIntoClientArea(hwnd, &shadow);
	if(FAILED(hr)){
		return false;
	}

	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	m_shadow = SHADOW_AERO;

	return true;
	*/
}

void C8CommonWindow::mousePressEvent(QMouseEvent *event)
{
    if (m_titlRect.isEmpty())
    {
        setTitleBarRect();
    }
    if (event->button() == Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        if (m_titlRect.contains(event->pos()))
        {
            m_moveable = true;
        }
        event->accept();
    }
}

void C8CommonWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_moveable)
    {
        move(event->globalPos() - m_dragPosition);
    }
    event->accept();
}

void C8CommonWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_moveable = false;
    event->accept();
}

void C8CommonWindow::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
    QPainterPath path;
    QRectF rect = widget->rect();
    path.addRoundRect(rect, roundX, roundY);
    QPolygon polygon= path.toFillPolygon().toPolygon();
    QRegion region(polygon);
    widget->setMask(region);
}

bool C8CommonWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	MSG* msg = (MSG*)message;
	switch (msg->message) {
	case WM_NCCALCSIZE:
		{
			*result=0;
		}
		return true;
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			if(msg->wParam==VK_ESCAPE){
				*result = 0;
				return true;
			}
		}
		break;
	}

	return QDialog::nativeEvent(eventType, message, result);
}

void C8CommonWindow::resetContentsMargin(QLayout* layout)
{
	if(m_shadow == SHADOW_QT || NULL==layout){
		return;
	}

	QMargins mg = layout->contentsMargins();
	if(mg.isNull()){
		return;
	}

	QRect rectScreen = QApplication::desktop()->screenGeometry();
	QRect rectWidget = this->geometry();
	
	QPoint pt((rectScreen.width() - rectWidget.width()) / 2,
		(rectScreen.height() - rectWidget.height()) / 2);
	
	int width = rectWidget.width() - (mg.left() + mg.right());
	int height = rectWidget.height() - (mg.top() + mg.bottom());
	
	this->setGeometry(pt.x(),pt.y(),width,height);

	mg.setLeft(0);
	mg.setRight(0);
	mg.setBottom(0);
	mg.setTop(0);

	layout->setContentsMargins(mg);
	
	this->updateGeometry();
}

void C8CommonWindow::centerWindow()
{
	QRect rectScreen = QApplication::desktop()->availableGeometry();
	QRect rectWidget = this->geometry();

	QPoint pt((rectScreen.width() - rectWidget.width()) / 2,
		(rectScreen.height() - rectWidget.height()) / 2);

	this->move(pt.x(),pt.y());
}