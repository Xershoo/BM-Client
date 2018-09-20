#pragma once

#include "xobject.h"
#include "xctrl.h"
#include "xctrltype.h"
#include "xskin.h"
#include "xgdi/xgdiutil.h"
#include <GdiPlus.h>

NAMESPACE_BEGIN(xctrl)

class XStyleParser
{
public:
	XStyleParser(LPCTSTR style)
	{
		dwStyle = 0;
		if(style == NULL) return;
		strStyle = style;
		if(strStyle.find(_T("WS_CHILD"))		!= xstring::npos) dwStyle |= WS_CHILD;
		if(strStyle.find(_T("WS_VISIBLE"))		!= xstring::npos) dwStyle |= WS_VISIBLE;
		if(strStyle.find(_T("WS_DISABLED"))		!= xstring::npos) dwStyle |= WS_DISABLED;
		if(strStyle.find(_T("WS_CLIPSIBLINGS"))	!= xstring::npos) dwStyle |= WS_CLIPSIBLINGS;
		if(strStyle.find(_T("WS_CLIPCHILDREN"))	!= xstring::npos) dwStyle |= WS_CLIPCHILDREN;
		if(strStyle.find(_T("WS_BORDER"))		!= xstring::npos) dwStyle |= WS_BORDER;
		if(strStyle.find(_T("WS_VSCROLL"))		!= xstring::npos) dwStyle |= WS_VSCROLL;
		if(strStyle.find(_T("WS_HSCROLL"))		!= xstring::npos) dwStyle |= WS_HSCROLL;
		if(strStyle.find(_T("WS_GROUP"))		!= xstring::npos) dwStyle |= WS_GROUP;
		if(strStyle.find(_T("WS_TABSTOP"))		!= xstring::npos) dwStyle |= WS_TABSTOP;
	}
	void AddStyle(LPCTSTR xx, DWORD style){
		if(strStyle.find(xx) != xstring::npos) dwStyle |= style;
	}
	DWORD	dwStyle;
	xstring strStyle;
};

class XExStyleParser
{

};

class XCssParser
    : implement ICssParser
{
public:
    XCssParser()
    {
        m_rcOutter		= NULL_RECT;
        m_rcInner		= NULL_RECT;
		m_rcBlur		= NULL_RECT;
        m_pBkImage		= NULL;
        m_pResLoader	= NULL;
		m_dwExStyle		= 0;
		m_dwStyle		= 0;
    }
    virtual ~XCssParser()
    {
        ReleaseBkImg();
    }

    virtual void Release()
    {

    }

    virtual BOOL Parse(IProperty* pProp)
    {
        return ParseCss(pProp);
    }

    virtual BOOL ParseCss(IProperty* pProp)
    {
        if(pProp == NULL) return FALSE;

        LPCTSTR border = pProp->GetProperty(_T("border"));
        if(border)
        {
            RECT rect;  CTextUtil::TextToRect(border,rect);

            WndBorder wb;
            wb.pix = rect.bottom;
            wb.clr = RGB(rect.left, rect.top, rect.right);
            m_brLeft = m_brRight = m_brTop = m_brBottom = wb;
            m_brAll	 = wb;
        }

        LPCTSTR brLeft = pProp->GetProperty(_T("left-border"));
        if(brLeft)
        {
            RECT rect; CTextUtil::TextToRect(brLeft, rect);
            m_brLeft.pix = rect.bottom;
            m_brLeft.clr = RGB(rect.left, rect.top, rect.right);
        }
        LPCTSTR brRight = pProp->GetProperty(_T("right-border"));
        if(brRight){
            RECT rect; CTextUtil::TextToRect(brRight, rect);
            m_brRight.pix = rect.bottom;
            m_brRight.clr = RGB(rect.left, rect.top, rect.right);
        }
        LPCTSTR brTop = pProp->GetProperty(_T("top-border"));
        if(brTop)
        {
            RECT rect; CTextUtil::TextToRect(brTop, rect);
            m_brTop.pix = rect.bottom;
            m_brTop.clr = RGB(rect.left, rect.top, rect.right);
        }
        LPCTSTR brBottom = pProp->GetProperty(_T("bottom-border"));
        if(brBottom)
        {
            RECT rect; CTextUtil::TextToRect(brBottom, rect);
            m_brBottom.pix = rect.bottom;
            m_brBottom.clr = RGB(rect.left, rect.top, rect.right);
        }
        LPCTSTR  bkclr = pProp->GetProperty(_T("bkgrd-color"));
        if(bkclr)
        {
            m_sBkColor = bkclr;
        }

        LPCTSTR  bkrgn = pProp->GetProperty(_T("bkgrd-region"));
        if(bkrgn)
        {
            m_sBkRgn = bkrgn;
        }


        LPCTSTR  bkmode = pProp->GetProperty(_T("bkgrd-stretch-mode"));
        if(bkmode){
            m_sBkMode	= bkmode;
        }

        LPCTSTR bkgrd_zone =  pProp->GetProperty(_T("bkgrd-stretch-zone"));
        if(bkgrd_zone)
        {
            CTextUtil::TextToRect(bkgrd_zone,m_rcInner);
        }	

        LPCTSTR bkgrd =  pProp->GetProperty(_T("bkgrd-image"));
        if(bkgrd && _tcslen(bkgrd))
        {
            LoadBkgrdImage(bkgrd, NULL, &m_rcInner);
        }

		LPCTSTR blur_border = pProp->GetProperty(_T("blur-border"));
		if(blur_border)
		{
			 CTextUtil::TextToRect(blur_border,m_rcBlur);
		}

		bkgrd =  pProp->GetProperty(_T("bkgrd-left"));
		if(bkgrd && _tcslen(bkgrd))
		{
			CTextUtil::TextToColor(bkgrd, m_crCalcBk);
		}

		bkgrd =  pProp->GetProperty(_T("bkgrd-right"));
		if(bkgrd && _tcslen(bkgrd))
		{
			CTextUtil::TextToColor(bkgrd, m_crGridRight);
		}

        return TRUE;
    }

	virtual void ChangeSkinBkColor(LPCTSTR pBkCr)
	{
		if(!pBkCr)
			m_sBkColor.clear();
		else
			m_sBkColor = pBkCr;
	}
	virtual void ChangeSkinImage(LPCTSTR bkgrd )
	{
		if(bkgrd && _tcslen(bkgrd))
		{
			LoadBkgrdImage(bkgrd, NULL, &m_rcInner);
		}
	}

protected:
    void ReleaseBkImg()
    {
        if(!m_sLoadPath.empty() && m_pResLoader){
            m_pResLoader->UnloadImage(m_sLoadPath.c_str());
        }
        m_sLoadPath = _T("");
        m_pBkImage  = NULL;
    }

    void LoadBkgrdImage(LPCTSTR path, LPRECT outter, LPRECT inner)
    {	
        if(m_pResLoader == NULL) return;

        ReleaseBkImg();

        xgdi::IImage* pImage = NULL;
		if(m_sBkMode.find(_T("DIF_CALABKCOLOR")) != xstring::npos)
			pImage = m_pResLoader->LoadImage(path, m_crCalcBk);
		else
			pImage = m_pResLoader->LoadImage(path);

        if(pImage)
        {
            SetBkgrdImage(pImage, outter, inner);		
            m_sLoadPath = path;
        }
    }
    void SetBkgrdImage(xgdi::IImage* image, LPRECT outter, LPRECT inner)
    {		
        ReleaseBkImg();

        m_pBkImage = image;
        m_rcOutter = xgdi::CImgStreRect(image, outter);
        m_rcInner  = xgdi::CImgStreRect(image, inner);

		/*if(image && m_sBkMode.find(_T("DIF_CALABKCOLOR")) != xstring::npos)
		{
			Gdiplus::Bitmap bmp((HBITMAP)image->GetUIHandle(), NULL);		
			m_crCalcBk = HalftoneBitmap(&bmp);
		}*/
    }

	COLORREF HalftoneBitmap(Gdiplus::Bitmap* background_image) 
	{
		if(background_image == NULL)
			return 0;
		Gdiplus::BitmapData bitmap_data;
		int img_width = background_image->GetWidth();
		int img_height = background_image->GetHeight();
		Gdiplus::Rect lock_rect(0, 0, img_width, img_height);
		background_image->LockBits(&lock_rect, Gdiplus::ImageLockModeWrite,
			PixelFormat24bppRGB, &bitmap_data);
		unsigned char* pixel = static_cast<unsigned char*>(bitmap_data.Scan0);
		DWORD sum_h = 0;
		DWORD sum_l = 0;
		DWORD sum_s = 0;
		// 1.计算半色调
		int max_h = (img_width > 100) ? 100 : img_width;
		int max_v = (img_height > 100) ? 100 : img_height;
		for (int i = 0; i < max_v; ++i) {
			int offset = i * bitmap_data.Stride;
			for (int j = 0; j < max_h; ++j) {
				unsigned char* it = pixel + offset + j * 3;
				unsigned char b = *it++;
				unsigned char g = *it++;
				unsigned char r = *it++;
				WORD h, l, s;
				ColorRGBToHLS(RGB(r, g, b), &h, &l, &s);
				sum_h += h;
				sum_l += l;
				sum_s += s;
			}
		}
		COLORREF halftone_color = ColorHLSToRGB(
			static_cast<WORD>(sum_h / (max_h * max_v)),
			static_cast<WORD>(sum_l / (max_h * max_v)),
			static_cast<WORD>(sum_s / (max_h * max_v)));
		unsigned char halftone_color_r = GetRValue(halftone_color);
		unsigned char halftone_color_g = GetGValue(halftone_color);
		unsigned char halftone_color_b = GetBValue(halftone_color);
		// 2.对图片横向渐变融合
		int multiply = (img_width >= 255) ? 255 : img_width;
		for (int i = 0; i < img_height; ++i) {
			int offset = i * bitmap_data.Stride;
			for (int j = img_width - multiply; j < img_width; ++j) {
				unsigned char* it = pixel + offset + j * 3;
				float alpha_src = static_cast<float>(img_width - j) / multiply;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_b * (1.0f - alpha_src));
				++it;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_g * (1.0f - alpha_src));
				++it;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_r * (1.0f - alpha_src));
				++it;
			}
		}
		// 2.对图片纵向渐变融合
		multiply = (img_height >= 255) ? 255 : img_height;
		for (int i = img_height - multiply; i < img_height; ++i) {
			int offset = i * bitmap_data.Stride;
			for (int j = 0; j < img_width; ++j) {
				unsigned char* it = pixel + offset + j * 3;
				float alpha_src = (float)(img_height - i) / 255;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_b * (1.0f - alpha_src));
				++it;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_g * (1.0f - alpha_src));
				++it;
				*it = static_cast<unsigned char>(*it * alpha_src +
					halftone_color_r * (1.0f - alpha_src));
				++it;
			}
		}
		background_image->UnlockBits(&bitmap_data);

		return halftone_color;
	}

    void DrawBorder(xgdi::ICanvas* pCanvas, RECT rcClient, RECT rcUpdate)
    {
        if(!m_sBkRgn.empty())
        {
            return ;
        }
        if(m_brLeft.pix){
            xgdi::XGDIObject<xgdi::IPen> pPen = xgdi::CreateGDI<xgdi::IPen, xgdi::IID_UIPen>();
            pPen->CreatePen(PS_SOLID, m_brLeft.pix, m_brLeft.clr);

            pCanvas->Line(pPen, rcClient.left + m_brLeft.pix/2,
                rcClient.top  + m_brTop.pix/2,
                rcClient.left + m_brLeft.pix/2,
                rcClient.bottom  - upper_count(m_brBottom.pix,2));
        }

        if(m_brTop.pix){
            xgdi::XGDIObject<xgdi::IPen> pPen = xgdi::CreateGDI<xgdi::IPen, xgdi::IID_UIPen>();
            pPen->CreatePen(PS_SOLID, m_brTop.pix, m_brTop.clr);

            pCanvas->Line(pPen, rcClient.left  + m_brLeft.pix/2 ,
                rcClient.top   + m_brTop.pix/2, 
                rcClient.right - upper_count(m_brRight.pix,2) ,
                rcClient.top   + m_brTop.pix/2);
        }

        if(m_brRight.pix){
            xgdi::XGDIObject<xgdi::IPen> pPen = xgdi::CreateGDI<xgdi::IPen, xgdi::IID_UIPen>();

            pPen->CreatePen(PS_SOLID, m_brRight.pix, m_brRight.clr);
            pCanvas->Line(pPen, rcClient.right - upper_count(m_brRight.pix,2), 
                rcClient.top   + m_brTop.pix/2,
                rcClient.right - upper_count(m_brRight.pix,2), 
                rcClient.bottom - upper_count(m_brBottom.pix,2));
        }

        if(m_brBottom.pix){
            xgdi::XGDIObject<xgdi::IPen> pPen = xgdi::CreateGDI<xgdi::IPen, xgdi::IID_UIPen>();
            pPen->CreatePen(PS_SOLID, m_brBottom.pix, m_brBottom.clr);

            pCanvas->Line(pPen, rcClient.left +  m_brLeft.pix/2,
                rcClient.bottom - upper_count(m_brBottom.pix,2),
                rcClient.right - upper_count(m_brRight.pix,2) + 1, 
                rcClient.bottom - upper_count(m_brBottom.pix,2));
        }
    }

    void DrawBkGrd(xgdi::ICanvas* pCanvas, RECT rcClient, RECT rcUpdate)
	{		
		if(m_sBkMode.find(_T("DIF_CALABKCOLOR")) != xstring::npos)
		{
			pCanvas->FillSolidRect(rcUpdate, m_crCalcBk);
		}
		else if(m_sBkMode.find(_T("DIF_GRIDLEFTRIGHT")) != xstring::npos)
		{
			pCanvas->DrawGradient(rcUpdate, m_crCalcBk, m_crGridRight, GRADIENT_FILL_RECT_H);
		}
		else if(m_sBkMode.find(_T("DIF_GRIDTOPBOTTOM")) != xstring::npos)
		{
			pCanvas->DrawGradient(rcUpdate, m_crCalcBk, m_crGridRight, GRADIENT_FILL_RECT_V);
		}
		else if(!m_sBkRgn.empty())
		{
            SIZE szTemp; CTextUtil::TextToSize(m_sBkRgn.c_str(), szTemp);

            xgdi::XGDIObject<xgdi::IRegion> spRegion;
            spRegion = (xgdi::IRegion*)xgdi::CreateGTUIObj(xgdi::IID_UIRegion);
            spRegion->CreateRoundRectRgn(rcClient,szTemp.cx,szTemp.cy);

            if(!m_sBkColor.empty()){
                COLORREF clr;  CTextUtil::TextToColor(m_sBkColor.c_str(), clr);
                pCanvas->FillSolidRgn(spRegion, clr); //fill region
            }
            if(m_brAll.pix){ //draw region border
                pCanvas->FrameSolidRgn(spRegion, m_brAll.clr, m_brAll.pix, m_brAll.pix);
            }
        }else{
            if(!m_sBkColor.empty()){
                COLORREF clr;  CTextUtil::TextToColor(m_sBkColor.c_str(), clr);
                pCanvas->FillSolidRect(rcUpdate, clr);
            }
        }
    }

    void DrawBkImage(xgdi::ICanvas* pCanvas, RECT rcClient, RECT rcUpdate)
    {
        if(m_pBkImage)
        {
			if(m_sBkMode.find(_T("DIF_BLUR_BORDER")) != xstring::npos)
			{
				COLORREF clrBlur = RGB(255,255,255);
				if(!m_sBkColor.empty()){
					CTextUtil::TextToColor(m_sBkColor.c_str(), clrBlur);
				}
				DrawBlurBorderImage(pCanvas, rcClient,m_pBkImage, m_rcBlur, clrBlur);
			}
			else
			{
				DWORD nFlag = 0;
				if(m_sBkMode.find(_T("DIF_ALIGNLEFT")) != xstring::npos)		nFlag |= xgdi::DIF_ALIGNLEFT;
				if(m_sBkMode.find(_T("DIF_ALIGNTOP")) != xstring::npos)			nFlag |= xgdi::DIF_ALIGNTOP;
				if(m_sBkMode.find(_T("DIF_ALIGNRIGHT")) != xstring::npos)		nFlag |= xgdi::DIF_ALIGNRIGHT;
				if(m_sBkMode.find(_T("DIF_ALIGNBOTTOM")) != xstring::npos)		nFlag |= xgdi::DIF_ALIGNBOTTOM;
				if(m_sBkMode.find(_T("DIF_TILE")) != xstring::npos)				nFlag |= xgdi::DIF_TILE;
				if(m_sBkMode.find(_T("DIF_STRETCH")) != xstring::npos)			nFlag |= xgdi::DIF_STRETCH;
				if(m_sBkMode.find(_T("DIF_TRANSPARENT")) != xstring::npos)		nFlag |= xgdi::DIF_TRANSPARENT;
				if(m_sBkMode.find(_T("DIF_NO_CENTER")) != xstring::npos)		nFlag |= xgdi::DIF_STRE_NO_CENTER;
				if(m_sBkMode.find(_T("DIF_ALPHA")) != xstring::npos)		nFlag |= xgdi::DIF_ALPHA;

				xgdi::DrawStretchImage(pCanvas, m_pBkImage, rcClient, m_rcOutter, m_rcInner,
					nFlag, xgdi::DEF_TRANS_COLOR);
			}
        }
    }

	void DrawBlurBorderImage(xgdi::ICanvas* pCanvas, RECT rcClient,xgdi::IImage* pImage, RECT rcBlur, COLORREF clrBlur)
	{
		xgdi::XGDIObject<xgdi::IBrush> pBrush = xgdi::CreateGDI<xgdi::IBrush, xgdi::IID_UIBrush>();
		pBrush->CreateSolidBrush(clrBlur);

		pCanvas->FillRect(rcClient, pBrush);

		HBITMAP hBlur = xgdi::GetBlurBorderImage(pImage->GetUIHandle(), rcBlur, clrBlur);
		 xgdi::XGDIObject<xgdi::IImage> pImageBlur = xgdi::CreateGDI<xgdi::IImage, xgdi::IID_UIImage>();
		pImageBlur->Attach(hBlur);

		SIZE size = pImage->GetSize();
		RECT rcImage={0,0,size.cx,size.cy};

		pCanvas->DrawImage(pImageBlur, rcClient, rcImage, 0);

	}

protected:
    xgdi::IImage*			m_pBkImage;
    RECT					m_rcOutter;
    RECT					m_rcInner;
	RECT					m_rcBlur;
    WndBorder				m_brLeft;
    WndBorder				m_brTop;
    WndBorder				m_brRight;
    WndBorder				m_brBottom;
    WndBorder				m_brAll;
    xstring					m_sLoadPath;
    xskin::IResLoader*		m_pResLoader;
    xstring					m_sBkMode;
    xstring					m_sBkColor;
    xstring					m_sBkRgn;
	DWORD					m_dwStyle;
	DWORD					m_dwExStyle;
	COLORREF				m_crCalcBk;  //根据图片自动计划背景色
	COLORREF				m_crGridRight;
};

class XWndCssParser
    : public XCssParser
{
public:
    virtual BOOL ParseCss(IProperty* pProp)
    {
        if(pProp == NULL) return FALSE;

        LPCTSTR proper;

        proper = pProp->GetProperty(_T("font"));
        if(proper)
        {
            m_pFont = m_pResLoader->LoadFont(proper);
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

        return xctrl::XCssParser::ParseCss(pProp);
    }

protected:
    xgdi::IFont*        m_pFont;
    COLORREF            m_crFont;
    COLORREF            m_crFontDisabled;
};

NAMESPACE_END(xctrl)