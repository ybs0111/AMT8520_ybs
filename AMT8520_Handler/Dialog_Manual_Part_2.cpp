// Dialog_Manual_Part_2.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Manual_Part_2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_2 dialog


CDialog_Manual_Part_2::CDialog_Manual_Part_2(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Manual_Part_2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Manual_Part_2)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Func.mp_manual_2 = NULL;
}


void CDialog_Manual_Part_2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Manual_Part_2)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Manual_Part_2, CDialog)
	//{{AFX_MSG_MAP(CDialog_Manual_Part_2)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_2_EXIT, OnBtn2Exit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_2 message handlers


BOOL CDialog_Manual_Part_2::OnInitDialog() 
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

void CDialog_Manual_Part_2::OnDestroy() 
{
	if(mp_manual_font != NULL)
	{
		delete mp_manual_font;
		mp_manual_font = NULL;
	}
	
	CDialog::OnDestroy();
}

BOOL CDialog_Manual_Part_2::Create() 
{
	return CDialog::Create( CDialog_Manual_Part_2::IDD );
}

void CDialog_Manual_Part_2::PostNcDestroy() 
{
	delete this;
	Func.mp_manual_2 = NULL;
	
	CDialog::PostNcDestroy();
}

void CDialog_Manual_Part_2::OnBtn2Exit() 
{
	DestroyWindow();
}
