#pragma once

#include "xskin.h"
#include <xstring.h>
#include <map>
#include "DeComp.h"

NAMESPACE_BEGIN(xskin)

struct RefObject
{
	BOOL				m_bReload;
    int			        m_cnf;
	xgdi::UI_OBJECT_IID	m_type;
    xgdi::IUIObject*    m_pObject;
};

class CResLoader
    : public xskin::IResLoader
{
public:
    virtual ~CResLoader()
    {
        ObjectMap::iterator itr = __images.begin();
        for(; itr != __images.end(); ++ itr)
        {
            if(xgdi::IUIObject* pObj = itr->second.m_pObject)
            {
                pObj->Release();
            }
        }
        __images.clear();
    }

    static IResLoader* inst()
    {
        static CResLoader skinloader;
        return &skinloader;
    }

	virtual xgdi::IImage*       LoadImage(LPCTSTR texture, COLORREF& cf)
	{
		return (xgdi::IImage*)LoadTexture(texture, cf);
	}

    virtual xgdi::IImage* LoadImage(LPCTSTR lpszTexture)
    {
        return (xgdi::IImage*)LoadTexture(lpszTexture, xgdi::IID_UIImage);
    }
    virtual BOOL UnloadImage(LPCTSTR lpszTexture)	
    {
        return UnloadTexture(lpszTexture);
    }

    virtual xgdi::IFont* LoadFont(LPCTSTR lpszTexture)
    {
        return (xgdi::IFont*)LoadTexture(lpszTexture, xgdi::IID_UIFont);
    }
    virtual BOOL UnloadFont(LPCTSTR lpszTexture)	
    {
        return UnloadTexture(lpszTexture);
    }

    virtual xgdi::IIconSet* LoadIconSet(LPCTSTR lpszTexture)
    {
        return (xgdi::IIconSet*)LoadTexture(lpszTexture, xgdi::IID_UIIconSet);
    }
    virtual BOOL UnloadIconSet(LPCTSTR lpszTexture)	
    {
        return UnloadTexture(lpszTexture);
    }
	//  [1/5/2011 魏永赳]
	void SetReLoad()
	{
		ObjectMap::iterator itr = __images.begin();
		for(; itr != __images.end(); ++itr)
		{
			switch(itr->second.m_type)
			{
			case xgdi::IID_UIImage:
			case xgdi::IID_UIIconSet:
				itr->second.m_bReload = TRUE;
				break;
			case xgdi::IID_UIFont:
				break;
			default:
				break;
			}
		}
	}

	BOOL	ReLoadTexture(LPCTSTR texture)
	{
		ObjectMap::iterator itr = __images.find(texture);
		if(itr != __images.end() && itr->second.m_bReload)
		{
			switch(itr->second.m_type)
			{
			case xgdi::IID_UIImage:
				LoadImageGTR(itr->first.c_str(), (xgdi::IImage*)itr->second.m_pObject);
				break;
			case xgdi::IID_UIIconSet:
				{
					xstring strTexture(itr->first);
					int nPos = strTexture.rfind(_T('@'));
					if(nPos != std::string::npos)
					{
						xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)itr->second.m_pObject;
						xstring strImage = strTexture.substr(0, nPos);
						xstring strSize  = strTexture.substr(nPos);

						int nWidth = 16, nHeight = 16;
						if(_stscanf_s(strSize.c_str(), _T("@%dx%d"), &nWidth, &nHeight))
						{
							xgdi::IImage* pImage = (xgdi::IImage*)CreateImageGTR(strImage.c_str());
							pIconSet->LoadIcon(pImage, CSize(nWidth, nHeight));
						}
					}
				}
				break;
			case xgdi::IID_UIFont:
				break;
			default:
				break;
			}
			itr->second.m_bReload = FALSE;
			return TRUE;
		}
		else
		{
			return TRUE;//只处理已加载过的资源，找不到则无错返回.
		}
	}

	//  [6/28/2010 魏永赳]
	virtual BOOL ReLoad()
	{
		ObjectMap::iterator itr = __images.begin();
		for(; itr != __images.end(); ++itr)
		{
			switch(itr->second.m_type)
			{
			case xgdi::IID_UIImage:
				LoadImageGTR(itr->first.c_str(), (xgdi::IImage*)itr->second.m_pObject);
				break;
			case xgdi::IID_UIIconSet:
				{
					xstring strTexture(itr->first);
					int nPos = strTexture.rfind(_T('@'));
					if(nPos != std::string::npos)
					{
						xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)itr->second.m_pObject;
						xstring strImage = strTexture.substr(0, nPos);
						xstring strSize  = strTexture.substr(nPos);

						int nWidth = 16, nHeight = 16;
						if(_stscanf_s(strSize.c_str(), _T("@%dx%d"), &nWidth, &nHeight))
						{
							xgdi::IImage* pImage = (xgdi::IImage*)CreateImageGTR(strImage.c_str());
							pIconSet->LoadIcon(pImage, CSize(nWidth, nHeight));
						}
					}
				}
				break;
			case xgdi::IID_UIFont:
				break;
			default:
				break;
			}
		}
		return TRUE;
	}
	//  [12/16/2010 魏永赳]
	virtual BOOL	AdjustHue(INT hue)
	{
		ObjectMap::iterator itr = __images.begin();
		for(; itr != __images.end(); ++itr)
		{
			switch(itr->second.m_type)
			{
			case xgdi::IID_UIImage:
				{
					if(itr->second.m_pObject != NULL)
						((xgdi::IImage*)itr->second.m_pObject)->AdjustHue(hue);
				}
				break;
			case xgdi::IID_UIIconSet:
				{
					if(itr->second.m_pObject != NULL)
						((xgdi::IIconSet*)itr->second.m_pObject)->AdjustHue(hue);
				}
				break;
			case xgdi::IID_UIFont:
				break;
			default:
				break;
			}
		}
		return TRUE;
	}

protected:
    inline xgdi::IUIObject* LoadTexture(LPCTSTR lpszTexture, xgdi::UI_OBJECT_IID iid)
    {
		if(_tcslen(lpszTexture) == 0)
		{
			int j = 0;
		}
        ObjectMap::iterator itr = __images.find(lpszTexture);
        if(itr != __images.end())
        {
            itr->second.m_cnf ++;
            return itr->second.m_pObject;
        }

        RefObject rimg = {FALSE,1,iid, NULL};
        switch(iid)
        {
        case xgdi::IID_UIImage: rimg.m_pObject = CreateImageGTR(lpszTexture); break;
        case xgdi::IID_UIFont: rimg.m_pObject = CreateFont(lpszTexture); break;
        case xgdi::IID_UIIconSet: rimg.m_pObject = CreateIconSetGTR(lpszTexture); break;
        default:
            break;
        }
        __images[lpszTexture] = rimg;

        return rimg.m_pObject;
    }

	inline xgdi::IUIObject* LoadTexture(LPCTSTR lpszTexture, COLORREF& cf)
	{
		if(_tcslen(lpszTexture) == 0)
		{
			int j = 0;
		}
		ObjectMap::iterator itr = __images.find(lpszTexture);
		if(itr != __images.end())
		{
			itr->second.m_cnf ++;
			if(itr->second.m_pObject)
				((xgdi::IImage*)itr->second.m_pObject)->GetCalcBkColor(cf);
			return itr->second.m_pObject;
		}

		RefObject rimg = {FALSE,1,xgdi::IID_UIImage, NULL};
		xgdi::IImage* pImage = (xgdi::IImage*)xgdi::CreateGTUIObj(xgdi::IID_UIImage);
		if(	LoadImageGTR(lpszTexture, pImage, cf) == 0)
		{
			pImage->Release();
			return rimg.m_pObject = NULL;
		}
		else
			rimg.m_pObject = pImage;
		__images[lpszTexture] = rimg;

		return rimg.m_pObject;
	}

    inline BOOL UnloadTexture(LPCTSTR lpszTexture)
    {
        ObjectMap::iterator itr = __images.find(lpszTexture);
        if(itr != __images.end())
        {
            if(!(--itr->second.m_cnf))
            {
				if(itr->second.m_pObject)
					itr->second.m_pObject->Release();
                __images.erase(itr);
            }
            return TRUE;
        }else{
            return FALSE;
        }
    }


    inline xgdi::IUIObject* CreateImageGTR(LPCTSTR pTexture)
    {
        xgdi::IImage* pImage = (xgdi::IImage*)xgdi::CreateGTUIObj(xgdi::IID_UIImage);

		// 判断是否绝对路径
		if (_tcschr(pTexture, _T(':')) > 0)
			pImage->LoadImage(pTexture, CSize(0, 0));
		else
		if(	LoadImageGTR(pTexture, pImage) == 0)
		{
			pImage->Release();
			return NULL;
		}

        return pImage;
    }
	inline int LoadImageGTR(LPCTSTR pTexture, xgdi::IImage* pImage)
	{
		int len = _tcslen(pTexture);
		int i = 0;
		int j = -1;
		for(i = 0; i<len; i++)
		{
			if(pTexture[i] == _T('\\') || pTexture[i] == _T('/'))
			{
				j = i;
				break;
			}
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
		if(pGTR != NULL && _tcscmp(pGTR, CDeComp::Faces) != 0)
			CDeComp::Inst()->ExtractToMem(pGTR , pName, &pBuf, &len);
		else
			CDeComp::Inst()->ExtractToMem( CDeComp::Faces, pName, &pBuf, &len);
		if(pGTR) delete[] pGTR;
		pGTR = NULL;

		if (pBuf)
		{			
			pImage->LoadImage(pBuf,len, CSize(0, 0), pName);
			CDeComp::Inst()->FreeBuffer(pBuf);
		}
		else
			return 0;
		return 1;
	}

	inline int LoadImageGTR(LPCTSTR pTexture, xgdi::IImage* pImage, COLORREF& cf)
	{
		int len = _tcslen(pTexture);
		int i = 0;
		int j = -1;
		for(i = 0; i<len; i++)
		{
			if(pTexture[i] == _T('\\') || pTexture[i] == _T('/'))
			{
				j = i;
				break;
			}
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
		if(pGTR != NULL && _tcscmp(pGTR, CDeComp::Faces) != 0)
			CDeComp::Inst()->ExtractToMem(pGTR , pName, &pBuf, &len);
		else
			CDeComp::Inst()->ExtractToMem( CDeComp::Faces, pName, &pBuf, &len);
		if(pGTR) delete[] pGTR;
		pGTR = NULL;

		if (pBuf)
		{			
			pImage->LoadImage(pBuf,len, CSize(0, 0), cf);
			CDeComp::Inst()->FreeBuffer(pBuf);
		}
		else
			return 0;
		return 1;
	}
	
	inline xgdi::IUIObject* CreateImage(LPCTSTR pTexture)
	{
		xgdi::IImage* pImage = (xgdi::IImage*)xgdi::CreateGTUIObj(xgdi::IID_UIImage);
		pImage->LoadImage(pTexture, CSize(0, 0));
		return pImage;
	}


    inline xgdi::IUIObject* CreateFont(LPCTSTR pTexture)
    {
        xgdi::IFont* pFont = (xgdi::IFont*)xgdi::CreateGTUIObj(xgdi::IID_UIFont);

        xstring strTexture(pTexture);
        int nPos = strTexture.find(_T('@'));
        if(nPos != std::string::npos)
        {
            xstring strFace = strTexture.substr(0, nPos);
            xstring strSize = strTexture.substr(nPos);

            int nPx = 12;
            if(_stscanf_s(strSize.c_str(), _T("@%dpx"), &nPx))
            {
                BOOL bBold      = (strTexture.find(_T("#bold")) != -1) ? TRUE : FALSE;
                BOOL bItalic    = (strTexture.find(_T("#italic")) != -1) ? TRUE : FALSE;
                BOOL bUnderline = (strTexture.find(_T("#underline")) != -1) ? TRUE : FALSE;

//                 ATLTRACE(_T("Loading Texture Font:%s %dpx %s%s%s\n"), strFace.c_str(), nPx,
//                     (bBold ? _T("bold ") : _T(" ")),
//                     (bItalic ? _T("italic ") : _T(" ")),
//                     (bUnderline ? _T("underline ") : _T(" "))
//                     );
                pFont->CreateSimpleFont(strFace.c_str(), CSize(0, nPx), bBold, bItalic, bUnderline, NULL);
                return pFont;
            }

        }
        pFont->CreateSimpleFont(pTexture, CSize(0, 12), FALSE, FALSE, FALSE, NULL);
        return pFont;
    }

    inline xgdi::IUIObject* CreateIconSet(LPCTSTR pTexture)
    {
        xstring strTexture(pTexture);
        int nPos = strTexture.rfind(_T('@'));
        if(nPos != std::string::npos)
        {
            xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)CreateGTUIObj(xgdi::IID_UIIconSet);
            xstring strImage = strTexture.substr(0, nPos);
            xstring strSize  = strTexture.substr(nPos);

            int nWidth = 16, nHeight = 16;
            if(_stscanf_s(strSize.c_str(), _T("@%dx%d"), &nWidth, &nHeight))
            {
                ATLTRACE(_T("Loading Texture IconSet:%s@%dx%d\n"), strImage.c_str(), nWidth, nHeight);
                pIconSet->LoadIcon(strImage.c_str(), CSize(nWidth, nHeight));
                return pIconSet;
            }
        }
        return NULL;
    }

	inline xgdi::IUIObject* CreateIconSetGTR(LPCTSTR pTexture)
	{
		xstring strTexture(pTexture);
		int nPos = strTexture.rfind(_T('@'));
		if(nPos != std::string::npos)
		{
			xgdi::IIconSet* pIconSet = (xgdi::IIconSet*)CreateGTUIObj(xgdi::IID_UIIconSet);
			xstring strImage = strTexture.substr(0, nPos);
			xstring strSize  = strTexture.substr(nPos);

			int nWidth = 16, nHeight = 16;
			if(_stscanf_s(strSize.c_str(), _T("@%dx%d"), &nWidth, &nHeight))
			{
				xgdi::IImage* pImage = (xgdi::IImage*)CreateImageGTR(strImage.c_str());
				pIconSet->LoadIcon(pImage, CSize(nWidth, nHeight));
				return pIconSet;
			}
		}
		return NULL;
	}

protected:
    typedef std::map<xstring, RefObject> ObjectMap;
    ObjectMap	__images;
};

NAMESPACE_END(xskin)
