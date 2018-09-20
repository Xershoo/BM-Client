#ifndef QMEDIAPLAYTOOL_H
#define QMEDIAPLAYTOOL_H

#include <QWidget>
#include "ui_qmediaplaytool.h"

class QMediaPlayTool : public QWidget
{
    Q_OBJECT

public:
    QMediaPlayTool(QWidget *parent = 0);
    ~QMediaPlayTool();
    void SetPlayTime(long nNowTime, long nTotleTime, unsigned int nState);

protected slots:
    void palyBtnClicked();
    void pauseBtnClicked();
    void rewindBtnClicked();
    void FFBtnClicked();
    void setLineEditValue(int);
    void setShowProgress(unsigned int nPos);
    void dragNowValue(int);

protected:
    void timerEvent(QTimerEvent *);

protected:
    int     m_nTimerID;
    

private:
    Ui::QMediaPlayTool ui;
};

#endif // QMEDIAPLAYTOOL_H
