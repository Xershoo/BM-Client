#include "logindialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QListView>
#include <QDebug>
#include <QDateTime>
#include "control/accountitem.h"
#include "lobbydialog.h"
#include "BizInterface.h"
#include "common/Env.h"
#include "qclassremindwindow.h"
#include "systemtray/systemtray.h"
#include "define/publicdefine.h"
#include "session/classsession.h"
#include "common/Config.h"
#include "classroomdialog.h"
#include "SingleApp/SingleApp.h"
#include "c8messagebox.h"

#include "setdebugnew.h"

extern SingleApp *g_singApp;

LoginDialog::LoginDialog(QWidget *parent)
    : C8CommonWindow(parent)
{
    ui.setupUi(this);    
    g_systemTray->setSysSettingActionEnable(false);
    g_systemTray->setLogoutActionEnable(false);
    this->setFixedSize( this->width (),this->height ());

    QString url = QString(Config::getConfig()->m_urlForgetPwd.c_str());
	QString strpwd = ui.label_forget_pwd->text();
	QString strContent = "<a style='font-size:12px;font-family:\"SimSun\";color: rgb(23, 116, 200);text-decoration:none;' href = \"" + url +  "\"> " + strpwd + "</a>";
	ui.label_forget_pwd->setOpenExternalLinks(true);
	ui.label_forget_pwd->setText(strContent);

	QString strregster = ui.label_register->text();
	url = QString(Config::getConfig()->m_urlRegister.c_str());
    strContent = "<a style='font-size:12px;font-family:\"SimSun\";color: rgb(23, 116, 200);text-decoration:none;' href = \"" + url +  "\"> " + strregster + "</a>";
	ui.label_register->setOpenExternalLinks(true);
    ui.label_register->setText(strContent);

    m_accountListWidget = new AccountListWidget();
    connect(m_accountListWidget, SIGNAL(removeItem(AccountItem*)), this, SLOT(removeAccountItem(AccountItem*)));
    connect(m_accountListWidget, SIGNAL(accountListItemClicked(AccountItem*, int)), this, SLOT(accountListItemChanged(AccountItem*, int)));
    addItemToAccountListWidget();

    //ui.lineEdit_pwd->setEchoMode(QLineEdit::Password);
    //ui.lineEdit_pwd->setStyleSheet("lineedit-password-character: 42;");
  
	//connect(ui.label_forget_pwd,SIGNAL(linkActivated(QString)),this,SLOT(doForgetPwd(QString)));  
	//connect(ui.label_forget_pwd, SIGNAL(linkHovered(QString)),this, SLOT(linkHovered(QString)));
	connect(ui.checkBox_autoLogin, SIGNAL(clicked()), this, SLOT(checkAuto()));
	connect(ui.checkBox_keepPwd, SIGNAL(clicked()), this, SLOT(checkKeeppwd()));
	connect(ui.pushButton_logIn, SIGNAL(clicked()), this, SLOT(doLogin()));
	connect(ui.comboBox_name, SIGNAL(activated(QString)), this, SLOT(doSelectUser(QString)));
	connect(ui.comboBox_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(doChangeUser(const QString &)));
    connect(ui.lineEdit_pwd, SIGNAL(textChanged(const QString &)), this, SLOT(doChangePwd(const QString &)));
	
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback, SIGNAL(Login(LoginInfo)), this, SLOT(DoLoginOK(LoginInfo)));
        connect(pCallback, SIGNAL(LoginError(int)), this, SLOT(DoLoginError(int)));
        connect(pCallback, SIGNAL(LogKickOut(__int64)), this, SLOT(DoLogKickOut(__int64)));
        connect(pCallback, SIGNAL(ConnectServerError(ServerErrorInfo)),this,SLOT(onConnectServerError(ServerErrorInfo)));

    }
	showHistoryAccount();  
    QString iconPath = Env::currentThemeResPath();
    ui.gifpushButton_minSize->setIconPath(iconPath + "login_minsize_normal.gif", iconPath + "login_minsize_hover.gif", iconPath + "login_minsize_pressed.gif");
    ui.gifpushButton_close->setIconPath(iconPath + "login_close_normal.gif", iconPath + "login_close_hover.gif", iconPath + "login_close_pressed.gif");
    ui.widget_logininfo->hide();
    setWindowRoundCorner(ui.widget_loginWnd);

    ui.comboBox_name->installEventFilter(this);
    ui.lineEdit_pwd->installEventFilter(this);
	ui.lineEdit_pwd->setMaxLength(MAX_PASSWORD_LEN);
	ui.comboBox_name->lineEdit()->setMaxLength(MAX_ACCOUNT_LEN);

	ui.label_name->setEnabled(false);
    ui.label_pwd->setEnabled(false);
    
    //暂时不上注册账号功能
    //ui.label_register->hide();

    hide();
}

LoginDialog::~LoginDialog()
{
    if (m_accountListWidget)
    {
        delete m_accountListWidget;
        m_accountListWidget = NULL;
    }
}

void LoginDialog::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar_nofill->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar_nofill->size());//ui.titleLabel->frameRect();
}

void LoginDialog::showMinimized()
{
    QDialog::showMinimized();
}

void LoginDialog::close()
{
    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
        disconnect(pCallback,0,0,0);
    C8CommonWindow::close();
}

void LoginDialog::doLogin()
{
    QString strError = "";
    QString strUser = ui.comboBox_name->currentText();
    QString strPwd = ui.lineEdit_pwd->text();
    if (!m_login.BeginLoginToDlg(strUser, strPwd, strError))
    {
        //对错误进行处理
        qDebug("Login Error: %s", strError);
        ui.widget_logininfo->show();
        ui.label_error->setText(strError);
        return;
    }
    
    setControlState(false);
}

void LoginDialog::doForgetPwd(QString strUrl)
{
    QDesktopServices::openUrl(QUrl(strUrl));
}

void LoginDialog::linkHovered(QString strUrl)
{
    
}

void LoginDialog::checkAuto()
{
    if (ui.checkBox_autoLogin->checkState() == Qt::Checked)
    {
    	ui.checkBox_autoLogin->setCheckState(Qt::Checked);
    	ui.checkBox_keepPwd->setCheckState(Qt::Checked);
        m_login.SetAutoLogin(true);
    }
    else
    {
    	ui.checkBox_autoLogin->setCheckState(Qt::Unchecked);
		m_login.SetAutoLogin(false);
    }
}

void LoginDialog::checkKeeppwd()
{
    if (ui.checkBox_keepPwd->checkState() == Qt::Checked)
    {
    	ui.checkBox_keepPwd->setCheckState(Qt::Checked);
        m_login.SetRemPwd(true);
    }
    else
    {
    	ui.checkBox_autoLogin->setCheckState(Qt::Unchecked);
		ui.checkBox_keepPwd->setCheckState(Qt::Unchecked);
        m_login.SetRemPwd(false);
    }
}

void LoginDialog::doSelectUser(QString strUser)
{
	if (strUser.isEmpty())
	{
		return;
	}
    QString strPwd;
    bool bAuto = false;
    bool bRemPwd = false;

    if (m_login.SelectUser(strUser, strPwd, bAuto, bRemPwd))
    {
        ui.checkBox_autoLogin->setCheckState(bAuto ? Qt::Checked : Qt::Unchecked);
        ui.checkBox_keepPwd->setCheckState(bRemPwd ? Qt::Checked : Qt::Unchecked);
        if ((bAuto || bRemPwd) && !strPwd.isEmpty())
        {
            ui.lineEdit_pwd->setText(strPwd.left(g_nShowPwdLen));
        }
        else
        {
            ui.lineEdit_pwd->setText("");
        }
    }
    else
    {
        ui.checkBox_autoLogin->setCheckState(Qt::Unchecked);
        ui.checkBox_keepPwd->setCheckState(Qt::Unchecked);
        ui.lineEdit_pwd->setText("");
    }
    
    if (bRemPwd)
    {   
        m_login.SetRemPwd(true);
        if(bAuto)
        {
            m_login.SetAutoLogin(true);
        }
        else
        {
            m_login.SetAutoLogin(false);
        }
    }
    else
    {
        m_login.SetRemPwd(false);
        m_login.SetAutoLogin(false);
    }
}

void LoginDialog::showHistoryAccount()
{
    QString strArr[g_nRecordUserNum] = {0};
    int nSize = 0;
    m_login.InitHistoryUser(strArr, nSize);
    if (nSize > 0)
    {
        for (int i = 0; i < nSize; i++)
        {
            QString strUser(strArr[i]);
            QListWidgetItem *listItem = new QListWidgetItem(m_accountListWidget);
            AccountItem *item = new AccountItem(m_accountListWidget);
            listItem->setText(strUser);
            item->setItem(listItem);
            m_accountListWidget->setItemWidget(listItem, item);
        }
        QString strFirstUser = strArr[0];
        doSelectUser(strFirstUser);
    }
}

void LoginDialog::doChangeUser(const QString &strUser)
{
    ui.widget_logininfo->hide();
    ui.label_error->setText("");
	doSelectUser(strUser);
}

void LoginDialog::doChangePwd(const QString &strPwd)
{
    ui.widget_logininfo->hide();
    ui.label_error->setText("");
}

void LoginDialog::DoLoginOK(LoginInfo info)
{
    if (!m_login.ISHistoryAccount(ui.comboBox_name->currentText()))
    {
        int nSize = ui.comboBox_name->count();
        for (int i = nSize; i > g_nRecordUserNum; i--)
        {
            QListWidgetItem *item = m_accountListWidget->item(i);
            AccountItem *account = (AccountItem *)m_accountListWidget->itemWidget(item);
            m_accountListWidget->removeItemWidget(item);
            delete account;
            delete item;
            //ui.comboBox_name->removeItem(i);
        }
        QString strUser = ui.comboBox_name->currentText();
        QListWidgetItem *listItem = new QListWidgetItem(m_accountListWidget);
        AccountItem *item = new AccountItem(m_accountListWidget);
        listItem->setText(strUser);
        item->setItem(listItem);
        m_accountListWidget->setItemWidget(listItem, item);
    }
    m_login.loginOk();
    if (ClassSeeion::GetInst()->m_loginBytokenUid)
    {
        LobbyDialog::getInstance()->hide();
    }
    else
    {        
		ClassSeeion::GetInst()->_nUserId = info._nUserId;
        LobbyDialog::getInstance()->show();
        LobbyDialog::getInstance()->openClassWebPage(true);
        g_singApp->m_widget = LobbyDialog::getInstance();
    }
	ui.widget_logininfo->hide();
	
	this->hide();   
    setControlState(true);
}

void LoginDialog::DoLoginError(int nErrorCode)
{
    QString strError = "";
	m_login.loginError(nErrorCode, strError);
    //错误处理
    qDebug("Login Error: %s", strError);
    ui.widget_logininfo->show();
    ui.label_error->setText(strError);
    setControlState(true);
}

void LoginDialog::DoLogOut(__int64 nUserId)
{
	m_login.logout();
}

void LoginDialog::loginByTokenUid(QString acc, QString token, __int64 uid)
{
	m_login.BeginLoginToWeb(uid, token);
}

void LoginDialog::removeAccountItem(AccountItem* item)
{
	if (NULL == item)
	{
		return;
	}
    m_login.DeleteUser(item->getItem()->text());
    m_accountListWidget->removeItemWidget(item->getItem());
    delete item->getItem();

    ui.comboBox_name->hidePopup();
	
}

void LoginDialog::accountListItemChanged(AccountItem* item, int row)
{
	if (NULL == item | row < 0)
	{
		return;
	}
    ui.comboBox_name->setCurrentIndex(row);
	m_accountListWidget->setCurrentRow(row);
    ui.comboBox_name->hidePopup();
}

void LoginDialog::addItemToAccountListWidget()
{
    m_accountListWidget->setFixedHeight(200);
    ui.comboBox_name->setModel(m_accountListWidget->model());
    ui.comboBox_name->setView(m_accountListWidget);
}

void LoginDialog::setControlState(bool bISEnble)
{
    ui.comboBox_name->setEnabled(bISEnble);
    ui.lineEdit_pwd->setEnabled(bISEnble);
    ui.checkBox_autoLogin->setEnabled(bISEnble);
    ui.checkBox_keepPwd->setEnabled(bISEnble);
    ui.pushButton_logIn->setEnabled(bISEnble);
}

bool LoginDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui.comboBox_name)
    {
        if (QEvent::FocusIn == event->type())
        {
            ui.label_name->setEnabled(true);
        }
        else if (QEvent::FocusOut == event->type())
        {
            ui.label_name->setEnabled(false);
        }
    }
    if (watched == ui.lineEdit_pwd)
    {
        if (QEvent::FocusIn == event->type())
        {
            ui.label_pwd->setEnabled(true);
        }
        else if (QEvent::FocusOut == event->type())
        {
            ui.label_pwd->setEnabled(false);
        }
    }
    return false;
}

void LoginDialog::DoLogKickOut(__int64 nUserId)
{
    if (ClassRoomDialog::isValid())
    {
        ClassRoomDialog::getInstance()->closeWnd();

        if(ClassSeeion::GetInst()->IsTeacher())
        {   
            biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId,0, biz::eUserspeekstate_clean_speak,  ClassSeeion::GetInst()->_nUserId);
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

    this->show();

    C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("KickOutInfo")));
    msgBox.exec();

    ClassSeeion::GetInst()->_nUserId = 0;
}

void LoginDialog::onConnectServerError(ServerErrorInfo errInfo)
{
    if (errInfo._nServerState == biz::eServerState_connect ||
        errInfo._nServerState == biz::eServerState_ConClose ||
		errInfo._nServerState == biz::eServerState_AutoReconnect)
    {
        return;
    }

    if(this->isVisible())    
    {
        QString strError;
        m_login.loginError(AU_SERVERERROR, strError);
                
        ui.widget_logininfo->show();
        ui.label_error->setText(strError);
    }

    setControlState(true);
    this->show();
}