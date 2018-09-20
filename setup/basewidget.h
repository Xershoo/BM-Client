#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <QDialog>
#include "setup.h"

class BaseWidget : public QDialog
{
    Q_OBJECT
public:
    explicit BaseWidget(QWidget *parent = 0);
	virtual ~BaseWidget();
    void setSetUp(SetUp *setup);
	void setMoveWidget(QWidget* widget);
	void setWindowRoundCorner(QWidget *widget, int roundX, int roundY);

	virtual void retranslateUI() = 0;
protected:
	virtual void setTitleBarRect() = 0;
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
    SetUp *m_setup;
	QRect m_titlRect;
private:
	QWidget* m_moveWidget;
    bool m_moveable;
    QPoint m_dragPosition;
};

#endif // BASEWIDGET_H
