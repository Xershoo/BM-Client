#ifndef VARIABLESIZELABEL_H
#define VARIABLESIZELABEL_H

#include <QLabel>
#include <QPicture>
#include <QPixmap>

class VariableSizeLabel : public QLabel
{
    Q_OBJECT

public:
    VariableSizeLabel(QWidget *parent);
    ~VariableSizeLabel();

public slots:
    void setPixmap(const QPixmap &);
    
protected:
    virtual void resizeEvent(QResizeEvent *e);
private:
    QPixmap m_pic;
    QSize m_originalSize;
};

#endif // VARIABLESIZELABEL_H
