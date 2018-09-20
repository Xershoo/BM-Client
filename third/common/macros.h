//����һϵ�о��
#pragma once


#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#pragma warning(disable : 4267)


#include <assert.h>
#include <string>
#include <tchar.h>



#if defined(_AFXDLL)||defined(_USRDLL)
#define DLL_EXP __declspec(dllexport)
#elif defined(_MSC_VER)&&(_MSC_VER<1200)
#define DLL_EXP __declspec(dllimport)
#else
#define DLL_EXP
#endif

#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
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
 
#define POSTMSG2PARENT(message,wParam,lParam) \
	if(GetParent() && ::IsWindow(GetParent()->GetSafeHwnd())) \
		GetParent()->PostMessage(message,wParam,lParam) \

#define SENDMSG2PARENT(message,wParam,lParam) \
	if(GetParent() && ::IsWindow(GetParent()->GetSafeHwnd())) \
		GetParent()->SendMessage(message,wParam,lParam) \

//��ȫɾ������
#define SAFE_DELETE_ARRAY(p)	{ if(NULL != p) { delete[] (p);   (p)=NULL; } }

//��ȫ�ͷŶ���
#define SAFE_RELEASE(p)			{ if(NULL != p) { (p)->Release(); (p)=NULL; } }
#define safe_release(p)			{ if(NULL != p) { (p)->release(); (p)=NULL; } }

//��鴰���Ƿ����
#define	SAFE_CHECKWND(p)		if(NULL != p && ::IsWindow(p->m_hWnd)) 

//��鴰���Ƿ�Ϊ�Ƿ�����
#define	SAFE_CHECKFALSEWND(p)	if(NULL == p || !::IsWindow(p->m_hWnd)) 

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

//trace
#define TRACE_OUTPUT(a)			::OutputDebugString(a)


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

const TCHAR CHAR_PATH_SEPARATE = _T('|');

#define DEFAULT_COLORKEY RGB(255,0,255)




/*
#ifndef _AFX
#pragma message(_T("use ATL::String for AWString"))
#include <atlstr.h>
#define AWString ATL::CString
#else
#pragma message(_T("use MFC::String for AWString"))
#include <afx.h>
#include <afxstr.h>
#define AWString CString
#endif
*/
#include <atlstr.h>
#define AWString ATL::CString

#define binString	std::string

#ifndef ASSERT
#define ASSERT(f) ATLASSERT(f)
#endif

#define FOR_CONTAINER_LOOP(containertype, container, it)  for(containertype::iterator it = container.begin(); it != container.end(); ++it) 