#ifndef MASKLABEL_H
#define MASKLABEL_H

#include <QLabel>

class MaskLabel : public QLabel
{
public:
    MaskLabel(QWidget *parent=0);

protected:
    void paintEvent(QPaintEvent *);
};

#endif // MASKLABEL_H