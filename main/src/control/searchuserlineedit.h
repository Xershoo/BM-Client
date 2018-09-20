#ifndef SEARCHUSERLINEEDIT_H
#define SEARCHUSERLINEEDIT_H

#include <QLineEdit>
class QKeyEvent;
class SearchUserLineEdit : public QLineEdit
{
    Q_OBJECT

signals:
    void enterKeyPressed();
public:
    SearchUserLineEdit(QWidget *parent);
    ~SearchUserLineEdit();

    void keyPressEvent(QKeyEvent *e);
private:
    
};

#endif // SEARCHUSERLINEEDIT_H
