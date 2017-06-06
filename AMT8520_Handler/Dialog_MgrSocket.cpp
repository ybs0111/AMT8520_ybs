// Dialog_MgrSocket.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_MgrSocket.h"
#include "ManagerSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_MgrSocket dialog


CDialog_MgrSocket::CDialog_MgrSocket(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_MgrSocket::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_MgrSocket)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDialog_MgrSocket::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_MgrSocket)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_MgrSocket, CDialog)
	//{{AFX_MSG_MAP(CDialog_MgrSocket)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_MgrSocket message handlers

BOOL CDialog_MgrSocket::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ManagerOpen(20, 2, 10);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_MgrSocket::OnPaint() 
{
 	CPaintDC dc(this); // device context for painting	
}

void CDialog_MgrSocket::OnButtonView() 
{
	// TODO: Add your control notification handler code here
	ManagerDisplay(CPoint(10, 10), 0);
}

void CDialog_MgrSocket::OnOK() 
{
	// TODO: Add extra validation here
	
	ManagerClose();
	CDialog::OnOK();
}

BOOL CDialog_MgrSocket::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ManagerClose();
	
	return CDialog::DestroyWindow();
}
