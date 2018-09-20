/*
 * xctrl_if.h
 *
 */
#pragma once

#include <CommCtrl.h>

#include "xbase.h"
#include "xgdi/xgdi.h"
#include "xobject.h"

#define MSG_COMBOBOX_CHANGESELECT    (WM_USER + 1923)
#define WM_CHILD_DRAW_BKGRD (WM_USER + 1922)

NAMESPACE_BEGIN(xctrl)


using namespace xgdi;

enum WINDOW_LESS_IID {
    IID_Button                  ,
    IID_Label                   ,
    IID_Picture                 ,
    IID_ScrollBar               ,
    IID_TabCtrl                 ,
    IID_TreeItem                ,
    IID_TreeViewCtrl            ,
	IID_ListViewCtrl			,

    IID_Max
};

enum DRAW_ITEM_FLAG {
    DIF_SELECTED        = 0x00000001,//ODS_SELECTED
    DIF_GRAYED          = 0x00000002,//ODS_GRAYED
    DIF_DISABLED        = 0x00000004,//ODS_DISABLED
    DIF_CHECKED         = 0x00000008,//ODS_CHECKED
    DIF_FOCUSED         = 0x00000010,//ODS_FOCUS
    DIF_HIGHLIGHTED     = 0x00000020,
    DIF_DROP			= 0x00000040,

    DIF_EXPANDED        = 0x00001000,
    DIF_PUSHLIKE        = 0x00002000
};

struct IParser;
struct ICssParser;

struct IWindowHandler;
struct IWindowLessObject;
struct IWindowLessHost;

struct IButton;
struct IButtonHandler;
struct IItem;
struct IItemHandler;
struct IScrollBar;
struct IScrollHost;
struct ITabCtrl;
struct ITreeItem;
struct ITreeItemHandler;
struct ITreeViewCtrl;

struct IParser
    : public IObject
{
    //************************************
    // Method:    Parse
    // FullName:  xctrl::IParser::Parse
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IProperty *
    // Desc:      脚本处理
    //************************************
    virtual BOOL                Parse(__in IProperty*)                                                      = 0;
};

struct ICssParser
    : public IParser
{
    //************************************
    // Method:    ParseCss
    // FullName:  xctrl::ICssParser::ParseCss
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IProperty *
    // Desc:      处理内嵌样式
    //************************************
    virtual BOOL                ParseCss(__in IProperty*)                                                   = 0;
};

struct IWindowLessObject
    : public IUIObject
{
    //************************************
    // Method:    Paint
    // FullName:  xctrl::IWindowLessObject::Paint
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ICanvas *            画布
    // Parameter: __in const RECT &         目标绘制区域
    // Parameter: __in const RECT &         Dirty区域
    // Desc:      窗口句柄无关对象的绘制
    //************************************
    virtual BOOL                Paint(__in ICanvas*, __in const RECT&, __in const RECT&)                    = 0;

    //************************************
    // Method:    GetRect
    // FullName:  xctrl::IWindowLessObject::GetRect
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __inout RECT &
    // Desc:      获取对象所在位置/大小
    //************************************
    virtual BOOL                GetRect(__inout RECT&) const                                                = 0;

    //************************************
    // Method:    InvalidateRect
    // FullName:  xctrl::IWindowLessObject::InvalidateRect
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const RECT &
    // Desc:      无效化窗口的某一区域，使其重绘
    //************************************
    virtual void                InvalidateRect(__in const RECT&)                                            = 0;

    //************************************
    // Method:    OnMessage
    // FullName:  xctrl::IWindowLessObject::OnMessage
    // Access:    virtual public 
    // Returns:   LRESULT
    // Qualifier:
    // Parameter: __in UINT                 消息号
    // Parameter: __in WPARAM               参数1
    // Parameter: __in LPARAM               参数2
    // Parameter: __out BOOL &              是否被处理(TRUE-被处理，FALSE-未被处理)
    // Desc:      响应窗口事件
    //************************************
    virtual LRESULT             OnMessage(__in UINT, __in WPARAM, __in LPARAM, __out BOOL&)                 = 0;

    //************************************
    // Method:    SetSite
    // FullName:  xctrl::IWindowLessObject::SetSite
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IWindowLessHost *
    // Desc:      设置对象所在的HostSite
    //************************************
    virtual void                SetSite(__in IWindowLessHost*)                                              = 0;
};

struct IWindowLessHost
{
    //************************************
    // Method:    Register
    // FullName:  xctrl::IWindowLessHost::Register
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IWindowLessObject *
    // Desc:      注册一个无窗口对象，使其位于当前Site管理下
    //************************************
    virtual void                Register(__in IWindowLessObject*)                                           = 0;

    //************************************
    // Method:    UnRegister
    // FullName:  xctrl::IWindowLessHost::UnRegister
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IWindowLessObject *
    // Desc:      注销一个无窗口对象
    //************************************
    virtual void                UnRegister(__in IWindowLessObject*)                                         = 0;

    //************************************
    // Method:    Invalidate
    // FullName:  xctrl::IWindowLessHost::Invalidate
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const RECT &         要更新的区域
    // Desc:      更新Site指定区域内容
    //************************************
    virtual void                Invalidate(__in const RECT&)                                                = 0;
};

struct IWindowHandler
    : public IObject
{
    //************************************
    // Method:    Clone
    // FullName:  xctrl::IWindowHandler::Clone
    // Access:    virtual public 
    // Returns:   xctrl::IWindowHandler*
    // Qualifier:
    //************************************
    virtual IWindowHandler*     Clone()                                                                     = 0;

    //************************************
    // Method:    OnMessage
    // FullName:  xctrl::IWindowHandler::OnMessage
    // Access:    virtual public 
    // Returns:   LRESULT
    // Qualifier:
    // Parameter: __in UINT
    // Parameter: __in WPARAM
    // Parameter: __in LPARAM
    // Parameter: __inout BOOL &
    //************************************
    virtual LRESULT             OnMessage(__in UINT, __in WPARAM, __in LPARAM, __inout BOOL&)               = 0;
};

struct IItemHandler
    : public IWindowHandler
{
    //************************************
    // Method:    CalcItemSize
    // FullName:  xctrl::IItemHandler::CalcItemSize
    // Access:    virtual public 
    // Returns:   const CSize&
    // Qualifier:
    // Parameter: __in ICanvas *            传入的画布参数，通常用来计算TextItem实际大小
    // Desc:      计算Item节点在画布中所需要的大小
    //************************************
    virtual const SIZE&	        CalcItemSize(__in ICanvas*)                                         = 0;

    //************************************
    // Method:    Draw
    // FullName:  xctrl::IItemHandler::Draw
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ICanvas *            节点绘制的画布
    // Parameter: __in const RECT &         坐标系相对于画布的Item节点完整矩形区域
    // Parameter: __in const RECT &         坐标系相对于Item自身的脏矩形区域
    // Parameter: int                       DRAW_ITEM_FLAG标志位组合
    // Desc:      绘制节点的内容
    //************************************
    virtual void	            Draw(__in ICanvas*, __in const RECT&, __in const RECT&, __in int)   = 0;

    //************************************
    // Method:    GetItemText
    // FullName:  xctrl::IItemHandler::GetItemText
    // Access:    virtual public 
    // Returns:   LPCTSTR
    // Qualifier: const
    // Desc:      获取节点字符串
    //************************************
    virtual LPCTSTR             GetItemText() const												    = 0;

    //************************************
    // Method:    SetItemText
    // FullName:  xctrl::IItemHandler::SetItemText
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in LPCTSTR              要设置的节点字符串
    // Desc:      设置节点字符串
    //************************************
    virtual void	            SetItemText(__in LPCTSTR)										    = 0;

	HWND						GetItemHwnd() { return m_hWnd;}
	HWND						m_hWnd;
};

struct IItem
    : public IWindowLessObject
{
    //************************************
    // Method:    GetItemSelected
    // FullName:  xctrl::ITreeItem::GetItemSelected
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      判断项目是否为选中状态
    //************************************
    virtual BOOL	GetItemSelected() const							= 0;

    //************************************
    // Method:    GetItemHighlighted
    // FullName:  xctrl::ITreeItem::GetItemHighlighted
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      判断项目是否为高亮状态
    //************************************
    virtual BOOL	GetItemHighlighted() const						= 0;

    //************************************
    // Method:    GetItemFocused
    // FullName:  xctrl::ITreeItem::GetItemFocused
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      判断项目是否具有焦点
    //************************************
    virtual BOOL	GetItemFocused() const							= 0;

    //************************************
    // Method:    SetItemFocused
    // FullName:  xctrl::IItem::SetItemFocused
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in BOOL
    // Desc:      设置项目具有焦点
    //************************************
    virtual void    SetItemFocused(__in BOOL)                       = 0;
};

struct IButton
    : public IItem
{
    //************************************
    // Method:    GetItemChecked
    // FullName:  xctrl::IButton::GetItemChecked
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      判断项目是否为选中状态
    //************************************
    virtual BOOL                GetItemChecked() const                                              = 0;

    //************************************
    // Method:    GetHandler
    // FullName:  xctrl::IButton::GetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: const
    // Desc:      获取节点处理器
    //************************************
    virtual IButtonHandler*     GetHandler() const                                                  = 0;

    //************************************
    // Method:    SetButtonCheck
    // FullName:  xctrl::IButton::SetButtonCheck
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int
    // Desc:      设置按钮选中状态
    //************************************
    virtual int                 SetButtonCheck(__in int)                                            = 0;

    //************************************
    // Method:    SetHandler
    // FullName:  xctrl::ITreeItem::SetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IButtonHandler *   要设置的节点处理句柄（内部复制，可以即时释放）
    // Desc:      设置节点处理句柄
    //************************************
    virtual void	            SetHandler(__in IButtonHandler*)				                    = 0;
};

struct IButtonHandler
    : public IItemHandler
{
	virtual xgdi::IFont* GetTextFont() = 0;
    virtual void                SetItem(__in IButton*)                                              = 0;

    virtual int                 UpdateCheckButton(__in int)                                         = 0;
};

struct IScrollBar
    : public IWindowLessObject
{
    //************************************
    // Method:    GetScrollInfo
    // FullName:  xctrl::IScrollBar::GetScrollInfo
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __inout LPSCROLLINFO
    // Desc:      获取ScrollBar信息
    //************************************
    virtual int                 GetScrollInfo(__inout LPSCROLLINFO) const                           = 0;

    //************************************
    // Method:    SetExtHandler
    // FullName:  xctrl::IScrollBar::SetExtHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: IWindowHandler *
    // Parameter: __in int                      方向SB_HORZ/SB_VERT
    // Desc:      设置外部Scroll事件通知接收者
    //************************************
    virtual void                SetExtHandler(__in IWindowHandler*, __in int)                       = 0;

    //************************************
    // Method:    SetItemHandler
    // FullName:  xctrl::IScrollBar::SetItemHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IItemHandler *
    // Parameter: __in int                      0 left/top 1 thumb 2 right/bottom 3 bkgnd
    // Desc:      设置Scroll按钮处理器
    //************************************
    virtual void                SetItemHandler(__in IItemHandler*, __in int)                        = 0;

    //************************************
    // Method:    SetScrollCapture
    // FullName:  xctrl::IScrollBar::SetScrollCapture
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in BOOL
    // Desc:      设置Scroll是否为捕获状态
    //************************************
    virtual BOOL                SetScrollCapture(__in BOOL)                                         = 0;

    //************************************
    // Method:    SetScrollInfo
    // FullName:  xctrl::IScrollBar::SetScrollInfo
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in LPSCROLLINFO
    // Desc:      设置ScrollBar信息
    //************************************
    virtual int                 SetScrollInfo(__in LPSCROLLINFO)                                    = 0;

	virtual	int					SetScrollInfoNotThnmb(__in LPSCROLLINFO)                            = 0;

    //************************************
    // Method:    ShowScrollBar
    // FullName:  xctrl::IScrollBar::ShowScrollBar
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in BOOL
    // Desc:      显示/隐藏ScrollBar
    //************************************
    virtual BOOL                ShowScrollBar(__in BOOL)                                            = 0;

	virtual void				SetBkImage(IImage* pImage)											= 0;

	virtual void				SetThumbLength(int nLen)											= 0;
};

struct IScrollHost
{
    //************************************
    // Method:    CalcScrollHost
    // FullName:  xctrl::IScrollHost::CalcScrollHost
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __inout RECT &            输入原始大小，返回ScrollHost外的位置
    // Desc:      更新ScrollHost位置
    //************************************
    virtual BOOL                CalcScrollHost(__inout RECT&)                                       = 0;

    //************************************
    // Method:    GetScrollBar
    // FullName:  xctrl::IScrollHost::GetScrollBar
    // Access:    virtual public 
    // Returns:   IScrollBar*
    // Qualifier:
    // Parameter: __in int                  方向 SB_VERT/SB_HORZ
    // Desc:      获取滚动条
    //************************************
    virtual IScrollBar*         GetScrollBar(__in int)                                              = 0;

    //************************************
    // Method:    OnScroll
    // FullName:  xctrl::IScrollHost::OnScroll
    // Access:    virtual public 
    // Returns:   LRESULT
    // Qualifier:
    // Parameter: __in int                  方向 SB_VERT/SB_HORZ
    // Parameter: __in int                  滚动事件
    // Desc:      响应滚动条事件
    //************************************
    virtual LRESULT             OnScroll(__in int, __in int)                                        = 0;

    //************************************
    // Method:    SetScrollBar
    // FullName:  xctrl::IScrollHost::SetScrollBar
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int                  方向 SB_VERT/SB_HORZ
    // Parameter: __in IScrollBar *         滚动条
    // Desc:      设置滚动条
    //************************************
    virtual void                SetScrollBar(__in int, __in IScrollBar*)                            = 0;

    //************************************
    // Method:    ShowScrollBar
    // FullName:  xctrl::IScrollHost::ShowScrollBar
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int                  方向 SB_VERT/SB_HORZ
    // Parameter: __in int                  是否显示 0-disabled, 1-auto, 2-always
    // Parameter: __in int                  宽度/高度
    // Desc:      设置滚动条显示状态
    //************************************
    virtual void                ShowScrollBar(__in int, __in int, __in int)                         = 0;

	//************************************
	// Method:    UpdateScrollBar
	// FullName:  xctrl::IScrollHost::UpdateScrollBar
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in int                  方向 SB_VERT/SB_HORZ
    // Desc:      更新滚动条
	//************************************
	virtual void                UpdateScrollBar(__in int)                                           = 0;
};

struct ITabCtrl
    : public IWindowLessObject
{
    //************************************
    // Method:    DeleteItem
    // FullName:  xctrl::ITabCtrl::DeleteItem
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int
    // Desc:      删除某一指定项
    //************************************
    virtual void                DeleteItem(__in int)                                                = 0;

    //************************************
    // Method:    GetItemCount
    // FullName:  xctrl::ITabCtrl::GetItemCount
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      获取TabCtrl中的项目个数
    //************************************
    virtual int                 GetItemCount() const                                                = 0;

    //************************************
    // Method:    GetCurSel
    // FullName:  xctrl::ITabCtrl::GetCurSel
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      获取当前选中项的Index
    //************************************
    virtual int                 GetCurSel() const                                                   = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITabCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int                  插入项的索引位置(0-head, 1-tail)
    // Parameter: __in IItemHandler *       插入项的事件处理器
    // Desc:      向TabCtrl插入一项
    //************************************
    virtual int                 InsertItem(__in int, __in IItemHandler*)                            = 0;

    //************************************
    // Method:    SetCurSel
    // FullName:  xctrl::ITabCtrl::SetCurSel
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int
    // Desc:      设置当前选中的项
    //************************************
    virtual void                SetCurSel(__in int)                                                 = 0;
};

struct ITreeItem
    : public IItem
{
    //************************************
    // Method:    GetHandler
    // FullName:  xctrl::ITreeItem::GetHandler
    // Access:    virtual public 
    // Returns:   ITreeItemHandler*
    // Qualifier: const
    // Desc:      获取节点处理器
    //************************************
    virtual ITreeItemHandler*   GetHandler() const                              = 0;

    //************************************
    // Method:    GetItemExpand
    // FullName:  xctrl::ITreeItem::GetItemExpand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      判断项目是否为展开状态
    //************************************
    virtual BOOL                GetItemExpand() const                           = 0;

	//************************************
    // Method:    GetItemExpand
    // FullName:  xctrl::ITreeItem::GetItemExpand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      设定项目是否为展开状态
    //************************************
	virtual BOOL       SetItemExpand(BOOL bExpand)								= 0;
    //************************************
    // Method:    GetTreeView
    // FullName:  xctrl::ITreeItem::GetTreeView
    // Access:    virtual public 
    // Returns:   ITreeViewCtrl*
    // Qualifier: const
    // Desc:      获取节点所在树
    //************************************
    virtual ITreeViewCtrl*      GetTreeView() const                             = 0;

    //************************************
    // Method:    SetHandler
    // FullName:  xctrl::ITreeItem::SetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ITreeItemHandler *   要设置的节点处理句柄（内部复制，可以即时释放）
    // Desc:      设置节点处理句柄
    //************************************
    virtual void	            SetHandler(__in ITreeItemHandler*)				= 0;
};

struct ITreeItemHandler
    : public IItemHandler
{
    //************************************
    // Method:    Compare
    // FullName:  xctrl::ITreeItemHandler::Compare
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      返回当前与指定节点的比较结果
    //************************************
    virtual int                 Compare(__in const ITreeItem*) const                                = 0;

    //************************************
    // Method:    SetItem
    // FullName:  xctrl::ITreeItemHandler::SetItem
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      设定Handler所处理的当前Item节点
    //************************************
    virtual void	            SetItem(__in ITreeItem*)										    = 0;

	virtual void				SetItemData(DWORD nDate){};
	virtual DWORD				GetItemData( ){return 0;};
}; 


#define		TVN_GT_DRAGGING		1
#define		TVN_GT_DRAGEND		2
#define		TVN_GT_MENU			3

#define		TREEITEM_ONLY_SHOW    1 //树节点显示出来，靠上显示在顶部，靠下显示在底部
#define		TREEITEM_TO_TOP		2 //如果有部分被挡住则显示在最上面，如果全部显示了，就不靠上
#define		TREEITEM_TO_BOTTOM  3 //如果有部分被挡住则显示在最下面，如果全部显示了，就不靠下 
#define		TREEITEM_MUST_TO_TOP 4 //不管节点在哪，都让其显示在最上面，除非滚动条无法滚动
#define		TREEITEM_MUST_TO_BOTTOM 5 //不管节点在哪，都让其显示在最下面，除非滚动条无法滚动
struct ITreeViewCtrl
    : public IWindowLessObject
    , public IScrollHost
{
	virtual void SetFillAllClient(BOOL bFill) = 0;
    //************************************
    // Method:    Clear
    // FullName:  xctrl::ITreeViewCtrl::Clear
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Desc::     清空树的所有节点
    //************************************
    virtual void                Clear()                                                                     = 0;

    //************************************
    // Method:    DeleteItem
    // FullName:  xctrl::ITreeViewCtrl::DeleteItem
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      删除一个树节点
    //************************************
    virtual BOOL                DeleteItem(__in ITreeItem*)                                                 = 0;

    //************************************
    // Method:    Expand
    // FullName:  xctrl::ITreeViewCtrl::Expand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in_opt ITreeItem        指定的树节点（默认NULL则为根节点）
    // Parameter: __in_opt UINT             展开/收起动作（默认展开TVE_EXPAND）
    // Desc:      递归展开/收起一个树节点的所有子节点
    //************************************
    virtual BOOL                Expand(__in_opt ITreeItem*, __in_opt UINT)                                  = 0;

    //************************************
    // Method:    FindItem
    // FullName:  xctrl::ITreeViewCtrl::FindItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              节点标识
    // Parameter: __in ITreeItem*           要查找的父节点，为NULL则从根开始查找
    // Desc:      查找树中的某个节点
    //************************************
    virtual ITreeItem*          FindItem(__in LPCTSTR, __in_opt ITreeItem*)                                 = 0;

    //************************************
    // Method:    FindItem
    // FullName:  xctrl::ITreeViewCtrl::FindItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              节点标识
    // Parameter: __in LPCTSTR              父节点标识
    // Parameter: __in_opt ITreeItem *      指定从查找初始位置，为NULL则从根开始查找
    // Desc:      查找树中的指定父节点的某个节点
    //************************************
    virtual ITreeItem*          FindItem(__in LPCTSTR, __in LPCTSTR, __in_opt ITreeItem*)                   = 0;

    //************************************
    // Method:    GetCurrentSel
    // FullName:  xctrl::ITreeViewCtrl::GetCurrentSel
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      获取当前选中节点
    //************************************
    virtual ITreeItem*          GetCurrentSel()                                                             = 0;

	//************************************
    // Method:    GetPreSel
    // FullName:  xctrl::ITreeViewCtrl::GetPreSel
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      获取前一个选中节点
    //************************************
    virtual ITreeItem*          GetPreSel()                                                             = 0;


	//************************************
	// Method:    GetDropHilightItem
	// FullName:  xctrl::ITreeViewCtrl::GetDropHilightItem
	// Access:    virtual public 
	// Returns:   ITreeItem*
	// Qualifier:
	// Desc:      获取当前拖拽落点节点<wyj 2010-04-27>
	//************************************
	virtual ITreeItem*          GetDropHilightItem()                                                        = 0;

    //************************************
    // Method:    GetFirstChild
    // FullName:  xctrl::ITreeViewCtrl::GetFirstChild
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点第一个直接子节点
    //************************************
    virtual ITreeItem*          GetFirstChild(__in ITreeItem*)                                              = 0;

    //************************************
    // Method:    GetLastChild
    // FullName:  xctrl::ITreeViewCtrl::GetLastChild
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点最后一个直接子节点
    //************************************
    virtual ITreeItem*          GetLastChild(__in ITreeItem*)                                               = 0;

    //************************************
    // Method:    GetNext
    // FullName:  xctrl::ITreeViewCtrl::GetNext
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点的下一个节点（前序遍历顺序）
    //************************************
    virtual ITreeItem*          GetNext(__in ITreeItem*)                                                    = 0;

    //************************************
    // Method:    GetNextSibling
    // FullName:  xctrl::ITreeViewCtrl::GetNextSibling
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点下一个同级节点
    //************************************
    virtual ITreeItem*          GetNextSibling(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetNextVisible
    // FullName:  xctrl::ITreeViewCtrl::GetNextVisible
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点下一个可见节点
    //************************************
    virtual ITreeItem*          GetNextVisible(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetParent
    // FullName:  xctrl::ITreeViewCtrl::GetParent
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点的父节点
    //************************************
    virtual ITreeItem*          GetParent(__in ITreeItem*)                                                  = 0;

    //************************************
    // Method:    GetPrev
    // FullName:  xctrl::ITreeViewCtrl::GetPrev
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点前一个节点（前序遍历顺序）
    //************************************
    virtual ITreeItem*          GetPrev(__in ITreeItem*)                                                    = 0;

    //************************************
    // Method:    GetPrevSibling
    // FullName:  xctrl::ITreeViewCtrl::GetPrevSibling
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点前一个同级节点
    //************************************
    virtual ITreeItem*          GetPrevSibling(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetPrevVisible
    // FullName:  xctrl::ITreeViewCtrl::GetPrevVisible
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      获取指定节点前一个可见节点
    //************************************
    virtual ITreeItem*          GetPrevVisible(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetRootItem
    // FullName:  xctrl::ITreeViewCtrl::GetRootItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      获取树根节点
    //************************************
    virtual ITreeItem*          GetRootItem()                                                               = 0;

    //************************************
    // Method:    GetVisibleCount
    // FullName:  xctrl::ITreeViewCtrl::GetVisibleCount
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier:
    // Desc:      获取当前所有可视节点
    //************************************
    virtual UINT                GetVisibleCount()                                                           = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITreeViewCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *          要插入的树节点
    // Parameter: __in_opt ITreeItem *      指定插入节点的父节点（默认NULL则为树根节点）
    // Parameter: __in_opt ITreeItem *      指定在该节点之后插入（默认NULL则为父节点的最后一个子节点）
    // Desc:      在树的指定位置插入节点
    //************************************
    virtual ITreeItem*          InsertItem(__in ITreeItem*, __in_opt ITreeItem*, __in_opt ITreeItem*)       = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITreeViewCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              要插入的树节点文本
    // Parameter: __in_opt ITreeItem *      指定插入节点的父节点（默认NULL则为树根节点）
    // Parameter: __in_opt ITreeItem *      指定在该节点之后插入（默认NULL则为父节点的最后一个子节点）
    // Desc:      在树的指定位置插入文本节点
    //************************************
    virtual ITreeItem*          InsertItem(__in LPCTSTR, __in_opt ITreeItem*, __in_opt ITreeItem*)          = 0;

    virtual ITreeItem*          InsertItem(__in ITreeItemHandler*, __in_opt ITreeItem*, __in_opt ITreeItem*)= 0;

    //************************************
    // Method:    ItemFromYPos
    // FullName:  xctrl::ITreeViewCtrl::ItemFromYPos
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in INT                  检测的垂直坐标
    // Desc:      根据垂直坐标获取当前可见树的一个节点
    //************************************
    virtual ITreeItem*			ItemFromYPos(__in INT)                                                      = 0;

    //************************************
    // Method:    ItemFromPoint
    // FullName:  xctrl::ITreeViewCtrl::ItemFromPoint
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in const POINT &        检测的点
    // Desc:      获取点所在位置的当前可见树的一个节点
    //************************************
    virtual ITreeItem*          ItemFromPoint(__in const POINT&)                                            = 0;

    //************************************
    // Method:    ItemToClient
    // FullName:  xctrl::ITreeViewCtrl::ItemToClient
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Parameter: __inout LPRECT
    // Desc:      将矩形从树节点坐标系转化为树画布的坐标系
    //************************************
    virtual BOOL                ItemToClient(__in ITreeItem*, __inout LPRECT)                               = 0;

    //************************************
    // Method:    MoveItem
    // FullName:  xctrl::ITreeViewCtrl::MoveItem
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *          指定树中的某个子节点
    // Parameter: __in ITreeItem *          要移动到的地方（NULL则仅将子树取下而不插入）
    // Desc:      将树中结点移动到指定父节点下
    //************************************
    virtual ITreeItem*          MoveItem(__in ITreeItem*, __in ITreeItem*)                                  = 0;

	

    //************************************
    // Method:    Scroll
    // FullName:  xctrl::ITreeViewCtrl::Scroll
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in INT                  水平方向滚动距离
    // Parameter: __in INT                  垂直方向滚动距离
    // Desc:      滚动树的可视区域
    //************************************
    virtual BOOL                Scroll(__in INT, __in INT)                                                  = 0;

    //************************************
    // Method:    Select
    // FullName:  xctrl::ITreeViewCtrl::Select
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in_opt ITreeItem *      要选中的节点（默认NULL则为不选中任何一个节点）
    // Parameter: __in_opt BOOL *           是否自动选中
    // Desc:      选择树的某个节点
    //************************************
    virtual BOOL                Select(__in_opt ITreeItem*, __in_opt BOOL)                                  = 0;
	
	//************************************
	// Method:    SelectDropTarget
	// FullName:  xctrl::ITreeViewCtrl::SelectDropTarget
	// Access:    virtual public 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: __in_opt ITreeItem *      要选中的节点（默认NULL则为不选中任何一个节点）
	// Desc:      选择树的拖拽落点，<wyj 2010-04-27>
	//************************************
	virtual BOOL				SelectDropTarget(__in_opt ITreeItem*)										= 0;

    //************************************
    // Method:    SetBkImage
    // FullName:  xctrl::ITreeViewCtrl::SetBkImage
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IImage *             选择的背景图
    // Desc:      设置树的背景图像（客户区有效）
    //************************************
    virtual void                SetBkImage(__in IImage*)                                                    = 0;

    //************************************
    // Method:    SetItemText
    // FullName:  xctrl::ITreeViewCtrl::SetItemText
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Parameter: __in LPCTSTR
    // Desc:      设置树节点的文本内容
    //************************************
    virtual void                SetItemText(__in ITreeItem*, __in LPCTSTR)                                  = 0;


	virtual void                SetItemHandler(__in ITreeItem *, __in ITreeItemHandler*)                    = 0;

    //************************************
    // Method:    SortChildren
    // FullName:  xctrl::ITreeViewCtrl::SortChildren
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *          需要排序子节点的树节点
    // Parameter: __in_opt BOOL             是否继续递归排序子节点（默认不进行递归排序）
    // Desc:      使用默认规则（节点文本域的字典排序）对一个树节点的所有子节点排序
    //************************************
    virtual BOOL                SortChildren(__in ITreeItem*, __in_opt BOOL)                                = 0;

    //************************************
    // Method:    SortChildrenCB
    // FullName:  xctrl::ITreeViewCtrl::SortChildrenCB
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: LPTVSORTCB                排序信息
    // Parameter: BOOL                      是否继续递归排序子节点（默认不进行递归排序）
    // Desc:      使用指定规则对一个树节点的所有子节点排序
    //************************************
    virtual BOOL                SortChildrenCB(__in LPTVSORTCB, __in_opt BOOL)                              = 0;

    virtual void                SetBatchMode(__in BOOL)                                                     = 0;
	virtual void				SetClientBkColor(COLORREF rc)		{}											;
	//*********************************
	//Desc: 让节点全部显示
	//Parameter: ITreeItem                要显示的节点
	//Parameter: int                     nShowStyl显示方式，TREEITEM_TO_SHOW,TREEITEM_TO_TOP 五种中的一种方式
	//*********************************
	virtual void				SetItemAllShow(__in ITreeItem*, int nShowStyle)   {};
};

struct IListViewCtrl : public ITreeViewCtrl 
{
	virtual void AddColumn(int nPos, LPCTSTR lpszText, int nWidth) = 0;
	virtual void SetHeadHeight(int nHeight) = 0;

	//************************************
    // Method:    MoveItem
    // FullName:  xctrl::ITreeViewCtrl::MoveItem
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *          交换的节点1
    // Parameter: __in ITreeItem *          交换的节点2
    // Desc:      交换两个节点的位置
    //************************************
    virtual bool          ChangeItem(__in ITreeItem*, __in ITreeItem*)                                  = 0;
	
};

//************************************
// Method:    CoCreateWindowLessObject
// FullName:  xctrl::CoCreateWindowLessObject
// Access:    public 
// Returns:   extern "C" IWindowLessObject*
// Qualifier:
// Parameter: WINDOW_LESS_IID iid
// Desc:      创建无窗口控件对象
//************************************
XCTRLAPI IWindowLessObject*   CreateGTUIBaseObj(WINDOW_LESS_IID iid);




NAMESPACE_END(xctrl)