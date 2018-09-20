#include "pushpicwidget.h"
#include "ui_pushpicwidget.h"
#include <QImage>
#include <QTimer>
#include <qdebug.h>
#include <QPainter>
#include <QFile>

PushPicWidget::PushPicWidget(QWidget *parent) :
    QWidget(parent),m_currentPicCount(-1),isRolling(false),
    ui(new Ui::PushPicWidget)
{
    m_pushTimer = 0;
    m_pushSpeedTimer = 0;
    m_picWidgetWidget = 0;
    m_move_x = 0;
    ui->setupUi(this);
}

PushPicWidget::~PushPicWidget()
{
    delete ui;
}

void PushPicWidget::paintEvent(QPaintEvent *)
{
    if(isRolling)
    {
        picRoll(m_picVec[m_currentPicCount%m_picVec.size()],m_picVec[(m_currentPicCount+1)%m_picVec.size()]);
    }else
		if(m_picVec.size()>0)
		{
			QPainter painter(this);
			painter.drawImage(0,0,m_picVec[0]);
		}
    isRolling = false;
}

void PushPicWidget::picRoll(const QImage &font,const QImage &behind)
{
    QPainter painter(this);
    painter.drawImage(m_move_x,0,font);
    painter.drawImage(m_move_x-font.width(),0,behind);
}

void PushPicWidget::timerEvent(QTimerEvent *event)
{
    m_picWidgetWidget = this->width();
    if(event->timerId() == m_pushSpeedTimer)
    {
        m_move_x += X_MOVE;
        if(m_move_x>m_picWidgetWidget)
            m_move_x = m_picWidgetWidget;
        isRolling = true;
        update();
    }
    else if(event->timerId() == m_pushTimer)
    {
		++m_currentPicCount;
        killTimer(m_pushSpeedTimer);
        m_pushSpeedTimer = 0;
        m_pushSpeedTimer = startTimer(ROLLSPEED);
        m_move_x = 0;
    }

}
void PushPicWidget::addPic(const QString &fileName)
{
    if(QFile(fileName).exists())
        m_picVec.append(QImage(fileName));
}
void PushPicWidget::show()
{
    m_pushTimer = startTimer(ROLLPICTIMER);
    QWidget::show();
}

void PushPicWidget::hide()
{
    isRolling = false;
    killTimer(m_pushTimer);
	killTimer(m_pushSpeedTimer);
    m_pushTimer = 0;
	m_pushSpeedTimer = 0;
    m_move_x = 0;
    QWidget::hide();
}
