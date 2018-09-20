#include "userlistwidget.h"
#include "userlistitemwidget.h"
#include "common/Env.h"
#include <QContextMenuEvent>
#include <QMenu>
#include "../session/classsession.h"
#include "BizInterface.h"

UserListWidget::UserListWidget(QWidget *parent)
    : QListWidget(parent), m_userListMenue(NULL)
{
}

UserListWidget::~UserListWidget()
{
	if (m_userListMenue)
	{
		delete m_userListMenue;
		m_userListMenue = NULL;
	}
}

bool UserListWidget::isTriggeredSelf(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
        return ClassSeeion::GetInst()->_nUserId == widget->userID();

    return true;
    
}

bool UserListWidget::isTriggeredTea(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
        return widget->isTeacher();

    return true;
}

bool UserListWidget::isTriggeredStu(const QPoint &pt)
{
    return !isTriggeredTea(pt);
}

bool UserListWidget::isTriggeredItemSpeaking(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
    {
        __int64 userID = widget->userID();
        auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
        return sInfo._nUserState & biz::eUserState_speak;
    }

    return true;
}

bool UserListWidget::isTriggeredItemHandsup(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
    {
        __int64 userID = widget->userID();
        auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
        return sInfo._nUserState & biz::eUserState_ask_speak;
    }

    return true;
}

bool UserListWidget::isTriggeredItemDisableSpeaking(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
    {
        __int64 userID = widget->userID();
        auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
        return sInfo._nUserState & biz::eUserState_user_mute;
    }

    return true;
}

bool UserListWidget::isTriggeredItemAssistant(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
    {
        __int64 userID = widget->userID();
        auto sInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(userID);
        return sInfo.nUserAuthority == biz::UserAu_Assistant;
    }

    return true;
}

__int64 UserListWidget::triggeredItemUserID(const QPoint &pt)
{
    QListWidgetItem *item = itemAt(pt);
    if (UserListItemWidget *widget = qobject_cast<UserListItemWidget*>(itemWidget(item)))
        return widget->userID();

    return 0;
}

void UserListWidget::contextMenuEvent(QContextMenuEvent *e)
{
    if (!itemAt(e->pos()) || isTriggeredSelf(e->pos()))
        return;

	if (!m_userListMenue)
	{
		m_userListMenue = new UserListMenue();
		connect(m_userListMenue, SIGNAL(sg_sendClickObj(int)), this, SLOT(sl_recvClickObj(int)));
	}

	m_userListMenue->setData(triggeredItemUserID(e->pos()));

    //老师点学生
    if (ClassSeeion::GetInst()->IsTeacher() && isTriggeredStu(e->pos()))
    {
		if (isTriggeredItemSpeaking(e->pos()))
			m_userListMenue->setAskBtnText(tr("cancel speaking"));
        else if (isTriggeredItemHandsup(e->pos()))
            m_userListMenue->setAskBtnText(tr("allow speaking"));
        else
            m_userListMenue->setAskBtnText(tr("ask"));

        if (isTriggeredItemAssistant(e->pos()))
			m_userListMenue->setPromotedToAssistantBtnText(tr("cancel promoted to assistant"));
        else
			m_userListMenue->setPromotedToAssistantBtnText(tr("promoted to assistant"));

        if (isTriggeredItemDisableSpeaking(e->pos()))
            m_userListMenue->setDisableSpeakBtnText(tr("cancel disable speak"));
        else
            m_userListMenue->setDisableSpeakBtnText(tr("disable speak"));
    }
    
    //学生点老师
    if (ClassSeeion::GetInst()->IsStudent() && isTriggeredTea(e->pos()))
    {
		m_userListMenue->setMenueType(STUDENT_TO_TEACHER,ClassSeeion::GetInst()->_bBeginedClass);
		m_userListMenue->setFixedHeight(68);
    }

    //学生点学生
    if (ClassSeeion::GetInst()->IsStudent() && isTriggeredStu(e->pos()))
    {
		m_userListMenue->setMenueType(STUDENT_TO_TEACHER,ClassSeeion::GetInst()->_bBeginedClass);
		m_userListMenue->setFixedHeight(68);
    }

	//老师点学生
	if (ClassSeeion::GetInst()->IsTeacher() && isTriggeredStu(e->pos()))
	{
		m_userListMenue->setMenueType(TEACHER_TO_STUDENT,ClassSeeion::GetInst()->_bBeginedClass);
		m_userListMenue->setFixedHeight(180);
	}

	m_userListMenue->setGeometry(QRect(e->globalPos(), m_userListMenue->size()));
	m_userListMenue->show();
}

void UserListWidget::sl_recvClickObj(int obj)
{
	switch (obj)
	{
	case ASK:
		askActionTriggered(m_userListMenue->getData());
		break;
	case CHAT_WITH:
		chatWithActionTriggered(m_userListMenue->getData());
		break;
	case DISABEL_SPEAK:
		disableSpeakActionTriggered(m_userListMenue->getData());
		break;
	case PROMOTED_TO_ASSISTANT:
		promotedToAssistant(m_userListMenue->getData());
		break;
	case KICKOUT:
		kickoutActionTriggered(m_userListMenue->getData());
		break;
	case VIEW_PROFILE:
		viewProfileActionTriggered(m_userListMenue->getData());
		break;
	}
}