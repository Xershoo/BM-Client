#pragma once
#include "Markup.h"
#include "LayoutEncrypt.h"
#include "DeComp.h"
#include "Env.h"
NAMESPACE_BEGIN(xskin)

typedef struct tagTEXTURE
{
    xstring             name;
    xstring             file;
    xgdi::UI_OBJECT_IID type;
    int                 id;

} TEXTURE, *LPTEXTURE;

typedef std::map<xstring, xgdi::IUIObject*>     SKIN_OBJECT_MAP;
typedef std::map<xstring, LPTEXTURE>            SKIN_TEXTURE_MAP;
typedef std::map<xstring, DWORD>                SKIN_VALUE_MAP;
typedef std::map<xstring, HBITMAP>				SKIN_BITMAP_MAP;

class CSkin
{
private:
    CSkin(BOOL bDelay)
    {
		TCHAR pNameBuffer[MAX_PATH] = { 0};		
		_sntprintf(pNameBuffer, MAX_PATH, _T("other%d.gt"), CEnv::nLanguageId_);
        LoadSkin(pNameBuffer);
    }

    ~CSkin()
    {
        UnLoadSkin();
    }

public:
    inline static CSkin* Instance(BOOL bDelay = FALSE)
    {
        static CSkin Skin(bDelay);

        return &Skin;
    }

    void mapTexture(CMarkup& xml, const xstring& strMapName)
    {
        while(xml.FindChildElem())
        {
            xml.IntoElem();

            if(xml.GetTagName() == _T("style"))
            {
                mapTexture(xml, strMapName + xml.GetAttrib(_T("name")) + _T("."));
            }

            else if(xml.GetTagName() == _T("texture"))
            {
                if(xml.GetAttrib(_T("class")) == _T("value"))
                {
                    m_mapValue[strMapName + xml.GetAttrib(_T("name"))] = TransValue(xml.GetAttrib(_T("value")));
                }
                else
                {
                    LPTEXTURE pTexture = new TEXTURE;
                    pTexture->name = strMapName + xml.GetAttrib(_T("name"));
                    pTexture->id   = 0;
                    pTexture->type = TransType(xml.GetAttrib(_T("class")));
                    pTexture->file = xml.GetAttrib(_T("value"));
                    m_mapTexture[pTexture->name.c_str()] = pTexture;
                }
            }

            xml.OutOfElem();
        }
    }

    BOOL LoadSkin(LPCTSTR lpszSkinPath = _T("other0.gt"))
    {
        UnLoadSkin();
		void* buf = NULL;
		int len = 0;
		CDeComp::Inst()->ExtractToMem(CDeComp::Image,lpszSkinPath, &buf, &len);
		wstring strContent;
		CLayoutEncrypt::DecodeMem((const unsigned char *)buf, len, strContent);
		CDeComp::Inst()->FreeBuffer(buf);

		CMarkup xml;
        if(xml.SetDoc(strContent.c_str()) && xml.FindElem(_T("resource")))
        {
            mapTexture(xml, _T(""));
        }

        return TRUE;
    }

    BOOL UnLoadSkin()
    {
        {
            SKIN_OBJECT_MAP::iterator iter = m_mapObject.begin();
            for(; iter != m_mapObject.end(); iter++)
            {
                if(xgdi::IUIObject* pObj = iter->second)
                {
                    pObj->Release();
                }
            }
            m_mapObject.clear();
        }
        
        {
            SKIN_TEXTURE_MAP::iterator iter = m_mapTexture.begin();
            for(; iter != m_mapTexture.end(); iter++)
            {
                if(LPTEXTURE pTexture = iter->second)
                {
                    delete pTexture;
                }
            }
            m_mapTexture.clear();
        }

		{
			SKIN_BITMAP_MAP::iterator iter = m_mapBitmap.begin();
			for (; iter != m_mapBitmap.end(); iter++)
			{
				if (HBITMAP hBitmap = iter->second)
				{
					DeleteObject(hBitmap);
				}
			}
			m_mapBitmap.clear();
		}

        return TRUE;
    }

	BOOL ReLoad()
	{
		//清空
		{
			SKIN_TEXTURE_MAP::iterator iter = m_mapTexture.begin();
			for(; iter != m_mapTexture.end(); iter++)
			{
				if(LPTEXTURE pTexture = iter->second)
				{
					delete pTexture;
				}
			}
			m_mapTexture.clear();
		}
		//重新加载xml
		void* buf = NULL;
		int len = 0;
		TCHAR pNameBuffer[MAX_PATH] = { 0};		
		_sntprintf(pNameBuffer,MAX_PATH, _T("other%d.gt"), CEnv::nLanguageId_);
		CDeComp::Inst()->ExtractToMem(CDeComp::Image,pNameBuffer, &buf, &len);
		wstring strContent;
		CLayoutEncrypt::DecodeMem((const unsigned char *)buf, len, strContent);
		CDeComp::Inst()->FreeBuffer(buf);

		CMarkup xml;
		if(xml.SetDoc(strContent.c_str()) && xml.FindElem(_T("resource")))
		{
			mapTexture(xml, _T(""));
		}
		//重新加载图
		SKIN_OBJECT_MAP::iterator iter = m_mapObject.begin();
		for(; iter != m_mapObject.end(); iter++)
		{
			// TextFont暂时不重新加载
			if(iter->first == _T("TextFont") || iter->first == _T("TextFontEng"))
				continue;
			if(xgdi::IUIObject* pObj = iter->second)
			{
				LPTEXTURE pTexture = m_mapTexture[iter->first];
				if(pTexture == NULL) continue;
				switch(pTexture->type)
				{
				case xgdi::IID_UIImage: LoadImageGTR(pObj, pTexture->file.c_str()); break;
				case xgdi::IID_UIIconSet: LoadIconSetGTR(pObj, pTexture->file.c_str()); break;
				case xgdi::IID_UIFont: LoadFont(pObj, pTexture->file.c_str()); break;
					break;
				default:
					break;
				}
			}
		}

		return TRUE;
	}

    inline xgdi::IUIObject* LoadTexture(LPCTSTR lpszRes)
    {
        LPTEXTURE pTexture = m_mapTexture[lpszRes];
        if(pTexture == NULL) return NULL;

        xgdi::IUIObject* pObj = NULL;
        if(m_mapObject[pTexture->name.c_str()] == NULL)
        {
            if(pObj = xgdi::CreateGTUIObj(pTexture->type))
            {
                m_mapObject[pTexture->name.c_str()] = pObj;
                switch(pTexture->type)
                {
                case xgdi::IID_UIImage: LoadImageGTR(pObj, pTexture->file.c_str()); break;
                case xgdi::IID_UIFont: LoadFont(pObj, pTexture->file.c_str()); break;
                case xgdi::IID_UIIconSet: LoadIconSetGTR(pObj, pTexture->file.c_str()); break;
                default:
                    break;
                }
            }
        }

        return pObj;
    }

	inline HBITMAP LoadBitmap(LPCTSTR lpszRes)
	{
		LPTEXTURE pTexture = m_mapTexture[lpszRes];
		if(pTexture == NULL) return NULL;

		HBITMAP hBitmap = NULL;
		if(m_mapBitmap[pTexture->name.c_str()] == NULL)
		{
			xstring strBitmapPath = pTexture->file;
			INT nPosition = pTexture->file.find(_T('@'));
			if (nPosition >= 0)
			{
				strBitmapPath = pTexture->file.substr(0, nPosition);
			}
			LPCTSTR szBitmapPath = strBitmapPath.c_str();
			
			//hBitmap = (HBITMAP)::LoadImage(_Module.GetModuleInstance(), szBitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			void * buf = NULL;
			int len = _tcslen(szBitmapPath);
			int i = 0;
			int j = -1;
			for(i = 0; i<len; i++)
			{
				if(szBitmapPath[i] == _T('\\') || szBitmapPath[i] == _T('/'))
					j = i;
			}
			LPTSTR pGTR = NULL;
			if(j > 0)
			{
				pGTR = new TCHAR[j+1];
				ZeroMemory(pGTR, (j+1) * sizeof(TCHAR));
				memcpy(pGTR, szBitmapPath, j*sizeof(TCHAR));
			}
			LPCTSTR pName = szBitmapPath+j+1;
			int ret = CDeComp::Inst()->ExtractToMem((pGTR != NULL ? pGTR : CDeComp::Faces), pName, &buf, &len);
			if(pGTR) delete[] pGTR;
			pGTR = NULL;
			if(buf != NULL)
			{
				BITMAPFILEHEADER* bmpHead = (BITMAPFILEHEADER *)buf;
				BITMAPINFOHEADER* bmp=(BITMAPINFOHEADER* ) ((BYTE*)buf+sizeof(BITMAPFILEHEADER));
				hBitmap = CreateDIBitmap(::GetDC(NULL), bmp, CBM_INIT, (BYTE*)buf+bmpHead->bfOffBits, (BITMAPINFO *)bmp, DIB_RGB_COLORS);
				CDeComp::Inst()->FreeBuffer(buf);
			}

			m_mapBitmap[pTexture->name.c_str()] = hBitmap;
		}

		return hBitmap;
	}

    inline static xgdi::IUIObject* GetObjectByName(LPCTSTR lpszRes)
    {
        if(NULL == m_mapObject[lpszRes])
        {
            //延迟加载
			xgdi::IUIObject* pObject = Instance()->LoadTexture(lpszRes);
            m_mapObject[lpszRes] = pObject;
        }

        return m_mapObject[lpszRes];
    }

    inline static DWORD GetValueByName(LPCTSTR lpszRes)
    {
        return m_mapValue[lpszRes];
    }

	inline static HBITMAP GetBitmapByName(LPCTSTR lpszRes)
	{
		if (NULL == m_mapBitmap[lpszRes])
		{
			m_mapBitmap[lpszRes] = Instance()->LoadBitmap(lpszRes);
		}

		return m_mapBitmap[lpszRes];
	}
	inline static BOOL ReloadGT()
	{
		return Instance()->ReLoad();
	}

private:
    inline xgdi::UI_OBJECT_IID TransType(const xstring& strClass)
    {
        if(strClass == _T("font"))
        {
            return xgdi::IID_UIFont;
        }
        if(strClass == _T("image"))
        {
            return xgdi::IID_UIImage;
        }
        if(strClass == _T("iconset"))
        {
            return xgdi::IID_UIIconSet;
        }

        return xgdi::IID_UIMax;
    }

    inline DWORD TransValue(const xstring& strValue)
    {
        DWORD dwValue = 0;
        xstring strV = strValue;
        strV.erase(0, strV.find_first_not_of(_T(" ")));

        if(strV[0] == _T('#'))
        {
            _stscanf(strV.c_str(), _T("#%x"), &dwValue);
        }
        else
        {
            _stscanf(strV.c_str(), _T("%d"), &dwValue);
        }
        
        return dwValue;
    }

    inline BOOL LoadImage(xgdi::IUIObject* pObj, LPCTSTR pTexture)
    {
        ATLTRACE(_T("Loading Texture Image:%s\n"), pTexture);
        xgdi::IImage* pImage = (xgdi::IImage*)pObj;
        return (pImage->LoadImage(pTexture, CSize(0, 0)) ? TRUE : FALSE);
    }

	inline BOOL LoadImageGTR(xgdi::IUIObject* pObj,LPCTSTR pTexture)
	{
		xgdi::IImage* pImage = (xgdi::IImage*)pObj;
		int len = _tcslen(pTexture);
		int i = 0;
		int j = -1;
		for(i = 0; i<len; i++)
		{
			if(pTexture[i] == _T('\\') || pTexture[i] == _T('/'))
				j = i;
		}
		LPTSTR pGTR = NULL;
		if(j > 0)
		{
			pGTR = new TCHAR[j+1];
			ZeroMemory(pGTR, (j+1) * sizeof(TCHAR));
			memcpy(pGTR, pTexture, j*sizeof(TCHAR));
		}
		
		LPCTSTR pName = pTexture+j+1;
		void* pBuf = NULL;
		len = 0;
		CDeComp::Inst()->ExtractToMem((pGTR != NULL ? pGTR : CDeComp::Faces), pName, &pBuf, &len);
		if(pGTR) delete[] pGTR;
		pGTR = NULL;

		BOOL bRet =  pImage->LoadImage(pBuf,len, CSize(0, 0), pName) != NULL;
		//ATL::CString strFile(_T("D:\\AK源码\\akclient\\src\\bin.vc9\\image_default\\"));
		//strFile += pName;
		//pImage->LoadImage(strFile);
		CDeComp::Inst()->FreeBuffer(pBuf);
		pBuf = NULL;
		return TRUE;
	}

    inline BOOL LoadFont(xgdi::IUIObject* pObj, LPCTSTR pTexture)
    {
        xgdi::IFont* pFont = (xgdi::IFont*)pObj;
		HFONT hFont = pFont->Detach();
		if(hFont != NULL)
			::DeleteObject(hFont);
        ATL::CString strTexture(pTexture);
        int nPos = strTexture.Find(_T('@'));
        if(nPos != -1)
        {
            ATL::CString strFace = strTexture.Left(nPos);
            ATL::CString strSize = strTexture.Mid(nPos);

            int nPx = 12;
            if(_stscanf(strSize, _T("@%dpx"), &nPx))
            {
                BOOL bBold      = (strTexture.Find(_T("#bold")) != -1) ? TRUE : FALSE;
                BOOL bItalic    = (strTexture.Find(_T("#italic")) != -1) ? TRUE : FALSE;
                BOOL bUnderline = (strTexture.Find(_T("#underline")) != -1) ? TRUE : FALSE;

                return (pFont->CreateSimpleFont(strFace, CSize(0, nPx), bBold, bItalic, bUnderline, NULL) == NULL ? FALSE : TRUE);
            }
        }

        return (pFont->CreateSimpleFont(pTexture, CSize(0, 12), FALSE, FALSE, FALSE, NULL) == NULL ? FALSE : TRUE);
    }

    inline BOOL LoadIconSet(xgdi::IUIObject* pObj, LPCTSTR pTexture)
    {
        xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)pObj;

        ATL::CString strTexture(pTexture);
        int nPos = strTexture.ReverseFind(_T('@'));
        if(nPos != -1)
        {
            ATL::CString strImage = strTexture.Left(nPos);
            ATL::CString strSize  = strTexture.Mid(nPos);

            int nWidth = 16, nHeight = 16;
            if(_stscanf(strSize, _T("@%dx%d"), &nWidth, &nHeight))
            {
                ATLTRACE(_T("Loading Texture IconSet:%s@%dx%d\n"), strImage.GetString(), nWidth, nHeight);
                return pIconSet->LoadIcon(strImage.GetString(), CSize(nWidth, nHeight));
            }
        }
        return FALSE;
    }

	inline BOOL LoadIconSetGTR(xgdi::IUIObject* pObj, LPCTSTR pTexture)
	{
		xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)pObj;

		ATL::CString strTexture(pTexture);
		int nPos = strTexture.ReverseFind(_T('@'));
		if(nPos != -1)
		{
			ATL::CString strImage = strTexture.Left(nPos);
			ATL::CString strSize  = strTexture.Mid(nPos);

			int nWidth = 16, nHeight = 16;
			if(_stscanf(strSize, _T("@%dx%d"), &nWidth, &nHeight))
			{
				ATLTRACE(_T("Loading Texture IconSet:%s@%dx%d\n"), strImage.GetString(), nWidth, nHeight);
				xgdi::IImage* pImage    = (xgdi::IImage*)xgdi::CreateGTUIObj(xgdi::IID_UIImage);
				LoadImageGTR(pImage, strImage.GetString());
				return pIconSet->LoadIcon(pImage, CSize(nWidth, nHeight));
			}
		}
		return FALSE;
	}

    static SKIN_OBJECT_MAP          m_mapObject;
    static SKIN_VALUE_MAP           m_mapValue;
    static SKIN_TEXTURE_MAP         m_mapTexture;
	static SKIN_BITMAP_MAP			m_mapBitmap;
};

SKIN_VALUE_MAP CSkin::m_mapValue;
SKIN_OBJECT_MAP CSkin::m_mapObject;
SKIN_TEXTURE_MAP CSkin::m_mapTexture;
SKIN_BITMAP_MAP CSkin::m_mapBitmap;

xgdi::IUIObject* QueryUIObject(LPCTSTR lpszRes)
{
    return CSkin::GetObjectByName(lpszRes);
}

DWORD QueryUIValue(LPCTSTR lpszRes)
{
    return CSkin::GetValueByName(lpszRes);
}

HBITMAP QueryBitmap(LPCTSTR lpszRes)
{
	return CSkin::GetBitmapByName(lpszRes);
}

BOOL GTReload()
{
	return CSkin::ReloadGT();
}

NAMESPACE_END(xskin)

