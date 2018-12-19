#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include "biz/BizInterface.h"
#include "src/chat/chatupdownserver.h"
#include "chatwidget.h"
#include <QVariant>

class ChatManager;
class QWidget;
class ChatWidget;

struct ChatData
{
	biz::ETextMsgType m_chatType;
	int m_groupId;
	__int64 m_sendUserId;
	__int64 m_classRoomId;
	__int64 m_recvUserId;
	ChatData(const ChatData&chat)
	{
		this->m_recvUserId = chat.m_recvUserId;
		this->m_sendUserId = chat.m_sendUserId;
		this->m_classRoomId = chat.m_classRoomId;
		this->m_groupId = chat.m_groupId;
		this->m_chatType = chat.m_chatType;
	}
	ChatData(__int64 sendUserId = -1,__int64 recvUserId = -1,__int64 classRoomId = -1,
		biz::ETextMsgType chatType = biz::eTestMsgType_class,int groupId = -1):
	m_sendUserId(sendUserId),m_classRoomId(classRoomId),m_recvUserId(recvUserId),m_chatType(chatType),m_groupId(groupId)
	{
	}
};

class Chat : public QObject
{
    Q_OBJECT
public:
	explicit Chat(QObject *parent = 0,ChatManager *chatManager = NULL,const ChatData &chatData = ChatData());
	virtual ~Chat();

    virtual void recvMsg(biz::SLTextMsgInfo info);
    virtual void recvHttpResult(HttpResult httpResult,ChatUserHttpData userData);

	void setChatData(const ChatData &chatData = ChatData());
	ChatData getChatData()const;
	QWidget* getWidget()const;
	void setWidget(QWidget* widget,ChatType type);
    
protected:
    ChatWidget *getChatWidget();
	void showChatMsg(const QString& msgHtml,__int64 sendUserId,__int64 userId);

private slots:
	void sendMsg(QString msg);
	void destoryMyself();

private:
    Chat(const Chat&){}
    void sendMsgInfo(const QString &msg);

protected:
	ChatManager *m_chatManager;
	biz::SLTextMsgInfo m_sMsgInfo;
	ChatData m_chatData;
	QWidget* m_widget;
    int m_chatType;
};

#endif // CHAT_H
