#ifndef EVALUATEBUTTON_H
#define EVALUATEBUTTON_H

#include <QPushButton>

class EvaluateButton : public QPushButton
{
    Q_OBJECT

signals:
    void evaluateSelected(int);
    void evaluateHover(int);
public:
    EvaluateButton(QWidget *parent);
    ~EvaluateButton();

    int getStar() {return m_star;}
protected:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void leaveEvent(QEvent *e);
private:
    int m_star;
    int m_hoverStar;
    int m_starCount;
    QSize m_starSize;
    QSize m_starPixmapSize;
};

#endif // EVALUATEBUTTON_H
