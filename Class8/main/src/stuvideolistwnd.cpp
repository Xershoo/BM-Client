#include "stuvideolistwnd.h"
#include "../session/classsession.h"
#include "BizInterface.h"
#include "common/Share_Ptr.h"
#include "common/Util.h"
#include "MediaPublishMgr.h"
#include "common/Env.h"
#include <QtWidgets/QToolTip>
#include <QtGui/QHoverEvent>

#define MAX_STUDENT_VIDEO_COUNT  (8)

StuVideoListWnd::StuVideoListWnd(QWidget *parent)
    : QWidget(parent)	
{
    ui.setupUi(this);

    m_videoList.push_back(ui.widget_stuVideo1);
    m_videoList.push_back(ui.widget_stuVideo2);
    m_videoList.push_back(ui.widget_stuVideo3);
    m_videoList.push_back(ui.widget_stuVideo4);
    m_videoList.push_back(ui.widget_stuVideo5);
    m_videoList.push_back(ui.widget_stuVideo6);
    m_videoList.push_back(ui.widget_stuVideo7);
    m_videoList.push_back(ui.widget_stuVideo8);

    for(int i=0;i<m_videoList.size();i++)
    {
        StudentVideoWnd* wndVideo = m_videoList.at(i);
        if(NULL == wndVideo)
        {
            continue;
        }

        connect(wndVideo,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
        wndVideo->installEventFilter(this);
    }

    /*
    connect(ui.widget_stuVideo1,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo2,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo3,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo4,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo5,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo6,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo7,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
    connect(ui.widget_stuVideo8,SIGNAL(stuSpeakBtnClicked(__int64,bool)),this,SLOT(onUserSpeakChange(__int64,bool)));
	*/

	CBizCallBack * pCallBack = getBizCallBack();
	if(pCallBack)
	{
		connect(pCallBack,SIGNAL(UserEnter(__int64)),this,SLOT(onUserEnter(__int64)));
		connect(pCallBack,SIGNAL(UserLeave(UserLeaveInfo)),this,SLOT(onUserLeave(UserLeaveInfo)));
		connect(pCallBack,SIGNAL(TopVideoChange(TopVideoChangeInfo)),this,SLOT(onTopVideoChange(TopVideoChangeInfo)));
		connect(pCallBack,SIGNAL(RecvStudentVideoList(StudentVideoListInfo)),this,SLOT(onRecvStudentVideoList(StudentVideoListInfo)));
        connect(pCallBack,SIGNAL(UpdateUserInfo(__int64)),this,SLOT(onUpdateUserInfo(__int64)));
        connect(pCallBack,SIGNAL(UserSpeakAction(UserSpeakActionInfo)), this, SLOT(onUserSpeakAction(UserSpeakActionInfo)),Qt::AutoConnection);
	}

	CMediaPublishMgr* mgrMediaPublish = CMediaPublishMgr::getInstance();
	if(NULL != mgrMediaPublish)
	{
		connect(mgrMediaPublish,SIGNAL(selCameraChanged(int)),this,SLOT(onSelCameraChanged(int)));
	}

    QString iconPath = Env::currentThemeResPath();
    ui.gifIconPushButton_flush->setIconPath(iconPath + "gificon_flush_normal.gif", iconPath + "gificon_flush_hover.gif", iconPath + "gificon_flushs_pressed.gif");

	setVideoWidgetTooltip(ui.widget_self,ClassSeeion::GetInst()->_nUserId);
}

StuVideoListWnd::~StuVideoListWnd()
{
	enableAllStudentVideo(false);

	CRTMPPublisher* rtmpPublish = ui.widget_self->getPublisher();
	if(rtmpPublish)
	{
		rtmpPublish->stop();
	}

	CBizCallBack * pCallBack = getBizCallBack();
	if(pCallBack)
	{
		disconnect(pCallBack,NULL);
	}
	
}

void StuVideoListWnd::showStuVideoListWnd()
{
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
    this->show();

	if(ClassSeeion::GetInst()->_bBeginedClass)
	{	
		enableAllStudentVideo(true);
	}

	if (ClassSeeion::GetInst()->IsTeacher())
	{
		ui.widget_self->hide();
	}
	else
	{
		ui.widget_self->setVideoIndex(CMediaPublishMgr::getInstance()->getSelCamera());
        ui.gifIconPushButton_flush->hide();
	}
}

void StuVideoListWnd::hideStuVideoListWnd()
{
	enableAllStudentVideo(false);

    this->hide();
    this->setWindowFlags(Qt::Widget);
}

StudentVideoWnd* StuVideoListWnd::getStudentVideoWnd(__int64 userId)
{
	if(m_videoList.empty())
	{
		return NULL;
	}

	for(int i=0 ; i<m_videoList.size();i++)
	{
		__int64 uid = m_videoList[i]->getUserId();
		if(uid == userId)
		{			
			return m_videoList[i];
		}
	}

	return NULL;
}

void StuVideoListWnd::onUserEnter(__int64 nUserId)
{	
	if(nUserId == ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}
	
	addVideoListUser(nUserId);
}

void StuVideoListWnd::onUserLeave(UserLeaveInfo info)
{
	if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	if(info._nUserId == ClassSeeion::GetInst()->_nUserId)
	{	
		return;
	}

	delVideoListUser(info._nUserId);	
}

void StuVideoListWnd::onTopVideoChange(TopVideoChangeInfo info)
{
	if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	if(info._nUserId == ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}

	if(info._bAddToTop)
	{
		addVideoListUser(info._nUserId);
	}
	else
	{
		delVideoListUser(info._nUserId);
	}

	return;
}

void StuVideoListWnd::onRecvStudentVideoList(StudentVideoListInfo info)
{
	if(info._nRoomId != ClassSeeion::GetInst()->_nClassRoomId)
	{
		return;
	}

	enableAllStudentVideo(false);
	updateVideoList(info._stuVideoList);
}

void StuVideoListWnd::updateVideoList(std::vector<__int64>& vecUser)
{
	if(vecUser.empty())
	{
		return;
	}

	IUserInfoDataContainer *userContainer = biz::GetBizInterface()->GetUserInfoDataContainer();
	if(NULL == userContainer)
	{
		return;
	}

	int nUser = 0;

	for(int i=0 ; i<m_videoList.size();)
	{
		if(nUser >= vecUser.size())
		{
			setVideoWidget(m_videoList[i],0);
			
			i++;
			continue;
		}

		CRTMPPlayer* rtmpPlayer = m_videoList[i]->getPlayer();
		if(NULL == rtmpPlayer)
		{
			i++ ;
			continue;
		}

		__int64 uid = vecUser.at(nUser);
		if(uid <= 0 || uid == ClassSeeion::GetInst()->_nUserId) 
		{
			nUser++;
			continue;
		}

		if(!setVideoPlayerUserId(rtmpPlayer,uid))
		{
			nUser++;
			continue;
		}

		setVideoWidget(m_videoList[i],uid);
		i++;
		nUser++;
	}

}

void StuVideoListWnd::enableAllStudentVideo(bool enable)
{
	for (size_t i = 0; i != m_videoList.size(); i++)
	{
		CRTMPPlayer* rtmpPlayer = m_videoList[i]->getPlayer();
		if(NULL == rtmpPlayer)
		{
			continue;
		}

		if(enable)
		{
			rtmpPlayer->start();
		}
		else
		{
            __int64 userId = m_videoList[i]->getUserId();
            auto pSpeakUser = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
            if(pSpeakUser&&pSpeakUser->nUserId == userId)
            {
                continue;
            }

			rtmpPlayer->stop();
		}
	}
}

void StuVideoListWnd::addVideoListUser(__int64 userId)
{
	if(userId <= 0 || userId == ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}
	
    IUserInfoDataContainer *userContainer = biz::GetBizInterface()->GetUserInfoDataContainer();
    if(NULL == userContainer)
    {
        return;
    }

    bool userExsit = true;
    biz::SLUserInfo userInfo = userContainer->GetUserInfoById(userId,&userExsit);
    if(!userExsit)
    {
        return;
    }

    if(userInfo.nUserAuthority != biz::UserAu_Student)
    {
        return;
    }

	int nIndex = -1;
	for(int i=0 ; i<m_videoList.size();i++)
	{
		__int64 uid = m_videoList[i]->getUserId();
		if(uid == userId)
		{			
			return;
		}

		if(uid <= 0 && nIndex < 0)
		{
			nIndex = i;
		}
	}

	if(nIndex < 0)
	{
		return;
	}

	CRTMPPlayer* rtmpPlayer = m_videoList[nIndex]->getPlayer();
	if(NULL == rtmpPlayer)
	{
		return;
	}

	setVideoPlayerUserId(rtmpPlayer,userId);
    setVideoWidget(m_videoList[nIndex],userId);
    
	return;
}

void StuVideoListWnd::delVideoListUser(__int64 userId)
{
	if(userId <= 0 || userId == ClassSeeion::GetInst()->_nUserId)
	{
		return;
	}

	int nIndex = -1;
	int nLast = -1;
	for(int i=0 ; i<m_videoList.size();i++)
	{
		__int64 uid = m_videoList[i]->getUserId();
		if(uid == userId)
		{	
			nIndex = i;
            continue;
		}

		if(uid > 0 )
		{
			nLast = i;
		}
	}

	if(nIndex < 0)
	{
		return;
	}

	CRTMPPlayer* rtmpPlayer = m_videoList[nIndex]->getPlayer();
	if(NULL == rtmpPlayer)
	{
		return;
	}

	rtmpPlayer->stop();
    rtmpPlayer->setUserId(0);
    setVideoWidget(m_videoList[nIndex],0);

	if(nLast <= nIndex)
	{
		return;
	}

	CRTMPPlayer* rtmpPlayerLast = m_videoList[nLast]->getPlayer();
	if(NULL == rtmpPlayerLast)
	{
		return;
	}
	
	__int64 nLastUserId = m_videoList[nLast]->getUserId();
	rtmpPlayerLast->stop();
	
    rtmpPlayerLast->setUserId(0);
    setVideoWidget(m_videoList[nLast],0);

    setVideoPlayerUserId(rtmpPlayer,nLastUserId);    
    setVideoWidget(m_videoList[nIndex],nLastUserId);
	return;
}

bool StuVideoListWnd::setVideoPlayerUserId(CRTMPPlayer* rtmpPlayer,__int64 userId)
{
	if(NULL == rtmpPlayer)
	{
		return false;
	}

	if(userId <= 0)
	{
		return false;
	}

	IUserInfoDataContainer *userContainer = biz::GetBizInterface()->GetUserInfoDataContainer();
	if(NULL == userContainer)
	{
		return false;
	}

	bool userExsit = true;
	biz::SLUserInfo userInfo = userContainer->GetUserInfoById(userId,&userExsit);
	if(!userExsit)
	{
		return false;
	}

	if(NULL == userInfo.szPullUrl[0])
	{
		return false;
	}

    auto pSpeakUser = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
    if(pSpeakUser && pSpeakUser->nUserId == userId)
    {
        rtmpPlayer->setPlayAudio(true);
    }
    else
    {
        rtmpPlayer->setPlayAudio(false);
    }

	std::string url;
	Util::QStringToString(QString::fromWCharArray(userInfo.szPullUrl), url);
	rtmpPlayer->setUrl(url.c_str());
    
	rtmpPlayer->setUserId(ClassSeeion::GetInst()->_nUserId);

	if (isVisible())
	{
		rtmpPlayer->start();
	}

	return true;
}

void StuVideoListWnd::onSelCameraChanged(int nCamera)
{	
	if (ClassSeeion::GetInst()->IsTeacher())
	{
		return;
	}

	if(ui.widget_self->getVideoIndex() == nCamera)
	{
		return;
	}

	ui.widget_self->setVideoIndex(nCamera);	
}

void StuVideoListWnd::setVideoWidgetTooltip(QWidget* videoWidget,__int64 userId)
{
	if(NULL == videoWidget)
	{
		return;
	}
	SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userId);
	QString userName;
	if (NULL == userInfo.szRealName|| NULL == userInfo.szRealName[0])
	{
		userName = QString::fromWCharArray(userInfo.szNickName);
	}
	else
	{
		userName = QString::fromWCharArray(userInfo.szRealName);
	}

	videoWidget->setToolTip(userName);
}


void StuVideoListWnd::setVideoWidget(StudentVideoWnd* videoWidget,__int64 userId)
{
    if(NULL == videoWidget)
    {
        return;
    }

    QString userName;
    bool	userSpeak = false;
    int		userHand = false;

    if(userId > 0)
    {
        SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userId);
        if (NULL == userInfo.szRealName|| NULL == userInfo.szRealName[0])
        {
            userName = QString::fromWCharArray(userInfo.szNickName);
        }
        else
        {
            userName = QString::fromWCharArray(userInfo.szRealName);
        }

        if(userInfo._nUserState == biz::eUserspeekstate_Ask_speak)
        {
            userHand = StudentVideoWnd::USER_HAND_ASK;
        }
        else if(userInfo._nUserState == biz::eUserspeekstate_allow_speak)
        {
            userHand = StudentVideoWnd::USER_HAND_SPK;
        }

        auto pSpeakUser = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
        if(pSpeakUser && pSpeakUser->nUserId == userId)
        {
            userSpeak = true;
        }
    }

    videoWidget->setUserId(userId);
    videoWidget->setUserName(userName);
    videoWidget->setUserSpeak(userSpeak);
    videoWidget->setShowHand(userHand);
}

void StuVideoListWnd::onUpdateUserInfo(__int64 nUserId)
{
    StudentVideoWnd* videoWidget = getStudentVideoWnd(nUserId);
    if(NULL == videoWidget)
    {
        return;
    }

    setVideoWidget(videoWidget,nUserId);
}

void StuVideoListWnd::onUserSpeakChange(__int64 userId,bool speak)
{
    if(userId <= 0 || !ClassSeeion::GetInst()->IsTeacher())
    {
        return;
    }

    auto pSpeakUser = biz::GetBizInterface()->GetClassRoomDataContainer()->GetClassRoomInterface(ClassSeeion::GetInst()->_nClassRoomId)->GetSpeakUserInfo();
    if(speak)
    {		
        if(pSpeakUser)
        {
            if(pSpeakUser->nUserId == userId)
            {
                return;
            }

            StudentVideoWnd* videoWidget = getStudentVideoWnd(pSpeakUser->nUserId);
            if(NULL != videoWidget)
            {
                videoWidget->setUserSpeak(false);
            }

            //È¡Ïû·¢ÑÔ
            biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, pSpeakUser->nUserId, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
        }

        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userId, biz::eUserspeekstate_allow_speak, ClassSeeion::GetInst()->_nUserId);
    }
    else
    {
        if(pSpeakUser && pSpeakUser->nUserId != userId)
        {
            StudentVideoWnd* videoWidget = getStudentVideoWnd(userId);
            if(NULL != videoWidget)
            {
                videoWidget->setUserSpeak(false);
            }

            return;
        }

        biz::GetBizInterface()->UserClassAction(ClassSeeion::GetInst()->_nClassRoomId, userId, biz::eUserspeekstate_clean_speak, ClassSeeion::GetInst()->_nUserId);
    }
}

void StuVideoListWnd::onUserSpeakAction(UserSpeakActionInfo info)
{
    StudentVideoWnd* videoWidget = NULL;
    if(info._nUserId > 0)
    {
        videoWidget = getStudentVideoWnd(info._nUserId);
        if(NULL == videoWidget)
        {
            return;
        }	
    }

    switch(info._nAction)
    {
    case biz::eUserspeekstate_clean_speak:
        {
            if(info._nUserId > 0)
            {
                videoWidget->setUserSpeak(false);
                videoWidget->setShowHand(StudentVideoWnd::USER_HAND_NO);

                return;
            }

            for(int i=0 ; i<m_videoList.size();i++)
            {
                m_videoList[i]->setUserSpeak(false);
                m_videoList[i]->setShowHand(StudentVideoWnd::USER_HAND_NO);
            }
        }
        break;
    case biz::eUserspeekstate_allow_speak:
        {
            videoWidget->setUserSpeak(true);
            videoWidget->setShowHand(StudentVideoWnd::USER_HAND_SPK);
        }
        break;
    case biz::eUserspeekstate_cancel_Speak:
        {
            videoWidget->setShowHand(StudentVideoWnd::USER_HAND_NO);
        }
        break;
    case biz::eUserspeekstate_Ask_speak:
        {
            videoWidget->setShowHand(StudentVideoWnd::USER_HAND_ASK);
        }
        break;
    case biz::eClassAction_mute:
        {
            for(int i=0 ; i<m_videoList.size();i++)
            {				
                m_videoList[i]->setShowHand(StudentVideoWnd::USER_HAND_NO);
            }
        }
        break;
    }
}

bool StuVideoListWnd::eventFilter(QObject * obj, QEvent * event)
{
    if(NULL==obj||NULL==event)
    {
        return false;
    }

    if(event->type() == QEvent::MouseButtonDblClick)
    {
        StudentVideoWnd* wndVideo = dynamic_cast<StudentVideoWnd*>(obj);
        if(NULL==wndVideo)
        {
            return false;
        }

        __int64 uid = wndVideo->getUserId();
        onUserSpeakChange(uid,true);        

        return false;
    }

    return false;
}