#ifndef USERLISTMENUE_H
#define USERLISTMENUE_H

#include <QWidget>
#include "ui_userlistmenue.h"
#include "control/c8popupwindow.h"

enum MenueType{
	TEACHER_TO_STUDENT,
	STUDENT_TO_TEACHER,
	STUDENT_TO_STUDENT
};

enum ClickObjectId{
	VIEW_PROFILE,
	ASK,
	DISABEL_SPEAK,
	KICKOUT,
	CHAT_WITH,
	PROMOTED_TO_ASSISTANT
};

class UserListMenue : public C8PopupWindow
{
	Q_OBJECT

signals:
	void sg_sendClickObj(int id);

public:
	UserListMenue(QWidget *parent = 0);
	~UserListMenue();

	void setMenueType(MenueType menueType,bool inClassroom = true);
	void setAskBtnText(const QString &text);
	void setPromotedToAssistantBtnText(const QString &text);
	void setDisableSpeakBtnText(const QString &text);
	void setData(__int64 userID);
	__int64 getData()const;

private:
	Ui::UserListMenue ui;
	__int64 m_userId;
private slots:
	void onViewProfileClicked();
	void onAskClicked();;
	void onDisableSpeakClicked();
	void onKickoutClicked();
	void onChatWithClicked();
	void onPromotedToAssistantClicked();
};

#endif // USERLISTMENUE_H
