#ifndef WEB_CHAT_WIDGET
#define WEB_CHAT_WIDGET

#include <QtWidgets/QWidget>
#include "ClassWebBrowser.h"

class IChatWebBrowserCallback
{
public:
	virtual void onShowUserInfo(__int64 userId) = 0;
	virtual void onShowPicture(LPCWSTR pwszDate) = 0;
	virtual void onDocumentComplete() = 0;
};

class CChatWebBrowser : public WebBrowser
{
public:
	CChatWebBrowser(IChatWebBrowserCallback* pCallback);
	virtual ~CChatWebBrowser();

	virtual void setCallback(IChatWebBrowserCallback* pCallback);
public:
	virtual HRESULT _stdcall GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,unsigned int cNames,LCID lcid,DISPID FAR* rgDispId);
	virtual HRESULT _stdcall Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pDispParams,VARIANT FAR* pVarResult,EXCEPINFO FAR* pExcepInfo,unsigned int FAR* puArgErr);

protected:
	IChatWebBrowserCallback* m_pCallback;
};

class QWebChatWidget : public QWidget,public IChatWebBrowserCallback
{
	Q_OBJECT
public:
	QWebChatWidget(QWidget* parent);
	virtual ~QWebChatWidget();

public:
	virtual void onShowUserInfo(__int64 userId);
	virtual void onShowPicture(LPCWSTR pwszDate);
	virtual void onDocumentComplete();

	virtual void addSystemMsg(QString qstrSysMsg);
	virtual void addChatMsg(bool isMe,QString qstrUserName,QString qstrHeadImage,QString qstrMsg);
	virtual void clearAllMsg();
	virtual void delFrontMsg(int delMsgNum);
protected:
	void initWebBrowser();
	void unitWebBrowser();

protected:
	void resizeEvent(QResizeEvent *event);
	void timerEvent(QTimerEvent *event);

signals:
	void loadFinished();

protected:
	CChatWebBrowser*	m_chatWebBrowser;
	bool				m_initWebBrowser;
};

#endif