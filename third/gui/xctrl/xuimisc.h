#pragma once

#include <vector>
#include "macros.h"
#include <xstring.h>
#include <GdiPlus.h>

inline int upper_count(int x, int y)
{
	return x/y + (x%y ? 1 : 0);
}

class CClientRect
    : public CRect
{
public:
    CClientRect(HWND hWnd)
    {
        ::GetClientRect(hWnd, (RECT*)this);
    }
};

class CRelativeRect
    : public CRect
{
public:
    CRelativeRect(HWND hInWnd, HWND hOutWnd)
    {
        ::GetWindowRect(hInWnd, (RECT*)this);
        CPoint pt = TopLeft();
        ::ScreenToClient(hOutWnd, &pt);
        MoveToXY(pt);
    }
};

class CWindowRect
    : public CRect
{
public:
    CWindowRect(HWND hWnd)
    {
        ::GetWindowRect(hWnd, (RECT*)this);
    }
};

class CUpdateRect
    : public CRect
{
public:
    CUpdateRect(HWND hWnd)
    {
        ::GetUpdateRect(hWnd, (RECT*)this, FALSE);
    }
};

class CSubRect
    : public CRect
{
public:
    CSubRect(const RECT& rcBase, int nLeft, int nTop, int nWidth, int nHeight)
    {
        left        = rcBase.left + nLeft;
        top         = rcBase.top + nTop;
        right       = (nWidth > 0) ? (left + nWidth) : (rcBase.right + nWidth);
        bottom      = (nHeight > 0) ? (top + nHeight) : (rcBase.bottom + nHeight);
    }
};

class CLeftTopRect
    : public CRect
{
public:
    CLeftTopRect(const RECT& rcClient, const SIZE& size)
    {
        SetRect(rcClient.left, rcClient.top, rcClient.left + size.cx, rcClient.top + size.cy);
    }
};

class CRightTopRect
    : public CRect
{
public:
    CRightTopRect(const RECT& rcClient, const SIZE& size)
    {
        SetRect(rcClient.right - size.cx, rcClient.top, rcClient.right, rcClient.top + size.cy);
    }
};

class CLeftBottomRect
    : public CRect
{
public:
    CLeftBottomRect(const RECT& rcClient, const SIZE& size)
    {
        SetRect(rcClient.left, rcClient.bottom - size.cy, rcClient.left + size.cx, rcClient.bottom);
    }
};

class CRightBottomRect
    : public CRect
{
public:
    CRightBottomRect(const RECT& rcClient, const SIZE& size)
    {
        SetRect(rcClient.right - size.cx, rcClient.bottom - size.cy, rcClient.right, rcClient.bottom);
    }
};

class CIntString
    : public WTL::CString
{
public:
    CIntString(int nInt)
    {
        Format(_T("%d"), nInt);
    }

    CIntString(LPCTSTR lpszFormat, int nInt)
    {
        Format(lpszFormat, nInt);
    }
};

class CStringInt
{
public:
    CStringInt(LPCTSTR lpszText, int nDefValue = 0)
    {
        m_nInt = lpszText ? _ttoi(lpszText) : nDefValue;
    }

    operator int() const
    {
        return m_nInt;
    }

private:
    int m_nInt;
};

class CCurPoint
    : public CPoint
{
public:
    CCurPoint()
    {
        ::GetCursorPos((LPPOINT)this);
    }
};


#define guid_format			_T("%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X")
#define xml_splitter_char	_T(",")


class CTextUtil
{
public:

inline static int stoi(const xstring& p)
{
		#ifndef _UNICODE
				return atoi(p.c_str());
		#else
				return _wtoi(p.c_str());
		#endif
}

inline static void split(const xstring& pattern,std::vector<xstring>& des,const xstring& src)
{
	int size = pattern.size();
	int off = 0;
	int oldoff = 0; 
	while(1)
	{
		oldoff = off;
		off = src.find(pattern,off);
		if(off < 0)
		{
			break;
		}
		des.push_back(src.substr(oldoff,off - oldoff));
		off += size;
	}
	des.push_back(src.substr(oldoff,src.size() - oldoff));
}


inline static xstring Format(LPCTSTR lpszFormat, ...)
{
		TCHAR buffer[256];

		va_list		ap;
		va_start(ap, lpszFormat);
		int len = _vsntprintf_s(buffer, -1, lpszFormat, ap);
		va_end(ap);
		return xstring(buffer, len);
}

inline static BOOL	TextToInt(LPCTSTR text,int& value)
{
	value = stoi(text);
	return TRUE;
}
inline static BOOL	TextToRect(LPCTSTR text,RECT& value)
{
	std::vector<xstring> svals;
	split(xml_splitter_char,svals,text);

	AFFIRM_BOOL(svals.size() == 4);

	value.left		= stoi(svals[0]);
	value.top		= stoi(svals[1]);
	value.right		= stoi(svals[2]);
	value.bottom	= stoi(svals[3]);
	return TRUE;
}


inline static BOOL	TextToPoint(LPCTSTR text,POINT& value)
{
	std::vector<xstring> svals;
	split(xml_splitter_char,svals,text);
	AFFIRM_BOOL(svals.size() == 2);
	value.x = stoi(svals[0]);
	value.y = stoi(svals[1]);
	return TRUE;
}
inline static BOOL	TextToSize(LPCTSTR text,SIZE& value)
{
	std::vector<xstring> svals;
	split(xml_splitter_char,svals,text);
	AFFIRM_BOOL(svals.size() == 2);
	value.cx = stoi(svals[0]);
	value.cy = stoi(svals[1]);
	return TRUE;
}

inline static BOOL	TextToGuid(LPCTSTR text,GUID& value)
{
	xstring gval = text;

	std::vector<xstring> gvals;
	split(_T("-"),gvals,gval);
	//AFFIRM_BOOL(gvals.size() == 5);
    if(gvals.size() != 5)
    {
        return FALSE;
    }

	value.Data1 = (unsigned long)atohex(gvals[0]);
	value.Data2 = (unsigned short)atohex(gvals[1]);
	value.Data3 = (unsigned short)atohex(gvals[2]);
	unsigned __int64 res = atohex(gvals[3]);
	value.Data4[0] = (unsigned char)(res/(16*16));
	value.Data4[1] = (unsigned char)(res%(16*16));
	res = atohex(gvals[4]);
	for(int i =7; i>=2; i--)
	{
		value.Data4[i] =  (unsigned char)(res%(16*16));
		res = res/(16*16);
	}  
	return TRUE;
}
inline static BOOL	TextToColor(LPCTSTR text,COLORREF& value)
{
	std::vector<xstring> svals;
	split(xml_splitter_char,svals,text);
	AFFIRM_BOOL(svals.size() == 3);
	value = RGB(stoi(svals[0]),stoi(svals[1]),stoi(svals[2]));
	return TRUE;
}

inline static BOOL	TextToGdiPlusColor(LPCTSTR text,DWORD& value)
{
	std::vector<xstring> svals;
	split(xml_splitter_char,svals,text);
	AFFIRM_BOOL(svals.size() == 4);
	Gdiplus::Color color(stoi(svals[0]),stoi(svals[1]),stoi(svals[2]),stoi(svals[3]));
	value = color.GetValue();
	return TRUE;
}



inline static __int64 atohex(const xstring& str) 
{
	__int64 nResult = 0;
	for(size_t i = 0;  i<str.size();  i++)
	{
		int nDecNum = 0;

		TCHAR  w = str.at(i);
		switch(w)
		{
		case _T('A'):
        case _T('a'):	nDecNum = 10; break;
		case _T('B'):
        case _T('b'):	nDecNum = 11; break;
		case _T('C'):
        case _T('c'):	nDecNum = 12;   break;
		case _T('D'):
        case _T('d'):	nDecNum = 13;   break;
        case _T('E'):
        case _T('e'):	nDecNum = 14;   break;
		case _T('F'):
        case _T('f'):	nDecNum = 15;   break;
		case _T('0'):	nDecNum = 0;   break;
		case _T('1'):	nDecNum = 1;   break;
		case _T('2'):	nDecNum = 2;   break;
		case _T('3'):	nDecNum = 3;   break;
		case _T('4'):	nDecNum = 4;   break;
		case _T('5'):	nDecNum = 5;   break;
		case _T('6'):	nDecNum = 6;   break;
		case _T('7'):	nDecNum = 7;   break;
		case _T('8'):	nDecNum = 8;   break;
		case _T('9'):	nDecNum = 9;   break;
		default:
			ASSERT(false);
			return 0;   
		}
		nResult = nResult*16 + nDecNum;
	}
	return nResult;
}
inline static xstring IntToText(int value)
{
	xstring str = Format(_T("%d"),value);
	return str;
}
inline static xstring RectToText(const RECT& value)
{
	xstring str = Format(_T("%d,%d,%d,%d"),value.left,value.top,value.right,value.bottom);
	return str;
}
inline static xstring PointToText(const POINT& value)
{
	xstring str = Format(_T("%d,%d"),value.x,value.y);
	return str;
}
inline static xstring SizeToText(const SIZE& value)
{
	xstring str = Format(_T("%d,%d"),value.cx,value.cy);
	return str;
}
inline static xstring GuidToText(const GUID& value)
{
	xstring str = Format(guid_format,value.Data1, value.Data2, value.Data3, 
		value.Data4[0], value.Data4[1], value.Data4[2], value.Data4[3],
		value.Data4[4], value.Data4[5], value.Data4[6], value.Data4[7]);		
	return str;
}
inline static xstring ColorToText(const COLORREF& value)
{
	xstring str = Format(_T("%d,%d,%d"),GetRValue(value),GetGValue(value),GetBValue(value));
	return str;
}

};



