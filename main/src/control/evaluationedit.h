#ifndef EVALUATIONEDIT_H
#define EVALUATIONEDIT_H

#include <QTextEdit>

class EvaluationEdit : public QTextEdit
{
    Q_OBJECT

public:
    EvaluationEdit(QWidget *parent);
    ~EvaluationEdit();

    void mousePressEvent(QMouseEvent *);

protected:
    virtual bool canInsertFromMimeData(const QMimeData *source) const override;

protected slots:
    void textChanged();
private:
    bool m_edited;
};

#endif // EVALUATIONEDIT_H
