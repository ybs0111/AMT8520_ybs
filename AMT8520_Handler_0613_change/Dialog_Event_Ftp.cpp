// Dialog_Event_Ftp.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Event_Ftp.h"
#include "MainFrm.h"
#include "IO_Manager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Event_Ftp dialog


CDialog_Event_Ftp::CDialog_Event_Ftp(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Event_Ftp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Event_Ftp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_caption_main = "Event Message Box";
}


void CDialog_Event_Ftp::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Event_Ftp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDOK, m_btn_ok);
	DDX_Control(pDX, IDC_LABEL_EVENT_FTP, m_label_event_ftp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Event_Ftp, CDialog)
	//{{AFX_MSG_MAP(CDialog_Event_Ftp)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Event_Ftp message handlers
BOOL CDialog_Event_Ftp::Create() 
{  
	return StandardDialog::Create(IDD);
}

BOOL CDialog_Event_Ftp::OnInitDialog() 
{
	StandardDialog::OnInitDialog();
	
	OnEventFtp_Label_Set();
	OnEventFtp_Text_Set();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Event_Ftp::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CDialog_Event_Ftp::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void CDialog_Event_Ftp::OnDestroy() 
{
	g_ioMgr.set_out_bit(st_io.o_buzzer_1, CTL_OFF);
	
	StandardDialog::OnDestroy();
}

void CDialog_Event_Ftp::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	StandardDialog::OnTimer(nIDEvent);
}

BOOL CDialog_Event_Ftp::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
		if (pMsg->wParam == VK_ESCAPE)
			return TRUE;
		
		return StandardDialog::PreTranslateMessage(pMsg);
}

void CDialog_Event_Ftp::OnEventFtp_Label_Set()
{
	m_label_event_ftp.SetText("");
	m_label_event_ftp.SetTextColor(RGB(250, 50, 50));
	m_label_event_ftp.SetBkColor(RGB(181, 182, 140));
	m_label_event_ftp.SetFontBold(FALSE);
	m_label_event_ftp.SetFontName("MS Sans Serif");
	m_label_event_ftp.SetFontSize(36);
}

void CDialog_Event_Ftp::OnEventFtp_Text_Set()
{
	int i;
	CString strFtp;
	
	for (i = 0; i < 3; i++)
	{
		if (st_ftp.mstr_event_ftp[i] != "")
		{
			strFtp += st_ftp.mstr_event_ftp[i];
			strFtp += "\n";
		}
	}
	
	m_label_event_ftp.SetText(strFtp);
}

void CDialog_Event_Ftp::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect r;
	GetWindowRect(&r);
	
	int w = r.Width(), h = r.Height();
	
	r.left = 1280 / 2 - w / 2;
	r.right = r.left + w;
	
	r.top = 1024 / 2 - h / 2;
	r.bottom = r.top + h;
	
	MoveWindow(&r);
}