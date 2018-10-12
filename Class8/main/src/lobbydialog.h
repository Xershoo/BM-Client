#ifndef LOBBYDIALOG_H
#define LOBBYDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_lobbydialog.h"
#include "BizCallBack.h"
#include <QTimer>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "CourseDialog.h"

#include <QWebPage>  
#include <QNetworkRequest> 

#include "ClassWebWidget.h"
#include "WaitDlg.h"

class LobbyDialog : public C8CommonWindow
{
    Q_OBJECT

public:
	enum
	{
		enterClass_undo = 0,
		enterClass_doing,
		enterClass_done
	};

public:
    static LobbyDialog* getInstance();
    static void freeInstance();

    ~LobbyDialog();
    void openClassWebPage(bool force=false);
    void setCloseClassRoom();

	void hide();

	void autoEnterClass();
private:
    LobbyDialog(QWidget *parent = 0);

public slots:
	void enterClass(QString courseID, QString classId);
	void playClass(QString courseID, QString classId);

	void enterClass(__int64 courseId,__int64 classId);

	void showClassList(QString qstrDate);
public slots:
    void onRecvLoginToken(QString pwszToken);
    void onReturnUserInfo(__int64);
    void showClassRoom(ClassErrorInfo);
    void close();
	void onConnectServerError(ServerErrorInfo);

	void onWebPageLoadFinished();
	void onWebViewLoadFinished(bool);
protected:
    virtual void setTitleBarRect() override;
    void showSelfHome(LPCWSTR wszToken);

    void updateUserInfo();
    void timerEvent(QTimerEvent *event);

    void showSysTimer();
    void flushNotifyMsgTimer();

	void initChatWebStyle();

	int getDevState();
    
    QString getLangString();

	void openLocalPage();

	void showCourseClass(string jsonList);

	void setCourseClassItem(CourseClassItem* ccItem,string courseId,string classId,
		string classState,string imageUrl,string className,string isTeacher,
		string teacherName,string startTime,string endTime);
protected slots:
    void showMinimized();
    void clickedNotify();
    void clickedSetting();
    void clickedUserInfo(QString);
    void clickedShowCourse();
    void addObjectToJs();

    void HttpDownImageCallBack(int, unsigned int, bool, unsigned int);
private:
    Ui::LobbyDialog ui;
    
    int     m_enterClassState;
	int		m_enterClassTimer;

    bool    m_isClickedHome;
    bool    m_isOpenWebCourseware;
	bool	m_autoEnterClass;
    WCHAR   m_wszWebToken[MAX_PATH];

signals:
	void	setWaitStop();

protected:
    int     m_nsysTime;
    int     m_nnotifyTimer;
    int     m_reloadTimer;
    int     m_idDownHeadImage;
	int		m_idGetClassList;
    
    QWebView * m_webViewChat;

    QNetworkDiskCache *m_netDiskCache ;
    QNetworkAccessManager * m_netAccessMger;

	QClassWebWidget*	m_webWidget;

	QWaitDlg*			m_dlgWait;

//	CourseDialog*		m_dlgCourse;
private:
    static LobbyDialog*  m_instance;
};

#endif // LOBBYDIALOG_H
