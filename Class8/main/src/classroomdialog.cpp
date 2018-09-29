#include "classroomdialog.h"
#include "c8messagebox.h"
#include "settingdialog.h"
#include "classroomsetting.h"
#include "userlisttabpage.h"
#include "lobbydialog.h"
#include "Courseware/courseware.h"
#include "session/classsession.h"
#include <QBitmap>
#include <time.h>
#include <QMutexLocker>
#include <QSpacerItem>
#include <QShortcut>
#include <QDesktopWidget>
#include <QWindowStateChangeEvent>
#include "common/Env.h"
#include "media/MediaPublishMgr.h"
#include "BizInterface.h"
#include "Util.h"
#include "common/Share_Ptr.h"
#include "common/Config.h"
#include "common/HttpSessionMgr.h"
#include "webcamerasettingpage.h"
#include "qclassremindwindow.h"
#include "classoverevaluate.h"
#include "recordSettingdialog.h"
#include "systemtray/systemtray.h"
#include "define/publicdefine.h"
#include "src/chat/chat.h"
#include "src/chat/privatechatwidget.h"
#include "userinfo/userinfomanager.h"
#include "../3rdparty/qxtglobalshortcut/qxtglobal.h"
#include "../3rdparty/qxtglobalshortcut/qxtglobalshortcut.h"
#include "logindialog.h"
#include "control/pictureviewer.h"
#include "Courseware/SkyCursewaveData.h"
#include "whiteboard/WhileBoardDataMgr.h"
#include "Courseware/CoursewarePanel.h"
#include "Courseware/CoursewareTaskMgr.h"

ClassRoomDialog* ClassRoomDialog::m_instance =  NULL;
extern LoginDialog* g_loginDlg;

#define TEACHER_MULTI_VIDEO  100
#define ASSERT_CLASSROOM_DLG_EXSIT		if(!ClassRoomDialog::isValid()) { return ;}

const QEvent::Type CLASSROOM_DLG_BEGIN_RECORD_EVENT = (QEvent::Type)(QEvent::User + 101);
const QEvent::Type CLASSROOM_DLG_STOP_RECORD_EVENT = (QEvent::Type)(QEvent::User + 102);

ClassRoomDialog * ClassRoomDialog::getInstance()
{
    if(NULL == m_instance)
    {
        m_instance = new ClassRoomDialog;
    }
    return m_instance;
}

void ClassRoomDialog::freeInstance()
{
    if(NULL == m_instance)
    {
        return;
    }

    delete m_instance;
    m_instance = NULL;
}

bool ClassRoomDialog::isValid()
{
    return NULL == m_instance ? false : true;
}

ClassRoomDialog::ClassRoomDialog(QWidget *parent)
    : C8CommonWindow(parent)
	, m_dlgWait(NULL)
	, m_popupDlg(NULL)
	, m_rtmpSpeakPlayer(NULL)
	, m_nClassProcessSec(0)
	, m_nClassOverCountDown(0)
	, m_bISCoundDown(false)
	, m_typeMainShow(biz::eMainShow_CWWB)
	, m_nLatencyTime (0)
	, m_nStartTimeSec(0)
	, m_bISAutoClassOver(false)
	, m_nBenginTimer(0)
	, m_nClassTimer(0)
	, m_nCountDownTimer(0)
	, m_nFlushSysMsgTimer(0)
	, m_initEnterClassRoom(false)
	, m_bIsSetUI(false)
	, m_isShowNormal(true)
{
    ui.setupUi(this);
	initUI();
	initNetMsgNotify();
	initUiMsgNotify();
	initHotkey();
	this->installEventFilter(this);
    
	CoursewareDataMgr::GetInstance();
    CSkyCursewaveData::getInstance();
    CHttpSessionMgr::GetInstance();
    WhiteBoardDataMgr::getInstance();
    CoursewareTaskMgr::getInstance();

	CMediaPublishMgr* pMediaMgr = CMediaPublishMgr::getInstance();
	if(pMediaMgr)
	{
		connect(pMediaMgr,SIGNAL(initFinished()),this,SLOT(onMediaInitFinish()));
	}

	this->windowShowMaxmized();
}

ClassRoomDialog::~ClassRoomDialog()
{
	
}

void ClassRoomDialog::initNetMsgNotify()
{
	CBizCallBack* bizCallback = getBizCallBack();
	if(NULL==bizCallback){
		return;
	}

	connect(bizCallback, SIGNAL(UserEnter(__int64)), this, SLOT(onUserEnter(__int64)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(UpdateUserInfo(__int64)), this, SLOT(onUpdateUserInfo(__int64)));
	connect(bizCallback, SIGNAL(RecvStudentVideoList(StudentVideoListInfo)), this, SLOT(onRecvStuVideoList(StudentVideoListInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(ClassStateChanged(ClassStateChangeInfo)), this, SLOT(onClassStateChange(ClassStateChangeInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(UserLeave(UserLeaveInfo)), this, SLOT(onUserLeave(UserLeaveInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(UserSpeakAction(UserSpeakActionInfo)), this, SLOT(onUserSpeakAction(UserSpeakActionInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(RecvClassMsgReminder(ClassMsgReminderInfo)), this, SLOT(onRecvClassMsgReminder(ClassMsgReminderInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(SetClassMode(SetClassModeInfo)), this, SLOT(onSetClassMode(SetClassModeInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(MainShowChanged(ClassMainShowInfo)),this,SLOT(onMainShowChanged(ClassMainShowInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(ConnectServerError(ServerErrorInfo )),this,SLOT(onConnectServerError(ServerErrorInfo)),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(RecvSetAssistantMsg(SetAssistantInfo )),this,SLOT(onRecvSetAssistantMsg(SetAssistantInfo )),Qt::AutoConnection);
	connect(bizCallback, SIGNAL(TeacherSwitchVideo(TeacherSwitchVideoInfo )),this,SLOT(onSwitchTeacherVideo(TeacherSwitchVideoInfo )),Qt::AutoConnection);
}

void ClassRoomDialog::unitNetMsgNotify()
{
	CBizCallBack* bizCallback = getBizCallBack();
	if(NULL==bizCallback){
		return;
	}

	disconnect(bizCallback,NULL,this,NULL);

}

void ClassRoomDialog::initUI()
{
	//left slide bar
	QString pixPath = Env::currentThemeResPath() + "widget_leftSildBar_mask.png";
	QPixmap pix(pixPath);
	ui.widget_leftSildBar->setRegion();
	ui.widget_leftSildBar->showUI(false);

	pixPath = Env::currentThemeResPath() + "widget_classroomToolBtns_mask.png";
	QPixmap pixBtnToolsMask(pixPath);
	ui.widget_classroomToolBtns->setMask(pixBtnToolsMask.mask());

	//Gif button set
	QString iconPath = Env::currentThemeResPath();
	ui.gifIconPushButton_close->setIconPath(iconPath + "gificon_close_normal.gif", iconPath + "gificon_close_hover.gif", iconPath + "gificon_close_pressed.gif");
	ui.gifIconpushButton_minSize->setIconPath(iconPath + "gificon_minSize_normal.gif", iconPath + "gificon_minSize_hover.gif", iconPath + "gificon_minSize_pressed.gif");
	ui.gifIconpushButton_max_normalSize->setIconPath(iconPath + "gificon_maxSize_normal.gif", iconPath + "gificon_maxSize_hover.gif", iconPath + "gificon_maxSize_pressed.gif");
	ui.gifIconpushButton_showSettingDlg->setIconPath(iconPath + "gificon_setting_normal.gif", iconPath + "gificon_setting_hover.gif", iconPath + "gificon_setting_pressed.gif");
	ui.gifIconPushButton_lock->setIconPath(iconPath + "lock_normal.gif", iconPath + "lock_hover.gif", iconPath + "lock_pressed.gif");
	ui.gifIconPushButton_unlock->setIconPath(iconPath + "unlock_normal.gif", iconPath + "unlock_hover.gif", iconPath + "unlock_pressed.gif");

	//widget list tool bar set
	ui.widget_listTools->setStyleSheet ("venus--TitleBar {background-color: rgb(0,0,0);color: rgb(255,255,255);}");
	ui.widget_listTools->setWindowOpacity(0.6);

	//chat
	Chat *classChat = m_chatManager.createChatObj();
	classChat->setChatData(ChatData(ClassSeeion::GetInst()->_nUserId,0,ClassSeeion::GetInst()->_nClassRoomId,biz::eTestMsgType_class,0));
	classChat->setWidget(ui.tab_chatWiget,CLASS_CHAT);

	//set system menu
	g_systemTray->setSysSettingActionEnable(true);
	g_systemTray->setLogoutActionEnable(true);

	//shortcut key
	m_show_hideMainWnd = new QxtGlobalShortcut(this);
	m_curScreenShortcut = new QShortcut(this);
	m_open_closeSound = new QShortcut(this);
	m_open_closeMIc = new QShortcut(this);
	m_incressSound = new QShortcut(this);
	m_decressSound = new QShortcut(this);
	m_handsUpDown = new QShortcut(this);

	//set hide interface child control
	ui.pushButton_webCameraSetting->hide();
	ui.pushButton_mulitCamera->hide();
	ui.widget_stuVideoListWnd->hide();
	ui.widget_teaVideo->hide();
	ui.pushButton_allowHandsupBtn->hide();
	ui.pushButton_allowSendMsgBtn->hide();
	ui.pushButton_showCoursewareDownlistWnd->hide();
	ui.gifIconPushButton_lock->hide();
	ui.widget_mediatool->hide();
	ui.widget_teaVideoToolBar_bk->hide();  //2018.09.28
}

void ClassRoomDialog::initUiMsgNotify()
{
	//bottom bar button
	connect(ui.pushButton_stuHandsUp, SIGNAL(clicked()), this, SLOT(handUpStuBtnClicked()));
	connect(ui.pushButton_stuHandsDown, SIGNAL(clicked()), this, SLOT(handDownStuBtnClicked()));
	connect(ui.pushButton_showAddCoursewareWindow, SIGNAL(clicked()), this, SLOT(showCoursewareWindow()));
	connect(ui.pushButton_classOverBtn, SIGNAL(clicked()), this, SLOT(classOverBtnClicked()));
	connect(ui.pushButton_classBeginBtn,SIGNAL(clicked()), this, SLOT(classBeginBtnClicked()));
	connect(ui.pushButton_recordClassBtn, SIGNAL(clicked()), this, SLOT(recordBtnClicked()));
	connect(ui.pushButton_stopRecordClassBtn, SIGNAL(clicked()), this, SLOT(stopRecordBtnClicked()));
	connect(ui.pushButton_disableHandsupBtn, SIGNAL(clicked()), this, SLOT(disableHandsUpBtnClicked()));
	connect(ui.pushButton_allowHandsupBtn,SIGNAL(clicked()), this, SLOT(enableHandsUpBtnClicked()));
	connect(ui.pushButton_disableSendMsgBtn, SIGNAL(clicked()), this, SLOT(disableSendMsgBtnClicked()));
	connect(ui.pushButton_allowSendMsgBtn,SIGNAL(clicked()), this, SLOT(enableSendMsgBtnClicked()));
	connect(ui.pushButton_showCoursewareDownlistWnd,SIGNAL(clicked()), this, SLOT(showCoursewareDownloadWindow()));
	connect(ui.pushButton_webCameraSetting,SIGNAL(clicked()), this, SLOT(webCameraSettingBtnClicked()));
	connect(ui.pushButton_mulitCamera,SIGNAL(clicked()), this, SLOT(showTeaMulitVideoBtnClicked()));
	connect(ui.pushButton_switchMainShow,SIGNAL(clicked()), this, SLOT(switchVideoAndCourseware()));

	//title bar button
	connect(ui.pushButton_title, SIGNAL(clicked()), this, SLOT(classSettingBtnClicked()));
	connect(ui.gifIconpushButton_showSettingDlg,SIGNAL(clicked()), this, SLOT(showSettingDlg()));
	connect(ui.gifIconPushButton_lock, SIGNAL(clicked()), this, SLOT(lockClass()));
	connect(ui.gifIconPushButton_unlock, SIGNAL(clicked()), this, SLOT(unlockClass()));
	connect(ui.gifIconpushButton_minSize, SIGNAL(clicked()), this, SLOT(showMinimized()));
	connect(ui.gifIconPushButton_close, SIGNAL(clicked()), this, SLOT(close()));
	connect(ui.gifIconpushButton_max_normalSize, SIGNAL(clicked()), this, SLOT(max_minSizeBtnClicked()));

	//list tool bar button
	connect(ui.pushButton_in, SIGNAL(clicked()), this, SLOT(zoomInBtnClicked()));
	connect(ui.pushButton_out, SIGNAL(clicked()), this, SLOT(zoomOutBtnClicked()));

	//widget custom signal
	connect(ui.widget_leftSildBar, SIGNAL(show_stu_video_list()), this, SLOT(showStuVideoListWndBtnClicked()));
	connect(ui.label_classroomTitle, SIGNAL(linkActivated(QString)), this, SLOT(doClassSetting(QString)));
	connect(ui.tab_userList,SIGNAL(sg_privateChatCreate(__int64)),this,SLOT(showPrivateChatWidght(__int64)));

	//shortcut key
	connect(m_show_hideMainWnd, SIGNAL(activated()), this, SLOT(showHideMainWnd()));
	connect(m_curScreenShortcut, SIGNAL(activated()), ui.tabWidget_classroom->widget(0), SLOT(screenShotBtnClicked()));
	connect(m_open_closeSound, SIGNAL(activated()), ui.tabWidget_classroom->widget(0), SLOT(open_closeSound()));
	connect(m_open_closeMIc, SIGNAL(activated()), ui.tabWidget_classroom->widget(0), SLOT(open_closeMic()));
	connect(m_incressSound, SIGNAL(activated()), ui.tabWidget_classroom->widget(0), SLOT(incressSound()));
	connect(m_decressSound, SIGNAL(activated()), ui.tabWidget_classroom->widget(0), SLOT(decressSound()));
	connect(m_handsUpDown, SIGNAL(activated()), this, SLOT(handsUpDown()));
}

void ClassRoomDialog::setTitleBarRect()
{
    QPoint pt = ui.widget_titleBar->mapTo(this, QPoint(0, 0));
    m_titlRect = QRect(pt, ui.widget_titleBar->size());
}

void ClassRoomDialog::showMinimized()
{
    QDialog::showMinimized();
}

bool ClassRoomDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && e->type() == QEvent::WindowStateChange)
    {
        return true;
    }

    return C8CommonWindow::eventFilter(o, e);
}

void ClassRoomDialog::closeWnd()
{
	unitNetMsgNotify();

	::savePushDataToLocalFlvFileEnd();
    
    CSkyCursewaveData::freeInstance();
    CHttpSessionMgr::freeInstance();
    CoursewareDataMgr::freeInstance();
    WhiteBoardDataMgr::freeInstance();
    CoursewareTaskMgr::freeInstance();

    ui.widget_coursewaretools->clearData();
	
	//......
	//m_mediaRecord.stop();

    ui.widget_teaVideo->stop();
    ui.widget_teaVideo->setUrl(NULL);
	ui.widget_teaVideo->setStudentVideoStream(m_rtmpSpeakPlayer,false);
	ui.widget_stuVideoListWnd->hideStuVideoListWnd();
	ui.widget_teaPublishVideo->setStudentVideoStream(m_rtmpSpeakPlayer,false);

	if(m_rtmpSpeakPlayer)
	{
		m_rtmpSpeakPlayer->stop();
		delete m_rtmpSpeakPlayer;
		m_rtmpSpeakPlayer = NULL;
	}

	m_chatManager.closeAllChat();
	PictureViewer::freeInstance();

	if(m_popupDlg)
	{
		C8CommonWindow * dlg = m_popupDlg;
		m_popupDlg = NULL;

		dlg->reject();
		delete dlg;
		dlg = NULL;
	}

    CMediaPublishMgr::getInstance()->stopSeatVideo(-1);
    CMediaPublishMgr::getInstance()->setMediaUrl(L"",L"");
	CMediaPublishMgr::getInstance()->stopRecordScreen();
}

void ClassRoomDialog::close()
{
	m_popupDlg = new C8MessageBox(C8MessageBox::Question, QString(tr("info")), QString(tr("ClassQuit")));
	if(!dlgExec())
	{
		return;
	};

	ASSERT_CLASSROOM_DLG_EXSIT;

    PictureViewer::getInstance()->close();
    closeWnd();    	
	if(ClassSeeion::GetInst()->IsTeacher() )
	{   
		biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, 0, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
	}

    biz::GetBizInterface()->LeaveClass(ClassSeeion::GetInst()->_nClassRoomId);

    QDialog::close();

    LobbyDialog::getInstance()->setCloseClassRoom();	
}

void ClassRoomDialog::showCoursewareWindow()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    QSize size = m_addCoursewareWindow.size();
    QPoint ptGlobal = ui.pushButton_showAddCoursewareWindow->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y() - size.height());
    ptGlobal.setX(ptGlobal.x() - (size.width() / 2 )+ ui.pushButton_showAddCoursewareWindow->size().width() / 2);
    m_addCoursewareWindow.setGeometry(QRect(ptGlobal, size));
	m_addCoursewareWindow.show();
	m_addCoursewareWindow.activateWindow();
	qApp->processEvents(); 
/*  
	m_addCoursewareWindow.hide();
    m_addCoursewareWindow.setWindowOpacity(1);
    m_addCoursewareWindow.showNormal();
    m_addCoursewareWindow.activateWindow();
*/

}

void ClassRoomDialog::showSettingDlg()
{
    m_popupDlg = new SettingDialog();
    dlgExec();
}

void ClassRoomDialog::showClassroomSettingDlg()
{
	m_popupDlg = new ClassroomSetting();
	dlgExec();
}

void ClassRoomDialog::showCoursewareDownloadWindow()
{
    QSize size = m_coursewareDownloadWindow.size();
    QPoint ptGlobal = ui.pushButton_showCoursewareDownlistWnd->mapToGlobal(QPoint(0, 0));
    ptGlobal.setY(ptGlobal.y() - size.height());
    ptGlobal.setX(ptGlobal.x() - size.width() + ui.pushButton_showCoursewareDownlistWnd->size().width() + ui.pushButton_showCoursewareDownlistWnd->size().width()/2);
    m_coursewareDownloadWindow.setGeometry(QRect(ptGlobal, size));
    m_coursewareDownloadWindow.setWindowOpacity(1);
    m_coursewareDownloadWindow.showNormal();
    m_coursewareDownloadWindow.activateWindow();
}

void ClassRoomDialog::onRecvStuVideoList(StudentVideoListInfo info)
{

}

void ClassRoomDialog::showStuVideoListWndBtnClicked()
{
    if (!ui.widget_stuVideoListWnd->isVisible())
    {
        ui.widget_stuVideoListWnd->showStuVideoListWnd();
    }
    else
    {
        ui.widget_stuVideoListWnd->hideStuVideoListWnd();
    }
}

void ClassRoomDialog::showTeaMulitVideoBtnClicked()
{
	if(!ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

    if (ui.widget_teaPublishVideo->isMulitChannelShow())
    {
        ui.widget_teaPublishVideo->setMultiChannelShow(false);
		biz::GetBizInterface()->SwitchTeacherVideo(ClassSeeion::GetInst()->_nClassRoomId,
			ClassSeeion::GetInst()->_nUserId,
			ui.widget_teaPublishVideo->getMainVideoIndex());
    }
    else
    {
        ui.widget_teaPublishVideo->setMultiChannelShow(true);

		biz::GetBizInterface()->SwitchTeacherVideo(ClassSeeion::GetInst()->_nClassRoomId,
			ClassSeeion::GetInst()->_nUserId,
			TEACHER_MULTI_VIDEO);
    }    
}

void ClassRoomDialog::switchVideoAndCourseware()
{
    QBoxLayout *teaVideoToolsLayout = qobject_cast<QBoxLayout*>(ui.widget_teaVideoToolBar_bk->layout());
    QBoxLayout *coursewareLayout = qobject_cast<QBoxLayout*>(ui.widget_listTools->layout());

    QBoxLayout *teaVideoLayout = qobject_cast<QBoxLayout*>(ui.widget_teaVideoWnd_bk->layout());
    QBoxLayout *courseMainLayout = qobject_cast<QBoxLayout*>(ui.widget_main->layout());

    if (!teaVideoToolsLayout || !coursewareLayout || !teaVideoLayout || !courseMainLayout)
        return;

    int videoLayoutItemCount = teaVideoToolsLayout->count();
    int coursewareLayoutItemCount = coursewareLayout->count();

    if (videoLayoutItemCount == 3)
    {
        delete teaVideoToolsLayout->takeAt(2);
        videoLayoutItemCount = teaVideoToolsLayout->count();
    }
    
	if(m_typeMainShow == biz::eMainShow_CWWB)
    {
        // 视频换到左边，课件换到右边
        coursewareLayout->removeWidget(ui.widget_coursewaretools);
        teaVideoToolsLayout->insertWidget(1, ui.widget_coursewaretools);
        teaVideoToolsLayout->insertSpacerItem(2, new QSpacerItem(40, 20, QSizePolicy::Expanding));

        teaVideoToolsLayout->removeWidget(ui.pushButton_webCameraSetting);
        teaVideoToolsLayout->removeWidget(ui.pushButton_mulitCamera);
        teaVideoToolsLayout->removeWidget(ui.pushButton_switchMainShow);

        coursewareLayout->insertWidget(0, ui.pushButton_switchMainShow);
        coursewareLayout->insertWidget(0, ui.pushButton_mulitCamera);
        coursewareLayout->insertWidget(0, ui.pushButton_webCameraSetting);
		
        teaVideoLayout->removeWidget(ui.widget_teaPublishVideo);
        teaVideoLayout->removeWidget(ui.widget_teaVideo);
        courseMainLayout->insertWidget(0, ui.widget_teaPublishVideo);
        courseMainLayout->insertWidget(1, ui.widget_teaVideo);
        courseMainLayout->removeWidget(ui.stackedWidget_main);
        teaVideoLayout->insertWidget(0, ui.stackedWidget_main);
        QSize s = ui.widget_teaVideoWnd_bk->size();
        ui.stackedWidget_main->setFixedSize(s);
        int ncount = ui.stackedWidget_main->count();
        for (int i = 0; i < ncount; i++)
        { 
             ui.stackedWidget_main->widget(i)->setFixedSize(s);
             if (QShowClassState *showClassState = qobject_cast<QShowClassState*>(ui.stackedWidget_main->widget(i)))
             {
                 showClassState->updatePic(s);
             }
             ui.stackedWidget_main->widget(i)->update();
        }
        
        m_typeMainShow = biz::eMainShow_VIDEO;
    }
    else if (m_typeMainShow == biz::eMainShow_VIDEO)
    {
        // 课件换到左边，视频换到右边
        coursewareLayout->removeWidget(ui.pushButton_webCameraSetting);
        coursewareLayout->removeWidget(ui.pushButton_mulitCamera);
        coursewareLayout->removeWidget(ui.pushButton_switchMainShow);
        teaVideoToolsLayout->insertWidget(1, ui.pushButton_webCameraSetting);
        teaVideoToolsLayout->insertWidget(2, ui.pushButton_mulitCamera);
        teaVideoToolsLayout->insertWidget(3, ui.pushButton_switchMainShow);

        teaVideoToolsLayout->removeWidget(ui.widget_coursewaretools);
        coursewareLayout->insertWidget(1, ui.widget_coursewaretools);
        
        teaVideoLayout->removeWidget(ui.stackedWidget_main);
        courseMainLayout->insertWidget(0, ui.stackedWidget_main);
        courseMainLayout->removeWidget(ui.widget_teaPublishVideo);
        courseMainLayout->removeWidget(ui.widget_teaVideo);
        teaVideoLayout->insertWidget(0, ui.widget_teaVideo);
        teaVideoLayout->insertWidget(0, ui.widget_teaPublishVideo);

        QSize s = ui.widget_main->size();
        ui.stackedWidget_main->setFixedSize(s);
        int ncount = ui.stackedWidget_main->count();
        for (int i = 0; i < ncount; i++)
        {           
            ui.stackedWidget_main->widget(i)->setFixedSize(s);
            if (QShowClassState *showClassState = qobject_cast<QShowClassState*>(ui.stackedWidget_main->widget(i)))
            {
                showClassState->updatePic(s);
            }
            ui.stackedWidget_main->widget(i)->update();
        }

        m_typeMainShow = biz::eMainShow_CWWB;
    }

    if(ClassSeeion::GetInst()->IsTeacher())
    {
        biz::GetBizInterface()->SetMainShow(m_typeMainShow,ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId);
    }

    setClassRoomUI();
}

void ClassRoomDialog::onUpdateUserInfo(__int64 userID)
{
    SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    QString userName = QString::fromWCharArray(userInfo.szRealName);
    UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1));
    page->updateUserName(userID, userName);
}

void ClassRoomDialog::addUserToList(__int64 userID)
{
    UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1));
    if (page->userInList(userID))
        return;

    SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    QString userName = QString::fromWCharArray(userInfo.szRealName);
    int loginDeviceType = userInfo.nDevice;
    int headIconType = 0;
    if (userInfo.nUserAuthority == biz::UserAu_Teacher)
    {
        if (userInfo.nGender == 1)
            headIconType = 0;
        else
            headIconType = 2;
    }
    else
    {
        if (userInfo.nGender == 1)
            headIconType = 1;
        else
            headIconType = 3;
    }
    bool hasCamera = userInfo._nUserState & biz::eUserState_user_voice;
    bool hasMic = userInfo._nUserState & biz::eUserState_user_video;
    
    page->addItem(userID, userName, loginDeviceType, headIconType);
    
    page->updateUserHasMic(userID, hasMic);
    page->updateUserHasCamera(userID, hasCamera);
    page->reSortItems();
}

void ClassRoomDialog::onUserEnter(__int64 userID)
{
	if(!m_initEnterClassRoom)
	{
		return;
	}

    SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
    if (userInfo.nUserAuthority < UserAu_Student)
    {
        return;
    }

	biz::IClassRoomInfo* classRoomInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	if(NULL == classRoomInfo)
	{
		return;
	}

	biz::SLClassRoomBaseInfo roomBaseInfo = classRoomInfo->GetRoomBaseInfo();
	bool isClassBegin = roomBaseInfo._nClassState == biz::EClassroomstate_Doing ? true : false;

    if (userID == ClassSeeion::GetInst()->_nUserId)
    {   
		if(wcslen(userInfo.szPullUrl) > 0 && wcslen(userInfo.szPushUrl) > 0 && (
            ClassSeeion::GetInst()->IsStudent() ||ClassSeeion::GetInst()->IsTeacher()))
		{
            CMediaPublishMgr::getInstance()->setMediaUrl(userInfo.szPushUrl, userInfo.szPullUrl);
            CMediaPublishMgr::getInstance()->setPublishParam(ClassSeeion::GetInst()->IsTeacher());
            if(isClassBegin)
            {
                CMediaPublishMgr::getInstance()->startAllPublishVideoAsync();
            }
		}
        
		if (ClassSeeion::GetInst()->IsTeacher())
		{
			ui.widget_teaPublishVideo->show();
			ui.widget_teaVideo->hide();
		}
		else
		{
			ui.widget_teaPublishVideo->hide();
			ui.widget_teaVideo->show();
		}
		
		setUserUI();
        setClassRoomUI();

		int nClassMode = biz::Eclassroommode_Speakable | biz::Eclassroommode_Textable | biz::Eclassroommode_Lock;
		setClassMode(roomBaseInfo._nClassMode,nClassMode,false); 
	}

    if (userInfo.nUserAuthority == biz::UserAu_Teacher)
    {
        //更新课堂标题栏
        
		setClassRoomDlgTitle();
    
        if (userInfo.nUserId != ClassSeeion::GetInst()->_nUserId)
        {
            std::string url;
            Util::QStringToString(QString::fromWCharArray(userInfo.szPullUrl), url);
            ui.widget_teaVideo->setUrl(url.c_str());
			ui.widget_teaVideo->setPlayerId(ClassSeeion::GetInst()->_nUserId);

			if(isClassBegin)
            {
				ui.widget_teaVideo->start();
			}
        }
    }

    //添加用户
    addUserToList(userID);
}

void ClassRoomDialog::onUserLeave(UserLeaveInfo info)
{
    if (info._nUserId == ClassSeeion::GetInst()->_nUserId)
    {
        closeWnd();
        QDialog::close();

		m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("Kicked_out")));
		dlgExec();

        LobbyDialog::getInstance()->setCloseClassRoom();

		return;
    }

    //del user    
    biz::SLUserInfo sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(info._nUserId);
    if(sInfo.nUserId == 0)
    {
        return;
    }

    if(sInfo.nUserAuthority < biz::UserAu_Student)
    {
        return;
    }

    if (UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1)))
    {
        page->removeItem(sInfo.nUserId);
    }

    if (sInfo.nUserAuthority >= biz::UserAu_Teacher)
    {
        ui.widget_teaVideo->stop();
        showSysMsg(tr("teacher_left"));        
    }
    
    auto pSpeakUser = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
    if(pSpeakUser)
	{
        if(pSpeakUser->nUserId == info._nUserId || sInfo.nUserAuthority >= biz::UserAu_Teacher)
        {
            setStudentSpeak(*pSpeakUser,false);

            int state = sInfo._nUserState ;
            state = state & (~eUserState_speak);
            state = state & (~eUserState_ask_speak);

            biz::GetBizInterface()->GetUserInfoDataContainer()->SetUserState(pSpeakUser->nUserId,state);
        }
    }
}

void ClassRoomDialog::onUserSpeakAction(UserSpeakActionInfo info)
{
    ClassSeeion *classSession = ClassSeeion::GetInst();
	UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1));
	biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);

	if(info._nUserId== 0 && info._nAction == biz::eUserspeekstate_clean_speak)
    {
        auto pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(info._nRoomId);	
        auto sRoomInfo = pRoom->GetRoomBaseInfo();
        
        if(sRoomInfo._nClassMode & biz::Eclassroommode_Speakable)
        {	
            //显示系统消息 - 取消所有人发言
            if (page)
            {
				page->clearAllSpeakState();
			}

            showWhiteboardTools(false);
        }

        if(classSession->getAuthority() == biz::UserAu_Student || classSession->getAuthority() == biz::UserAu_Assistant)
        {
            ui.pushButton_stuHandsDown->hide();
            ui.pushButton_stuHandsUp->show();

			if(myInfo._nUserState & eUserState_user_mute)
			{
				ui.pushButton_stuHandsUp->setEnabled(false);
			}
			else
			{
				ui.pushButton_stuHandsUp->setEnabled(true);
			}

			ui.widget_coursewaretools->setWhiteBoardEnable(WB_CTRL_NONE);
        }

        biz::SLUserInfo * spkUser = pRoom->GetSpeakUserInfo();
        if(spkUser)
        {
            setStudentSpeak(*spkUser,false);
        }

        qApp->processEvents();
        return;
    }

    int nIndex = 0;
	auto userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(info._nUserId);
	
	if(info._nAction == biz::eUserspeekstate_allow_speak)
    {
        //被提问者不管是否在坐席区，是否是自己暂时都显示
		setStudentSpeak(userInfo,true);

        if(info._nUserId == classSession->_nUserId)
        {   
            ui.widget_coursewaretools->setWhiteBoardEnable(WB_CTRL_SELF);            
        }
    }
    else if(info._nAction == biz::eUserspeekstate_clean_speak)
    {
		setStudentSpeak(userInfo,false);       

        if( info._nUserId == classSession->_nUserId && classSession->IsStudent())
        {	
			ui.widget_coursewaretools->setWhiteBoardEnable(WB_CTRL_NONE);
		}
    }
    else if(info._nAction == biz::eUserspeekstate_Ask_speak)
    {
        if(info._nUserId == classSession->_nUserId)
        {
            ui.pushButton_stuHandsUp->hide();
            ui.pushButton_stuHandsDown->setEnabled(true);
            ui.pushButton_stuHandsDown->show();
        }
		
		if (page)
        {
            page->updateUserHandsUpState(info._nUserId, true);
        }
		//显示系统消息 - 请求发言
        
    }
    else if(info._nAction == biz::eUserspeekstate_cancel_Speak)
    {
        //setStudentSpeak(userInfo,false);
		//显示系统消息 - 取消举手
        if(info._nUserId == classSession->_nUserId)
        {
            ui.pushButton_stuHandsUp->show();
            ui.pushButton_stuHandsUp->setEnabled(true);
            ui.pushButton_stuHandsDown->setEnabled(false);
            ui.pushButton_stuHandsDown->hide();
        }

        if (page)
        {
            page->updateUserHandsUpState(info._nUserId, false);
        }
        
    }
	else if(info._nAction == biz::eClassAction_Open_voice || info._nAction == biz::eClassAction_Close_voice)
	{
		if (page)
		{
			bool bHasMic = info._nAction == biz::eClassAction_Close_voice ? false : true;
			page->updateUserHasMic(info._nUserId, bHasMic);
		}
	}
	else if(info._nAction == biz::eClassAction_Open_video || info._nAction == biz::eClassAction_Close_video)
	{
		if (page)
		{
			bool bHasMic = info._nAction == biz::eClassAction_Close_video ? false : true;
			page->updateUserHasCamera(info._nUserId, bHasMic);
		}
	}
    
    if( (info._nAction == biz::eClassAction_mute || info._nAction == biz::eClassAction_unmute) &&
        classSession->IsStudent() && info._nUserId == classSession->_nUserId) //禁言和取消禁言的处理
    {
        BOOL bEnable = (info._nAction == biz::eClassAction_mute) ? FALSE : TRUE;
		ui.pushButton_stuHandsDown->hide();
		ui.pushButton_stuHandsUp->show();
        ui.pushButton_stuHandsUp->setEnabled(bEnable);

		if(!bEnable && page)
		{
			page->updateUserHandsUpState(info._nUserId,false);
		}
    }

    qApp->processEvents();
    return;
}

void ClassRoomDialog::showMainView(QWidget *pwidget, bool bAddNew)
{
    if (NULL == pwidget)
    {
        return;
    }

	/*
    if (bAddNew)
    {
        ui.stackedWidget_main->addWidget(pwidget);
		pwidget->setParent(ui.stackedWidget_main);
    }
	*/

    ui.stackedWidget_main->setCurrentWidget(pwidget);
}

void ClassRoomDialog::addMainView(QWidget *pwidget)
{
	if (NULL == pwidget)
	{
		return;
	}
	
	ui.stackedWidget_main->addWidget(pwidget);
	pwidget->setParent(ui.stackedWidget_main);	
}

void ClassRoomDialog::classBeginBtnClicked()
{
	IClassRoomDataContainer* classContainer =  biz::GetBizInterface()->GetClassRoomDataContainer();
	if(NULL == classContainer)
	{
		return;
	}

	IClassRoomInfo * classInfo = classContainer->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	if(NULL == classInfo)
	{
		return;
	}

	SLClassRoomBaseInfo roomInfo = classInfo->GetRoomBaseInfo();
	if(roomInfo._nClassState!=Eclassroomstate_Going)
	{
		//提示，没到计划时间        
		QString strTime;
        if(ClassSeeion::GetInst()->_nInterBeginTime >0)
        {
            int nm = ClassSeeion::GetInst()->_nInterBeginTime/60 + 1;        
            strTime = QString(tr("waitClassTime")).arg(nm);
        }
				
        m_popupDlg = new QClassRemindWindow(QClassRemindWindow::WatiStartClass, "", "", "", "", strTime);
        dlgExec();

		return;
	}

	biz::GetBizInterface()->ChangeClassState(ClassSeeion::GetInst()->_nCourseId, ClassSeeion::GetInst()->_nClassRoomId, biz::EClassroomstate_Doing, ClassSeeion::GetInst()->_nUserId);

	if (ClassSeeion::GetInst()->IsTeacher())
	{
		ui.pushButton_classBeginBtn->show();
		ui.pushButton_classBeginBtn->setEnabled(false);
		ui.pushButton_classOverBtn->hide();
	}

    return;
}

void ClassRoomDialog::classOverBtnClicked()
{
	doClassOver();
}

void ClassRoomDialog::onClassStateChange(ClassStateChangeInfo info)
{
    if (ClassSeeion::GetInst()->_nClassRoomId != info._nClassId)
    {
		return;
	}

	if(info._ret != 0)
	{
		if(ClassSeeion::GetInst()->IsTeacher())
		{
			//弹出提示消息......

		}

		return;
	}

    setBenginClass(info._nState, true);
}

void ClassRoomDialog::onRecvClassMsgReminder(ClassMsgReminderInfo info)
{
    if (ClassSeeion::GetInst()->_nClassRoomId != info._nClassId)
    {
        return;
    }
   
    switch (info._nMsgType)
    {
    case biz::EClassMsgSign:
        {
            doClassRoomState_Ready();
        }
        break;
    case biz::EClassMsgBegin:
        {
            doClassRoomState_Going();
        }
        break;
    case biz::EClassMsgSignOut:
        {
            doClassRoomStateSignOut();
        }
        break;
    case biz::EClassMsgEnd:
        {
           doClassMsgEnd();
        }
        break;
    default:
        break;
    }
}

void ClassRoomDialog::removeMainView(QWidget *pwidget)
{
    if (NULL == pwidget)
    {
        return;
    }
	
	if(pwidget->parent() != ui.stackedWidget_main)
	{
		return;
	}

    ui.stackedWidget_main->removeWidget(pwidget);
}

void ClassRoomDialog::clearStackWidget()
{
    int ncount = ui.stackedWidget_main->count();
    for (int i = 0; i < ncount; i++)
    {
        ui.stackedWidget_main->removeWidget(ui.stackedWidget_main->widget(0));
    }
}

void ClassRoomDialog::showMainState(QString fileName, int nPos)
{
	ASSERT_CLASSROOM_DLG_EXSIT;

    if (MAIN_SHOW_TYPE_DOWN_FALIED == nPos || MAIN_SHOW_TYPE_UPLOAD_FAILED == nPos) //表示课件处理失败,弹出提示信息
    {
        QString infoText;
        if (MAIN_SHOW_TYPE_UPLOAD_FAILED == nPos)
        {
            infoText = QString(tr("CoursewareUploadFailed").arg(fileName));
        }
        else if (MAIN_SHOW_TYPE_DOWN_FALIED == nPos)
        {
            infoText = QString(tr("CoursewareDownFailed").arg(fileName));
        }
        else if(MAIN_SHOW_TYPE_TRANS_FAILED == nPos)
        {
            infoText = QString(tr("CoursewareTransFailed").arg(fileName));
        }

        C8MessageBox msgBox(C8MessageBox::Warning, QString(tr("info")), infoText);
        msgBox.exec();
    }

	QWidget * curShowWidget =ui.stackedWidget_main->currentWidget();
	if(curShowWidget != (QWidget*)ui.stackedWidget_mainPage1)
	{
		ui.stackedWidget_main->setCurrentWidget(ui.stackedWidget_mainPage1);
	}
	
    if (100 == nPos)
    {
        ui.stackedWidget_mainPage1->ShowMainState(ClassSeeion::GetInst()->_bBeginedClass, fileName, ClassSeeion::GetInst()->_bBeginedClass?nPos:nPos);		
    }
    else
    {	
        ui.stackedWidget_mainPage1->ShowMainState(ClassSeeion::GetInst()->_bBeginedClass, fileName, nPos);
    }
}

void ClassRoomDialog::reshowMainState(QString filename)
{
    if (!filename.isEmpty())
    {
        ui.stackedWidget_mainPage1->ReshowMainState(ClassSeeion::GetInst()->_bBeginedClass, filename);
    }
}

void ClassRoomDialog::showPreviewView(QString filePath, int npage)
{
    ui.widget_leftSildBar->ShowPreview(filePath, npage);
}

void ClassRoomDialog::removePreviewView(QString filePath)
{
    ui.widget_leftSildBar->RemovePreview(filePath);
}

void ClassRoomDialog::showSysMsg(QString message)
{
	ui.tab_chatWiget->showSysMsg(message);
}

void ClassRoomDialog::setUserUI()
{
	if(m_bIsSetUI)
	{
		return;
	}

	m_bIsSetUI = true;

    biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);
    ClassSeeion::GetInst()->_nMyAuthority = sMyInfo.nUserAuthority;

    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
    biz::SLUserInfo nTeacherInfo = pRoom->GetTeacherInfo();
    biz::SLClassRoomBaseInfo sBaseInfo = pRoom->GetRoomBaseInfo();
    
	int nTotelCount(0), nAside(0);
    nTotelCount = pRoom->GetUserCount(NULL, &nAside, NULL);

    ClassSeeion::GetInst()->_nCourseId = sBaseInfo._nCourseId;    
    ClassSeeion::GetInst()->_nInterBeginTime = sBaseInfo._nBeforeTime * 60;
    if(m_nBenginTimer == 0 && sBaseInfo._nBeforeTime > 0)
    {
        m_nBenginTimer = this->startTimer(1000);
    }

    m_nClassProcessSec = (sBaseInfo._nProcessTime * 60);

    if (ClassSeeion::GetInst()->IsTeacher())
    {
        ui.pushButton_classOverBtn->hide();
        ui.pushButton_stopRecordClassBtn->hide();
        ui.pushButton_showCoursewareDownlistWnd->hide();
        ui.pushButton_stuHandsUp->hide();
        ui.pushButton_stuHandsDown->hide();
        ui.pushButton_classBeginBtn->show();
        ui.pushButton_recordClassBtn->show();
        ui.pushButton_showAddCoursewareWindow->show();
        ui.pushButton_disableHandsupBtn->show();
        ui.pushButton_allowHandsupBtn->hide();
        ui.pushButton_disableSendMsgBtn->show();
        ui.pushButton_allowSendMsgBtn->hide();

        ui.widget_coursewaretools->showUI();
        ui.widget_leftSildBar->showUI(true);

		if(sBaseInfo._nClassMode & biz::Eclassroommode_Lock)
		{
			ui.gifIconPushButton_lock->show();
			ui.gifIconPushButton_unlock->hide();
		}
		else
		{
			ui.gifIconPushButton_lock->hide();
			ui.gifIconPushButton_unlock->show();
		}
    }
    else
    {
        ui.pushButton_classOverBtn->hide();
        ui.pushButton_stopRecordClassBtn->hide();

        ui.pushButton_showCoursewareDownlistWnd->show();
        ui.pushButton_stuHandsUp->show();
        ui.pushButton_stuHandsDown->hide();
        ui.pushButton_classBeginBtn->hide();
        ui.pushButton_recordClassBtn->hide();
        ui.pushButton_showAddCoursewareWindow->hide();
        ui.pushButton_disableHandsupBtn->hide();
        ui.pushButton_allowHandsupBtn->hide();
        ui.pushButton_disableSendMsgBtn->hide();
        ui.pushButton_allowSendMsgBtn->hide();
        ui.pushButton_webCameraSetting->hide();
        ui.pushButton_mulitCamera->hide();
        ui.pushButton_switchMainShow->show();

        ui.widget_coursewaretools->showUI();
        ui.widget_leftSildBar->showUI(false);

        ui.gifIconPushButton_lock->hide();
        ui.gifIconPushButton_unlock->hide();
    }

	setClassRoomDlgTitle();
    updateUserHead();
}

void ClassRoomDialog::timerEvent(QTimerEvent *event)
{
    if (NULL == event)
    {
        return;
    }

    if (event->timerId() == m_nBenginTimer)
    {
        doBenginTimerFunc();
    }

    if (event->timerId() == m_nCountDownTimer)
    {
        doCountDownTimerFunc();
    }

    if (event->timerId() == m_nClassTimer)
    {
        doClassTimerFunc();
    }

    if (event->timerId() == m_nFlushSysMsgTimer)
    {
        doFlushSysMsgFunc();
    }
}

void ClassRoomDialog::doBenginTimerFunc()
{
    if (ClassSeeion::GetInst()->_nInterBeginTime <= 0)
    {
       this->killTimer(m_nBenginTimer);
       m_nBenginTimer = 0;
    }
    else
    {
        ClassSeeion::GetInst()->_nInterBeginTime--;
    }
}

void ClassRoomDialog::doCountDownTimerFunc()
{
    if (m_nClassOverCountDown <= 0)
    {
        m_bISCoundDown = false;
    }
    else
    {
        QString strTime;
        m_nClassOverCountDown--;
        if (m_nClassOverCountDown <= 0)
        {   
            this->killTimer(m_nCountDownTimer);
            m_nCountDownTimer = -1;

            if(ClassSeeion::GetInst()->_bBeginedClass)
            {
                ui.label_time->setText(QString(tr("Lacenty_ClassOver")));
            }
            else
            {
                ui.label_time->setText(QString(tr("Class_Is_Over")));
            }
			return;
        }

        if (m_nClassOverCountDown <= 60)
        {
            strTime = QString("%1%2").arg(m_nClassOverCountDown).arg(QString(tr("ClassOverSecond")));
        }
        else
        {
            strTime = QString("%1%2").arg(m_nClassOverCountDown/60+1).arg(QString(tr("ClassOverMin")));
        }

        if (m_bISCoundDown && !m_bISAutoClassOver)
        {
            ui.label_time->setText(strTime);
        }
    }
}

void ClassRoomDialog::doClassTimerFunc()
{
    m_nClassProcessSec++;
    QString strTime;
    
    char szTime[20] = {0};
    int nh = m_nClassProcessSec/60/60;
    int nm = (m_nClassProcessSec/60)%60;
    int ns = m_nClassProcessSec%60;
    sprintf(szTime, "%02d : %02d : %02d", nh, nm, ns);
    strTime = QString("%1").arg(szTime);
    
    if (!m_bISCoundDown)
    {
        ui.label_time->setText(strTime);
    }
}

void ClassRoomDialog::doFlushSysMsgFunc()
{

}

void ClassRoomDialog::setBenginClass(int nState, bool bISClearCList /* = false */)
{   
    ClassSeeion::GetInst()->_bBeginedClass = nState == biz::EClassroomstate_Doing ? true : false;

    if (nState == biz::Eclassroomstate_Ready)
    {
        // 计划开始时间前10分钟 允许开始上课
        doClassRoomState_Ready();
        showMainState("", MAIN_SHOW_TYPE_NO);
    }
    else if (nState == biz::Eclassroomstate_Going)
    {
        //计划时间已到， 提醒老师开始上课
        doClassRoomState_Going();
        showMainState("", MAIN_SHOW_TYPE_NO);
    }
    else if (nState == biz::EClassroomstate_Doing)
    {
        //更新标题栏时间
        doClassRoomState_Doing();
        setClassRoomUI();
        showMainState("", MAIN_SHOW_TYPE_NO);
    }
    else
    {
        //关闭视频
        doClassRoomState_Other(nState, bISClearCList);
        setClassRoomUI();
        showMainState("", MAIN_SHOW_TYPE_CLASS_OVER);
        ui.pushButton_classBeginBtn->hide();
        ui.pushButton_showAddCoursewareWindow->hide();

        if(ClassSeeion::GetInst()->IsStudent())
        {
            m_popupDlg = new ClassOverEvaluate();
            dlgExec();
        }
    }
    
}

void ClassRoomDialog::doClassRoomState_Ready()
{
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        ui.pushButton_classBeginBtn->show();
        ui.pushButton_classBeginBtn->setEnabled(true);
        ui.pushButton_classOverBtn->hide();
    }

    if (ClassSeeion::GetInst()->IsStudent())
    {

    }
}

void ClassRoomDialog::doClassRoomState_Going()
{   
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
		return;
	}
    ui.pushButton_classBeginBtn->show();
    ui.pushButton_classBeginBtn->setEnabled(true);
    ui.pushButton_classOverBtn->hide();
        
	//提示老师准备上课
    m_popupDlg = new C8MessageBox(C8MessageBox::Question, QString(tr("info")), QString(tr("ClassReminder")));
    if (!dlgExec())
    {
		return;	
    }
    
	ASSERT_CLASSROOM_DLG_EXSIT;

	classBeginBtnClicked();
}

void ClassRoomDialog::doClassRoomState_Doing()
{
	m_nStartTimeSec = time(0);

    this->killTimer(m_nBenginTimer);
    m_nBenginTimer = 0;

    m_nClassTimer = this->startTimer(TIMER_LEN_CLASS_PROCESS);
    m_bISCoundDown = false;

    biz::IClassRoomInfo* classRoomInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	if(NULL == classRoomInfo)
	{
		return;
	}
        
    {
        biz::SLClassRoomBaseInfo sBaseInfo = classRoomInfo->GetRoomBaseInfo(); 
        m_nStartTimeSec = (long long)time(0) - (long long)(sBaseInfo._nClassTimeLen*60);
    }

	
	biz::SLUserInfo teaInfo = classRoomInfo->GetTeacherInfo();
	biz::SLClassRoomBaseInfo roomBaseInfo = classRoomInfo->GetRoomBaseInfo();
	
	setClassRoomDlgTitle();
	
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        ui.pushButton_classBeginBtn->hide();
        ui.pushButton_classOverBtn->show();
    }
	else
	{
		if(!ui.widget_teaVideo->isStart())
		{
			std::string url;
			Util::QStringToString(QString::fromWCharArray(teaInfo.szPullUrl), url);
			ui.widget_teaVideo->setUrl(url.c_str());
			ui.widget_teaVideo->setPlayerId(ClassSeeion::GetInst()->_nUserId);
			ui.widget_teaVideo->start();
		}
	}

	biz::SLUserInfo myInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);	
	CMediaPublishMgr::getInstance()->startAllPublishVideoAsync();

	auto pSpeakUserInfo = classRoomInfo->GetSpeakUserInfo();
	if(pSpeakUserInfo)
	{
		UserSpeakActionInfo speakInfo;
		speakInfo._nUserId = pSpeakUserInfo->nUserId;
		speakInfo._nRoomId = pSpeakUserInfo->_nClassRoomId;
		speakInfo._nAction = biz::eUserspeekstate_allow_speak;
		onUserSpeakAction(speakInfo);
	}
}

void ClassRoomDialog::doClassMsgEnd()
{
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        if (m_nLatencyTime > 0)
        {
            this->killTimer(m_nCountDownTimer);
            m_nCountDownTimer = this->startTimer(TIMER_LEN_COUNTDOWN_TIME);
            m_nClassOverCountDown = CLASSOVER_COUNTDOWN_TIME;
            m_bISAutoClassOver = true;
            ui.label_time->setText(QString(tr("Lacenty_ClassOver")));
        }
        else
        {
            doClassOver(true);
            this->killTimer(m_nCountDownTimer);
            m_nClassOverCountDown = 0;
            m_nCountDownTimer = -1;
            ui.label_time->setText(QString(tr("Lacenty_ClassOver")));
        }
    }
    else if (ClassSeeion::GetInst()->IsStudent())
    {
        biz::IClassRoomInfo* pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
        biz::SLClassRoomBaseInfo sInfo = pRoom->GetRoomBaseInfo();
        if (biz::EClassroomstate_Doing != sInfo._nClassState)
        {
            //老师没来上过课，提示学生可以下课了            
            doClassOver();
            this->killTimer(m_nCountDownTimer);
            m_nClassOverCountDown = 0;
            m_nCountDownTimer = -1;
            ui.label_time->setText(QString(tr("Lacenty_ClassOver")));
        }
    }
    
}

void ClassRoomDialog::doClassRoomStateSignOut()
{
    if (m_nLatencyTime < LATENCY_CLASS_TIME)
    {
        biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
        biz::SLUserInfo nTeacherInfo = pRoom->GetTeacherInfo();
        QString teacher;
        if (NULL == nTeacherInfo.szRealName || NULL ==  nTeacherInfo.szRealName[0])
        {
            teacher = QString::fromWCharArray(nTeacherInfo.szRealName);
        }
        else
        {
            teacher = QString::fromWCharArray(nTeacherInfo.szNickName);
        }

        if(nTeacherInfo.nUserId == ClassSeeion::GetInst()->_nUserId)
        {
            QClassRemindWindow msgBox(QClassRemindWindow::LatencyClassOver, teacher);
			int nr = msgBox.exec();            
			ASSERT_CLASSROOM_DLG_EXSIT;            
			if(nr)
			{
				m_nLatencyTime++;
				biz::GetBizInterface()->SetClassGoOn(ClassSeeion::GetInst()->_nCourseId, ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId);
			}
        }
    }

    biz::IClassRoomInfo* pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
    biz::SLClassRoomBaseInfo sInfo = pRoom->GetRoomBaseInfo();

    if (sInfo._nClassState == biz::EClassroomstate_Doing)
    {
        // 倒计时
        m_bISCoundDown = true;
        m_nCountDownTimer = this->startTimer(TIMER_LEN_COUNTDOWN_TIME);
        m_nClassOverCountDown = 60 * 15;    // 5分钟 + 延迟的10分钟
    }	
}

void ClassRoomDialog::doClassRoomState_Other( int nState, bool bISClearCList )
{	
	//隐藏工具栏
	ui.widget_listTools->hide();
	ui.widget_leftSildBar->showUI(false);
	ui.pushButton_classBeginBtn->hide();
	ui.pushButton_classOverBtn->hide();
	ui.pushButton_stopRecordClassBtn->hide();
	ui.pushButton_showCoursewareDownlistWnd->hide();
	ui.pushButton_stuHandsUp->hide();
	ui.pushButton_stuHandsDown->hide();
	ui.pushButton_recordClassBtn->hide();
	ui.pushButton_showAddCoursewareWindow->hide();
	ui.pushButton_disableHandsupBtn->hide();
	ui.pushButton_allowHandsupBtn->hide();
	ui.pushButton_disableSendMsgBtn->hide();
	ui.pushButton_allowSendMsgBtn->hide();
	ui.pushButton_mulitCamera->hide();
	ui.pushButton_webCameraSetting->hide();
	ui.gifIconPushButton_lock->hide();
	ui.gifIconPushButton_unlock->hide();
	ui.widget_listTools->hide();

    if (ClassSeeion::GetInst()->IsTeacher())
    {  
        ui.widget_teaPublishVideo->update();
    }
	else
	{
		ui.widget_teaVideo->stop();
		ui.widget_teaVideo->setPlayerId(0);
		ui.widget_teaVideo->setUrl(NULL);
		ui.widget_teaVideo->update();
	}

    closeWnd();
	::savePushDataToLocalFlvFileEnd();
	CMediaPublishMgr::getInstance()->stopSeatVideo(-1);
	
	this->killTimer(m_nClassTimer);
	m_nClassTimer = -1;
    ui.label_time->setText(QString(tr("Class_Is_Over")));

    ClassSeeion::GetInst()->_bBeginedClass = false;

    qApp->processEvents();
}

void ClassRoomDialog::disableHandsUpBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing == sRoomInfo._nClassState)
    {
        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, 0, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_Speakdisable);
        ui.pushButton_disableHandsupBtn->hide();
        ui.pushButton_allowHandsupBtn->show();
    }
    else
    {
         m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotHand")));
         dlgExec();
    }
}

void ClassRoomDialog::enableHandsUpBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing == sRoomInfo._nClassState)
    {
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_Speakable);
        ui.pushButton_allowHandsupBtn->hide();
        ui.pushButton_disableHandsupBtn->show();
    }
    else
    {
        m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotHand")));
        dlgExec();
    }
}

void ClassRoomDialog::disableSendMsgBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing == sRoomInfo._nClassState)
    {
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_Textdisable);
        ui.pushButton_allowSendMsgBtn->show();
        ui.pushButton_disableSendMsgBtn->hide();
    }
    else
    {
        m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotSendMsg")));
        dlgExec();
    }
}

void ClassRoomDialog::enableSendMsgBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing == sRoomInfo._nClassState)
    {
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_Textable);
        ui.pushButton_allowSendMsgBtn->hide();
        ui.pushButton_disableSendMsgBtn->show();
    }
    else
    {
        m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotSendMsg")));
        dlgExec();
    }
}

void ClassRoomDialog::handUpStuBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsStudent())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing != sRoomInfo._nClassState)
    {
        m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotHandStu")));
        dlgExec();

        return;
    }
    if(!(sRoomInfo._nClassMode & biz::Eclassroommode_Speakable))
    {
        ui.pushButton_stuHandsUp->setEnabled(false);
        return;
    }

    biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, eUserspeekstate_Ask_speak, 0);
    ui.pushButton_stuHandsUp->setEnabled(false);    
}

void ClassRoomDialog::handDownStuBtnClicked()
{
    if (!ClassSeeion::GetInst()->IsStudent())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    if (biz::EClassroomstate_Doing != sRoomInfo._nClassState)
    {
        m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotHandStu")));
        dlgExec();
        return;
    }
    if(!(sRoomInfo._nClassMode & biz::Eclassroommode_Speakable))
    {
        ui.pushButton_stuHandsDown->setEnabled(false);
        return;
    }
    biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, ClassSeeion::GetInst()->_nUserId, biz::eUserspeekstate_cancel_Speak, 0);    
    ui.pushButton_stuHandsDown->setEnabled(false);
}

void ClassRoomDialog::webCameraSettingBtnClicked()
{
    m_popupDlg = new WebCameraSettingPage();
    dlgExec();
}

void ClassRoomDialog::closeEvent(QCloseEvent * event)
{
    freeInstance();
}

void ClassRoomDialog::doClassSetting(QString url)
{
    classSettingBtnClicked();
}

void ClassRoomDialog::classSettingBtnClicked()
{
	//xiewb 2018.08.30
	/*
    if (ClassSeeion::GetInst()->IsTeacher())
    {
        m_popupDlg = new ClassroomSetting();
        dlgExec();
    }
	*/
}

#include "common/md5.h"
#define PERFECT "perfect"

static char * EncryptionStreamIDMd5(char * name)
{
	char str_streamid[256]= {0};
	//这里加上秘钥
	sprintf(str_streamid,"%s_%s",(char *)PERFECT,name);

	char * str_md5sum_val  = (char*)calloc(128,sizeof(char)); 

	CMD5 md5;
	md5.Init();
	unsigned char md5_output[32]={0};
	md5.Update((unsigned char *)str_streamid,strlen(str_streamid));
	memcpy(md5_output,md5.Finalize(),16);

	int  mn = 0;
	char * pstr = (char*)str_md5sum_val;
	for(; mn < 16;++mn)
	{
		_snprintf(pstr,3,"%2.2x",md5_output[mn]);
		pstr += 2;
	}
	return str_md5sum_val;
}

void ClassRoomDialog::recordBtnClicked()
{
	biz::IClassRoomInfo* roomInfo =  biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	SLClassRoomBaseInfo roomBaseInfo = roomInfo->GetRoomBaseInfo();
	
	/* xiewb 2018.07.24 record screen test */
	/*
	PublishParam paramPublish;
	int typeStream = SOURCESCREEN | SOURCEDEMUSIC;
	char szUrl[MAX_PATH] = { 0 };
	time_t curTime = time(NULL);
	char* szSecert = NULL;
	char szPrefix[64] = {0};

	sprintf_s(szPrefix,sizeof(szPrefix),"%I64u_%I64u_%I64u",roomBaseInfo._nCourseId,ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId);

	szSecert = EncryptionStreamIDMd5(szPrefix);
	if(NULL == szPrefix) {
		return;
	}

	sprintf_s(szUrl,MAX_PATH,"rtmp://121.43.33.4:1935/rec_live/%s_%I64u?secert=%s",szPrefix,curTime,szSecert);
	free(szSecert);

	paramPublish.ml = STANDARRDLEVEL;
	paramPublish.bAudioKaraoke = true;
	paramPublish.bIsFullScreen = true;

	bool ret = ::rtmpPushStreamToServerBegin(szUrl,typeStream,paramPublish);

	return;
	*/

	if(roomBaseInfo._nClassState != biz::EClassroomstate_Doing)
	{
		m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("NotClassNotRecord")));
		dlgExec();

		return;
	}

	/* xiewb 2018.07.26 改为网络录像
    m_popupDlg = new recordSettingdialog(m_mediaRecord.getPath());
    if(!dlgExec())
	{	
		return;
	};

	ASSERT_CLASSROOM_DLG_EXSIT;

	// 开始录像
	QString qstrPath =  recordSettingdialog::getRecordPath();
	
	bool ret = m_mediaRecord.start(qstrPath,(HWND)this->winId());
	*/

	bool ret = CMediaPublishMgr::getInstance()->startRecordScreen();
	if(!ret)
	{
		m_popupDlg = new C8MessageBox(C8MessageBox::Critical, QString(tr("info")), QString(tr("ClassRecordFailed")));
		dlgExec();

		return;
	}

	QApplication::postEvent(this,new QEvent(CLASSROOM_DLG_BEGIN_RECORD_EVENT));
	
	//录像时间显示......
	
}

void ClassRoomDialog::stopRecordBtnClicked()
{
	m_popupDlg = new C8MessageBox(C8MessageBox::Question, QString(tr("info")), QString(tr("ClassRecordStop")));
	if(!dlgExec())
	{
		return;
	};

	ASSERT_CLASSROOM_DLG_EXSIT;

    QApplication::postEvent(this,new QEvent(CLASSROOM_DLG_STOP_RECORD_EVENT));
	CMediaPublishMgr::getInstance()->stopRecordScreen();

	//m_mediaRecord.stop();
}

void ClassRoomDialog::showPrivateChatWidght(__int64 userID)
{
	bool exits = false;
	SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID,&exits);
	if(exits && userInfo.nUserId != ClassSeeion::GetInst()->_nUserId)
	{
		m_chatManager.createPrivateChat(userID);
	}
}

void ClassRoomDialog::onSetClassMode(SetClassModeInfo info)
{
    if (ClassSeeion::GetInst()->_nClassRoomId != info._nRoomId)
    {
        return;
    }

    biz::IClassRoomInfo* pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
		
    if(sRoomInfo._nClassMode == info._nMode)
    {
        return;
    }

    setClassMode(info._nMode,sRoomInfo._nClassMode);
}


void ClassRoomDialog::setClassMode(int newMode,int oldMode,bool showMsg /* = true */)
{
	if(newMode == oldMode)
	{
		return;
	}

	QString strText;
	biz::SLUserInfo sMyInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);

	if((newMode & biz::Eclassroommode_Lock)!=(oldMode & biz::Eclassroommode_Lock))
	{
		if(newMode & biz::Eclassroommode_Lock)
		{
			//锁定课堂
			strText = QString(tr("LockClass"));

			if(ClassSeeion::GetInst()->IsTeacher())
			{
				ui.gifIconPushButton_unlock->hide();
				ui.gifIconPushButton_lock->show();

				qApp->processEvents();
			}
		}
		else
		{
			//解除解锁
			strText = QString(tr("UnLockClass"));

			if(ClassSeeion::GetInst()->IsTeacher())
			{
				ui.gifIconPushButton_unlock->show();
				ui.gifIconPushButton_lock->hide();

				qApp->processEvents();
			}
		}

		if(showMsg)
		{
			showSysMsg(strText);
		}
	}

	if((newMode & biz::Eclassroommode_Speakable)!=(oldMode  & biz::Eclassroommode_Speakable) )
	{
		// 是否可以举手发言 
		if(newMode & biz::Eclassroommode_Speakable)
		{
			//sysmsg: 系统：老师已经允许课堂中举手提问
			strText = QString(tr("TeacherAllowHandUp"));

			if(ClassSeeion::GetInst()->IsStudent() && !(sMyInfo._nUserState & biz::eUserState_user_mute))			
			{
				ui.pushButton_stuHandsUp->setEnabled(true);
			}
		}
		else
		{
			//sysmsg: 系统：老师已经禁止课堂中举手提问
			strText = QString(tr("TeacherNotAllowHandUp"));
			if(!ClassSeeion::GetInst()->IsTeacher())
			{
				ui.pushButton_stuHandsUp->setEnabled(false);
			}
		}

		if(showMsg)
		{
			showSysMsg(strText);
		}
	}

	if((newMode & biz::Eclassroommode_Textable)!=(oldMode & biz::Eclassroommode_Textable) )
	{
		// 是否可以聊天	
		if(newMode & biz::Eclassroommode_Textable)
		{
			//sysmsg: 系统：老师已经允许课堂聊天
			strText = QString(tr("TeacherAllowSendMsg"));
			if(!ClassSeeion::GetInst()->IsTeacher())
			{
				//设置课堂聊天区域可发送消息
				ui.tab_chatWiget->setEnbleChat(true);
			}
		}
		else
		{
			//sysmsg: 系统：老师已经禁止课堂中聊天
			strText = QString(tr("TeacherNotAllowSendMsg"));
			if(!ClassSeeion::GetInst()->IsTeacher())
			{
				//设置课堂聊天区域不可发送消息
				ui.tab_chatWiget->setEnbleChat(false);
			}
		}

		if(showMsg)
		{
			showSysMsg(strText);
		}
	}
}


void ClassRoomDialog::setStudentSpeak(biz::SLUserInfo& userInfo,bool setSpeak)
{
    bool isSelf =  userInfo.nUserId == ClassSeeion::GetInst()->_nUserId ? true : false;
	StudentVideoWnd * wndStudentVideo = ui.widget_stuVideoListWnd->getStudentVideoWnd(userInfo.nUserId);
	CRTMPPlayer * player = NULL;

    if(!isSelf)
	{
        if(wndStudentVideo)
	    {
		    player = wndStudentVideo->getPlayer();
	    }
	    else
	    {
		    if(setSpeak)
		    {
			    player = new CRTMPPlayer();

			    std::string url;
			    Util::QStringToString(QString::fromWCharArray(userInfo.szPullUrl), url);

			    player->setUrl(url.c_str());
			    player->setUserId(ClassSeeion::GetInst()->_nUserId);
			
			    if(m_rtmpSpeakPlayer)
			    {
				    m_rtmpSpeakPlayer->stop();
				    delete m_rtmpSpeakPlayer;
			    }

			    m_rtmpSpeakPlayer = player;
	    	}
		    else
		    {
			    player = m_rtmpSpeakPlayer;
		    }
	    }

        if(setSpeak)
        {
            player->setVideoIndex(0);
            player->setPlayAudio(true);

            if(player->isStart())
            {
                player->change();
            }
            else
            {
                player->start();
            }
        }

        if(ClassSeeion::GetInst()->IsTeacher())
        {
            ui.widget_teaPublishVideo->setStudentVideoStream(player,setSpeak);
        }
        else
        {
            ui.widget_teaVideo->setStudentVideoStream(player,setSpeak);
        }
    }
    else
    {   
        if(setSpeak)
        {
            ui.pushButton_stuHandsDown->show();
            ui.pushButton_stuHandsDown->setEnabled(false);

            ui.pushButton_stuHandsUp->hide();
            ui.pushButton_stuHandsUp->setEnabled(false);
        }
        else
        {
            ui.pushButton_stuHandsDown->hide();
            ui.pushButton_stuHandsDown->setEnabled(false);

            ui.pushButton_stuHandsUp->show();
            ui.pushButton_stuHandsUp->setEnabled(true);
        }

        //开启或关闭音频
        LPPUBLISHSEATINFO seatPublish = CMediaPublishMgr::getInstance()->getPublishSeatInfo(0);
        if(seatPublish && seatPublish->_rtmp)
        {
            CRTMPPublisher* rtmpPublish = dynamic_cast<CRTMPPublisher*>(seatPublish->_rtmp);
			if(rtmpPublish)
			{   
				int sourceType = rtmpPublish->getSourceType();
				if(setSpeak)
				{	
					sourceType |= SOURCEDEVAUDIO;
					rtmpPublish->setSourceType(sourceType);
					rtmpPublish->setMediaLevel(STANDARRDLEVEL);
					rtmpPublish->setPublish(true);
					rtmpPublish->setMediaRole(LISTENERROLE);
				}
				else
				{	
					sourceType &= ~SOURCEDEVAUDIO;
					rtmpPublish->setMediaLevel(SMOOTHLEVEL);
					rtmpPublish->setSourceType(sourceType);					
				}


                rtmpPublish->change();

                if(!ClassSeeion::GetInst()->IsTeacher())
                {
                    ui.widget_teaVideo->setStudentVideoStream(rtmpPublish,setSpeak);
                }
            }
        }

        //开起或关闭白板工具栏功能
        showWhiteboardTools(setSpeak);       
    }
    
	if(!setSpeak && m_rtmpSpeakPlayer)
	{
		m_rtmpSpeakPlayer->stop();
		delete m_rtmpSpeakPlayer;

		m_rtmpSpeakPlayer = NULL;
	}

	UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1));
	if(page)
	{
		page->updateUserSpeakState(userInfo.nUserId,setSpeak);
        page->updateUserHandsUpState(userInfo.nUserId,false);
	}
}

void ClassRoomDialog::onMainShowChanged(ClassMainShowInfo info)
{
    if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
    {
        return;
    }

    if(info._nTeacherId == ClassSeeion::GetInst()->_nUserId)
    {
        return;
    }
    
	if(info._nShowType == biz::eMainShow_CWWB || info._nShowType == biz::eMainShow_Unknown)
	{
		m_typeMainShow = biz::eMainShow_VIDEO;
	}
	else
	{
		m_typeMainShow = biz::eMainShow_CWWB;
	}

    switchVideoAndCourseware();
}

bool ClassRoomDialog::doClassOver(bool timeOut /* = false */)
{
	//学生
	if(ClassSeeion::GetInst()->IsStudent())
    {
        setClassRoomUI();
		m_popupDlg = new ClassOverEvaluate();
		dlgExec();

		return true;
	}

	if(!ClassSeeion::GetInst()->IsTeacher())
	{
		return true;
	}

    if (!ClassSeeion::GetInst()->_bBeginedClass)
    {
        return false;
    }
	//老师
	biz::IClassRoomInfo * classInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);

    //提示老师是否下课
    if ((m_bISCoundDown || m_nClassOverCountDown > 0) || !m_bISAutoClassOver)
    {   
        biz::SLUserInfo nTeacherInfo = classInfo->GetTeacherInfo();
        QString teacher;
        if (NULL == nTeacherInfo.szRealName || NULL ==  nTeacherInfo.szRealName[0])
        {
            teacher = QString::fromWCharArray(nTeacherInfo.szRealName);
        }
        else
        {
            teacher = QString::fromWCharArray(nTeacherInfo.szNickName);
        }

		if(timeOut)
        {
			m_popupDlg = new C8MessageBox(C8MessageBox::Information, QString(tr("ClassInfo")), QString(tr("ClassOver_Info")).arg(teacher));
			dlgExec();
		}
		else
		{
			m_popupDlg = new QClassRemindWindow(QClassRemindWindow::RemindClassOver, teacher);
			if (!dlgExec())
			{
				return false;
			}
		}
    }

	if(!ClassRoomDialog::isValid())
	{
		return false;
	}

    showMainState(NULL, MAIN_SHOW_TYPE_CLASS_OVER);//表示老师已下课
    
    biz::GetBizInterface()->ChangeClassState(ClassSeeion::GetInst()->_nCourseId, ClassSeeion::GetInst()->_nClassRoomId, biz::Eclassroomstate_Free, ClassSeeion::GetInst()->_nUserId);
    setBenginClass(biz::Eclassroomstate_Free, true);

    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
    biz::SLClassRoomBaseInfo sBaseInfo = pRoom->GetRoomBaseInfo();
    long long nPlanTimeLen = sBaseInfo._nEndTime - sBaseInfo._nBeginTime;
    long long nActualTimeLen = m_nClassProcessSec;
    
    char szTime[20] = {0};
    sprintf(szTime, "%02d: %02d: %02d", nPlanTimeLen/60/60, (nPlanTimeLen/60)%60, nPlanTimeLen%60);
    QString planTime = QString("%1").arg(szTime);

    memset(szTime, 0, sizeof(szTime));
    sprintf(szTime, "%02d: %02d: %02d", nActualTimeLen/60/60, (nActualTimeLen/60)%60, nActualTimeLen%60);
    QString ActualTime = QString("%1").arg(szTime);

    memset(szTime, 0, sizeof(szTime));
    long long nLatency = (m_nLatencyTime > 0 ? 900 : 600) - m_nClassOverCountDown;
    sprintf(szTime, "%02d: %02d: %02d", nLatency/60/60, (nLatency/60)%60, nLatency%60);
    QString LatencyTime = QString("%1").arg(szTime);

    m_popupDlg = new QClassRemindWindow(QClassRemindWindow::StatisticsClassTime, "", planTime, ActualTime, LatencyTime);
    dlgExec();
	
	return true;
}

void ClassRoomDialog::initHotkey()
{
    UserInfoManager userinfoManager(ClassSeeion::GetInst()->_nUserId);
    QString value = userinfoManager.getHotkeyValue("Show_HideMainWnd");
    regHotkey(0,  value.isEmpty() ? "CTRL + SHIFT+ Z" : value);

    value = userinfoManager.getHotkeyValue("CutScreen");
    regHotkey(1, value.isEmpty() ? "CTRL + ALT + T" : value);

    value = userinfoManager.getHotkeyValue("Open_CloseClassSound");
    regHotkey(2, value.isEmpty() ? "CTRL + ALT + V" : value);

    value = userinfoManager.getHotkeyValue("Open_CloseMic");
    regHotkey(3, value.isEmpty() ? "CTRL + ALT + D" : value);
    
    value = userinfoManager.getHotkeyValue("HandsUp_Down");
    regHotkey(4, value.isEmpty() ? "CTRL + ALT + A" : value);

    value = userinfoManager.getHotkeyValue("IncressClassroomSound");
    regHotkey(5, value.isEmpty() ? "CTRL + ALT + X" : value);

    value = userinfoManager.getHotkeyValue("DecressClassroomSound");
    regHotkey(6, value.isEmpty() ? "CTRL + ALT + C" : value);
}

bool ClassRoomDialog::regHotkey(int key, QString value)
{
    ChatWidget *chatWidget = qobject_cast<ChatWidget*>(ui.tabWidget_classroom->widget(0));
    value.replace(' ', "");
    switch (key)
    {
    case 0:
        m_show_hideMainWnd->setShortcut(QKeySequence(value));
        break;
    case 1:
        m_curScreenShortcut->setKey(QKeySequence(value));
        break;
    case 2:
        m_open_closeSound->setKey(QKeySequence(value));
        break;
    case 3:
        m_open_closeMIc->setKey(QKeySequence(value));
        break;
    case 4:
        m_handsUpDown->setKey(QKeySequence(value));
        break;
    case 5:
        m_incressSound->setKey(QKeySequence(value));
        break;
    case 6:
        m_decressSound->setKey(QKeySequence(value));
        break;
    default:
        break;
    }
    return true;
}

void ClassRoomDialog::handsUpDown()
{
	if(!ui.pushButton_stuHandsUp->isHidden())
	{
        handUpStuBtnClicked();
    }
	else
	{
        handDownStuBtnClicked();
    }
}

void ClassRoomDialog::windowShowMaxmized() 
{
    QString iconPath = Env::currentThemeResPath();
	if (this->layout()&& getShadow() != SHADOW_AERO)
    {
        this->layout()->setMargin(0);
    }
    m_titlRect = QRect(-2, -1, 1, 1);
    ui.gifIconpushButton_max_normalSize->setToolTip(tr("restoreSize"));
    ui.gifIconpushButton_max_normalSize->setIconPath(iconPath + "gificon_showNormal_normal.gif", iconPath + "gificon_showNormal_hover.gif", iconPath + "gificon_showNormal_pressed.gif");
    QDialog::showMaximized();
    m_isShowNormal = false;
}

void ClassRoomDialog::windowShowNormal() 
{
    QString iconPath = Env::currentThemeResPath();
    if (this->layout() && getShadow() != SHADOW_AERO)
    {
        this->layout()->setMargin(9);
    }
    setTitleBarRect();
    ui.gifIconpushButton_max_normalSize->setToolTip(tr("maxSize"));
    ui.gifIconpushButton_max_normalSize->setIconPath(iconPath + "gificon_maxSize_normal.gif", iconPath + "gificon_maxSize_hover.gif", iconPath + "gificon_maxSize_pressed.gif");        
    QDialog::showNormal(); 
    m_isShowNormal = true;
}

void ClassRoomDialog::showHideMainWnd()
{
    if (this->windowState() == Qt::WindowMaximized || this->windowState() == Qt::WindowNoState)
    {
        showMinimized();
    }
    else
    {
        if(m_isShowNormal){
            windowShowNormal();
		}
        else {
            windowShowMaxmized();
		}
    }
}

void ClassRoomDialog::adjustElementPos()
{
    QSize size = ui.widget_classroomLeft->size();
    ui.widget_main->setFixedSize(size);
    ui.stackedWidget_main->setFixedSize(size);

	//xiewb 2018.09.29
	int ncount = ui.stackedWidget_main->count();
	for (int i = 0; i < ncount; i++)
	{ 
		ui.stackedWidget_main->widget(i)->setFixedSize(size);
		if (QShowClassState *showClassState = qobject_cast<QShowClassState*>(ui.stackedWidget_main->widget(i)))
		{
			showClassState->updatePic(size);
		}
		ui.stackedWidget_main->widget(i)->update();
	}

    QRect rcListTools;
    rcListTools.setX(ui.widget_main->width() - ui.widget_listTools->width() - 20);
    rcListTools.setY(ui.widget_main->height() - ui.widget_listTools->height() - 10);
    rcListTools.setWidth(ui.widget_listTools->width());
    rcListTools.setHeight(ui.widget_listTools->height());
    ui.widget_listTools->setGeometry(rcListTools);

    QRect rcRoomToolBtns;
    rcRoomToolBtns.setX(50);
    rcRoomToolBtns.setY(ui.widget_main->height() - ui.widget_classroomToolBtns->height());
    rcRoomToolBtns.setWidth(ui.widget_classroomToolBtns->width());
    rcRoomToolBtns.setHeight(ui.widget_classroomToolBtns->height());
    ui.widget_classroomToolBtns->setGeometry(rcRoomToolBtns);

    QRect rcLeftToolBar;
    rcLeftToolBar.setX(0);
    rcLeftToolBar.setY(0);
    rcLeftToolBar.setWidth(ui.widget_leftSildBar->width());
    rcLeftToolBar.setHeight(ui.widget_main->height());
    ui.widget_leftSildBar->setGeometry(rcLeftToolBar);
    ui.widget_leftSildBar->setRegion();

    QRect rcStuVideoList;
    rcStuVideoList.setX(0/*(ui.widget_main->width() - ui.widget_stuVideoListWnd->width()) / 2 + 10*/);
    rcStuVideoList.setY(0);
    rcStuVideoList.setWidth(ui.widget_main->width()/*ui.widget_stuVideoListWnd->width()*/);
    rcStuVideoList.setHeight(ui.widget_stuVideoListWnd->height());
    ui.widget_stuVideoListWnd->setGeometry(rcStuVideoList);
}

void ClassRoomDialog::max_minSizeBtnClicked()
{
    if (this->isMaximized())
    {
        windowShowNormal();
    }
    else
    {
        windowShowMaxmized();
    }
}

void ClassRoomDialog::lockClass()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();

    m_popupDlg = new C8MessageBox(C8MessageBox::Question, QString(tr("ClassInfo")), QString(tr("UnLock_Info")));
    if (dlgExec())
    {
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_UnLock);
    }
}

void ClassRoomDialog::unlockClass()
{
    if (!ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }
    biz::IClassRoomInfo * pRoom = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);	
    biz::SLClassRoomBaseInfo sRoomInfo = pRoom->GetRoomBaseInfo();
    m_popupDlg = new C8MessageBox(C8MessageBox::Question, QString(tr("ClassInfo")), QString(tr("Lock_Info")));
    if (dlgExec())
    {
        biz::GetBizInterface()->SetClassMode(ClassSeeion::GetInst()->_nClassRoomId,ClassSeeion::GetInst()->_nUserId,biz::Eclassroommode_Lock);
    }
}
void ClassRoomDialog::showMediaTool(bool isMedia, QString filename, long nowTime, long totleTime, int state)
{
    if (isMedia)
    {
        ui.pushButton_in->hide();
        ui.pushButton_out->hide();
        ui.label_page_size->hide();
        ui.label_empty->hide();
        ui.widget_mediatool->show();
        ui.widget_mediatool->SetPlayTime(nowTime, totleTime, state);
    }
    else
    {
        ui.widget_mediatool->hide();
        ui.pushButton_in->show();
        ui.pushButton_out->show();
        ui.label_page_size->show();
        ui.label_empty->show();
        ui.label_empty->setFixedWidth(70);
    }
    setCoursewareToolUI(isMedia);
}

void ClassRoomDialog::setCoursewareToolUI(bool isMedia)
{
    ui.widget_coursewaretools->setMediaUI(isMedia);
}

void ClassRoomDialog::onConnectServerError(ServerErrorInfo info)
{
	if (info._nServerState == biz::eServerState_ConClose)
	{
		return;
	}

	if(!this->isVisible())
	{
		return;
	}

	switch(info._nServerState)
	{
	case biz::eServerState_connect:
		if(NULL == m_dlgWait)
		{
			break;
		}

		m_dlgWait->cancel();
		m_dlgWait = NULL;

		//xiewb 2018.09.07 close classroom dialog,and enter classroom again
		closeWnd();
		QDialog::close();

		LobbyDialog::getInstance()->autoEnterClass();

		break;
	case biz::eServerState_AutoReconnect:
		if(NULL != m_dlgWait)
		{
			break;
		}

		m_dlgWait = new QWaitDlg(this);
		m_dlgWait->wait(this,SIGNAL(servError()),QObject::tr("msgAutoReconnect"),QString(tr("DisconnectServer")));

		m_dlgWait = NULL;

		break;

	case biz::eServerState_ConFailed:
	case biz::eServerState_ConDis:
		{
			bool dlgWait =  false;
			if (NULL != m_dlgWait) 
			{
				emit servError();
				m_dlgWait = NULL;
				dlgWait=true;
			}

			closeWnd();
			QDialog::close();

			if(dlgWait) {
				break;
			}
			
			m_popupDlg = new C8MessageBox(C8MessageBox::Warning, QString(tr("info")), QString(tr("DisconnectServer")));
			dlgExec();
		}
		break;
	}
}

void ClassRoomDialog::onRecvSetAssistantMsg(SetAssistantInfo info)
{
	
}

void ClassRoomDialog::showWhiteboardTools(bool bIsShow)
{
    if (!ClassSeeion::GetInst()->IsStudent())
    {
        return;
    }
    if (bIsShow)
    {
        ui.widget_leftSildBar->showUI(true);
    }
    else 
    {
        ui.widget_leftSildBar->showUI(false);
    }
}

void ClassRoomDialog::zoomInBtnClicked()
{
    CoursewareDataMgr::GetInstance()->ZoomIn();
}

void ClassRoomDialog::zoomOutBtnClicked()
{
    CoursewareDataMgr::GetInstance()->ZoomOut();
}

void ClassRoomDialog::enterClassRoom(__int64 nClassRoomId)
{
	ClassSeeion::GetInst()->_nClassRoomId = (long long)nClassRoomId;

	this->show();
	this->showMainState("", MAIN_SHOW_TYPE_NO);

	biz::GetBizInterface()->QueryLessonCourseware(ClassSeeion::GetInst()->_nCourseId, ClassSeeion::GetInst()->_nUserId);

	m_initEnterClassRoom = true;
}

void ClassRoomDialog::setClassRoomUI()
{
	UserListTabPage *page = qobject_cast<UserListTabPage *>(ui.tabWidget_classroom->widget(1));	
	biz::IClassRoomInfo* classRoomInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	biz::SLUserInfo teaInfo;
	
	if(NULL != classRoomInfo)
	{
		teaInfo = classRoomInfo->GetTeacherInfo();
	}

    if (ClassSeeion::GetInst()->_bBeginedClass)
    {
        if (ClassSeeion::GetInst()->IsTeacher())
        {
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
                ui.widget_leftSildBar->showUI(true);
                if (ui.widget_coursewaretools->IsCurrentMedia())
                {
                    ui.pushButton_in->hide();
                    ui.pushButton_out->hide();
                }
                else
                {
                    ui.pushButton_in->show();
                    ui.pushButton_out->show();
                }
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
                ui.widget_leftSildBar->showUI(false);
            }

            ui.pushButton_classBeginBtn->hide();
            ui.pushButton_classOverBtn->show();

            ui.pushButton_stopRecordClassBtn->setEnabled(true);
            ui.pushButton_recordClassBtn->setEnabled(true);

            ui.pushButton_showAddCoursewareWindow->show();

            ui.pushButton_disableSendMsgBtn->setEnabled(true);
            ui.pushButton_allowSendMsgBtn->setEnabled(true);

            ui.pushButton_disableHandsupBtn->setEnabled(true);
            ui.pushButton_allowHandsupBtn->setEnabled(true);
        }
        
        if (ClassSeeion::GetInst()->IsStudent())
        {
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
                if (ui.widget_coursewaretools->IsCurrentMedia())
                {
                    ui.pushButton_in->hide();
                    ui.pushButton_out->hide();
                }
                else
                {
                    ui.pushButton_in->show();
                    ui.pushButton_out->show();
                }
                
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
                ui.widget_leftSildBar->showUI(false);
            }
            
            ui.pushButton_showCoursewareDownlistWnd->show();
            ui.pushButton_stuHandsUp->show();
            ui.pushButton_stuHandsDown->hide();
        }

		if(NULL != page && teaInfo.nUserId != 0)
		{
			page->updateUserSpeakState(teaInfo.nUserId,true);
		}
    }
    else
    {
        if (ClassSeeion::GetInst()->IsTeacher())
        {
            ui.widget_leftSildBar->showUI(false);
            ui.pushButton_in->hide();
            ui.pushButton_out->hide();
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
            }

            ui.pushButton_classBeginBtn->show();
            ui.pushButton_classOverBtn->hide();

            ui.pushButton_stopRecordClassBtn->setEnabled(false);
            ui.pushButton_recordClassBtn->setEnabled(false);

            ui.pushButton_showAddCoursewareWindow->show();

            ui.pushButton_disableSendMsgBtn->setEnabled(false);
            ui.pushButton_allowSendMsgBtn->setEnabled(false);

            ui.pushButton_disableHandsupBtn->setEnabled(false);
            ui.pushButton_allowHandsupBtn->setEnabled(false);
        }

        if (ClassSeeion::GetInst()->IsStudent())
        {
            ui.widget_listTools->hide();
            ui.widget_leftSildBar->showUI(false);

            ui.pushButton_showCoursewareDownlistWnd->hide();
            ui.pushButton_stuHandsUp->hide();
            ui.pushButton_stuHandsDown->hide();

            if(m_typeMainShow == biz::eMainShow_VIDEO)
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->hide();
                ui.pushButton_out->hide();
                ui.pushButton_in->hide();
            }
        }

		if(NULL != page && teaInfo.nUserId != 0)
		{
			page->updateUserSpeakState(teaInfo.nUserId,false);
		}
    }

	ui.tab_chatWiget->resetUI();
}


void ClassRoomDialog::setClassCousewareToolUI()
{
    if (ClassSeeion::GetInst()->_bBeginedClass)
    {   
        if (ClassSeeion::GetInst()->IsTeacher())
        {
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
                if (ui.widget_coursewaretools->IsCurrentMedia())
                {
                    ui.pushButton_in->hide();
                    ui.pushButton_out->hide();
                }
                else
                {
                    ui.pushButton_in->show();
                    ui.pushButton_out->show();
                }
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
            }
        }

        if (ClassSeeion::GetInst()->IsStudent())
        {   
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
                if (ui.widget_coursewaretools->IsCurrentMedia())
                {
                    ui.pushButton_in->hide();
                    ui.pushButton_out->hide();
                }
                else
                {
                    ui.pushButton_in->show();
                    ui.pushButton_out->show();
                }
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
            }
        }
    }
    else
    {        
        if (ClassSeeion::GetInst()->IsTeacher())
        {            
            ui.pushButton_in->hide();
            ui.pushButton_out->hide();
            if (ui.widget_coursewaretools->IsHaveCourseware())
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->show();
            }
            else
            {
                ui.widget_listTools->hide();
                if(m_typeMainShow == biz::eMainShow_VIDEO)
                {
                    ui.widget_listTools->show();
                    ui.widget_coursewaretools->hide();
                }
            }
        }

        if (ClassSeeion::GetInst()->IsStudent())
        {
            ui.widget_listTools->hide();

            if(m_typeMainShow == biz::eMainShow_VIDEO)
            {
                ui.widget_listTools->show();
                ui.widget_coursewaretools->hide();
                ui.pushButton_out->hide();
                ui.pushButton_in->hide();
            }
        }
    }
}

void ClassRoomDialog::setClassRoomDlgTitle()
{
	biz::IClassRoomInfo* classRoomInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	if(NULL == classRoomInfo)
	{
		return;
	}

	biz::SLUserInfo teaInfo = classRoomInfo->GetTeacherInfo();
	biz::SLClassRoomBaseInfo roomBaseInfo = classRoomInfo->GetRoomBaseInfo();
	 QString strTeaInfo = QString(tr("RoomTitle")).arg(QString::fromWCharArray(teaInfo.szRealName)).arg(QString::fromWCharArray(roomBaseInfo._szRoomName));    
	ui.label_classroomTitle->setText(strTeaInfo);
	ui.label_classroomTitle->update();
}

void ClassRoomDialog::onSwitchTeacherVideo(TeacherSwitchVideoInfo info)
{
	if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	if(ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	biz::IClassRoomInfo* classRoomInfo = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId);
	if(NULL == classRoomInfo)
	{
		return;
	}

	biz::SLUserInfo teaInfo = classRoomInfo->GetTeacherInfo();
	if(teaInfo.nUserId != info._nUserId)
	{
		return;
	}

	if(info._nVideo == TEACHER_MULTI_VIDEO)
	{
		ui.widget_teaVideo->setMultiChannelShow(true);
		return;
	}

	ui.widget_teaVideo->setMainVideo(info._nVideo);
	ui.widget_teaVideo->setMultiChannelShow(false);
}

int ClassRoomDialog::dlgExec()
{
	if(NULL == m_popupDlg)
	{
		return 0;
	}

	int nr = m_popupDlg->exec();
	if(!ClassRoomDialog::isValid())
	{
		return 0;
	}

	if(NULL != m_popupDlg)
	{
		delete  m_popupDlg;
		m_popupDlg = NULL;
	}

	return nr;
}

void ClassRoomDialog::onMediaInitFinish()
{
    SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(ClassSeeion::GetInst()->_nUserId);
    if (userInfo.nUserAuthority < UserAu_Student)
    {
        return;
    }

    if(wcslen(userInfo.szPullUrl) <= 0 && wcslen(userInfo.szPushUrl) <= 0)
    {
        return;
    }

    CMediaPublishMgr::getInstance()->setMediaUrl(userInfo.szPushUrl,userInfo.szPullUrl);
    CMediaPublishMgr::getInstance()->setPublishParam(ClassSeeion::GetInst()->IsTeacher());

    if(ClassSeeion::GetInst()->_bBeginedClass)
    {
        CMediaPublishMgr::getInstance()->startAllPublishVideoAsync();
    }
}

void ClassRoomDialog::customEvent(QEvent * event)
{
    if(NULL == event)
    {
        return;
    }

    QEvent::Type eType= event->type();
    switch(eType)
    {
    case CLASSROOM_DLG_BEGIN_RECORD_EVENT:
        {
            ui.pushButton_stopRecordClassBtn->show();
            ui.pushButton_recordClassBtn->hide();
        }
        break;
    case CLASSROOM_DLG_STOP_RECORD_EVENT:
        {
            ui.pushButton_stopRecordClassBtn->hide();
            ui.pushButton_recordClassBtn->show();
        }
        break;
    default:
        break;
    }
}

void ClassRoomDialog::updateUserHead()
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
    ui.label_userpic2->clear();
    ui.label_userpic2->setPixmap(headPixmap.scaled(size));
}

void ClassRoomDialog::resizeEvent(QResizeEvent * event)
{
	QDialog::resizeEvent(event);
	adjustElementPos();
}