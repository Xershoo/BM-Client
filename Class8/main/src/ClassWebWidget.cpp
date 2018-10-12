#include "ClassWebWidget.h"
#include <QResizeEvent>

//////////////////////////////////////////////////////////////////////////
QClassWebWidget::QClassWebWidget(QWidget* parent/* =NULL */):QWidget(parent)	
	, m_webBrowser(NULL)
	, m_initWebBrowser(FALSE)
	, m_showTimer(0)
{
	initWebBrowser();	
}

QClassWebWidget::~QClassWebWidget()
{
	unitWebBrowser();
}

void QClassWebWidget::initWebBrowser()
{
	if(NULL!=m_webBrowser)
	{
		return;
	}
	
	m_webBrowser = new CClassWebBrowser();
	if(NULL==m_webBrowser)
	{
		return;
	}

	BOOL br = m_webBrowser->Initialize();
	if(!br)
	{
		return;
	}

	m_initWebBrowser = m_webBrowser->OpenWebBrowser((HWND)this->winId());
	if(!m_initWebBrowser)
	{
		return;
	}

	openUrl(QString("about:blank"));

	setCallback(this);
}

void QClassWebWidget::unitWebBrowser()
{
	if(!m_initWebBrowser)
	{
		return;
	}

	m_initWebBrowser = false;

	if(NULL==m_webBrowser)
	{
		return;
	}

	m_webBrowser->Uninitialize();
	m_webBrowser->SetCallback(NULL);
//	delete m_webBrowser;
	m_webBrowser = NULL;
}

void QClassWebWidget::resizeEvent(QResizeEvent *event)
{
	QRect rcWidget = this->rect();
	RECT rtWeb = { 1,1,rcWidget.width() - 2,rcWidget.height()-2};
	
	if(m_webBrowser)
	{
		m_webBrowser->SetWebRect(&rtWeb);
	}

	QWidget::resizeEvent(event);
}

void QClassWebWidget::timerEvent(QTimerEvent *event)
{
	if(NULL== event)
	{
		return;
	}

	int tid = event->timerId();
	if(tid==m_showTimer)
	{
		killTimer(m_showTimer);
		m_webBrowser->CallWebScriptFunction(L"setWindowExternal");
		emit webPageloadFinished();
	}
}

void QClassWebWidget::openUrl(QString& strUrl)
{
	if(!m_initWebBrowser)
	{
		return;
	}

	if(NULL==m_webBrowser)
	{
		return;
	}
	
	WCHAR wszUrl[1024] = { 0 };
	strUrl.toWCharArray(wszUrl);

	CComVariant varUrl((LPCWSTR)wszUrl);
	m_initWebBrowser = m_webBrowser->OpenURL(&varUrl);	
}

void QClassWebWidget::setCallback(IClassCallback* pCallBack)
{
	if(m_webBrowser)
	{
		m_webBrowser->SetCallback(pCallBack);
	}
}

void QClassWebWidget::onDocumentComplete()
{	
	m_showTimer = startTimer(500);	
}

void QClassWebWidget::onEnterClassRoom(__int64 nClassId,__int64 nCourseId)
{
	QString strCourseId = QString("%1").arg(nCourseId);
	QString strClassId = QString("%1").arg(nClassId);

	emit enterClassroom(strCourseId,strClassId);
}

void QClassWebWidget::onPlayClassRoom(__int64 nClassId,__int64 nCourseId)
{
	QString strCourseId = QString("%1").arg(nCourseId);
	QString strClassId = QString("%1").arg(nClassId);

	emit playClassroom(strCourseId,strClassId);
}

void QClassWebWidget::setUserInfo(__int64 uid,QString name,QString image)
{
	if(NULL==m_webBrowser){
		return;
	}

	WCHAR wszUid[1024] = { 0 };
	WCHAR wszName[1024] = { 0 };
	WCHAR wszImage[1024] = { 0 };

	_itow(uid,wszUid,10);
	name.toWCharArray(wszName);
	image.toWCharArray(wszImage);

	CComVariant varParam[3]={CComVariant(wszImage),CComVariant(wszName),CComVariant(wszUid)};

	m_webBrowser->CallWebScriptFunction(L"setUserInfo",varParam,3);
}

void QClassWebWidget::onShowClassList(LPCWSTR pwszDate)
{
	if(NULL==pwszDate||NULL==pwszDate[0]){
		return;
	}

	emit showClassList(QString::fromStdWString(pwszDate));
}