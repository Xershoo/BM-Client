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
#include "CourseClassItem.h"
#include "jason/include/json.h"
#include "DeviceDetectDialog.h"
#include "./common/Util.h"

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
    : C8CommonWindow(parent,SHADOW_AERO)
	, m_enterClassState(enterClass_undo)
	, m_isClickedHome(false)
	, m_idDownHeadImage(0)
	, m_enterClassTimer(0)
	, m_dlgWait(NULL)
	, m_autoEnterClass(false)
	, m_sysTimer(-1)
	, m_notifyTimer(-1)
	, m_getUserInfoTimer(-1)
	, m_idGetClassList(0)
	, m_loadingMovie(NULL)
{   
	ui.setupUi(this);

	m_loadingMovie = new QMovie(QString(":/res/res/image/default/web_loading.gif"));
	ui.label_loading->setMovie(m_loadingMovie);
	m_loadingMovie->start();

    connect(ui.gifIconpushButton_notify, SIGNAL(clicked()), this, SLOT(clickedNotify()));
    connect(ui.gifIconpushButton_showSettingDlg, SIGNAL(clicked()), this, SLOT(clickedSetting()));

	//hide ui element xiewenbing 2019.10.09
	ui.btn_showCourse->hide();
	ui.widget_course->hide();
	ui.widget_courseListItems->hide();
	ui.label_noCourseTip->hide();
	
	ui.gifIconpushButton_notify->hide();

    biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);

    connect(ui.btn_showCourse,SIGNAL(clicked()),this,SLOT(clickedShowCourse()));
    connect(ui.label_username,SIGNAL(linkActivated(QString)),this,SLOT(clickedUserInfo(QString)));  
    connect(ui.widget_calendar,SIGNAL(clickDay(QString)),this,SLOT(showClassList(QString)));
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
    
    memset(m_wszWebToken,NULL,sizeof(m_wszWebToken));

    CMediaPublishMgr::getInstance()->setUserId(ClassSeeion::GetInst()->_nUserId);

	//http callback
    connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,SLOT(HttpDownImageCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);

	centerWindow();

	if(NULL!=g_systemTray){
		g_systemTray->setSysSettingActionEnable(true);
		g_systemTray->setLogoutActionEnable(true);
	}

	m_getUserInfoTimer = startTimer(3000);
}

LobbyDialog::~LobbyDialog()
{
 	SAFE_DELETE(m_loadingMovie);
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
    this->killTimer(m_sysTimer);
    this->killTimer(m_notifyTimer);
	this->killTimer(m_getUserInfoTimer);
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
	/*
	SettingDialog settingDlg;
    settingDlg.exec();
	*/

	DeviceDetectDialog detectDlg(ClassSeeion::GetInst()->_nUserId);
	detectDlg.exec();
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
}

void LobbyDialog::onRecvLoginToken(QString token)
{
	
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

void LobbyDialog::onReturnUserInfo(__int64 nUserId)
{
	biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(nUserId);	
    updateUserHead();

	if (wcslen(myInfo.szRealName) > 0)
    {
        ui.label_username->setText(QString::fromWCharArray(myInfo.szRealName));
    }
    else
    {
        ui.label_username->setText(QString::fromWCharArray(myInfo.szNickName));
    }
	
	ui.label_userId->setText(QString("%1").arg(myInfo.nUserId));

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
    m_notifyTimer = this->startTimer(60000);

	if(m_getUserInfoTimer>=0){
		this->killTimer(m_getUserInfoTimer);
		m_getUserInfoTimer = -1;
	}

	showClassList(QString(""));
}

void LobbyDialog::updateUserHead()
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
    
    headPixmap = headPixmap.scaled(ui.label_userpic->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui.label_userpic->clear();
    ui.label_userpic->setPixmap(headPixmap);
    
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

    if (event->timerId() == m_sysTimer)
    {
        showSysTimer();
		return;
    }

    if (event->timerId() == m_notifyTimer)
    {
        flushNotifyMsgTimer();
		return;
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

		return;
	}

	if (event->timerId() == m_getUserInfoTimer)
	{
		biz::GetBizInterface()->QueryUserInfo(ClassSeeion::GetInst()->_nUserId);
		return;
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

	__int64 nClassID = classId.toLongLong();
	__int64 nCourseID = courseID.toLongLong();

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
	m_enterClassState = enterClass_undo;
	C8CommonWindow::hide();
}

void LobbyDialog::HttpDownImageCallBack(int httpEventType, unsigned int lpUser, bool bIsFirst, unsigned int Param)
{
    LobbyDialog *pThis = (LobbyDialog*)lpUser;
    LPHTTPSESSION pHttpS = (LPHTTPSESSION)Param;
    if (NULL == pThis || NULL == pHttpS || (pHttpS->id != m_idDownHeadImage && pHttpS->id != m_idGetClassList))
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
			if(pHttpS->id==m_idDownHeadImage)
            {
				updateUserHead();
				m_idDownHeadImage = 0;

				if(ClassRoomDialog::isValid())
				{
					ClassRoomDialog::getInstance()->updateUserHead();
				}
				break;
			}

			if(pHttpS->id==m_idGetClassList)
			{
				string strList = pHttpS->response;
				showCourseClass(strList);
			}
        }
        break;
    }
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

void LobbyDialog::showClassList(QString qstrDate)
{
	QString qstrTime = qstrDate;
	if(qstrTime.isNull()||qstrTime.isEmpty()){
		QDateTime current_time = QDateTime::currentDateTime(); 
		qstrTime = current_time.toString("yyyyMMdd");
	}

	QString qstrRequest=QString("userId=%1&date=%2-%3-%4").arg(ClassSeeion::GetInst()->_nUserId).arg(qstrTime.left(4)).arg(qstrTime.mid(4,2)).arg(qstrTime.right(2));
	//get class list from server
	char szUrl[MAX_PATH]={0};
	char szRequest[MAX_PATH]={0};
	Util::QStringToChar(qstrRequest,szRequest,MAX_PATH);
	strcpy_s(szUrl,Config::getConfig()->m_urlCourseClassList.c_str());

	//reconnect signal and slot
	if(!CHttpSessionMgr::isValid()){
		connect(CHttpSessionMgr::GetInstance(),SIGNAL(httpEvent(int, unsigned int, bool, unsigned int)),this,
			SLOT(HttpDownImageCallBack(int, unsigned int, bool, unsigned int)),Qt::QueuedConnection);
	}

	m_idGetClassList = CHttpSessionMgr::GetInstance()->DoHttpRequest(
		HTTP_SESSION_POST,
		szUrl,szRequest,
		HTTP_DOWN_TIMEOUT, 
		(LPVOID)NULL, (LPVOID)this);

	
}

void LobbyDialog::showCourseClass(string jsonList)
{
	bool showList = false;
	do 
	{
		if(jsonList.empty()){
			break;
		}

		Json::Reader jReader; 
		Json::Value jValue; 

		jReader.parse(jsonList,jValue);

		int itemCount =jValue["count"].asInt();
		if(itemCount<=0){
			break;
		}

		bool isArray = jValue["list"].isArray();
		int  size = jValue["list"].size();
		if(!isArray || size <= 0 || itemCount != size){
			break;
		}

		size = size<6?size:6;
		ui.widget_courseListItems->show();
		CourseClassItem* classItem[] = {ui.widgetItem_1,ui.widgetItem_2,ui.widgetItem_3,ui.widgetItem_4,ui.widgetItem_5,ui.widgetItem_6};
		for(int i=0; i<6; i++)
		{
			if(i>=size){
				classItem[i]->hide();
				continue;
			}

			Json::Value itemValue = jValue["list"][i];
			std::string itemContent = itemValue.toStyledString();

			Json::Reader jItemReader; 
			Json::Value jItemValue;

			jItemReader.parse(itemContent,jItemValue);

			string startTime = jItemValue["startTime"].asString();
			string endTime = jItemValue["endTime"].asString();
			string courseId = jItemValue["courseid"].asString();
			string classId = jItemValue["classid"].asString();
			string className = jItemValue["className"].asString();
			string teacherName = jItemValue["teacherName"].asString();
			string imageUrl = jItemValue["url"].asString();
			string classState = jItemValue["classState"].asString();
			string isTeacher = jItemValue["isTeacher"].asString();

			setCourseClassItem(classItem[i],courseId,classId,classState,imageUrl,className,isTeacher,teacherName,startTime,endTime);
		}

		showList = true;
	} while (false);

	if(!showList){
		ui.widget_courseListItems->hide();
		ui.label_noCourseTip->show();
	}else{
		ui.label_noCourseTip->hide();
	}

	if(!ui.widget_course->isVisible()){
		ui.widget_course->show();
		ui.label_loading->hide();
	}
}

void LobbyDialog::setCourseClassItem(CourseClassItem* ccItem,string courseId,string classId, string classState,string imageUrl,string className,string isTeacher, string teacherName,string startTime,string endTime)
{
	if(NULL==ccItem){
		return;
	}
	
	QString qstrTemp;
	Util::AnsiToQString(courseId.c_str(),courseId.length(),qstrTemp);
	__int64 _courseId = qstrTemp.toLongLong();
	
	Util::AnsiToQString(classId.c_str(),courseId.length(),qstrTemp);
	__int64 _classId = qstrTemp.toLongLong();

	int _classState = atoi(classState.c_str());
	if (_classState == 15){
		_classState = CourseClassItem::class_wait;
	}else if (_classState == 16){
		_classState = CourseClassItem::class_doing;
	}else if (_classState == 17 || _classState == 20){
		_classState = CourseClassItem::class_end;
	}else {
		_classState = CourseClassItem::class_unknow;
	}

	bool _isTeacher = (bool)atoi(isTeacher.c_str());
	
	QString _startTime ;
	Util::AnsiToQString(startTime.c_str(),startTime.length(),_startTime);
	_startTime = _startTime.mid(_startTime.lastIndexOf(' ')+1,5);

	QString _endTime;
	Util::AnsiToQString(endTime.c_str(),endTime.length(),_endTime);
	_endTime = _endTime.mid(_endTime.lastIndexOf(' ')+1,5);
	
	QString _teacherName;
	QString _className;
	QString _imageUrl;

	Util::Utf8ToQString(teacherName.c_str(),teacherName.length(),_teacherName);
	Util::Utf8ToQString(className.c_str(),className.length(),_className);
	Util::Utf8ToQString(imageUrl.c_str(),imageUrl.length(),_imageUrl);

	if(_imageUrl.indexOf("://")<0){
		Util::AnsiToQString(Config::getConfig()->m_urlWebSite.c_str(),Config::getConfig()->m_urlWebSite.length(),qstrTemp);
		_imageUrl = qstrTemp+_imageUrl;
	}

	ccItem->setItemParam(_courseId,_classId,_classState,_imageUrl,_isTeacher,_className,_teacherName,_startTime,_endTime);
	ccItem->show();
}