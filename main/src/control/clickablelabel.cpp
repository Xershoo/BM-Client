#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent)
    : QLabel(parent)
{

}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    emit clicked();
}