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
    // Desc:      将无窗口对象附加到一个界面句柄上（例如HDC,HWND等）
    //************************************
    virtual BOOL                Attach(__in const UI_HANDLE&)                                       = 0;
    
    //************************************
    // Method:    Detach
    // FullName:  xgdi::IUIObject::Detach
    // Access:    virtual public 
    // Returns:   xgdi::UI_HANDLE
    // Qualifier:
    // Desc:      分离界面句柄与无窗口对象
    //************************************
    virtual UI_HANDLE           Detach()                                                            = 0;

    //************************************
    // Method:    GetUIHandle
    // FullName:  xgdi::IUIObject::GetUIHandle
    // Access:    virtual public 
    // Returns:   UI_HANDLE
    // Qualifier: const
    // Desc:      获取界面对象
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
	// Parameter: IImage *                  画刷模式图像
    // Desc:      创建图像模式画刷
	//************************************
	virtual IBrush*		CreatePatternBrush(__in IImage*)							                = 0;
	
    //************************************
	// Method:    CreateSolidBrush
	// FullName:  wtlui::IBrush::CreateSolidBrush
	// Access:    virtual public 
	// Returns:   IBrush*
	// Qualifier:
	// Parameter: int                       画刷颜色
    // Desc:      创建单色画刷
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
    // Desc:      使用指定画布对当前画布进行Alpha混色
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
    // Desc:      对画布当前内容进行单色Alpha混色
    //************************************
    virtual BOOL        AlphaSolidBlend(__in int, __in const RECT&, __in BLENDFUNCTION)                     = 0;

    //************************************
    // Method:    DrawIcon
    // FullName:  xgdi::ICanvas::DrawIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IIcon *              要绘制的图标
    // Parameter: __in const RECT &         指定绘制的目标区域
    // Parameter: __in int                  绘图标记
    // Desc:      在画布指定位置上绘制图标
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
    // Parameter: __in const RECT &         指定边界拉伸区
    // Parameter: __in int
    // Desc:      在画布指定位置上绘制图标
    //************************************
    virtual BOOL        DrawIcon(__in IIcon*, __in const RECT&, __in const RECT&, __in int)                 = 0;

	//************************************
	// Method:    DrawImage
	// FullName:  wtlui::ICanvas::DrawImage
	// Access:    virtual public 
	// Returns:   BOOL
	// Qualifier:
	// Parameter: IImage *                  要绘制的图像
	// Parameter: const RECT &              在画布上的目标矩形位置
	// Parameter: const RECT &              指定图像的矩形区域（默认为全部图像）
	// Parameter: int                       绘图标记
    // Desc:      在画布指定位置绘制图像上指定的矩形区域
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
	// Parameter: IFont *                   指定文字的字体（NULL代表画布默认字体）
	// Parameter: LPCTSTR                   要绘制的文字内容
	// Parameter: LPRECT                    在画布上绘制文字的矩形区域
	// Parameter: int                       绘制标记
    // Desc:      在画布指定位置上显示文字
	//************************************
	virtual int			DrawText(__in_opt IFont*, __in LPCTSTR, __inout LPRECT, __in int)                   = 0;
	virtual int			DrawTextGlow(__in_opt IFont*, __in LPCTSTR, __inout LPRECT, __in int, __in DWORD nGlowColor) = 0;

	//************************************
	// Method:    FillRect
	// FullName:  wtlui::ICanvas::FillRect
	// Access:    virtual public 
	// Returns:   int
	// Qualifier:
	// Parameter: const RECT &              要填充的矩形区域
	// Parameter: IBrush *                  使用的画刷
    // Desc:      用指定画刷填充画布上的矩形区域
	//************************************
	virtual int			FillRect(__in const RECT&, __in IBrush*)						                    = 0;
	
    //************************************
    // Method:    FillRgn
    // FullName:  wtlui::ICanvas::FillRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: IRegion *                 要填充的不规则区域
    // Parameter: IBrush *                  使用的画刷
    // Desc:      用指定画刷填充画布上的不规则区域
    //************************************
    virtual int			FillRgn(__in IRegion*, __in IBrush*)							                    = 0;
	
    //************************************
    // Method:    FillSolidRect
    // FullName:  wtlui::ICanvas::FillSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: const RECT &              要填充的矩形区域
    // Parameter: int                       画刷颜色
    // Desc:      用单色画刷填充画布上的矩形区域
    //************************************
    virtual int			FillSolidRect(__in const RECT&, __in int)						                    = 0;
	
    //************************************
    // Method:    FillSolidRect
    // FullName:  wtlui::ICanvas::FillSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       要填充的矩形左边界坐标
    // Parameter: int                       要填充的矩形上边界坐标
    // Parameter: int                       要填充的矩形右边界坐标
    // Parameter: int                       要填充的矩形下边界坐标
    // Parameter: int                       画刷颜色
    // Desc:      用单色画刷填充画布上的矩形区域
    //************************************
    virtual int			FillSolidRect(__in int, __in int, __in int, __in int, __in int)                     = 0;
	
    //************************************
    // Method:    FillSolidRgn
    // FullName:  wtlui::ICanvas::FillSolidRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: IRegion *                 要填充的不规则区域
    // Parameter: int                       画刷颜色
    // Desc:      用单色画刷填充画布上的不规则区域
    //************************************
    virtual int			FillSolidRgn(__in IRegion*, __in int)							                    = 0;
	
    //************************************
    // Method:    FrameRect
    // FullName:  wtlui::ICanvas::FrameRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: const RECT &              绘制的矩形边界
    // Parameter: IBrush *                  使用的画刷
    // Desc:      用指定画刷绘制画布上的矩形边界
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
    // Desc:      用单色画刷绘制画布上的矩形边界
    //************************************
    virtual int			FrameSolidRect(__in const RECT&, __in int)					                        = 0;
	
    //************************************
    // Method:    FrameSolidRect
    // FullName:  wtlui::ICanvas::FrameSolidRect
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       要绘制的矩形左边界坐标left
    // Parameter: int                       要绘制的矩形上边界坐标top
    // Parameter: int                       要绘制的矩形右边界坐标right
    // Parameter: int                       要绘制的矩形下边界坐标bottom
    // Parameter: int                       画刷颜色
    // Desc:      用单色画刷绘制画布上的矩形边界
    //************************************
    virtual int			FrameSolidRect(__in int, __in int, __in int, __in int, __in int)			        = 0;
	
    //************************************
    // Method:    FrameSolidRgn
    // FullName:  xgdi::ICanvas::FrameSolidRgn
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in IRegion *            绘制的目标区域
    // Parameter: __in int                  画刷颜色
    // Parameter: __in int                  水平方向边界宽度
    // Parameter: __in int                  垂直方向边界宽度
    // Desc:      用单色画刷绘制画布上的矩形边界
    //************************************
    virtual int         FrameSolidRgn(__in IRegion*, __in int, __in int, __in int)                          = 0;


	virtual int			Draw3DRect(RECT rcBounds, COLORREF clr1, COLORREF clr2)								= 0;

	//画渐变色，从左到右，或者从上到下
	virtual int			DrawGradient(RECT rcRect, COLORREF rgbTopLeft, COLORREF rgbBottomRight, DWORD nMode)= 0;

	virtual	BOOL		Clear(COLORREF clr)																	= 0;

    //************************************
    // Method:    GetSize
    // FullName:  xgdi::ICanvas::GetSize
    // Access:    virtual public 
    // Returns:   const SIZE&
    // Qualifier: const
    // Desc:      获取画布大小
    //************************************
    virtual const SIZE& GetSize() const                                                                     = 0;

    //************************************
    // Method:    Line
    // FullName:  wtlui::ICanvas::Line
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: IPen *                    使用的画笔（为NULL则使用默认画笔）
    // Parameter: int                       直线起点水平坐标x1
    // Parameter: int                       直线起点垂直坐标y1
    // Parameter: int                       直线终点水平坐标x2
    // Parameter: int                       直线终点垂直坐标y2
    // Desc:      用指定画笔绘制画布上的直线
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
    // Desc:      将画布内容显示在目标设备的指定位置上
    //************************************
    virtual BOOL        Present(__in const UI_HANDLE&, __in const RECT &, __in DWORD dwRop)                 = 0;

    //************************************
    // Method:    SetBkMode
    // FullName:  wtlui::ICanvas::SetBkMode
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: int                       0-正常，1-透明
    // Desc:      设置画布背景模式
    //************************************
    virtual int			SetBkMode(__in int)										                            = 0;

    //************************************
    // Method:    SetFont
    // FullName:  xgdi::ICanvas::SetFont
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IFont *
    // Desc:      设置默认字体
    //************************************
    virtual BOOL        SetFont(__in IFont*)                                                                = 0;

    //************************************
    // Method:    SetPen
    // FullName:  xgdi::ICanvas::SetPen
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in IPen *
    // Desc:      设置默认画笔
    //************************************
    virtual BOOL		SetPen(__in IPen*)										                            = 0;

    //************************************
    // Method:    SetSize
    // FullName:  xgdi::ICanvas::SetSize
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in const SIZE &
    // Desc:      设置画布大小
    //************************************
    virtual void        SetSize(__in const SIZE&)                                                           = 0;

    //************************************
    // Method:    SetTextColor
    // FullName:  xgdi::ICanvas::SetTextColor
    // Access:    virtual public 
    // Returns:   int
    // Qualifier:
    // Parameter: __in int
    // Desc:      设置文本颜色
    //************************************
   virtual int         SetTextColor(__in int)                                                              = 0;


   //创建一个裁剪区域，区域内的被剪掉，rcArray为所要围成的区域
	virtual void CreateClip(RECT* rcArray, int size)												= 0;
	//创建一个裁减区域，区域外的被剪掉，rcArray为所要围成的区域
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
    // Parameter: LPCTSTR                   字体名称（facename）
    // Parameter: const SIZE &              字体大小
    // Parameter: BOOL                      是否为粗体
    // Parameter: BOOL                      是否为斜体
    // Parameter: BOOL                      是否含有下划线
    // Parameter: LPCTSTR                   要装载的字体文件
    // Desc:      创建简单字体
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
    // Desc:      获取Image帧数
    //************************************
    virtual UINT        GetFrameCount() const                                                       = 0;

    //************************************
    // Method:    GetFrameElapse
    // FullName:  xgdi::IImage::GetFrameElapse
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier: const
    // Desc:      获取当前帧至下一帧的延迟
    //************************************
    virtual UINT        GetFrameElapse() const                                                      = 0;

	//************************************
	// Method:    GetSize
	// FullName:  wtlui::IImage::GetSize
	// Access:    virtual public 
	// Returns:   const SIZE&
	// Qualifier:
    // Desc:      获取图像实际大小
	//************************************
	virtual const SIZE&	GetSize() const                                                             = 0;

    //************************************
    // Method:    LoadImage
    // FullName:  wtlui::IImage::LoadImage
    // Access:    virtual public 
    // Returns:   IImage*
    // Qualifier:
    // Parameter: __in LPCTSTR              加载的图像文件名
    // Parameter: __in_opt const SIZE&      指定图像大小
    // Desc:      使用图像文件生成图像
    //************************************
    virtual IImage*     LoadImage(__in LPCTSTR, __in_opt const SIZE&)                               = 0;
	//  [6/17/2010 魏永赳]
	virtual IImage*		LoadImage(__in void* buff, __in int len, __in_opt const SIZE&, LPCTSTR pName )				= 0;
	virtual IImage*		LoadImage(__in void* buff, __in int len, __in_opt const SIZE&, COLORREF& cf )				= 0;
    //************************************
    // Method:    LoadImage
    // FullName:  wtlui::IImage::LoadImage
    // Access:    virtual public 
    // Returns:   IImage*
    // Qualifier:
    // Parameter: __in LPCTSTR              加载的图像文件名
    // Parameter: __in const SIZE&          指定图像大小
    // Desc:      使用UI对象生成Image
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
    // Desc:      定位显示帧后移
    //************************************
    virtual void        NextFrame(__in UINT)                                                        = 0;

    //************************************
    // Method:    PrevFrame
    // FullName:  xgdi::IImage::PrevFrame
    // Access:    virtual public 
    // Returns:   void
    // Qualifier:
    // Parameter: __in UINT
    // Desc:      定位显示帧前移
    //************************************
    virtual void        PrevFrame(__in UINT)                                                        = 0;

	//************************************
	//  [12/16/2010 魏永赳]
	// Method:    AdjustHue
	// FullName:  xgdi::IImage::AdjustHue
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in INT 参数范围：-180~180（度）,=0不作调整 
	// Desc:      改变色调
	//************************************
	virtual void        AdjustHue(__in INT)                                                        = 0;

	//计算图片的拉伸色值
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
    // Desc:      获取Icon所在图像
    //************************************
    virtual IImage*             GetImage()                                                          = 0;

    //************************************
    // Method:    GetRect
    // FullName:  xgdi::IIcon::GetRect
    // Access:    virtual public 
    // Returns:   const RECT&
    // Qualifier:
    // Desc:      获取Icon所在图像上的位置
    //************************************
    virtual const RECT&         GetRect()                                                           = 0;

    //************************************
    // Method:    GetColorKey
    // FullName:  xgdi::IIcon::GetColorKey
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Desc:      获取ColorKey
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
    // Desc:      返回IconSet中的一个图标
    //************************************
    virtual IIcon*              GetIcon(__in int)                                                   = 0;

    //************************************
    // Method:    GetIconCount
    // FullName:  xgdi::IIconSet::GetIconCount
    // Access:    virtual public 
    // Returns:   UINT
    // Qualifier: const
    // Desc:      返回IconSet中的Icon个数
    //************************************
    virtual UINT                GetIconCount() const                                                = 0;

    //************************************
    // Method:    GetIconSize
    // FullName:  xgdi::IIconSet::GetIconSize
    // Access:    virtual public 
    // Returns:   const SIZE&
    // Qualifier:
    // Desc：      返回IconSet的图标大小
    //************************************
    virtual const SIZE&         GetIconSize()                                                       = 0;

    //************************************
    // Method:    LoadIcon
    // FullName:  xgdi::IIconSet::LoadIcon
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in LPCTSTR              图标文件名称
    // Parameter: __in const SIZE &         图标大小
    // Desc:      装载图标文件
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
    // Desc:      装载从Image对象创建图标
    //************************************
    virtual BOOL                LoadIcon(__in IImage*, __in const SIZE&)                            = 0;

	//************************************
	//  [12/16/2010 魏永赳]
	// Method:    AdjustHue
	// FullName:  xgdi::IImage::AdjustHue
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: __in INT 参数范围：-180~180（度）,=0不作调整 
	// Desc:      改变色调
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
	// Parameter: __in int                  画笔样式
	// Parameter: __in int                  画笔宽度
	// Parameter: __in int                  画笔颜色
    // Desc:      创建指定画笔
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
	// Parameter: __in const RECT &         矩形区域
    // Desc:      使用矩形创建不规则区域
	//************************************
	virtual IRegion*	CreateRectRgn(__in const RECT&)							                        = 0;
	
    //************************************
    // Method:    CreateRectRgn
    // FullName:  wtlui::IRegion::CreateRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in int                  矩形区域左边界坐标
    // Parameter: __in int                  矩形区域上边界坐标
    // Parameter: __in int                  矩形区域右边界坐标
    // Parameter: __in int                  矩形区域下边界坐标
    // Desc:      使用矩形创建不规则区域
    //************************************
    virtual IRegion*	CreateRectRgn(__in int, __in int, __in int, __in int)                           = 0;

    //************************************
    // Method:    CreateRoundRectRgn
    // FullName:  xgdi::IRegion::CreateRoundRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in const RECT &         基础矩形
    // Parameter: __in int                  水平方向圆角直径
    // Parameter: __in int                  垂直方向圆角直径
    // Desc:      创建圆角矩形区域
    //************************************
    virtual IRegion*    CreateRoundRectRgn(__in const RECT&, __in int, __in int)                        = 0;

    //************************************
    // Method:    CreateRoundRectRgn
    // FullName:  xgdi::IRegion::CreateRoundRectRgn
    // Access:    virtual public 
    // Returns:   IRegion*
    // Qualifier:
    // Parameter: __in int                  矩形区域左边界坐标
    // Parameter: __in int                  矩形区域上边界坐标
    // Parameter: __in int                  矩形区域右边界坐标
    // Parameter: __in int                  矩形区域下边界坐标
    // Parameter: __in int                  水平方向圆角直径
    // Parameter: __in int                  垂直方向圆角直径
    // Desc:      创建圆角矩形区域
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
// Desc:      创建UIObject实例
//************************************
XCTRLAPI IUIObject*   CreateGTUIObj(UI_OBJECT_IID iid);

//************************************
// Method:    InitGTUI
// FullName:  xgdi::CoCreateUIObject
// Access:    public 
// Returns:   extern "C" bool
// Qualifier:
// Parameter: void
// Desc:      模块初始化
//************************************
XCTRLAPI   bool InitGTUI();
XCTRLAPI void UnInitGTUI();

NAMESPACE_END(xgdi)