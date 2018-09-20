#ifndef C8COMMOMWINDOW_H
#define C8COMMOMWINDOW_H

//#include <QMainWindow>
#include <QDialog>
class C8CommonWindow : public QDialog/*QMainWindow*/
{
    Q_OBJECT

public:
    C8CommonWindow(QWidget *parent = 0,bool shadow = true);
    ~C8CommonWindow();

protected:
    virtual void setTitleBarRect() = 0;

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    //virtual void paintEvent(QPaintEvent *event);
    void setWindowRoundCorner(QWidget *widget, int roundX = 2, int roundY = 2);
protected:
    QRect m_titlRect;
	void setShadow(bool shadow);
private:
    bool m_moveable;
    QPoint m_dragPosition;
// private:
//     Ui::C8CommomWindow ui;
};

#endif // C8COMMOMWINDOW_H
