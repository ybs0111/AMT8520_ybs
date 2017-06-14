// XgemInterface.h : main header file for the XGEMINTERFACE application
//

#if !defined(AFX_XGEMINTERFACE_H__7DD35272_79B0_4CB7_8370_BA39BCD82D53__INCLUDED_)
#define AFX_XGEMINTERFACE_H__7DD35272_79B0_4CB7_8370_BA39BCD82D53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXgemInterfaceApp:
// See XgemInterface.cpp for the implementation of this class
//

class CXgemInterfaceApp : public CWinApp
{
public:
	CXgemInterfaceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXgemInterfaceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXgemInterfaceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XGEMINTERFACE_H__7DD35272_79B0_4CB7_8370_BA39BCD82D53__INCLUDED_)
