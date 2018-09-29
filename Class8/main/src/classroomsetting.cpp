#include "classroomsetting.h"
#include "common/Env.h"
#include <QFileDialog>

ClassroomSetting::ClassroomSetting(QWidget *parent)
    : C8CommonWindow(parent,SHADOW_QT)
{
    ui.setupUi(this);
    setWindowRoundCorner(ui.widget_classroomsetting);
    initStyleSheet();
    classroomSettingBtnClicked();

	resetContentsMargin(ui.verticalLayout);
}

ClassroomSetting::~ClassroomSetting()
{

}

void ClassroomSetting::initStyleSheet()
{
    QIcon roomSettingIcon;
    roomSettingIcon.addFile(Env::currentThemeResPath() + "roomSetting_normal.png", QSize(), QIcon::Normal, QIcon::On);
    roomSettingIcon.addFile(Env::currentThemeResPath() + "roomSetting_pressed.png", QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_roomSetting->setIcon(roomSettingIcon);

    QIcon privilegeSettingIcon;
    privilegeSettingIcon.addFile(Env::currentThemeResPath() + "privilegeSetting_normal.png", QSize(), QIcon::Normal, QIcon::On);
    privilegeSettingIcon.addFile(Env::currentThemeResPath() + "privilegeSetting_pressed.png", QSize(), QIcon::Disabled, QIcon::On);
    ui.pushButton_privilegeSetting->setIcon(privilegeSettingIcon);

}

void ClassroomSetting::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void ClassroomSetting::closeBtnClicked()
{
    C8CommonWindow::close();
}

void ClassroomSetting::minsizeBtnClicked()
{
    C8CommonWindow::showMinimized();
}

void ClassroomSetting::classroomSettingBtnClicked()
{
    ui.stackedWidget->setCurrentIndex(0);
    ui.pushButton_roomSetting->setEnabled(false);
    ui.pushButton_privilegeSetting->setEnabled(true);
}

void ClassroomSetting::privilegeSettingBtnClicked()
{
    ui.stackedWidget->setCurrentIndex(1);
    ui.pushButton_roomSetting->setEnabled(true);
    ui.pushButton_privilegeSetting->setEnabled(false);
}

void ClassroomSetting::changeVideoRecordPathBtnClicked()
{
    QString floder = QFileDialog::getExistingDirectory(this, tr(""), "D:/");
    ui.lineEdit_videoPath->setText(floder);

}