#pragma once

#include <xgdi\xgdi.h>
#include "skinloader.h"

template <class T>
class CGTMenuHandle
{
private:
	COLORREF			m_crFontNormal;			// ����������ɫ
	COLORREF			m_crFontHighted;		// ����������ɫ
	COLORREF			m_crBkgrdNormal;		// ����������ɫ
	COLORREF			m_crBkgrdHighted;		// �����󱳾���ɫ
	COLORREF			m_crSeparator;			// �ָ�����ɫ
	COLORREF			m_crSideBar;			// �������ɫ

	UINT				m_nSeparatorHeight;		// �ָ����߶�
	UINT				m_nMenuItemHeight;		// ��ť��߶�
	UINT				m_nSideBarWidth;		// ��������
	UINT				m_nIntervalWidth;		// ���ֺͲ�������

	xgdi::IFont*		m_pFont;				// �������
	xgdi::IIconSet*		m_pIconSet;				// ͼ�꼯
	CSimpleStack<HMENU>	m_stackMenuHandle;		// �˵��б����ͷ��ڴ�

private:
	typedef struct _MENU_CONTEXT
	{
		WTL::CString	strMenuText;			// �˵�����
		DWORD			nMenuType;				// �˵�״̬
		INT				nIconIndex;				// ͼ�����
	}
	MENU_CONTEXT, *PMENU_CONTEXT;

public:
	CGTMenuHandle(): m_pFont(NULL), m_crFontNormal(0), m_crFontHighted(0), m_crBkgrdNormal(0)
		, m_crBkgrdHighted(0), m_crSeparator(0), m_crSideBar(0), m_pIconSet(NULL)
		, m_nSeparatorHeight(0), m_nMenuItemHeight(0), m_nSideBarWidth(0)
		, m_nIntervalWidth(0)
	{
		// ȡ����������
		m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(Menu.Font));
		if (!m_pFont)
			m_pFont = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));

		// ȡ�ø�����ɫ
		m_crFontNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorNormal));
		m_crFontHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.FontColorHighted));
		m_crBkgrdNormal = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorNormal));
		m_crBkgrdHighted = (COLORREF)xskin::QueryUIValue(SKINID(Menu.BkgrdColorHighted));
		m_crSeparator = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSeparator));
		m_crSideBar = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSideBar));

		// ȡ�ø߶ȿ��
		m_nSeparatorHeight = xskin::QueryUIValue(SKINID(Menu.HeightSeparator));
		m_nMenuItemHeight = xskin::QueryUIValue(SKINID(Menu.HeightMenuItem));
		m_nSideBarWidth = xskin::QueryUIValue(SKINID(Menu.WidthSideBar));
		m_nIntervalWidth = xskin::QueryUIValue(SKINID(Menu.WidthInterval));

		// ȡ��ͼ�꼯
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
	// ����������������ͼ����˵���Ĺ���
	//
	virtual INT GetAssociateIconIndex(INT nMenuId)
	{
		// -1��ʾû�ж�Ӧ��ͼ��
		return -1;
	}

	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// ϵͳ�˵�������
		if ((BOOL)HIWORD(lParam))   
		{
			bHandled = FALSE;
			return 1;
		}

		CMenuHandle menuPopup = (HMENU)wParam;
		TCHAR szString[MAX_PATH] = {0};
		int nMenuCount = menuPopup.GetMenuItemCount();
		//���˵�û��ѡ��ʱ��ֱ���˳����޸� by ljs 2012-8-22
		if(nMenuCount == 0)
			return 1;
		// �������в˵���
		for (int i = 0; i < nMenuCount; i++)
		{
			// ȡ�ò˵���Ϣ
			CMenuItemInfo menuInfo;
			menuInfo.cch = MAX_PATH;
			menuInfo.fMask = MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU | MIIM_TYPE;
			memset(szString, 0, sizeof(szString));
			menuInfo.dwTypeData = szString;
			menuPopup.GetMenuItemInfo(i, TRUE, &menuInfo);

			// �����ص���Ϣ
			if (!(menuInfo.fType & MFT_OWNERDRAW))
			{
				// ��ʼ���˵���������
				PMENU_CONTEXT pMenuContext = new MENU_CONTEXT;
				pMenuContext->strMenuText = szString;
				pMenuContext->nMenuType = menuInfo.fType;
				pMenuContext->nIconIndex = GetAssociateIconIndex(menuInfo.wID);

				// ���������ĺͷ��
				menuInfo.fMask = MIIM_DATA | MIIM_TYPE;
				menuInfo.dwItemData = (DWORD)pMenuContext;
				menuInfo.fType |= MFT_OWNERDRAW;
				menuPopup.SetMenuItemInfo(i, TRUE, &menuInfo);
			}
		}

		// ��ӵ��Ѵ���Ĳ˵��б�
		m_stackMenuHandle.Push(menuPopup.m_hMenu);

		return 0;
	}

	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// �жϲ˵��Ƿ񽫱��ر�
		if (HIWORD(wParam) == 0xFFFF && lParam == NULL)
		{
			// ����������Ĳ˵���
			HMENU hMenu = NULL;
			while ((hMenu = m_stackMenuHandle.Pop()) != NULL)
			{
				CMenuHandle menuPopup = hMenu;
				for (int i = 0; i < menuPopup.GetMenuItemCount(); i++)
				{
					// ȡ�ò˵���Ϣ
					CMenuItemInfo menuInfo;
					menuInfo.fMask = MIIM_DATA;
					menuPopup.GetMenuItemInfo(i, TRUE, &menuInfo);

					PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)menuInfo.dwItemData;
					if (pMenuContext != NULL)
					{
						// ��ԭ�˵���Ϣ
						menuInfo.fMask = MIIM_DATA;
						menuInfo.dwItemData = NULL;
						menuPopup.SetMenuItemInfo(i, TRUE, &menuInfo);

						// �ͷ��ڴ�
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

		// �ж��Ƿ���Ҫ�������Ϣ
		if (wParam != 0  || pMeasureItemStruct->CtlType != ODT_MENU || !pMenuContext)
		{
			bHandled = FALSE;
			return FALSE;
		}

		// �ָ��ߣ�����Ĭ�ϸ߶�
		if (pMenuContext->nMenuType & MFT_SEPARATOR)
		{
			m_nSeparatorHeight = xskin::QueryUIValue(SKINID(Menu.HeightSeparator));
			pMeasureItemStruct->itemHeight = m_nSeparatorHeight;
		}

		// ���֣������������ÿ��
		else
		{
			// ȡ�û�������߶ȺͿ��
			CWindowDC dc(NULL);
			SIZE sizeText = {0};
			WTL::CString strText = pMenuContext->strMenuText;
			::GetTextExtentPoint32(dc, strText, strText.GetLength(), &sizeText);

			// ���ϲ�����Ŀ���Լ����
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

		// �ж�������������Ƿ�Ӧ�ô���
		if (wParam != 0 || !pDrawItemStruct || pDrawItemStruct->CtlType != ODT_MENU
			|| !pDrawItemStruct->itemData)
		{
			bHandled = FALSE;
			return FALSE;
		}

		// ���Ʊ���
		DrawBackGround(pDrawItemStruct);

		// ��������
		DrawText(pDrawItemStruct);

		// ����ͼ��
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
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// �Ƿ����
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// �Ƿ����

		// ����������λ��
		CRect rectItem = pDrawItemStruct->rcItem;
		CRect rectSideBar = rectItem;
		rectSideBar.right = rectSideBar.left + m_nSideBarWidth;

		// ���Ʋ˵�����
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
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// �Ƿ����
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// �Ƿ����
		dc.SetBkMode(TRANSPARENT);

		// ȡ���Ѿ����õ�������
		PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)pDrawItemStruct->itemData;

		// �������ֵ�λ��
		CRect rectText = pDrawItemStruct->rcItem;
		rectText.left = rectText.left + m_nSideBarWidth + m_nIntervalWidth*2;

		// �ָ���
		if (pMenuContext->nMenuType & MFT_SEPARATOR)
		{
			rectText.left -= m_nIntervalWidth;
			rectText.top = rectText.Height() / 2 + rectText.top;
			rectText.bottom = rectText.top + 2;
			m_crSeparator = (COLORREF)xskin::QueryUIValue(SKINID(Menu.ColorSeparator));
			dc.Draw3dRect(rectText, m_crSeparator, RGB(255,255,255));
			return;
		}

		// ����״̬
		if (bDisabled)
		{
			SIZE sizeText = {0};
			WTL::CString strText = pMenuContext->strMenuText;
			::GetTextExtentPoint32(dc, strText, strText.GetLength(), &sizeText);

			// ��������λ��
			INT nOffset = (rectText.Height() - sizeText.cy) / 2;
			rectText.top += nOffset;
			rectText.bottom -= nOffset;
			dc.DrawState(rectText.TopLeft(), rectText.Size(), strText, DSS_DISABLED, TRUE, 0, (HBRUSH)NULL);
		}

		// ��ͨ����
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
		// ͼ��δ�������ܻ���
		if (!m_pIconSet)
			return;

		CDCHandle dc(pDrawItemStruct->hDC);
		BOOL bDisabled = pDrawItemStruct->itemState & ODS_GRAYED;		// �Ƿ����
		BOOL bFocused = pDrawItemStruct->itemState & ODS_SELECTED;		// �Ƿ����
		BOOL bChecked = pDrawItemStruct->itemState & ODS_CHECKED;		// �Ƿ�ѡ��

		// ȡ���Ѿ����õ�������
		PMENU_CONTEXT pMenuContext = (PMENU_CONTEXT)pDrawItemStruct->itemData;

		xgdi::IIcon* pIcon = NULL;

		// ȡ��ͼ��ָ��
		if (bChecked)
		{
			pIcon = m_pIconSet->GetIcon(0);
		}
		else if (pMenuContext->nIconIndex >= 0)
		{
			pIcon = m_pIconSet->GetIcon(pMenuContext->nIconIndex);
		}

		// ָ����ͼ����ЧҲ���ܻ���
		if (!pIcon)
			return;

		// ����λͼƫ��
		CRect rectIcon = pIcon->GetRect();
		CRect rectItem = pDrawItemStruct->rcItem;
		INT nOffsetY = (m_nMenuItemHeight - rectIcon.Height()) / 2 + rectItem.top;
		INT nOffsetX = (m_nSideBarWidth - rectIcon.Width()) / 2 + rectItem.left;

		// ѡ��ʱ�޸�ƫ��
		if (bFocused && !bDisabled)
			nOffsetY -= 1;

		// ����ͼ��
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


		// ���ٶ���
		::SelectObject(hBitmapDC, (HGDIOBJ)hOldBmp);
		::DeleteDC(hBitmapDC);
	}
};
