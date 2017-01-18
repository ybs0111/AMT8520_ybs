// Dialog_Title.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_Title.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Title dialog


CDialog_Title::CDialog_Title()
{
	//{{AFX_DATA_INIT(CDialog_Title)
	//}}AFX_DATA_INIT
}


void CDialog_Title::DoDataExchange(CDataExchange* pDX)
{
	CInitDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Title)
	DDX_Control(pDX, IDC_TITLE_TIMEOUT_MODE, m_title_auto_timeout_mode);
	DDX_Control(pDX, IDC_TITLE_EMPTY_MODE, m_title_emptytray_mode);
	DDX_Control(pDX, IDC_TITLE_ONLINE_MODE, m_title_online_mode);
	DDX_Control(pDX, IDC_TITLE_REAR_KEY, m_title_rear_key);
	DDX_Control(pDX, IDC_TITLE_FRONT_KEY, m_title_front_key);
	DDX_Control(pDX, IDC_TITLE_LEVEL_MODE, m_title_level_mode);
	DDX_Control(pDX, IDC_TITLE_DEVICE_MODE, m_title_device_mode);
	DDX_Control(pDX, IDC_MSG_STATUS_INFO, m_msg_status_info);
	DDX_Control(pDX, IDC_MSG_FORM_NAME, m_msg_form_name);
	DDX_Control(pDX, IDC_GIF_TITLE_LOGO, m_gif_title_logo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Title, CInitDialogBar)
	//{{AFX_MSG_MAP(CDialog_Title)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STATUS_CHANGE,OnStatusDataUpdate)  // 타이틀 바 화면으로 전송된 메시지 출력시키기 위한 사용자 정의 메시지 추가 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Title message handlers

BOOL CDialog_Title::OnInitDialogBar() 
{
	CInitDialogBar::OnInitDialogBar();
	
	// **************************************************************************
	// 컨트롤에 설정할 폰트 생성한다
	// **************************************************************************
// 	mp_title_font = NULL;
// 	mp_title_font = new CFont;
// 	mp_title_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	// **************************************************************************
	
	OnTitle_Lable_Set();
	OnTitle_Picture_Set(); // 타이틀 바 화면에 대한 GIF 그림 파일 로딩 함수
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Title::OnDestroy() 
{
	// **************************************************************************
	// 생성한 폰트 정보 삭제한다
	// **************************************************************************
// 	delete mp_title_font;
// 	mp_title_font = NULL;
	// **************************************************************************

	CInitDialogBar::OnDestroy();
}

LRESULT CDialog_Title::OnStatusDataUpdate(WPARAM wParam,LPARAM lParam)
{
	int mn_change;			// PostMessage를 통해 수신된 메시지 ID 저장 변수 
	
	mn_change = lParam;						// 명령어 ID 정보 저장 
	
	if(wParam == FORM_NAME)					// 화면 뷰 전환인 경우 
	{
		OnTitle_Form_Name_Change(mn_change);// 현재 출력된 뷰 화면 이름 정보 출력 함수
	}
	else if(wParam == MACHINE_STATUS)		// 장비 상태 정보 변경인 경우 
	{
		OnTitle_Status_Change(mn_change);
	}
	else if(wParam == DEVICE_MODE)			// Device Mode
	{
		OnTitle_Device_mode_Display(mn_change);
	}
	else if(wParam == LEVEL_MODE)
	{
		OnTitle_Level_mode_Display();
	}
	else if(wParam == FRONT_KEY_MODE)
	{
		OnTitle_Front_Key_Display(mn_change);
	}
	else if(wParam == REAR_KEY_MODE)
	{
		OnTitle_Rear_Key_Display(mn_change);
	}
	else if(wParam == ONLINE_MODE)
	{
		OnTitle_Online_Mode_Display();
	}
	else if(wParam == EMPTYTRAY_MODE)
	{
		OnTitle_EmptyTray_Mode_Display();
	}
	else if(wParam == AOTO_TIMEOUT_MODE)
	{
		OnTitle_Auto_Timeut_mode(mn_change);
	}
	
	return 0;
}

void CDialog_Title::OnTitle_Picture_Set()
{
	if (m_gif_title_logo.Load(MAKEINTRESOURCE(IDR_GIF_N_LOGO),_T("GIF")))
		m_gif_title_logo.Draw();
}

void CDialog_Title::OnTitle_Lable_Set()
{
	m_msg_form_name.SetAlignment(CNewLabel::HORZ_LEFT | CNewLabel::VERT_CENTER);
	m_msg_form_name.SetTextColor(RGB(0, 0, 0));
	m_msg_form_name.SetBkColor(RGB(181, 182, 140));
	m_msg_form_name.SetFontBold(TRUE);
	m_msg_form_name.SetFontName("MS Sans Serif");
	m_msg_form_name.SetFontSize(12);
	
	m_msg_status_info.SetFont(Func.mp_title_font);
	m_msg_status_info.SetCenterText();
	m_msg_status_info.SetColor(RGB(255,255,255));
	m_msg_status_info.SetGradientColor(RGB(0,255,0));
	m_msg_status_info.SetTextColor(RGB(255,255,255));

	m_title_device_mode.SetFont(Func.mp_title_font);
	m_title_device_mode.SetCenterText();
	m_title_device_mode.SetColor(RGB(0,0,255));
	m_title_device_mode.SetGradientColor(RGB(0,0,0));
	m_title_device_mode.SetTextColor(RGB(255,255,255));
	
	m_title_level_mode.SetFont(Func.mp_title_font);
	m_title_level_mode.SetCenterText();
	m_title_level_mode.SetColor(RGB(0,0,255));
	m_title_level_mode.SetGradientColor(RGB(0,0,0));
	m_title_level_mode.SetTextColor(RGB(255,255,255));

	m_title_front_key.SetFont(Func.mp_title_font);
	m_title_front_key.SetCenterText();
	m_title_front_key.SetColor(RGB(0,0,255));
	m_title_front_key.SetGradientColor(RGB(0,0,0));
	m_title_front_key.SetTextColor(RGB(255,255,255));

	m_title_rear_key.SetFont(Func.mp_title_font);
	m_title_rear_key.SetCenterText();
	m_title_rear_key.SetColor(RGB(0,0,255));
	m_title_rear_key.SetGradientColor(RGB(0,0,0));
	m_title_rear_key.SetTextColor(RGB(255,255,255));

	m_title_online_mode.SetFont(Func.mp_title_font);
	m_title_online_mode.SetCenterText();
	m_title_online_mode.SetColor(RGB(0,0,255));
	m_title_online_mode.SetGradientColor(RGB(0,0,0));
	m_title_online_mode.SetTextColor(RGB(255,255,255));	

	m_title_emptytray_mode.SetFont(Func.mp_title_font);
	m_title_emptytray_mode.SetCenterText();
	m_title_emptytray_mode.SetColor(RGB(0,0,255));
	m_title_emptytray_mode.SetGradientColor(RGB(0,0,0));
	m_title_emptytray_mode.SetTextColor(RGB(255,255,255));	

	m_title_auto_timeout_mode.SetFont(Func.mp_title_font);
	m_title_auto_timeout_mode.SetCenterText();
	m_title_auto_timeout_mode.SetColor(RGB(0,0,255));
	m_title_auto_timeout_mode.SetGradientColor(RGB(0,0,0));
	m_title_auto_timeout_mode.SetTextColor(RGB(255,255,255));

}

void CDialog_Title::OnTitle_Form_Name_Change(int nViewID)
{
	CString mstr_form_name;  // 현재 설정된 화면 정보 저장 변수 
	
	switch(nViewID)
	{
	case IDW_SCREEN_MAIN:
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: Main");
		break;
	case IDW_SCREEN_LOCK:
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: System Lock");
		break;
	case IDW_SCREEN_INIT:
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: Initial");
		break;
	case IDW_SCREEN_BASIC :
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: Basic");
		break;
	case IDW_SCREEN_SET_MAINTENANCE :
		mstr_form_name = _T(" Menu: Setting");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Maintenance");
		break;
	case IDW_SCREEN_WAIT_TIME :
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Wait Time");
		break;
	case IDW_SCREEN_MOTOR_TEACH:
		mstr_form_name = _T(" Menu: Motor");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Teach");
		break;
	case IDW_SCREEN_MOTOR_SPEED :
		mstr_form_name = _T(" Menu: Motor");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Speed");
		break;

	case IDW_SCREEN_MOTOR_WITH_IO:
		mstr_form_name = _T(" Menu: Motor");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: With IO");
		break;

	case IDW_SCREEN_LIST_OPERATION :
		mstr_form_name = _T(" Menu: List");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Operation");
		break;
	case IDW_SCREEN_LIST_DATA_VIEW:
		mstr_form_name = _T(" Menu: List");
		mstr_form_name += _T("\n");
		mstr_form_name += _T(" Screen: Data View");
		break;
	case IDW_SCREEN_IO :
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: I/O");
		break;
	case IDW_SCREEN_ADMINISTRATOR :
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: ADMINISTRATOR");
		break;
	case IDW_SCREEN_FILE_EDITOR:
		mstr_form_name = _T("\n");
		mstr_form_name += _T(" Screen: File Editor");
		break;
	default:
		mstr_form_name = _T(" ERROR: SCREEN VIEW ID");
		break;
	}
	
	m_msg_form_name.SetText(_T(mstr_form_name));
}

void CDialog_Title::OnTitle_Status_Change(int n_status)
{
	switch(n_status)
	{
	case dSTOP :		// STOP 상태
		m_msg_status_info.SetWindowText(_T("STOP"));
		m_msg_status_info.SetTextColor(RGB(255,0,0));
		break;
	case dRUN :			// RUN 상태
		m_msg_status_info.SetWindowText(_T("RUN"));
		m_msg_status_info.SetTextColor(RGB(0, 0, 255));
		break;
	case dJAM :			// ALARM 상태
		m_msg_status_info.SetWindowText(_T("ALARM"));
		m_msg_status_info.SetTextColor(RGB(255,0,0));
		break;
	case dWARNING :		// ALARM 상태
		m_msg_status_info.SetWindowText(_T("ALARM"));
		m_msg_status_info.SetTextColor(RGB(255,0,0));
		break;
	case dMAINT :		// MAINT 상태
		m_msg_status_info.SetWindowText(_T("MAINT"));
		m_msg_status_info.SetTextColor(RGB(0,0,200));
		break;
	case dINIT :		// INIT 상태
		m_msg_status_info.SetWindowText(_T("INIT"));
		m_msg_status_info.SetTextColor(RGB(50,100,150));
		break;
	case dLOTEND :		// LOT END 상태
		m_msg_status_info.SetWindowText(_T("LOTEND"));
		m_msg_status_info.SetTextColor(RGB(50,100,150));
		break;
	case dLOCK :		// LOCK 상태
		m_msg_status_info.SetWindowText(_T("LOCK"));
		m_msg_status_info.SetTextColor(RGB(50,100,150));
		break;
	case dSELFCHECK :	// SELFCHECK 상태
		m_msg_status_info.SetWindowText(_T("SELFCHECK"));
		m_msg_status_info.SetTextColor(RGB(50,100,150));
		break;
	default :
		break;
	}
	
	m_msg_status_info.SetColor(RGB(255,255,255));
	m_msg_status_info.SetGradientColor(RGB(0,255,0));
}

void CDialog_Title::OnTitle_Device_mode_Display(int n_device)
{
	CString str_device;   // 임시 저장 변수

	switch(st_basic.n_mode_device)
	{
	case 0:
		m_title_device_mode.SetColor(RGB(255,0,0));
		str_device = _T("WITHOUT DEVICE");
		break;
	case 1:
		m_title_device_mode.SetColor(RGB(0,0,255));
		str_device = _T("WITH DEVICE");
		break;
	default:
		m_title_device_mode.SetColor(RGB(255,0,0));
		str_device = _T("ERROR");
		break;
	}

	m_title_device_mode.SetWindowText(_T(str_device));
}

void CDialog_Title::OnTitle_Level_mode_Display()
{
	CString str_level;   // 임시 저장 변수
	
	if (st_handler.mn_level_teach == TRUE)
	{
		m_title_level_mode.SetColor(RGB(255,0,0));
		str_level = _T("LEVEL 2 (Teaching Mode)");
	}
	else if (st_handler.mn_level_maint == TRUE)
	{
		m_title_level_mode.SetColor(RGB(255,0,0));
		str_level = _T("LEVEL 1 (Maint Mode)");
	}
	else
	{
		m_title_level_mode.SetColor(RGB(0,0,255));
		str_level = _T("LEVEL 0 (Operation Mode)");
	}
	
	m_title_level_mode.SetWindowText(_T(str_level));
}

void CDialog_Title::OnTitle_Front_Key_Display(int n_key)
{
	CString str_key;   // 임시 저장 변수
	
	switch(n_key)
	{
	case 0:
		m_title_front_key.SetColor(RGB(255,0,0));
		str_key = _T("Front : Manual Mode");
		break;
	case 1:
		m_title_front_key.SetColor(RGB(0,0,255));
		str_key = _T("Front : Auto Mode");
		break;
	default:
		m_title_front_key.SetColor(RGB(255,0,0));
		str_key = _T("Front : Error");
		break;
	}
	
	m_title_front_key.SetWindowText(_T(str_key));
}

void CDialog_Title::OnTitle_Rear_Key_Display(int n_key)
{
	CString str_key;   // 임시 저장 변수
	
	switch(n_key)
	{
	case 0:
		m_title_rear_key.SetColor(RGB(255,0,0));
		str_key = _T("Rear : Manual Mode");
		break;
	case 1:
		m_title_rear_key.SetColor(RGB(0,0,255));
		str_key = _T("Rear : Auto Mode");
			break;
	default:
		m_title_rear_key.SetColor(RGB(255,0,0));
		str_key = _T("Rear : Error");
		break;
	}
	
	m_title_rear_key.SetWindowText(_T(str_key));
}

void CDialog_Title::OnTitle_Online_Mode_Display()
{
	CString str_online;   // 임시 저장 변수
	
	switch(st_basic.mn_mode_run)
	{
	case 0:
		m_title_online_mode.SetColor(RGB(255,0,0));
		str_online = _T("OFFLINE");
		break;
	case 1:
		m_title_online_mode.SetColor(RGB(0,0,255));
		str_online = _T("ONLINE");
		break;
	default:
		m_title_online_mode.SetColor(RGB(255,0,0));
		str_online = _T("ERROR");
		break;
	}
	
	m_title_online_mode.SetWindowText(_T(str_online));
}

void CDialog_Title::OnTitle_EmptyTray_Mode_Display()
{
	CString str_empty;   // 임시 저장 변수
	
	switch(st_basic.mn_mode_run)
	{
	case 0:
		m_title_emptytray_mode.SetColor(RGB(255,0,0));
		str_empty.Format("EMPTY OUT : %d IN: %d",st_handler.mn_emptyout_cnt,st_handler.mn_emptyin_cnt);
		break;
	case 1:
		m_title_emptytray_mode.SetColor(RGB(0,0,255));
		str_empty.Format("EMPTY OUT : %d IN: %d",st_handler.mn_emptyout_cnt,st_handler.mn_emptyin_cnt);
		break;
	default:
		m_title_emptytray_mode.SetColor(RGB(255,0,0));
		str_empty = _T("ERROR");
		break;
	}
	
	m_title_emptytray_mode.SetWindowText(_T(str_empty));
}

void CDialog_Title::OnTitle_Auto_Timeut_mode( int n_key )
{
	CString str_auto_timeout_mode;   // 임시 저장 변수
	
	switch(st_basic.n_auto_timeout_mode)
	{
	case 0:
		m_title_auto_timeout_mode.SetColor(RGB(0,0,255));
		str_auto_timeout_mode.Format( "Time(%d)", st_basic.mn_time_out);
		break;
	case 1:
		m_title_auto_timeout_mode.SetColor(RGB(2550,0,0));
		str_auto_timeout_mode.Format( "Time(%d)", n_key);
		break;
	default:
		m_title_auto_timeout_mode.SetColor(RGB(255,0,0));
		str_auto_timeout_mode = _T("ERROR");
		break;
	}
	
	m_title_auto_timeout_mode.SetWindowText(_T(str_auto_timeout_mode));
}