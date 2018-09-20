#ifndef CHATWINDOWOUTPUTEDIT_H
#define CHATWINDOWOUTPUTEDIT_H

#include <QTextEdit>

class ChatWindowTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    ChatWindowTextEdit(QWidget *parent);
    ~ChatWindowTextEdit();

protected:
    void mouseDoubleClickEvent(QMouseEvent *e);
private:
    
};

#endif // CHATWINDOWOUTPUTEDIT_H
