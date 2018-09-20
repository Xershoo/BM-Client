#ifndef PUSHPICWIDGET_H
#define PUSHPICWIDGET_H

#include <QWidget>
#include <QVector>

namespace Ui {
class PushPicWidget;
}

const int ROLLPICTIMER = 3000;
const int ROLLSPEED = 1;
const int X_MOVE = 3;

class PushPicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PushPicWidget(QWidget *parent = 0);
    ~PushPicWidget();
    void addPic(const QString &fileName);
    void show();
    void hide();
protected:
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *event);
    void picRoll(const QImage &font,const QImage &behind);
private:
    Ui::PushPicWidget *ui;
    int m_pushTimer;
    int m_pushSpeedTimer;
    int m_picWidgetWidget;
    int m_move_x;
    int m_currentPicCount;
    bool isRolling;
    QVector<QImage> m_picVec;
};

#endif // PUSHPICWIDGET_H
