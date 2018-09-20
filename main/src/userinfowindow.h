#ifndef USERINFOWINDOW_H
#define USERINFOWINDOW_H

#include "control/C8CommonWindow.h"
#include "ui_userinfowindow.h"
#include <QTimer>
#include "varname.h"

const int USERINFO_CLOSE_TIME = 3000;

class UserInfoWindow : public C8CommonWindow
{
    Q_OBJECT

signals:
	void sg_privateChatCreate(__int64 userID);

public:
    UserInfoWindow(QWidget *parent = 0);
    ~UserInfoWindow();
	void show();
    void setHeadIconPath(QString headIconPath);
    void setUserInfo(__int64 userID);
    void setGeometry(const QRect &);

protected slots:

    void doShowHomePage(QString);
    void doPrivateChat(QString);
    void onRecvLoginToken(LPCWSTR);

private:
	void timerEvent( QTimerEvent *event );
    void showHomePage(LPCWSTR wszToken);

protected:
    void setTitleBarRect();
	virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::UserInfoWindow ui;
	int m_closeTimerId;
	bool m_inWindow;
    bool m_bISClickedHome;
    long long  m_nUserID;
};

#endif // USERINFOWINDOW_H
