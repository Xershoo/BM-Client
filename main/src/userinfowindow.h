#ifndef USERINFOWINDOW_H
#define USERINFOWINDOW_H

#include "control/C8CommonWindow.h"
#include "ui_userinfowindow.h"
#include <QTimer>
#include "varname.h"

const int USERINFO_CLOSE_TIME = 3000;
const int SHOW_USERINFO_TIME = 200;

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

	inline __int64 getUserId(){
		return m_nUserID;
	}
protected slots:

    void doShowHomePage(QString);
    void doPrivateChat(QString);
    void onRecvLoginToken(LPCWSTR);

private:
	void timerEvent( QTimerEvent *event );
    void showHomePage(LPCWSTR wszToken);

	void showUserInfo();
protected:
    void setTitleBarRect();
	virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::UserInfoWindow ui;
	int m_closeTimerId;
	int m_showTimeId;
	bool m_inWindow;
    bool m_bISClickedHome;
    long long  m_nUserID;
};

#endif // USERINFOWINDOW_H
