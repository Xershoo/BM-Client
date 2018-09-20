#include "searchuserlineedit.h"
#include <qDebug>
#include <QKeyEvent>

SearchUserLineEdit::SearchUserLineEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

SearchUserLineEdit::~SearchUserLineEdit()
{

}

void SearchUserLineEdit::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "keyPressEvent";
    if (e->key() == Qt::Key_Enter)
    {
        qDebug() << "Qt::Key_Enter";
        emit enterKeyPressed();
        return;
    }
    return QLineEdit::keyPressEvent(e);
}