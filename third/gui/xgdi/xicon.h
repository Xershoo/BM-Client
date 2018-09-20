#pragma once
#include "xgdi.h"
#include <map>

NAMESPACE_BEGIN(xgdi)

template<bool t_bManaged>
class CIconT
	: implement IIcon
{
public:
    CIconT(IImage* pImage = NULL, CRect rcIcon = 0, COLORREF crKey = RGB(255, 0, 255))
    {
        if(t_bManaged)
        {
            m_hIcon     = NULL;
        }
        else
        {
            m_pImage    = pImage;
            m_crKey     = crKey;
			m_hIcon		= NULL;
        }
        m_rcRect = rcIcon;
    }

    ~CIconT()
    {
        if(t_bManaged)
        {
            m_pImage->Release();
        }
    }

    // From IObject
    virtual void Release()
    {
        delete this;
    }

    // From IUIObject
	virtual BOOL Attach(const UI_HANDLE& hIcon)
	{
        Detach();

        return TRUE;
	}

	virtual UI_HANDLE Detach()
	{
        return NULL;
	}

    virtual UI_HANDLE GetUIHandle() const
    {
        return m_hIcon;
    }

    // From IIcon
    virtual IImage* GetImage()
    {
        return m_pImage;
    }

    virtual const RECT& GetRect()
    {
        return m_rcRect;
    }

    virtual int GetColorKey() const
    {
        return m_crKey;
    }

private:
    HICON           m_hIcon;
    IImage*         m_pImage;
    CRect           m_rcRect;
    COLORREF        m_crKey;
};

typedef CIconT<true>		CIcon;
typedef CIconT<false>	    CIconHandle;

template<bool t_bManaged>
class CIconSetT
    : implement IIconSet
{
public:
    CIconSetT()
        : m_pImage(NULL), m_nxCount(0), m_sizeIcon(0)
    {
    }

    ~CIconSetT()
    {
        if(t_bManaged && m_pImage)
        {
            for(size_t i = 0; i < m_mapIcon.size(); i++)
            {
                if(CIconHandle* pIcon = m_mapIcon[i])
                {
                    pIcon->Release();
                }
            }
            m_pImage->Release();
        }
    }

    // From IObject
    virtual void Release()
    {
        delete this;
    }

    // From IUIObject
    virtual BOOL Attach(const UI_HANDLE& hBitmap)
    {
        return TRUE;
    }

    virtual UI_HANDLE Detach()
    {
        return NULL;
    }

    virtual UI_HANDLE GetUIHandle() const
    {
		if(m_pImage)
		{
			return m_pImage->GetUIHandle();
		}
        return NULL;
    }

    // From IIconSet
    virtual IIcon* GetIcon(int nIndex)
    {
        if(UINT nCount = GetIconCount())
        {
            nIndex %= nCount;
        }

        if(m_pImage == NULL) return NULL;

        if(m_mapIcon[nIndex] == NULL)
        {
            CRect rcIcon(0, 0, m_sizeIcon.cx, m_sizeIcon.cy);
            if(m_nxCount)
            {
                rcIcon.OffsetRect((nIndex % m_nxCount) * m_sizeIcon.cx, (nIndex / m_nxCount) * m_sizeIcon.cy);
                m_mapIcon[nIndex] = new CIconHandle(m_pImage, rcIcon);
            }
        }

        return m_mapIcon[nIndex];
    }

    virtual UINT GetIconCount() const
    {
        return m_nxCount * m_nyCount;
    }

    virtual const SIZE& GetIconSize()
    {
        return m_sizeIcon;
    }

    virtual BOOL LoadIcon(LPCTSTR lpszIcon, const SIZE& size)
    {
        BOOL bRet = FALSE;
        if(t_bManaged)
        {
            m_sizeIcon  = size;

			// 添加判断以支持多次设置 [4/9/2010 何春龙]
			if (m_pImage)
			{
				for(size_t i = 0; i < m_mapIcon.size(); i++)
				{
					if(CIconHandle* pIcon = m_mapIcon[i])
					{
						pIcon->Release();
					}
				}
				m_mapIcon.clear();
				m_pImage->Release();
			}
			m_pImage    = (IImage*)CreateGTUIObj(IID_UIImage);
            bRet        = m_pImage->LoadImage(lpszIcon, CSize(0, 0)) ? TRUE : FALSE;
            m_nxCount   = UINT(m_pImage->GetSize().cx / size.cx);
            m_nyCount   = UINT(m_pImage->GetSize().cy / size.cy);
        }

        return bRet;
    }

    virtual BOOL LoadIcon(IImage* pImage, const SIZE& size)
    {

        BOOL bRet = FALSE;
        if(t_bManaged)
        {
			if (m_pImage)
			{
				for(size_t i = 0; i < m_mapIcon.size(); i++)
				{
					if(CIconHandle* pIcon = m_mapIcon[i])
					{
						pIcon->Release();
					}
				}
				m_mapIcon.clear();
				m_pImage->Release();
				m_pImage = NULL;
			}

           
            m_pImage    = pImage;
            bRet        = TRUE;
			if(m_pImage && size.cx && size.cy)
			{
				m_nxCount   = UINT(m_pImage->GetSize().cx / size.cx);
				m_nyCount   = UINT(m_pImage->GetSize().cy / size.cy); 
				m_sizeIcon  = size;
			}
			else
			{
				m_nyCount = 0;
				m_nxCount = 0;
				m_sizeIcon.cx = m_sizeIcon.cy = 0;
			}
        }

        return bRet;
    }
	virtual void        AdjustHue(__in INT hue)
	{
		if(m_pImage != NULL)
		{
			m_pImage->AdjustHue(hue);
		}
	}
private:
    IImage*                         m_pImage;
    std::map<int, CIconHandle*>     m_mapIcon;
    CSize                           m_sizeIcon;
    UINT                            m_nxCount;
    UINT                            m_nyCount;
};

typedef CIconSetT<true>		CIconSet;
typedef CIconSetT<false>	CIconSetHandle;

NAMESPACE_END(xgdi)