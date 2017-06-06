// KeyBoard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "KeyBoard.h"
#include "DialogKeyBoard.h"
#include "DialogKeyPad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardApp

BEGIN_MESSAGE_MAP(CKeyBoardApp, CWinApp)
	//{{AFX_MSG_MAP(CKeyBoardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyBoardApp construction

CKeyBoardApp::CKeyBoardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKeyBoardApp object

CKeyBoardApp theApp;


extern "C" __declspec(dllexport)void KeyBoard(CString *strKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CDialogKeyBoard dlgKey;
	

	if (dlgKey.DoModal() == IDOK)
	{
		*strKey = dlgKey.m_strKeyData;
	}
}

extern "C" __declspec(dllexport)void KeyPadI(int nMin, int nMax, int *nKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CDialogKeyPad dlgKey;
	
	dlgKey.m_nMode = 0;
	dlgKey.m_strMin.Format(_T("%d"), nMin);
	dlgKey.m_strMax.Format(_T("%d"), nMax);
	dlgKey.m_strCurrent.Format(_T("%d"), *nKey);
	
	if (dlgKey.DoModal() == IDOK)
	{
		*nKey = atoi(dlgKey.m_strKeyData);
	}
}

extern "C" __declspec(dllexport)void KeyPadD(double dMin, double dMax, double *dKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CDialogKeyPad dlgKey;
	
	dlgKey.m_nMode = 1;
	dlgKey.m_strMin.Format(_T("%.3f"), dMin);
	dlgKey.m_strMax.Format(_T("%.3f"), dMax);
	dlgKey.m_strCurrent.Format(_T("%.3f"), *dKey);
	
	if (dlgKey.DoModal() == IDOK)
	{
		*dKey = atof(dlgKey.m_strKeyData);
	}
}