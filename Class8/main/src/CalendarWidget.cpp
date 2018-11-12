#include "CalendarWidget.h"
#include <QGraphicsDropShadowEffect>
#include <QDate>
#include <QPainter>
#include <QMouseEvent>

#define CALENDAR_MAX_YEAR	(2030)
#define CALENDAR_MIN_YEAR	(2018)
#define CALENDAR_MIN_MONTH	(1)
#define CALENDAR_MAX_MONTH	(12)
#define CALENDAR_MIN_DAY	(1)
#define CALENDAR_MAX_DAY	(31)

QCustomCalendarWidget::QCustomCalendarWidget(QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	QGraphicsDropShadowEffect *wndShadow = new QGraphicsDropShadowEffect;
	wndShadow->setBlurRadius(12.0);
	wndShadow->setColor(QColor(0, 0, 0, 160));
	wndShadow->setOffset(0.0);
	this->setGraphicsEffect(wndShadow);

	QDate curDate = QDate::currentDate();
	m_year = curDate.year();
	m_month = curDate.month();
	m_day = curDate.day();
	m_selDay = -1;

	showCalendar();

	connect(ui.pushButton_preMonth,SIGNAL(clicked()),this,SLOT(prevMonth()));
	connect(ui.pushButton_nextMonth,SIGNAL(clicked()),this,SLOT(nextMonth()));

	ui.widget_Days->setMouseTracking(true);
	ui.widget_Days->installEventFilter(this);
}

QCustomCalendarWidget::~QCustomCalendarWidget()
{

}

void QCustomCalendarWidget::showCalendar()
{
	QString txtYears = QString(tr("yearAndMonth")).arg(m_year).arg(m_month);
	ui.label_Years->setText(txtYears);

	QDate curDate;
	curDate.setDate(m_year,m_month,CALENDAR_MIN_DAY);
	int weekBegin = curDate.dayOfWeek();
	if(weekBegin == 0){
		weekBegin = 6;
	}else{
		weekBegin--;
	}

	m_mday[0] = curDate.addDays(-weekBegin);
	for(int i=1;i<DATE_SHOW_NUMBER;i++){
		m_mday[i] = m_mday[i-1].addDays(1);
	}

	QMetaObject::invokeMethod(ui.widget_Days,"update",Qt::QueuedConnection);
}

void QCustomCalendarWidget::nextMonth()
{
	m_month = (m_month == CALENDAR_MAX_MONTH) ? CALENDAR_MIN_MONTH:(m_month+1);
	if(m_month==CALENDAR_MIN_MONTH){
		m_year++;
	}

	showCalendar();

	if(m_month == CALENDAR_MAX_MONTH && m_year == CALENDAR_MAX_YEAR){
		ui.pushButton_nextMonth->setEnabled(false);
	}

	ui.pushButton_preMonth->setEnabled(true);
}

void QCustomCalendarWidget::prevMonth()
{
	m_month = (m_month == CALENDAR_MIN_MONTH) ? CALENDAR_MAX_MONTH:(m_month-1);
	if(m_month==CALENDAR_MAX_MONTH){
		m_year--;
	}

	showCalendar();

	if(m_month == CALENDAR_MIN_MONTH && m_year == CALENDAR_MIN_YEAR){
		ui.pushButton_preMonth->setEnabled(false);
	}

	ui.pushButton_nextMonth->setEnabled(true);
}

bool QCustomCalendarWidget::eventFilter(QObject * obj, QEvent * event)
{
	if(NULL==obj || NULL == event){
		return false;
	}

	if(obj==ui.widget_Days){
		QEvent::Type eType = event->type();
		if(eType==QEvent::Paint){
			drawDays(dynamic_cast<QPaintEvent*>(event));
			return true;
		}else if(eType == QEvent::MouseMove||
			eType == QEvent::Leave || 
			eType == QEvent::Enter){
			QMetaObject::invokeMethod(ui.widget_Days,"update",Qt::QueuedConnection);
			return true;
		}else if(eType==QEvent::MouseButtonPress){
			QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
			if(NULL== mouseEvent){
				return false;
			}

			m_selDay = getPtInDay(mouseEvent->pos());
			return true;
		}else if(eType==QEvent::MouseButtonRelease){
			QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
			if(NULL== mouseEvent){
				return false;
			}

			int selDay = getPtInDay(mouseEvent->pos());
			if(selDay == m_selDay){
				QString qstrDate=m_mday[m_selDay].toString(QString("yyyyMMdd"));
				emit clickDay(qstrDate);
			}

			m_selDay = -1;
			return true;
		}else{
			return false;
		}
	}

	return QWidget::eventFilter(obj,event);
}

void QCustomCalendarWidget::drawDays(QPaintEvent* event)
{
	if(NULL==event){
		return;
	}

	QPainter painter(ui.widget_Days);
	
	QRect rect;

	int width = (ui.widget_Days->width() - 20)/7;
	int height = (ui.widget_Days->height() - 20)/6;

	QFont fontDay(QString("Microsoft YaHei"),9);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setFont(fontDay);

	for(int i=0;i<DATE_SHOW_NUMBER;i++){
		// 位置偏上和偏左
		rect.setTop(i/7*height);
		rect.setLeft(i%7*width + 8);
		rect.setWidth(width);
		rect.setHeight(height);

		//当天
		if (m_mday[i].month() == QDate::currentDate().month()&&
			m_mday[i].day() == QDate::currentDate().day()){
			
			int x = rect.left() + width / 2;
			int y = rect.top() + height / 2;
			int r = (width > height ? height : width)/2;
			painter.setPen(QColor(0,128,255)); 
			painter.setBrush(QColor(0,128,255)); 
			painter.drawEllipse(QPoint(x,y),r,r);

			painter.setPen(QColor(255,255,255));
		}else{
			//鼠标移上
			QPoint ptCursor = ui.widget_Days->mapFromGlobal(QCursor::pos());
			if(rect.contains(ptCursor)){
				int x = rect.left() + width / 2;
				int y = rect.top() + height / 2;
				int r = (width > height ? height : width)/2;
				painter.setPen(QColor(198,226,255)); 
				painter.setBrush(QColor(198,226,255)); 
				painter.drawEllipse(QPoint(x,y),r,r);
			}

			if(m_mday[i].month() == m_month){
				painter.setPen(QColor(100,177,255));
			}else{
				painter.setPen(QColor(128,128,128));
			}
		}
		
		painter.drawText(rect,Qt::AlignCenter,QString("%1").arg(m_mday[i].day()));
	}
}

int QCustomCalendarWidget::getPtInDay(QPoint& pt)
{
	QRect rect;

	int width = (ui.widget_Days->width() - 20)/7;
	int height = (ui.widget_Days->height() - 20)/6;

	for(int i=0;i<DATE_SHOW_NUMBER;i++){
		// 位置偏上和偏左
		rect.setTop(i/7*height);
		rect.setLeft(i%7*width + 8);
		rect.setWidth(width);
		rect.setHeight(height);

		if(rect.contains(pt)){
			return i;
		}
	}

	return -1;
}