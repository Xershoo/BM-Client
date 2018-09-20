#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_logindialog.h"
#include <QListWidget>
#include "BizCallBack.h"
#include "login/Login.h"

class AccountItem;

class LoginDialog : public C8CommonWindow
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

	void loginByTokenUid(QString acc, QString token, __int64 uid);


signals:
    void showcoursepage(__int64 nUserID);

protected:
    virtual void setTitleBarRect() override;
	void showHistoryAccount();
    bool eventFilter(QObject *, QEvent *);

protected slots:
    void showMinimized();
    void close();

    void doForgetPwd(QString);
    void linkHovered(QString);
    void checkAuto();
    void checkKeeppwd();
    void doLogin();
	void doSelectUser(QString strUser);
	void doChangeUser(const QString &strUser);
    void doChangePwd(const QString &strPwd);
    void removeAccountItem(AccountItem* item);
    void accountListItemChanged(AccountItem* item, int row);

    void DoLoginOK(LoginInfo info);
	void DoLoginError(int nErrorCode);
	void DoLogOut(__int64 nUserId);
    void DoLogKickOut(__int64 nUserId);

    void onConnectServerError(ServerErrorInfo errInfo);
private:
    void addItemToAccountListWidget();
    void setControlState(bool bISEnble);

private:

    Ui::LoginDialog ui;
	QListWidget *m_accountListWidget;
    CLogin      m_login;
};

#endif // LOGINDIALOG_H
