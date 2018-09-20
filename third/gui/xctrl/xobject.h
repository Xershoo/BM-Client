#pragma once
#include "xbase.h"

template<class T>
class XObjPtr
{
public:
    XObjPtr(){
        __obj = NULL;
    }
    ~XObjPtr()
    {
        if(__obj) __obj->Release();
    }
    XObjPtr(T* obj)
    {
        __obj = obj;
    }

    XObjPtr& operator=(T* obj)
    {
        if(__obj) __obj->Release();
        __obj = obj;
        return *this;
    }
    T* operator->()
    {
        return __obj;
    }
    T* operator->() const
    {
        return __obj;
    }
    operator T*()
    {
        return __obj;
    }
    operator T*() const
    {
        return __obj;
    }

	T* Detach()
	{
		T* obj = __obj;
		__obj = NULL;
		return obj;
	}

protected:
    T*	__obj;
};


struct IProperty
{
    //************************************
    // Method:    GetProperty
    // FullName:  IProperty::GetProperty
    // Access:    virtual public 
    // Returns:   LPCTSTR
    // Qualifier: const
    // Parameter: __in LPCTSTR
    // Parameter: __in LPCTSTR
    //************************************
    virtual LPCTSTR     GetProperty(__in LPCTSTR, __in LPCTSTR) const       = 0;

    //************************************
    // Method:    GetProperty
    // FullName:  IProperty::GetProperty
    // Access:    virtual public 
    // Returns:   int
    // Qualifier: const
    // Parameter: __in LPCTSTR
    // Parameter: __in int
    //************************************
    virtual int         GetProperty(__in LPCTSTR, __in int) const           = 0;


    virtual LPCTSTR     GetProperty(__in LPCTSTR)           const           = 0;

    //************************************
    // Method:    SetProperty
    // FullName:  IProperty::SetProperty
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in LPCTSTR
    //************************************
    virtual BOOL        SetProperty(__in LPCTSTR)                           = 0;

    //************************************
    // Method:    SetProperty
    // FullName:  IProperty::SetProperty
    // Access:    virtual public 
    // Returns:   BOOL
    // Qualifier:
    // Parameter: __in int
    //************************************
    virtual BOOL        SetProperty(__in int)                               = 0;
};
