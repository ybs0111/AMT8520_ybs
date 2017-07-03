// Dialog_Part_Manual.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Part_Manual.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Part_Manual dialog


CDialog_Part_Manual::CDialog_Part_Manual(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Part_Manual::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Part_Manual)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Func.m_p_manual = NULL;
}


void CDialog_Part_Manual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Part_Manual)
	DDX_Control(pDX, IDC_BTN_IO_MOVE_12, m_btn_io_move_12);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_11, m_btn_io_move_11);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_10, m_btn_io_move_10);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_9, m_btn_io_move_9);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_8, m_btn_io_move_8);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_7, m_btn_io_move_7);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_6, m_btn_io_move_6);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_5, m_btn_io_move_5);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_4, m_btn_io_move_4);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_3, m_btn_io_move_3);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_2, m_btn_io_move_2);
	DDX_Control(pDX, IDC_BTN_IO_MOVE_1, m_btn_io_move_1);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_12, m_btn_motor_move_12);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_11, m_btn_motor_move_11);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_10, m_btn_motor_move_10);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_9, m_btn_motor_move_9);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_8, m_btn_motor_move_8);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_7, m_btn_motor_move_7);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_6, m_btn_motor_move_6);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_5, m_btn_motor_move_5);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_4, m_btn_motor_move_4);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_3, m_btn_motor_move_3);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_2, m_btn_motor_move_2);
	DDX_Control(pDX, IDC_BTN_MOTOR_MOVE_1, m_btn_motor_move_1);
	DDX_Control(pDX, IDC_GROUP_MOTOR, m_group_motor);
	DDX_Control(pDX, IDC_GROUP_IO, m_group_io);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Part_Manual, CDialog)
	//{{AFX_MSG_MAP(CDialog_Part_Manual)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_1, OnBtnMotorMove1)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_2, OnBtnMotorMove2)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_3, OnBtnMotorMove3)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_4, OnBtnMotorMove4)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_5, OnBtnMotorMove5)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_6, OnBtnMotorMove6)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_7, OnBtnMotorMove7)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_8, OnBtnMotorMove8)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_9, OnBtnMotorMove9)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_10, OnBtnMotorMove10)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_11, OnBtnMotorMove11)
	ON_BN_CLICKED(IDC_BTN_MOTOR_MOVE_12, OnBtnMotorMove12)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_1, OnBtnIoMove1)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_2, OnBtnIoMove2)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_3, OnBtnIoMove3)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_4, OnBtnIoMove4)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_5, OnBtnIoMove5)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_6, OnBtnIoMove6)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_7, OnBtnIoMove7)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_8, OnBtnIoMove8)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_9, OnBtnIoMove9)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_10, OnBtnIoMove10)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_11, OnBtnIoMove11)
	ON_BN_CLICKED(IDC_BTN_IO_MOVE_12, OnBtnIoMove12)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Part_Manual message handlers

BOOL CDialog_Part_Manual::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rectArea;
	
// 	m_p_font = NULL;
// 	m_p_font = new CFont;
// 	m_p_font->CreateFont(16,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	
	GetClientRect(rectArea);
	MoveWindow(m_cp_coord.x, m_cp_coord.y, rectArea.Width(), rectArea.Height(), TRUE);

	OnPManual_GroupBox_Set();
	OnPManual_Button_Set();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Part_Manual::OnDestroy() 
{
// 	if(m_p_font != NULL)
// 	{
// 		delete m_p_font;
// 		m_p_font = NULL;
// 	}

	CDialog::OnDestroy();
}

void CDialog_Part_Manual::OnBtnExit() 
{
	DestroyWindow();
}

BOOL CDialog_Part_Manual::Create()
{
	return CDialog::Create( CDialog_Part_Manual::IDD );
}

void CDialog_Part_Manual::PostNcDestroy() 
{
	delete this;
	Func.m_p_manual = NULL;
	
	CDialog::PostNcDestroy();
}

void CDialog_Part_Manual::OnPManual_GroupBox_Set()
{
	CSxLogFont manual_font(15,FW_SEMIBOLD,false,"Arial");
	
	m_group_motor.SetFont(manual_font);
	m_group_motor.SetCatptionTextColor(RGB(145,25,0));
	m_group_motor.SetFontBold(TRUE);

	m_group_io.SetFont(manual_font);
	m_group_io.SetCatptionTextColor(RGB(145,25,0));
	m_group_io.SetFontBold(TRUE);
}

void CDialog_Part_Manual::OnPManual_Button_Set()
{
// 	switch (m_n_part)
// 	{
// 	case M_LD_TRAY_ELEVATOR:
// 		m_btn_motor_move_1.SetWindowText("Goto Top");
// 		break;
// 	
// 	case M_TRAY_TRANSFER:
// 		m_btn_motor_move_1.SetWindowText("Goto AirBlow Box");
// 		break;
// 	
// 	case M_ROBOT_X:
// 		m_btn_motor_move_1.SetWindowText("Goto Front Buffer");
// 
// 		break;
// 	
// 	case M_ROBOT_Y:
// 		m_btn_motor_move_1.SetWindowText("Goto Front Buffer");
// 		break;
// 	
// 	case M_ROBOT_Z:
// 		m_btn_motor_move_1.SetWindowText("Goto Buffer Pick");
// 		break;
// 	
// 	case M_ROBOT_PITCH:
// 		m_btn_motor_move_1.SetWindowText("Goto Work");
// 		break;
// 	
// 	case M_ULD_TRAY_ELEVATOR:
// 		m_btn_motor_move_1.SetWindowText("Goto Base");
// 		break;
// 	}
}

void CDialog_Part_Manual::OnBtnMotorMove1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove2() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove3() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove4() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove5() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove6() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove7() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove8() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove9() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove10() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove11() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnMotorMove12() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove1() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove2() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove3() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove4() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove5() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove6() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove7() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove8() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove9() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove10() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove11() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialog_Part_Manual::OnBtnIoMove12() 
{
	// TODO: Add your control notification handler code here
	
}

