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
    // Desc:      �ű�����
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
    // Desc:      ������Ƕ��ʽ
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
    // Parameter: __in ICanvas *            ����
    // Parameter: __in const RECT &         Ŀ���������
    // Parameter: __in const RECT &         Dirty����
    // Desc:      ���ھ���޹ض���Ļ���
    //************************************
    virtual BOOL                Paint(__in ICanvas*, __in const RECT&, __in const RECT&)                    = 0;

    //************************************
    // Method:    GetRect
    // FullName:  xctrl::IWindowLessObject::GetRect
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __inout RECT &
    // Desc:      ��ȡ��������λ��/��С
    //************************************
    virtual BOOL                GetRect(__inout RECT&) const                                                = 0;

    //************************************
    // Method:    InvalidateRect
    // FullName:  xctrl::IWindowLessObject::InvalidateRect
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const RECT &
    // Desc:      ��Ч�����ڵ�ĳһ����ʹ���ػ�
    //************************************
    virtual void                InvalidateRect(__in const RECT&)                                            = 0;

    //************************************
    // Method:    OnMessage
    // FullName:  xctrl::IWindowLessObject::OnMessage
    // Access:    virtual public 
    // Returns:   LRESULT
    // Qualifier:
    // Parameter: __in UINT                 ��Ϣ��
    // Parameter: __in WPARAM               ����1
    // Parameter: __in LPARAM               ����2
    // Parameter: __out BOOL &              �Ƿ񱻴���(TRUE-������FALSE-δ������)
    // Desc:      ��Ӧ�����¼�
    //************************************
    virtual LRESULT             OnMessage(__in UINT, __in WPARAM, __in LPARAM, __out BOOL&)                 = 0;

    //************************************
    // Method:    SetSite
    // FullName:  xctrl::IWindowLessObject::SetSite
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IWindowLessHost *
    // Desc:      ���ö������ڵ�HostSite
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
    // Desc:      ע��һ���޴��ڶ���ʹ��λ�ڵ�ǰSite������
    //************************************
    virtual void                Register(__in IWindowLessObject*)                                           = 0;

    //************************************
    // Method:    UnRegister
    // FullName:  xctrl::IWindowLessHost::UnRegister
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IWindowLessObject *
    // Desc:      ע��һ���޴��ڶ���
    //************************************
    virtual void                UnRegister(__in IWindowLessObject*)                                         = 0;

    //************************************
    // Method:    Invalidate
    // FullName:  xctrl::IWindowLessHost::Invalidate
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const RECT &         Ҫ���µ�����
    // Desc:      ����Siteָ����������
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
    // Parameter: __in ICanvas *            ����Ļ���������ͨ����������TextItemʵ�ʴ�С
    // Desc:      ����Item�ڵ��ڻ���������Ҫ�Ĵ�С
    //************************************
    virtual const SIZE&	        CalcItemSize(__in ICanvas*)                                         = 0;

    //************************************
    // Method:    Draw
    // FullName:  xctrl::IItemHandler::Draw
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ICanvas *            �ڵ���ƵĻ���
    // Parameter: __in const RECT &         ����ϵ����ڻ�����Item�ڵ�������������
    // Parameter: __in const RECT &         ����ϵ�����Item��������������
    // Parameter: int                       DRAW_ITEM_FLAG��־λ���
    // Desc:      ���ƽڵ������
    //************************************
    virtual void	            Draw(__in ICanvas*, __in const RECT&, __in const RECT&, __in int)   = 0;

    //************************************
    // Method:    GetItemText
    // FullName:  xctrl::IItemHandler::GetItemText
    // Access:    virtual public 
    // Returns:   LPCTSTR
    // Qualifier: const
    // Desc:      ��ȡ�ڵ��ַ���
    //************************************
    virtual LPCTSTR             GetItemText() const												    = 0;

    //************************************
    // Method:    SetItemText
    // FullName:  xctrl::IItemHandler::SetItemText
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in LPCTSTR              Ҫ���õĽڵ��ַ���
    // Desc:      ���ýڵ��ַ���
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
    // Desc:      �ж���Ŀ�Ƿ�Ϊѡ��״̬
    //************************************
    virtual BOOL	GetItemSelected() const							= 0;

    //************************************
    // Method:    GetItemHighlighted
    // FullName:  xctrl::ITreeItem::GetItemHighlighted
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      �ж���Ŀ�Ƿ�Ϊ����״̬
    //************************************
    virtual BOOL	GetItemHighlighted() const						= 0;

    //************************************
    // Method:    GetItemFocused
    // FullName:  xctrl::ITreeItem::GetItemFocused
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      �ж���Ŀ�Ƿ���н���
    //************************************
    virtual BOOL	GetItemFocused() const							= 0;

    //************************************
    // Method:    SetItemFocused
    // FullName:  xctrl::IItem::SetItemFocused
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in BOOL
    // Desc:      ������Ŀ���н���
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
    // Desc:      �ж���Ŀ�Ƿ�Ϊѡ��״̬
    //************************************
    virtual BOOL                GetItemChecked() const                                              = 0;

    //************************************
    // Method:    GetHandler
    // FullName:  xctrl::IButton::GetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier: const
    // Desc:      ��ȡ�ڵ㴦����
    //************************************
    virtual IButtonHandler*     GetHandler() const                                                  = 0;

    //************************************
    // Method:    SetButtonCheck
    // FullName:  xctrl::IButton::SetButtonCheck
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int
    // Desc:      ���ð�ťѡ��״̬
    //************************************
    virtual int                 SetButtonCheck(__in int)                                            = 0;

    //************************************
    // Method:    SetHandler
    // FullName:  xctrl::ITreeItem::SetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IButtonHandler *   Ҫ���õĽڵ㴦�������ڲ����ƣ����Լ�ʱ�ͷţ�
    // Desc:      ���ýڵ㴦����
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
    // Desc:      ��ȡScrollBar��Ϣ
    //************************************
    virtual int                 GetScrollInfo(__inout LPSCROLLINFO) const                           = 0;

    //************************************
    // Method:    SetExtHandler
    // FullName:  xctrl::IScrollBar::SetExtHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: IWindowHandler *
    // Parameter: __in int                      ����SB_HORZ/SB_VERT
    // Desc:      �����ⲿScroll�¼�֪ͨ������
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
    // Desc:      ����Scroll��ť������
    //************************************
    virtual void                SetItemHandler(__in IItemHandler*, __in int)                        = 0;

    //************************************
    // Method:    SetScrollCapture
    // FullName:  xctrl::IScrollBar::SetScrollCapture
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in BOOL
    // Desc:      ����Scroll�Ƿ�Ϊ����״̬
    //************************************
    virtual BOOL                SetScrollCapture(__in BOOL)                                         = 0;

    //************************************
    // Method:    SetScrollInfo
    // FullName:  xctrl::IScrollBar::SetScrollInfo
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in LPSCROLLINFO
    // Desc:      ����ScrollBar��Ϣ
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
    // Desc:      ��ʾ/����ScrollBar
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
    // Parameter: __inout RECT &            ����ԭʼ��С������ScrollHost���λ��
    // Desc:      ����ScrollHostλ��
    //************************************
    virtual BOOL                CalcScrollHost(__inout RECT&)                                       = 0;

    //************************************
    // Method:    GetScrollBar
    // FullName:  xctrl::IScrollHost::GetScrollBar
    // Access:    virtual public 
    // Returns:   IScrollBar*
    // Qualifier:
    // Parameter: __in int                  ���� SB_VERT/SB_HORZ
    // Desc:      ��ȡ������
    //************************************
    virtual IScrollBar*         GetScrollBar(__in int)                                              = 0;

    //************************************
    // Method:    OnScroll
    // FullName:  xctrl::IScrollHost::OnScroll
    // Access:    virtual public 
    // Returns:   LRESULT
    // Qualifier:
    // Parameter: __in int                  ���� SB_VERT/SB_HORZ
    // Parameter: __in int                  �����¼�
    // Desc:      ��Ӧ�������¼�
    //************************************
    virtual LRESULT             OnScroll(__in int, __in int)                                        = 0;

    //************************************
    // Method:    SetScrollBar
    // FullName:  xctrl::IScrollHost::SetScrollBar
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int                  ���� SB_VERT/SB_HORZ
    // Parameter: __in IScrollBar *         ������
    // Desc:      ���ù�����
    //************************************
    virtual void                SetScrollBar(__in int, __in IScrollBar*)                            = 0;

    //************************************
    // Method:    ShowScrollBar
    // FullName:  xctrl::IScrollHost::ShowScrollBar
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int                  ���� SB_VERT/SB_HORZ
    // Parameter: __in int                  �Ƿ���ʾ 0-disabled, 1-auto, 2-always
    // Parameter: __in int                  ���/�߶�
    // Desc:      ���ù�������ʾ״̬
    //************************************
    virtual void                ShowScrollBar(__in int, __in int, __in int)                         = 0;

	//************************************
	// Method:    UpdateScrollBar
	// FullName:  xctrl::IScrollHost::UpdateScrollBar
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in int                  ���� SB_VERT/SB_HORZ
    // Desc:      ���¹�����
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
    // Desc:      ɾ��ĳһָ����
    //************************************
    virtual void                DeleteItem(__in int)                                                = 0;

    //************************************
    // Method:    GetItemCount
    // FullName:  xctrl::ITabCtrl::GetItemCount
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      ��ȡTabCtrl�е���Ŀ����
    //************************************
    virtual int                 GetItemCount() const                                                = 0;

    //************************************
    // Method:    GetCurSel
    // FullName:  xctrl::ITabCtrl::GetCurSel
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      ��ȡ��ǰѡ�����Index
    //************************************
    virtual int                 GetCurSel() const                                                   = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITabCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int                  �����������λ��(0-head, 1-tail)
    // Parameter: __in IItemHandler *       ��������¼�������
    // Desc:      ��TabCtrl����һ��
    //************************************
    virtual int                 InsertItem(__in int, __in IItemHandler*)                            = 0;

    //************************************
    // Method:    SetCurSel
    // FullName:  xctrl::ITabCtrl::SetCurSel
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in int
    // Desc:      ���õ�ǰѡ�е���
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
    // Desc:      ��ȡ�ڵ㴦����
    //************************************
    virtual ITreeItemHandler*   GetHandler() const                              = 0;

    //************************************
    // Method:    GetItemExpand
    // FullName:  xctrl::ITreeItem::GetItemExpand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      �ж���Ŀ�Ƿ�Ϊչ��״̬
    //************************************
    virtual BOOL                GetItemExpand() const                           = 0;

	//************************************
    // Method:    GetItemExpand
    // FullName:  xctrl::ITreeItem::GetItemExpand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier: const
    // Desc:      �趨��Ŀ�Ƿ�Ϊչ��״̬
    //************************************
	virtual BOOL       SetItemExpand(BOOL bExpand)								= 0;
    //************************************
    // Method:    GetTreeView
    // FullName:  xctrl::ITreeItem::GetTreeView
    // Access:    virtual public 
    // Returns:   ITreeViewCtrl*
    // Qualifier: const
    // Desc:      ��ȡ�ڵ�������
    //************************************
    virtual ITreeViewCtrl*      GetTreeView() const                             = 0;

    //************************************
    // Method:    SetHandler
    // FullName:  xctrl::ITreeItem::SetHandler
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ITreeItemHandler *   Ҫ���õĽڵ㴦�������ڲ����ƣ����Լ�ʱ�ͷţ�
    // Desc:      ���ýڵ㴦����
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
    // Desc:      ���ص�ǰ��ָ���ڵ�ıȽϽ��
    //************************************
    virtual int                 Compare(__in const ITreeItem*) const                                = 0;

    //************************************
    // Method:    SetItem
    // FullName:  xctrl::ITreeItemHandler::SetItem
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      �趨Handler������ĵ�ǰItem�ڵ�
    //************************************
    virtual void	            SetItem(__in ITreeItem*)										    = 0;

	virtual void				SetItemData(DWORD nDate){};
	virtual DWORD				GetItemData( ){return 0;};
}; 


#define		TVN_GT_DRAGGING		1
#define		TVN_GT_DRAGEND		2
#define		TVN_GT_MENU			3

#define		TREEITEM_ONLY_SHOW    1 //���ڵ���ʾ������������ʾ�ڶ�����������ʾ�ڵײ�
#define		TREEITEM_TO_TOP		2 //����в��ֱ���ס����ʾ�������棬���ȫ����ʾ�ˣ��Ͳ�����
#define		TREEITEM_TO_BOTTOM  3 //����в��ֱ���ס����ʾ�������棬���ȫ����ʾ�ˣ��Ͳ����� 
#define		TREEITEM_MUST_TO_TOP 4 //���ܽڵ����ģ���������ʾ�������棬���ǹ������޷�����
#define		TREEITEM_MUST_TO_BOTTOM 5 //���ܽڵ����ģ���������ʾ�������棬���ǹ������޷�����
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
    // Desc::     ����������нڵ�
    //************************************
    virtual void                Clear()                                                                     = 0;

    //************************************
    // Method:    DeleteItem
    // FullName:  xctrl::ITreeViewCtrl::DeleteItem
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ɾ��һ�����ڵ�
    //************************************
    virtual BOOL                DeleteItem(__in ITreeItem*)                                                 = 0;

    //************************************
    // Method:    Expand
    // FullName:  xctrl::ITreeViewCtrl::Expand
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in_opt ITreeItem        ָ�������ڵ㣨Ĭ��NULL��Ϊ���ڵ㣩
    // Parameter: __in_opt UINT             չ��/��������Ĭ��չ��TVE_EXPAND��
    // Desc:      �ݹ�չ��/����һ�����ڵ�������ӽڵ�
    //************************************
    virtual BOOL                Expand(__in_opt ITreeItem*, __in_opt UINT)                                  = 0;

    //************************************
    // Method:    FindItem
    // FullName:  xctrl::ITreeViewCtrl::FindItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              �ڵ��ʶ
    // Parameter: __in ITreeItem*           Ҫ���ҵĸ��ڵ㣬ΪNULL��Ӹ���ʼ����
    // Desc:      �������е�ĳ���ڵ�
    //************************************
    virtual ITreeItem*          FindItem(__in LPCTSTR, __in_opt ITreeItem*)                                 = 0;

    //************************************
    // Method:    FindItem
    // FullName:  xctrl::ITreeViewCtrl::FindItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              �ڵ��ʶ
    // Parameter: __in LPCTSTR              ���ڵ��ʶ
    // Parameter: __in_opt ITreeItem *      ָ���Ӳ��ҳ�ʼλ�ã�ΪNULL��Ӹ���ʼ����
    // Desc:      �������е�ָ�����ڵ��ĳ���ڵ�
    //************************************
    virtual ITreeItem*          FindItem(__in LPCTSTR, __in LPCTSTR, __in_opt ITreeItem*)                   = 0;

    //************************************
    // Method:    GetCurrentSel
    // FullName:  xctrl::ITreeViewCtrl::GetCurrentSel
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      ��ȡ��ǰѡ�нڵ�
    //************************************
    virtual ITreeItem*          GetCurrentSel()                                                             = 0;

	//************************************
    // Method:    GetPreSel
    // FullName:  xctrl::ITreeViewCtrl::GetPreSel
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      ��ȡǰһ��ѡ�нڵ�
    //************************************
    virtual ITreeItem*          GetPreSel()                                                             = 0;


	//************************************
	// Method:    GetDropHilightItem
	// FullName:  xctrl::ITreeViewCtrl::GetDropHilightItem
	// Access:    virtual public 
	// Returns:   ITreeItem*
	// Qualifier:
	// Desc:      ��ȡ��ǰ��ק���ڵ�<wyj 2010-04-27>
	//************************************
	virtual ITreeItem*          GetDropHilightItem()                                                        = 0;

    //************************************
    // Method:    GetFirstChild
    // FullName:  xctrl::ITreeViewCtrl::GetFirstChild
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ��һ��ֱ���ӽڵ�
    //************************************
    virtual ITreeItem*          GetFirstChild(__in ITreeItem*)                                              = 0;

    //************************************
    // Method:    GetLastChild
    // FullName:  xctrl::ITreeViewCtrl::GetLastChild
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ����һ��ֱ���ӽڵ�
    //************************************
    virtual ITreeItem*          GetLastChild(__in ITreeItem*)                                               = 0;

    //************************************
    // Method:    GetNext
    // FullName:  xctrl::ITreeViewCtrl::GetNext
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ����һ���ڵ㣨ǰ�����˳��
    //************************************
    virtual ITreeItem*          GetNext(__in ITreeItem*)                                                    = 0;

    //************************************
    // Method:    GetNextSibling
    // FullName:  xctrl::ITreeViewCtrl::GetNextSibling
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ���һ��ͬ���ڵ�
    //************************************
    virtual ITreeItem*          GetNextSibling(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetNextVisible
    // FullName:  xctrl::ITreeViewCtrl::GetNextVisible
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ���һ���ɼ��ڵ�
    //************************************
    virtual ITreeItem*          GetNextVisible(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetParent
    // FullName:  xctrl::ITreeViewCtrl::GetParent
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ�ĸ��ڵ�
    //************************************
    virtual ITreeItem*          GetParent(__in ITreeItem*)                                                  = 0;

    //************************************
    // Method:    GetPrev
    // FullName:  xctrl::ITreeViewCtrl::GetPrev
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ�ǰһ���ڵ㣨ǰ�����˳��
    //************************************
    virtual ITreeItem*          GetPrev(__in ITreeItem*)                                                    = 0;

    //************************************
    // Method:    GetPrevSibling
    // FullName:  xctrl::ITreeViewCtrl::GetPrevSibling
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ�ǰһ��ͬ���ڵ�
    //************************************
    virtual ITreeItem*          GetPrevSibling(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetPrevVisible
    // FullName:  xctrl::ITreeViewCtrl::GetPrevVisible
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *
    // Desc:      ��ȡָ���ڵ�ǰһ���ɼ��ڵ�
    //************************************
    virtual ITreeItem*          GetPrevVisible(__in ITreeItem*)                                             = 0;

    //************************************
    // Method:    GetRootItem
    // FullName:  xctrl::ITreeViewCtrl::GetRootItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Desc:      ��ȡ�����ڵ�
    //************************************
    virtual ITreeItem*          GetRootItem()                                                               = 0;

    //************************************
    // Method:    GetVisibleCount
    // FullName:  xctrl::ITreeViewCtrl::GetVisibleCount
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier:
    // Desc:      ��ȡ��ǰ���п��ӽڵ�
    //************************************
    virtual UINT                GetVisibleCount()                                                           = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITreeViewCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in ITreeItem *          Ҫ��������ڵ�
    // Parameter: __in_opt ITreeItem *      ָ������ڵ�ĸ��ڵ㣨Ĭ��NULL��Ϊ�����ڵ㣩
    // Parameter: __in_opt ITreeItem *      ָ���ڸýڵ�֮����루Ĭ��NULL��Ϊ���ڵ�����һ���ӽڵ㣩
    // Desc:      ������ָ��λ�ò���ڵ�
    //************************************
    virtual ITreeItem*          InsertItem(__in ITreeItem*, __in_opt ITreeItem*, __in_opt ITreeItem*)       = 0;

    //************************************
    // Method:    InsertItem
    // FullName:  xctrl::ITreeViewCtrl::InsertItem
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in LPCTSTR              Ҫ��������ڵ��ı�
    // Parameter: __in_opt ITreeItem *      ָ������ڵ�ĸ��ڵ㣨Ĭ��NULL��Ϊ�����ڵ㣩
    // Parameter: __in_opt ITreeItem *      ָ���ڸýڵ�֮����루Ĭ��NULL��Ϊ���ڵ�����һ���ӽڵ㣩
    // Desc:      ������ָ��λ�ò����ı��ڵ�
    //************************************
    virtual ITreeItem*          InsertItem(__in LPCTSTR, __in_opt ITreeItem*, __in_opt ITreeItem*)          = 0;

    virtual ITreeItem*          InsertItem(__in ITreeItemHandler*, __in_opt ITreeItem*, __in_opt ITreeItem*)= 0;

    //************************************
    // Method:    ItemFromYPos
    // FullName:  xctrl::ITreeViewCtrl::ItemFromYPos
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in INT                  ���Ĵ�ֱ����
    // Desc:      ���ݴ�ֱ�����ȡ��ǰ�ɼ�����һ���ڵ�
    //************************************
    virtual ITreeItem*			ItemFromYPos(__in INT)                                                      = 0;

    //************************************
    // Method:    ItemFromPoint
    // FullName:  xctrl::ITreeViewCtrl::ItemFromPoint
    // Access:    virtual public 
    // Returns:   ITreeItem*
    // Qualifier:
    // Parameter: __in const POINT &        ���ĵ�
    // Desc:      ��ȡ������λ�õĵ�ǰ�ɼ�����һ���ڵ�
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
    // Desc:      �����δ����ڵ�����ϵת��Ϊ������������ϵ
    //************************************
    virtual BOOL                ItemToClient(__in ITreeItem*, __inout LPRECT)                               = 0;

    //************************************
    // Method:    MoveItem
    // FullName:  xctrl::ITreeViewCtrl::MoveItem
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *          ָ�����е�ĳ���ӽڵ�
    // Parameter: __in ITreeItem *          Ҫ�ƶ����ĵط���NULL���������ȡ�¶������룩
    // Desc:      �����н���ƶ���ָ�����ڵ���
    //************************************
    virtual ITreeItem*          MoveItem(__in ITreeItem*, __in ITreeItem*)                                  = 0;

	

    //************************************
    // Method:    Scroll
    // FullName:  xctrl::ITreeViewCtrl::Scroll
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in INT                  ˮƽ�����������
    // Parameter: __in INT                  ��ֱ�����������
    // Desc:      �������Ŀ�������
    //************************************
    virtual BOOL                Scroll(__in INT, __in INT)                                                  = 0;

    //************************************
    // Method:    Select
    // FullName:  xctrl::ITreeViewCtrl::Select
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in_opt ITreeItem *      Ҫѡ�еĽڵ㣨Ĭ��NULL��Ϊ��ѡ���κ�һ���ڵ㣩
    // Parameter: __in_opt BOOL *           �Ƿ��Զ�ѡ��
    // Desc:      ѡ������ĳ���ڵ�
    //************************************
    virtual BOOL                Select(__in_opt ITreeItem*, __in_opt BOOL)                                  = 0;
	
	//************************************
	// Method:    SelectDropTarget
	// FullName:  xctrl::ITreeViewCtrl::SelectDropTarget
	// Access:    virtual public 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: __in_opt ITreeItem *      Ҫѡ�еĽڵ㣨Ĭ��NULL��Ϊ��ѡ���κ�һ���ڵ㣩
	// Desc:      ѡ��������ק��㣬<wyj 2010-04-27>
	//************************************
	virtual BOOL				SelectDropTarget(__in_opt ITreeItem*)										= 0;

    //************************************
    // Method:    SetBkImage
    // FullName:  xctrl::ITreeViewCtrl::SetBkImage
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in IImage *             ѡ��ı���ͼ
    // Desc:      �������ı���ͼ�񣨿ͻ�����Ч��
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
    // Desc:      �������ڵ���ı�����
    //************************************
    virtual void                SetItemText(__in ITreeItem*, __in LPCTSTR)                                  = 0;


	virtual void                SetItemHandler(__in ITreeItem *, __in ITreeItemHandler*)                    = 0;

    //************************************
    // Method:    SortChildren
    // FullName:  xctrl::ITreeViewCtrl::SortChildren
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ITreeItem *          ��Ҫ�����ӽڵ�����ڵ�
    // Parameter: __in_opt BOOL             �Ƿ�����ݹ������ӽڵ㣨Ĭ�ϲ����еݹ�����
    // Desc:      ʹ��Ĭ�Ϲ��򣨽ڵ��ı�����ֵ����򣩶�һ�����ڵ�������ӽڵ�����
    //************************************
    virtual BOOL                SortChildren(__in ITreeItem*, __in_opt BOOL)                                = 0;

    //************************************
    // Method:    SortChildrenCB
    // FullName:  xctrl::ITreeViewCtrl::SortChildrenCB
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: LPTVSORTCB                ������Ϣ
    // Parameter: BOOL                      �Ƿ�����ݹ������ӽڵ㣨Ĭ�ϲ����еݹ�����
    // Desc:      ʹ��ָ�������һ�����ڵ�������ӽڵ�����
    //************************************
    virtual BOOL                SortChildrenCB(__in LPTVSORTCB, __in_opt BOOL)                              = 0;

    virtual void                SetBatchMode(__in BOOL)                                                     = 0;
	virtual void				SetClientBkColor(COLORREF rc)		{}											;
	//*********************************
	//Desc: �ýڵ�ȫ����ʾ
	//Parameter: ITreeItem                Ҫ��ʾ�Ľڵ�
	//Parameter: int                     nShowStyl��ʾ��ʽ��TREEITEM_TO_SHOW,TREEITEM_TO_TOP �����е�һ�ַ�ʽ
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
    // Parameter: __in ITreeItem *          �����Ľڵ�1
    // Parameter: __in ITreeItem *          �����Ľڵ�2
    // Desc:      ���������ڵ��λ��
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
// Desc:      �����޴��ڿؼ�����
//************************************
XCTRLAPI IWindowLessObject*   CreateGTUIBaseObj(WINDOW_LESS_IID iid);




NAMESPACE_END(xctrl)