#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include "atlctrls.h"
#include "atlctrlw.h"
#include "atlstr.h"
#include "xgdi/xgdi.h"
#include "xgdi/xgdidef.h"
#include "xgdi/xgdiutil.h"
#include "xctrltype.h"
#include "layoutwindow.h"

#include "xuimisc.h"
#include "cssparser.h"


NAMESPACE_BEGIN(xctrl)

class XCssLayoutWnd
    : public xlayout::CLayoutWindowLess
    , public XCssParser
{
public:
    virtual void AttachLayout(xlayout::ILayoutObj* layout)
    {
        xlayout::CLayoutWindowLess::AttachLayout(layout);
        ParseCss(&CLayoutProperty(layout));
    }

    void SetResLoader(xskin::IResLoader* resloader)
    {
        m_pResLoader = resloader;
    }

    void DrawLayout(xgdi::ICanvas* pCanvas, RECT rcUpdate)
    {
        if(m_pLayoutObj == NULL) 
        {
            return;
        }

        RECT rcClient = *GetBounds();
        DrawBkGrd(pCanvas, rcClient, rcUpdate);
        DrawBkImage(pCanvas, rcClient, rcUpdate);
        DrawBorder(pCanvas, rcClient, rcUpdate);
    }
	void ReLoadBkImages(LPCTSTR pName)
	{
		if(pName == NULL)
			return;
		LoadBkgrdImage(pName, NULL, &m_rcInner);
	}
};

template<class T>
class XCssLayoutMgrWnd
    : public xlayout::CLayoutMgrWindow<T>
{
public:
    class CssLayoutCreate
        : public xlayout::ILayoutVisitor
    {
    public:
        CssLayoutCreate()
        {
        }

        struct LayoutVItem
        {
            xlayout::ILayoutObj* obj;
            int					 depth;
        };

        virtual void dumpBegin(){}
        virtual void dumpEnd(){}

        static int compare(const LayoutVItem& a,  const LayoutVItem& b)
        {
            return a.depth < b.depth;
        }
        virtual void dump(xlayout::ILayoutObj* pObj, int nDepth)
        {		
            LayoutVItem litem = {pObj, nDepth};

            std::vector<LayoutVItem>::iterator itr = std::lower_bound(objs.begin(),
                objs.end(),
                litem,
                &CssLayoutCreate::compare);
            objs.insert(itr, litem);
        }
    public:
        std::vector<LayoutVItem>			objs;
        RECT								rect;
        RECT								nothing;
    };

    BEGIN_MSG_MAP(XCssLayoutMgrWnd<T>)
        MESSAGE_HANDLER(WM_CREATE,          OnCreate)
        MESSAGE_HANDLER(WM_SIZE,            OnSize)
        CHAIN_MSG_MAP( xlayout::CLayoutMgrWindow<T>)
    END_MSG_MAP()

    XCssLayoutMgrWnd()
    {
        __resLoader = NULL;
    }

    virtual ~XCssLayoutMgrWnd()
    {
       ClearCssWnd();
    }

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        CLayoutMgrWindow<T>::OnCreate(uMsg, wParam, lParam, bHandled);


        CssLayoutCreate x;
        m_pLayout->Visit(&x);

        for(size_t i=0; i<x.objs.size(); ++ i)
        {
            CreateCssWnd(x.objs[i].obj);
        }

        return 0;
    }

    void CreateCssWnd(xlayout::ILayoutObj* pObj)
    {
        xstring name = pObj->GetName();
        if(!name.empty())
        {
            XCssLayoutWnd* wcss = new XCssLayoutWnd;
            wcss->SetResLoader(__resLoader);
            wcss->AttachLayout(pObj);			

            __cssWnds[pObj->GetName()] = wcss;
            __cssWndList.push_back(wcss);

            ATLTRACE(_T("create css window %s \r\n"), name.c_str());
        }
    }

	XCssLayoutWnd* GetCssLayoutWnd(LPCTSTR pName)
	{
		if(pName == NULL)
			return NULL;
		std::map<xstring, XCssLayoutWnd*>::iterator iter = __cssWnds.find(pName);
		if(iter != __cssWnds.end())
			return iter->second;
		else
			return NULL;
	}

	void ClearCssWnd()
	{
		CssWndMap::iterator itr = __cssWnds.begin();
		for(; itr != __cssWnds.end(); ++ itr)
		{
			delete itr->second;
		}
		__cssWnds.clear();
		__cssWndList.clear();
	}

    void SetResLoader(xskin::IResLoader* rloader)
    {
        __resLoader = rloader;

    }

    void DrawLayouts(xgdi::ICanvas* pCanvas, RECT rcUpdate)
    {
		pCanvas->CreateClipOpAnd(&rcUpdate, 1);

        for(size_t i=0; i<__cssWndList.size(); ++i)
        {
            CRect rc; 
            if(rc.IntersectRect(&rcUpdate, __cssWndList[i]->GetBounds()) && IsObjectVisible(__cssWndList[i]->GetLayoutObj()))
            {
				
				__cssWndList[i]->DrawLayout(pCanvas, rc); 				
				
            }

        }
		pCanvas->ResetClip(NULL);
    }

	//»»Æ¤·ô£¬¸Ä±³¾°Í¼
	void ChangeSkinImage(LPCTSTR pImageName)
	{
		 for(size_t i=0; i<__cssWndList.size(); ++i)
		 {
			XCssLayoutWnd* pWnd = __cssWndList[i];
			xlayout::ILayoutObj* pObj = pWnd->GetLayoutObj();
			if(pObj)
			{
				if(pObj->GetProperty(_T("changeskin_image")))
					pWnd->ChangeSkinImage(pImageName);
			}
		 }
	}

	//»»Æ¤·ô£¬¸Ä±³¾°É«
	void ChangeSkinBkColor(LPCTSTR pColorText)
	{
		for(size_t i=0; i<__cssWndList.size(); ++i)
		{
			XCssLayoutWnd* pWnd = __cssWndList[i];
			xlayout::ILayoutObj* pObj = pWnd->GetLayoutObj();
			if(pObj)
			{
				if(pObj->GetProperty(_T("changeskin_bkcolor")))
					pWnd->ChangeSkinBkColor(pColorText);
			}
		}
	}

    virtual BOOL OnLayoutEvent(int nType, xlayout::ILayoutObj* pObj)
    {
        if(xlayout::CLayoutMgrWindow<T>::OnLayoutEvent(nType, pObj))
        {
            if(!m_hDwp)
            {
                CClientRect rcClient(m_hWnd);
                InvalidateRect(rcClient, FALSE);
            }
        }
        return TRUE;
    }

    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        return CLayoutMgrWindow<T>::OnSize(uMsg, wParam, lParam, bHandled);
    }

protected:
    typedef std::map<xstring, XCssLayoutWnd*>	  CssWndMap;
    typedef std::vector<XCssLayoutWnd*>			  CssWndList;
    CssWndMap		__cssWnds;
    CssWndList		__cssWndList;

    xskin::IResLoader* __resLoader;
};


NAMESPACE_END(xctrl)