#ifndef C8POPUPWINDOW_H
#define C8POPUPWINDOW_H

#include <QWidget>

class C8PopupWindow : public QWidget
{
    Q_OBJECT

public:
    C8PopupWindow(QWidget *parent, QString background);
    ~C8PopupWindow();
    

protected:
    void paintEvent(QPaintEvent *event);

    virtual bool eventFilter(QObject *obj, QEvent *event) override;
protected:
    QString m_background;
private:
    bool m_inWindow;
};

#endif // C8POPUPWINDOW_H
