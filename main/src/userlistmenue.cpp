#include "userlistmenue.h"

UserListMenue::UserListMenue(QWidget *parent)
	: C8PopupWindow(parent,""),m_userId(0)
{
	ui.setupUi(this);
	
	connect(ui.promotedToAssistant_Btn, SIGNAL(clicked()), this, SLOT(onPromotedToAssistantClicked()));
	connect(ui.ask_Btn, SIGNAL(clicked()), this, SLOT(onAskClicked()));
	connect(ui.chatWith_Btn, SIGNAL(clicked()), this, SLOT(onChatWithClicked()));
	connect(ui.disableSpeak_Btn, SIGNAL(clicked()), this, SLOT(onDisableSpeakClicked()));
	connect(ui.kickout_Btn, SIGNAL(clicked()), this, SLOT(onKickoutClicked()));
	connect(ui.viewProfile_Btn, SIGNAL(clicked()), this, SLOT(onViewProfileClicked()));

    ui.promotedToAssistant_Btn->hide();
}

UserListMenue::~UserListMenue()
{

}

void UserListMenue::onViewProfileClicked()
{
    this->hide();
	emit sg_sendClickObj(VIEW_PROFILE);
}

void UserListMenue::onAskClicked()
{
    this->hide();
	emit sg_sendClickObj(ASK);
}

void UserListMenue::onDisableSpeakClicked()
{
    this->hide();
	emit sg_sendClickObj(DISABEL_SPEAK);
}

void UserListMenue::onKickoutClicked()
{
    this->hide();
	emit sg_sendClickObj(KICKOUT);
}

void UserListMenue::onChatWithClicked()
{
    this->hide();
	emit sg_sendClickObj(CHAT_WITH);
}

void UserListMenue::onPromotedToAssistantClicked()
{
    this->hide();
	emit sg_sendClickObj(PROMOTED_TO_ASSISTANT);
}

void UserListMenue::setMenueType(MenueType menueType,bool inClassroom /* = true */)
{   
	switch (menueType)
	{
		case TEACHER_TO_STUDENT:
		{	
			ui.promotedToAssistant_Btn->setEnabled(inClassroom);
			ui.ask_Btn->setEnabled(inClassroom);
			ui.disableSpeak_Btn->setEnabled(inClassroom);
			
			//xiewb 2018.09.10
			ui.kickout_Btn->hide();
			ui.disableSpeak_Btn->hide();
			ui.line_userListMenue_1->hide();
			break;
		}
		case STUDENT_TO_STUDENT:
		case STUDENT_TO_TEACHER:
		{
			ui.promotedToAssistant_Btn->hide();
			ui.ask_Btn->hide();
			ui.disableSpeak_Btn->hide();
			ui.kickout_Btn->hide();
			ui.line_userListMenue_1->hide();
			ui.line_userListMenue_2->hide();
			break;
		}	
	}
}

void UserListMenue::setAskBtnText(const QString &text)
{
	ui.ask_Btn->setText(text);
}
void UserListMenue::setPromotedToAssistantBtnText(const QString &text)
{
	ui.promotedToAssistant_Btn->setText(text);
}
void UserListMenue::setDisableSpeakBtnText(const QString &text)
{
	ui.disableSpeak_Btn->setText(text);
}

void UserListMenue::setData(__int64 userID)
{
	m_userId = userID;
}

__int64 UserListMenue::getData()const
{
	return m_userId;
}