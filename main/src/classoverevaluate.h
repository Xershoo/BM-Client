#ifndef CLASSOVEREVALUATE_H
#define CLASSOVEREVALUATE_H

#include "control/c8commonwindow.h"
#include "ui_classoverevaluate.h"

class ClassOverEvaluate : public C8CommonWindow
{
    Q_OBJECT

public:
    ClassOverEvaluate(QWidget *parent = 0);
    ~ClassOverEvaluate();

protected:
    virtual void setTitleBarRect() override;

protected slots:
    void submitAndSignoutBtnClicked();
    void closeBtnClicked();
    void evaluateBtnSelected(int);
    void evaluateBtnHover(int);
    void changeText();

protected:
    QString     m_strContent;
    int         m_nEvaluteStar;

private:
    Ui::ClassOverEvaluate ui;
};

#endif // CLASSOVEREVALUATE_H
