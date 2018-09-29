#ifndef C8COMMOMWINDOW_H
#define C8COMMOMWINDOW_H

#include <QDialog>
class C8CommonWindow : public QDialog
{
	Q_OBJECT

protected:
	enum{
		SHADOW_NO = 0,
		SHADOW_QT,
		SHADOW_AERO,
	};

public:
    C8CommonWindow(QWidget *parent = 0,int shadow = SHADOW_NO);
    ~C8CommonWindow();

	void resetContentsMargin(QLayout* layout);
	int  getShadow(){
		return m_shadow;
	};
protected:
    virtual void setTitleBarRect() = 0;
	
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

    void setWindowRoundCorner(QWidget *widget, int roundX = 2, int roundY = 2);
	void centerWindow();
protected:
    QRect m_titlRect;
	void setShadow();

private:
	void setShadowByQt();
	bool setShadowByAero();

private:
	int		m_shadow;
    bool	m_moveable;
    QPoint	m_dragPosition;
};

#endif // C8COMMOMWINDOW_H
