#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QWidget *parent);
    ~ClickableLabel();

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *ev);
private:
    
};

#endif // CLICKABLELABEL_H
