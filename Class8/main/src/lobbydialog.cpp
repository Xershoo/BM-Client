#include "lobbydialog.h"
#include <QtGlobal>
#include <QWebFrame>
#include <QDesktopServices>
#include <QWebSettings>
#include <QFileInfo>
#include "c8messagebox.h"
#include "common/Config.h"
#include "./session/classsession.h"
#include "./././biz/interface/interface.h"
#include "./././biz/interface/data.h"
#include "biz/interface/IUserInfoData.h"
#include "biz/interface/IClassRoomInfoContainer.h"
#include "./token/LoginTokenMgr.h"
#include "settingdialog.h"
#include "classroomdialog.h"
#include "BizInterface.h"
#include "common/Env.h"
#include "noticWindow/noticwindow.h"
#include "systemtray/systemtray.h"
#include "define/publicdefine.h"
#include "LoginByToken/qloginbttokendialog.h"
#include "MediaPublishMgr.h"
#include "logindialog.h"
#include "SingleApp/SingleApp.h"
#include "token/UploadTokenMgr.h"
#include "Courseware/Courseware.h"
#include <QFile>
#include "macros.h"
#include "HttpSessionMgr.h"
#include "Util.h"
#include "QClassWebDialog.h"
#include "lang.h"
#include "setdebugnew.h"

extern SingleApp *g_singApp;

//////////////////////////////////////////////////////////////////////////
LobbyDialog * LobbyDialog::m_instance = NULL;

LobbyDialog * LobbyDialog::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new LobbyDialog(NULL);
    }
    return m_instance;
}

void LobbyDialog::freeInstance()
{
    SAFE_DELETE(m_instance);
}

extern LoginDialog* g_loginDlg;

LobbyDialog::LobbyDialog(QWidget *parent)
    : C8CommonWindow(parent,false)
	, m_enterClassState(enterClass_undo)
	, m_isClickedHome(false)
	, m_isOpenWebCourseware(false)
    , m_webViewChat(NULL)
    , m_netAccessMger(NULL)
    , m_netDiskCache(NULL)
	, m_idDownHeadImage(0)
	, m_webWidget(NULL)
	, m_enterClassTimer(0)
	, m_dlgWait(NULL)
	, m_autoEnterClass(false)
	, m_nsysTime(-1)
	, m_nnotifyTimer(-1)
	, m_reloadTimer(-1)
//	, m_dlgCourse(NULL)
{   
    ui.setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

	/*
	change by xiewb 2018.06.27
	*/
	m_webWidget = new QClassWebWidget(NULL);
	m_webWidget->setGeometry(0,0,800,600);
	m_webWidget->hide();

	QMovie* loadingMovie = new QMovie(QString(":/res/res/image/default/web_loading.gif"));
	ui.label_loading->setMovie(loadingMovie);
	loadingMovie->start();

    connect(ui.gifIconpushButton_notify, SIGNAL(clicked()), this, SLOT(clickedNotify()));
    connect(ui.gifIconpushButton_showSettingDlg, SIGNAL(clicked()), this, SLOT(clickedSetting()));

	ui.webView_course->setContextMenuPolicy(Qt::NoContextMenu);
	connect(ui.webView_course->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addObjectToJs()));
    ui.webView_course->hide();
	ui.btn_showCourse->hide();
	ui.gifIconpushButton_notify->hide();

    biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);

    connect(ui.btn_showCourse,SIGNAL(clicked()),this,SLOT(clickedShowCourse()));
    connect(ui.label_username,SIGNAL(linkActivated(QString)),this,SLOT(clickedUserInfo(QString)));  
    
    connect(CLoginTokenMgr::GetInstance(), SIGNAL(recvLoginToken(QString)), this, SLOT(onRecvLoginToken(QString)));

    CBizCallBack * pCallback = ::getBizCallBack();
    if(pCallback)
    {
        connect(pCallback, SIGNAL(UserInfoReturn(__int64)), this, SLOT(onReturnUserInfo(__int64)));
        connect(pCallback, SIGNAL(EnterClassError(ClassErrorInfo)), this, SLOT(showClassRoom(ClassErrorInfo)));
        connect(pCallback, SIGNAL(RecvMsgList(MessageListInfo)), NoticWindow::getInstance(), SLOT(updateMsg(MessageListInfo)));
		connect(pCallback, SIGNAL(ConnectServerError(ServerErrorInfo )),this,SLOT(onConnectServerError(ServerErrorInfo)));
    }

    QString iconPath = Env::currentThemeResPath();
    ui.gifIconPushButton_close->setIconPath(iconPath + "gificon_close_normal.gif", iconPath + "gificon_close_hover.gif", iconPath + "gificon_close_pressed.gif");
    ui.gifIconpushButton_minSize->setIconPath(iconPath + "gificon_minSize_normal.gif", iconPath + "gificon_minSize_hover.gif", iconPath + "gificon_minSize_pressed.gif");
    ui.gifIconpushButton_showSettingDlg->setIconPath(iconPath + "gificon_setting_normal.gif", iconPath + "gificon_setting_hover.gif", iconPath + "gificon_setting_pressed.gif");
    ui.gifIconpushButton_notify->setIconPath(iconPath + "gificon_notification_normal.gif", iconPath + "gificon_notification_hover.gif", iconPath + "gificon_notification_pressed.gif");
    
	m_reloadTimer = startTimer(1800000);;
    
    memset(m_wszWebToken,NULL,sizeof(m_wszWebToken));

    CMediaPublishMgr::getInstance()->setUserId(ClassSeeion::GetInst()->_nUserId);

	initChatWebStyle();

    //Set cache
    m_netDiskCache = new QNetworkDiskCache(this);    
    QString location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);     
    m_netDiskCache->setCacheDirectory(location);    
    ui.webView_course->page()->networkAccessManager()->setCache(m_netDiskCache); 

    //http callback
    connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpDownImageCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);

	connect(m_webWidget,SIGNAL(enterClassroom(QString,QString)),this,SLOT(enterClass(QString,QString)));
	connect(m_webWidget,SIGNAL(playClassroom(QString,QString)),this,SLOT(playClass(QString,QString)));
	connect(m_webWidget,SIGNAL(webPageloadFinished()),this,SLOT(onWebPageLoadFinished()));
	connect(ui.webView_course,SIGNAL(loadFinished()),this,SLOT(onWebViewLoadFinished(bool)));
}

LobbyDialog::~LobbyDialog()
{
//	SAFE_DELETE(m_dlgCourse);
    SAFE_DELETE(m_webViewChat);
    SAFE_DELETE(m_netDiskCache);
    SAFE_DELETE(m_netAccessMger);
}

void LobbyDialog::initChatWebStyle()
{
	m_webViewChat = new QWebView(this);
	m_webViewChat->hide();
	QFile chatFile(":/chatstyle/res/chatstyle/test.html");
	if (chatFile.open(QIODevice::ReadOnly))
	{
		m_webViewChat->setHtml(QString::fromUtf8(chatFile.readAll().constData()));        
	}
	chatFile.close();
}

void LobbyDialog::setTitleBarRect()
{
    QPoint pt = ui.titleLabel->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void LobbyDialog::showMinimized()
{
    QDialog::showMinimized();
}

void LobbyDialog::close()
{
// 	if(m_dlgCourse)
// 	{
// 		m_dlgCourse->close();
// 	}

    this->killTimer(m_nsysTime);
    this->killTimer(m_nnotifyTimer);
    this->killTimer(m_reloadTimer);
    QDialog::close();

    CWebDlgMgr::getInstance()->closeAll();
}

QString LobbyDialog::getLangString()
{
    QString regKey(QObject::tr("product"));
    char    szKey[MAX_PATH] = {0};
    QString strLang;
    Util::QStringToChar(regKey,szKey,MAX_PATH);
    int langId = LangSet::loadLangSet(szKey);
    if(langId == LangSet::LANG_ENG)
    {
        strLang=QString("eng");
    }
    else
    {
        strLang=QString("chn");
    }

    return strLang;
}

void LobbyDialog::openClassWebPage(bool force/* =false */)
{
	if(m_isOpenWebCourseware&&!force)
    {
        return;
    }

    __int64 uid = ClassSeeion::GetInst()->_nUserId;

    if(NULL == m_wszWebToken[0])
    {
        if(!CLoginTokenMgr::GetInstance()->GetLoginToken(uid, m_wszWebToken))
        {
			if(m_webWidget)
			{
				m_webWidget->openUrl(QString("about:blank"));
				ui.label_loading->show();
				m_webWidget->hide();
			}

            return;
        }
    }
    
    QString strToken(QString::fromWCharArray(m_wszWebToken));
    QString strUrl = QString(Config::getConfig()->m_urlCourseSelf.c_str()).arg(uid).arg(strToken).arg(uid).arg(getLangString());
	
	if(m_webWidget)
	{
		m_webWidget->openUrl(strUrl);
	}

	/*
	QString strUrl = QString(Config::getConfig()->m_urlClassHome.c_str()).arg(uid).arg(strToken).arg(getLangString());

	QNetworkRequest webRequest;    
    webRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    webRequest.setUrl(QUrl(strUrl));
    ui.webView_course->load(webRequest);    
	*/

    m_isOpenWebCourseware = true;
	
    g_systemTray->setSysSettingActionEnable(true);
    g_systemTray->setLogoutActionEnable(true);
}

void LobbyDialog::clickedNotify()
{
    QSize size = NoticWindow::getInstance()->size();
    QPoint ptGlobal = ui.gifIconpushButton_notify->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y() + ui.gifIconpushButton_notify->size().height());
    ptGlobal.setX(ptGlobal.x() - size.width()/2 + + ui.gifIconpushButton_notify->size().width());
    NoticWindow::getInstance()->setGeometry(QRect(ptGlobal, size));
    NoticWindow::getInstance()->show();
    NoticWindow::getInstance()->activateWindow();
}

void LobbyDialog::clickedSetting()
{
	SettingDialog settingDlg;
    settingDlg.exec();
}

void LobbyDialog::clickedUserInfo(QString str)
{
    m_isClickedHome = true;
    WCHAR szToken[MAX_PATH] = {0};
    if (CLoginTokenMgr::GetInstance()->GetLoginToken(ClassSeeion::GetInst()->_nUserId, szToken))
    {
        showSelfHome(szToken);
    }
}

void LobbyDialog::clickedShowCourse()
{
// 	if(NULL == m_dlgCourse)
// 	{
// 		m_dlgCourse = new CourseDialog();
// 	}
// 
// 	m_dlgCourse->openCoursePage();
// 	m_dlgCourse->show();    
}

void LobbyDialog::onRecvLoginToken(QString token)
{
	/*
    if(m_isOpenWebCourseware)
    {
        showSelfHome(wstring((wchar_t*)(token).unicode()).data());
    }
    else
    {
        wcscpy_s(m_wszWebToken,(wchar_t*)(token).unicode());
        openClassWebPage();
    }
	*/

	openClassWebPage();
}

void LobbyDialog::showSelfHome(LPCWSTR wszToken)
{
    if (!m_isClickedHome)
    {
        return;
    }
    m_isClickedHome = false;
    if (NULL == wszToken || NULL == wszToken[0])
    {
        return;
    }

    QString strToken(QString::fromWCharArray(wszToken));
    QString strUrl = QString(Config::getConfig()->m_urlUserHome.c_str()).arg(ClassSeeion::GetInst()->_nUserId).arg(strToken).arg(getLangString());

    QDesktopServices::openUrl(QUrl(strUrl));
}

void LobbyDialog::addObjectToJs()
{
    ui.webView_course->page()->mainFrame()->addToJavaScriptWindowObject("CoursePage", this);
}

void LobbyDialog::onReturnUserInfo(__int64 nUserId)
{
    biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(nUserId);	
    updateUserInfo();
    
	if (wcslen(myInfo.szRealName) > 0)
    {
        ui.label_username->setText(QString::fromWCharArray(myInfo.szRealName));
    }
    else
    {
        ui.label_username->setText(QString::fromWCharArray(myInfo.szNickName));
    }

    CUploadTokenMgr::GetInstance()->SetUserId(nUserId);
    if (ClassSeeion::GetInst()->m_loginBytokenUid)
    {
        QString classid = QString("%1").arg(ClassSeeion::GetInst()->m_classID);
        QString courseid = QString("%1").arg(ClassSeeion::GetInst()->m_courseID);

		enterClass(courseid, classid);
    } else if(m_autoEnterClass){
		enterClass(ClassSeeion::GetInst()->_nCourseId,ClassSeeion::GetInst()->_nClassRoomId);
	}

    biz::GetBizInterface()->QueryMessageList(ClassSeeion::GetInst()->_nUserId, 1);
    m_nnotifyTimer = this->startTimer(60000);
}

void LobbyDialog::updateUserInfo()
{  
    QString imgFile;
    QPixmap headPixmap ;

    bool br = ClassSeeion::GetInst()->getHeadImage(imgFile);
    if(!br)
    {
        headPixmap = QPixmap(imgFile);
    }
    else
    {
        QImage headImage;
        QFile fileHead(imgFile);
        fileHead.open(QIODevice::ReadOnly);
        
        br = headImage.loadFromData(fileHead.readAll());
        headPixmap = QPixmap::fromImage(headImage);
    }
        
    QSize size = QSize(30,30);
    headPixmap = headPixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_userpic->clear();
    ui.label_userpic->setPixmap(headPixmap.scaled(size));
    
    //更新图片
    if(m_idDownHeadImage == 0)
    {
        biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);
        QString imgHeader = Env::GetUserPicCachePath(ClassSeeion::GetInst()->_nUserId)+"user_head.jpg";
        QString imgUrl = QString(Config::getConfig()->m_urlUserHeadImage.c_str()) + QString::fromWCharArray(myInfo.szPicUrl);
        QFile::remove(imgHeader);

        char szFile[1024] = { 0 };
        char szUrl[1024] = { 0 };

        Util::QStringToAnsi(imgHeader,szFile,sizeof(szFile)-1);
        Util::QStringToAnsi(imgUrl,szUrl,sizeof(szUrl)-1);
        m_idDownHeadImage = CHttpSessionMgr::GetInstance()->HttpDownloadFile(
            szUrl,szFile,
            HTTP_DOWN_TIMEOUT, 0, 
            (LPVOID)NULL, (LPVOID)this);
    }
}

void LobbyDialog::showClassRoom(ClassErrorInfo cinfo)
{
	QString errMsg;
	m_autoEnterClass = false;

	switch(cinfo._nErrorCode)
    {  
    case biz::ENTER_CLASSROOM_SUCCESS:
		{
			m_enterClassState = enterClass_done;

			if(m_dlgWait){
				m_dlgWait->cancel();
				m_dlgWait = NULL;
			}

			ClassRoomDialog::getInstance()->enterClassRoom(cinfo._nRoomId);
			g_singApp->m_widget = ClassRoomDialog::getInstance();

			this->hide();

			char szToken[MAX_PATH] = {0}; 
			GetUploadToken(szToken, ClassSeeion::GetInst()->_nUserId);
		}

        return;
    case biz::ENTER_CLASSROOM_ERROR_NOT_INTIME:
        {
			errMsg = QString(tr("enterClassNotInTime"));
        }
        break;
    case biz::ENTER_CLASSROOM_ERROR_FINISHED:
        {
			errMsg = QString(tr("enterClassFinish"));            
        }
        break;
    case biz::ENTER_CLASSROOM_ERROR_AUTHORITY:
        {
			errMsg = QString(tr("enterClassNotAuth"));
        }
        break;
    case biz::ENTER_CLASSROOM_ERROR_BLACKLIST:
        {
			errMsg = QString(tr("enterClassInBlacklist"));
        }
        break;
    case biz::ENTER_CLASSROOM_ERROR_LOCK:
        {
			errMsg = QString(tr("enterClassLocked"));
        }
        break;
    default:
        {
           
        }		
        break;
    }

	m_enterClassState = enterClass_undo;

	if(m_dlgWait){

		m_dlgWait->endWait(errMsg);
		m_dlgWait = NULL;

	}else{

		C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), errMsg);
		msgBox.exec();
	}
    return;
}

void LobbyDialog::setCloseClassRoom()
{
    m_enterClassState = enterClass_undo;
    this->show();
    g_singApp->m_widget = this;

    if (!m_isOpenWebCourseware) 
    {	
		memset(m_wszWebToken,NULL,sizeof(m_wszWebToken));
        openClassWebPage(true);
    }
}

void LobbyDialog::showSysTimer()
{
    QTime current_time = QTime::currentTime();
    int hour = current_time.hour();
    int minute = current_time.minute();
    int second = current_time.second();
    int msec = current_time.msec();

    char szTime[20] = {0};
    sprintf(szTime, "%d: %02d: %02d", hour, minute, second);

    QString strTime = QString("%1").arg(szTime);
    ui.label_username->setText(strTime);
}

void LobbyDialog::flushNotifyMsgTimer()
{
    biz::GetBizInterface()->QueryMessageList(ClassSeeion::GetInst()->_nUserId, 1);
    
    QString iconPath = Env::currentThemeResPath();
    if (NoticWindow::getInstance()->GetCount() > 0)
    {
        ui.gifIconpushButton_notify->setIconPath(iconPath + "gificon_notification_normal.gif", iconPath + "gificon_notification_hover.gif", iconPath + "gificon_notification_pressed.gif");
    }
    else
    {
        ui.gifIconpushButton_notify->setIconPath(iconPath + "gificon_notification_normal.gif", iconPath + "gificon_notification_hover.gif", iconPath + "gificon_notification_pressed.gif");
    }
}

void LobbyDialog::timerEvent(QTimerEvent *event)
{
    if (NULL == event)
    {
        return;
    }

    if (event->timerId() == m_nsysTime)
    {
        showSysTimer();
    }

    if (event->timerId() == m_nnotifyTimer)
    {
        flushNotifyMsgTimer();
    }

    if(event->timerId() == m_reloadTimer)
    {
        m_isOpenWebCourseware = false;
        memset(m_wszWebToken,NULL,sizeof(m_wszWebToken));
        openClassWebPage();
    }

	if(event->timerId() == m_enterClassTimer)
	{
		this->killTimer(m_enterClassTimer);
		m_enterClassTimer = 0;

		if(m_enterClassState == enterClass_doing) {
			m_enterClassState = enterClass_undo;

			if(m_autoEnterClass) {
				
			}else if(biz::GetBizInterface()->relogin()) {
				m_autoEnterClass = true;
				return;
			}else{
				
			}
			
			m_autoEnterClass = false;

			if(NULL!=m_dlgWait){
				m_dlgWait->endWait(QString(tr("enterClassFailed")));;
				m_dlgWait = NULL;
			}
			
		}
	}
}

void LobbyDialog::onConnectServerError(ServerErrorInfo info)
{
	if (info._nServerState == biz::eServerState_ConClose)
	{
		m_enterClassState = enterClass_undo;
		return;
	}
	
	if(!this->isVisible())
	{
		return;
	}

	switch(info._nServerState)
	{
	case biz::eServerState_connect:
		{
			if(NULL==m_dlgWait || m_autoEnterClass){
				break;
			}

			m_dlgWait->cancel();
			m_dlgWait = NULL;
		}
		break;
	case biz::eServerState_AutoReconnect:
		{
			if(NULL!=m_dlgWait){
				break;
			}
			
			m_enterClassState = enterClass_undo;
			m_dlgWait = new QWaitDlg(this);
			m_dlgWait->wait(this,SIGNAL(setWaitStop()),QObject::tr("msgAutoReconnect"),QString(tr("DisconnectServer")));

			m_dlgWait = NULL;
		}
		break;
	case biz::eServerState_ConDis:
	case biz::eServerState_ConFailed:
		{
			if (NULL!=m_dlgWait) {
				
				emit setWaitStop();
				m_dlgWait = NULL;

			} else {
				
				C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("DisconnectServer")));
				msgBox.exec();
			}

			this->hide();
			m_enterClassState = enterClass_undo;
		}
		break;
	}
}

void LobbyDialog::enterClass(QString courseID, QString classId)
{	
	if (m_enterClassState != LobbyDialog::enterClass_undo)
	{
		return;
	}

	__int64 nClassID = classId.toInt();
	__int64 nCourseID = courseID.toInt();

	enterClass(nCourseID,nClassID);
}

void LobbyDialog::enterClass(__int64 courseId,__int64 classId)
{
	if (m_enterClassState != LobbyDialog::enterClass_undo)
	{
		return;
	}

	//进入课堂
	ClassSeeion::GetInst()->_nClassRoomId = (long long)classId;
	ClassSeeion::GetInst()->_nCourseId = (long long)courseId;

	biz::GetBizInterface()->EnterClass(ClassSeeion::GetInst()->_nUserId, courseId,classId,getDevState()/*有无摄像头和麦克风*/);
	m_enterClassState = LobbyDialog::enterClass_doing;

	//close LoginByWeb dialog
	QLoginBtTokenDialog::getInstance()->hide();

	//close popup web dialog
	CWebDlgMgr::getInstance()->closeAll();

	m_enterClassTimer = this->startTimer(10000,Qt::CoarseTimer);

	if(NULL==m_dlgWait){
		m_dlgWait = new QWaitDlg(this,false);
		m_dlgWait->wait(this,SIGNAL(setWaitStop()),QString(tr("WatiEnterClass")),QString(""));
	}
}

void LobbyDialog::playClass(QString courseID, QString classId)
{
	__int64 nClassID = classId.toInt();
	__int64 nCourseID = courseID.toInt();
}

int LobbyDialog::getDevState()
{
	int devState = 0;
	if(!CMediaPublishMgr::getInstance()->isInitMedia())
	{
		return 0;
	}

	MDevStatus mState = CMediaPublishMgr::getInstance()->getCamDevStatus();
	if(mState != NODEV)
	{
		devState |= biz::eUserState_user_video;
	}

	mState = CMediaPublishMgr::getInstance()->getMicDevStatus();
	if(mState != NODEV)
	{
		devState |= biz::eUserState_user_voice;
	}

	return devState;
}

void LobbyDialog::hide()
{
// 	if(m_dlgCourse)
// 	{
// 		m_dlgCourse->close();
// 	}

	C8CommonWindow::hide();
}

void LobbyDialog::HttpDownImageCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param)
{
    LobbyDialog *pThis = (LobbyDialog*)lpUser;
    LPHTTPSESSION pHttpS = (LPHTTPSESSION)Param;
    if (NULL == pThis || NULL == pHttpS || pHttpS->id != m_idDownHeadImage)
    {
        return;
    }

    switch (httpEventType)
    {
    case HTTP_EVENT_BEGIN:
        {

        }
        break;

    case HTTP_EVENT_PROGRESS:
        {

        }
        break;

    case HTTP_EVENT_END:
        {   
            updateUserInfo();
            m_idDownHeadImage = 0;

            if(ClassRoomDialog::isValid())
            {
                ClassRoomDialog::getInstance()->updateUserHead();
            }
        }
        break;
    }
}

void LobbyDialog::onWebPageLoadFinished()
{	
	if(NULL==m_webWidget)
	{
		return;
	}
	
	QBoxLayout *showLayout = qobject_cast<QBoxLayout*>(ui.widget_show->layout());
	if(showLayout)
	{
		showLayout->addWidget(m_webWidget);
	}
	
	ui.label_loading->hide();
	m_webWidget->show();

	m_isOpenWebCourseware = false;
	memset(m_wszWebToken,NULL,sizeof(m_wszWebToken));
}

void LobbyDialog::onWebViewLoadFinished(bool error)
{

}

void LobbyDialog::autoEnterClass()
{
	m_autoEnterClass = true;
	m_enterClassState = enterClass_undo;

	if(NULL==m_dlgWait){
		m_dlgWait = new QWaitDlg(this,false);
		m_dlgWait->wait(this,SIGNAL(setWaitStop()),QString(tr("WatiEnterClass")),QString(""));
	}
}