#pragma once
#include "xgdi.h"
#include "xgdidef.h"

#include <GdiPlus.h>
#include <vector>

NAMESPACE_BEGIN(xgdi)

using namespace Gdiplus;


template<bool t_bManaged>
class CImageT
	: implement IImage
{
public:
	CImageT(HBITMAP hBitmap = NULL)
	{
        m_nCurrentFrame = 0;
		m_sizeImage.cx	= 0;
		m_sizeImage.cy	= 0;

        if(hBitmap)
        {
            m_vtFrames.push_back(hBitmap);
        }
	}

	CImageT(LPCTSTR lpszFile)
	{
        m_nCurrentFrame = 0;
        m_sizeImage.cx	= 0;
        m_sizeImage.cy	= 0;

        LoadImage(lpszFile);
	}

    CImageT(HDC hDC, const CRect& rcCapture)
    {
        m_nCurrentFrame = 0;
        m_sizeImage.cx	= rcCapture.Width();
        m_sizeImage.cy	= rcCapture.Height();

        if(HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rcCapture.Width(), rcCapture.Height()))
        {
            HDC hMemDC = ::CreateCompatibleDC(hDC);
            HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, (HGDIOBJ)hBitmap);
            ::BitBlt(
                hMemDC, 0, 0, rcCapture.Width(), rcCapture.Height(),
                hDC, rcCapture.left, rcCapture.top, SRCCOPY);

            hBitmap = (HBITMAP)::SelectObject(hMemDC, (HGDIOBJ)hOldBitmap);
            ::DeleteDC(hMemDC);

            m_vtFrames.push_back(hBitmap);
        }
    }

	~CImageT()
	{
		if(t_bManaged)
		{
			DeleteObject();
		}
	}

	CImageT<t_bManaged> operator =(HBITMAP hBitmap)
	{
		Attach(hBitmap);
		return *this;
	}

	operator HBITMAP() const
	{
        return (m_nCurrentFrame < m_vtFrames.size()) ? m_vtFrames[m_nCurrentFrame] : NULL;
	}

	BOOL DeleteObject()
	{
        if(t_bManaged)
        {
            std::vector<HBITMAP>::iterator iter = m_vtFrames.begin();
            for(; iter != m_vtFrames.end(); iter++)
            {
                ::DeleteObject((HGDIOBJ)*iter);
            }
            m_vtFrames.clear();
            m_nCurrentFrame = 0;
        }
        
        return TRUE;
	}

	BOOL Attach(const UI_HANDLE& hBitmap)
	{
		if(t_bManaged && !m_vtFrames.empty() && m_vtFrames[m_nCurrentFrame] != NULL && m_vtFrames[m_nCurrentFrame] != hBitmap)
			::DeleteObject(m_vtFrames[m_nCurrentFrame]);
		
        if(!m_vtFrames.empty())
        {
            m_vtFrames[m_nCurrentFrame] = hBitmap;
        }
        else
        {
            m_vtFrames.push_back(hBitmap);
        }

		BITMAP bm = {0};      
		if (!::GetObject((HBITMAP)hBitmap, sizeof(BITMAP), (LPBYTE)&bm))   
			return NULL;   

		m_sizeImage.cx = bm.bmWidth;
		m_sizeImage.cy = bm.bmHeight;

        return (m_vtFrames[m_nCurrentFrame] == NULL) ? FALSE : TRUE;
	}

	UI_HANDLE Detach()
	{
		return NULL;
	}

    UINT GetFrameCount() const
    {
        return m_vtFrames.size();
    }

    UINT GetFrameElapse() const
    {
        return m_vtFrameFlapse[m_nCurrentFrame];
    }

	const SIZE& GetSize() const
	{
		return m_sizeImage;
	}

    UI_HANDLE GetUIHandle() const
    {
        return (m_nCurrentFrame < m_vtFrames.size()) ? m_vtFrames[m_nCurrentFrame] : NULL;
    }

	bool IsNull() const
	{
		return m_vtFrames.empty();
	}

    IImage* LoadImage(LPCTSTR lpszFile, const SIZE& size = 0)
    {
        assert(lpszFile);

        DeleteObject();

		if(lpszFile != NULL)
		{
			ATL::CString  strImageName(lpszFile);
			strImageName = strImageName.Right(3);
			strImageName.MakeUpper();
			if(strImageName != _T("GIF"))
			{

				return LoadImageFromFile(lpszFile);
			}
		}

		if(Image* pImage = new Image(lpszFile, FALSE))
        {
           GetFramesFromImage(pImage, size);
            delete pImage;
        }

        return (m_vtFrames.empty() ? NULL : this);
    }

	IImage* LoadImageFromFile(LPCTSTR pFileName)
	{
		if(pFileName == NULL)
		{
			return NULL;
		}
		
		DWORD theFileAttribute = GetFileAttributes(pFileName);
		if( theFileAttribute == -1 )
		{
			return NULL;
		}
		Gdiplus::Bitmap* pBitmap = NULL;
		pBitmap = new Gdiplus::Bitmap(T2CW(pFileName)); 
		if( pBitmap == NULL )
		{
			return NULL;
		}

		if(pBitmap->GetLastStatus() !=  Gdiplus::Ok)
		{
			delete pBitmap;
			pBitmap = NULL;
			return NULL;
		}

		HBITMAP hBitmap = NULL;
		pBitmap->GetHBITMAP(NULL, &hBitmap); 
		if(NULL == hBitmap)
		{
			delete pBitmap;
			pBitmap = NULL;
			return NULL;
		}

		BITMAP hInfo;
		::GetObject(hBitmap, sizeof(BITMAP), &hInfo);
		m_sizeImage.cx      = hInfo.bmWidth;
		m_sizeImage.cy      = hInfo.bmHeight;
		m_vtFrames.push_back(hBitmap);
		delete pBitmap;

		return (m_vtFrames.empty() ? NULL : this);  
	}

	IImage* LoadImage(IStream *  pStreamImage)
	{
		Gdiplus::Bitmap* pBitmap = NULL;

		pBitmap = new Gdiplus::Bitmap(pStreamImage); 
		if( pBitmap == NULL )
		{
			return NULL;
		}

		if(pBitmap->GetLastStatus() !=  Gdiplus::Ok)
		{
			delete pBitmap;
			pBitmap = NULL;
			return NULL;
		}

		HBITMAP hBitmap = NULL;
		pBitmap->GetHBITMAP(NULL, &hBitmap); 
		if(NULL == hBitmap)
		{
			delete pBitmap;
			pBitmap = NULL;
			return NULL;
		}

		BITMAP hInfo;
		::GetObject(hBitmap, sizeof(BITMAP), &hInfo);
		m_sizeImage.cx      = hInfo.bmWidth;
		m_sizeImage.cy      = hInfo.bmHeight;
		m_vtFrames.push_back(hBitmap);
		delete pBitmap;

		return (m_vtFrames.empty() ? NULL : this);  
	}

	virtual	void GetCalcBkColor(COLORREF& cf)
	{
		cf = m_crCalcBk;
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
		if(max_h == 0 || max_v == 0)
			return 0;
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
		m_crCalcBk = halftone_color;
		return halftone_color;
	}

	virtual IImage*		LoadImage(__in void* buff, __in int len, __in_opt const SIZE&, COLORREF& cf )
	{
		assert(buff);
		if(len <= 0)
		{
			return NULL;
		}

		DeleteObject();

		HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, len);  
		void * pData  =  GlobalLock(hGlobal);  
		memcpy(pData, buff, len);  
		GlobalUnlock(hGlobal);  
		IStream *  pStreamImage  =  NULL;  
		if(CreateStreamOnHGlobal(hGlobal, TRUE,  & pStreamImage)  ==  S_OK)
		{
			Gdiplus::Bitmap* pBitmap = NULL;
			pBitmap = new Gdiplus::Bitmap(pStreamImage); 
			if( pBitmap == NULL )
				return NULL;
			cf = HalftoneBitmap(pBitmap);
			HBITMAP hBitmap = NULL;
			pBitmap->GetHBITMAP(NULL, &hBitmap); 

			BITMAP hInfo;
			::GetObject(hBitmap, sizeof(BITMAP), &hInfo);
			m_sizeImage.cx      = hInfo.bmWidth;
			m_sizeImage.cy      = hInfo.bmHeight;
			m_vtFrames.push_back(hBitmap);
			delete pBitmap;
			pStreamImage->Release();
			return (m_vtFrames.empty() ? NULL : this);
		}
		else
			return NULL;
	}
	// begin [6/17/2010 魏永赳]
	virtual IImage*	LoadImage(void* buff, int len, const SIZE& size = 0, LPCTSTR pName = NULL)
	{
		assert(buff);
		if(len <= 0)
		{
			return NULL;
		}

		DeleteObject();

		HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, len);  
		void * pData  =  GlobalLock(hGlobal);  
		memcpy(pData, buff, len);  
		GlobalUnlock(hGlobal);  
		IStream *  pStreamImage  =  NULL;  
		if(CreateStreamOnHGlobal(hGlobal, TRUE,  & pStreamImage)  ==  S_OK)
		{
			if(pName != NULL)
			{
				ATL::CString  strImageName(pName);
				strImageName = strImageName.Right(3);
				strImageName.MakeUpper();
				if(strImageName != _T("GIF"))
				{
					IImage* pReturnImage = LoadImage(pStreamImage);
					pStreamImage->Release();
					return pReturnImage;
				}
			}
			if(Image* pImage = new Image(pStreamImage, TRUE))
			{
				GetFramesFromImage(pImage, size);
				delete pImage;
			}
			pStreamImage->Release();
		}

		return (m_vtFrames.empty() ? NULL : this);
	}
	// end [6/17/2010 魏永赳]
    IImage* LoadImage(const UI_HANDLE& hBitmap, const SIZE& size)
    {
        if(Attach(hBitmap)) m_sizeImage = size;        

        return (m_vtFrames.empty() ? NULL : this);
    }

	// Parlace add to save png file
	//
	BOOL SaveImage(LPCTSTR lpszSavePath)
	{
		if(m_vtFrames.empty())
			return FALSE;

		CLSID nClsid;
		GetEncoderClsid(TEXT("image/png"), &nClsid);

		HBITMAP hBmp = m_vtFrames.at(0);
		Bitmap* bmp = Bitmap::FromHBITMAP(hBmp, NULL);
		return (bmp->Save(lpszSavePath, &nClsid) == Ok);
	}

    void NextFrame(UINT nFrame = -1)
    {
        if(nFrame != -1)
        {
            m_nCurrentFrame = nFrame;
			m_nCurrentFrame %= m_vtFrames.size();
        }
        else
        {
            m_nCurrentFrame ++;
            m_nCurrentFrame %= m_vtFrames.size();
        }
    }

    void PrevFrame(UINT nFrame = -1)
    {
        if(nFrame != -1)
        {
            m_nCurrentFrame = nFrame;
        }
        else
        {
            m_nCurrentFrame --;
            m_nCurrentFrame %= m_vtFrames.size();
        }
    }


	virtual void Release()
	{
		delete this;
	}

	virtual void AdjustHue(__in INT degHue)
	{
		if(m_vtFrames.empty())
			return;
		std::vector<HBITMAP> tmpFrames;
		std::vector<HBITMAP>::iterator iter = m_vtFrames.begin();
		for(; iter != m_vtFrames.end(); iter++)
		{
			HBITMAP hTmp = AdjustHSL(*iter, degHue);
			tmpFrames.push_back(hTmp);
			::DeleteObject((HGDIOBJ)*iter);
		}
		m_vtFrames.clear();
		m_vtFrames = tmpFrames;

	}
private:
	void GetFramesFromImage(Image* pImage,const SIZE& size = 0)
	{
		if(pImage == NULL)
			return;
		m_sizeImage.cx      = pImage->GetWidth();
		m_sizeImage.cy      = pImage->GetHeight();

		UINT nDimensionsCount = pImage->GetFrameDimensionsCount();
		if(GUID *pDimensionIDs = (GUID *)new GUID[nDimensionsCount])
		{
			pImage->GetFrameDimensionsList(pDimensionIDs, nDimensionsCount);
			UINT nFrameCount = pImage->GetFrameCount(&pDimensionIDs[0]);
			for(UINT nIndex = 0; nIndex < nFrameCount; nIndex++)
			{
				pImage->SelectActiveFrame(&pDimensionIDs[0], nIndex);

				Bitmap* pBitmap = new Bitmap(pImage->GetWidth(), pImage->GetHeight(),
					pImage->GetPixelFormat());
				Graphics g(pBitmap);
				g.DrawImage(pImage, 0, 0);

				Color color(255,255,255);
				HBITMAP hBitmap;
				pBitmap->GetHBITMAP(NULL, &hBitmap);
				m_vtFrames.push_back(hBitmap);
							
				delete pBitmap;

			}
			delete []pDimensionIDs;
		}

		if(UINT nLength = pImage->GetPropertyItemSize(PropertyTagFrameDelay))
		{
			PropertyItem* ppi   = (PropertyItem*)new BYTE[nLength];
			pImage->GetPropertyItem(PropertyTagFrameDelay, nLength, ppi);
			int nFrameFlapse = 0;
			for(int i = 0; i< ppi->length/sizeof(UINT) ; i++)
			{
				nFrameFlapse = 0;
				if(!(nFrameFlapse = ((UINT*)ppi->value)[i] * 10))
				{
					nFrameFlapse  = 100;
				}
				m_vtFrameFlapse.push_back(nFrameFlapse);
			}
			delete []ppi;
		}
	}
	BYTE HueToRGB(float rm1,float rm2,float rh)  
	{  
		while(rh > 360.0f)  
			rh -= 360.0f;  
		while(rh < 0.0f)  
			rh += 360.f;  

		if(rh < 60.0f)  
			rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;  
		else if(rh < 180.0f)  
			rm1 = rm2;  
		else if(rh < 240.0f)  
			rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;  

		float n = rm1*255;  
		int m = min((int)n,255);  
		m = max(0,m);  
		return (BYTE)m;  
	}  

	COLORREF HSLtoRGB(float H,float S,float L)  
	{  
		BYTE r,g,b;  

		L = min(1,L);  
		S = min(1,S);  

		if(S == 0.0)  
		{  
			r = g = b = (BYTE)(255 * L);  
		}   
		else   
		{  
			float rm1, rm2;  

			if (L <= 0.5f)   
				rm2 = L + L * S;  
			else  
				rm2 = L + S - L * S;  
			rm1 = 2.0f * L - rm2;     

			r = HueToRGB(rm1, rm2, H + 120.0f);  
			g = HueToRGB(rm1, rm2, H);  
			b = HueToRGB(rm1, rm2, H - 120.0f);  
		}  
		return RGB(r,g,b);  
	}  

	void RGBtoHSL(BYTE R,BYTE G,BYTE B,float* H,float* S,float* L)  
	{  
		BYTE minval = min(R,min(G,B));  
		BYTE maxval = max(R,max(G,B));  
		float mdiff = float(maxval) - float(minval);  
		float msum  = float(maxval) + float(minval);  

		*L = msum / 510.0f;  

		if (maxval == minval)   
		{  
			*S = 0.0f;  
			*H = 0.0f;   
		}     
		else   
		{   
			float rnorm = (maxval - R) / mdiff;        
			float gnorm = (maxval - G) / mdiff;  
			float bnorm = (maxval - B) / mdiff;     

			*S = (*L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));  

			if(R == maxval)   
				*H = 60.0f * (6.0f + bnorm - gnorm);  
			if(G == maxval)
				*H = 60.0f * (2.0f + rnorm - bnorm);  
			if(B == maxval)   
				*H = 60.0f * (4.0f + gnorm - rnorm);  
			if (*H > 360.0f)   
				*H -= 360.0f;  
		}  
	} 

	//调整色调，参数范围：-180~180（度）,=0不作调整  
	HBITMAP AdjustHSL(HBITMAP hBitmap,int degHue)  
	{  
		// 获取对象信息   
		BITMAP bm = {0};      
		if (!::GetObject(hBitmap, sizeof(BITMAP), (LPBYTE)&bm))   
			return NULL;   
		// 定义位图信息   
		BITMAPINFO bi;   
		bi.bmiHeader.biSize = sizeof(bi.bmiHeader);   
		bi.bmiHeader.biWidth = bm.bmWidth;   
		bi.bmiHeader.biHeight = -bm.bmHeight;   
		bi.bmiHeader.biPlanes = 1;   
		bi.bmiHeader.biBitCount = 32;    
		bi.bmiHeader.biCompression = BI_RGB;    
		bi.bmiHeader.biSizeImage = bm.bmWidth * 4 * bm.bmHeight; // 32 bit   
		bi.bmiHeader.biClrUsed = 0;   
		bi.bmiHeader.biClrImportant = 0;   

		// 获取位图数据   
		HDC hdc = GetDC(NULL);   
		BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];   
		::ZeroMemory(pBits, bi.bmiHeader.biSizeImage);   
		if (!::GetDIBits(hdc, hBitmap, 0, bm.bmHeight, pBits, &bi, DIB_RGB_COLORS))   
		{   
			delete pBits;   
			pBits = NULL;   
		}   

		// 对位图数据进行处理   
		COLORREF* pSrc = (COLORREF*)pBits;   
		COLORREF* pDest = (COLORREF*)new BYTE[bi.bmiHeader.biSizeImage];   
		::ZeroMemory(pDest, bi.bmiHeader.biSizeImage);  
		float H,S,L;  
		for (LONG i = 0; i < bm.bmWidth; i++)      
		{      
			for (LONG j = 0; j < bm.bmHeight; j++)      
			{      
				COLORREF* pRGBSrc = &pSrc[j * bm.bmWidth + i];   
				COLORREF* pRGBDest = &pDest[j * bm.bmWidth + i];   

				BYTE b = GetRValue(*pRGBSrc);
				BYTE g = GetGValue(*pRGBSrc);
				BYTE r = GetBValue(*pRGBSrc);
				if ( 255 != r || 0 != g || 255 !=b )
				{
					RGBtoHSL(r,g,b,&H,&S,&L);  
					H += degHue; 
					*pRGBDest = HSLtoRGB(H,S,L);
				}
				else
				{	*pRGBDest = *pRGBSrc; }

			}      
		}     

		// 创建新位图，设置位图数据   
		HBITMAP hGray = ::CreateCompatibleBitmap(hdc, bm.bmWidth, bm.bmHeight);   
		::SetDIBits(hdc, hGray, 0, bm.bmHeight, pDest, &bi, DIB_RGB_COLORS);   

		// 释放资源   
		delete[] pBits;   
		pBits = NULL;   
		delete[] pDest;    
		pDest = NULL;   
		::ReleaseDC(NULL,hdc);      

		return hGray;  
	}  


private:
    IStream* IStreamFromImage(Image* pImage)
    {
        CLSID bmpClsid;
        GetEncoderClsid(TEXT("image/png"), &bmpClsid);

        HGLOBAL     hMem = ::GlobalAlloc(GMEM_MOVEABLE, 0);
        IStream*    pStream = NULL;
        ::CreateStreamOnHGlobal(hMem, TRUE, &pStream);

        pImage->Save(pStream, &bmpClsid, NULL);
        return pStream;
    }

    int GetEncoderClsid(LPCTSTR format, CLSID* pClsid)
    {
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if(size == 0)
            return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if(pImageCodecInfo == NULL)
            return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for(UINT j = 0; j < num; ++j)
        {
            if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
            {
                *pClsid = pImageCodecInfo[j].Clsid;
                free(pImageCodecInfo);
                return j;  // Success
            }    
        }

        free(pImageCodecInfo);
        return -1;  // Failure
    }

    int SplitHBitmap(HBITMAP hBmp, const SIZE& size, std::vector<HBITMAP>* pvtBitmaps)
    {
        return 0;
    }

private:
    std::vector<HBITMAP>    m_vtFrames;
	std::vector<UINT>		m_vtFrameFlapse;
    UINT                    m_nCurrentFrame;
	SIZE					   m_sizeImage;
	COLORREF				m_crCalcBk;
};

typedef CImageT<true>	CImage;
typedef CImageT<false>	CImageHandle;

NAMESPACE_END(xgdi)