// Dialog_Manual_Part_4.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Manual_Part_4.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_4 dialog


CDialog_Manual_Part_4::CDialog_Manual_Part_4(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Manual_Part_4::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Manual_Part_4)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Func.mp_manual_4 = NULL;
}


void CDialog_Manual_Part_4::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Manual_Part_4)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Manual_Part_4, CDialog)
	//{{AFX_MSG_MAP(CDialog_Manual_Part_4)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_4_EXIT, OnBtn4Exit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_4 message handlers

BOOL CDialog_Manual_Part_4::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rectArea;
	
	mp_manual_font = NULL;
	mp_manual_font = new CFont;
	mp_manual_font->CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	
	GetClientRect(rectArea);
	MoveWindow(mcp_coord.x, mcp_coord.y, rectArea.Width(), rectArea.Height(), TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Manual_Part_4::OnDestroy() 
{
	if(mp_manual_font != NULL)
	{
		delete mp_manual_font;
		mp_manual_font = NULL;
	}
	

	CDialog::OnDestroy();
}

BOOL CDialog_Manual_Part_4::Create() 
{
	return CDialog::Create( CDialog_Manual_Part_4::IDD );
}

void CDialog_Manual_Part_4::PostNcDestroy() 
{
	delete this;
	Func.mp_manual_4 = NULL;
	
	CDialog::PostNcDestroy();
}

void CDialog_Manual_Part_4::OnBtn4Exit() 
{
	DestroyWindow();
}
