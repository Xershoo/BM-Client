#ifndef SYSTEMTRAY_H
#define SYSTEMTRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include "src/systemtray/systemmenue.h"

class SystemTray : public QObject
{
    Q_OBJECT

public:
    explicit SystemTray(QObject *parent = NULL);
    ~SystemTray();

    void setSysSettingActionEnable(bool enable);
    void setLogoutActionEnable(bool enable);
	void setOpenMainWndEnabel(bool enable);

 private slots:

	void showSystemMenue(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon *m_trayIcon;
	SystemMenue sysMenue;
};

#endif // SYSTEMTRAY_H
