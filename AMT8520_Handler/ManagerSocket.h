// ManagerSocket.h : main header file for the MANAGERSOCKET DLL
//

#if !defined(AFX_MANAGERSOCKET_H__F21B6176_C836_4C54_8F16_B7303DBEB33A__INCLUDED_)
#define AFX_MANAGERSOCKET_H__F21B6176_C836_4C54_8F16_B7303DBEB33A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CManagerSocketApp
// See ManagerSocket.cpp for the implementation of this class
//

class CManagerSocketApp : public CWinApp
{
public:
	CManagerSocketApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManagerSocketApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CManagerSocketApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// DLL Open
extern "C" __declspec(dllexport)void		ManagerOpen(int nSlot, int nRow, int nCol);
extern "C" __declspec(dllexport)void		ManagerClose();
extern "C" __declspec(dllexport)void		ManagerDisplay(CPoint cpXY, int nHifix);
extern "C" __declspec(dllexport)void		ManagerWriteData(int nSlot, int nHifix, int nCount);
extern "C" __declspec(dllexport)void		ManagerReadData(int nSlot, int nHifix, CString *strSerial, int *nCount);
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGERSOCKET_H__F21B6176_C836_4C54_8F16_B7303DBEB33A__INCLUDED_)
