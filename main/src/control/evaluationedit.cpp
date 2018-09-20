#include "evaluationedit.h"
#include <QMimeData>

EvaluationEdit::EvaluationEdit(QWidget *parent)
    : m_edited(false), QTextEdit(parent)
{
    QString html = QString("<body style=\" font-size:9pt;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" color:#dadada;\">%1</span></p></body>").arg(tr("defaultHtml"));
    setHtml(html);
    setCursorWidth(0);

    connect(this, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

EvaluationEdit::~EvaluationEdit()
{

}

void EvaluationEdit::textChanged()
{
    QString textContent = this->toPlainText();  
    int length = textContent.count();  
    int maxLength = 200; // ×î´ó×Ö·ûÊý  
    if(length > maxLength) {  
        int position = this->textCursor().position();  
        QTextCursor textCursor = this->textCursor();  
        textContent.remove(position-(length-maxLength), length-maxLength);  
        this->setText(textContent);  
        textCursor.setPosition(position-(length-maxLength));  
        this->setTextCursor(textCursor);  
    } 
}

void EvaluationEdit::mousePressEvent(QMouseEvent *e)
{
    if (m_edited)        
    {
        QTextEdit::mousePressEvent(e);
        return;
    }

    clear();
    m_edited = true;
    setCursorWidth(1);
    QTextEdit::mousePressEvent(e);

}

bool EvaluationEdit::canInsertFromMimeData(const QMimeData *source) const 
{
    if (source->hasImage())
    {
        return false;
    }
    if (source->hasUrls())
    {
        return false;
    }
    return true;
}