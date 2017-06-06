// KeyBoard.h : main header file for the KEYBOARD DLL
//

#if !defined(AFX_KEYBOARD_H__52DD04BB_7A06_4221_AAFD_AE63C9EA1969__INCLUDED_)
#define AFX_KEYBOARD_H__52DD04BB_7A06_4221_AAFD_AE63C9EA1969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardApp
// See KeyBoard.cpp for the implementation of this class
//

class CKeyBoardApp : public CWinApp
{
public:
	CKeyBoardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyBoardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CKeyBoardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARD_H__52DD04BB_7A06_4221_AAFD_AE63C9EA1969__INCLUDED_)


extern "C" __declspec(dllexport)void			KeyBoard(CString *strKey);
extern "C" __declspec(dllexport)void			KeyPadI(int nMin, int nMax, int *nKey);
extern "C" __declspec(dllexport)void			KeyPadD(double dMin, double dMax, double *dKey);