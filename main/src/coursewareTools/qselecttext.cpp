#include "qselecttext.h"
#include "../Courseware/coursewaredatamgr.h"

QSelectText::QSelectText(QWidget *parent)
    : C8PopupWindow(parent, "")
{
    ui.setupUi(this);
    
    connect(ui.pushButton_big, SIGNAL(clicked()), this, SLOT(bigClicked()));
    connect(ui.pushButton_middle, SIGNAL(clicked()), this, SLOT(middleClicked()));
    connect(ui.pushButton_small, SIGNAL(clicked()), this, SLOT(smallClicked()));
    m_nSize = WB_SIZE_SMALL;
}

QSelectText::~QSelectText()
{

}

void QSelectText::SetFontSize(int nSize)
{
    switch (nSize)
    {
    case WB_SIZE_BIG:
        ui.pushButton_big->setChecked(true);
        ui.pushButton_middle->setChecked(false);
        ui.pushButton_small->setChecked(false);
        break;

    case WB_SIZE_MID:
        ui.pushButton_big->setChecked(false);
        ui.pushButton_middle->setChecked(true);
        ui.pushButton_small->setChecked(false);
        break;

    case WB_SIZE_SMALL:
        ui.pushButton_big->setChecked(false);
        ui.pushButton_middle->setChecked(false);
        ui.pushButton_small->setChecked(true);
        break;
    }
    CoursewareDataMgr::GetInstance()->SetTextSize((WBSize)nSize);
}

void QSelectText::bigClicked()
{
    SetFontSize(WB_SIZE_BIG);
}

void QSelectText::middleClicked()
{
    SetFontSize(WB_SIZE_MID);
}

void QSelectText::smallClicked()
{
    SetFontSize(WB_SIZE_SMALL);
}