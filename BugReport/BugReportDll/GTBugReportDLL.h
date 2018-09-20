// GTBugReportDLL.h : main header file for the GTBUGREPORTDLL DLL
//

#if !defined(AFX_GTBUGREPORTDLL_H__6289F882_8D71_4609_A026_6479CA45CE31__INCLUDED_)
#define AFX_GTBUGREPORTDLL_H__6289F882_8D71_4609_A026_6479CA45CE31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CGTBugReportDLLApp
// See GTBugReportDLL.cpp for the implementation of this class
//

class CGTBugReportDLLApp : public CWinApp
{
public:
	CGTBugReportDLLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGTBugReportDLLApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CGTBugReportDLLApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTBUGREPORTDLL_H__6289F882_8D71_4609_A026_6479CA45CE31__INCLUDED_)
