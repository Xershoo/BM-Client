#pragma once
#include <map>
#include "ILayout.h"
#include "LayoutEvent.h"
#include <xstring.h>
#include "LayoutWindow.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlstatic.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlscrollbar.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlpicture.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlbutton.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtltree.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtledit.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/XEdit.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlhyperlink.h"
#include "../../../../../raidcall/thunk/include/client/xctrl/wtlslider.h"


NAMESPACE_BEGIN(xlayout)

template<class TWindow>
class LayoutWndCreator
    : public ILayoutWndCreator
{
public:
    static LayoutWndCreator<TWindow>* inst()
    {
        static LayoutWndCreator<TWindow> xx;
        return &xx;
    }

    ILayoutWindow* Create(ILayoutMgrWindow* pMgrWnd, ILayoutObj* pLayoutObj)
    {
        CWTLayoutWindow<TWindow> * pLayoutWindow = new CWTLayoutWindow<TWindow>;
        pLayoutWindow->Create(pMgrWnd->GetHWND(), NULL, pLayoutObj->GetProperty(_T("text")), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, pMgrWnd->GetCtrlIdByName(pLayoutObj->GetName()));
        return pLayoutWindow;
    }
};


inline static void InitCreators()
{
    LayoutWndFactory::inst()->RegisterCreator(_T("Static"),		LayoutWndCreator<xctrl::CStatic>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("Button"),	    LayoutWndCreator<xctrl::CButton>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("TBButton"),   LayoutWndCreator<xctrl::CButton>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("Picture"),	LayoutWndCreator<xctrl::CPicture>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("ScrollBar"),	LayoutWndCreator<xctrl::CScrollBar>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("Slider"),	    LayoutWndCreator<xctrl::CSliderBar>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("Tree"),	    LayoutWndCreator<xctrl::CWTLTreeCtrl>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("Edit"),		LayoutWndCreator<xctrl::CEdit>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("RichEdit"),	LayoutWndCreator<xctrl::XCssRichEdit>::inst());
    LayoutWndFactory::inst()->RegisterCreator(_T("HyperLink"),	LayoutWndCreator<xctrl::CHyperLink>::inst());
}

NAMESPACE_END(xlayout)
