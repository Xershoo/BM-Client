#include "qclassremindwindow.h"

#define DEFAULT_LENGTH_TIME (60*1000)    //延时提醒窗口默认显示60秒

QClassRemindWindow::QClassRemindWindow(ShowType showtype, QString teacher, QString planTime, QString actualTime, QString latencyTime, QString waitTime)
{
    ui.setupUi(this);
    
    m_nShowType = showtype;
    switch (showtype)
    {
    case RemindClassOver:
        ShowRemindClassOver(teacher);
        break;

    case StatisticsClassTime:
        ShowStatisticsClassTime(planTime, actualTime, latencyTime);
        break;

    case LatencyClassOver:
        ShowLatencyClassOver(teacher);
        break;

    case WatiStartClass:
        ShowWatiStartClass(waitTime);
        break;
    }
    connect(ui.pushButton_close, SIGNAL(clicked()), this, SLOT(CloseClicked()));

	resetContentsMargin(ui.horizontalLayout);
}

QClassRemindWindow::~QClassRemindWindow()
{

}

void QClassRemindWindow::setTitleBarRect()
{
    m_titlRect = ui.widget_titleBar->geometry();
}

int QClassRemindWindow::exec()
{
    return QDialog::exec();
}

void QClassRemindWindow::CloseClicked()
{
    rejectBtnClicked();
}

void QClassRemindWindow::acceptBtnClicked()
{
    QDialog::accept();
}

void QClassRemindWindow::rejectBtnClicked()
{
    QDialog::reject();
}

void QClassRemindWindow::ShowRemindClassOver(QString teacher)
{
    ui.stackedWidget->setCurrentWidget(ui.page_IsClassOver);
    QString strinfo = ui.label_isclassover_info->text();
    QString text;
    text.append("<p style='line-height:150%'>").append(strinfo).append("</p>");
    ui.label_isclassover_info->setText(QString("%1").arg(text).arg(teacher));
    connect(ui.pushButton_OK, SIGNAL(clicked()), this, SLOT(OkClicked()));
    connect(ui.pushButton_Cancel, SIGNAL(clicked()), this, SLOT(CancelClicked()));
}

void QClassRemindWindow::OkClicked()
{
    acceptBtnClicked();
}

void QClassRemindWindow::CancelClicked()
{
    rejectBtnClicked();
}

void QClassRemindWindow::ShowStatisticsClassTime(QString planTime, QString actualTime, QString latencyTime)
{
    ui.stackedWidget->setCurrentWidget(ui.page_ClassOver);
    QString strPlan = ui.label_plan_time->text();
    QString strActual = ui.label_actual_time->text();
    QString strLatency = ui.label_latency_time->text();
    
    //ui.label_plan_time->setText(QString("%1").arg(strPlan).arg(planTime));
    
    ui.label_plan_time->hide();

    ui.label_actual_time->setText(QString("%1").arg(strActual).arg(actualTime));
    ui.label_latency_time->setText(QString("%1").arg(strLatency).arg(latencyTime));
	
    connect(ui.pushButton_iknow, SIGNAL(clicked()), this, SLOT(IKnowClicked()));
}

void QClassRemindWindow::IKnowClicked()
{
    acceptBtnClicked();
}

void QClassRemindWindow::ShowLatencyClassOver(QString teacher)
{
    ui.stackedWidget->setCurrentWidget(ui.page_is_plan_classover);
    QString strinfo = ui.label_planclassover->text();
    QString text;
    text.append("<p style='line-height:150%'>").append(strinfo).append("</p>");
    ui.label_planclassover->setText(QString("%1").arg(text).arg(teacher));
    connect(ui.pushButton_retard_classover, SIGNAL(clicked()), this, SLOT(LatencyClicked()));
    connect(ui.pushButton_plan_classover, SIGNAL(clicked()), this, SLOT(PlanClicked()));

    m_nTimerID = this->startTimer(DEFAULT_LENGTH_TIME);
}

void QClassRemindWindow::LatencyClicked()
{
    acceptBtnClicked();
}

void QClassRemindWindow::PlanClicked()
{
    rejectBtnClicked();
}

void QClassRemindWindow::ShowWatiStartClass(QString waitTime)
{
    ui.label_remding_title->setText(tr("WaitTitle"));
    ui.stackedWidget->setCurrentWidget(ui.page_wait_start_class);
    if (waitTime.isEmpty())
    {
        ui.label_wait_startclass->setText(QString(tr("WaitStartClassOnCourse")));
    }
    else
    {        
        ui.label_wait_startclass->setText(QString(tr("WaitStartClass")).arg(waitTime));
    }
    connect(ui.pushButton_iknow_2, SIGNAL(clicked()), this, SLOT(IKnowClicked()));
}

void QClassRemindWindow::timerEvent(QTimerEvent *event)
{
    if (NULL == event)
    {
        return;
    }

    if (m_nTimerID == event->timerId())
    {
        this->killTimer(m_nTimerID);
        m_nTimerID = 0;

        switch (m_nShowType)
        {
        case RemindClassOver:
            {

            }
            break;

        case StatisticsClassTime:
            {

            }
            break;

        case LatencyClassOver:
            {
                CloseClicked();
            }
            break;

        case WatiStartClass:
            {

            }
            break;

        default:
            break;
        }
    }
}