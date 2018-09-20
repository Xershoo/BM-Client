//����һϵ�о��
#pragma once


#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)


#include <assert.h>
#include <string>
#include "varname.h"


#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif

#ifndef LOBYTE
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#endif

#ifndef HIBYTE
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#endif

#ifndef RGB

#define RGB(r,g,b)          ((DWORD)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))

#endif




//ȡ�������
#define GETMAXINVAL(i)			{if(i > (int)i) i++;}

#define PTR_NULL(p)				{assert(p);if(NULL == p) return NULL;}

#define PTR_VOID(p)				{assert(p);if(NULL == p) return ;}

#define PTR_SFALSE(p)			{assert(p);if(NULL == p) return E_FAIL;}

#define PTR_UNKNOWN(p)			{assert(p);if(NULL == p) return HRESULT_ERR_UNKNOWN;}

#define PTR_ARG(p)				{assert(p);if(NULL == p) return HRESULT_ERR_ARG;}

//���һ���ַ���ָ���Ƿ���Ч���������Ч����return
#define STR_VOID(p)				{assert(p);if(NULL == p || 0 == _tcslen(p)) return;}

//���һ���ַ���ָ���Ƿ���Ч���������Ч����return FALSE
#define STR_FALSE(p)			{assert(p);if(NULL == p || 0 == _tcslen(p)) return FALSE;}
#define STR_FALSE_BLTIN(p)		{assert(p);if(NULL == p || 0 == _tcslen(p)) return false;}

//���һ��ָ���Ƿ�ΪNULL�����ΪNULL����return FALSE
#define PTR_FALSE(p)			{assert(p);if(NULL == p) return FALSE;}
#define PTR_FALSE_BLTIN(p)		{assert(p);if(NULL == p) return false;}
//ȡ�������
#define GETMAXINVAL(i)			{if(i > (int)i) i++;}

//��ȫɾ��ָ��
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{ if(NULL != p) { delete (p);     (p)=NULL; } }
#endif

#define SAFEDESTROY(f) if(f) f->Destroy(); f = NULL

//��ȫɾ������
#define SAFE_DELETE_ARRAY(p)	{ if(NULL != p) { delete[] (p);   (p)=NULL; } }

//��ȫ�ͷŶ���
#define SAFE_RELEASE(p)			{ if(NULL != p) { (p)->Release(); (p)=NULL; } }
#define safe_release(p)			{ if(NULL != p) { (p)->release(); (p)=NULL; } }

//��ȫ�Ƿ��ڴ�
#ifndef SAFE_FREE
#define SAFE_FREE(p)			{ if(NULL != p) { free(p);     (p)=NULL; } }
#endif

//���ָ��
#define	SAFE_CHECKPOINTER(p)	if(NULL != p)

//��ȫ�ݻٶ���
#define SAFE_DESTROYOBJECT(p)	if(NULL != p) { (p)->Destroy(); (p)=NULL; }
#define SAFE_DESTROYOBJECT2(p)	if(NULL != p) { (p)->Destroy(); delete (p); (p)=NULL; }

//��ȫ�ݻ�GDI����
#define SAFE_DELETEGDIOBJECT(p) if(NULL != p) {::DeleteObject(p); (p)=NULL;}
#define SAFE_DELETEGDIOBJECT2(p) if(NULL != p) {(p)->DeleteObject(); delete (p); (p)=NULL;}

#define	COMPAREPOINTER(p, q)			if(p == q)

#define NAMESPACE_BEGIN(n)		namespace n {
#define NAMESPACE_END(n)		}	

//�ж�b���Ƿ����a
#define include(a, b)			((a & b) == b)

#define AFFIRM(f,r) \
	do{ \
	if(!(f)) {assert(false); return r;} \
	}while(0) \

#define AFFIRM_VOID(f) \
	do{ \
	if(!(f)) {assert(false); return ;} \
	}while(0) \

#ifndef VERIFY
	#ifdef _DEBUG
		#define VERIFY(f) \
		do{ \
			if(!(f)) {assert(false); } \
		}while(0) 
	#else
		#define VERIFY(f) (f);
	#endif
#endif

#define AFFIRM_BOOL(f) \
	do{ \
	if(!(f)) {assert(FALSE); return FALSE;} \
	}while(0) \

#define AFFIRM_INT(f,r) \
	do{ \
	if(!(f)) {assert(FALSE); return r;} \
	}while(0) \

#define AFFIRM_ANY(f,r) \
	do{ \
	if(!(f)) {assert(FALSE); return r;} \
	}while(0) \


#define CHECK_VOID(f) \
			if(!(f)) {return ;} \

#define CHECK_BOOL(f) \
	if(!(f)) {return FALSE;} \

#define CHECK_ANY(f,r) \
	if(!(f)) {return r;} \


#define		SEF_MK_CTRL		0x0002
#define		SEF_MK_ALT		0x0004
#define		SEF_MK_SHIFT	0x0001

template<class T>
class TDestructor
{
public:
	TDestructor()
	{
	}
	~TDestructor()
	{
		T::Release();
	}
};

#ifdef   USE_EXCEPTION
	#define __TRY	try{
	#define __CATCH }catch(...){
	#define __END	}
#else
	#define __TRY 
	#define __CATCH if(0){
	#define __END	}
#endif

template <typename T>
class AUTO_RELEASE_PTR
{
public:
	AUTO_RELEASE_PTR(T *obj) { xobj_ = obj; }
	~AUTO_RELEASE_PTR()	{ SAFE_RELEASE(xobj_); }

	T* operator->() const{ 	return xobj_; }
	operator T*() const { return xobj_; }
private:
	T *xobj_;
};

class Autoref //һ�������ṹ������һ��refֵ
{
public:
	Autoref()
	{
		m_cRef = 0;
	};
	
public:
	int add(void)
	{
		++m_cRef;
		return m_cRef;
	}
	
	int dec(void)
	{
		assert(m_cRef);
		if(m_cRef > 0)
			--m_cRef;
		return m_cRef;
	}
	
	bool zero(void)
	{
		if(m_cRef == 0)
			return true;
		return false;
	}
protected:
	long m_cRef;
};

#ifndef DEBUG_NEW
#define DEBUG_NEW new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif

#define DEFAULT_COLORKEY RGB(255,0,255)

#define binString	std::string

#ifndef ASSERT
#define ASSERT(f) ATLASSERT(f)
#endif

#define FOR_CONTAINER_LOOP(containertype, container, it)  for(containertype::iterator it = container.begin(); it != container.end(); ++it) 