//播放器进度条

#pragma once

#include <QSlider>

class PlayerSlider : public QSlider
{
    Q_OBJECT
public:
    PlayerSlider(QWidget *parent = 0);

signals:
    void drag_now_value(int);

public:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

};
