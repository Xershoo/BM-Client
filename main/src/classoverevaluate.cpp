#include "classoverevaluate.h"
#include "biz/interface/IUserInfoData.h"
#include "Util.h"
#include "biz/BizInterface.h"
#include "session/classsession.h"
#include <QString>

ClassOverEvaluate::ClassOverEvaluate(QWidget *parent)
    : C8CommonWindow(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton_evaluateButton, SIGNAL(evaluateSelected(int)), this, SLOT(evaluateBtnSelected(int)));
    connect(ui.pushButton_evaluateButton, SIGNAL(evaluateHover(int)), this, SLOT(evaluateBtnHover(int)));
    connect(ui.pushButton_Cancel, SIGNAL(clicked()), this, SLOT(closeBtnClicked()));
    connect(ui.textEdit_evaluate, SIGNAL(textChanged()), this, SLOT(changeText()));
    setWindowRoundCorner(ui.widget_classoverEvaluate_bk);
    ui.label_TextCount->setText(QString(tr("TextCount")).arg(0));

    m_nEvaluteStar = 5;
    m_strContent = "";
}

ClassOverEvaluate::~ClassOverEvaluate()
{

}

void ClassOverEvaluate::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));

    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void ClassOverEvaluate::submitAndSignoutBtnClicked()
{
    QString content = ui.textEdit_evaluate->toPlainText();

    std::wstring strContent;
    Util::QStringToWString(m_strContent, strContent);
    biz::GetBizInterface()->EvaluateClass(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, m_nEvaluteStar, strContent.data()); 

    C8CommonWindow::close();
}

void ClassOverEvaluate::closeBtnClicked()
{
    biz::GetBizInterface()->EvaluateClass(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, 5, L"");
    C8CommonWindow::close();
}

void ClassOverEvaluate::evaluateBtnSelected(int star)
{
    m_nEvaluteStar = star;
}

void ClassOverEvaluate::evaluateBtnHover(int star)
{
    switch (star)
    {
    case 1:
        ui.label_evaluate->setText(tr("star_1"));
        break;
    case 2:
        ui.label_evaluate->setText(tr("star_2"));
        break;
    case 3:
        ui.label_evaluate->setText(tr("star_3"));
        break;
    case 4:
        ui.label_evaluate->setText(tr("star_4"));
        break;
    case 5:
        ui.label_evaluate->setText(tr("star_5"));
        break;
    }
}

void ClassOverEvaluate::changeText()
{
    m_strContent = ui.textEdit_evaluate->toPlainText();
//     std::string strContent;
//     Util::QStringToString(m_strContent, strContent);
    
    int nSize = m_strContent.count();//strContent.length();
    ui.label_TextCount->setText(QString(tr("TextCount")).arg(nSize));
}