#include "ClassWebBrowser.h"

#define DISPID_EXTERNAL_METHOD_ENTER_CLASSROOM		(40001)
#define DISPID_EXTERNAL_METHOD_PLAY_CLASSROOM		(40002)
#define DISPID_EXTERNAL_METHOD_SHOW_CLASSLIST		(40003)

CClassWebBrowser::CClassWebBrowser():m_pCallback(NULL)	
{
}

CClassWebBrowser::~CClassWebBrowser()
{
	Uninitialize();
}


void CClassWebBrowser::SetCallback(IClassCallback * pCallBack)
{
	m_pCallback = pCallBack;
}

HRESULT _stdcall CClassWebBrowser::GetIDsOfNames(REFIID riid, 
	OLECHAR FAR* FAR* rgszNames, 
	unsigned int cNames, 
	LCID lcid, 
	DISPID FAR* rgDispId )
{
	if(lstrcmp(rgszNames[0],L"enterClass") == 0) 
	{
		*rgDispId = DISPID_EXTERNAL_METHOD_ENTER_CLASSROOM;
		return S_OK;
	};

	if(lstrcmp(rgszNames[0],L"playClass") == 0) 
	{
		*rgDispId = DISPID_EXTERNAL_METHOD_PLAY_CLASSROOM;
		return S_OK;
	};

	if(lstrcmp(rgszNames[0],L"showClass") == 0) 
	{
		*rgDispId = DISPID_EXTERNAL_METHOD_SHOW_CLASSLIST;
		return S_OK;
	};

	return WebBrowser::GetIDsOfNames(riid,rgszNames,cNames,lcid,rgDispId);
}

HRESULT _stdcall CClassWebBrowser::Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS* pDispParams,
	VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo,
	unsigned int* puArgErr)
{   
	if ( dispIdMember == DISPID_EXTERNAL_METHOD_ENTER_CLASSROOM)
	{		
		if(m_pCallback && pDispParams->cArgs >= 2)
		{
			__int64 nClassId = 0;
			__int64 nCourseId = 0;

			nClassId = pDispParams->rgvarg[0].ulVal;
			nCourseId = pDispParams->rgvarg[1].ulVal;
			
			m_pCallback->onEnterClassRoom(nClassId,nCourseId);
		}
		return S_OK;
	};	

	if ( dispIdMember == DISPID_EXTERNAL_METHOD_PLAY_CLASSROOM)
	{		
		if(m_pCallback && pDispParams->cArgs >= 2)
		{
			__int64 nClassId = 0;
			__int64 nCourseId = 0;

			nClassId = pDispParams->rgvarg[0].ulVal;
			nCourseId = pDispParams->rgvarg[1].ulVal;

			m_pCallback->onPlayClassRoom(nClassId,nCourseId);
		}
		return S_OK;
	};

	if ( dispIdMember == DISPID_EXTERNAL_METHOD_SHOW_CLASSLIST)
	{		
		if(m_pCallback && pDispParams->cArgs >= 1)
		{
			LPWSTR pwszDate = (LPWSTR)pDispParams->rgvarg[0].bstrVal;
			
			m_pCallback->onShowClassList((LPCWSTR)pwszDate);
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