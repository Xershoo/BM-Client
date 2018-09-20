#include "WaitDlg.h"
#include <QtWidgets/QLabel>
#include <QtGui/QMovie>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>

QWaitDlg::QWaitDlg(QWidget* parent,bool trans/* =true */):m_parent(parent)
	,m_timerWait(0)
	,m_trans(trans)
{
	ui.setupUi(this);

	if(NULL == parent) {
		this->hide();
		return;
	}

	setDlgPos();
	setDlgFrame();
	initDlgCtrl();
}

QWaitDlg::~QWaitDlg()
{

}

void QWaitDlg::okBtnClicked()
{
	cancel();
}

void QWaitDlg::setDlgPos()
{
	QWidget * parent = (m_parent == NULL)? QApplication::desktop():m_parent;

	QRect prect = parent->rect();
	QRect mrect = this->rect();
	QPoint ppt = m_parent->mapToGlobal(QPoint(0,0));

	int left = ppt.x() + (prect.width() - mrect.width())/ 2;
	int top = ppt.y() + (prect.height() - mrect.height())/2;

	this->setGeometry(left,top,mrect.width(),mrect.height());
}

void QWaitDlg::timerEvent(QTimerEvent * event)
{
	if(NULL==event){
		return;
	}

	int tid = event->timerId();
	if(tid == m_timerWait){
		this->killTimer(m_timerWait);
		m_timerWait = 0;
		endWait();
	}
}

void QWaitDlg::initDlgCtrl()
{
	ui.waitDlg_okButton->setText(QObject::tr("btnOK"));
	ui.waitDlg_okButton->hide();
	QMovie* gifWait = new QMovie(QString(":/res/res/image/default/wait.gif"));
	ui.waitDlg_gifLabel->setMovie(gifWait);
	gifWait->start();

	connect(ui.waitDlg_okButton,SIGNAL(clicked()),this,SLOT(okBtnClicked()));
}

void QWaitDlg::setDlgFrame()
{
	this->setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);    
	
	if(m_trans){
		this->setAttribute(Qt::WA_TranslucentBackground);

		QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
		wndShadow->setBlurRadius(12.0);
		wndShadow->setColor(QColor(0, 0, 0, 160));
		wndShadow->setOffset(0.0);
		this->setGraphicsEffect(wndShadow);
	}else{
		this->setStyleSheet(QString("QDialog#QWaitDlg{background-color:rgb(0,162,232);}"));
		ui.waitDlg_mainWidget->setStyleSheet(QString("QWidget#waitDlg_mainWidget{background-color:rgb(255,255,255);}"));
	}

	this->setWindowState(Qt::WindowActive);
}

void QWaitDlg::wait(QObject* endSender,const char* endSignal, QString waitMsg,QString endMsg)
{
	if(endSender&&endSignal) {
		connect(endSender,endSignal,this,SLOT(endWait()));
	}

	m_endMsg = endMsg;
	ui.waitDlg_txtLabel->setText(waitMsg);
	this->exec();
}

void QWaitDlg::wait(int interval,QString waitMsg,QString endMsg)
{
	if(interval > 0) {
		m_timerWait = this->startTimer(interval);
	}
	
	m_endMsg = endMsg;
	ui.waitDlg_txtLabel->setText(waitMsg);
	this->exec();
}

void QWaitDlg::cancel()
{
	QDialog::accept();
	delete this;
}

void QWaitDlg::endWait(QString msg)
{
	if(m_trans) {
		this->showMinimized();
	}

	if(!msg.isEmpty()) {
		ui.waitDlg_txtLabel->setText(msg);
	}

	ui.waitDlg_okButton->show();

	if(m_trans){
		this->showNormal();
	}
}

void QWaitDlg::endWait()
{
	if(m_trans) {
		this->showMinimized();
	}

	if(!m_endMsg.isEmpty()) {
		ui.waitDlg_txtLabel->setText(m_endMsg);
	}

	ui.waitDlg_okButton->show();

	if(m_trans){
		this->showNormal();
	}
}

void QWaitDlg::showEvent(QShowEvent * event)
{
	this->setAttribute(Qt::WA_Mapped);
	QDialog::showEvent(event);
}