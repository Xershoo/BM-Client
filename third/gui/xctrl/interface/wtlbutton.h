#pragma once
#include "xctrl.h"
#include "xgdi/xgdidef.h"
#include "xgdi/xgdiutil.h"
#include "transwindow.h"
#include "cssparser.h"
#include "xgdi/xgdi.h"

NAMESPACE_BEGIN(xctrl)

// normal button with skin support
class CButtonItemHandler
	: implement IButtonHandler
	, public XCssParser
{
public:
	CButtonItemHandler(LPCTSTR lpszText)
	{
		m_strText           = lpszText;

		m_pFont             = (xgdi::IFont*)xskin::QueryUIObject(SKINID(TextFont));
		m_crFont            = (COLORREF)xskin::QueryUIValue(SKINID(TextColor));
		m_crFontDisabled    = (COLORREF)xskin::QueryUIValue(SKINID(TextColorDisabled));
		m_pIconSet          = NULL;
		m_pImageSet         = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(ButtonBack));
		m_nTextLeftInter	= 0;
		
		m_nDrawImageStyle	= xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT;
		m_rcBkInner.SetRect(4, 4, 4, 4);
		m_rcIconInner.SetRect(0, 0, 0, 0);

		m_pResLoader        = xskin::CResLoader::inst();

		m_nBkImageIndex = -1;
		m_bDisableDraw	= FALSE;
		m_pImageBKSet = NULL;
		m_bGlowText			= false;
		m_nGlowColor		= 0;
		m_nIconLeftInter	= 0;
	}

	virtual ~CButtonItemHandler()
	{
	}

	enum BUTTON_STATUS_IMAGE
	{
		BSI_NORMAL              = 0,
		BSI_FOCUSED             ,
		BSI_SELECTED            ,
		BSI_DISALBED            ,

		BSI_NORMAL_CHK          = 4,
		BSI_FOCUSED_CHK         ,
		BSI_SELECTED_CHK        ,
		BSI_DISALBED_CHK        ,

		BSI_MAX
	};
	
	virtual xgdi::IFont* GetTextFont()
	{
		return m_pFont;
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		if(pProp == NULL) return FALSE;

		LPCTSTR proper;

		proper = pProp->GetProperty(_T("font"));
		if(proper)
		{
			m_pFont = m_pResLoader->LoadFont(proper);
		}
		proper = pProp->GetProperty(_T("font-disabled"));
		if (proper) {
			m_pFontDisabled = m_pResLoader->LoadFont(proper);
		}
		proper = pProp->GetProperty(_T("font-color"));
		if(proper)
		{
			CTextUtil::TextToColor(proper, m_crFont);
		}
		proper = pProp->GetProperty(_T("font-color-disabled"));
		if(proper)
		{
			CTextUtil::TextToColor(proper, m_crFontDisabled);
		}
		if(proper = pProp->GetProperty(_T("image")))
		{
			m_pImageSet = m_pResLoader->LoadIconSet(proper);
			
		}
		
		if(proper = pProp->GetProperty(_T("image-inner")))
		{
			CTextUtil::TextToRect(proper, m_rcBkInner);
		}

		if(proper = pProp->GetProperty(_T("image_bk")))
		{
			m_pImageBKSet = m_pResLoader->LoadIconSet(proper);
			
		}

		if(proper = pProp->GetProperty(_T("bk_index")))
		{
			CTextUtil::TextToInt(proper, m_nBkImageIndex);
			
		}

		if(proper = pProp->GetProperty(_T("text_left")))
		{
			CTextUtil::TextToInt(proper, m_nTextLeftInter);

		}


		if(proper = pProp->GetProperty(_T("bk_index_disable")))
		{
			int nTemp = 0;
			CTextUtil::TextToInt(proper, nTemp);
			if(nTemp != 0)
				m_bDisableDraw = TRUE;

		}

		proper = pProp->GetProperty(_T("glowtext"));
		if(proper)
		{
			m_bGlowText = true;
		}
		proper = pProp->GetProperty(_T("glowcolor"));
		if(proper)
		{
			CTextUtil::TextToGdiPlusColor(proper, m_nGlowColor);
		}

		if(proper = pProp->GetProperty(_T("icon")))
		{
			m_pIconSet = m_pResLoader->LoadIconSet(proper);
			if(proper = pProp->GetProperty(_T("icon-inner")))
			{
				CTextUtil::TextToRect(proper, m_rcIconInner);
			}
		}
		if(proper = pProp->GetProperty(_T("icon_left")))
		{
			CTextUtil::TextToInt(proper, m_nIconLeftInter);

		}
		if(proper = pProp->GetProperty(_T("drawstyle")))
		{
			m_nDrawImageStyle = 0;
			xstring str(proper);
			if(str.find(_T("DIF_ALPHA")) >= 0)
				m_nDrawImageStyle |= xgdi::DIF_ALPHA;
			if(str.find(_T("DIF_CENTER_ALL_SHOW")) >= 0)
				m_nDrawImageStyle |= xgdi::DIF_CENTER_ALL_SHOW;
			if(str.find(_T("DIF_TRANSPARENT")) >= 0)
				m_nDrawImageStyle |= xgdi::DIF_TRANSPARENT;
			if(str.find(_T("DIF_NORMAL")) >= 0)
				m_nDrawImageStyle |= xgdi::DIF_NORMAL;
		}
		return XCssParser::ParseCss(pProp);
	}

	virtual const CSize& CalcItemSize(ICanvas* pCanvas) { return m_sizeItem; }
	virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return 0; }
	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{
		pCanvas->SetBkMode(TRANSPARENT);
		CRect rcText(rcItem);
		BOOL bSelected	    = (nFlag & DIF_SELECTED) ? TRUE : FALSE;
		BOOL bHighlighted   = (nFlag & DIF_HIGHLIGHTED) ? TRUE : FALSE;
		BOOL bDisabled      = (nFlag & DIF_DISABLED) ? TRUE : FALSE;
		BOOL bFocused       = (nFlag & DIF_FOCUSED) ? TRUE : FALSE;
		BOOL bChecked       = (nFlag & DIF_CHECKED) ? TRUE : FALSE;

		pCanvas->SetTextColor(m_crFont);
		xgdi::IFont* pFont= m_pFont;
		// normal button
		int nIndex;
		if(bDisabled)
		{
			nIndex = BSI_DISALBED;
			pCanvas->SetTextColor(m_crFontDisabled);
			//pFont = m_pFontDisabled;
		}
		else if(bSelected || bChecked)
		{
			/*if (bHighlighted) {
				nIndex = BSI_SELECTED;
			} else {
				nIndex = BSI_FOCUSED;
			}*/
			//当设置了可选时，用第三个表示，即以按钮按下时的图片表示
			nIndex = BSI_SELECTED;
		}
		else if(bHighlighted)
		{
			nIndex = BSI_FOCUSED;
		}
		else if(bFocused)
		{
			nIndex = BSI_FOCUSED;
		}
		else
		{
			nIndex = BSI_NORMAL;
		}

		if(m_pImageBKSet)
		{
			int nBackIndex = nIndex;
			if(m_nBkImageIndex >= 0)
				nBackIndex = m_nBkImageIndex;
			if(xgdi::IIcon* pIcon = m_pImageBKSet->GetIcon(nBackIndex))
			{
				if((bDisabled == m_bDisableDraw) || m_nBkImageIndex < 0)
					pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}
			if(m_pImageSet)
			{
				if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
				{
					RECT rcSet = pIcon->GetRect();
					RECT rcTemp = rcItem;
					int nTempInt = (rcTemp.bottom - rcTemp.top -(rcSet.bottom - rcSet.top));
					rcTemp.top = rcTemp.top + nTempInt / 2;
					if(nTempInt % 2 == 1)
					{
						rcTemp.top++;
					}
					rcTemp.bottom = rcTemp.top + (rcSet.bottom - rcSet.top);
					nTempInt = (rcTemp.right - rcTemp.left - (rcSet.right - rcSet.left));
					rcTemp.left = rcTemp.left + nTempInt / 2;
					if(nTempInt % 2 == 1)
						rcTemp.left++;
					rcTemp.right = rcTemp.left + (rcSet.right - rcSet.left);
					pCanvas->DrawIcon(pIcon, rcTemp,xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
				}
			}
		}

		else if(m_pImageSet)
		{
			if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
			{
				if(m_nDrawImageStyle & xgdi::DIF_CENTER_ALL_SHOW)
					pCanvas->DrawIcon(pIcon, rcItem, m_nDrawImageStyle);
				else
					pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner, xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}
		}

		
		if(FALSE == (::GetWindowLong(m_pItem->GetUIHandle(), GWL_STYLE) & BS_BITMAP))
		{
			pCanvas->SetBkMode(TRANSPARENT);
			if(nIndex == BSI_SELECTED)
			{
				rcText.left += 2;
				rcText.top += 2;
			}
			rcText.left += m_nTextLeftInter;
			//pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

			if(!m_bGlowText)
				pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			else
				pCanvas->DrawTextGlow(pFont, GetItemText(), &rcText, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS, m_nGlowColor);
		}
	}
	virtual void Release() { delete this; }
	virtual IWindowHandler* Clone() { return new CButtonItemHandler(m_strText.c_str()); }
	virtual void SetItem(IButton* pItem) { m_pItem = pItem; }
	virtual LPCTSTR GetItemText() const { return m_strText.c_str(); }
	virtual void SetItemText(LPCTSTR lpszText) { m_strText = lpszText; }
	virtual int UpdateCheckButton(int nCheck) { return nCheck; }
	xgdi::IFont* GetButtonFont() { return m_pFont; }
	void		 SetImageSet(LPCTSTR pProper) 
	{ 
		if(pProper)
		{
			m_pImageSet = m_pResLoader->LoadIconSet(pProper);
		}
	}

protected:
	xstring     	        m_strText;
	CSize			        m_sizeItem;

	COLORREF                m_crFont;
	COLORREF                m_crFontDisabled;
	CRect                   m_rcBkInner, m_rcIconInner;
	IItem*                  m_pItem;
	xgdi::IIconSet*         m_pImageSet;
	xgdi::IIconSet*         m_pImageBKSet;  //背景图片，add by ljs
	xgdi::IIconSet*         m_pIconSet;
	
	xgdi::IFont*            m_pFont;
	xgdi::IFont*            m_pFontDisabled;
	int						m_nBkImageIndex;
	int						m_nTextLeftInter;
	int						m_nIconLeftInter;
	BOOL					m_bDisableDraw;
	bool					m_bGlowText;
	DWORD					m_nGlowColor;
	int						m_nDrawImageStyle;
};

// normal checkbox button with skin support
class CCheckboxHandler
	: public CButtonItemHandler
{
public:
	CCheckboxHandler(LPCTSTR lpszText)
		: CButtonItemHandler(lpszText)
	{
		m_pIconSet  = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(CheckboxIcon));
		m_pImageSet = NULL;
	}

	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{
		if(BOOL bPushlike = ((nFlag & DIF_PUSHLIKE) ? TRUE : FALSE))
		{
			if(m_pImageSet == NULL) m_pImageSet = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(ButtonBack));
			__super::Draw(pCanvas, rcItem, rcUpdate, nFlag);
		}
		else
		{
			BOOL bChecked       = (nFlag & DIF_CHECKED) ? TRUE : FALSE;
			BOOL bFocused       = (nFlag & DIF_FOCUSED) ? TRUE : FALSE;
			BOOL bSelected	    = (nFlag & DIF_SELECTED) ? TRUE : FALSE;
			BOOL bHighlighted   = (nFlag & DIF_HIGHLIGHTED) ? TRUE : FALSE;
			BOOL bDisabled      = (nFlag & DIF_DISABLED) ? TRUE : FALSE;

			pCanvas->SetTextColor(m_crFont);
			xgdi::IFont* pFont = m_pFont;

			CRect rcText;
			int nIndex;
			if(bDisabled)
			{
				nIndex = BSI_DISALBED;
				pCanvas->SetTextColor(m_crFontDisabled);
				//pFont = m_pFontDisabled;
			}
			else if(bSelected)
			{
				nIndex = BSI_SELECTED;
			}
			else if(bHighlighted)
			{
				nIndex = BSI_FOCUSED;
			}
			else
			{
				nIndex = BSI_NORMAL;
			}
			if(bChecked) nIndex += BSI_NORMAL_CHK;

			if(m_pImageSet)
				if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
				{
					pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner,xgdi::DIF_ALPHA |  xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
				}

				if(m_pIconSet)
					if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(nIndex))
					{
						CRect rcIcon(rcItem.left, rcItem.top, rcItem.left + CRect(pIcon->GetRect()).Width(), rcItem.bottom);
						pCanvas->DrawIcon(pIcon, rcIcon, xgdi::DIF_ALPHA |  xgdi::DIF_ALIGNLEFT | xgdi::DIF_TRANSPARENT);
						rcText.SetRect(rcIcon.right + 4, rcItem.top, rcItem.right, rcItem.bottom);
					}

					if(FALSE == (::GetWindowLong(m_pItem->GetUIHandle(), GWL_STYLE) & BS_BITMAP))
					{
						pCanvas->SetBkMode(TRANSPARENT);
						pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
					}
		}
	}

	virtual IWindowHandler* Clone()
	{
		return new CCheckboxHandler(m_strText.c_str());
	}

	virtual int UpdateCheckButton(int nCheck)
	{
		return (nCheck ? FALSE : TRUE);
	}
};

// normal radio button with skin support
class CRadioHandler
	: public CButtonItemHandler
{
public:
	CRadioHandler(LPCTSTR lpszText)
		: CButtonItemHandler(lpszText)
	{
		m_pIconSet  = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(RadioIcon));
		m_pImageSet = NULL;
	}

	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{
		if(BOOL bPushlike = ((nFlag & DIF_PUSHLIKE) ? TRUE : FALSE))
		{
			__super::Draw(pCanvas, rcItem, rcUpdate, nFlag);
		}
		else
		{
			BOOL bChecked       = (nFlag & DIF_CHECKED) ? TRUE : FALSE;
			BOOL bFocused       = (nFlag & DIF_FOCUSED) ? TRUE : FALSE;
			BOOL bSelected	    = (nFlag & DIF_SELECTED) ? TRUE : FALSE;
			BOOL bHighlighted   = (nFlag & DIF_HIGHLIGHTED) ? TRUE : FALSE;
			BOOL bDisabled      = (nFlag & DIF_DISABLED) ? TRUE : FALSE;

			pCanvas->SetTextColor(m_crFont);
			xgdi::IFont* pFont = m_pFont;

			CRect rcText;
			int nIndex;
			if(bDisabled)
			{
				nIndex = BSI_DISALBED;
				pCanvas->SetTextColor(m_crFontDisabled);
				//pFont = m_pFontDisabled;
			}
			else if(bSelected)
			{
				nIndex = BSI_SELECTED;
			}
			else if(bHighlighted)
			{
				nIndex = BSI_FOCUSED;
			}
			else
			{
				nIndex = BSI_NORMAL;
			}
			if(bChecked) nIndex += BSI_NORMAL_CHK;

			if(m_pImageSet)
			{
				if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
				{
					pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner,xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
				}
			}

			if(m_pIconSet)
			{
				if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(nIndex))
				{
					CRect rcIcon(rcItem.left, rcItem.top, rcItem.left + CRect(pIcon->GetRect()).Width(), rcItem.bottom);
					pCanvas->DrawIcon(pIcon, rcIcon, xgdi::DIF_ALPHA | xgdi::DIF_ALIGNLEFT | xgdi::DIF_TRANSPARENT);
					rcText.SetRect(rcIcon.right + 4, rcItem.top, rcItem.right, rcItem.bottom);
				}
			}

			if(FALSE == (::GetWindowLong(m_pItem->GetUIHandle(), GWL_STYLE) & BS_BITMAP))
			{
				pCanvas->SetBkMode(TRANSPARENT);
				pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			}
		}
	}

	virtual IWindowHandler* Clone()
	{
		return new CRadioHandler(m_strText.c_str());
	}

	virtual int UpdateCheckButton(int nCheck)
	{
		return nCheck;
	}
};

//按钮，按钮背景上加图标和文字，图标可以左也可以右
class CImageTextHandler
	: public CButtonItemHandler
{
public:
	CImageTextHandler(LPCTSTR lpszText)
		: CButtonItemHandler(lpszText)
	{
		m_pIconSet  = NULL;
		m_pImageSet = NULL;
	}

	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{
		pCanvas->SetBkMode(TRANSPARENT);

		BOOL bChecked       = (nFlag & DIF_CHECKED) ? TRUE : FALSE;
		BOOL bFocused       = (nFlag & DIF_FOCUSED) ? TRUE : FALSE;
		BOOL bSelected	    = (nFlag & DIF_SELECTED) ? TRUE : FALSE;
		BOOL bHighlighted   = (nFlag & DIF_HIGHLIGHTED) ? TRUE : FALSE;
		BOOL bDisabled      = (nFlag & DIF_DISABLED) ? TRUE : FALSE;

		pCanvas->SetTextColor(m_crFont);
		xgdi::IFont* pFont = m_pFont;

		CRect rcText;
		int nIndex;
		if(bDisabled)
		{
			nIndex = BSI_DISALBED;
			pCanvas->SetTextColor(m_crFontDisabled);
			//pFont = m_pFontDisabled;
		}
		else if(bSelected || bChecked)
		{
			nIndex = BSI_SELECTED;
		}
		else if(bHighlighted)
		{
			nIndex = BSI_FOCUSED;
		}
		else
		{
			nIndex = BSI_NORMAL;
		}
		
		//if(bChecked) nIndex += BSI_NORMAL_CHK;

		if(m_pImageSet)
		{
			if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
			{
				pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner,xgdi::DIF_ALPHA |  xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
			}
		}
		if(m_pIconSet)
		{
			if(xgdi::IIcon* pIcon = m_pIconSet->GetIcon(nIndex))
			{
				CRect rcIcon(rcItem.left, rcItem.top, rcItem.left + CRect(pIcon->GetRect()).Width(), rcItem.bottom);
				CRect rcIconSize(pIcon->GetRect());
				rcIcon.left = rcItem.left + m_nIconLeftInter;
				rcIcon.right = rcIcon.left + rcIconSize.Width();
				rcIcon.top = rcItem.top + ((rcItem.bottom - rcItem.top) - rcIconSize.Height() + 1) / 2;
				rcIcon.bottom = rcIcon.top + rcIconSize.Height();
				pCanvas->DrawIcon(pIcon, rcIcon, xgdi::DIF_ALPHA |  xgdi::DIF_ALIGNLEFT | xgdi::DIF_TRANSPARENT);

			}
		}
		rcText = rcItem;
		rcText.left += m_nTextLeftInter;
		pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);


	}

	virtual IWindowHandler* Clone()
	{
		return new CImageTextHandler(m_strText.c_str());
	}

};

// toolbar 
class CTBItemHandler
	: public CButtonItemHandler
{
public:
	CTBItemHandler(LPCTSTR lpszText)
		: CButtonItemHandler(lpszText), m_pIcon(NULL), m_nIndex(0)
	{
		m_pImageSet     = (xgdi::IIconSet*)xskin::QueryUIObject(SKINID(TBButtonBack));
		m_pIconSet      = NULL;
		m_rcBkInner.SetRect(4, 4, 4, 4);
		m_bIconFirstDraw = false;
		m_nInconSize.cx = 0;
		m_nInconSize.cy = 0;
		m_bAlpha = true;
		m_nTxtLeft = 0;
		m_pGameOnline		= NULL;
		m_pGtOnline			= NULL;
		m_pPhoneOnline		= NULL;
		m_bGameOnline		= false;
		m_bGtOnline			= false;
		m_bPhoneOnline		= false;
		m_nOnlineInter		= 0;
	}

	virtual void Draw(ICanvas* pCanvas, const RECT& rcItem, const RECT& rcUpdate, int nFlag)
	{
		CRect rcText(rcItem);

		BOOL bSelected	    = (nFlag & DIF_SELECTED) ? TRUE : FALSE;
		BOOL bHighlighted   = (nFlag & DIF_HIGHLIGHTED) ? TRUE : FALSE;
		BOOL bDisabled      = (nFlag & DIF_DISABLED) ? TRUE : FALSE;
		BOOL bFocused       = (nFlag & DIF_FOCUSED) ? TRUE : FALSE;
		BOOL bChecked       = (nFlag & DIF_CHECKED) ? TRUE : FALSE;

		pCanvas->SetTextColor(m_crFont);
		xgdi::IFont* pFont = m_pFont;

		// normal button
		int nIndex;
		if(bDisabled)
		{
			nIndex = BSI_DISALBED;
			pCanvas->SetTextColor(m_crFontDisabled);
			//pFont = m_pFontDisabled;
		}
		else if(bSelected || bChecked)
		{
			if (bHighlighted) {
				nIndex = BSI_SELECTED;
			} else {
				nIndex = BSI_FOCUSED;
			}
		}
		else if(bHighlighted)
		{
			nIndex = BSI_FOCUSED;
		}
		else
		{
			nIndex = BSI_NORMAL;
		}
		if(bChecked) nIndex += BSI_NORMAL_CHK;
		int nDrawFlag = xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT;
		if(m_bAlpha)
			nDrawFlag |= xgdi::DIF_ALPHA;
		if(!m_bIconFirstDraw)
		{
			if(m_pImageSet)
			{
				if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
				{
					pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner,xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
				}
			}
		}

		rcText.DeflateRect(m_rcBkInner);
		pCanvas->SetBkMode(TRANSPARENT);

		if(::GetWindowLong(m_pItem->GetUIHandle(), GWL_STYLE) & BS_BITMAP)
		{
			if(m_pIcon) pCanvas->DrawIcon(m_pIcon, rcText, xgdi::DIF_TRANSPARENT);
		}
		else
		{
			//  [10/21/2010 魏永赳]
			xgdi::IIcon* pIcon = NULL;
			if(m_pIconSet){
				pIcon = m_pIconSet->GetIcon(m_nIndex);
			}else{
				pIcon = m_pIcon;
			}
			
			if(pIcon)
			{
				CRect rcIcon(rcText.left, rcText.top, rcText.left +  rcText.bottom - rcText.top, rcText.bottom);
				//CRect rcIcon(rcItem);
				if(m_pIconSet)
				{
					//刘金山 7/20/2011
					SIZE IconSize = m_pIconSet->GetIconSize();
					rcIcon.top = rcItem.top + (rcItem.bottom - rcItem.top -  IconSize.cy) / 2;
					rcIcon.bottom = rcIcon.top + IconSize.cy;
					rcIcon.right = rcIcon.left + IconSize.cx;
				}
				if(m_nInconSize.cx > 0)
				{
					rcIcon.left += (rcIcon.Width() - m_nInconSize.cx) / 2;
					rcIcon.right = rcIcon.left + m_nInconSize.cx;
					rcIcon.top += (rcIcon.Height() - m_nInconSize.cy) / 2;
					rcIcon.bottom = rcIcon.top + m_nInconSize.cy;
				}
				pCanvas->DrawIcon(pIcon, rcIcon,nDrawFlag);
				rcText.left += rcIcon.Width() + 4;
			}
			if(m_nTxtLeft > 0)
				rcText.left = m_nTxtLeft;
			else if(m_nTxtLeft < 0)
				rcText.left = rcItem.right + m_nTxtLeft;
			pCanvas->DrawText(pFont, GetItemText(), &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			if(m_bIconFirstDraw)
			{
				if(m_pImageSet)
				{
					if(xgdi::IIcon* pIcon = m_pImageSet->GetIcon(nIndex))
					{
						pCanvas->DrawIcon(pIcon, rcItem, m_rcBkInner,xgdi::DIF_ALPHA | xgdi::DIF_STRETCH | xgdi::DIF_TRANSPARENT);
					}
				}
			}
		}
		if(m_bGameOnline && m_pGameOnline)
		{
			RECT rectBlock = {0,0,0,0}; 		

			RECT rectImg		= {0, 0, m_pGameOnline->GetSize().cx, m_pGameOnline->GetSize().cy};

			rectBlock.bottom = rcItem.bottom - m_nOnlineInter;
			rectBlock.top = rectBlock.bottom-rectImg.bottom ;
			
			rectBlock.right = rcItem.right - m_nOnlineInter;
			rectBlock.left = rectBlock.right - rectImg.right;
			pCanvas->DrawImage(m_pGameOnline, rectBlock,rectImg, xgdi::DIF_ALPHA |  xgdi::DIF_TRANSPARENT|xgdi::DIF_STRETCH);
		}

		if(m_bGtOnline && m_pGtOnline)
		{
			RECT rectBlock = {0,0,0,0}; 
			RECT rectImg		= {0, 0, m_pGtOnline->GetSize().cx, m_pGtOnline->GetSize().cy};
			rectBlock.bottom = rcItem.bottom - m_nOnlineInter;
			rectBlock.top = rectBlock.bottom-rectImg.bottom ;
			rectBlock.left = rcItem.left + m_nOnlineInter;
			rectBlock.right = rectBlock.left + rectImg.right;

			pCanvas->DrawImage(m_pGtOnline, rectBlock, rectImg, xgdi::DIF_ALPHA | xgdi::DIF_TRANSPARENT|xgdi::DIF_STRETCH);
		}

		if(m_bPhoneOnline && m_pPhoneOnline)
		{
			RECT rectBlock = {0,0,0,0}; 
			RECT rectImg		= {0, 0, m_pPhoneOnline->GetSize().cx, m_pPhoneOnline->GetSize().cy};
			rectBlock.bottom = rcItem.bottom - m_nOnlineInter;
			rectBlock.top = rectBlock.bottom-rectImg.bottom ;
			rectBlock.left = rcItem.left + m_nOnlineInter;
			rectBlock.right = rectBlock.left + rectImg.right;

			pCanvas->DrawImage(m_pPhoneOnline, rectBlock, rectImg, xgdi::DIF_ALPHA | xgdi::DIF_TRANSPARENT|xgdi::DIF_STRETCH);
		}

	}

	virtual IWindowHandler* Clone()
	{
		return new CTBItemHandler(m_strText.c_str());
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		LPCTSTR proper;
		if(proper = pProp->GetProperty(_T("icon")))
		{
			xgdi::IIconSet* pIconSet = m_pResLoader->LoadIconSet(proper);
			if(pIconSet && (proper = pProp->GetProperty(_T("icon-index"))))
			{
				int nIndex = 0;
				CTextUtil::TextToInt(proper, nIndex);
				// 如果是iconset,不保留pIcon,否则换肤时失效 [10/21/2010 魏永赳]
				// m_pIcon = pIconSet->GetIcon(nIndex);
				m_nIndex = nIndex;
			}
			m_pIconSet = pIconSet;
		}
		else if(proper = pProp->GetProperty(_T("IconSize")))
		{
			CTextUtil::TextToSize(proper, m_nInconSize);
		}

		if(proper = pProp->GetProperty(_T("txtleft")))
		{
			CTextUtil::TextToInt(proper, m_nTxtLeft);
		}

		if(proper = pProp->GetProperty(_T("gtonline")))
		{
			m_pGtOnline = m_pResLoader->LoadImage(proper);

		}

		if(proper = pProp->GetProperty(_T("gameoline")))
		{
			m_pGameOnline = m_pResLoader->LoadImage(proper);

		}

		if(proper = pProp->GetProperty(_T("phoneoline")))
		{
			m_pPhoneOnline = m_pResLoader->LoadImage(proper);

		}

		if(proper = pProp->GetProperty(_T("onlineinter")))
		{
			CTextUtil::TextToInt(proper, m_nOnlineInter);

		}


		return __super::ParseCss(pProp);
	}

	virtual int UpdateCheckButton(int nCheck)
	{
		if(::GetWindowLong(m_pItem->GetUIHandle(), GWL_STYLE) & BS_CHECKBOX)
		{
			return nCheck ? FALSE : TRUE;
		}
		return nCheck;
	}

	BOOL SetImage(xgdi::IIcon* pIcon, bool bAlpha = true)
	{
		m_pIcon = pIcon;
		m_bAlpha = bAlpha;
		return (m_pIcon == NULL) ? TRUE : FALSE;
	}

	BOOL SetImage(xgdi::IIconSet* pIconSet, int nIndex)
	{
		m_pIconSet = pIconSet; 
		m_nIndex = nIndex;
		return TRUE;
	}

	BOOL SetIconFirstDraw(bool bIconFirst)
	{
		m_bIconFirstDraw = bIconFirst;
		return TRUE;
	}

	void    SetOnlineInfo(bool bGtonline, bool bGameonline, bool bPhoneOnline)
	{
		m_bGameOnline = bGameonline;
		m_bGtOnline = bGtonline;
		m_bPhoneOnline = bPhoneOnline;
	}

private:
	bool			m_bAlpha;
	xgdi::IIcon*    m_pIcon;
	int				m_nIndex;
	bool			m_bIconFirstDraw;			//是否先画ICON，再画背景，主要是头像和头像框，由于头像框的圆角和透明，需要先画ICON
	SIZE			m_nInconSize;				//画ICON的大小，有些地上是需要背景大，ICON小，居中
	int				m_nTxtLeft;					//字的左边值，即按钮上的字的偏移
	xgdi::IImage*			m_pGameOnline;
	xgdi::IImage*			m_pGtOnline;
	xgdi::IImage*			m_pPhoneOnline;
	bool					m_bGameOnline;
	bool					m_bGtOnline;
	bool					m_bPhoneOnline;
	int				m_nOnlineInter; //GT在线，游戏在线 图标偏移量
};

#define TIME_FLASH_BUTTON	250

#define BN_MOUSEOVER		(10)

// wtlbutton
class CButton
	: public CWindowImpl<CButton, CWindow, CControlWinTraits>
	, public TransChildWindow<CButton>
	, implement ICssParser
{
public:
	CButton()
	{
		m_spButton = (xctrl::IButton*)xctrl::CreateGTUIBaseObj(xctrl::IID_Button);
		m_bFocusState = FALSE;
		m_bShowFocus = FALSE;
		m_bAlpha = FALSE;
		m_dwData = 0;
	}

	virtual void Release()
	{

	}

	virtual xgdi::IFont* GetTextFont()
	{
		return m_spButton->GetHandler()->GetTextFont();
	}

	virtual BOOL Parse(IProperty* pProp)
	{
		return ParseCss(pProp);
	}

	virtual BOOL ParseCss(IProperty* pProp)
	{
		if(pProp == NULL) return FALSE;

		XStyleParser stylePraser(pProp->GetProperty(_T("style")));
		stylePraser.AddStyle(_T("BS_ICON"),		 BS_ICON);
		stylePraser.AddStyle(_T("BS_CHECKBOX"),  BS_CHECKBOX);
		stylePraser.AddStyle(_T("BS_BITMAP"),	 BS_BITMAP);
		stylePraser.AddStyle(_T("BS_PUSHLIKE"),  BS_PUSHLIKE);
		stylePraser.AddStyle(_T("BS_CHECKBOX"),  BS_CHECKBOX);
		m_dwStyle = stylePraser.dwStyle;
		if(IsWindow()){
			ModifyStyle(0,m_dwStyle,0);
		}

		
		LPCTSTR proper;
		if(proper = pProp->GetProperty(_T("stretch-mode")))
		{
			xstring tempStr = proper;
			if(tempStr.find(_T("DIF_ALPHA")) != xstring::npos)
				m_bAlpha = TRUE;
		}
		if(proper = pProp->GetProperty(_T("data")))
		{
			int nData = 0;
			CTextUtil::TextToInt(proper, nData);
			m_dwData = nData;
		}
		if(CButtonItemHandler* pHandler = dynamic_cast<CButtonItemHandler*>(m_spButton->GetHandler()))
		{
			pHandler->ParseCss(pProp);
			return TRUE;
		}

		return FALSE;
	}

	enum TIMER_ID
	{
		TIMER_FLASH_BUTTON       = 0x200,

		TIMER_MAX
	};

public:
	DECLARE_WND_CLASS(_T("wtlButton"));

	BEGIN_MSG_MAP(CButton)
		MESSAGE_HANDLER(WM_CREATE,                  OnCreate)
		MESSAGE_HANDLER(WM_DESTROY,                 OnDestroy)
		MESSAGE_HANDLER(WM_PAINT,                   OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,              OnEraseBkgnd)
		MESSAGE_HANDLER(WM_SETTEXT,                 OnSetWindowText)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK,           OnLButton)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,             OnLButton)
		MESSAGE_HANDLER(WM_LBUTTONUP,               OnLButton)
		MESSAGE_HANDLER(WM_MOUSEMOVE,               OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE,              OnMouseLeave)
		MESSAGE_HANDLER(WM_MOUSEHOVER,				OnMouseHover)
		MESSAGE_HANDLER(WM_KEYDOWN,					OnKeyDown)
		MESSAGE_HANDLER(WM_ENABLE,                  OnEnable)
		MESSAGE_HANDLER(WM_SETFOCUS,                OnFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS,               OnFocus)
		MESSAGE_HANDLER(WM_TIMER,					OnTimer)
		MESSAGE_HANDLER(WM_SHOWWINDOW,				OnShowWindow)
		MESSAGE_HANDLER(WM_CAPTURECHANGED,			OnCaptureChange)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,				OnRButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONUP,				OnRButtonUp)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_spButton->Attach(m_hWnd);

		ATL::CString strWindowText;
		GetWindowText(strWindowText);
		m_bTracking = false;
		int nStyle = GetStyle();

		if(nStyle & BS_ICON)
		{
			m_spButton->SetHandler(&CTBItemHandler(strWindowText));
		}
		else if(nStyle & BS_CHECKBOX)
		{
			m_spButton->SetHandler(&CCheckboxHandler(strWindowText));
		}
		else if(nStyle & BS_RADIOBUTTON)
		{
			m_spButton->SetHandler(&CRadioHandler(strWindowText));
		}
		else if(nStyle & BS_BITMAP)
		{
			m_spButton->SetHandler(&CImageTextHandler(strWindowText));
		}
		else
		{
			m_spButton->SetHandler(&CButtonItemHandler(strWindowText));
		}

		m_bButtonDown = FALSE;

		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CUpdateRect rcUpdate(m_hWnd);
		CPaintDC dc(m_hWnd);
	//	CMemoryDC dcMem( dc.m_hDC, dc.m_ps.rcPaint );
		dc.SetBkMode(TRANSPARENT);
		CClientRect rcClient(m_hWnd);

		xgdi::XGDIObject<xgdi::ICanvas> spCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
		spCanvas->SetSize(rcClient.Size());
		spCanvas->Attach(dc.m_hDC);

		ParentDrawBkgrd(spCanvas, rcUpdate);

		m_spButton->Paint(spCanvas, rcClient, rcUpdate);

		if ( m_bShowFocus && m_spButton->GetItemFocused() )
		{
			::DrawFocusRect(spCanvas->GetUIHandle(), rcClient);
		}

		if(m_bAlpha)
			spCanvas->Present(dc.m_hDC, rcClient, DIF_ALPHA);
		else
			spCanvas->Present(dc.m_hDC, rcClient, SRCCOPY);
		spCanvas->Detach();

		return 0;
	}

	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	void GetTextRect(CRect & rc)
	{
		CButtonItemHandler* pHandler = dynamic_cast<CButtonItemHandler*>(m_spButton->GetHandler());
		if(pHandler == NULL)
			return;
		ATL::CString strWindowText;
		GetWindowText(strWindowText);
		CPaintDC dc(m_hWnd);
		CClientRect rcTemp(m_hWnd);
		xgdi::ICanvas* pCanvas = (xgdi::ICanvas*)xgdi::CreateGTUIObj(xgdi::IID_UICanvas);
		pCanvas->SetSize(rcTemp.Size());
		pCanvas->Attach(dc.m_hDC);
		pCanvas->DrawText(pHandler->GetButtonFont(), strWindowText, &rc, DT_CALCRECT);
		pCanvas->Detach();
		pCanvas->Release();
	}

	LRESULT OnSetWindowText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(xctrl::IItemHandler* pHandler = m_spButton->GetHandler())
		{
			pHandler->SetItemText((LPCTSTR)lParam);
		}
		bHandled = FALSE;
		Invalidate(FALSE);

		return 0;
	}

	LRESULT OnLButton(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//HWND hActiveWnd = ::GetActiveWindow();
		//if(::GetParent(m_hWnd) != hActiveWnd)
		//{
		//    bHandled = FALSE;
		//    return 0;
		//}

		return m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if ( !m_bTracking )
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof( tme );
			tme.hwndTrack = m_hWnd;
			tme.dwFlags = TME_LEAVE|TME_HOVER;
			tme.dwHoverTime = 200;
			m_bTracking = ::_TrackMouseEvent( &tme ) ? true : false;
		}

		return m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bTracking = false;
		return m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
	}

	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int id = ::GetDlgCtrlID(m_hWnd);
		::PostMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELPARAM(id, BN_MOUSEOVER), (LPARAM)m_hWnd);
		return m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
	}


	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bButtonDown = TRUE;
		return 0;
	}

	LRESULT OnRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(!m_bButtonDown)
			return 0;
		m_bButtonDown = FALSE;
		::PostMessage(::GetParent(m_hWnd), WM_RBUTTONUP, NULL, (LPARAM)m_hWnd);
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(wParam == VK_SPACE)
		{
			if(m_spButton->GetItemFocused())
			{
				int id = ::GetDlgCtrlID(m_hWnd);
				//m_spButton->OnMessage(WM_LBUTTONDOWN, 0, 0, bHandled);
				//m_spButton->OnMessage(WM_LBUTTONUP, 0, 0, bHandled);
				::PostMessage(::GetParent(m_hWnd), WM_COMMAND, MAKELPARAM(id, BN_CLICKED), (LPARAM)m_hWnd);
				return 0;
			}
		}
		return 0;
	}

	LRESULT OnEnable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
		Invalidate(FALSE);

		return 0;
	}

	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
		Invalidate(FALSE);

		return 0;
	}

	LRESULT OnCaptureChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_spButton->OnMessage(uMsg, wParam, lParam, bHandled);
		Invalidate(FALSE);

		return 0;
	}

	LRESULT OnFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch(uMsg)
		{
		case WM_SETFOCUS: 
			m_spButton->SetItemFocused(TRUE); 
			break;
		case WM_KILLFOCUS: 
			m_bButtonDown = FALSE;
			m_spButton->SetItemFocused(FALSE); 
			break;

		default:
			break;
		}
		Invalidate(TRUE);

		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (wParam == TIMER_FLASH_BUTTON)
		{
			m_spButton->SetButtonCheck(m_bFocusState ? BST_UNCHECKED : BST_CHECKED);
			m_spButton->SetItemFocused(m_bFocusState);
			m_bFocusState = !m_bFocusState;
			Invalidate();
		}

		return 0;
	}

public:
	int SetCheck(int nCheck)
	{
		return m_spButton->SetButtonCheck(nCheck);
	}

	int GetCheck() const
	{
		return m_spButton->GetItemChecked();
	}

	BOOL SetIconImage(xgdi::IIcon* pIcon, bool bAlpha = true)
	{
		if(!(GetStyle() & BS_ICON))
			return FALSE;
		if(CTBItemHandler* pTBHandler = dynamic_cast<CTBItemHandler*>(m_spButton->GetHandler()))
		{
			return pTBHandler->SetImage(pIcon, bAlpha);
		}

		return FALSE;
	}

	BOOL SetImageSet(LPCTSTR pProper)
	{
		if(CButtonItemHandler* pHandler = dynamic_cast<CButtonItemHandler*>(m_spButton->GetHandler()))
		{
			pHandler->SetImageSet(pProper);
			if(IsWindow())
				Invalidate(TRUE);
		}
		return TRUE;
	}

	BOOL SetIconImage(xgdi::IIconSet* pIconSet, int nIndex)
	{
		if(!(GetStyle() & BS_ICON))
			return FALSE;
		if(CTBItemHandler* pTBHandler = dynamic_cast<CTBItemHandler*>(m_spButton->GetHandler()))
		{
			return pTBHandler->SetImage(pIconSet, nIndex);
		}

		return FALSE;
	}

	BOOL SetIconFirstDraw(bool bIconFirst)
	{
		if(!(GetStyle() & BS_ICON))
			return FALSE;
		if(CTBItemHandler* pTBHandler = dynamic_cast<CTBItemHandler*>(m_spButton->GetHandler()))
		{
			return pTBHandler->SetIconFirstDraw(bIconFirst);
		}

		return FALSE;
	}

	BOOL SetFriendOnlineInfo(bool bGameOnline, bool bGtOnline, bool bPhoneOnline)
	{
		if(!(GetStyle() & BS_ICON))
			return FALSE;
		if(CTBItemHandler* pTBHandler = dynamic_cast<CTBItemHandler*>(m_spButton->GetHandler()))
		{
			pTBHandler->SetOnlineInfo(bGtOnline, bGameOnline, bPhoneOnline);
		}

		return FALSE;
	}

	void SetHandler(IButtonHandler* pHandler)
	{
		return m_spButton->SetHandler(pHandler);
	}

	void StartFlash()
	{
		SetTimer(TIMER_FLASH_BUTTON, TIME_FLASH_BUTTON, NULL);
	}

	void StopFlash()
	{
		KillTimer(TIMER_FLASH_BUTTON);
		m_spButton->SetButtonCheck(BST_UNCHECKED);
		m_spButton->SetItemFocused(FALSE);
	}
	
	void SetShowFocus(BOOL bShow)
	{
		m_bShowFocus = bShow;
	}

	BOOL GetShowFocus()
	{
		return m_bShowFocus;
	}

	void SetAlpha(BOOL b) { m_bAlpha = b;}

	void SetData(DWORD nData) { m_dwData = nData; }
	DWORD GetData() { return m_dwData; }

private:
	XObjPtr<IButton>        m_spButton;
	DWORD			        m_dwStyle;
	DWORD					m_dwData;
	BOOL					m_bFocusState;
	BOOL					m_bShowFocus;
	BOOL					m_bButtonDown;
	BOOL					m_bAlpha;
	bool					m_bTracking;
};

NAMESPACE_END(xctrl)
