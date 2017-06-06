// Dialog_AlignPogo.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_AlignPogo.h"
#include "Dialog_Infor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_AlignPogo dialog


CDialog_AlignPogo::CDialog_AlignPogo(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_AlignPogo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_AlignPogo)
	//}}AFX_DATA_INIT
}


void CDialog_AlignPogo::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_AlignPogo)
	DDX_Control(pDX, IDC_RADIO_POGO_MODE, m_radio_pogouse);
	DDX_Control(pDX, IDC_RADIO_JIG_NOTUSE, m_radio_alignnotuse);
	DDX_Control(pDX, IDC_RADIO_JIG_MODE, m_radio_alignuse);
	DDX_Control(pDX, IDC_GROUP_POGO_TYPE, m_group_pogo_type);
	DDX_Control(pDX, IDC_GROUP_JIG_MODE, m_group_alignbuffer);
	DDX_Control(pDX, IDC_RADIO_POGO_NOTUSE, m_radio_pogonotuse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_AlignPogo, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_AlignPogo)
	ON_BN_CLICKED(IDC_RADIO_POGO_MODE, OnRadioPogoMode)
	ON_BN_CLICKED(IDC_RADIO_POGO_NOTUSE, OnRadioPogoNotuse)
	ON_BN_CLICKED(IDC_RADIO_JIG_MODE, OnRadioJigMode)
	ON_BN_CLICKED(IDC_RADIO_JIG_NOTUSE, OnRadioJigNotuse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_AlignPogo message handlers

BOOL CDialog_AlignPogo::OnInitDialog() 
{
	StandardDialog::OnInitDialog();
	
	mn_PogoMode = 0;
	mn_AlignMode = 0;
	OnData_Change();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CDialog_AlignPogo::OnData_Change()
{
	OnPogoMode_Change();
	OnAlignMode_Change();
}

void CDialog_AlignPogo::OnPogoMode_Change()
{
	switch (mn_PogoMode)
	{
	case 0:
		m_radio_pogouse.SetCheck(FALSE);
		m_radio_pogonotuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_pogouse.SetCheck(TRUE);
		m_radio_pogonotuse.SetCheck(FALSE);
		break;
	}
}

void CDialog_AlignPogo::OnAlignMode_Change()
{
	switch (mn_AlignMode)
	{
	case 0:
		m_radio_alignuse.SetCheck(FALSE);
		m_radio_alignnotuse.SetCheck(TRUE);
		break;
		
	case 1:
		m_radio_alignuse.SetCheck(TRUE);
		m_radio_alignnotuse.SetCheck(FALSE);
		break;
	}
}

void CDialog_AlignPogo::OnOK() 
{
	CDialog_Infor info_dlg;
	CString strmsg = _T("");
	CString strmsg2 = _T("");
	int mn_response;

	if(mn_PogoMode == 1) {st_handler.mn_temp_pogo_type = CTL_YES; strmsg = _T("Pogo 사용");}
	else                 {st_handler.mn_temp_pogo_type = CTL_NO;  strmsg = _T("Pogo 비사용");} 
	
	if(mn_AlignMode == 1) {st_handler.mn_temp_alignBuffer_type = CTL_YES; strmsg2 = _T("Align Buffer 사용");}
	else                  {st_handler.mn_temp_alignBuffer_type = CTL_NO;  strmsg2 = _T("Align Buffer 비사용");} 

	st_msg.mstr_confirm_msg.Format("%s, %s",strmsg,strmsg2);
	mn_response = info_dlg.DoModal();
	
	if (mn_response == IDCANCEL)
	{
		return;
	}	
	
	CDialog::OnOK();
}

void CDialog_AlignPogo::OnRadioPogoMode() 
{
	mn_PogoMode = 1;
	OnPogoMode_Change();	
	
}

void CDialog_AlignPogo::OnRadioPogoNotuse() 
{
	mn_PogoMode = 0;
	OnPogoMode_Change();	
	
}

void CDialog_AlignPogo::OnRadioJigMode() 
{
	mn_AlignMode = 1;
	OnAlignMode_Change();
	
}

void CDialog_AlignPogo::OnRadioJigNotuse() 
{
	mn_AlignMode = 0;
	OnAlignMode_Change();
	
}
