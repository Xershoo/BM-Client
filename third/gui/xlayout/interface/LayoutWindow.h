#pragma once
#include <map>
#include "ILayout.h"
#include "LayoutEvent.h"
#include <xstring.h>
#include "xctrl.h"
#include <list>

NAMESPACE_BEGIN(xlayout)
struct ILayoutWindow;
struct ILayoutDriver;
struct ILayoutRequest;
struct ILayoutStateMachine;

struct ILayoutWindow
{
    virtual void        AttachLayout(xlayout::ILayoutObj* layout)				= 0;
    virtual void        DetachLayout()											= 0;
    virtual void        MoveLayoutWnd(LPCRECT lpCect, BOOL bRepaint)			= 0;
    virtual void        ShowLayoutWnd(bool bShow)								= 0;
    virtual HDWP        DeferWindowPos(LPCRECT lpCect, HDWP hDwp)				= 0;
    virtual void		DestroySelf()											= 0;
};

struct ILayoutDrawer
{
    virtual void        DrawLayout(xgdi::ICanvas* canvas, RECT rcUpdate)		= 0;
};

struct ILayoutDriver
{
    virtual bool        Request(ILayoutRequest* pRequest)           = 0;
    virtual void        AddLayout(ILayoutStateMachine* pLayout)     = 0;
};

struct ILayoutRequest
{
    virtual LPCTSTR     GetRequest() const                          = 0;
    virtual LPARAM      GetParam(UINT) const                        = 0;
    virtual UINT        GetParamCount() const                       = 0;
};

struct ILayoutStateMachine
{
    virtual void        SetILayoutDriver(ILayoutDriver* pDriver)    = 0;
    virtual void        SetILayout(ILayout* pLayout)                = 0;
    virtual void        Trigger(LPCTSTR lpszCondition)              = 0;
    virtual void        Release()                                   = 0;
};


struct ILayoutMgrWindow
{
    virtual HWND        GetHWND()	                                    = 0;
    virtual int         GetCtrlIdByName(LPCTSTR)                        = 0;
};

struct ILayoutWndCreator 
{
    virtual ILayoutWindow* Create(ILayoutMgrWindow* , ILayoutObj*)		= 0;
};

class LayoutWndFactory
{
public:
    static LayoutWndFactory* inst()
    {
        static LayoutWndFactory factory;
        return &factory;
    }

    void RegisterCreator(LPCTSTR wndType, ILayoutWndCreator* creator)
    {
        __creators[wndType] = creator;
    }
    ILayoutWindow* Create(LPCTSTR wndType,	ILayoutMgrWindow* pMgrWnd, ILayoutObj* pLayoutObj)
    {
        WndCreatorMap::iterator itr = __creators.find(wndType);
        if(itr != __creators.end()){
            ILayoutWndCreator* creator = itr->second;
            return creator->Create(pMgrWnd, pLayoutObj);
        }
        return NULL;
    }
protected:
    typedef std::map<xstring, ILayoutWndCreator*>  WndCreatorMap;
    WndCreatorMap __creators;
};

template<class T>
class CLayoutMgrWindow
    : public CWindowImpl<T>
    , public CMessageFilter
    , public ILayoutMgrWindow
{
#define XLAYOUT_ELEMENT(name) m_layout##name

#define XLAYOUT_DEF(tclass, name) \
    xlayout::CWTLayoutWindow<tclass> XLAYOUT_ELEMENT(name)

#define XLAYOUT_ID(name) \
    GetCtrlIdByName(_T(#name))

#define XLAYOUT_CREATE(name, text) \
    if(m_layout##name.Create(m_hWnd, 0, text, WS_CHILD | WS_VISIBLE, 0, XLAYOUT_ID(name))) { \
    m_layout##name.SetFont(m_fontDefault); \
    AddLayoutWindow(&m_layout##name, _T(#name)); \
    }
#define XLAYOUT_CREATE_EX(name, text, style) \
    if(m_layout##name.Create(m_hWnd, 0, text, WS_CHILD | WS_VISIBLE | style, 0, XLAYOUT_ID(name))) { \
    m_layout##name.SetFont(m_fontDefault); \
    AddLayoutWindow(&m_layout##name, _T(#name)); \
    }

#define XLAYOUT_CREATE_EX2(name, text, style, exstyle) \
    if(m_layout##name.Create(m_hWnd, 0, text, WS_CHILD | WS_VISIBLE | style, exstyle, XLAYOUT_ID(name))) { \
    m_layout##name.SetFont(m_fontDefault); \
    AddLayoutWindow(&m_layout##name, _T(#name)); \
    }

#define XLAYOUT_CREATE_POPUP(name, text, style, exstyle) \
	if(m_layout##name.Create(m_hWnd, 0, text, style, exstyle)) { \
	m_layout##name.SetFont(m_fontDefault); \
	AddLayoutWindow(&m_layout##name, _T(#name)); \
	}

public:
    CLayoutMgrWindow()
        : m_fontDefault(NULL), m_nDefID(0)
    {
        m_pLayout = CreateAKXML();
        m_pLayout->SetCallBack(CLayoutMgrWindow::LayoutEventProc, (DWORD)this);
        m_hDwp	  = NULL;
    }

    virtual ~CLayoutMgrWindow()
    {
        if(m_pLayout)
        {
            m_pLayout->Uninit();
            m_pLayout->Release();
        }
        LayoutGetter::iterator itr = m_layouts.begin();
        for(; itr != m_layouts.end(); ++ itr)
        {
            if(ILayoutWindow* pWndObj = m_mapObj2Wnd[*itr])
            {
                pWndObj->DestroySelf();
            }
        }
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }

    virtual int GetCtrlIdByName(LPCTSTR lpszName) 
    {
        static int id = 100;

        ctrl_id_map::iterator itr = m_idMap.find(lpszName);
        if(itr == m_idMap.end()){
            m_idMap[lpszName] = ++ id;
            return id;                
        }
        return itr->second;
    }

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        if(::IsWindowVisible(m_hWnd) && pMsg->message == WM_KEYDOWN)
        {
            ATL::CString strTitle;
            GetWindowText(strTitle);
            //ATLTRACE(_T("Try CWindow(%s)\n"), strTitle.GetString());
            switch(pMsg->wParam)
            {
            case VK_TAB:
                if(HWND hWnd = ::GetNextDlgTabItem(m_hWnd, ::GetFocus(), ::GetKeyState(VK_SHIFT) & 0x8000))
                {
                    ::SetFocus(hWnd);
                    //m_nDefID = ::GetDlgCtrlID(hWnd);
                    return TRUE;
                }
                break;

            case VK_RETURN:
                if(UINT nID = GetDefID())
                {
                    PostMessage(WM_COMMAND, MAKEWPARAM(nID, BN_CLICKED), (LPARAM)::GetDlgItem(m_hWnd, nID));
                    return TRUE;
                }
                break;
			
				break;
				
            default:
                break;
            }
        }
		if(::IsWindowVisible(m_hWnd) && pMsg->message == WM_SYSKEYDOWN && ::GetActiveWindow() == m_hWnd)
		{
			if (pMsg->wParam == VK_F4 && ((::GetKeyState(VK_MENU) & 0x8000) || (::GetKeyState(VK_LMENU) & 0x8000)))
			{
				PostMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
				return TRUE;
			}
		}

        return FALSE;
    }


    BEGIN_MSG_MAP(CLayoutMgrWindow<T>)
        MESSAGE_HANDLER(WM_PAINT,           OnPaint)
        MESSAGE_HANDLER(WM_CREATE,          OnCreate)
        MESSAGE_HANDLER(WM_DESTROY,         OnDestroy)
        MESSAGE_HANDLER(WM_ERASEBKGND,      OnEraseBkgnd)
        MESSAGE_HANDLER(WM_SIZE,            OnSize)

        MESSAGE_HANDLER(WM_LBUTTONDOWN,     OnLButtonDown)
        MESSAGE_HANDLER(WM_LBUTTONUP,       OnLButtonUp)
        MESSAGE_HANDLER(WM_MOUSEMOVE,       OnMouseMove)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //    LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //    LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //    LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        //TODO: Add your drawing code here
#ifdef _DEBUG_DUMP_LAYOUT
        RECT rcClient;
        GetClientRect(&rcClient);
        dc.FillSolidRect(&rcClient, RGB(250, 250, 250));

        class CVisitObject
            : public xlayout::ILayoutVisitor
        {
        public:
            CVisitObject(CPaintDC& dc)
                : m_dcObject(dc)
            {
            }

            virtual void dumpBegin(){}
            virtual void dumpEnd(){}

            virtual void dump(ILayoutObj* pObj, int nDepth)
            {
                WTL::CString str;
                str.Format(_T("%d"), pObj->GetID());
                m_dcObject.SetBkMode(TRANSPARENT);
                switch(pObj->GetType())
                {
                case TYPE_OBJ:
                    m_dcObject.DrawText(pObj->GetName(), -1, (LPRECT)pObj->GetBounds(), DT_LEFT | DT_SINGLELINE);
                    m_dcObject.DrawText(str, -1, (LPRECT)pObj->GetBounds(), DT_RIGHT | DT_BOTTOM |DT_SINGLELINE);
                    m_dcObject.FrameRect(pObj->GetBounds(), (HBRUSH)::GetStockObject(BLACK_BRUSH));
                    break;

                case TYPE_SPLITTER_H:
                case TYPE_SPLITTER_V:
                case TYPE_GROUP:
                default:
                    break;
                }
            }

        protected:
            CPaintDC& m_dcObject;
        } visitobj(dc);
        m_pLayout->Visit(&visitobj);
#endif

        return 0;
    }

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        m_LayoutEvent.SetLayout(m_pLayout);
        m_LayoutEvent.SetHwnd(m_hWnd);

        return 0;
    }

    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        return 0;
    }

    LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return 0;
    }

    LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        RECT rcClient;
        ::SetRect(&rcClient, 0, 0, LOWORD(lParam), HIWORD(lParam));

        m_hDwp = ::BeginDeferWindowPos(100);

		m_rectMap.clear();
        m_pLayout->ReLayout(&rcClient);

		wnd_rect_map::iterator iter;
		for (iter = m_rectMap.begin(); iter != m_rectMap.end(); iter++)
		{
			m_hDwp = iter->first->DeferWindowPos(&(iter->second), m_hDwp);
		}
		m_rectMap.clear();
        ::EndDeferWindowPos(m_hDwp);

        bHandled = FALSE;
        m_hDwp   = NULL;

		if (m_rectMap.size() > 0)
		{
			m_hDwp = ::BeginDeferWindowPos(m_rectMap.size());
			for (iter = m_rectMap.begin(); iter != m_rectMap.end(); iter++)
			{
				m_hDwp = iter->first->DeferWindowPos(&(iter->second), m_hDwp);
			}
			m_rectMap.clear();
			::EndDeferWindowPos(m_hDwp);
			m_hDwp   = NULL;
		}
        return 0;
    }

    LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        m_LayoutEvent.OnLButtonDown(wParam, pt);

        return 0;
    }

    LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        m_LayoutEvent.OnLButtonUp(wParam, pt);

        return 0;
    }

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        m_LayoutEvent.OnMouseMove(wParam, pt);

        return 0;
    }

    void AddLayoutWindow(ILayoutWindow* pWindow, LPCTSTR name)
    {
        if(ILayoutObj* pObj = m_pLayout->GetLayoutObj(name))
        {
            if(m_mapObj2Wnd[pObj] == NULL)
            {
                m_mapObj2Wnd[pObj] = pWindow;
                pWindow->AttachLayout(pObj);
            }
        }
    }

    void AddLayoutWindow(ILayoutWindow* pWindow, int nID)
    {
        if(ILayoutObj* pObj = m_pLayout->GetLayoutObj(nID))
        {
            if(m_mapObj2Wnd[pObj] == NULL)
            {
                m_mapObj2Wnd[pObj] = pWindow;
                pWindow->AttachLayout(pObj);
            }
        }   
    }

    virtual BOOL OnLayoutEvent(int nType, ILayoutObj* pObj)
    {
        LAYOUT_MAP::iterator iter = m_mapObj2Wnd.find(pObj);
        if(iter != m_mapObj2Wnd.end())
        {
            ILayoutWindow* pWindow = iter->second;
            // type = 0  MOVE
            if(nType == 0){
                if(m_hDwp)
                {
					m_rectMap[pWindow] = *(pObj->GetBounds());
                    //m_hDwp = pWindow->DeferWindowPos(pObj->GetBounds(), m_hDwp);
                }else
                {
                    pWindow->MoveLayoutWnd(pObj->GetBounds(), TRUE);
                }

                // type = 1 SHOW
            }else{
                //pWindow->ShowLayoutWnd(pObj->IsHidden());
                pWindow->ShowLayoutWnd(IsObjectVisible(pObj));
            }
            return TRUE;
        }
        return FALSE;
    }

    bool IsObjectVisible(ILayoutObj* pObj)
    {
        if(pObj == NULL) return TRUE;
        if(pObj->IsHidden()) return FALSE;

        return IsObjectVisible(pObj->GetParent());
    }

    inline void LoadLayout(LPCTSTR filename)
    {
        if(m_pLayout){
            m_pLayout->Init(filename);
        }
    }

    inline ILayout* GetILayout() const
    {
        return m_pLayout;
    }

    inline UINT GetDefID() const
    {
        return m_nDefID;
    }

    inline void SetDefID(UINT nID)
    {
        m_nDefID = nID;
    }

    // layout obj auto-creator
    void AutoCreateObject()
    {
        m_layouts.clear();
        m_pLayout->Visit(&m_layouts);

        // Begin Auto Creating Procedure
        LayoutGetter::iterator itr = m_layouts.begin();
        for(; itr != m_layouts.end(); ++itr)
        {
            ILayoutObj* pLayoutObj = (*itr);
            // class-specified only
            if(LPCTSTR wndClass = pLayoutObj->GetProperty(_T("class")))
            {
                if(ILayoutWindow* pLayoutWnd = LayoutWndFactory::inst()->Create(wndClass, this, pLayoutObj))
                {
                    AddLayoutWindow(pLayoutWnd, pLayoutObj->GetName());
                }
            }
        }
    }

#define XELEMENT(type,name) \
    Element<type>(_T(#name))

    template <class TWindow>
    TWindow* Element(LPCTSTR name)
    {
        ILayoutObj* pLayObj = m_pLayout->GetLayoutObj(name);
        if(pLayObj){
                ILayoutWindow* pLayWnd= m_mapObj2Wnd[pLayObj];
                return dynamic_cast<TWindow*>(pLayWnd);
        }   
        return NULL;
    }

private:
    static void CALLBACK LayoutEventProc(int nType, ILayoutObj* pObj, DWORD_PTR data)
    {
        CLayoutMgrWindow* pLayoutMgr = (CLayoutMgrWindow*)data;
        pLayoutMgr->OnLayoutEvent(nType, pObj);
    }

protected:
    HFONT                                   m_fontDefault;
    ILayout*                                m_pLayout;

    typedef     std::map<xstring, int>      ctrl_id_map;
    ctrl_id_map                             m_idMap;
    HDWP									m_hDwp;

    UINT                                    m_nDefID;

	typedef std::map<ILayoutWindow*,RECT>	wnd_rect_map;
	wnd_rect_map							m_rectMap;
private:
    class LayoutGetter
        : public ILayoutVisitor
        , public std::list<ILayoutObj*>
    {
    public:
        LayoutGetter(){}

        virtual void dumpBegin(){}
        virtual void dumpEnd(){}
        virtual void dump(xlayout::ILayoutObj* pObj, int nDepth)
        {
            // tag "obj" only
            if(pObj && pObj->GetType() == TYPE_OBJ)
            {
                this->push_back(pObj);
            }
        }

    } m_layouts;

    CLayoutEvent                            m_LayoutEvent;
protected:
    typedef std::map<ILayoutObj*, ILayoutWindow*> LAYOUT_MAP;
    LAYOUT_MAP									  m_mapObj2Wnd;
};

// Windowless
class CLayoutWindowLess
    : implement ILayoutWindow
    , implement ILayoutDrawer
{
public:
    // From ILayoutWindow
    virtual void AttachLayout(ILayoutObj* layout)
    {
        m_pLayoutObj = layout;
    }

    virtual void DetachLayout()
    {
        m_pLayoutObj = NULL;
    }

    virtual void MoveLayoutWnd(LPCRECT lpRect, BOOL bRepaint)
    {
    }

    virtual HDWP DeferWindowPos(LPCRECT lpRect, HDWP pos)
    {
        return pos;
    }

    virtual void ShowLayoutWnd(bool bShow)
    {
        m_bShow = bShow;
    }

    virtual void DrawLayout(xgdi::ICanvas* canvas, RECT rcUpdate)
    {

    }
    virtual void DestroySelf()
    {
        delete this;
    }

    inline ILayoutObj* GetLayoutObj()
    {
        return m_pLayoutObj;
    }

    LPCRECT GetBounds()
    {
        return m_pLayoutObj->GetBounds();
    }

public:
    class CLayoutProperty
        : public IProperty
    {
    public:
        CLayoutProperty(ILayoutObj* pObj)
        {
            m_pLayoutObj = pObj;
        }
        // From IProperty
        virtual LPCTSTR GetProperty(LPCTSTR lpszProp) const
        {
            return (m_pLayoutObj ? m_pLayoutObj->GetProperty(lpszProp) : NULL);
        }
        virtual LPCTSTR GetProperty(LPCTSTR lpszProp, LPCTSTR lpszDefault) const
        {
            return (m_pLayoutObj ? m_pLayoutObj->GetProperty(lpszProp) : lpszDefault);
        }
        virtual int GetProperty(LPCTSTR lpszProp, int nDefValue) const
        {
            return (m_pLayoutObj ? _ttoi(m_pLayoutObj->GetProperty(lpszProp)) : nDefValue);
        }
        virtual BOOL SetProperty(LPCTSTR lpszProp)
        {
            return FALSE;
        }
        virtual BOOL SetProperty(int nValue)
        {
            return FALSE;
        }

    protected:
        ILayoutObj* m_pLayoutObj;
    };

protected:
    bool				    m_bShow;
    ILayoutObj*             m_pLayoutObj;
};


// Window/Control
template <class TWindow>
class CWTLayoutWindow
    : public TWindow
    , public CLayoutWindowLess
{
public:
    CWTLayoutWindow()
    {
        m_pLayoutObj = NULL;
    }

    virtual void AttachLayout(ILayoutObj* layout)
    {
        CLayoutWindowLess::AttachLayout(layout);

        if(xctrl::ICssParser* pParser = dynamic_cast<xctrl::ICssParser*>(this))
        {
            pParser->ParseCss(&CLayoutProperty(m_pLayoutObj));
        }
    }

    virtual void MoveLayoutWnd(LPCRECT lpRect, BOOL bRepaint)
    {
        CRect rcWindow(lpRect);
        TWindow::MoveWindow(&rcWindow, FALSE);
        TWindow::Invalidate(TRUE);

        ATLTRACE(_T("MoveLayoutWnd: %s\n"), m_pLayoutObj->GetName());
    }

    virtual void ShowLayoutWnd(bool bShow)
    {
        ::ShowWindow(TWindow::m_hWnd, bShow ? SW_SHOW : SW_HIDE);

        ATLTRACE(_T("ShowLayoutWnd %s: %s\n"), (bShow ? _T("On") : _T("Off")), m_pLayoutObj->GetName());
    }

    virtual HDWP DeferWindowPos(LPCRECT lpRect, HDWP pos)
    {
        return ::DeferWindowPos(pos,TWindow::m_hWnd,NULL,lpRect->left,lpRect->top,
            lpRect->right - lpRect->left,
            lpRect->bottom - lpRect->top,
            SWP_NOACTIVATE|SWP_NOZORDER);
    }

    virtual void DestroySelf()
    {
        delete this;
    }
};

NAMESPACE_END(xlayout)
