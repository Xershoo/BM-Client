#include "systemtray.h"
#include "common/Env.h"
#include "setdebugnew.h"

template<class T>void safe_delete(T &t){if(t){delete t;t = NULL;}}

SystemTray::SystemTray(QObject *parent)
	: QObject(parent), m_trayIcon(NULL),sysMenue(NULL)
{
	QIcon icon = QIcon(Env::currentThemeResPath() + "systemTray.ico");
	m_trayIcon = new QSystemTrayIcon(this);
	m_trayIcon->setIcon(icon);
	m_trayIcon->setToolTip(QObject::tr("productName"));
	m_trayIcon->show();
	m_trayIcon->showMessage(QObject::tr("productName"), tr("welcomUse"), QSystemTrayIcon::Information, 5000);
	connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showSystemMenue(QSystemTrayIcon::ActivationReason)));
	sysMenue.setSysSettingActionEnable(false);
	sysMenue.setLogoutActionEnable(false);
	sysMenue.setOpenMainWndEnabel(false);
}

void SystemTray::showSystemMenue(QSystemTrayIcon::ActivationReason reason)
{
	QRect rect = m_trayIcon->geometry();
	rect.setX(rect.x());
	rect.setY(rect.y()-sysMenue.height());
	rect.setWidth(sysMenue.width());
	rect.setHeight(sysMenue.height());
	sysMenue.setGeometry(rect);
	sysMenue.show();
}

SystemTray::~SystemTray()
{
	safe_delete(m_trayIcon);
}

void SystemTray::setSysSettingActionEnable(bool enable)
{
	sysMenue.setSysSettingActionEnable(enable);
}
void SystemTray::setLogoutActionEnable(bool enable)
{
	sysMenue.setLogoutActionEnable(enable);
}


void SystemTray::setOpenMainWndEnabel(bool enable)
{
	sysMenue.setEnabled(enable);
}
