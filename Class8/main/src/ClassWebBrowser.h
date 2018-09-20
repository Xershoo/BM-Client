#ifndef CLASS_WEB_BROWSER_H
#define CLASS_WEB_BROWSER_H

#include "WebBrowser.h"

class IClassCallback
{
public:
	virtual void onEnterClassRoom(__int64 nClassId,__int64 nCourseId) = 0;
	virtual void onPlayClassRoom(__int64 nClassId,__int64 nCourseId) = 0;
	virtual void onDocumentComplete() = 0;
};

class CClassWebBrowser : public WebBrowser
{
public:
	CClassWebBrowser();
	virtual ~CClassWebBrowser();

public:
	virtual HRESULT _stdcall GetIDsOfNames(REFIID riid,OLECHAR FAR* FAR* rgszNames,unsigned int cNames,LCID lcid,DISPID FAR* rgDispId);
	virtual HRESULT _stdcall Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS FAR* pDispParams,VARIANT FAR* pVarResult,EXCEPINFO FAR* pExcepInfo,unsigned int FAR* puArgErr);

public:	
	void	SetCallback(IClassCallback * pCallBack);

protected:
	IClassCallback*	m_pCallback;
};

#endif
