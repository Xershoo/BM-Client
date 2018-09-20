#include "variablesizelabel.h"
#include <QResizeEvent>
#include <QPicture>

VariableSizeLabel::VariableSizeLabel(QWidget *parent)
    : QLabel(parent)
{

}

VariableSizeLabel::~VariableSizeLabel()
{

}

void VariableSizeLabel::setPixmap(const QPixmap &pic)
{
    QLabel::setPixmap(pic);    
    m_pic = QPixmap(pic);
}

void VariableSizeLabel::resizeEvent(QResizeEvent *e)
{
    if (m_pic.isNull())
        return;

    QSize picSize = m_pic.size();
    if (picSize.width() > e->size().width() || picSize.height() > e->size().height())
    {
        QLabel::setPixmap(m_pic.scaled(e->size(), Qt::KeepAspectRatio));
    }
    else
    {
        QLabel::setPixmap(m_pic);
    }
}