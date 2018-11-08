#ifndef CLASSROOMDIALOG_H
#define CLASSROOMDIALOG_H

#include "src/control/c8commonwindow.h"
#include "ui_classroomdialog.h"
#include <QListWidget>
#include <QMutex>
#include <QTimer>
#include "addcoursewarewindow.h"
#include "coursewaredownloadlistwindow.h"
#include "../varname.h"
#include "noticWindow/noticwindow.h"
#include "Courseware/coursewaredatamgr.h"
#include "control/accountitem.h"
#include "src/chat/chatmanager.h"
#include "media/MediaRecord.h"
#include "WaitDlg.h"
#include "qselectcolor.h"
#include "qselecttext.h"
#include "coursewaretoolsData.h"

//刷新时长
#define  TIMER_LEN_BENGIN_TIME      (1000)   //上课倒计时
#define  TIMER_LEN_CLASS_STATE      (4000)   //课堂状态
#define  TIMER_LEN_CLASS_PROCESS    (1000)   //上课时长
#define  TIMER_LEN_CLASS_PROCESSMIN (60000)  //上课时长(分钟)
#define  TIMER_LEN_RECORD_TIME      (1000)   //录像时长
#define  TIMER_LEN_ENABLE_VIDE      (3000)   //刷新摄像头和麦克风状态
#define  TIMER_LEN_FLUSH_SYSMSG     (100)    //刷新系统消息
#define  TIMER_LEN_SHOW_PRIVATECHAT (100)    //有私聊状态
#define  TIMER_LEN_COUNTDOWN_TIME   (1000)   //下课倒计时

#define  CLASSOVER_COUNTDOWN_TIME   (10*60)  //延迟下课10分钟

#define LATENCY_CLASS_TIME  (1) //延迟下课次数

class QxtGlobalShortcut;

class ClassRoomDialog : public C8CommonWindow
{
    Q_OBJECT

public:
    static ClassRoomDialog *getInstance();
    static void freeInstance();
    static bool isValid();

    bool regHotkey(int key, QString value);
    void closeWnd();
    
	virtual ~ClassRoomDialog();

	void enterClassRoom(__int64 nClassRoomId);

    bool eventFilter(QObject *o, QEvent *e);

signals:
    void set_classroom_chat(bool nState);
	void servError();

private:
    ClassRoomDialog(QWidget *parent = 0);
    
protected slots:

    void showMinimized();
    void doClose();
    void classSettingBtnClicked();
    void doClassSetting(QString);

    void showCoursewareWindow();
    void showSettingDlg();

    void showClassroomSettingDlg();
    void showCoursewareDownloadWindow();
    
    void showStuVideoListWndBtnClicked();
    void showTeaMulitVideoBtnClicked();
    void switchVideoAndCourseware();

    void recordBtnClicked(); 
	void stopRecordBtnClicked();
    void classBeginBtnClicked();
    void classOverBtnClicked();
    void onClassStateChange(ClassStateChangeInfo info);
    void onRecvClassMsgReminder(ClassMsgReminderInfo info);
    void onSetClassMode(SetClassModeInfo info);
    void onMainShowChanged(ClassMainShowInfo info);
    
    void onUserLeave(UserLeaveInfo info);
    void onUserSpeakAction(UserSpeakActionInfo info);

    void disableHandsUpBtnClicked();
    void enableHandsUpBtnClicked();
    void disableSendMsgBtnClicked();
    void enableSendMsgBtnClicked();
    void handUpStuBtnClicked();
    void handDownStuBtnClicked();
    
    void webCameraSettingBtnClicked();

    void max_minSizeBtnClicked();

    void windowShowMaxmized();
    void windowShowNormal();

    void lockClass();
	void unlockClass();

    void zoomInBtnClicked();
    void zoomOutBtnClicked();

	//xiewb 2018.10.17
	void showClassChatWidget();
	void showClassUserList();

	void enableCameraClicked();
	void disableCameraClicked();
	
	void enableMicrophoneClicked();
	void disableMicrophoneClicked();

	void enableSpeakerClicked();
	void disableSpeakerClicked();

	void micVolumeSliderChange(int);
	void spkVolumeSliderChange(int);

	//xiewb 2018.10.24
	void wbUndoClicked();
	void wbTextClicked();
	void wbPenClicked();
	void wbEraseClicked();
	void wbColorClicked();
	void wbClearClicked();

	//xiewb 2018.10.25
	void preCoursewareClicked();
	void nextCoursewareClicked();
	void preFilePageClicked();
	void nextFilePageClicked();

	//xiewb 2018.10.29
	void mediaPlayBtnClicked();
	void mediaPauseBtnClicked();
	void mediaSliderPressed();
	void mediaSliderReleased();
	void mediaSliderValueChanged(int);

	//xiewb 2018.10.30
	void closeCoursewareClicked();
public slots:	
	void addMainView(QWidget *pwidget);
    void showMainView(QWidget *pwidget, bool bAddNew = false);
    void removeMainView(QWidget *pwidget);

    void showPreviewView(QString, int){};
    void removePreviewView(QString){};

    void showMainState(QString, int);
    void reshowMainState(QString);
    
    void onUserEnter(__int64 userID);
    void onRecvStuVideoList(StudentVideoListInfo info);
    void onUpdateUserInfo(__int64 userID);

	void onConnectServerError(ServerErrorInfo);
	void onRecvSetAssistantMsg(SetAssistantInfo);
	void onSwitchTeacherVideo(TeacherSwitchVideoInfo);

	void showPrivateChatWidght(__int64 userID);
    void handsUpDown();
    void showHideMainWnd();

    void setClassRoomUI();
    void onMediaInitFinish();
    void updateUserHead();

	//xiewb 2018.10.24
	void setCoursewareTool(int type,int cur,int all,int state,QWidget* showWidget=NULL);
	void setCoursewareToolUI();
	void setPageShowText(int curPage,int allPage);
protected:
    void addUserToList(__int64 userID);
    void clearStackWidget();
    
    void showSysMsg(QString message);

    void setUserUI();
    void timerEvent(QTimerEvent *event);
    void setBenginClass(int nState, bool bISClearCList = false);

	void setStudentSpeak(biz::SLUserInfo& userInfo,bool setSpeak);
    virtual void setTitleBarRect() override;
    virtual void closeEvent(QCloseEvent * event);

    void  doBenginTimerFunc();
    void  doCountDownTimerFunc();
    void  doClassTimerFunc();
    void  doFlushSysMsgFunc();
    
    void initHotkey();
    void adjustElementPos();

    void showWhiteboardTools(bool bIsShow);

	void setClassRoomDlgTitle();

	int  dlgExec();

	void setClassMode(int newMode,int oldMode,bool showMsg = true);

	//xiewb 2018.09.28
	void initNetMsgNotify();
	void unitNetMsgNotify();

	void initUI();
	void initUiMsgNotify();

	//xiewb 2018.10.25
	void setWhiteboardEnable(int enable);

	//xiewb 2018.10.26
	void coursewareShowCtrl(int showCtrl);

	//xiewb 2018.10.29
	void setCoursewareCtrlBtnUI();

	//xiewb 2018.10.30
	void setCoursewareNameShow(QString qstrName);
protected:
    void customEvent(QEvent * event);
	void resizeEvent(QResizeEvent *);
private:

    void doClassRoomState_Ready();
    void doClassRoomState_Going();
    void doClassRoomState_Doing();
    void doClassMsgEnd();
    void doClassRoomStateSignOut();
    void doClassRoomState_Other( int nState, bool bISClearCList );
	
    bool doClassOver(bool timeOut = false); 
private:
    Ui::ClassRoomDialog ui;
	AddCoursewareWindow m_addCoursewareWindow;
    CoursewareDownloadListWindow m_coursewareDownloadWindow;

protected:
    WBAndCourseViewVector   m_vecWBAndCourseView;
    QMutex      m_mutex;

    static ClassRoomDialog*  m_instance;
	ChatManager m_chatManager;

    long long  m_nClassProcessSec;  //课堂已进行时长（秒）
    long long  m_nStartTimeSec;     //开始上课的时间
    long long  m_nClassOverCountDown;  
    bool       m_bISCoundDown;
    int        m_nLatencyTime;      //延迟下课次数
    bool       m_bISAutoClassOver;  //自动下课（时间到了,用于延时下课）

    //定时器
    int     m_nBenginTimer;       //上课倒计时
    int     m_nCountDownTimer;    //下课倒计时
    int     m_nClassTimer;        //上课时长
    int     m_nFlushSysMsgTimer;  //刷新系统消息
    
    bool    m_bIsSetUI;				//是否已设定课堂状态
	bool	m_initEnterClassRoom;	//是否初始化显示

	CRTMPPlayer * m_rtmpSpeakPlayer;
    char    m_typeMainShow;

    bool m_isShowNormal;
    QxtGlobalShortcut *m_show_hideMainWnd;
    QShortcut *m_curScreenShortcut;
    QShortcut *m_open_closeSound;
    QShortcut *m_open_closeMIc;
    QShortcut *m_handsUpDown;
    QShortcut *m_incressSound;
    QShortcut *m_decressSound;

	// xiewb 2018.07.26
	//CMediaRecord   m_mediaRecord;   

	C8CommonWindow*  m_popupDlg;

	//xiewb 2018.09.06
	QWaitDlg*		m_dlgWait;

	//xiewb 2018.10.15
	bool			m_doClose;

	//xiewb 2018.10.24
	QSelectColor       m_selectColor;
	QSelectText        m_selectText;

	//xiewb 2018.10.29
	bool			   m_mediaSliderPressed;
};


#endif // CLASSROOMDIALOG_H
