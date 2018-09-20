#include "systemmenue.h"
#include <QApplication>
#include "about/aboutdialog.h"
#include "classroomdialog.h"
#include "../session/classsession.h"
#include "logindialog.h"
#include "lobbydialog.h"
#include "../settingdialog.h"
#include "setdebugnew.h"

extern LoginDialog* g_loginDlg;

SystemMenue::SystemMenue(QWidget *parent)
	: C8PopupWindow(parent,"")
{
	ui.setupUi(this);
	connect(ui.openMainWindows_Btn, SIGNAL(clicked()), this, SLOT(openMainWindows()));
	connect(ui.help_Btn, SIGNAL(clicked()), this, SLOT(help()));
	connect(ui.feedBack_Btn, SIGNAL(clicked()), this, SLOT(feedBack()));
	connect(ui.logout_Btn, SIGNAL(clicked()), this, SLOT(logout()));
	connect(ui.quit_Btn, SIGNAL(clicked()), this, SLOT(quit()));
	connect(ui.showAboutDlg_Btn, SIGNAL(clicked()), this, SLOT(showAboutDlg()));
	connect(ui.systemSetting_Btn, SIGNAL(clicked()), this, SLOT(systemSetting()));

}

SystemMenue::~SystemMenue()
{

}

void SystemMenue::systemSetting()
{
	SettingDialog settingDlg;
	settingDlg.exec();
}

void SystemMenue::showAboutDlg()
{
	AboutDialog aboutDlg;
	aboutDlg.exec();
}

void SystemMenue::closeLobbyAndClassroom()
{
	if (ClassRoomDialog::isValid())
	{
		ClassRoomDialog::getInstance()->closeWnd();
        
        if(ClassSeeion::GetInst()->IsTeacher())
        {   
            biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, 0, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
        }
		biz::GetBizInterface()->LeaveClass(ClassSeeion::GetInst()->_nClassRoomId);
		ClassRoomDialog::getInstance()->hide();
		ClassRoomDialog::getInstance()->freeInstance();
		biz::GetBizInterface()->Logout();
	}

	if (LobbyDialog::getInstance()->isVisible())
	{
		LobbyDialog::getInstance()->hide();
		biz::GetBizInterface()->Logout();
	}
}

void SystemMenue::logout()
{
	closeLobbyAndClassroom();
	if(g_loginDlg)
	{
		g_loginDlg->show();
	}
}

void SystemMenue::quit()
{
	closeLobbyAndClassroom();
	qApp->exit(0);
}

void SystemMenue::setLogoutActionEnable(bool enable)
{
	ui.systemSetting_Btn->setEnabled(enable);
}

void SystemMenue::setSysSettingActionEnable(bool enable)
{
	ui.logout_Btn->setEnabled(enable);
}

void SystemMenue::setOpenMainWndEnabel(bool enable)
{
	ui.openMainWindows_Btn->setEnabled(enable);
}

void SystemMenue::openMainWindows()
{
	if(ClassRoomDialog::isValid())
		ClassRoomDialog::getInstance()->show();
}
void SystemMenue::feedBack()
{

}

void SystemMenue::help()
{
}
