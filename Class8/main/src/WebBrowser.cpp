#include "WebBrowser.h"
#include <WinInet.h>

#pragma comment(lib,"wininet.lib")

#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;};

/*
==================
|   构造和析构   |
==================
*/
WebBrowser::WebBrowser(void):_refNum(0),
			_bInPlaced(false),
			_bExternalPlace(false),
			_bCalledCanInPlace(false),
			_bWebWndInited(false),
			_pOleObj(NULL),
			_pInPlaceObj(NULL),
			_pStorage(NULL), 
			_pWB2(NULL),
			_pHtmlDoc2(NULL), 
			_pHtmlDoc3(NULL), 
			_pHtmlWnd2(NULL), 
			_pHtmlEvent(NULL),
			_hWebWnd(NULL),
			_hMenu(NULL),
			_bInit(false),
			_bNavigateComplete(false)
{
    ::memset( (PVOID)&_rcWebWnd,0,sizeof(_rcWebWnd));
	
	OleInitialize(NULL);
}

WebBrowser::~WebBrowser(void)
{	
	OleUninitialize();
}

BOOL WebBrowser::Initialize()
{
	HRESULT hr = S_OK;
	
	do 
	{
		hr = StgCreateDocfile(0,STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT | STGM_CREATE,0,&_pStorage);
		if(FAILED(hr) || NULL== _pStorage)
		{
			break;
		}

		hr = OleCreate(CLSID_WebBrowser,IID_IOleObject,OLERENDER_DRAW,0,this,_pStorage,(void**)&_pOleObj);
		if(FAILED(hr) || NULL==_pOleObj)
		{
			break;
		}
			
		hr = _pOleObj->QueryInterface(IID_IOleInPlaceObject,(LPVOID*)&_pInPlaceObj);
		if(FAILED(hr) || NULL==_pInPlaceObj)
		{
			break;
		}

		_bInit = true;
	} while (FALSE);
	
	if(!_bInit) {
		Uninitialize();
	};

	return _bInit ? TRUE : FALSE;
}

void WebBrowser::Uninitialize()
{
	return;

	if(!_bInit)
	{
		return;
	}
		
	SAFE_RELEASE(_pHtmlDoc2);
	SAFE_RELEASE(_pHtmlDoc3);
	SAFE_RELEASE(_pHtmlWnd2);
	SAFE_RELEASE(_pHtmlEvent);
	SAFE_RELEASE(_pWB2);
	SAFE_RELEASE(_pInPlaceObj);
	SAFE_RELEASE(_pOleObj);
	SAFE_RELEASE(_pStorage);

	if(_hMenu){
		DestroyMenu(_hMenu);
		_hMenu=NULL;
	}
	_bInit = false;
}

/*
==================
|IUnknown methods|
==================
*/
STDMETHODIMP WebBrowser::QueryInterface(REFIID iid,void**ppvObject)
{
    *ppvObject = 0;
    if ( iid == IID_IOleClientSite )
        *ppvObject = (IOleClientSite*)this;
    if ( iid == IID_IUnknown )
        *ppvObject = this;
    if ( iid == IID_IDispatch )
        *ppvObject = (IDispatch*)this;
    
	if ( _bExternalPlace == false)
    {
        if ( iid == IID_IOleInPlaceSite )
            *ppvObject = (IOleInPlaceSite*)this;
        if ( iid == IID_IOleInPlaceFrame )
            *ppvObject = (IOleInPlaceFrame*)this;
        if ( iid == IID_IOleInPlaceUIWindow )
            *ppvObject = (IOleInPlaceUIWindow*)this;
    }

    if ( iid == DIID_DWebBrowserEvents2 )
        *ppvObject = (DWebBrowserEvents2 *)this;
    if ( iid == IID_IDocHostUIHandler)
        *ppvObject = (IDocHostUIHandler*)this;       
    if ( *ppvObject )
    {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG)  WebBrowser::AddRef()
{
    return ::InterlockedIncrement( &_refNum );
}

STDMETHODIMP_(ULONG)  WebBrowser::Release()
{
    return ::InterlockedDecrement( &_refNum );
}

/*
=====================
| IDispatch Methods |
=====================
*/
HRESULT _stdcall WebBrowser::GetTypeInfoCount(
    unsigned int * pctinfo) 
{
    return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::GetTypeInfo(
    unsigned int iTInfo,
    LCID lcid,
    ITypeInfo FAR* FAR* ppTInfo) 
{
    return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::GetIDsOfNames(REFIID riid, 
  OLECHAR FAR* FAR* rgszNames, 
  unsigned int cNames, 
  LCID lcid, 
  DISPID FAR* rgDispId )
{
	return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::Invoke(
    DISPID dispIdMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS* pDispParams,
    VARIANT* pVarResult,
    EXCEPINFO* pExcepInfo,
    unsigned int* puArgErr)
{      
    // DWebBrowserEvents2
    if( dispIdMember == DISPID_DOCUMENTCOMPLETE)
    {
        DocumentComplete(pDispParams->rgvarg[1].pdispVal,pDispParams->rgvarg[0].pvarVal);
        return S_OK;
    }

	if (dispIdMember == DISPID_NAVIGATECOMPLETE2)
	{
		_bNavigateComplete = true;
		return S_OK;
	}

    if( dispIdMember == DISPID_BEFORENAVIGATE2)
    {
        BeforeNavigate2( pDispParams->rgvarg[6].pdispVal,
                         pDispParams->rgvarg[5].pvarVal,
                         pDispParams->rgvarg[4].pvarVal,
                         pDispParams->rgvarg[3].pvarVal,
                         pDispParams->rgvarg[2].pvarVal,
                         pDispParams->rgvarg[1].pvarVal,
                         pDispParams->rgvarg[0].pboolVal);
        return S_OK;
    }
    
	return E_NOTIMPL;
}

/*
========================
|IOleClientSite methods|
========================
*/
STDMETHODIMP WebBrowser::SaveObject()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::GetMoniker(DWORD dwA,DWORD dwW,IMoniker**pm)
{
    *pm = 0;
    return E_NOTIMPL;
}

STDMETHODIMP WebBrowser::GetContainer(IOleContainer**pc)
{
    *pc = 0;
    return E_FAIL;
}

STDMETHODIMP WebBrowser::ShowObject()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::OnShowWindow(BOOL f)
{
    return S_OK;
}

STDMETHODIMP WebBrowser::RequestNewObjectLayout()
{
    return S_OK;
}

/*
=========================
|IOleInPlaceSite methods|
=========================
*/
STDMETHODIMP WebBrowser::GetWindow(HWND *p)
{
    *p = GetHWND();
    return S_OK;
}

STDMETHODIMP WebBrowser::ContextSensitiveHelp(BOOL)
{
    return E_NOTIMPL;
}

STDMETHODIMP WebBrowser::CanInPlaceActivate()//If this function return S_FALSE, AX cannot activate in place!
{
    if ( _bInPlaced )//Does WebBrowser Control already in placed?
    {
        _bCalledCanInPlace = true;
        return S_OK;
    }
    return S_FALSE;
}

STDMETHODIMP WebBrowser::OnInPlaceActivate()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::OnUIActivate()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::GetWindowContext(IOleInPlaceFrame** ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT r1,LPRECT r2,LPOLEINPLACEFRAMEINFO o)
{    
    *ppFrame = (IOleInPlaceFrame*)this;
    AddRef();
    *ppDoc = NULL;
    ::GetClientRect(  GetHWND() ,&_rcWebWnd );
    *r1 = _rcWebWnd;
    *r2 = _rcWebWnd;
    o->cb = sizeof(OLEINPLACEFRAMEINFO);
    o->fMDIApp = false;
    o->hwndFrame = GetParent( GetHWND() );
    o->haccel = 0;
    o->cAccelEntries = 0;
    
    return S_OK;
}

STDMETHODIMP WebBrowser::Scroll(SIZE s)
{
    return E_NOTIMPL;
}

STDMETHODIMP WebBrowser::OnUIDeactivate(int)
{
    return S_OK;
}

STDMETHODIMP WebBrowser::OnInPlaceDeactivate()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::DiscardUndoState()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::DeactivateAndUndo()
{
    return S_OK;
}

STDMETHODIMP WebBrowser::OnPosRectChange(LPCRECT)
{
    return S_OK;
}

/*
==========================
|IOleInPlaceFrame methods|
==========================
*/
STDMETHODIMP WebBrowser::GetBorder(LPRECT l)
{
    ::GetClientRect(  GetHWND() ,&_rcWebWnd );
    *l = _rcWebWnd;
    return S_OK;
}

STDMETHODIMP WebBrowser::RequestBorderSpace(LPCBORDERWIDTHS b)
{
    return S_OK;
}

STDMETHODIMP WebBrowser::SetBorderSpace(LPCBORDERWIDTHS b)
{
    return S_OK;
}

STDMETHODIMP WebBrowser::SetActiveObject(IOleInPlaceActiveObject*pV,LPCOLESTR s)
{
    return S_OK;
}

STDMETHODIMP WebBrowser::SetStatusText(LPCOLESTR t)
{
    return E_NOTIMPL;
}

STDMETHODIMP WebBrowser::EnableModeless(BOOL f)
{
    return E_NOTIMPL;
}

STDMETHODIMP WebBrowser::TranslateAccelerator(LPMSG,WORD)
{
    return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::RemoveMenus(HMENU h)
{
    return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::InsertMenus(HMENU h,LPOLEMENUGROUPWIDTHS x)
{
    return E_NOTIMPL;
}

HRESULT _stdcall WebBrowser::SetMenu(HMENU h,HOLEMENU hO,HWND hw)
{
    return E_NOTIMPL;
}

/*
====================
|DWebBrowserEvents2|
====================
*/
void WebBrowser::DocumentComplete( IDispatch *pDisp,VARIANT *URL)
{
	IHTMLDocument2* pDocument = GetHTMLDocument2();
	if(NULL == pDocument)
	{
		return;
	}
	
	IHTMLElement * pElement = NULL;
	IHTMLBodyElement * pBody = NULL;
	IHTMLStyle * pStyle = NULL;
	do 
	{
		HRESULT hr = pDocument->get_body(&pElement);
		if(FAILED(hr) || NULL == pElement)
		{
			break;
		};

		hr = pElement->get_style(&pStyle);
		if(FAILED(hr) || NULL == pStyle)
		{
			break;
		};

		hr = pElement->QueryInterface(IID_IHTMLBodyElement,(void**)&pBody);
		if(FAILED(hr) || NULL == pBody)
		{
			break;
		};

		//滚动条不显示
		hr = pBody->put_scroll(L"no");
		hr = pStyle->put_border(L"none");
		hr = pStyle->put_overflow(L"hidden");

		hr = _pWB2->put_Silent(VARIANT_TRUE);
		
	} while (FALSE);
		
	SAFE_RELEASE(pElement);
	SAFE_RELEASE(pStyle);
    return ;
}

void WebBrowser::BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel)
{
    PCWSTR pcwApp = L"about:blank";
    if( url->vt != VT_BSTR )
    {
		return;
	}

    if( 0 == _wcsnicmp( pcwApp, url->bstrVal,wcslen(pcwApp)) )
    {
        *Cancel = VARIANT_FALSE;
		return;
    }

	if( NULL != wcsstr(url->bstrVal,L"javvascript:"))
	{
		*Cancel = VARIANT_TRUE;
		return;
	}

	LPCWSTR  pwszUrl = url->bstrVal;
	LPCWSTR  pwszCol = wcschr(pwszUrl,L':');
	if(NULL != pwszCol)
	{
		WCHAR wszPrefix[32] = { 0 };
		wcsncpy_s(wszPrefix,pwszUrl,pwszCol- pwszUrl);
		if( wcsicmp(wszPrefix,L"http") != 0 && 
			wcsicmp(wszPrefix,L"https") != 0)
		{
			*Cancel = VARIANT_TRUE;
			return;
		}
	}

    *Cancel = VARIANT_FALSE;
}

/*
=====================
| IDocHostUIHandler |
=====================
*/

//IDocHostUIHanler
HRESULT WebBrowser::ShowContextMenu( 
    DWORD dwID,
    POINT *ppt,
    IUnknown *pcmdtReserved,
    IDispatch *pdispReserved)
{
	if(_hMenu)
	{
		::TrackPopupMenu(_hMenu,TPM_RIGHTBUTTON|TPM_LEFTALIGN|TPM_RETURNCMD,ppt->x,ppt->y,0,_hWebWnd,NULL);
	}
	return S_OK;
}

HRESULT WebBrowser::GetHostInfo(DOCHOSTUIINFO *pInfo)
{
	if(NULL != pInfo)
	{
		pInfo->dwFlags = pInfo->dwFlags | DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO; //滚动条
	}
	return S_OK;
}

HRESULT WebBrowser::ShowUI(DWORD dwID,
						   IOleInPlaceActiveObject *pActiveObject,
						   IOleCommandTarget *pCommandTarget,
						   IOleInPlaceFrame *pFrame,
						   IOleInPlaceUIWindow *pDoc)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::HideUI(void)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::UpdateUI(void)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::OnDocWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::OnFrameWindowActivate(BOOL fActivate)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::ResizeBorder(LPCRECT prcBorder,
								  IOleInPlaceUIWindow *pUIWindow,
								  BOOL fRameWindow)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::TranslateAccelerator(PMSG lpMsg,
										 const GUID *pguidCmdGroup,
										 DWORD nCmdID)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::GetOptionKeyPath( LPOLESTR *pchKey,DWORD dw)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::GetDropTarget( IDropTarget *pDropTarget,IDropTarget **ppDropTarget)
{
    return E_NOTIMPL;//使用默认拖拽
    //return S_OK;//自定义拖拽
}

HRESULT WebBrowser::GetExternal( IDispatch **ppDispatch)
{
	HRESULT hr = QueryInterface(IID_IDispatch,(void**)ppDispatch);
    return hr;
}

HRESULT WebBrowser::TranslateUrl(DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut)
{
	return E_NOTIMPL;
}

HRESULT WebBrowser::FilterDataObject(IDataObject *pDO,IDataObject **ppDORet)
{
	return E_NOTIMPL;
}
/*
===============
|Other Methods|
===============
*/
IWebBrowser2* WebBrowser::GetWebBrowser2()
{
    if( _pWB2 != NULL )
        return _pWB2;

	if(NULL==_pOleObj)
		return NULL;
		
	HRESULT hr = _pOleObj->QueryInterface(IID_IWebBrowser2,(void**)&_pWB2);
	if(FAILED(hr))
		return NULL;

	return _pWB2; 
}

IHTMLDocument3* WebBrowser::GetHTMLDocument3()
{
    if( _pHtmlDoc3 != NULL )
        return _pHtmlDoc3;

    IWebBrowser2* pWB2 = GetWebBrowser2();
	if(NULL==pWB2) {
		return NULL;
	}

	IDispatch* pDp =  NULL;
	HRESULT hr = pWB2->get_Document(&pDp);
	if(FAILED(hr) || NULL == pDp) {
		return NULL;
	}
	
	pDp->QueryInterface(IID_IHTMLDocument3,(void**)&_pHtmlDoc3);
	pDp->Release();
 
	return _pHtmlDoc3;
}

IHTMLDocument2* WebBrowser::GetHTMLDocument2()
{
	if( _pHtmlDoc2 != NULL )
		return _pHtmlDoc2;

	IWebBrowser2* pWB2 = GetWebBrowser2();
	if(NULL==pWB2) {
		return NULL;
	}

	IDispatch* pDp =  NULL;
	HRESULT hr = pWB2->get_Document(&pDp);
	if(FAILED(hr) || NULL == pDp) {
		return NULL;
	}

	pDp->QueryInterface(IID_IHTMLDocument2,(void**)&_pHtmlDoc2);
	pDp->Release();
	
	return _pHtmlDoc2;
}

IHTMLWindow2* WebBrowser::GetHTMLWindow2()
{
    if( _pHtmlWnd2 != NULL)
        return _pHtmlWnd2;

    IHTMLDocument2*  pHD2 = GetHTMLDocument2();
	if(NULL==pHD2) {
		return NULL;
	};

    HRESULT hr = pHD2->get_parentWindow(&_pHtmlWnd2);
	if(FAILED(hr)) {
		return NULL;
	};

    return _pHtmlWnd2;
}

IHTMLEventObj* WebBrowser::GetHTMLEventObject()
{
    if( _pHtmlEvent != NULL )
        return _pHtmlEvent;

    IHTMLWindow2* pHW2 = GetHTMLWindow2();
	if(NULL==pHW2) {
		return NULL;
	};

    HRESULT hr = pHW2->get_event(&_pHtmlEvent);
	if(FAILED(hr)) {
		return NULL;
	};
    return _pHtmlEvent;
}

BOOL WebBrowser::SetWebRect(LPRECT lprc)
{
	if(NULL==lprc)
	{
		return FALSE;
	}

    if( false == _bInPlaced )//尚未OpenWebBrowser操作,直接写入_rcWebWnd
    {
       _rcWebWnd = *lprc;
	   return TRUE;
    }
    
	SIZEL size = { 0 };
	size.cx = lprc->right - lprc->left;
	size.cy = lprc->bottom - lprc->top;

	IOleObject* pOleObj = _GetOleObject();
	if(NULL==pOleObj) {
		return FALSE;
	};

	HRESULT hr = pOleObj->SetExtent(1,&size );
	if(FAILED(hr))
	{
		return FALSE;
	}

	IOleInPlaceObject* pInPlace = _GetInPlaceObject();
	if(NULL==pInPlace) {
		return FALSE;
	};

	hr = pInPlace->SetObjectRects(lprc,lprc);
	if(FAILED(hr))
	{
		return FALSE;
	}

	_rcWebWnd = *lprc;

	return TRUE;
}

BOOL WebBrowser::OpenWebBrowser(HWND hWebWnd)
{
	IOleObject* pOleObj = _GetOleObject();
	if(NULL==pOleObj)
	{
		return FALSE;
	}
	
	bool bret = false;
	_hWebWnd = hWebWnd;

	if( _rcWebWnd.left == _rcWebWnd.right && _rcWebWnd.bottom == _rcWebWnd.top)
	{
		::GetClientRect( hWebWnd,&_rcWebWnd);//设置WebBrowser的大小为窗口的客户区大小.
	}

	if( _bInPlaced == false )    // Activate In Place
	{
		_bInPlaced = true;	     //_bInPlaced must be set as true, before INPLACEACTIVATE, otherwise, once DoVerb, it would return error;
		_bExternalPlace = 0;	 //lParam;

		HRESULT hr = pOleObj->DoVerb(OLEIVERB_INPLACEACTIVATE,0,this,0,_hWebWnd,&_rcWebWnd);
		if(FAILED(hr)) {
			_bInPlaced = false;
			return FALSE;
		};

		_bInPlaced = true;

		//* 挂接DWebBrwoser2Event
		IConnectionPointContainer* pCPC = NULL;
		IConnectionPoint*          pCP  = NULL;
		IWebBrowser2 *			   pWebBrowser = GetWebBrowser2();
		
		if(NULL==pWebBrowser)
		{
			return FALSE;
		}
		
		do 
		{
			hr = pWebBrowser->QueryInterface(IID_IConnectionPointContainer,(void**)&pCPC);
			if(FAILED(hr) || NULL==pCPC)
			{
				break;
			}

			hr = pCPC->FindConnectionPoint( DIID_DWebBrowserEvents2,&pCP);
			if(FAILED(hr) || NULL==pCP)
			{
				break;
			}

			DWORD dwCookie = 0;
			hr = pCP->Advise( (IUnknown*)(void*)this,&dwCookie);
			if(FAILED(hr))
			{
				break;
			}
			
			bret = true;
		} while (FALSE);
		
		SAFE_RELEASE(pCP);
		SAFE_RELEASE(pCPC);
	}

	return bret ? TRUE : FALSE;
}

BOOL WebBrowser::OpenURL(VARIANT* pVarUrl)
{    
	IWebBrowser2* pWebBrowser = GetWebBrowser2();
	if(NULL==pWebBrowser) {
		return FALSE;
	};
		
	
	_bNavigateComplete = false;
	ATL::CComVariant varFlag(navNoHistory|navOpenInBackgroundTab|navNoReadFromCache|navNoWriteToCache);
	HRESULT hr = pWebBrowser->Navigate2(pVarUrl,&varFlag,NULL,NULL,NULL);
	if(SUCCEEDED(hr)) {
		return TRUE;
	};
	
	return FALSE;   
}

BOOL WebBrowser::CallWebScriptFunction(LPOLESTR lpwszFuncName,VARIANT* pVar/* =NULL */,int nVarNum/* =0 */)
{
	if(NULL==lpwszFuncName||NULL==lpwszFuncName[0]){
		return FALSE;
	};

	IHTMLWindow2* pHtmlWnd = GetHTMLWindow2();
	if(NULL==pHtmlWnd) {
		return FALSE;
	};

	VARIANT ret;
	HRESULT hr = InvokeMethod((IDispatch*)pHtmlWnd,(LPOLESTR)lpwszFuncName,&ret,pVar,nVarNum);
	if(FAILED(hr)) {
		return FALSE;
	};

	return ret.iVal == 0 ? TRUE : FALSE;
}

BOOL WebBrowser::CallWebObjectFunction(LPOLESTR lpwszObjName,LPOLESTR lpwszFuncName,VARIANT* pVar/* =NULL */,int nVarNum/* =0 */)
{
	if( NULL==lpwszObjName||NULL==lpwszObjName[0] ||
		NULL==lpwszFuncName||NULL==lpwszFuncName[0] ){
		return FALSE;
	};

	IHTMLWindow2* pHtmlWnd = GetHTMLWindow2();
	if(NULL==pHtmlWnd) {
		return FALSE;
	};

	VARIANT varObj;
	VARIANT varRet;

	HRESULT hr = GetProperty((IDispatch*)pHtmlWnd,lpwszObjName,&varObj);
	if(FAILED(hr)) {
		return FALSE;
	};

	hr = InvokeMethod(varObj.pdispVal,lpwszFuncName,&varRet,pVar,nVarNum);
	if(FAILED(hr)) {
		return FALSE;
	};

	return varRet.iVal == 0 ? TRUE : FALSE;
}

/*
实现网页调用接口
*/
DISPID WebBrowser::FindId(IDispatch *pObj, LPOLESTR pName)
{
	DISPID id = 0;
	if(FAILED(pObj->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&id))) id = -1;
	return id;
}

HRESULT WebBrowser::InvokeMethod(IDispatch *pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs)
{
	DISPPARAMS ps;
	ps.cArgs = cArgs;
	ps.rgvarg = p;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	DISPID dispid = FindId(pObj, pName);
	if(dispid != -1) 
	{
		return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, pVarResult, NULL, NULL);
	}

	IDispatch* pHtmlScript = NULL;
	HRESULT hr = _pHtmlDoc2->get_Script(&pHtmlScript);
	if(FAILED(hr) || NULL == pHtmlScript)
	{
		return E_FAIL;
	}

	
	pHtmlScript->GetIDsOfNames(IID_NULL,&pName,1,LOCALE_SYSTEM_DEFAULT,&dispid);
	if(dispid != -1)
	{
		return pHtmlScript->Invoke(dispid,IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, pVarResult, NULL, NULL);
	}

	return E_FAIL;
}

HRESULT WebBrowser::GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &ps, pValue, NULL, NULL);
}

HRESULT WebBrowser::SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue)
{
	DISPID dispid = FindId(pObj, pName);
	if(dispid == -1) return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 1;
	ps.rgvarg = pValue;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	return pObj->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &ps, NULL, NULL, NULL);
}