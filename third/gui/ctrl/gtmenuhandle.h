#pragma once

#include <xgdi\xgdi.h>
#include "skinloader.h"

template <class T>
class CGTMenuHandle
{
private:
	COLORREF			m_crFontNormal;			// 正常文字颜色
	COLORREF			m_crFontHighted;		// 高亮文字颜色
	COLORREF			m_crBkgrdNormal;		// 正常背景颜色
	COLORREF			m_crBkgrdHighted;		// 高亮后背景颜色
	COLORREF			m_crSeparator;			// 分隔线颜色
	COLORREF			m_crSideBar;			// 侧边条颜色

	UINT				m_nSeparatorHeight;		// 分隔条高度
	UINT				m_nMenuItemHeight;		// 按钮项高度
	UINT				m_nSideBarWidth;		// 侧边条宽度
	UINT				m_nIntervalWidth;		// 文字和侧边条间距

	xgdi::IFont*		m_pFont;				// 字体对象
	xgdi::IIconSet*		m_pIconSet;				// 图标集
	CSimpleStack<HMENU>	m_stackMenuHandle;		// 菜单列表以释放内存

private:
	typedef struct _MENU_CONTEXT
	{
		WTL::CString	strMenuText;			// 菜单文字
		DWORD			nMenuType;				// 菜单状态
		INT				nIconIndex;				// 图标序号
	}
	MENU_CONTEXT, *PMENU_CONTEXT;

public:
	CGTMenuHandle(): m_pFont(NULL), m_crFontNormal(0), m_crFontHighted(0), m_crBkgrdNormal(0)
		, m_crBkgrdHighted(0), m_crSeparator(0), m_crSideBar(0), m_pIconSet(NULL)
		, m_nSeparatorHeight(0), m_nMenuItemHeight(0), m_nSideBarWidth(0)
		, m_nIntervalWidth(0)
	{
		// 取得文字字体
		m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(Menu.Font));
		if (!m_pFont)
			m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));

		// 取得各种颜色
		m_crFontNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorNormal));
		m_crFontHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorHighted));
		m_crBkgrdNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorNormal));
		m_crBkgrdHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorHighted));
		m_crSeparator = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSeparator));
		m_crSideBar = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSideBar));

		// 取得高度宽度
		m_nSeparatorHeight = xskin::QueryUIValue(SKINID(Menu.HeightSeparator));
		m_nMenuItemHeight = xskin::QueryUIValue(SKINID(Menu.HeightMenuItem));
		m_nSideBarWidth = xskin::QueryUIValue(SKINID(Menu.WidthSideBar));
		m_nIntervalWidth = xskin::QueryUIValue(SKINID(Menu.WidthInterval));

		// 取得图标集
		m_pIconSet = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(Menu.IconSet));
	}

	virtual ~CGTMenuHandle()
	{

	}

	BEGIN_MSG_MAP(CGTMenuHandle)
		MESSAGE_HANDLER(WM_INITMENUPOPUP,	OnInitMenuPopup)
		MESSAGE_HANDLER(WM_MENUSELECT,		OnMenuSelect)
		MESSAGE_HANDLER(WM_DRAWITEM,		OnDrawItem)
		MESSAGE_HANDLER(WM_MEASUREITEM,		OnMeasureItem)
	END_MSG_MAP()

protected:
	//
	// 派生类重载以设置图标与菜单项的关联
	//
	virtual INT GetAssociateIconIndex(INT nMenuId)
	{
		// -1表示没有对应的图标
		return -1;
	}

	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 系统菜单不处理
		if ((BOOL)HIWORD(lParam))   
		{
			bHandled = FALSE;
			return 1;
		}

		CMenuHandle menuPopup = (HMENU)wParam;
		TCHAR szString[MAX_PATH] = {0};
		int nMenuCount = menuPopup.GetMenuItemCount();
		//当菜单没有选项时，直接退出，修改 by ljs 2012-8-22
		if(nMenuCount == 0)
			return 1;
		// 遍历所有菜单项
		for (int i = 0; i < nMenuCount; i++)
		{
			// 取得菜单信息
			CMenuItemInfo menuInfo;
			menuInfo.cch = MAX_PATH;
			menuInfo.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
			memset(szString, 0, sizeof(szString));
			menuInfo.dwTypeData = szString;
			menuPopup.GetMenuItemInfo(i, TRUE, &menuInfo);

			// 添加相关的信息
			if (!(menuInfo.fType & MFT_OWNERDRAW))
			{
				// 初始化菜单项上下文
				PMENU_CONTEXT pMenuContext = new MENU_CONTEXT;
				pMenuContext->strMenuText = szString;
				pMenuContext->nMenuType = menuInfo.fType;
				pMenuContext->nIconIndex = GetAssociateIconIndex(menuInfo.wID);

				// 设置上下文和风格
				menuInfo.fMask = MIIM_DATA | MIIM_TYPE;
				menuInfo.dwItemData = (DWORD)pMenuContext;
				menuInfo.fType |= MFT_OWNERDRAW;
				menuPopup.SetMenuItemInfo(i, TRUE, &menuInfo);
			}
		}

		// 添加到已处理的菜单列表
		m_stackMenuHandle.Push(menuPopup.m_hMenu);

		return 0;
	}

	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 判断菜单是否将被关闭
		if (HIWORD(wParam) == 0xFFFF && lParam == NULL)
		{
			// 变量被处理的菜单项
			HMENU hMenu = NULL;
			while ((hMenu = m_stackMenuHandle.Pop()) != NULL)
			{
				CMenuHandle menuPopup = hMenu;
				for (int i = 0; i < menuPopup.GetMenuItemCount(); i++)
				{
					// 取得菜单信息
					CMenuItemInfo menuInfo;
					menuInfo.fMask = MIIM_DATA;
					menuPopup.GetMenuItemInfo(i, TRUE, &menuInfo);

					PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)menuInfo.dwItemData;
					if (pMenuContext != NULL)
					{
						// 还原菜单信息
						menuInfo.fMask = MIIM_DATA;
						menuInfo.dwItemData = NULL;
						menuPopup.SetMenuItemInfo(i, TRUE, &menuInfo);

						// 释放内存
						delete pMenuContext;
					}
				}
			}
		}

		return 0;
	}

	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMEASUREITEMSTRUCT pMeasureItemStruct = (LPMEASUREITEMSTRUCT)lParam;
		PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)pMeasureItemStruct->itemData;

		// 判断是否需要处理该消息
		if (wParam != 0  || pMeasureItemStruct->CtlType != ODT_MENU || !pMenuContext)
		{
			bHandled = FALSE;
			return FALSE;
		}

		// 分隔线，设置默认高度
		if (pMenuContext->nMenuType & MFT_SEPARATOR)
		{
			m_nSeparatorHeight = xskin::QueryUIValue(SKINID(Menu.HeightSeparator));
			pMeasureItemStruct->itemHeight = m_nSeparatorHeight;
		}

		// 文字，根据字体设置宽度
		else
		{
			// 取得绘制所需高度和宽度
			CWindowDC dc(NULL);
			SIZE sizeText = {0};
			WTL::CString strText = pMenuContext->strMenuText;
			::GetTextExtentPoint32(dc, strText, strText.GetLength(), &sizeText);

			// 加上侧边条的宽度以及间隔
			pMeasureItemStruct->itemWidth = sizeText.cx;	
			m_nSideBarWidth = xskin::QueryUIValue(SKINID(Menu.WidthSideBar));
			m_nIntervalWidth = xskin::QueryUIValue(SKINID(Menu.WidthInterval));
			pMeasureItemStruct->itemWidth += m_nSideBarWidth + m_nIntervalWidth;	
			m_nMenuItemHeight = xskin::QueryUIValue(SKINID(Menu.HeightMenuItem));
			pMeasureItemStruct->itemHeight = m_nMenuItemHeight;
		}

		return 0;
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPDRAWITEMSTRUCT pDrawItemStruct = (LPDRAWITEMSTRUCT)lParam;

		// 判断输入参数决定是否应该处理
		if (wParam != 0 || !pDrawItemStruct || pDrawItemStruct->CtlType != ODT_MENU
			|| !pDrawItemStruct->itemData)
		{
			bHandled = FALSE;
			return FALSE;
		}

		// 绘制背景
		DrawBackGround(pDrawItemStruct);

		// 绘制文字
		DrawText(pDrawItemStruct);

		// 绘制图标
		DrawIcon(pDrawItemStruct);

		return TRUE;
	}

	void SetMenuIconSet(xgdi::IIconSet* pIconSet)
	{
		m_pIconSet = pIconSet;
	}

private:
	void DrawBackGround(LPDRAWITEMSTRUCT pDrawItemStruct)
	{
		CDCHandle dc(pDrawItemStruct->hDC);
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// 是否禁用
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// 是否高亮

		// 计算侧边栏的位置
		CRect rectItem = pDrawItemStruct->rcItem;
		CRect rectSideBar = rectItem;
		rectSideBar.right = rectSideBar.left + m_nSideBarWidth;

		// 绘制菜单背景
		if (!bFocused)
		{
			m_crBkgrdNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorNormal));
			dc.FillSolidRect(&rectItem, m_crBkgrdNormal);
		}
		m_crSideBar = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSideBar));
		dc.FillSolidRect(rectSideBar, m_crSideBar);
		if (bFocused && !bDisabled)
		{
			m_crBkgrdHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorHighted));
			rectItem.left = rectSideBar.right; 
			dc.FillSolidRect(&rectItem, m_crBkgrdHighted);
		}
	}

	void DrawText(LPDRAWITEMSTRUCT pDrawItemStruct)
	{
		CDCHandle dc(pDrawItemStruct->hDC);
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// 是否禁用
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// 是否高亮
		dc.SetBkMode(TRANSPARENT);

		// 取得已经设置的上下文
		PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)pDrawItemStruct->itemData;

		// 计算文字的位置
		CRect rectText = pDrawItemStruct->rcItem;
		rectText.left = rectText.left + m_nSideBarWidth + m_nIntervalWidth*2;

		// 分隔线
		if (pMenuContext->nMenuType & MFT_SEPARATOR)
		{
			rectText.left -= m_nIntervalWidth;
			rectText.top = rectText.Height() / 2 + rectText.top;
			rectText.bottom = rectText.top + 2;
			m_crSeparator = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSeparator));
			dc.Draw3dRect(rectText, m_crSeparator, RGB(255,255,255));
			return;
		}

		// 禁用状态
		if (bDisabled)
		{
			SIZE sizeText = {0};
			WTL::CString strText = pMenuContext->strMenuText;
			::GetTextExtentPoint32(dc, strText, strText.GetLength(), &sizeText);

			// 计算文字位置
			INT nOffset = (rectText.Height() - sizeText.cy) / 2;
			rectText.top += nOffset;
			rectText.bottom -= nOffset;
			dc.DrawState(rectText.TopLeft(), rectText.Size(), strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}

		// 普通文字
		else
		{
			m_crFontNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorNormal));
			m_crFontHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorHighted));
			dc.SetTextColor(bFocused ? m_crFontHighted : m_crFontNormal);
			dc.DrawText(pMenuContext->strMenuText, -1, rectText, DT_LEFT|DT_EXPANDTABS|DT_VCENTER|DT_SINGLELINE);
		}
	}

	void DrawIcon(LPDRAWITEMSTRUCT pDrawItemStruct)
	{
		// 图像未载入则不能绘制
		if (!m_pIconSet)
			return;

		CDCHandle dc(pDrawItemStruct->hDC);
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// 是否禁用
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// 是否高亮
		BOOL bChecked = pDrawItemStruct->itemState & ODS_CHECKED;		// 是否选中

		// 取得已经设置的上下文
		PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)pDrawItemStruct->itemData;

		xgdi::IIcon* pIcon = NULL;

		// 取得图标指针
		if (bChecked)
		{
			pIcon = m_pIconSet->GetIcon(0);
		}
		else if (pMenuContext->nIconIndex >= 0)
		{
			pIcon = m_pIconSet->GetIcon(pMenuContext->nIconIndex);
		}

		// 指定的图标无效也不能绘制
		if (!pIcon)
			return;

		// 计算位图偏移
		CRect rectIcon = pIcon->GetRect();
		CRect rectItem = pDrawItemStruct->rcItem;
		INT nOffsetY = (m_nMenuItemHeight - rectIcon.Height()) / 2 + rectItem.top;
		INT nOffsetX = (m_nSideBarWidth - rectIcon.Width()) / 2 + rectItem.left;

		// 选中时修改偏移
		if (bFocused && !bDisabled)
			nOffsetY -= 1;

		// 绘制图像
		HDC hBitmapDC = ::CreateCompatibleDC(dc);
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hBitmapDC, (HGDIOBJ)(HBITMAP)(pIcon->GetImage()->GetUIHandle()));

		//dc.TransparentBlt(nOffsetX, nOffsetY, rectIcon.Width(), rectIcon.Height(), hBitmapDC, 
		//	rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height(), RGB(255,0,255));

		BLENDFUNCTION m_bf;
		m_bf.BlendOp     = AC_SRC_OVER; 
		m_bf.BlendFlags  = 0; 
		m_bf.AlphaFormat = AC_SRC_ALPHA;
		m_bf.SourceConstantAlpha = 255;

		dc.AlphaBlend(nOffsetX, nOffsetY, rectIcon.Width(), rectIcon.Height(), hBitmapDC, 
			rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height(), m_bf);


		// 销毁对象
		::SelectObject(hBitmapDC, (HGDIOBJ)hOldBmp);
		::DeleteDC(hBitmapDC);
	}
};
