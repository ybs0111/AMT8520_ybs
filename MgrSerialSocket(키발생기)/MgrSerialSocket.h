// MgrSerialSocket.h : main header file for the MGRSERIALSOCKET application
//

#if !defined(AFX_MGRSERIALSOCKET_H__87E41E4E_98F5_4BD5_907F_3B020333BDC3__INCLUDED_)
#define AFX_MGRSERIALSOCKET_H__87E41E4E_98F5_4BD5_907F_3B020333BDC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include <string>

#define REG_KEY_SER	"Software\\M2Soft\\SERI\\"
#define REG_VALUE_CREATE_SER	"SER"
#define REG_VALUE_CREATE_COUNT	"COUNT"
#define REG_VALUE_CREATE_RCOUNT	"RCOUNT"

/////////////////////////////////////////////////////////////////////////////
// CMgrSerialSocketApp:
// See MgrSerialSocket.cpp for the implementation of this class
//

class CMgrSerialSocketApp : public CWinApp
{
public:
	CMgrSerialSocketApp();
	BOOL CheckHardDisk();
	static std::string RegReadString( LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault );
	CString GetLocalMacAddress();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMgrSerialSocketApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMgrSerialSocketApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGRSERIALSOCKET_H__87E41E4E_98F5_4BD5_907F_3B020333BDC3__INCLUDED_)
