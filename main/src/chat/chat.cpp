#include "chat.h"
#include "common/Env.h"
#include "common/Util.h"
#include <time.h>
#include <QString>
#include <QDomDocument>
#include <QFileInfo>
#include <QXmlStreamReader>
#include "src/chat/chatmanager.h"
#include "chatwidget.h"
#include "privatechatwidget.h"
#include <QWidget>

Chat::Chat(QObject *parent,ChatManager *chatManager,const ChatData &chatData) 
	: QObject(parent),m_chatManager(chatManager),m_chatData(chatData),m_widget(NULL)
{
	m_sMsgInfo._nClassRoomId = chatData.m_classRoomId;
	m_sMsgInfo._nSendUserId = chatData.m_sendUserId;
	m_sMsgInfo._nRecvUserId = chatData.m_recvUserId;
    m_sMsgInfo._nMsgType = (biz::ETextMsgType)chatData.m_chatType;
    m_sMsgInfo._nGroupId = chatData.m_groupId;
	m_sMsgInfo._nTimer = time(NULL);
    m_sMsgInfo._nMsgLen = 0;
	qRegisterMetaType<QVariant>("QVariant"); 
}

void Chat::sendMsgInfo(const QString &msg)
{
	ChatWidget *chatWidget = getChatWidget();
    
	std::string strContent;
    Util::QStringToString(msg, strContent);
    int size = strContent.length();
    if (size > (MAXTEXTLEN-1))
    {
        if (NULL!= chatWidget){
            chatWidget->showSysMsg(tr("input too long"));
		}
        return;
    }
	int nLen = msg.length();
	nLen = (nLen * 2) + 1;
	char *chMgs = new char[nLen];
	wchar_t *wchMgs = new wchar_t[nLen];
	
	memset(chMgs,0,sizeof(char)*nLen);
	memset(wchMgs,0,sizeof(wchar_t)*nLen);
	
	Util::QStringToAnsi(msg,chMgs,nLen);
	Util::AnsiToUnicode(chMgs,nLen,wchMgs,nLen);
	
	int wchMgsLen = wcslen(wchMgs);
    if (wchMgsLen > (MAXTEXTLEN-1))
    {
		if (NULL!= chatWidget){
			chatWidget->showSysMsg(tr("input too long"));
		}
    }
    else
    {
        m_sMsgInfo._nMsgLen = wchMgsLen;
        memcpy(m_sMsgInfo._szMsg,wchMgs, wchMgsLen*sizeof(wchar_t));
        biz::GetBizInterface()->SendTextMessage(m_sMsgInfo);
    }	

	delete []chMgs;
	delete []wchMgs;	
}

void Chat::sendMsg(QString str)
{
	QString strHtml = str;
	strHtml = strHtml.remove(0, strHtml.indexOf("<body"));
	strHtml = strHtml.left(strHtml.lastIndexOf("</html>"));

	QDomDocument docDom;
	docDom.setContent(strHtml);

	QDomNodeList nodeList = docDom.elementsByTagName("body");
	for(int i=0;i<nodeList.count();i++){
		QDomNode node = nodeList.at(i);
		if(!node.hasAttributes()){
			continue;
		}
		node.attributes().removeNamedItem(QString("style"));
	}

	nodeList = docDom.elementsByTagName("span");
	for(int i=0;i<nodeList.count();i++){
		QDomNode node = nodeList.at(i);
		if(!node.hasAttributes()){
			continue;
		}
		node.attributes().removeNamedItem(QString("style"));
	}

	nodeList = docDom.elementsByTagName("p");
	for(int i=0;i<nodeList.count();i++){
		QDomNode node = nodeList.at(i);
		if(!node.hasAttributes()){
			continue;
		}
		node.attributes().removeNamedItem(QString("style"));
	}

	nodeList = docDom.elementsByTagName("img");
	for(int i=0;i<nodeList.count();i++){
		QDomNode node = nodeList.at(i);
		if(!node.hasAttributes()){
			continue;
		}

		QDomNamedNodeMap attriMap = node.attributes();
		if(!attriMap.contains(QString("src"))){
			continue;
		}

		QString imgSrc = attriMap.namedItem(QString("src")).toAttr().value();
		if(imgSrc.indexOf(":/") == 0){
			attriMap.namedItem(QString("src")).toAttr().setValue(QString("qrc")+imgSrc);
		}else{
			node.attributes().removeNamedItem(QString("style"));
			QString md5 = "";
			Util::CaleFileMd5(imgSrc,md5);
			attriMap.namedItem(QString("src")).toAttr().setValue(md5+'.'+QFileInfo(imgSrc).suffix());
			if(ChatUpDownServer::copyFileToMD5Path(imgSrc))
				m_chatManager->upLoadFile(this,imgSrc);
		}
	}

	strHtml = docDom.toString();
	strHtml = strHtml.remove(0, QString("<body>").length());
	strHtml = strHtml.left(strHtml.lastIndexOf("</body>"));
	sendMsgInfo(strHtml);

	showChatMsg(strHtml,m_sMsgInfo._nSendUserId,m_sMsgInfo._nSendUserId);
	return;
}

void Chat::showChatMsg(const QString& msgHtml,__int64 sendUserId,__int64 userId)
{
	if(msgHtml.isNull()||msgHtml.isEmpty()){
		return;
	}

	ChatWidget *chatWidget = getChatWidget();
	if(NULL==chatWidget){
		return;
	}

	QString contentHtml = QString("<body>%1</body>").arg(msgHtml);
	QDomDocument docDom;
	docDom.setContent(contentHtml);

	QDomNodeList nodeList = docDom.elementsByTagName("img");
	for(int i=0;i<nodeList.count();i++){
		QDomNode node = nodeList.at(i);
		if(!node.hasAttributes()){
			continue;
		}

		QDomNamedNodeMap attriMap = node.attributes();
		if(!attriMap.contains(QString("src"))){
			continue;
		}

		QString imgSrc = attriMap.namedItem(QString("src")).toAttr().value();
		if(imgSrc.indexOf("qrc:/") ==0){
			continue;
		}
		QString picCachePath = Env::GetUserPicCachePath(m_chatData.m_sendUserId) + imgSrc;
		picCachePath.replace("\\","/");
		
		if(!Util::isFileExists(picCachePath)){
			m_chatManager->downLoadFile(this,imgSrc);
		}

		imgSrc = QString("file:///") + picCachePath;
		attriMap.namedItem(QString("src")).toAttr().setValue(imgSrc);

		if(!attriMap.contains(QString("height"))){
			QDomAttr attrNode = docDom.createAttribute("height");
			attrNode.setValue("60");
			attriMap.setNamedItem(attrNode);
		}
	}

	contentHtml = docDom.toString();
	contentHtml = contentHtml.remove(0,contentHtml.lastIndexOf("<body>")+QString("<body>").length());
	contentHtml = contentHtml.left(contentHtml.indexOf("</body>"));

	CHAT_MSG msg;
	msg.content = contentHtml;
	msg._nSendUserId = sendUserId;
	msg._nUserId = userId;

	chatWidget->recvMsg(msg);
}

void Chat::recvMsg(biz::SLTextMsgInfo info)
{
	QString html =  QString::fromWCharArray(info._szMsg);
	showChatMsg(html,info._nSendUserId,m_chatData.m_sendUserId);
	
	return;
}

void Chat::recvHttpResult(HttpResult httpResult,ChatUserHttpData userData)
{
}

Chat::~Chat()
{
	if(NULL != m_widget)
	{
		if(m_widget->isWindow())
		{
			m_widget->close();
		}
		m_widget = NULL;
	}
}

void Chat::setChatData(const ChatData &chatData)
{
	m_chatData = chatData;
	m_sMsgInfo._nClassRoomId = chatData.m_classRoomId;
	m_sMsgInfo._nSendUserId = chatData.m_sendUserId;
	m_sMsgInfo._nRecvUserId = chatData.m_recvUserId;
	m_sMsgInfo._nMsgType = (biz::ETextMsgType)chatData.m_chatType;
	m_sMsgInfo._nGroupId = chatData.m_groupId;
}

ChatData Chat::getChatData()const
{
	return m_chatData;
}

QWidget* Chat::getWidget()const
{
	return m_widget;
}

void Chat::setWidget(QWidget* widget,ChatType type)
{
	m_widget = widget;

	if(NULL == m_widget)
	{
		return;
	}
    m_chatType = type;
	switch (type)
	{
	case PRIVATE_CHAT:
		{
			connect(((PrivateChatWidget*)m_widget)->getChatWidget(), SIGNAL(sg_sendMsg(QString)), this, SLOT(sendMsg(QString)));
			connect(m_widget, SIGNAL(sg_destory()), this, SLOT(destoryMyself()));
			SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(m_chatData.m_recvUserId);
			((PrivateChatWidget*)m_widget)->setTitleText(QString::fromWCharArray(userInfo.szRealName));
			break;
		}
	case CLASS_CHAT:
		{
			connect((ChatWidget*)m_widget, SIGNAL(sg_sendMsg(QString)), this, SLOT(sendMsg(QString)));
			break;
		}
	}
}

ChatWidget *Chat::getChatWidget()
{
    if (m_chatType == PRIVATE_CHAT)
    {
        if (PrivateChatWidget* widget = qobject_cast<PrivateChatWidget*>(m_widget))
        {
            return widget->getChatWidget();
        }
    }
    else if (m_chatType == CLASS_CHAT)
    {
        if (ChatWidget *widhet = qobject_cast<ChatWidget*>(m_widget))
        {
            return widhet;
        }
    }
    return NULL;
}

void Chat::destoryMyself()
{
	m_widget = NULL;
	m_chatManager->dettach(this);
	delete this;
}