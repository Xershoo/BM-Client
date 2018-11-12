#ifndef Q_CALENDAR_WIDGET_H
#define Q_CALENDAR_WIDGET_H

#include <QWidget>
#include <QDate>
#include "ui_CalendarWidget.h"

#define  DATE_SHOW_NUMBER	(42)

class QCustomCalendarWidget : public QWidget
{
	Q_OBJECT
public:
	QCustomCalendarWidget(QWidget* parent);
	~QCustomCalendarWidget();

protected:
	void showCalendar();
	void drawDays(QPaintEvent* event);
	int	 getPtInDay(QPoint& pt);

	virtual bool eventFilter(QObject *, QEvent *);
protected slots:
	void nextMonth();
	void prevMonth();

signals:
	void clickDay(QString);
protected:
	Ui::QCustomCalendarWidget ui;
	
	int m_year;
	int m_month;
	int	m_day;

	QDate m_mday[DATE_SHOW_NUMBER];

	int	m_selDay;
};

#endif