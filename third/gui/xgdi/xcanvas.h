#pragma once
#include "xgdi.h"
#include <GdiPlus.h>

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CCanvasT :
    implement ICanvas
{
public:
    CCanvasT(const CSize& sizeCanvas = 0, HDC hDC = NULL)
    {
        SetSize(sizeCanvas);
        Attach(hDC);
        m_crTransparent = RGB(255,0,255);
        hRgn_			= NULL;
    }

    ~CCanvasT()
    {
        Detach();
    }

    operator HDC() const
    {
        return m_hDC;
    }

    BOOL AlphaBlend(ICanvas* pCanvas, const RECT& rcDest, const RECT& rcSrc, BLENDFUNCTION blender)
    {
        CRect rectDest(rcDest), rectSrc(rcSrc);
        return ::AlphaBlend(
            m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
            *(CCanvas*)pCanvas, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(), blender);
    }

    BOOL AlphaSolidBlend(int nColorRef, const RECT& rcDest, BLENDFUNCTION blender)
    {
        CRect rectDest(rcDest);
        CCanvas canvasSolid(rectDest.Size(), m_hDC);
        canvasSolid.FillSolidRect(rectDest, nColorRef);
        return ::AlphaBlend(
            m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
            canvasSolid, 0, 0, rectDest.Width(), rectDest.Height(), blender);
    }

    BOOL Attach(const UI_HANDLE& hDC)
    {
        if(t_bManaged)
        {
            if(hDC)
            {
                m_hDC			= ::CreateCompatibleDC(hDC);
                m_hMemBitmap	= ::CreateCompatibleBitmap(hDC, m_sizeCanvas.cx, m_sizeCanvas.cy);
                m_hOldBitmap	= (HBITMAP)::SelectObject(m_hDC, (HGDIOBJ)m_hMemBitmap);
				m_hDesDC = hDC;
            }
            else
            {
                m_hDC			= hDC;
            }
        }
        else
        {
            m_hDC			= hDC;
        }

        return (m_hDC == NULL) ? FALSE : TRUE;
    }

    UI_HANDLE Detach()
    {
        if(m_hDC == NULL) return NULL;

        HDC hDC = NULL;

        if(t_bManaged)
        {
            ::SelectObject(m_hDC, (HGDIOBJ)m_hOldBitmap);
            ::DeleteObject((HGDIOBJ)m_hMemBitmap);
            ::DeleteDC(m_hDC);
        }
        else
        {
            hDC = m_hDC;
        }

        return hDC;
    }

    BOOL Flush(__in UI_HANDLE hUI, __in RECT rcDest, RECT rcSrc, __in int nFlag, COLORREF clrKey)
    {
        BOOL bRet = FALSE; 
        if(hUI != NULL)
        {
            CRect rectDest	= rcDest;
            CRect rectSrc	= rcSrc;

            if(nFlag & DIF_TRANSPARENT)
            {
                bRet = ::TransparentBlt(
                    hUI, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
                    m_hDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
                    clrKey);
            }
			else if(nFlag & DIF_ALPHA)
			{
				
				BLENDFUNCTION m_bf;
				m_bf.BlendOp     = AC_SRC_OVER; 
				m_bf.BlendFlags  = 0; 
				m_bf.AlphaFormat = AC_SRC_ALPHA;
				m_bf.SourceConstantAlpha = 255;

				::AlphaBlend(hUI, rectDest.left, rectDest.top, rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, m_hDC, 
					rectSrc.left, rectSrc.top, rectSrc.right - rectSrc.left, rectSrc.bottom - rectSrc.top, m_bf);

			}
            else
            {
                bRet = ::StretchBlt(
                    hUI, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
                    m_hDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
                    SRCCOPY);
            }
            return bRet;
        }
        return FALSE;
    }

    BOOL Flush(ICanvas* pCanvas, RECT rcDest, RECT rcSrc, int nFlag, COLORREF clrKey)
    {			
        return Flush(pCanvas->GetUIHandle(), rcDest, rcSrc, nFlag, clrKey);
    }

    BOOL DrawIcon(IIcon* pIcon, const RECT& rcDest, int nFlag)
    {
        BOOL bRet = FALSE;
        if(NULL == pIcon) return FALSE;

        if(nFlag & xgdi::DIF_TRANSPARENT)
        {
            bRet = DrawImage(pIcon->GetImage(), rcDest, pIcon->GetRect(), nFlag, pIcon->GetColorKey());
        }
        else
        {
            bRet = DrawImage(pIcon->GetImage(), rcDest, pIcon->GetRect(), nFlag);
        }
        return bRet;
    }

    BOOL DrawIcon(IIcon* pIcon, const RECT& rcDest, const RECT& rcBolder, int nFlag)
    {
        BOOL bRet = FALSE;
        if(NULL == pIcon) return FALSE;

        CRect rcInner = pIcon->GetRect();
        rcInner.DeflateRect(&rcBolder);
		if(rcInner.bottom < 0)
			rcInner.bottom = 0;

        bRet = xgdi::DrawStretchImage(this, pIcon->GetImage(), rcDest, pIcon->GetRect(), rcInner, nFlag, pIcon->GetColorKey());

        return bRet;
    }

	BOOL DrawImage(__in IImage* pImage, __in const RECT& rcDest, const __in RECT& rcSrc, const __in RECT& rcBolder, __in int nFlag)
	{
		BOOL bRet = FALSE;
		if(NULL == pImage) return FALSE;

		CRect rcInner(rcSrc);
		rcInner.DeflateRect(&rcBolder);
		if(rcInner.bottom < 0)
			rcInner.bottom = 0;

		bRet = xgdi::DrawStretchImage(this, pImage, rcDest, rcSrc, rcInner, nFlag, m_crTransparent);

		return bRet;
	}

    BOOL DrawImage(ICanvas* pCanvas, const RECT& rcDest, const RECT& rcSrc, int nFlag, COLORREF clrKey)
    {
        BOOL bRet = FALSE;

        CRect rectDest(rcDest), rectSrc(rcSrc);
        if(rectSrc.right == -1) rectSrc.right = pCanvas->GetSize().cx;
        if(rectSrc.bottom == -1) rectSrc.bottom = pCanvas->GetSize().cy;

        HDC hBitmapDC = pCanvas->GetUIHandle();
   
        // Tile Mode
        if(nFlag & DIF_TILE)
        {
            bRet = FALSE;
        }
        // Center Mode
        else
        {
            int xOffset =   (rectDest.Width() - rectSrc.Width()) / 2;
            int yOffset =   (rectDest.Height() - rectSrc.Height()) / 2;

            if(nFlag & DIF_ALIGNLEFT)		xOffset = 0;
            if(nFlag & DIF_ALIGNTOP)		yOffset = 0;
            if(nFlag & DIF_ALIGNRIGHT)		xOffset += xOffset;
            if(nFlag & DIF_ALIGNBOTTOM)     yOffset += yOffset;

            rectDest.OffsetRect(xOffset, yOffset);
            //yOffset>0? rectDest.top   += yOffset : rectSrc.top -= yOffset;
            //xOffset>0? rectDest.left   += xOffset : rectSrc.left -= xOffset;

			if(nFlag & DIF_ALPHA)
			{

				BLENDFUNCTION m_bf;
				m_bf.BlendOp     = AC_SRC_OVER; 
				m_bf.BlendFlags  = 0; 
				m_bf.AlphaFormat = AC_SRC_ALPHA;
				m_bf.SourceConstantAlpha = 255;

				::AlphaBlend(m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
					hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),m_bf);
			}
			else
			{
				bRet = (nFlag & DIF_TRANSPARENT) ?
					::TransparentBlt(
					m_hDC, rectDest.left, rectDest.top, rectSrc.Width(), rectSrc.Height(),
					hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
					clrKey) :
				::BitBlt(
					m_hDC, rectDest.left, rectDest.top, rectSrc.Width(), rectSrc.Height(),
					hBitmapDC, rectSrc.left, rectSrc.top, SRCCOPY);
			}
		}

		return bRet;
	}

    BOOL DrawImage(IImage* pImage, const RECT& rcDest, const RECT& rcSrc, int nFlag)
    {
        return DrawImage(pImage, rcDest, rcSrc, nFlag, m_crTransparent);
    }

    BOOL DrawImage(IImage* pImage, const RECT& rcDest, const RECT& rcSrc, int nFlag, COLORREF clrKey)
    {
		if(rcDest.bottom - rcDest.top <= 0 || rcDest.right - rcDest.left <= 0)
			return FALSE;
        BOOL bRet = FALSE;
        if(pImage)
        {
            HDC hBitmapDC = ::CreateCompatibleDC(m_hDC);
		
            HBITMAP hOldBmp = (HBITMAP)::SelectObject(hBitmapDC, (HGDIOBJ)(HBITMAP)(pImage->GetUIHandle()));

            CRect rectDest(rcDest), rectSrc(rcSrc);
            if(rectSrc.right == -1) rectSrc.right = pImage->GetSize().cx;
            if(rectSrc.bottom == -1) rectSrc.bottom = pImage->GetSize().cy;

			
            // Stretch Mode
            if(nFlag & DIF_STRETCH)
            {
                // auto-stretching disabled
                // ...
				if ( nFlag & DIF_GRAY )
				{
					// 把图片变成灰度，需要拷贝HBITMAP [刘睿 2010-5-6]
					HDC hTmpDC = ::CreateCompatibleDC(m_hDC);

					LPBITMAPINFO lpbi;

					// Fill in the BITMAPINFOHEADER
					lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
					lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					lpbi->bmiHeader.biWidth = rectSrc.right;
					lpbi->bmiHeader.biHeight = rectSrc.bottom;
					lpbi->bmiHeader.biPlanes = 1;
					lpbi->bmiHeader.biBitCount = 32;
					lpbi->bmiHeader.biCompression = BI_RGB;
					lpbi->bmiHeader.biSizeImage = rectSrc.right * rectSrc.bottom;
					lpbi->bmiHeader.biXPelsPerMeter = 0;
					lpbi->bmiHeader.biYPelsPerMeter = 0;
					lpbi->bmiHeader.biClrUsed = 0;
					lpbi->bmiHeader.biClrImportant = 0;

					COLORREF* pBits = NULL;
					HBITMAP hDibBmp = CreateDIBSection (
						hTmpDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
						NULL, NULL);

					HBITMAP hOldTmp = (HBITMAP)::SelectObject(hTmpDC, hDibBmp);

					::BitBlt(hTmpDC, 0, 0, rectSrc.right, rectSrc.bottom, hBitmapDC, 0, 0, SRCCOPY);

					for ( int i = 0; i < rectSrc.right; i++ )
					{
						for ( int j = 0; j < rectSrc.bottom; j++ )
						{
							COLORREF color = ::GetPixel(hBitmapDC, i, j);
							BYTE r = GetRValue(color);
							BYTE g = GetGValue(color);
							BYTE b = GetBValue(color);
							if ( 255 != r || 0 != g || 255 !=b )
							{
								int gray = r*0.3 + g*0.59 + b*0.11;
								color = RGB(gray, gray, gray);
								::SetPixel(hTmpDC, i, j, color);
							}

						}
					}

					if(nFlag & DIF_ALPHA)
					{

						BLENDFUNCTION m_bf;
						m_bf.BlendOp     = AC_SRC_OVER; 
						m_bf.BlendFlags  = 0; 
						m_bf.AlphaFormat = AC_SRC_ALPHA;
						m_bf.SourceConstantAlpha = 255;

						//::SelectObject(m_hDC, (HGDIOBJ)(HBITMAP)(pImage->GetUIHandle()));

						::AlphaBlend(m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),m_bf);
					}
					else
					{
						bRet = (nFlag & DIF_TRANSPARENT) ?
							::TransparentBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hTmpDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							clrKey) :
						::StretchBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hTmpDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							SRCCOPY);
					}

					::SelectObject(hTmpDC, (HGDIOBJ)hOldTmp);
					::DeleteDC(hTmpDC);

					delete[] lpbi;
				}
				else
				{
					if(nFlag & DIF_ALPHA)
					{

						BLENDFUNCTION m_bf;
						m_bf.BlendOp     = AC_SRC_OVER; 
						m_bf.BlendFlags  = 0; 
						m_bf.AlphaFormat = AC_SRC_ALPHA;
						m_bf.SourceConstantAlpha = 255;

						//::SelectObject(m_hDC, (HGDIOBJ)(HBITMAP)(pImage->GetUIHandle()));

						::AlphaBlend(m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),m_bf);
					}
					else
					{
						bRet = (nFlag & DIF_TRANSPARENT) ?
							::TransparentBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							clrKey) :
						::StretchBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							SRCCOPY);
					}
				}
     
                // ...

            }
            // Tile Mode
            else if(nFlag & DIF_TILE)
            {
                bRet = FALSE;
            }
            // Center Mode
            else
            {
                int xOffset =   (rectDest.Width() - rectSrc.Width()) / 2;
                int yOffset =   (rectDest.Height() - rectSrc.Height()) / 2;

				//changby ljs 2011.3.3
               /* if(nFlag & DIF_ALIGNLEFT)		xOffset = 0;
                if(nFlag & DIF_ALIGNTOP)		yOffset = 0;
                if(nFlag & DIF_ALIGNRIGHT)		xOffset += xOffset;
                if(nFlag & DIF_ALIGNBOTTOM)     yOffset += yOffset;

                rectDest.OffsetRect(xOffset, yOffset);*/

				if((nFlag & DIF_CENTER_ALL_SHOW) && (rectSrc.Width() > rectDest.Width() || rectSrc.Height() > rectDest.Height()))
				{
					float nWidth = (float)rectSrc.Width() / rectDest.Width();
					float nHeigth = (float)rectSrc.Height() / rectDest.Height();
					if(nWidth > nHeigth)
					{
						int nTemp = rectSrc.Height() / nWidth;
						rectDest.top = rectDest.top + (rectDest.Height() - nTemp) / 2;
						rectDest.bottom = rectDest.top + nTemp;
					}
					else
					{
						int nTemp = rectSrc.Width() / nHeigth;
						rectDest.left = (rectDest.Width() - nTemp) / 2;
						rectDest.right = rectDest.left + nTemp;
					}
				}
				else
				{

					if(nFlag & DIF_ALIGNLEFT)
					{
						rectDest.right = rectDest.left + rectSrc.Width();
					}
					else if(nFlag & DIF_ALIGNRIGHT)
					{
						rectDest.left = rectDest.right - rectSrc.Width();
					}
					else
					{
						rectDest.left += xOffset;
						rectDest.right = rectDest.left + rectSrc.Width();
					}
					if(nFlag & DIF_ALIGNTOP)
					{
						rectDest.bottom = rectDest.top + rectSrc.Height();
					}
					else if(nFlag & DIF_ALIGNBOTTOM)
					{
						rectDest.top = rectDest.bottom - rectSrc.Height();
					}

					else
					{
						rectDest.top += yOffset;
						rectDest.bottom = rectDest.top + rectSrc.Height();
					}
				}
				//end change by ljs


                //yOffset>0? rectDest.top   += yOffset : rectSrc.top -= yOffset;
                //xOffset>0? rectDest.left   += xOffset : rectSrc.left -= xOffset;

				if ( nFlag & DIF_GRAY )
				{
					// 灰度显示
					HDC hTmpDC = ::CreateCompatibleDC(m_hDC);

					LPBITMAPINFO lpbi;

					// Fill in the BITMAPINFOHEADER
					lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
					lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					lpbi->bmiHeader.biWidth = rectSrc.right;
					lpbi->bmiHeader.biHeight = rectSrc.bottom;
					lpbi->bmiHeader.biPlanes = 1;
					lpbi->bmiHeader.biBitCount = 32;
					lpbi->bmiHeader.biCompression = BI_RGB;
					lpbi->bmiHeader.biSizeImage = rectSrc.right * rectSrc.bottom;
					lpbi->bmiHeader.biXPelsPerMeter = 0;
					lpbi->bmiHeader.biYPelsPerMeter = 0;
					lpbi->bmiHeader.biClrUsed = 0;
					lpbi->bmiHeader.biClrImportant = 0;

					COLORREF* pBits = NULL;
					HBITMAP hDibBmp = CreateDIBSection (
						hTmpDC, lpbi, DIB_RGB_COLORS, (void **)&pBits,
						NULL, NULL);

					HBITMAP hOldTmp = (HBITMAP)::SelectObject(hTmpDC, hDibBmp);

					::BitBlt(hTmpDC, 0, 0, rectSrc.right, rectSrc.bottom, hBitmapDC, 0, 0, SRCCOPY);

					for ( int i = 0; i < rectSrc.right; i++ )
					{
						for ( int j = 0; j < rectSrc.bottom; j++ )
						{
							COLORREF color = ::GetPixel(hBitmapDC, i, j);
							BYTE r = GetRValue(color);
							BYTE g = GetGValue(color);
							BYTE b = GetBValue(color);
							if ( 255 != r || 0 != g || 255 !=b )
							{
								int gray = r*0.3 + g*0.59 + b*0.11;
								color = RGB(gray, gray, gray);
								::SetPixel(hTmpDC, i, j, color);
							}

						}
					}

					if(nFlag & DIF_ALPHA)
					{

						BLENDFUNCTION m_bf;
						m_bf.BlendOp     = AC_SRC_OVER; 
						m_bf.BlendFlags  = 0; 
						m_bf.AlphaFormat = AC_SRC_ALPHA;
						m_bf.SourceConstantAlpha = 255;

						//::SelectObject(m_hDC, (HGDIOBJ)(HBITMAP)(pImage->GetUIHandle()));

						::AlphaBlend(m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hTmpDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),m_bf);
					}
					else
					{

						bRet = (nFlag & DIF_TRANSPARENT) ?
							::TransparentBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hTmpDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							clrKey) :
						::StretchBlt(
							m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hTmpDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							SRCCOPY);
					}

					::SelectObject(hTmpDC, (HGDIOBJ)hOldTmp);
					::DeleteDC(hTmpDC);

					delete[] lpbi;
				}
				else
				{
					if(nFlag & DIF_ALPHA)
					{

						BLENDFUNCTION m_bf;
						m_bf.BlendOp     = AC_SRC_OVER; 
						m_bf.BlendFlags  = 0; 
						m_bf.AlphaFormat = AC_SRC_ALPHA;
						m_bf.SourceConstantAlpha = 255;

						//::SelectObject(m_hDC, (HGDIOBJ)(HBITMAP)(pImage->GetUIHandle()));

						::AlphaBlend(m_hDC, rectDest.left, rectDest.top, rectDest.Width(), rectDest.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),m_bf);
					}
					else
					{
						bRet = (nFlag & DIF_TRANSPARENT) ?
							::TransparentBlt(
							m_hDC, rectDest.left, rectDest.top, rectSrc.Width(), rectSrc.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, rectSrc.Width(), rectSrc.Height(),
							clrKey) :
						::BitBlt(
							m_hDC, rectDest.left, rectDest.top, rectSrc.Width(), rectSrc.Height(),
							hBitmapDC, rectSrc.left, rectSrc.top, SRCCOPY);
					}
				}


            }

            ::SelectObject(hBitmapDC, (HGDIOBJ)hOldBmp);
            ::DeleteDC(hBitmapDC);
			
        }

        return bRet;
    }

    int DrawText(IFont* pFont, LPCTSTR lpszText, LPRECT pRectItem, int nFlag)
    {
        int nRet = 0;
        if(pFont)
        {
            HFONT hOldFont = (HFONT)::SelectObject(m_hDC, (HGDIOBJ)(HFONT)(*(CFont*)pFont));
            nRet = ::DrawText(m_hDC, lpszText, -1, pRectItem, nFlag);
            ::SelectObject(m_hDC, (HGDIOBJ)hOldFont);
        }
        else
        {
            nRet = ::DrawText(m_hDC, lpszText, -1, pRectItem, nFlag);
        }

        return nRet;
    }

	int DrawTextGlow(IFont* pFont, LPCTSTR lpszText, LPRECT pRectItem, int nFlag, DWORD nGlowColor)
	{
		int nRet = 0;
		using namespace Gdiplus;
		Graphics graphics(m_hDC);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		LOGFONT fontinfo, fontinfo2;
		::GetObject(*(CFont*)pFont, sizeof(LOGFONT), &fontinfo);
		//((CFont*)pFont)->GetLogFont(fontinfo);
		FontFamily fontFamily(fontinfo.lfFaceName);
		RectF rectf(pRectItem->left, pRectItem->top, pRectItem->right - pRectItem->left, pRectItem->bottom - pRectItem->top);
		//Font font(&fontFamily, fontinfo.lfHeight, FontStyleRegular, UnitPixel);
		/*Font font(m_hDC, (HFONT)(*(CFont*)pFont));
		Unit nTyle = font.GetUnit();
		font.GetLogFontW(&graphics, &fontinfo2);*/
		StringFormat strformat;
		strformat.SetLineAlignment(StringAlignmentCenter);
		GraphicsPath path;
		int nStyle = FontStyleRegular;
		if(fontinfo.lfWeight == 700)
			nStyle |= FontStyleBold;
		if(fontinfo.lfItalic)
			nStyle |= FontStyleItalic;
		if(fontinfo.lfUnderline)
			nStyle |= FontStyleUnderline;

		path.AddString(lpszText, _tcslen(lpszText), &fontFamily, nStyle,
			fontinfo.lfHeight, rectf, &strformat);
		//for (int i = 1; i < 5; i++)
		//{
		//	//Pen pen(Color(32,255,0,0), i);
		//	Pen pen(nGlowColor, i);
		//	pen.SetLineJoin(LineJoinRound);
		//	graphics.DrawPath(&pen, &path);
		//}
		Pen pen(nGlowColor, 4);
		graphics.DrawPath(&pen, &path);
		COLORREF nTextColor = GetTextColor(m_hDC);
		SolidBrush brush(Color(GetRValue(nTextColor),GetGValue(nTextColor),GetBValue(nTextColor)));
		graphics.FillPath(&brush, &path);
		
		//graphics.DrawString(lpszText, -1, &font, rectf,&strformat, &brush);
		return nRet;
	}

    int FillRect(const RECT& rcRect, IBrush* pBrush)
    {
        return ::FillRect(m_hDC, &rcRect, (HBRUSH)(*(CBrush*)pBrush));
    }

    BOOL FillRgn(IRegion* pRegion, IBrush* pBrush)
    {
        if(NULL == pRegion) return -1;

        return ::FillRgn(m_hDC, (HRGN)(*(CRegion*)pRegion), (HBRUSH)(*(CBrush*)pBrush));
    }

    int FillSolidRect(const RECT& rcRect, int nColorRef)
    {
        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        int nRet = ::FillRect(m_hDC, &rcRect, hBrush);
        ::DeleteObject(hBrush);

        return nRet;
    }

    int FillSolidRect(int x1, int y1, int x2, int y2, int nColorRef)
    {
        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        RECT rcRect = {x1, y1, x2, y2};
        int nRet = ::FillRect(m_hDC, &rcRect, hBrush);
        ::DeleteObject(hBrush);

        return nRet;
    }

    BOOL FillSolidRgn(IRegion* pRegion, int nColorRef)
    {
        if(NULL == pRegion) return -1;

        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        BOOL bRet = ::FillRgn(m_hDC, (HRGN)(*(CRegion*)pRegion), hBrush);
        ::DeleteObject(hBrush);

        return bRet;
    }

    int FrameRect(const RECT& rcRect, IBrush* pBrush)
    {
        return ::FrameRect(m_hDC, &rcRect, (HBRUSH)(*(CBrush*)pBrush));
    }

    int FrameSolidRect(const RECT& rcRect, int nColorRef)
    {
        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        int nRet = ::FrameRect(m_hDC, &rcRect, hBrush);
        ::DeleteObject(hBrush);

        return nRet;
    }

    int FrameSolidRect(int x1, int y1, int x2, int y2, int nColorRef)
    {
        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        RECT rcRect = {x1, y1, x2, y2};
        int nRet = ::FrameRect(m_hDC, &rcRect, hBrush);
        ::DeleteObject(hBrush);

        return nRet;
    }

    int FrameSolidRgn(IRegion* pRegion, int nColorRef, int nXWeight, int nYWeight)
    {
        if(NULL == pRegion) return -1;

        HBRUSH hBrush = ::CreateSolidBrush(nColorRef);
        int nRet = ::FrameRgn(m_hDC, (HRGN)(*(CRegion*)pRegion), hBrush, nXWeight, nYWeight);
        ::DeleteObject(hBrush);

        return nRet;
    }

    const SIZE& GetSize() const
    {
        return m_sizeCanvas;
    }

    UI_HANDLE GetUIHandle() const
    {
        return m_hDC;
    }

    BOOL Line(IPen* pPen, int x1, int y1, int x2, int y2)
    {
        BOOL bRet;

        if(pPen)
        {
            HPEN hOldPen = (HPEN)::SelectObject(m_hDC, (HGDIOBJ)(HPEN)(*(CPen*)pPen));
            ::MoveToEx(m_hDC, x1, y1, NULL);
            bRet = ::LineTo(m_hDC, x2, y2);
            ::SelectObject(m_hDC, (HGDIOBJ)hOldPen);
        }
        else
        {
            ::MoveToEx(m_hDC, x1, y1, NULL);
            bRet = ::LineTo(m_hDC, x2, y2);
        }

        return bRet;
    }

    BOOL Present(const UI_HANDLE& hDC, const RECT &rcDest, DWORD dwRop)
    {
        return (t_bManaged ? Flush(hDC, rcDest, rcDest, (dwRop ? dwRop : DIF_TRANSPARENT), m_crTransparent) : FALSE);
    }

    //创建一个裁剪区域，区域内的被剪掉，rcArray为所要围成的区域
    void CreateClip(RECT* rcArray, int size)
    {
        if(hRgn_ == NULL){
            hRgn_ = CreateRectRgn(0, 0, m_sizeCanvas.cx, m_sizeCanvas.cy);
            ::CombineRgn(hRgn_, hRgn_, hRgn_, RGN_OR);
        }

        HRGN hMid = CreateRectRgn(0, 0, 0, 0);
        for(unsigned i =0; i < size; ++i){
            HRGN h1 = CreateRectRgn(rcArray[i].left, rcArray[i].top, 
                rcArray[i].right, rcArray[i].bottom);
            ::CombineRgn(hMid, hMid, h1, RGN_OR);
            DeleteObject(h1);
        }

        ::CombineRgn(hRgn_, hRgn_, hMid, RGN_OR);
        DeleteObject(hMid);
        SelectClipRgn(m_hDC, hRgn_);		
    }
    //创建一个裁减区域，区域外的被剪掉，rcArray为所要围成的区域
    HRGN CreateClipOpAnd(RECT* rcArray, int size)
    {
        if(hRgn_ == NULL){
            hRgn_ = CreateRectRgn(0, 0, m_sizeCanvas.cx, m_sizeCanvas.cy);
            ::CombineRgn(hRgn_, hRgn_, hRgn_, RGN_XOR);
        }

        HRGN hMid = CreateRectRgn(0, 0, 0, 0);

        for(unsigned i =0; i < size; ++i){
            HRGN h1 = CreateRectRgn(rcArray[i].left, rcArray[i].top, 
                rcArray[i].right, rcArray[i].bottom);
            ::CombineRgn(hMid, hMid, h1, RGN_OR);
            DeleteObject(h1);
        }

        ::CombineRgn(hRgn_, hRgn_, hMid, RGN_OR);
        ::DeleteObject(hMid);
        ::SelectClipRgn(m_hDC, hRgn_);
        return NULL;
    }

    int Draw3DRect(RECT rcBounds, COLORREF clr1, COLORREF clr2)
    {
        return TRUE;
    }

	virtual int			DrawGradient(RECT rcRect, COLORREF rgbTopLeft, COLORREF rgbBottomRight, DWORD nMode )
	{
		GRADIENT_RECT grdRect = { 0, 1 };
		TRIVERTEX triVertext[ 2 ] = {
			rcRect.left,
			rcRect.top,
			GetRValue( rgbTopLeft ) << 8,
			GetGValue( rgbTopLeft ) << 8,
			GetBValue( rgbTopLeft ) << 8,
			0x0000,			
			rcRect.right,
			rcRect.bottom,
			GetRValue( rgbBottomRight ) << 8,
			GetGValue( rgbBottomRight ) << 8,
			GetBValue( rgbBottomRight ) << 8,
			0x0000
		};
		return ::GradientFill(m_hDC, triVertext, 2, &grdRect, 1, nMode );
		return 0;
	}

    BOOL Clear(COLORREF clr)
    {
        HBRUSH	hBrush	=::CreateSolidBrush(clr);

        RECT	rcBounds;
        ZeroMemory(&rcBounds, sizeof(RECT));
        rcBounds.right	=m_sizeCanvas.cx;
        rcBounds.bottom	=m_sizeCanvas.cy;
        ::FillRect(m_hDC, &rcBounds, hBrush);

        DeleteObject(hBrush);

        return TRUE;
    }

    void ResetClip(HRGN hRgn)
    {
        ::SelectClipRgn(m_hDC, NULL);
        ::DeleteObject(hRgn_);

        hRgn_ = NULL;
    }

    BOOL SetFont(IFont* pFont)
    {
        if(!pFont) return FALSE;
        m_hOldFont = (HFONT)::SelectObject(m_hDC, (HGDIOBJ)(HFONT)(*(CFont*)pFont));

        return m_hOldFont ? TRUE : FALSE;
    }

    BOOL SetPen(IPen* pPen)
    {
        if(!pPen) return FALSE;
        m_hOldPen = (HPEN)::SelectObject(m_hDC, (HGDIOBJ)(HPEN)(*(CPen*)pPen));

        return m_hOldPen ? TRUE : FALSE;
    }

    int SetBkMode(int mode)
    {
        return ::SetBkMode(m_hDC, mode);
    }

    void SetSize(const SIZE& sizeCanvas)
    {
        m_sizeCanvas = sizeCanvas;
    }

    int SetTextColor(int nColor)
    {
        return ::SetTextColor(m_hDC, nColor);
    }

    virtual void Release()
    {
        delete this;
    }

protected:
    HDC			m_hDC;
	HDC			m_hDesDC;

    HBITMAP		m_hMemBitmap, m_hOldBitmap;
    HFONT		m_hOldFont;
    HPEN		m_hOldPen;
    CSize		m_sizeCanvas;
    COLORREF    m_crTransparent;
    HRGN		hRgn_;
};

typedef CCanvasT<true>	CCanvas;
typedef CCanvasT<false>	CCanvasHandle;

NAMESPACE_END(xgdi)