/*
 * wtlbase_if.h
 *
 */
#pragma once

#define NAMESPACE_BEGIN(n)		namespace n {
#define NAMESPACE_END(n)		}

#define implement				public

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __in_opt
#define __in_opt
#endif

#ifndef __inout
#define __inout
#endif


struct IObject
{
	virtual void				Release()						= 0;
};


