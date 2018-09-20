#include "qselectcolor.h"
#include "../Courseware/coursewaredatamgr.h"

QSelectColor::QSelectColor(QWidget *parent)
    : C8PopupWindow(parent, "")
{
    ui.setupUi(this);
    connect(ui.pushButton_red, SIGNAL(clicked()), this, SLOT(redClicked()));
    connect(ui.pushButton_yellow, SIGNAL(clicked()), this, SLOT(yellowClicked()));
    connect(ui.pushButton_blue, SIGNAL(clicked()), this, SLOT(blueClicked()));
    connect(ui.pushButton_green, SIGNAL(clicked()), this, SLOT(greenClicked()));
    connect(ui.pushButton_white, SIGNAL(clicked()), this, SLOT(blackClicked()));

    /*connect(ui.pushButton_red, SIGNAL(toggled(bool)), SLOT(doRedToggled(bool)));
    connect(ui.pushButton_yellow, SIGNAL(toggled(bool)), SLOT(doYellowToggled(bool)));
    connect(ui.pushButton_blue, SIGNAL(toggled(bool)), SLOT(doBlueToggled(bool)));
    connect(ui.pushButton_green, SIGNAL(toggled(bool)), SLOT(doGreenToggled(bool)));
    connect(ui.pushButton_white, SIGNAL(toggled(bool)), SLOT(doWhiteToggled(bool)));*/
    
    m_nColor = WB_COLOR_RED;
}

QSelectColor::~QSelectColor()
{

}

void QSelectColor::SetColor(int nColor)
{
    switch (nColor)
    {
    case WB_COLOR_RED:
        ui.pushButton_red->setChecked(true);
        ui.pushButton_yellow->setChecked(false);
        ui.pushButton_blue->setChecked(false);
        ui.pushButton_white->setChecked(false);
        ui.pushButton_green->setChecked(false);
        break;

    case WB_COLOR_BLUE:
        ui.pushButton_blue->setChecked(true);
        ui.pushButton_red->setChecked(false);
        ui.pushButton_yellow->setChecked(false);
        ui.pushButton_white->setChecked(false);
        ui.pushButton_green->setChecked(false);
        break;

    case WB_COLOR_YELLOW:
        ui.pushButton_yellow->setChecked(true);
        ui.pushButton_red->setChecked(false);
        ui.pushButton_blue->setChecked(false);
        ui.pushButton_white->setChecked(false);
        ui.pushButton_green->setChecked(false);
        break;

    case WB_COLOR_WHITE:
        ui.pushButton_red->setChecked(false);
        ui.pushButton_yellow->setChecked(false);
        ui.pushButton_blue->setChecked(false);
        ui.pushButton_white->setChecked(true);
        ui.pushButton_green->setChecked(false);
        break;

    case WB_COLOR_GREEN:
        ui.pushButton_green->setChecked(true);
        ui.pushButton_red->setChecked(false);
        ui.pushButton_yellow->setChecked(false);
        ui.pushButton_blue->setChecked(false);
        ui.pushButton_white->setChecked(false);
        break;
    }
    CoursewareDataMgr::GetInstance()->SetColor((WBColor)nColor);
    emit set_now_color(nColor);
}

void QSelectColor::redClicked()
{
    SetColor(WB_COLOR_RED);
}

void QSelectColor::blueClicked()
{
    SetColor(WB_COLOR_BLUE);
}

void QSelectColor::yellowClicked()
{
    SetColor(WB_COLOR_YELLOW);
}

void QSelectColor::blackClicked()
{
    SetColor(WB_COLOR_WHITE);
}

void QSelectColor::greenClicked()
{
    SetColor(WB_COLOR_GREEN);
}

void QSelectColor::doRedToggled(bool bgoggled)
{
    //redClicked();
    ui.pushButton_red->setChecked(true);
    ui.pushButton_yellow->setChecked(false);
    ui.pushButton_blue->setChecked(false);
    ui.pushButton_white->setChecked(false);
    ui.pushButton_green->setChecked(false);
}

void QSelectColor::doYellowToggled(bool bgoggled)
{
    //yellowClicked();
    ui.pushButton_red->setChecked(false);
    ui.pushButton_yellow->setChecked(true);
    ui.pushButton_blue->setChecked(false);
    ui.pushButton_white->setChecked(false);
    ui.pushButton_green->setChecked(false);
}

void QSelectColor::doBlueToggled(bool bgoggled)
{
    //blueClicked();
    ui.pushButton_red->setChecked(false);
    ui.pushButton_yellow->setChecked(false);
    ui.pushButton_blue->setChecked(true);
    ui.pushButton_white->setChecked(false);
    ui.pushButton_green->setChecked(false);
}

void QSelectColor::doGreenToggled(bool bgoggled)
{
    //greenClicked();
    ui.pushButton_red->setChecked(false);
    ui.pushButton_yellow->setChecked(false);
    ui.pushButton_blue->setChecked(false);
    ui.pushButton_white->setChecked(false);
    ui.pushButton_green->setChecked(true);
}

void QSelectColor::doWhiteToggled(bool bgoggled)
{
    //blackClicked();
    ui.pushButton_red->setChecked(false);
    ui.pushButton_yellow->setChecked(false);
    ui.pushButton_blue->setChecked(false);
    ui.pushButton_white->setChecked(true);
    ui.pushButton_green->setChecked(false);
}