#pragma once
#include "xgdi.h"
#include "xgdidef.h"
#include "macros.h"

NAMESPACE_BEGIN(xgdi)
enum StretchMode
{
	STRETCHMODE_NULL = 0,		//无
	STRETCHMODE_TILE,			//平铺	
	STRETCHMODE_STRETCH,		//拉伸
	STRETCHMODE_MID				//居中
};

#define StretchMask_LT	(1<<0)
#define StretchMask_MT	(1<<1)
#define StretchMask_RT	(1<<2)
#define StretchMask_LM	(1<<3)
#define StretchMask_MM	(1<<4)
#define StretchMask_RM	(1<<5)
#define StretchMask_LB	(1<<6)
#define StretchMask_MB	(1<<7)
#define StretchMask_RB	(1<<8)
#define StretchMask_ALL 0xFFFFFF

enum StretchPart
{
	StretchPart_LT	= 0,	//left-top
	StretchPart_MT,			//middle - top
	StretchPart_RT,			//right-top

	StretchPart_LM,			//left-middle
	StretchPart_MM,			//middle - middle
	StretchPart_RM,			//right-middle

	StretchPart_LB,			//left-bottom
	StretchPart_MB,			//middle - bottom
	StretchPart_RB,			//right-bottom
};
struct  StretchRgn
{
	StretchRgn() 
	{
		rgnLT = rgnRT = rgnLB = rgnRB = NULL;
	}
	virtual ~StretchRgn()
	{		
	}
	BOOL IsEmpty()
	{
		 return !rgnLT && !rgnRT && !rgnLB && !rgnRB;
	}
	void DestroyRgns()
	{
		SAFE_DELETEGDIOBJECT(rgnLT);
		SAFE_DELETEGDIOBJECT(rgnRT);
		SAFE_DELETEGDIOBJECT(rgnLB);
		SAFE_DELETEGDIOBJECT(rgnRB);
	}
	HRGN rgnLT;
	HRGN rgnRT;
	HRGN rgnLB;
	HRGN rgnRB;
};

struct StretchImg
{
public:
	StretchImg()
	{
		style = STRETCHMODE_STRETCH;
	};
	virtual ~StretchImg(){};
public:
	bool IsEmpty()
	{
		if(rcInner.IsEmpty() || rcOuter.IsEmpty())
			return true;
		return false;
	};

	virtual StretchImg& operator=(const StretchImg& data)
	{
		rcInner = data.rcInner;
		rcOuter = data.rcOuter;
		style   = data.style;
		rcFix	= data.rcFix;
		return *this;
	};

	geometry::Rect GetScaledInner(const geometry::Rect& rcBounds)
	{
		geometry::Rect rcScaled =	rcBounds;
		rcScaled.left			+=	GetPart(StretchPart_LT).w + rcFix.left;
		rcScaled.top			+=	GetPart(StretchPart_MT).h + rcFix.top;
		rcScaled.w				-=  GetPart(StretchPart_LT).w + GetPart(StretchPart_RT).w + rcFix.w;
		rcScaled.h				-=  GetPart(StretchPart_MT).h + GetPart(StretchPart_MB).h + rcFix.h;
		return rcScaled;
	};

	geometry::Rect  GetPart(xgdi::StretchPart part)
	{
		switch(part)
		{
			//left-top
		case StretchPart_LT:			
			{
				return geometry::Rect(rcOuter.left, rcOuter.top, (rcInner.left - rcOuter.left), (rcInner.top - rcOuter.top));
				break;
			}
			
			//middle - top
		case StretchPart_MT:			
			{
				return geometry::Rect(rcInner.left, rcOuter.top, rcInner.w, rcInner.top - rcOuter.top);
				break;
			}
			
			//right-top
		case StretchPart_RT:			
			{
				return geometry::Rect(rcInner.Right(), rcOuter.top, rcOuter.Right() - rcInner.Right(), rcInner.top - rcOuter.top);
				break;
			}

			//left-middle
		case StretchPart_LM:			
			{
				return geometry::Rect(rcOuter.left, rcInner.top, rcInner.left - rcOuter.left, rcInner.h);
				break;
			}
			
			//middle - middle
		case StretchPart_MM:			
			{
				return rcInner;
				break;
			}
			
			//right-middle
		case StretchPart_RM:			
			{
				return geometry::Rect(rcInner.Right(), rcInner.top, rcOuter.Right() - rcInner.Right(), rcInner.h);
				break;
			}

			//left-bottom
		case StretchPart_LB:			
			{
				return geometry::Rect(rcOuter.left, rcInner.Bottom(), rcInner.left - rcOuter.left, rcOuter.Bottom() - rcInner.Bottom());
				break;
			}
			
			//middle - bottom
		case StretchPart_MB:			
			{
				return geometry::Rect(rcInner.left, rcInner.Bottom(), rcInner.w, rcOuter.Bottom() - rcInner.Bottom());
				break;
			}
			
			//right-bottom	
		case StretchPart_RB:					
			{
				return geometry::Rect(rcInner.Right(), rcInner.Bottom(), rcOuter.Right() - rcInner.Right(), rcOuter.Bottom() - rcInner.Bottom());
				break;
			}
		default:
			break;
		}

		return geometry::Rect(0,0,0,0);
	};

public:
	geometry::Rect	rcInner;
	geometry::Rect	rcOuter;	
	geometry::Rect  rcFix;
	StretchMode		style;
};

NAMESPACE_END(xgdi)