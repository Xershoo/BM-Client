#ifndef GIFICONPUSHBUTTON_H
#define GIFICONPUSHBUTTON_H

#include <QPushButton>
#include <QMovie>
class GifIconPushButton : public QPushButton
{
    Q_OBJECT

public:
    GifIconPushButton(QWidget *parent);
    ~GifIconPushButton();
    
    void setIconPath(QString normal, QString hover, QString pressed);

protected slots:
    void updateIcon(int);

protected:
    bool eventFilter(QObject *o, QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void resetMovie(QString path);
private:
    QMovie *m_movie;
    QString m_iconNormal;
    QString m_iconHover;
    QString m_iconPressed;
    bool m_pressed;
};

#endif // GIFICONPUSHBUTTON_H
