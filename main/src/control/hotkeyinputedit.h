#ifndef HOTKEYINPUTEDIT_H
#define HOTKEYINPUTEDIT_H

#include <QLineEdit>

class HotkeyInputEdit : public QLineEdit
{
    Q_OBJECT

signals:
    void inputTextChanged(QString);
public:
    HotkeyInputEdit(QWidget *parent);
    ~HotkeyInputEdit();

    void keyPressEvent(QKeyEvent *);
public slots:
    void hide();
private:
    
};

#endif // HOTKEYINPUTEDIT_H
