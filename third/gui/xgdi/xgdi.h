/*
 * wtlui_if.h - wtl ui object interface
 *
 */
#pragma once

#include "xbase.h"

const RECT	NULL_RECT	={0,0,0,0};
const POINT NULL_POINT	={0,0};
const SIZE	NULL_SIZE	={0,0};

NAMESPACE_BEGIN(xgdi)

enum UI_OBJECT_IID {
    IID_UIBrush                 = 0,
    IID_UICanvas                ,
    IID_UIFont                  ,
    IID_UIIcon                  ,
    IID_UIIconSet               ,
    IID_UIImage                 ,
    IID_UIPen                   ,
    IID_UIRegion                ,

    IID_UIBrushHandle           ,
    IID_UICanvasHandle          ,
    IID_UIFontHandle            ,
    IID_UIIconHandle            ,
    IID_UIIconSetHandle         ,
    IID_UIImageHandle           ,
    IID_UIPenHandle             ,
    IID_UIRegionHandle          ,

    IID_UIMax
};

#ifndef WIN32
// geometry
typedef struct tagRECT
{
	int left;
	int top;
	int right;
	int bottom;

} RECT, *LPRECT;

typedef struct tagPOINT
{
	int x;
	int y;

} POINT, *LPPOINT;

typedef struct tagSIZE
{
	int cx;
	int cy;

} SIZE, *LPSIZE;

#endif

struct IUIObject;

// painting object
struct ICanvas;

// painting elements
struct IBrush;
struct IFont;
struct IIcon;
struct IIconSet;
struct IImage;
struct IPen;
struct IRegion;

// UI_HANDLE
// e.g. HWND, HDC, etc. for WIN32
struct UI_HANDLE
{
#define DECLARE_UI_HANDLE(handle) \
    UI_HANDLE(handle data) { m_uiHandle = (HANDLE)data; } \
    operator handle() const { return (handle)m_uiHandle; } \
    UI_HANDLE& operator=(handle data) { m_uiHandle = (HANDLE)data; return (*this); }

    DECLARE_UI_HANDLE(HDC)
    DECLARE_UI_HANDLE(HBRUSH)
    DECLARE_UI_HANDLE(HFONT)
    DECLARE_UI_HANDLE(HPEN)
    DECLARE_UI_HANDLE(HBITMAP)
    DECLARE_UI_HANDLE(HRGN)
    DECLARE_UI_HANDLE(HICON)

    DECLARE_UI_HANDLE(HWND)

    DECLARE_UI_HANDLE(BOOL)

private:
    HANDLE m_uiHandle;
};

struct IUIObject
	: public IObject
{
    //************************************
    // Method:    Attach
    // FullName:  xgdi::IUIObject::Attach
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in const UI_HANDLE &
    // Desc:      ���޴��ڶ��󸽼ӵ�һ���������ϣ�����HDC,HWND�ȣ�
    //************************************
    virtual BOOL                Attach(__in const UI_HANDLE&)                                       = 0;
    
    //************************************
    // Method:    Detach
    // FullName:  xgdi::IUIObject::Detach
    // Access:    virtual public 
    // Returns:   xgdi::UI_HANDLE
    // Qualifier:
    // Desc:      ������������޴��ڶ���
    //************************************
    virtual UI_HANDLE           Detach()                                                            = 0;

    //************************************
    // Method:    GetUIHandle
    // FullName:  xgdi::IUIObject::GetUIHandle
    // Access:    virtual public 
    // Returns:   UI_HANDLE
    // Qualifier: const
    // Desc:      ��ȡ�������
    //************************************
    virtual UI_HANDLE           GetUIHandle() const                                                 = 0;
};


struct IBrush
	: public IUIObject
{
	//************************************
	// Method:    CreatePatternBrush
	// FullName:  wtlui::IBrush::CreatePatternBrush
	// Access:    virtual public 
	// Returns:   IBrush*
	// Qualifier:
	// Parameter: IImage *                  ��ˢģʽͼ��
    // Desc:      ����ͼ��ģʽ��ˢ
	//************************************
	virtual IBrush*		CreatePatternBrush(__in IImage*)							                = 0;
	
    //************************************
	// Method:    CreateSolidBrush
	// FullName:  wtlui::IBrush::CreateSolidBrush
	// Access:    virtual public 
	// Returns:   IBrush*
	// Qualifier:
	// Parameter: int                       ��ˢ��ɫ
    // Desc:      ������ɫ��ˢ
	//************************************
	virtual IBrush*		CreateSolidBrush(__in int)								                    = 0;
};

struct ICanvas
	: public IUIObject
{
    //************************************
    // Method:    AlphaBlend
    // FullName:  xgdi::ICanvas::AlphaBlend
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in ICanvas *
    // Parameter: __in const RECT &
    // Parameter: __in const RECT &
    // Parameter: __in BLENDFUNCTION
    // Desc:      ʹ��ָ�������Ե�ǰ��������Alpha��ɫ
    //************************************
    virtual BOOL        AlphaBlend(__in ICanvas*, __in const RECT&, __in const RECT&, __in BLENDFUNCTION)   = 0;

    //************************************
    // Method:    AlphaSolidBlend
    // FullName:  xgdi::ICanvas::AlphaSolidBlend
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in int
    // Parameter: __in const RECT&
    // Parameter: __in BLENDFUNCTION
    // Desc:      �Ի�����ǰ���ݽ��е�ɫAlpha��ɫ
    //************************************
    virtual BOOL        AlphaSolidBlend(__in int, __in const RECT&, __in BLENDFUNCTION)                     = 0;

    //************************************
    // Method:    DrawIcon
    // FullName:  xgdi::ICanvas::DrawIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IIcon *              Ҫ���Ƶ�ͼ��
    // Parameter: __in const RECT &         ָ�����Ƶ�Ŀ������
    // Parameter: __in int                  ��ͼ���
    // Desc:      �ڻ���ָ��λ���ϻ���ͼ��
    //************************************
    virtual BOOL        DrawIcon(__in IIcon*, __in const RECT&, __in int)                                   = 0;

    //************************************
    // Method:    DrawIcon
    // FullName:  xgdi::ICanvas::DrawIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IIcon *
    // Parameter: __in const RECT &
    // Parameter: __in const RECT &         ָ���߽�������
    // Parameter: __in int
    // Desc:      �ڻ���ָ��λ���ϻ���ͼ��
    //************************************
    virtual BOOL        DrawIcon(__in IIcon*, __in const RECT&, __in const RECT&, __in int)                 = 0;

	//************************************
	// Method:    DrawImage
	// FullName:  wtlui::ICanvas::DrawImage
	// Access:    virtual public 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: IImage *                  Ҫ���Ƶ�ͼ��
	// Parameter: const RECT &              �ڻ����ϵ�Ŀ�����λ��
	// Parameter: const RECT &              ָ��ͼ��ľ�������Ĭ��Ϊȫ��ͼ��
	// Parameter: int                       ��ͼ���
    // Desc:      �ڻ���ָ��λ�û���ͼ����ָ���ľ�������
	//************************************
	virtual BOOL		DrawImage(__in IImage*, __in const RECT&, const __in RECT&, __in int nFlag, COLORREF clrKey)    = 0;
	virtual BOOL		DrawImage(__in IImage*, __in const RECT&, const __in RECT&, __in int nFlag)					    = 0;
	virtual BOOL		DrawImage(__in IImage*, __in const RECT&, const __in RECT&, const __in RECT& rcBolder, __in int nFlag)					    = 0;

	//************************************
	// Method:    Flush
	// FullName:  xgdi::ICanvas::Flush
	// Access:    virtual public 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: __in ICanvas *
	// Parameter: __in RECT rcDest
	// Parameter: RECT rcSrc
	// Parameter: __in int nFlag
	// Parameter: COLORREF clrKey
	//************************************
	virtual BOOL		Flush(__in ICanvas*, __in RECT rcDest, RECT rcSrc, __in int nFlag, COLORREF clrKey)			    = 0;
	virtual BOOL		Flush(__in UI_HANDLE, __in RECT rcDest, RECT rcSrc, __in int nFlag, COLORREF clrKey)            = 0;

    //************************************
	// Method:    DrawText
	// FullName:  wtlui::ICanvas::DrawText
	// Access:    virtual public 
	// Returns:   int
	// Qualifier:
	// Parameter: IFont *                   ָ�����ֵ����壨NULL������Ĭ�����壩
	// Parameter: LPCTSTR                   Ҫ���Ƶ���������
	// Parameter: LPRECT                    �ڻ����ϻ������ֵľ�������
	// Parameter: int                       ���Ʊ��
    // Desc:      �ڻ���ָ��λ������ʾ����
	//************************************
	virtual int			DrawText(__in_opt IFont*, __in LPCTSTR, __inout LPRECT, __in int)                   = 0;
	virtual int			DrawTextGlow(__in_opt IFont*, __in LPCTSTR, __inout LPRECT, __in int, __in DWORD nGlowColor) = 0;

	//************************************
	// Method:    FillRect
	// FullName:  wtlui::ICanvas::FillRect
	// Access:    virtual public 
	// Returns:   int
	// Qualifier:
	// Parameter: const RECT &              Ҫ���ľ�������
	// Parameter: IBrush *                  ʹ�õĻ�ˢ
    // Desc:      ��ָ����ˢ��仭���ϵľ�������
	//************************************
	virtual int			FillRect(__in const RECT&, __in IBrush*)						                    = 0;
	
    //************************************
    // Method:    FillRgn
    // FullName:  wtlui::ICanvas::FillRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: IRegion *                 Ҫ���Ĳ���������
    // Parameter: IBrush *                  ʹ�õĻ�ˢ
    // Desc:      ��ָ����ˢ��仭���ϵĲ���������
    //************************************
    virtual int			FillRgn(__in IRegion*, __in IBrush*)							                    = 0;
	
    //************************************
    // Method:    FillSolidRect
    // FullName:  wtlui::ICanvas::FillSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: const RECT &              Ҫ���ľ�������
    // Parameter: int                       ��ˢ��ɫ
    // Desc:      �õ�ɫ��ˢ��仭���ϵľ�������
    //************************************
    virtual int			FillSolidRect(__in const RECT&, __in int)						                    = 0;
	
    //************************************
    // Method:    FillSolidRect
    // FullName:  wtlui::ICanvas::FillSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       Ҫ���ľ�����߽�����
    // Parameter: int                       Ҫ���ľ����ϱ߽�����
    // Parameter: int                       Ҫ���ľ����ұ߽�����
    // Parameter: int                       Ҫ���ľ����±߽�����
    // Parameter: int                       ��ˢ��ɫ
    // Desc:      �õ�ɫ��ˢ��仭���ϵľ�������
    //************************************
    virtual int			FillSolidRect(__in int, __in int, __in int, __in int, __in int)                     = 0;
	
    //************************************
    // Method:    FillSolidRgn
    // FullName:  wtlui::ICanvas::FillSolidRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: IRegion *                 Ҫ���Ĳ���������
    // Parameter: int                       ��ˢ��ɫ
    // Desc:      �õ�ɫ��ˢ��仭���ϵĲ���������
    //************************************
    virtual int			FillSolidRgn(__in IRegion*, __in int)							                    = 0;
	
    //************************************
    // Method:    FrameRect
    // FullName:  wtlui::ICanvas::FrameRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: const RECT &              ���Ƶľ��α߽�
    // Parameter: IBrush *                  ʹ�õĻ�ˢ
    // Desc:      ��ָ����ˢ���ƻ����ϵľ��α߽�
    //************************************
    virtual int			FrameRect(__in const RECT&, __in IBrush*)						                    = 0;
	
    //************************************
    // Method:    FrameSolidRect
    // FullName:  wtlui::ICanvas::FrameSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: const RECT &
    // Parameter: int
    // Desc:      �õ�ɫ��ˢ���ƻ����ϵľ��α߽�
    //************************************
    virtual int			FrameSolidRect(__in const RECT&, __in int)					                        = 0;
	
    //************************************
    // Method:    FrameSolidRect
    // FullName:  wtlui::ICanvas::FrameSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       Ҫ���Ƶľ�����߽�����left
    // Parameter: int                       Ҫ���Ƶľ����ϱ߽�����top
    // Parameter: int                       Ҫ���Ƶľ����ұ߽�����right
    // Parameter: int                       Ҫ���Ƶľ����±߽�����bottom
    // Parameter: int                       ��ˢ��ɫ
    // Desc:      �õ�ɫ��ˢ���ƻ����ϵľ��α߽�
    //************************************
    virtual int			FrameSolidRect(__in int, __in int, __in int, __in int, __in int)			        = 0;
	
    //************************************
    // Method:    FrameSolidRgn
    // FullName:  xgdi::ICanvas::FrameSolidRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in IRegion *            ���Ƶ�Ŀ������
    // Parameter: __in int                  ��ˢ��ɫ
    // Parameter: __in int                  ˮƽ����߽���
    // Parameter: __in int                  ��ֱ����߽���
    // Desc:      �õ�ɫ��ˢ���ƻ����ϵľ��α߽�
    //************************************
    virtual int         FrameSolidRgn(__in IRegion*, __in int, __in int, __in int)                          = 0;


	virtual int			Draw3DRect(RECT rcBounds, COLORREF clr1, COLORREF clr2)								= 0;

	//������ɫ�������ң����ߴ��ϵ���
	virtual int			DrawGradient(RECT rcRect, COLORREF rgbTopLeft, COLORREF rgbBottomRight, DWORD nMode)= 0;

	virtual	BOOL		Clear(COLORREF clr)																	= 0;

    //************************************
    // Method:    GetSize
    // FullName:  xgdi::ICanvas::GetSize
    // Access:    virtual public 
    // Returns:   const SIZE&
    // Qualifier: const
    // Desc:      ��ȡ������С
    //************************************
    virtual const SIZE& GetSize() const                                                                     = 0;

    //************************************
    // Method:    Line
    // FullName:  wtlui::ICanvas::Line
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: IPen *                    ʹ�õĻ��ʣ�ΪNULL��ʹ��Ĭ�ϻ��ʣ�
    // Parameter: int                       ֱ�����ˮƽ����x1
    // Parameter: int                       ֱ����㴹ֱ����y1
    // Parameter: int                       ֱ���յ�ˮƽ����x2
    // Parameter: int                       ֱ���յ㴹ֱ����y2
    // Desc:      ��ָ�����ʻ��ƻ����ϵ�ֱ��
    //************************************
    virtual BOOL		Line(__in_opt IPen*, __in int, __in int, __in int, __in int)                        = 0;
	
    //************************************
    // Method:    Present
    // FullName:  xgdi::ICanvas::Present
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in const UI_HANDLE&
    // Parameter: __in const RECT &
    // Parameter: __in DWORD dwRop
    // Desc:      ������������ʾ��Ŀ���豸��ָ��λ����
    //************************************
    virtual BOOL        Present(__in const UI_HANDLE&, __in const RECT &, __in DWORD dwRop)                 = 0;

    //************************************
    // Method:    SetBkMode
    // FullName:  wtlui::ICanvas::SetBkMode
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       0-������1-͸��
    // Desc:      ���û�������ģʽ
    //************************************
    virtual int			SetBkMode(__in int)										                            = 0;

    //************************************
    // Method:    SetFont
    // FullName:  xgdi::ICanvas::SetFont
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IFont *
    // Desc:      ����Ĭ������
    //************************************
    virtual BOOL        SetFont(__in IFont*)                                                                = 0;

    //************************************
    // Method:    SetPen
    // FullName:  xgdi::ICanvas::SetPen
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IPen *
    // Desc:      ����Ĭ�ϻ���
    //************************************
    virtual BOOL		SetPen(__in IPen*)										                            = 0;

    //************************************
    // Method:    SetSize
    // FullName:  xgdi::ICanvas::SetSize
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const SIZE &
    // Desc:      ���û�����С
    //************************************
    virtual void        SetSize(__in const SIZE&)                                                           = 0;

    //************************************
    // Method:    SetTextColor
    // FullName:  xgdi::ICanvas::SetTextColor
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int
    // Desc:      �����ı���ɫ
    //************************************
   virtual int         SetTextColor(__in int)                                                              = 0;


   //����һ���ü����������ڵı�������rcArrayΪ��ҪΧ�ɵ�����
	virtual void CreateClip(RECT* rcArray, int size)												= 0;
	//����һ���ü�����������ı�������rcArrayΪ��ҪΧ�ɵ�����
	virtual HRGN CreateClipOpAnd(RECT* rcArray, int size)											= 0;

	virtual void ResetClip(HRGN hRgn)																		= 0;
};

struct IFont
	: public IUIObject
{
    //************************************
    // Method:    CreateSimpleFont
    // FullName:  wtlui::IFont::CreateSimpleFont
    // Access:    virtual public 
    // Returns:   IFont*
    // Qualifier:
    // Parameter: LPCTSTR                   �������ƣ�facename��
    // Parameter: const SIZE &              �����С
    // Parameter: BOOL                      �Ƿ�Ϊ����
    // Parameter: BOOL                      �Ƿ�Ϊб��
    // Parameter: BOOL                      �Ƿ����»���
    // Parameter: LPCTSTR                   Ҫװ�ص������ļ�
    // Desc:      ����������
    //************************************
    virtual IFont*      CreateSimpleFont(__in LPCTSTR, __in const SIZE&,
                            __in_opt BOOL, __in_opt BOOL, __in_opt BOOL, __in_opt LPCTSTR)          = 0;
};

struct IImage
	: public IUIObject
{
    //************************************
    // Method:    GetFrameCount
    // FullName:  xgdi::IImage::GetFrameCount
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      ��ȡImage֡��
    //************************************
    virtual UINT        GetFrameCount() const                                                       = 0;

    //************************************
    // Method:    GetFrameElapse
    // FullName:  xgdi::IImage::GetFrameElapse
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier: const
    // Desc:      ��ȡ��ǰ֡����һ֡���ӳ�
    //************************************
    virtual UINT        GetFrameElapse() const                                                      = 0;

	//************************************
	// Method:    GetSize
	// FullName:  wtlui::IImage::GetSize
	// Access:    virtual public 
	// Returns:   const SIZE&
	// Qualifier:
    // Desc:      ��ȡͼ��ʵ�ʴ�С
	//************************************
	virtual const SIZE&	GetSize() const                                                             = 0;

    //************************************
    // Method:    LoadImage
    // FullName:  wtlui::IImage::LoadImage
    // Access:    virtual public 
    // Returns:   IImage*
    // Qualifier:
    // Parameter: __in LPCTSTR              ���ص�ͼ���ļ���
    // Parameter: __in_opt const SIZE&      ָ��ͼ���С
    // Desc:      ʹ��ͼ���ļ�����ͼ��
    //************************************
    virtual IImage*     LoadImage(__in LPCTSTR, __in_opt const SIZE&)                               = 0;
	//  [6/17/2010 κ����]
	virtual IImage*		LoadImage(__in void* buff, __in int len, __in_opt const SIZE&, LPCTSTR pName )				= 0;
	virtual IImage*		LoadImage(__in void* buff, __in int len, __in_opt const SIZE&, COLORREF& cf )				= 0;
    //************************************
    // Method:    LoadImage
    // FullName:  wtlui::IImage::LoadImage
    // Access:    virtual public 
    // Returns:   IImage*
    // Qualifier:
    // Parameter: __in LPCTSTR              ���ص�ͼ���ļ���
    // Parameter: __in const SIZE&          ָ��ͼ���С
    // Desc:      ʹ��UI��������Image
    //************************************
    virtual IImage*     LoadImage(__in const UI_HANDLE&, __in const SIZE&)                          = 0;

	// Parlace add to save png file
	virtual BOOL SaveImage(__in LPCTSTR lpszSavePath)												= 0;

    //************************************
    // Method:    NextFrame
    // FullName:  xgdi::IImage::NextFrame
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in UINT
    // Desc:      ��λ��ʾ֡����
    //************************************
    virtual void        NextFrame(__in UINT)                                                        = 0;

    //************************************
    // Method:    PrevFrame
    // FullName:  xgdi::IImage::PrevFrame
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in UINT
    // Desc:      ��λ��ʾ֡ǰ��
    //************************************
    virtual void        PrevFrame(__in UINT)                                                        = 0;

	//************************************
	//  [12/16/2010 κ����]
	// Method:    AdjustHue
	// FullName:  xgdi::IImage::AdjustHue
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in INT ������Χ��-180~180���ȣ�,=0�������� 
	// Desc:      �ı�ɫ��
	//************************************
	virtual void        AdjustHue(__in INT)                                                        = 0;

	//����ͼƬ������ɫֵ
	virtual	void		GetCalcBkColor(COLORREF& cf)											   = 0;
};

struct IIcon
    : public IUIObject
{
    //************************************
    // Method:    GetImage
    // FullName:  xgdi::IIcon::GetImage
    // Access:    virtual public 
    // Returns:   IImage*
    // Qualifier:
    // Desc:      ��ȡIcon����ͼ��
    //************************************
    virtual IImage*             GetImage()                                                          = 0;

    //************************************
    // Method:    GetRect
    // FullName:  xgdi::IIcon::GetRect
    // Access:    virtual public 
    // Returns:   const RECT&
    // Qualifier:
    // Desc:      ��ȡIcon����ͼ���ϵ�λ��
    //************************************
    virtual const RECT&         GetRect()                                                           = 0;

    //************************************
    // Method:    GetColorKey
    // FullName:  xgdi::IIcon::GetColorKey
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      ��ȡColorKey
    //************************************
    virtual int                 GetColorKey() const                                                 = 0;
}; 

struct IIconSet
    : public IUIObject
{
    //************************************
    // Method:    GetIcon
    // FullName:  xgdi::IIconSet::GetIcon
    // Access:    virtual public 
    // Returns:   IIcon*
    // Qualifier:
    // Parameter: __in int
    // Desc:      ����IconSet�е�һ��ͼ��
    //************************************
    virtual IIcon*              GetIcon(__in int)                                                   = 0;

    //************************************
    // Method:    GetIconCount
    // FullName:  xgdi::IIconSet::GetIconCount
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier: const
    // Desc:      ����IconSet�е�Icon����
    //************************************
    virtual UINT                GetIconCount() const                                                = 0;

    //************************************
    // Method:    GetIconSize
    // FullName:  xgdi::IIconSet::GetIconSize
    // Access:    virtual public 
    // Returns:   const SIZE&
    // Qualifier:
    // Desc��      ����IconSet��ͼ���С
    //************************************
    virtual const SIZE&         GetIconSize()                                                       = 0;

    //************************************
    // Method:    LoadIcon
    // FullName:  xgdi::IIconSet::LoadIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in LPCTSTR              ͼ���ļ�����
    // Parameter: __in const SIZE &         ͼ���С
    // Desc:      װ��ͼ���ļ�
    //************************************
    virtual BOOL                LoadIcon(__in LPCTSTR, __in const SIZE&)                            = 0;

    //************************************
    // Method:    LoadIcon
    // FullName:  xgdi::IIconSet::LoadIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IImage*
    // Parameter: __in const SIZE &
    // Desc:      װ�ش�Image���󴴽�ͼ��
    //************************************
    virtual BOOL                LoadIcon(__in IImage*, __in const SIZE&)                            = 0;

	//************************************
	//  [12/16/2010 κ����]
	// Method:    AdjustHue
	// FullName:  xgdi::IImage::AdjustHue
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in INT ������Χ��-180~180���ȣ�,=0�������� 
	// Desc:      �ı�ɫ��
	//************************************
	virtual void        AdjustHue(__in INT)                                                        = 0;
};

struct IPen
	: public IUIObject
{
	//************************************
	// Method:    CreatePen
	// FullName:  wtlui::IPen::CreatePen
	// Access:    virtual public 
	// Returns:   IPen*
	// Qualifier:
	// Parameter: __in int                  ������ʽ
	// Parameter: __in int                  ���ʿ��
	// Parameter: __in int                  ������ɫ
    // Desc:      ����ָ������
	//************************************
	virtual BOOL		CreatePen(__in int, __in int, __in int)			                            = 0;
};

struct IRegion
	: public IUIObject
{
	//************************************
	// Method:    CreateRectRgn
	// FullName:  wtlui::IRegion::CreateRectRgn
	// Access:    virtual public 
	// Returns:   IRegion*
	// Qualifier:
	// Parameter: __in const RECT &         ��������
    // Desc:      ʹ�þ��δ�������������
	//************************************
	virtual IRegion*	CreateRectRgn(__in const RECT&)							                        = 0;
	
    //************************************
    // Method:    CreateRectRgn
    // FullName:  wtlui::IRegion::CreateRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in int                  ����������߽�����
    // Parameter: __in int                  ���������ϱ߽�����
    // Parameter: __in int                  ���������ұ߽�����
    // Parameter: __in int                  ���������±߽�����
    // Desc:      ʹ�þ��δ�������������
    //************************************
    virtual IRegion*	CreateRectRgn(__in int, __in int, __in int, __in int)                           = 0;

    //************************************
    // Method:    CreateRoundRectRgn
    // FullName:  xgdi::IRegion::CreateRoundRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in const RECT &         ��������
    // Parameter: __in int                  ˮƽ����Բ��ֱ��
    // Parameter: __in int                  ��ֱ����Բ��ֱ��
    // Desc:      ����Բ�Ǿ�������
    //************************************
    virtual IRegion*    CreateRoundRectRgn(__in const RECT&, __in int, __in int)                        = 0;

    //************************************
    // Method:    CreateRoundRectRgn
    // FullName:  xgdi::IRegion::CreateRoundRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in int                  ����������߽�����
    // Parameter: __in int                  ���������ϱ߽�����
    // Parameter: __in int                  ���������ұ߽�����
    // Parameter: __in int                  ���������±߽�����
    // Parameter: __in int                  ˮƽ����Բ��ֱ��
    // Parameter: __in int                  ��ֱ����Բ��ֱ��
    // Desc:      ����Բ�Ǿ�������
    //************************************
    virtual IRegion*    CreateRoundRectRgn(__in int, __in int, __in int, __in int, __in int, __in int)  = 0;
};

template<class T>
class XGDIObject
{
public:
	XGDIObject(){
		__obj = NULL;
	}
	~XGDIObject()
	{
			if(__obj) __obj->Release();
	}
	XGDIObject(T* obj)
	{
			__obj = obj;
	}

	XGDIObject& operator=(T* obj)
	{
			if(__obj) __obj->Release();
			__obj = obj;
			return *this;
	}
	T* operator->()
	{
			return __obj;
	}
	operator T*()
	{
			return __obj;
	}
protected:
	T*	__obj;

};

template<class T, UI_OBJECT_IID XIID>
struct CreateGDI
{
	CreateGDI() {	xobj_ = (T *)CreateGTUIObj(XIID);}

	T* operator->() const { return xobj_; }
	operator T*() const { return xobj_; }
private:
	T *xobj_;	
};

#ifdef GTUIDLL
	#define XCTRLAPI extern "C" __declspec(dllexport)
#else 
	#define  XCTRLAPI extern "C" __declspec(dllimport) 
#endif
//************************************
// Method:    CreateGTUIObj
// FullName:  xgdi::CreateGTUIObj
// Access:    public 
// Returns:   extern "C" IUIObject*
// Qualifier:
// Parameter: UI_OBJECT_IID iid
// Desc:      ����UIObjectʵ��
//************************************
XCTRLAPI IUIObject*   CreateGTUIObj(UI_OBJECT_IID iid);

//************************************
// Method:    InitGTUI
// FullName:  xgdi::CoCreateUIObject
// Access:    public 
// Returns:   extern "C" bool
// Qualifier:
// Parameter: void
// Desc:      ģ���ʼ��
//************************************
XCTRLAPI   bool InitGTUI();
XCTRLAPI void UnInitGTUI();

NAMESPACE_END(xgdi)