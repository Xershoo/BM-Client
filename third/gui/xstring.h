#ifndef	UNISTR_COMMON_H
#define UNISTR_COMMON_H

#include <stdlib.h>
#include <string>

#ifdef WIN32
#include <TCHAR.h>
#endif

		#ifdef _UNICODE
				#ifdef WIN32
						#define xstring std::wstring
				#else
						#define xstring std::string
				#endif
		#else
				#define xstring std::string
		#endif
#endif

