#pragma once
#include "xbase.h"
#include "xgdi/xgdi.h"
NAMESPACE_BEGIN(xskin)

struct IResLoader
{
	virtual xgdi::IImage*       LoadImage(LPCTSTR texture)			        = 0;
	virtual xgdi::IImage*       LoadImage(LPCTSTR texture, COLORREF& cf)	= 0;
	virtual BOOL		        UnloadImage(LPCTSTR texture)			    = 0;

    virtual xgdi::IFont*        LoadFont(LPCTSTR texture)                   = 0;
    virtual BOOL                UnloadFont(LPCTSTR texture)			        = 0;

    virtual xgdi::IIconSet*     LoadIconSet(LPCTSTR texture)                = 0;
    virtual BOOL                UnloadIconSet(LPCTSTR texture)			    = 0;
	
	virtual BOOL				ReLoad()									= 0;
	// 添加需重新加载标志 [1/5/2011 魏永赳]
	virtual void				SetReLoad()									= 0;
	virtual BOOL				ReLoadTexture(LPCTSTR texture)				= 0;
	//  [12/16/2010 魏永赳]
	virtual BOOL				AdjustHue(INT hue)							= 0;
};

extern "C" xgdi::IUIObject*     QueryUIObject(LPCTSTR lpszIID);

extern "C" DWORD                QueryUIValue(LPCTSTR lpszIID);

// 添加一个调用::LoadImage载入Bitmap的函数 [4/13/2010 何春龙]
// QueryUIObject载入的图像在ToolTip或ListCtrl中背景不透明
// 原因不清楚，增加该函数以解决背景不透明的问题
extern "C" HBITMAP				QueryBitmap(LPCTSTR lpszIID);

extern "C" BOOL					GTReload();

#if 0
#define SKINID(id) MAKEINTRESOURCE(xskin::IID_#id)
#else
#define SKINID(id) TEXT(#id)
#endif

NAMESPACE_END(xskin)