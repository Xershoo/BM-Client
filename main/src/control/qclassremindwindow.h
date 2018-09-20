#ifndef QCLASSREMINDWINDOW_H
#define QCLASSREMINDWINDOW_H

#include "c8commonwindow.h"
#include "ui_qclassremindwindow.h"

class QClassRemindWindow : public C8CommonWindow
{
    Q_OBJECT

public:
    enum ShowType
    {
        RemindClassOver = 0,
        StatisticsClassTime,
        LatencyClassOver,
        WatiStartClass
    };

    explicit QClassRemindWindow(ShowType showtype, QString teacher = "", QString planTime = "", QString actualTime = "", QString latencyTime = "", QString waitTime = "");
    //QClassRemindWindow(QWidget *parent = 0);
    ~QClassRemindWindow();

    void ShowRemindClassOver(QString teacher);
    void ShowStatisticsClassTime(QString planTime, QString actualTime, QString latencyTime);
    void ShowLatencyClassOver(QString teacher);
    void ShowWatiStartClass(QString waitTime);

protected slots:
    void OkClicked();
    void CancelClicked();
    void IKnowClicked();
    void LatencyClicked();
    void PlanClicked();
    void CloseClicked();

public slots:
    virtual int exec();
protected:
    virtual void setTitleBarRect() override;
    void timerEvent(QTimerEvent *event);

protected slots:
    void acceptBtnClicked();
    void rejectBtnClicked();

protected:
    int     m_nTimerID; 
    int     m_nShowType;

private:
    Ui::QClassRemindWindow ui;
};

#endif // QCLASSREMINDWINDOW_H
