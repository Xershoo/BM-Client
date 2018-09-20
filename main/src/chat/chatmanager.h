#ifndef CHATMANAGER_H
#define CHATMANAGER_H

#include <QObject>
#include "src/chat/chatmanager.h"
#include "src/chat/chatupdownserver.h"
#include "biz/BizInterface.h"
#include <QMap>

class Chat;
class PrivateChatWidget;

class ChatManager : public QObject
{
    Q_OBJECT
public:

    explicit ChatManager(QObject *parent = 0);

	bool downLoadFile(Chat *chat,const QString filePath);
	bool upLoadFile(Chat *chat,const QString filePath);

	Chat* createChatObj();									//创建聊天对象
	void createPrivateChat(__int64 userID);					//创建私聊对象

	void dettach(Chat *chat);
	
	void closeAllChat();
private slots:

    void recvMsgInfo(biz::SLTextMsgInfo info);
    void recvHttpResult(HttpResult httpResult,ChatUserHttpData userData);

private:

    QMap<int,Chat*> m_chatMap;
	int m_chatIdCount;
	ChatUpDownServer m_upDownServer;

	void attach(Chat *chat);
};

#endif // CHATMANAGER_H
