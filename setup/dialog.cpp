#include "dialog.h"
#include "ui_dialog.h"
#include "setup.h"
#include "env.h"
#include <QDebug>
#include <QPalette>
#include <QPainter>
#include <QtMath>
#include <QGraphicsDropShadowEffect>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),m_setUp(NULL),m_picPlayTimer(0),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/image/res/image/MainBig.ico"));

    m_setUp = new SetUp;
    ui->welcome_widget->setSetUp(m_setUp);
    ui->finish_widget->setSetUp(m_setUp);
    ui->process_widget->setSetUp(m_setUp);
    ui->select_widget->setSetUp(m_setUp);
    ui->serveragreemtn_widget->setSetUp(m_setUp);

	ui->welcome_widget->setWindowRoundCorner(ui->welcome_widget,2,2);
	ui->finish_widget->setWindowRoundCorner(ui->finish_widget,2,2);
	ui->process_widget->setWindowRoundCorner(ui->process_widget,2,2);
	ui->select_widget->setWindowRoundCorner(ui->select_widget,2,2);
	ui->serveragreemtn_widget->setWindowRoundCorner(ui->serveragreemtn_widget,2,2);

    ui->welcome_widget->show();
    ui->finish_widget->hide();
    ui->process_widget->hide();
    ui->select_widget->hide();
    ui->serveragreemtn_widget->hide();

    connect(ui->welcome_widget,SIGNAL(sg_fastInstallBtnClick()),this,SLOT(welcomeFastInstallClicked()));
    connect(ui->welcome_widget,SIGNAL(sg_customInstallBtnClick()),this,SLOT(customInstallBtnClicked()));
	connect(ui->welcome_widget,SIGNAL(sg_linkBtnClick()),this,SLOT(showAgreementWidget()));
	connect(ui->serveragreemtn_widget,SIGNAL(sg_cancelBtnClicked()),this,SLOT(agreementCancelBtnClicked()));

    connect(ui->select_widget,SIGNAL(sg_startInstall()),this,SLOT(startInstall()));
	connect(ui->select_widget,SIGNAL(sg_retureClicked()),this,SLOT(returnClicked()));
    connect(ui->process_widget,SIGNAL(sg_installSucess()),this,SLOT(showFinishWidget()));
    connect(m_setUp,SIGNAL(sendProcess(int)),ui->process_widget,SLOT(uncompressProcess(int)));

	m_widgetVec.append( ui->welcome_widget);
	m_widgetVec.append( ui->select_widget);
	m_widgetVec.append( ui->process_widget);
	m_widgetVec.append( ui->finish_widget);
	m_picPlayTimer = startTimer(2000);
	
	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);
	
}

Dialog::~Dialog()
{
    delete ui;
    if(m_setUp)
        delete m_setUp;
}

void Dialog::welcomeFastInstallClicked(){
    ui->welcome_widget->hide();
    ui->process_widget->show();
}

void Dialog::retranslateUI()
{	
	ui->welcome_widget->retranslateUI();
	ui->finish_widget->retranslateUI();
	ui->process_widget->retranslateUI();
	ui->select_widget->retranslateUI();
	ui->serveragreemtn_widget->retranslateUI();
	
}

void Dialog::customInstallBtnClicked(){
    ui->welcome_widget->hide();
    ui->select_widget->show();
}

void Dialog::showFinishWidget()
{
    ui->process_widget->hide();
    ui->finish_widget->show();
}

void Dialog::showAgreementWidget()
{
	ui->welcome_widget->hide();
	ui->serveragreemtn_widget->show();
}

void Dialog::startInstall()
{
    ui->select_widget->hide();
    ui->process_widget->show();
}

void Dialog::agreementCancelBtnClicked()
{
    ui->serveragreemtn_widget->hide();
    ui->welcome_widget->show();
}

void Dialog::returnClicked(){
    ui->select_widget->hide();
    ui->welcome_widget->show();
}

void Dialog::timerEvent(QTimerEvent *event)
{
	if(event->timerId() == m_picPlayTimer)
	{
		QVector<QWidget*>::Iterator it;
		for(it = m_widgetVec.begin();it != m_widgetVec.end();++it)
		{
			if(!(*it)->isHidden())
			{
				QPalette palette;
				palette.setBrush((*it)->backgroundRole(), QBrush(QImage(QString(":/image/res/image/title_%1").arg(m_picCount))));
				(*it)->setPalette(palette);
			}
		}
	}
	++m_picCount;
	m_picCount = m_picCount%3;
}

void Dialog::setWindowRoundCorner(QWidget *widget, int roundX, int roundY)
{
	QPainterPath path;
	QRectF rect = widget->rect();
	path.addRoundRect(rect, roundX, roundY);
	QPolygon polygon= path.toFillPolygon().toPolygon();
	QRegion region(polygon);
	widget->setMask(region);
}