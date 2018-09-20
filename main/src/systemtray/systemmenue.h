#ifndef SYSTEMMENUE_H
#define SYSTEMMENUE_H

#include <QWidget>
#include "ui_systemmenue.h"
#include "control/c8popupwindow.h"

class SystemMenue : public C8PopupWindow
{
	Q_OBJECT

public:
	SystemMenue(QWidget *parent = 0);
	~SystemMenue();

	void setSysSettingActionEnable(bool enable);
	void setLogoutActionEnable(bool enable);
	void setOpenMainWndEnabel(bool enable);

private:
	Ui::SystemMenue ui;

private slots:
	void showAboutDlg();
	void logout();
	void systemSetting();
	void quit(); 
	void openMainWindows();
	void feedBack();
	void help();

private:
	bool init();
	void initSystemTrayIcon();
	void initSystemTrayMenu();
	void closeLobbyAndClassroom();
};

#endif // SYSTEMMENUE_H
