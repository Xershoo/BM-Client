#include "chatmanager.h"
#include "src/chat/chat.h"
#include "src/chat/privatechatwidget.h"
#include "session/classsession.h"

ChatManager::ChatManager(QObject *parent) : QObject(parent),m_chatIdCount(0)
{
    connect(&m_upDownServer, SIGNAL(sendHttpResult(HttpResult,ChatUserHttpData)), this, SLOT(recvHttpResult(HttpResult,ChatUserHttpData)));
    connect(getBizCallBack(), SIGNAL(RecvClassMessage(biz::SLTextMsgInfo)), this, SLOT(recvMsgInfo(biz::SLTextMsgInfo)));
}

void ChatManager::recvMsgInfo(biz::SLTextMsgInfo info)
{
	QString msg =  QString::fromWCharArray(info._szMsg);  
	QMap<int,Chat*>::iterator it;
	//如果窗口已创建则直接显示
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++)
	{
		if(info._nMsgType == biz::eTestMsgType_class && (it).value()->getChatData().m_chatType == biz::eTestMsgType_class)
		{
			(it).value()->recvMsg(info);
			return;
		}
		else if(info._nMsgType == biz::eTestMsgType_group && (it).value()->getChatData().m_chatType == biz::eTestMsgType_group)
		{
			(it).value()->recvMsg(info);
			return;
		}
		else if(info._nMsgType == biz::eTestMsgType_user)
		{
			if((it).value()->getChatData().m_sendUserId == info._nRecvUserId 
				&& (it).value()->getChatData().m_chatType == biz::eTestMsgType_user
				&& (it).value()->getChatData().m_recvUserId == info._nSendUserId)
			{
				(it).value()->recvMsg(info);
				return;
			}
			else if((it).value()->getChatData().m_sendUserId == info._nSendUserId 
				&& (it).value()->getChatData().m_chatType == biz::eTestMsgType_user
				&& (it).value()->getChatData().m_recvUserId == info._nRecvUserId)
			{
				(it).value()->recvMsg(info);
				return;
			}
		}
	}
	//收到其他用户发来的信息创建新的窗口。
	Chat *privateChat = createChatObj();
	privateChat->setChatData(ChatData(ClassSeeion::GetInst()->_nUserId,info._nSendUserId,ClassSeeion::GetInst()->_nClassRoomId,biz::eTestMsgType_user,info._nGroupId));
	privateChat->setWidget(new PrivateChatWidget(),PRIVATE_CHAT);
	privateChat->getWidget()->show();
	privateChat->recvMsg(info);
}

void ChatManager::attach(Chat *chat)
{
	m_chatMap.insert(m_chatIdCount,chat);
	++m_chatIdCount;
}

void ChatManager::dettach(Chat *chat)
{
	QMap<int,Chat*>::iterator it;
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++)
	{
		if((it).value() == chat)
		{
			m_chatMap.erase(it);
			break;
		}
	}
}

void ChatManager::createPrivateChat(__int64 userID)
{
	QMap<int,Chat*>::iterator it;
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++)
	{
		if((it).value()->getChatData().m_chatType == biz::eTestMsgType_user 
			&& (it).value()->getChatData().m_sendUserId == ClassSeeion::GetInst()->_nUserId 
			&& (it).value()->getChatData().m_recvUserId == userID 
			)
		{
			(it).value()->getWidget()->show();
			return;
		}
	}

	Chat *privateChat = createChatObj();
	privateChat->setChatData(ChatData(ClassSeeion::GetInst()->_nUserId,userID,ClassSeeion::GetInst()->_nClassRoomId,biz::eTestMsgType_user,0));
	privateChat->setWidget(new PrivateChatWidget(),PRIVATE_CHAT);
	privateChat->getWidget()->show();
}

Chat* ChatManager::createChatObj()
{
	Chat* chat = new Chat(NULL,this);
	attach(chat);
	return chat;
}

bool ChatManager::downLoadFile(Chat *chat,const QString filePath)
{
	QMap<int,Chat*>::iterator it;
	bool isExits = false;
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++){
		if((it).value() == chat)
		{
			return m_upDownServer.downLoadFile(ChatUserHttpData((it).key(),filePath));
		}
	}
	return false;
}

bool ChatManager::upLoadFile(Chat *chat,const QString filePath)
{
	QMap<int,Chat*>::iterator it;
	bool isExits = false;
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++){
		if((it).value() == chat)
		{
			return m_upDownServer.upLoadFile(ChatUserHttpData((it).key(),filePath));
		}
	}
	return false;
}

void ChatManager::recvHttpResult(HttpResult httpResult,ChatUserHttpData userData)
{
	if(m_chatMap.contains(userData.m_chatId))
		m_chatMap[userData.m_chatId]->recvHttpResult(httpResult,userData);
}

void ChatManager::closeAllChat()
{
	QMap<int,Chat*>::iterator it;
	for(it = m_chatMap.begin(); it != m_chatMap.end();it++)
	{
		Chat * pChat = (it).value();
		if(NULL != pChat)
		{
			delete pChat;
			pChat = NULL;
		}
	}

	m_chatMap.clear();
}