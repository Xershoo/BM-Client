#pragma once

#include "xbase.h"
#include "LayoutDef.h"

NAMESPACE_BEGIN(xlayout)

// 类型
enum OBJ_TYPE
{
    TYPE_UNKNOWN = -1,
    TYPE_ALL = 0,
    TYPE_GROUP,
    TYPE_BOX_H,
    TYPE_BOX_V,
    TYPE_SPLITTER_H,
    TYPE_SPLITTER_V,
    TYPE_SPACER,
    TYPE_OBJ
};

// 方向
enum OBJ_DIR
{
    DIR_NULL = 0,
    DIR_LEFT,
    DIR_TOP,
    DIR_RIGHT,
    DIR_BOTTOM,
    DIR_HORZ,
    DIR_VERT
};

// layout object
struct ILayoutObj
{
    virtual int         GetID()                             = 0;
    virtual LPCTSTR     GetName()                           = 0;
    virtual OBJ_TYPE    GetType()                           = 0;
    virtual LPCTSTR     GetProperty( LPCTSTR )              = 0;
	virtual void		Set_Property( LPCTSTR, LPCTSTR)		= 0;//  [10/19/2010 魏永赳]
    virtual LPCRECT     GetBounds()                         = 0;
	virtual void		SetBounds(LPCRECT rc)				= 0;
	virtual void        ChangeBounds(LPCRECT rc)			= 0;
    virtual void        SetData( DWORD_PTR dwData )         = 0;
    virtual DWORD_PTR   GetData()                           = 0;
    virtual bool        IsHidden()                          = 0;
	virtual void	    SetPopup(bool bPopup)				= 0;
	virtual bool		IsPopup()							= 0;
    virtual bool        Show( bool bShow, bool bRelayout )  = 0;
    virtual int         PreSeek( OBJ_DIR dir, int nOffset ) = 0;
    virtual void        Seek( OBJ_DIR dir, int nOffset )    = 0;
    virtual ILayoutObj* GetParent()                         = 0;
};

// layout visitor
struct ILayoutVisitor
{
    virtual void        dumpBegin()								= 0;
    virtual void        dump( ILayoutObj* pObj, int nDepth )	= 0;
    virtual void        dumpEnd()								= 0;
};

// 回调函数
typedef void ( __stdcall* LAYOUTCALLBACK )( int, ILayoutObj*, DWORD_PTR );

struct ILayout
{
    // 释放
    virtual void        Release()                                               = 0;

    // 初始化/反初始化
    virtual bool        Init( LPCTSTR filename )                                = 0;
	virtual bool        InitFromBuffer( LPCTSTR fileContent )					= 0;
    virtual void        Uninit()                                                = 0;

    // 设置回调
    virtual void        SetCallBack( LAYOUTCALLBACK lpfn, DWORD_PTR dwData )    = 0;

    // 根据id得到对象
    virtual ILayoutObj* GetLayoutObj( int id )                                  = 0;

    // 根据名字得到对象
    virtual ILayoutObj* GetLayoutObj( LPCTSTR name )                            = 0;

    // 根据坐标得到对象
    virtual ILayoutObj* GetLayoutObj( POINT pt )                                = 0;

    // 重新布局
    virtual bool        ReLayout( LPCRECT lpRect )                              = 0;

    // 遍历布局元素
    virtual void        Visit( ILayoutVisitor* pVisitor )                       = 0;
};

// 导出函数
LAYOUT_EXT_API ILayout* CreateAKXML();

NAMESPACE_END(xlayout)
