#include "WebChatWidget.h"
#include <QtCore/QTimerEvent>

//////////////////////////////////////////////////////////////////////////

#define DISPID_EXTERNAL_METHOD_SHOW_PICTURE			(40001)
#define DISPID_EXTERNAL_METHOD_SHOW_USERINFO		(40002)

CChatWebBrowser::CChatWebBrowser(IChatWebBrowserCallback* pCallback):m_pCallback(pCallback)
{

}

CChatWebBrowser::~CChatWebBrowser()
{
	Uninitialize();
}

void CChatWebBrowser::setCallback(IChatWebBrowserCallback* pCallback)
{
	m_pCallback = pCallback;
}

HRESULT _stdcall CChatWebBrowser::GetIDsOfNames(REFIID riid, 
	OLECHAR FAR* FAR* rgszNames, 
	unsigned int cNames, 
	LCID lcid, 
	DISPID FAR* rgDispId )
{
	if(lstrcmp(rgszNames[0],L"showPicture") == 0) 
	{
		*rgDispId = DISPID_EXTERNAL_METHOD_SHOW_PICTURE;
		return S_OK;
	};

	if(lstrcmp(rgszNames[0],L"showUserInfo") == 0) 
	{
		*rgDispId = DISPID_EXTERNAL_METHOD_SHOW_USERINFO;
		return S_OK;
	};

	return WebBrowser::GetIDsOfNames(riid,rgszNames,cNames,lcid,rgDispId);
}

HRESULT _stdcall CChatWebBrowser::Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS* pDispParams,
	VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo,
	unsigned int* puArgErr)
{   
	if ( dispIdMember == DISPID_EXTERNAL_METHOD_SHOW_PICTURE)
	{		
		if(m_pCallback && pDispParams->cArgs >= 1)
		{
			__int64 nUserId = 0;
			nUserId = pDispParams->rgvarg[0].ulVal;
			
			m_pCallback->onShowUserInfo(nUserId);
		}
		return S_OK;
	};	

	if ( dispIdMember == DISPID_EXTERNAL_METHOD_SHOW_PICTURE)
	{		
		if(m_pCallback && pDispParams->cArgs >= 1)
		{
			LPWSTR pwszDate = (LPWSTR)pDispParams->rgvarg[0].bstrVal;

			m_pCallback->onShowPicture((LPCWSTR)pwszDate);
		}
		return S_OK;
	};

	if( dispIdMember == DISPID_NAVIGATECOMPLETE2)
	{		
		WCHAR wszUrl[MAX_PATH] = { 0 };
		wcscpy_s(wszUrl,MAX_PATH,pDispParams->rgvarg[0].pvarVal->bstrVal);
		if(wcsicmp(wszUrl, L"about:blank") != 0 && NULL != m_pCallback)
		{
			m_pCallback->onDocumentComplete();
		}
	}

	return WebBrowser::Invoke(dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr);
}

//////////////////////////////////////////////////////////////////////////
QWebChatWidget::QWebChatWidget(QWidget* parent):QWidget(parent)
	,m_chatWebBrowser(NULL)
	,m_initWebBrowser(false)
{
	initWebBrowser();
}

QWebChatWidget::~QWebChatWidget()
{
	unitWebBrowser();
}

void QWebChatWidget::initWebBrowser()
{
	if(NULL!=m_chatWebBrowser)
	{
		return;
	}

	m_chatWebBrowser = new CChatWebBrowser(this);
	if(NULL==m_chatWebBrowser)
	{
		return;
	}

	BOOL br = m_chatWebBrowser->Initialize();
	if(!br)
	{
		return;
	}

	m_initWebBrowser = m_chatWebBrowser->OpenWebBrowser((HWND)this->winId());
	if(!m_initWebBrowser)
	{
		return;
	}

	CComVariant varUrl((LPCWSTR)L"http://www.bmclass.com/chat.html");
	m_chatWebBrowser->OpenURL(&varUrl);
}

void QWebChatWidget::unitWebBrowser()
{
	if(!m_initWebBrowser)
	{
		return;
	}

	m_initWebBrowser = false;

	if(NULL==m_chatWebBrowser)
	{
		return;
	}

	m_chatWebBrowser->Uninitialize();
	m_chatWebBrowser->setCallback(NULL);
	m_chatWebBrowser = NULL;
}

void QWebChatWidget::onShowPicture(LPCWSTR pwszDate)
{

}

void QWebChatWidget::onShowUserInfo(__int64 userId)
{

}

void QWebChatWidget::onDocumentComplete()
{
	emit loadFinished();
}

void QWebChatWidget::resizeEvent(QResizeEvent *event)
{
	QRect rcWidget = this->rect();
	RECT rtWeb = { 1,1,rcWidget.width() - 2,rcWidget.height()-2};

	if(m_chatWebBrowser)
	{
		m_chatWebBrowser->SetWebRect(&rtWeb);
	}

	QWidget::resizeEvent(event);
}

void QWebChatWidget::timerEvent(QTimerEvent *event)
{
	if(NULL== event)
	{
		return;
	}
}

void QWebChatWidget::addChatMsg(bool isMe,QString qstrUserName,QString qstrHeadImage,QString qstrMsg)
{
	if(NULL==m_chatWebBrowser){
		return;
	}

	WCHAR wszUserName[1024]={0};
	WCHAR wszHeadImage[1024]={0};
	WCHAR wszChatMsg[1024]={0};

	qstrUserName.toWCharArray(wszUserName);
	qstrHeadImage.toWCharArray(wszHeadImage);
	qstrMsg.toWCharArray(wszChatMsg);

	CComVariant varParam[3]={CComVariant(wszChatMsg),CComVariant(wszHeadImage),CComVariant(wszUserName)};
	if(isMe){
		m_chatWebBrowser->CallWebScriptFunction(L"insertRight",varParam,3);
	}else{
		m_chatWebBrowser->CallWebScriptFunction(L"insertLeft",varParam,3);
	}
}

void QWebChatWidget::addSystemMsg(QString qstrSysMsg)
{
	if(NULL==m_chatWebBrowser){
		return;
	}

	WCHAR wszSysMsg[1024]={0};
	
	qstrSysMsg.toWCharArray(wszSysMsg);

	CComVariant varParam[1]={CComVariant(wszSysMsg)};
	m_chatWebBrowser->CallWebScriptFunction(L"insertSysMsg",varParam,1);
}

void QWebChatWidget::clearAllMsg()
{
	if(NULL==m_chatWebBrowser){
		return;
	}

	m_chatWebBrowser->CallWebScriptFunction(L"clearAll");
}

void QWebChatWidget::delFrontMsg(int delMsgNum)
{
	if(NULL==m_chatWebBrowser){
		return;
	}

	WCHAR wszDelMsgNum[1024]={0};
	_itow(delMsgNum,wszDelMsgNum,10);

	CComVariant varParam[1]={CComVariant(wszDelMsgNum)};
	m_chatWebBrowser->CallWebScriptFunction(L"clearMsg",varParam,1);
}