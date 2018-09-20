#include "chat.h"
#include "common/Env.h"
#include "common/Util.h"
#include <time.h>
#include <QString>
#include <QWebView>
#include <QWebElementCollection>
#include <QWebPage>
#include <QWebFrame>
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
    std::string strContent;
    Util::QStringToString(msg, strContent);
    int size = strContent.length();
    if (size > (MAXTEXTLEN-1))
    {
        if (ChatWidget *widget = getChatWidget())
            widget->showSysMsg(tr("input too long"));

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
        if (ChatWidget *widget = getChatWidget())
            widget->showSysMsg(tr("input too long"));
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
	QWebView webView;
	webView.setHtml(strHtml);

    QWebElementCollection collectionBody = webView.page()->currentFrame()->findAllElements("body");
    for (int i = 0; i < collectionBody.count(); i++)
    {
        collectionBody.at(i).removeAttribute("style");
    }

    QWebElementCollection collectionSpan = webView.page()->currentFrame()->findAllElements("span");
    for (int i = 0; i < collectionSpan.count(); i++)
    {
        collectionSpan.at(i).removeAttribute("style");
    }

    QWebElementCollection collectionStyle = webView.page()->currentFrame()->findAllElements("p");
    for (int i = 0; i < collectionStyle.count(); i++)
    {
        collectionStyle.at(i).removeAttribute("style");
    }

    QWebElementCollection collectionBR = webView.page()->currentFrame()->findAllElements("br");
    for (int i = 0; i < collectionBR.count(); i++)
    {
        collectionBR.at(i).replace("\n");
    }
	QWebElementCollection collection = webView.page()->currentFrame()->findAllElements("img");
	for (int i = 0; i < collection.count(); ++i)
	{
		QString src = collection.at(i).attribute("src","");
        int pos = src.lastIndexOf(":/");
		if (pos != 0)
		{
            if (src.indexOf("qrc:/") == 0)
            {
                src = src.right(src.length() - 3);
                collection.at(i).setAttribute("src", src);
            }
            else
            {
                collection.at(i).removeAttribute("style");
                //src = src.right(src.length() - pos - 1);
                QString md5 = "";
                Util::CaleFileMd5(src,md5);
                collection.at(i).setAttribute("src",md5+'.'+QFileInfo(src).suffix());
                if(ChatUpDownServer::copyFileToMD5Path(src))
                    m_chatManager->upLoadFile(this,src);
            }
		}
	}
	strHtml = webView.page()->currentFrame()->toHtml();
	strHtml = strHtml.remove(0, strHtml.indexOf("<body"));
	strHtml = strHtml.left(strHtml.lastIndexOf("</html>"));

	sendMsgInfo(strHtml);
}

void Chat::recvMsg(biz::SLTextMsgInfo info)
{
	QString html =  QString::fromWCharArray(info._szMsg);
    QString content = "";

    QWebView webView;
    webView.setHtml(html);
    QWebElementCollection collection = webView.page()->currentFrame()->findAllElements("p");
    for (int i = 0; i < collection.count(); i++)
    {
        QString x = collection.at(i).toInnerXml();
        QWebElementCollection imgs = collection.at(i).findAll("img");
        int j = imgs.count();
        for (j = 0; j < imgs.count(); j++)
        {
            QString img = imgs.at(j).toOuterXml();
            QString imagePath = imgs.at(j).attribute("src");
            QString imgR;
            if (imagePath.indexOf(":/") != 0)
            {
                m_chatManager->downLoadFile(this,imagePath);
                QString picCachePath = Env::GetUserPicCachePath(m_chatData.m_sendUserId) + imagePath;
                picCachePath.replace("\\","/");
                QImage img(picCachePath);                        
                imagePath = "file:///" + picCachePath;
                if (img.size().width() >= 60 || img.size().height() <= 60)
                    imgR = QString("<img src=\"%1\" onClick=\"toShowBigPic(this)\" width=\"60\" height=\"60\"/>").arg(imagePath);
                else
                    imgR = QString("<img src=\"%1\" onClick=\"toShowBigPic(this)\"/>").arg(imagePath);
            }
            else
            {
                imgR = QString("<img src=\"qrc%1\" />").arg(imagePath);
            }

            x.replace(img, imgR);
        }        
        content += x;
         if (x != "\n")
             content += "\n";
    }  
    content.replace("\n","<br />");
	Msg msg;
	msg.content = content;
	msg._nSendUserId = info._nSendUserId;
	msg._nUserId = m_chatData.m_sendUserId;
	QVariant var;
	var.setValue(msg);
	emit recvMsgSignal(var);
}

void Chat::recvHttpResult(HttpResult httpResult,ChatUserHttpData userData)
{
	emit refresh();
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
		connect(this, SIGNAL(refresh()), ((PrivateChatWidget*)m_widget)->getChatWidget(), SLOT(refresh()));
		connect(this, SIGNAL(recvMsgSignal(QVariant)), ((PrivateChatWidget*)m_widget)->getChatWidget(), SLOT(recvMsg(QVariant)));
		connect(m_widget, SIGNAL(sg_destory()), this, SLOT(destoryMyself()));
		SLUserInfo userInfo = biz::GetBizInterface()->GetUserInfoDataContainer()->GetUserInfoById(m_chatData.m_recvUserId);
		((PrivateChatWidget*)m_widget)->setTitleText(QString::fromWCharArray(userInfo.szRealName));
		break;
		}
	case CLASS_CHAT:
		{
		connect((ChatWidget*)m_widget, SIGNAL(sg_sendMsg(QString)), this, SLOT(sendMsg(QString)));
		connect(this, SIGNAL(refresh()), (ChatWidget*)m_widget, SLOT(refresh()));
		connect(this, SIGNAL(recvMsgSignal(QVariant)), (ChatWidget*)m_widget, SLOT(recvMsg(QVariant)));
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