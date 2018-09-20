#pragma once

#include <atlbase.h>
#include <exdisp.h>
#include <exdispid.h>
#include <mshtmhst.h>
#include <MsHTML.h>

class WebBrowser:
    public DWebBrowserEvents2,
    public IOleClientSite,
    public IOleInPlaceSite,
    public IOleInPlaceFrame,
    public IDocHostUIHandler
{
public:
    WebBrowser();
    virtual ~WebBrowser(void);

public:
	// IUnknown methods
    virtual STDMETHODIMP QueryInterface(REFIID iid,void**ppvObject);
    virtual STDMETHODIMP_(ULONG) AddRef();
    virtual STDMETHODIMP_(ULONG) Release();

	// IDispatch Methods
    virtual HRESULT _stdcall GetTypeInfoCount(unsigned int * pctinfo);
    virtual HRESULT _stdcall GetTypeInfo(unsigned int iTInfo,LCID lcid,ITypeInfo FAR* FAR* ppTInfo);
    virtual HRESULT _stdcall GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,unsigned int cNames,LCID lcid,DISPID FAR* rgDispId);
    virtual HRESULT _stdcall Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pDispParams,VARIANT FAR* pVarResult,EXCEPINFO FAR* pExcepInfo,unsigned int FAR* puArgErr);

	// IOleClientSite methods
    virtual STDMETHODIMP SaveObject();
    virtual STDMETHODIMP GetMoniker(DWORD dwA,DWORD dwW,IMoniker**pm);
    virtual STDMETHODIMP GetContainer(IOleContainer**pc);
    virtual STDMETHODIMP ShowObject();
    virtual STDMETHODIMP OnShowWindow(BOOL f);
    virtual STDMETHODIMP RequestNewObjectLayout();
 
	// IOleInPlaceSite methods
    virtual STDMETHODIMP GetWindow(HWND *p);
    virtual STDMETHODIMP ContextSensitiveHelp(BOOL);
    virtual STDMETHODIMP CanInPlaceActivate();
    virtual STDMETHODIMP OnInPlaceActivate();
    virtual STDMETHODIMP OnUIActivate();
    virtual STDMETHODIMP GetWindowContext(IOleInPlaceFrame** ppFrame,IOleInPlaceUIWindow **ppDoc,LPRECT r1,LPRECT r2,LPOLEINPLACEFRAMEINFO o);
    virtual STDMETHODIMP Scroll(SIZE s);
    virtual STDMETHODIMP OnUIDeactivate(int);
    virtual STDMETHODIMP OnInPlaceDeactivate();
    virtual STDMETHODIMP DiscardUndoState();
    virtual STDMETHODIMP DeactivateAndUndo();
    virtual STDMETHODIMP OnPosRectChange(LPCRECT);

	// IOleInPlaceFrame methods
    virtual STDMETHODIMP GetBorder(LPRECT l);
    virtual STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS);
    virtual STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS w);
    virtual STDMETHODIMP SetActiveObject(IOleInPlaceActiveObject*pV,LPCOLESTR s);
    virtual STDMETHODIMP InsertMenus(HMENU h,LPOLEMENUGROUPWIDTHS x);
    virtual STDMETHODIMP SetMenu(HMENU h,HOLEMENU hO,HWND hw);
    virtual STDMETHODIMP RemoveMenus(HMENU h);
    virtual STDMETHODIMP SetStatusText(LPCOLESTR t);
    virtual STDMETHODIMP EnableModeless(BOOL f);
    virtual STDMETHODIMP TranslateAccelerator(LPMSG,WORD);

	// IDocHostUIHandler methods
	virtual STDMETHODIMP ShowContextMenu( DWORD dwID,POINT *ppt,IUnknown *pcmdtReserved,IDispatch *pdispReserved);
	virtual STDMETHODIMP GetHostInfo(DOCHOSTUIINFO *pInfo);
	virtual STDMETHODIMP ShowUI( DWORD dwID,IOleInPlaceActiveObject *pActiveObject,IOleCommandTarget *pCommandTarget,
		IOleInPlaceFrame *pFrame,IOleInPlaceUIWindow *pDoc);
	virtual STDMETHODIMP HideUI(void);
	virtual STDMETHODIMP UpdateUI( void);
	virtual STDMETHODIMP OnDocWindowActivate( BOOL fActivate);
	virtual STDMETHODIMP OnFrameWindowActivate( BOOL fActivate);
	virtual STDMETHODIMP ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow *pUIWindow,BOOL fRameWindow);
	virtual STDMETHODIMP TranslateAccelerator( LPMSG lpMsg,const GUID *pguidCmdGroup,DWORD nCmdID);
	virtual STDMETHODIMP GetOptionKeyPath( LPOLESTR *pchKey,DWORD dw);
	virtual STDMETHODIMP GetDropTarget( IDropTarget *pDropTarget,IDropTarget **ppDropTarget);
	virtual STDMETHODIMP GetExternal( IDispatch **ppDispatch);
	virtual STDMETHODIMP TranslateUrl( DWORD dwTranslate,OLECHAR *pchURLIn,OLECHAR **ppchURLOut);
	virtual STDMETHODIMP FilterDataObject(IDataObject *pDO,IDataObject **ppDORet);

protected:
	virtual void DocumentComplete( IDispatch *pDisp,VARIANT *URL);
	virtual void BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel);

protected:
    virtual HWND GetHWND(){return _hWebWnd;};
    // 内部工具函数

private:
    inline IOleObject* _GetOleObject(){return _pOleObj;};
    inline IOleInPlaceObject* _GetInPlaceObject(){return _pInPlaceObj;};
    //外部方法

protected:
	//实现调用网页脚本接口
	DISPID	FindId(IDispatch *pObj, LPOLESTR pName);
	HRESULT InvokeMethod(IDispatch *pObj, LPOLESTR pName, VARIANT *pVarResult, VARIANT *p, int cArgs);
	HRESULT GetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);
	HRESULT SetProperty(IDispatch *pObj, LPOLESTR pName, VARIANT *pValue);

public:
    IWebBrowser2*      GetWebBrowser2();
	IHTMLDocument2*    GetHTMLDocument2();
	IHTMLDocument3*    GetHTMLDocument3();
	IHTMLWindow2*      GetHTMLWindow2();
	IHTMLEventObj*     GetHTMLEventObject();
    
public:
	BOOL	Initialize();
	void	Uninitialize();

public:
    BOOL	OpenWebBrowser(HWND hWebWnd);       
	BOOL	OpenURL(VARIANT* pVarUrl);
	BOOL	SetWebRect(LPRECT lprc);
  
	BOOL	CallWebScriptFunction(LPOLESTR lpwszFuncName,VARIANT* pVar=NULL,int nVarNum=0);			//调用网页的脚本函数
	BOOL	CallWebObjectFunction(LPOLESTR lpwszObjName,LPOLESTR lpwszFuncName,VARIANT* pVar=NULL,int nVarNum=0);			//调用网页的变量方法
    // 内部数据
protected:
    long   _refNum;

private:
    RECT  _rcWebWnd;
    bool  _bInPlaced;
    bool  _bExternalPlace;
    bool  _bCalledCanInPlace;
    bool  _bWebWndInited;
	HWND  _hWebWnd;
	bool  _bInit;
	HMENU _hMenu;
	bool  _bNavigateComplete;
private:
    //指针
    IOleObject*                 _pOleObj; 
    IOleInPlaceObject*          _pInPlaceObj;
    IStorage*                   _pStorage;
    IWebBrowser2*               _pWB2;
    IHTMLDocument2*             _pHtmlDoc2;
    IHTMLDocument3*             _pHtmlDoc3;
    IHTMLWindow2*               _pHtmlWnd2;
    IHTMLEventObj*              _pHtmlEvent;
};