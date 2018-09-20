#ifndef __LayoutDef_h__
#define __LayoutDef_h__

#if _MSC_VER > 1000
#	pragma once
#endif

#if 0
#define BEGIN_NAME_SPACE \
	 \
	{ \
	namespace xlayout \
		{

#define END_NAME_SPACE \
		} \
	}
#endif

#ifdef LAYOUT_DLL
#	define LAYOUT_EXT_API __declspec(dllexport)
#else
#	define LAYOUT_EXT_API __declspec(dllimport)
#endif

#endif // __LayoutDef_h__