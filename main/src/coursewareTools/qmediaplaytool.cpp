#include "qmediaplaytool.h"
#include "Courseware/coursewaredatamgr.h"
#include "Courseware/CoursewarePanel.h"
#include "session/classsession.h"
#include "media/FilePlayer.h"

#define PLAY_STEP       (5)  //5S
#define FLUSH_SLIDER    (1000)  //1S

QMediaPlayTool::QMediaPlayTool(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton_play, SIGNAL(clicked()), this, SLOT(palyBtnClicked()));
    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(pauseBtnClicked()));
    connect(ui.pushButton_Rew, SIGNAL(clicked()), this, SLOT(rewindBtnClicked()));
    connect(ui.pushButton_FF, SIGNAL(clicked()), this, SLOT(FFBtnClicked()));
    connect(ui.horizontalSlider_play, SIGNAL(valueChanged(int)), this, SLOT(setLineEditValue(int))); 
    connect(ui.horizontalSlider_play, SIGNAL(drag_now_value(int)), this, SLOT(dragNowValue(int)));
    ui.pushButton_pause->hide();
    ui.horizontalSlider_play->setSingleStep(PLAY_STEP);
    m_nTimerID = 0;
    ui.pushButton_FF->setEnabled(false);
    ui.pushButton_Rew->setEnabled(false);
}

QMediaPlayTool::~QMediaPlayTool()
{

}

void QMediaPlayTool::SetPlayTime(long nNowTime, long nTotleTime, unsigned int nState)
{
    if (nTotleTime <= 0 || nNowTime < 0 || nNowTime > nTotleTime)
    {
        return;
    }

    QString strTotle, strNow;
    char szContent[10] = {0};
    sprintf(szContent, "%02d:%02d", nTotleTime/60, nTotleTime%60);
    strTotle = QString("%1").arg(szContent);
    ui.label_totle_time->setText(strTotle);
    memset(szContent, 0, sizeof(szContent));
    sprintf(szContent, "%02d:%02d", nNowTime/60, nNowTime%60);
    strNow = QString("%1").arg(strNow);
    ui.label_now_time->setText(strNow);
    ui.horizontalSlider_play->setMinimum(0);
    ui.horizontalSlider_play->setMaximum(nTotleTime);
    ui.horizontalSlider_play->setValue(nNowTime);
    if (CMediaFilePlayer::PLAY == nState)
    {
        ui.pushButton_pause->show();
        ui.pushButton_play->hide();
    }
    else
    {
        ui.pushButton_play->show();
        ui.pushButton_pause->hide();
    }
    
    connect(CoursewareDataMgr::GetInstance()->m_CoursewarePanel, SIGNAL(setMediaPlayerPro(unsigned int)), this, SLOT(setShowProgress(unsigned int)));

    if (!ClassSeeion::GetInst()->IsTeacher())
    {
           ui.horizontalSlider_play->setEnabled(false);
           ui.pushButton_FF->setEnabled(false);
           ui.pushButton_pause->setEnabled(false);
           ui.pushButton_play->setEnabled(false);
           ui.pushButton_Rew->setEnabled(false);
    }
}

void QMediaPlayTool::palyBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    if (CoursewareDataMgr::GetInstance()->MediaCtrl(CMediaFilePlayer::PLAY))
    {
        ui.pushButton_play->hide();
        ui.pushButton_pause->show();
    }
}

void QMediaPlayTool::pauseBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    if (CoursewareDataMgr::GetInstance()->MediaCtrl(CMediaFilePlayer::PAUSE))
    {
        ui.pushButton_pause->hide();
        ui.pushButton_play->show();
    }
}

void QMediaPlayTool::rewindBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    if (CoursewareDataMgr::GetInstance()->MediaCtrl(-1, ui.horizontalSlider_play->value()))
    {
        if (ui.horizontalSlider_play->value() < ui.horizontalSlider_play->singleStep())
        {
            ui.horizontalSlider_play->setValue(0);
        }
        else
        {
            ui.horizontalSlider_play->setValue(ui.horizontalSlider_play->value() - ui.horizontalSlider_play->singleStep());
        }
    }
}

void QMediaPlayTool::FFBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    if (CoursewareDataMgr::GetInstance()->MediaCtrl(-1, ui.horizontalSlider_play->value()))
    {
        if (ui.horizontalSlider_play->value() > ui.horizontalSlider_play->maximum() - ui.horizontalSlider_play->singleStep())
        {
            ui.horizontalSlider_play->setValue(ui.horizontalSlider_play->maximum());
        }
        else
        {
            ui.horizontalSlider_play->setValue(ui.horizontalSlider_play->value() + ui.horizontalSlider_play->singleStep());
        }
    }
}

void QMediaPlayTool::setLineEditValue(int npos)
{
    QString strNow;
    char szContent[10] = {0};
    sprintf(szContent, "%02d:%02d", npos/60, npos%60);
    strNow = QString("%1").arg(szContent);
    ui.label_now_time->setText(strNow);
    ui.horizontalSlider_play->setToolTip(strNow);

    //CoursewareDataMgr::GetInstance()->MediaCtrl(-1, npos);
}

void QMediaPlayTool::timerEvent(QTimerEvent *event)
{
    if (m_nTimerID == event->timerId())
    {
        if (ui.horizontalSlider_play->maximum() <= ui.horizontalSlider_play->value())
        {
            killTimer(m_nTimerID);
        }
        else
        {
            ui.horizontalSlider_play->setValue(ui.horizontalSlider_play->value() + 1);
        }
    }
}

void QMediaPlayTool::setShowProgress(unsigned int nPos)
{
    ui.horizontalSlider_play->setValue(nPos);
}

void QMediaPlayTool::dragNowValue(int nValue)
{
    //拖动到当前value
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    CoursewareDataMgr::GetInstance()->MediaCtrl(-1, nValue);
}