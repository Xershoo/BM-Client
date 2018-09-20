#include "hotkeyinputedit.h"
#include <QKeyEvent>

HotkeyInputEdit::HotkeyInputEdit(QWidget *parent)
    : QLineEdit(parent)
{

}

HotkeyInputEdit::~HotkeyInputEdit()
{

}

void HotkeyInputEdit::hide()
{
    if (this->isVisible())
    {
        QLineEdit::hide();
        emit inputTextChanged(this->text());
    }
}

void HotkeyInputEdit::keyPressEvent(QKeyEvent *e)
{
    Qt::KeyboardModifiers modifiers = e->modifiers();

    if (modifiers & Qt::KeypadModifier)
        return;

    if (e->key() == Qt::Key_Return)
        hide();

    int value = e->key();
    QString mod;
    if (modifiers & Qt::ControlModifier)
    {
        mod = "CTRL";
    }

    if (modifiers & Qt::AltModifier)
    {
        if (mod.isEmpty())
            mod = "ALT";
        else
            mod += " + ALT";
    }

    if (modifiers & Qt::ShiftModifier)
    {
        if (mod.isEmpty())
            mod = "SHIFT";
        else
            mod += " + SHIFT";
    }

    if (value == Qt::Key_Control || value == Qt::Key_Alt || value == Qt::Key_Shift)
    {
        value = 0;
    }

    if (modifiers == 0)
    {
        mod = "CTRL";
        mod += " + ALT";
        QKeySequence seq(value);
        if (!seq.toString().isEmpty())
        {
            mod += " + ";
            mod += seq.toString();
        }
        this->setText(mod.toUpper());
    }
    else
    {
        QKeySequence seq(value);
        if (!seq.toString().isEmpty())
        {
            mod += " + ";
            mod += seq.toString();
        }
        this->setText(mod.toUpper());
    }
}