#pragma once

#include "xbase.h"

#include <windows.h>

NAMESPACE_BEGIN(xgdi)

const COLORREF DEF_TRANS_COLOR = RGB(255,0,255);

enum DRAW_IMAGE_FLAG
{
    DIF_NORMAL						= 1<<1, // 居中无缩放
    DIF_ALIGNLEFT					= 1<<2,
    DIF_ALIGNTOP					= 1<<3,
    DIF_ALIGNRIGHT					= 1<<4,
    DIF_ALIGNBOTTOM					= 1<<5,
    DIF_TILE						= 1<<6,
    DIF_STRETCH						= 1<<7,
    DIF_TRANSPARENT					= 1<<8,
	DIF_STRE_NO_CENTER				= 1<<9,
	DIF_GRAY						= 1<<10,
	DIF_ALPHA						= 1<<11,
	DIF_CENTER_ALL_SHOW				= 1<<12, //全部显示并居中，当区域大于图像，不拉伸，如果小于图像，图像宽高等比压小
    DIF_MAX
};

enum FRAME_IN_IMAGE
{
    FII_NORMAL                  = 0,
    FII_SELECTED                ,
    FII_HIGHLIGHTED             ,
    FII_DISABLED                ,

    FII_MAX
};

// pre-defined const object
const RECT rectOriginalImage = { 0, 0, -1, -1 };

NAMESPACE_END(xgdi)




namespace geometry
{
class Rect
{
public:
	Rect() : left(0), top(0), w(0),h(0) { };
	Rect(int x, int y, int width, int height) : left(x),top(y),w(width),h(height) { };
	Rect(const RECT& rect) : left(rect.left),top(rect.top),w(rect.right - rect.left),h(rect.bottom - rect.top){ };

public:
	operator RECT() const
	{
		RECT	rc;
		rc.left  =left;
		rc.top   =top;
		rc.right =left + w;
		rc.bottom=top + h;

		return rc;
	};

	bool operator==(Rect rc) const
	{
		return (rc.top == top && rc.left == left &&
			rc.w == w && rc.h == h);
	};
public:
	int Top() const 
	{
		return top;
	}
	int Left() const
	{
		return left;
	}
	int	Right(void) const
	{
		return left + w;
	};

	int Bottom(void) const
	{
		return top + h;
	};
public:
	int Left(int nLeft)
	{
		left = nLeft;
		return Left();
	}
	int Top(int nTop)
	{
		top = nTop;
		return Top();
	}
	int Right(int nRight)
	{
		w = nRight - left;

		return Right();
	};

	int Bottom(int nBottom)
	{
		h = nBottom - top;

		return Bottom();
	};
public:

	bool IsEmpty(void) const
	{
		return (w == 0 || h == 0);
	};

	void Offset(int x, int y)
	{
		left += x;
		top  += y;
	};

	void Contract(int nSize)
	{
		left += nSize;
		top  += nSize;
		w    -= nSize * 2;
		h    -= nSize * 2;
	};

	void Expand(int nSize)
	{
		left -= nSize;
		top  -= nSize;
		w    += nSize * 2;
		h    += nSize * 2;
	};
public:
	int left;
	int top;
	int w;
	int h;
};
}

