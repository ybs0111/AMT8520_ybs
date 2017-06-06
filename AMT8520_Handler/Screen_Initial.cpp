// Screen_Initial.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Screen_Initial.h"

// ******************************************************************************
// 대화 상자 클래스 추가                                                         
// ******************************************************************************
#include "Dialog_Infor.h"
#include "Dialog_Select.h"
#include "Dialog_Message.h"
#include "Dialog_Pass_Check.h"
// ******************************************************************************

#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "Srcbase\ALocalization.h"
#include "IO_Manager.h"
#include "ComizoaPublic.h"
#include "COMI_Manager.h"

#include "SrcPart/APartNetworkManager.h"
#include "SrcPart/APartHandler.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Initial

IMPLEMENT_DYNCREATE(CScreen_Initial, CFormView)

CScreen_Initial::CScreen_Initial()
	: CFormView(CScreen_Initial::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Initial)
	//}}AFX_DATA_INIT
}

CScreen_Initial::~CScreen_Initial()
{
}

void CScreen_Initial::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Initial)
	DDX_Control(pDX, IDC_RADIO_ROBOT_P, m_radio_robot_p);
	DDX_Control(pDX, IDC_RADIO_CNT_TRAY, m_radio_cnt_tray);
	DDX_Control(pDX, IDC_RADIO_TT_Y, m_radio_tt_y);
	DDX_Control(pDX, IDC_RADIO_TT_CLAMP, m_radio_tt_clamp);
	DDX_Control(pDX, IDC_RADIO_TT_BS_RAIL, m_radio_tt_rail);
	DDX_Control(pDX, IDC_RADIO_TS21_Z, m_radio_ts2_z);
	DDX_Control(pDX, IDC_RADIO_TS2_PLATE, m_radio_ts2_plate);
	DDX_Control(pDX, IDC_RADIO_TS2_LOCK, m_radio_ts2_lock);
	DDX_Control(pDX, IDC_RADIO_TS2_INSERT, m_radio_ts2_insert);
	DDX_Control(pDX, IDC_RADIO_TS1_Z, m_radio_ts1_z);
	DDX_Control(pDX, IDC_RADIO_TS1_PLATE, m_radio_ts1_plate);
	DDX_Control(pDX, IDC_RADIO_TS1_LOCK, m_radio_ts1_lock);
	DDX_Control(pDX, IDC_RADIO_TS1_INSERT, m_radio_ts1_insert);
	DDX_Control(pDX, IDC_RADIO_TRAY_TRANSFER, m_radio_tt);
	DDX_Control(pDX, IDC_RADIO_TEST_SITE_2, m_radio_ts2);
	DDX_Control(pDX, IDC_RADIO_TEST_SITE_1, m_radio_ts1);
	DDX_Control(pDX, IDC_RADIO_RS_Z, m_radio_rs_z);
	DDX_Control(pDX, IDC_RADIO_REJECT_STACKER, m_radio_rs);
	DDX_Control(pDX, IDC_RADIO_COK_Z, m_radio_cok_z);
	DDX_Control(pDX, IDC_RADIO_COK_Y, m_radio_cok_y);
	DDX_Control(pDX, IDC_RADIO_COK_LOCK, m_radio_cok_lock);
	DDX_Control(pDX, IDC_RADIO_COK, m_radio_cok);
	DDX_Control(pDX, IDC_RADIO_BS3_Z, m_radio_bs3_z);
	DDX_Control(pDX, IDC_RADIO_BS3_RAIL, m_radio_bs3_rail);
	DDX_Control(pDX, IDC_RADIO_BS3_R, m_radio_bs3_r);
	DDX_Control(pDX, IDC_RADIO_BS3_CONV, m_radio_bs3_conv);
	DDX_Control(pDX, IDC_RADIO_BS2_Z, m_radio_bs2_z);
	DDX_Control(pDX, IDC_RADIO_BS2_RAIL, m_radio_bs2_rail);
	DDX_Control(pDX, IDC_RADIO_BS2_R, m_radio_bs2_r);
	DDX_Control(pDX, IDC_RADIO_BS2_CONV, m_radio_bs2_conv);
	DDX_Control(pDX, IDC_RADIO_BS1_Z, m_radio_bs1_z);
	DDX_Control(pDX, IDC_RADIO_BS1_RAIL, m_radio_bs1_rail);
	DDX_Control(pDX, IDC_RADIO_BS1_R, m_radio_bs1_r);
	DDX_Control(pDX, IDC_RADIO_BS1_CONV, m_radio_bs1_conv);
	DDX_Control(pDX, IDC_RADIO_BIN_STACKER_3, m_radio_bs3);
	DDX_Control(pDX, IDC_RADIO_BIN_STACKER_2, m_radio_bs2);
	DDX_Control(pDX, IDC_RADIO_BIN_STACKER_1, m_radio_bs1);
	DDX_Control(pDX, IDC_RADIO_ROBOT_Z, m_radio_robot_z);
	DDX_Control(pDX, IDC_RADIO_ROBOT_Y, m_radio_robot_y);
	DDX_Control(pDX, IDC_RADIO_ROBOT_X, m_radio_robot_x);
	DDX_Control(pDX, IDC_RADIO_ROBOT_PICKER_VACUUM, m_radio_robot_picker_vacuum);
	DDX_Control(pDX, IDC_RADIO_ROBOT_MODULE_EMPTY, m_radio_robot_module);
	DDX_Control(pDX, IDC_RADIO_ROBOT, m_radio_robot);
	DDX_Control(pDX, IDC_RADIO_MOT_BD, m_radio_mot_bd);
	DDX_Control(pDX, IDC_RADIO_IO_BD, m_radio_io_bd);
	DDX_Control(pDX, IDC_GIF_INITIALIZING, m_gif_initializing);
	DDX_Control(pDX, IDC_BTN_INIT_SKIP, m_btn_init_skip);
	DDX_Control(pDX, IDC_BTN_INIT_RETRY, m_btn_init_retry);
	DDX_Control(pDX, IDC_MSG_INITIAL, m_msg_initial);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScreen_Initial, CFormView)
	//{{AFX_MSG_MAP(CScreen_Initial)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_INIT_SKIP, OnBtnInitSkip)
	ON_BN_CLICKED(IDC_BTN_INIT_RETRY, OnBtnInitRetry)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_INIT_BTN, OnInitBtn )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Initial diagnostics

#ifdef _DEBUG
void CScreen_Initial::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Initial::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Initial message handlers

void CScreen_Initial::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// **************************************************************************
	// 컨트롤에 설정할 폰트 생성한다                                             
	// **************************************************************************
// 	mp_init_font = NULL;
// 	mp_init_font = new CFont;
// 	//	mp_init_font->CreateFont(35,20,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"MS Sans Serif");
// 	mp_init_font->CreateFont(35,20,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial Black Italic");
	// **************************************************************************
	All_Stop = 0 ;  // 쓰레드 상태 플래그 설정: 쓰레드 동작 시작
	st_handler.mn_motor_init_check = CTL_READY;
	
	OnInitial_Progress_Set();
	OnInitial_Picture_Set();
	OnInitial_Label_Set();
	OnInitial_RadioButton_Set();
	OnInitial_Change_Status(RBTN_ALL_OFF);
	OnInitial_Step_Clear();
	OnInitial_Initial_Ready();
	
	st_handler.cwnd_init = this;
}

void CScreen_Initial::OnDestroy() 
{
	st_handler.cwnd_init = NULL;
	// **************************************************************************
	// 생성한 폰트 정보 삭제한다                                                 
	// **************************************************************************
// 	delete mp_init_font;
// 	mp_init_font = NULL;
	// **************************************************************************

	CFormView::OnDestroy();
}


void CScreen_Initial::OnTimer(UINT nIDEvent) 
{
	int mn_chk_run;			// 초기화 진행 함수 리턴 플래그 설정 변수
	int n_response;
	
	CDialog_Message  msg_dlg;

	if(nIDEvent == TMR_INIT)  
	{
		if (st_handler.mn_menu_lock != TRUE)  st_handler.mn_menu_lock = TRUE;			// 메뉴 사용 불가능 상태 플래그 설정 

		// **********************************************************************
		// 초기화 작업을 진행한다.                                               
		// **********************************************************************
		mn_chk_run = OnInitial_Init_Excution(); // 초기화 작업 진행 함수

		if (mn_chk_run == RET_GOOD)
		{
			KillTimer(TMR_INIT) ;  // 초기화 진행 타이머 삭제 

			if( g_local.GetLocalType() == LOCAL_KOR )
			{
//				st_msg.mstr_normal_msg = "[초 기 화] 장비 초기화 완료";
				sprintf(st_msg.c_normal_msg, "[초 기 화] 장비 초기화 완료");
			}
			else
			{
//				st_msg.mstr_normal_msg = "[Initialize] check end";
				sprintf(st_msg.c_normal_msg, "[Initialize] check end");
			}

			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG); //james 2010.1004 8); 

			// ******************************************************************
			// 현재 장비 상태 (STOP) 타이틀 바에 출력한다.                       
			// -> 초기화 후에 장비를 정지시킬 경우 이 부분 주석 해제             
			// ******************************************************************
			if (COMI.mn_run_status != dSTOP)
			{
				alarm.mn_reset_status = CTL_NO;
				
				Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수
				if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
			}
			// ******************************************************************

			if (st_handler.mn_initial != TRUE)  st_handler.mn_initial = TRUE;			// 초기화 작업 완료 플래그 설정 
			if (st_handler.mn_menu_lock != FALSE)  st_handler.mn_menu_lock = FALSE;	// 메뉴 사용 가능 상태 플래그 설정
			
			// 초기화가 정상적으로 끝났슴을 알려준다.
// 			FAS.Set_IO_OutportBit(st_io_def.o_reset, IO_ON);
// 			FAS.Set_IO_OutportBit(st_io_def.o_buzzer, IO_ON);

			g_ioMgr.set_out_bit(st_io.o_start_switch_lamp, IO_OFF);
			g_ioMgr.set_out_bit(st_io.o_alarm_clear_lamp, IO_OFF);
			g_ioMgr.set_out_bit(st_io.o_buzzer_off_lamp, IO_OFF);

// 			if( st_basic.mn_mode_xgem == CTL_YES )
// 			{
// 				CWnd* m_pWnd = CWnd::FindWindow(NULL,"XgemInterface");
// 				if(m_pWnd){
// 					//기존의 Program을 활성화 시킨다.
// 					m_pWnd->SetForegroundWindow(); 	
// 					m_pWnd->ShowWindow(SW_RESTORE);		
// 				}
// 				else{
// 					
// 					WinExec("C:\\AMT8520\\bin\\XgemInterface.exe",SW_SHOW);
// 				}
// 				
// // 				g_client_xgem.SetGemInit();
// // 				::Sleep(1000);
// // 				Func.OnSavePPID();//2013,1216
// 				
// 			}

			st_msg.str_fallacy_msg = _T("장비 초기화 완료.");
			if( g_local.GetLocalType() == LOCAL_ENG ) st_msg.str_fallacy_msg = _T("Initial completed.");
			
			n_response = msg_dlg.DoModal();

			Func.Handler_Recovery_Data_Write();

			::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 1);  // 메인 화면 전환 
		}
		else if (mn_chk_run == RET_ERROR)
		{
			KillTimer(TMR_INIT) ;  // 초기화 진행 타이머 삭제
			COMI.Set_MotStop(0, MOTOR_ROBOT_X);

			if ( g_local.GetLocalType() == LOCAL_KOR )	
			{
				sprintf(st_msg.c_abnormal_msg, "[초 기 화] 장비 초기화 실패");
			}
			else
			{
				sprintf(st_msg.c_abnormal_msg, "[initialize] handler initialize check error");
			}

			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);

			// ******************************************************************
			// 현재 장비 상태 (STOP) 타이틀 바에 출력한다.                       
			// -> 초기화 후에 장비를 정지시킬 경우 이 부분 주석 해제             
			// ******************************************************************
			if (COMI.mn_run_status != dSTOP)
			{
				Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수
				if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
			}
			// ******************************************************************

			OnInitial_Controls_Enable(TRUE); // 초기화 화면에 대한 버튼 컨트롤 Enabled/Disabled 설정 함수

			if (st_handler.mn_initial != FALSE)		st_handler.mn_initial = FALSE;	// 초기화 작업 완료 여부 초기화 
			if (st_handler.mn_menu_lock != FALSE)	st_handler.mn_menu_lock = FALSE; // 메뉴 사용 가능 상태 플래그 설정 
		}
		// ******************************************************************************
	}
	
	CFormView::OnTimer(nIDEvent);
}

void CScreen_Initial::OnInitial_Label_Set()
{
	m_msg_initial.SetFont(Func.mp_init_font);
	m_msg_initial.SetWindowText(_T("initializing~!! Wait a moment~"));
	m_msg_initial.SetCenterText();
	m_msg_initial.SetColor(RGB(0,0,0));
	m_msg_initial.SetGradientColor(RGB(0,0,255));
	m_msg_initial.SetTextColor(RGB(255,255,255));
}

void CScreen_Initial::OnInitial_Picture_Set()
{
	if (m_gif_initializing.Load(MAKEINTRESOURCE(IDR_GIF_INITIALIZING),_T("GIF")))
		m_gif_initializing.Draw();
}

void CScreen_Initial::OnInitial_Progress_Set()
{
	CSuperProgressCtrl::RegisterClass();
	
	m_ctrlProgress.Create(this, 4, 250, LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_INITIAL_STR_1)), 0);
	m_ctrlProgress.SetFillStyle(SP_FILL_HORZGRAD);
	m_ctrlProgress.SetColours(RGB(255,0,0), RGB(0,0,0));
	m_ctrlProgress.SetRange(0, 50);
	m_ctrlProgress.SetStep(1);

	m_ctrlProgress1.Create(this, 4, 400, LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_INITIAL_STR_2)), 0);
	m_ctrlProgress1.SetFillStyle(SP_FILL_HORZGRAD);
	m_ctrlProgress1.SetColours(RGB(0,0,255), RGB(0,0,0));
	m_ctrlProgress1.SetRange(50, 100);
	m_ctrlProgress1.SetStep(1);
}

void CScreen_Initial::OnInitial_RadioButton_Set()
{
	m_radio_io_bd.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_mot_bd.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);

	m_radio_robot.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_robot_module.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_robot_picker_vacuum.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_robot_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_robot_x.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_robot_y.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_bs1.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_bs1_conv.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs1_rail.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs1_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs1_r.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_bs2.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_bs2_conv.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs2_rail.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs2_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs2_r.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_bs3.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_bs3_conv.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs3_rail.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs3_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_bs3_r.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_rs.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_rs_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_cnt_tray.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE );
	
	m_radio_tt.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_tt_clamp.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_tt_y.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_tt_rail.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_cok.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_cok_lock.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_cok_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_cok_y.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);

	m_radio_ts1.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_ts1_lock.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts1_insert.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts1_plate.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts1_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	
	m_radio_ts2.SetIcons(IDI_GRAY_LED_ICON, IDI_RED_LED_ICON);
	m_radio_ts2_lock.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts2_insert.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts2_plate.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
	m_radio_ts2_z.SetIcons(IDI_CHECK_FALSE, IDI_CHECK_TRUE);
}

void CScreen_Initial::OnInitial_Controls_Enable(int n_init)
{
	if (n_init == TRUE)  m_btn_init_retry.ShowWindow(SW_SHOW);  
	else                 m_btn_init_retry.ShowWindow(SW_HIDE);
}

void CScreen_Initial::OnInitial_Change_Status(int n_status)
{
	switch(n_status)
	{
	case RBTN_ALL_OFF:  // 이니셜 초기화인 경우 
		m_radio_io_bd.SetLedState(FALSE);
		m_radio_mot_bd.SetLedState(FALSE);

		m_radio_robot.SetLedState(FALSE);
		m_radio_robot_module.SetLedState(FALSE);
		m_radio_robot_picker_vacuum.SetLedState(FALSE);
		m_radio_robot_z.SetLedState(FALSE);
		m_radio_robot_x.SetLedState(FALSE);
		m_radio_robot_y.SetLedState(FALSE);

		m_radio_bs1.SetLedState(FALSE);
		m_radio_bs1_conv.SetLedState(FALSE);
		m_radio_bs1_rail.SetLedState(FALSE);
		m_radio_bs1_z.SetLedState(FALSE);
		m_radio_bs1_r.SetLedState(FALSE);

		m_radio_bs2.SetLedState(FALSE);
		m_radio_bs2_conv.SetLedState(FALSE);
		m_radio_bs2_rail.SetLedState(FALSE);
		m_radio_bs2_z.SetLedState(FALSE);
		m_radio_bs2_r.SetLedState(FALSE);

		m_radio_bs3.SetLedState(FALSE);
		m_radio_bs3_conv.SetLedState(FALSE);
		m_radio_bs3_rail.SetLedState(FALSE);
		m_radio_bs3_z.SetLedState(FALSE);
		m_radio_bs3_r.SetLedState(FALSE);
		break;

	case RBTN_IO:					m_radio_io_bd.SetLedState(TRUE);				break;
	case RBTN_MOTOR:				m_radio_mot_bd.SetLedState(TRUE);				break;
	
	case RBTN_ROBOT:				m_radio_robot.SetLedState(TRUE);				break;
	case RBTN_ROBOT_MODULE_EMPTY:	m_radio_robot_module.SetLedState(TRUE);			break;
	case RBTN_ROBOT_PICKER_VACUUM:	m_radio_robot_picker_vacuum.SetLedState(TRUE);	break;
	case RBTN_ROBOT_Z:				m_radio_robot_z.SetLedState(TRUE);				break;
	case RBTN_ROBOT_P:				m_radio_robot_p.SetLedState(TRUE);				break;
	case RBTN_ROBOT_X:				m_radio_robot_x.SetLedState(TRUE);				break;
	case RBTN_ROBOT_Y:				m_radio_robot_y.SetLedState(TRUE);				break;

	case RBTN_BIN_STACKER1:			m_radio_bs1.SetLedState(TRUE);					break;
	case RBTN_BS1_CONV:				m_radio_bs1_conv.SetLedState(TRUE);				break;
	case RBTN_BS1_RAIL:				m_radio_bs1_rail.SetLedState(TRUE);				break;
	case RBTN_BS1_Z:				m_radio_bs1_z.SetLedState(TRUE);				break;
	case RBTN_BS1_R:				m_radio_bs1_r.SetLedState(TRUE);				break;

	case RBTN_BIN_STACKER2:			m_radio_bs2.SetLedState(TRUE);					break;
	case RBTN_BS2_CONV:				m_radio_bs2_conv.SetLedState(TRUE);				break;
	case RBTN_BS2_RAIL:				m_radio_bs2_rail.SetLedState(TRUE);				break;
	case RBTN_BS2_Z:				m_radio_bs2_z.SetLedState(TRUE);				break;
	case RBTN_BS2_R:				m_radio_bs2_r.SetLedState(TRUE);				break;

	case RBTN_BIN_STACKER3:			m_radio_bs3.SetLedState(TRUE);					break;
	case RBTN_BS3_CONV:				m_radio_bs3_conv.SetLedState(TRUE);				break;
	case RBTN_BS3_RAIL:				m_radio_bs3_rail.SetLedState(TRUE);				break;
	case RBTN_BS3_Z:				m_radio_bs3_z.SetLedState(TRUE);				break;
	case RBTN_BS3_R:				m_radio_bs3_r.SetLedState(TRUE);				break;

	case RBTN_REJECT_STACKER:		m_radio_rs.SetLedState(TRUE);					break;
	case RBTN_RS_Z:					m_radio_rs_z.SetLedState(TRUE);					break;
	case RBTN_RS_CNT_TRAY:			m_radio_cnt_tray.SetLedState(TRUE);				break;

	case RBTN_TRAY_TRANSFER:		m_radio_tt.SetLedState(TRUE);					break;
	case RBTN_TT_CLAMP:				m_radio_tt_clamp.SetLedState(TRUE);				break;
	case RBTN_TT_Y:					m_radio_tt_y.SetLedState(TRUE);					break;
	case RBTN_STACKER_RAIL:			m_radio_tt_rail.SetLedState(TRUE);				break;

	case RBTN_COK:					m_radio_cok.SetLedState(TRUE);					break;
	case RBTN_COK_LOCK:				m_radio_cok_lock.SetLedState(TRUE);				break;
	case RBTN_COK_Z:				m_radio_cok_z.SetLedState(TRUE);				break;
	case RBTN_COK_Y:				m_radio_cok_y.SetLedState(TRUE);				break;

	case RBTN_SITE1:				m_radio_ts1.SetLedState(TRUE);					break;
	case RBTN_SITE1_LOCK:			m_radio_ts1_lock.SetLedState(TRUE);				break;
	case RBTN_SITE1_COK_INSERT:		m_radio_ts1_insert.SetLedState(TRUE);			break;
	case RBTN_SITE1_PLATE_FORWARD:	m_radio_ts1_plate.SetLedState(TRUE);			break;
	case RBTN_SITE1_Z:				m_radio_ts1_z.SetLedState(TRUE);				break;

	case RBTN_SITE2:				m_radio_ts2.SetLedState(TRUE);					break;
	case RBTN_SITE2_LOCK:			m_radio_ts2_lock.SetLedState(TRUE);				break;
	case RBTN_SITE2_COK_INSERT:		m_radio_ts2_insert.SetLedState(TRUE);			break;
	case RBTN_SITE2_PLATE_FORWARD:	m_radio_ts2_plate.SetLedState(TRUE);			break;
	case RBTN_SITE2_Z:				m_radio_ts2_z.SetLedState(TRUE);				break;
	
	default :
		break;
	}
}

void CScreen_Initial::OnInitial_Step_Clear()
{
	int i = 0;
	
	ml_init_step = EIS_START;			// 초기화 진행 스텝 정보 저장 변수 초기화 
	
	for (i = 0; i < MAXSITE; i++)
	{
		st_handler.mn_init_state[i] = CTL_NO;
	}
}

void CScreen_Initial::OnInitial_Initial_Ready()
{
	int n_response;					// 대화 상자에 대한 리턴 값 저장 변수
	CDialog_Select select_dlg;
	char chr_data[25];	
	CString str_lock_check;
	int nRet=0;
	
	st_handler.mn_initial_error = CTL_READY;

	// Lock상태 였는지 확인을 한다.
	:: GetPrivateProfileString("Password", "SysLockStatus", "NO", chr_data, 20, st_path.mstr_basic);
	str_lock_check = chr_data;

	if (str_lock_check == "YES")			// Lock상태에서 강제 종료한것이라면...
	{
		// **************************************************************************
		// Lock 화면으로 전환하여 장비 사용을 막는다.
		// **************************************************************************
		::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, FORM_NAME, 2);  // 메인 프레임에 SYSTEM LOCK 화면 출력 요청
		// **************************************************************************
		if (st_handler.cwnd_list != NULL)
		{
//			st_msg.mstr_normal_msg = "System Locking.";
			sprintf(st_msg.c_normal_msg, "System Locking.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		return;
	}
	
	// **************************************************************************
	// 현재 장비 상태 (INIT) 타이틀 바에 출력한다.                               
	// **************************************************************************
	if (COMI.mn_run_status != dINIT)
	{
		Func.OnSet_IO_Port_Init(); // 장비 상태 : 동작 상태인 경우 I/O 출력 내보내는 함수
		if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);
	}
	// **************************************************************************
	
	if (st_handler.mn_motor_init_check != CTL_READY)			// 모터 초기화를 할것인지 말것인지 결정한다. 2K4/11/16/ViboX	
	{
		SetTimer(TMR_INIT, 250, NULL);  // 초기화 작업 진행 타이머 생성
		return;
	}

	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		g_client_next.SendTrayInfoReq();
		g_client_next.SendRecvStateReq();
	}
	else if( g_handler.GetMachinePos() == EMPOS_REAR )
	{
		g_client_front.SendTrayInfo();
		g_client_front.SendRecvState();
	}

	BOOL bRecovery = FALSE;
	CFile file;
	CString strPath = "C:\\AMT8520\\Setting\\BootUpFlag.txt";
	if( !file.Open( strPath, CFile::modeRead) )
	{
		bRecovery = Func.Handler_Recovery_Data_Read();
		if( bRecovery && OnLevelCheck() == -1)
			bRecovery = FALSE;
	}
	else
	{
		// 복구동작 불가
		file.Close();
		file.Remove( strPath );
		bRecovery = FALSE;
		::WritePrivateProfileString("SaveMode", "RecoveryMode", "0", st_path.mstr_file_basic );
	}

	if( bRecovery )
	{
		n_response = IDOK;
		bRecoveryInit = true;
	}
	else
	{
		bRecoveryInit = false;
		st_msg.mstr_confirm_msg = _T("핸들러 모터를 초기화 하시겠습니까?");
		if ( g_local.GetLocalType() == LOCAL_ENG )	st_msg.mstr_confirm_msg = _T("Are you initialize Handler motor?");
		n_response = select_dlg.DoModal();
	}
	
	if (n_response == IDOK)
	{
		st_handler.mn_motor_init_check = CTL_YES;
	}
	else if (n_response == IDCANCEL)
	{
		st_handler.mn_motor_init_check = CTL_NO;
	}

	SetTimer(TMR_INIT, 250, NULL);  // 초기화 작업 진행 타이머 생성
}

int CScreen_Initial::OnInitial_Init_Excution()
{
	mn_init_flag = RET_PROCEED;

	if (COMI.mn_run_status != dINIT && bRecoveryInit != true )
	{
		if( COMI.mn_run_status == dWARNING )
		{
			mn_init_flag = RET_ERROR;
		}
		return mn_init_flag;
	}

	switch (ml_init_step)
	{
	case EIS_START:					EIS_Start();			break;
	case EIS_ERRMSG:				EIS_ErrMsg();			break;
	case EIS_IO:					EIS_InitIO();			break;
	case EIS_MOTOR:					EIS_Motor();			break;
	case EIS_CHK_STATE:				EIS_Chk_State();		break;
	case EIS_RECOVERY:				EIS_Recovery();			break;
	case EIS_CREATE_THREAD:			EIS_Create_Thread();	break;
	case EIS_SET_THREAD_STEP:		EIS_Set_Thread_Step();	break;
	case EIS_CHK_ALL_FINISH:		EIS_Chk_All_Finish();	break;
	case EIS_FINISH:				EIS_Finish();			break;
	}

	// Progress
	if (mn_pos_step < 51)
	{
		m_ctrlProgress.SetPos(mn_pos_step);
	}
	else if (mn_pos_step > 49)
	{
		if (mn_pos_step > 100)	mn_pos_step = 100;
		m_ctrlProgress.SetPos(50);
		m_ctrlProgress1.SetPos(mn_pos_step);
	}
	
	//초기화중 문제가 생겼다~
	if (st_handler.mn_initial_error == TRUE)
	{
		mn_pos_step = 0;
		m_ctrlProgress.SetPos(0);
		m_ctrlProgress1.SetPos(50);
		
		ml_init_step = EIS_START;			// 초기화 진행 스텝 정보 초기화 
		mn_init_flag = RET_ERROR;	// 초기화 작업 에러 플래그 설정 
	}

	return mn_init_flag;
}

void CScreen_Initial::OnBtnInitSkip() 
{
	KillTimer(TMR_INIT);  // 초기화 진행 타이머 삭제 
	
	sprintf(st_msg.c_normal_msg, "[initialization] SSD Auto Sorter initialization Skip.");
	if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG); 
	
	// ******************************************************************
	// 현재 장비 상태 (STOP) 타이틀 바에 출력한다.                       
	// -> 초기화 후에 장비를 정지시킬 경우 이 부분 주석 해제             
	// ******************************************************************
	if (COMI.mn_run_status != dSTOP)
	{
		alarm.mn_reset_status = CTL_NO;
		g_ioMgr.set_out_bit(st_io.o_alarm_clear_lamp, IO_OFF);	// Reset Switch에 Lamp를 Off 시킨다.
		
		Func.OnSet_IO_Port_Stop(); // 장비 상태: 정지 상태인 경우 I/O 출력 내보내는 함수
		if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, dSTOP);
	}
	// ******************************************************************
	
	if (st_handler.mn_initial != TRUE)  st_handler.mn_initial = TRUE;			// 초기화 작업 완료 플래그 설정 
	if (st_handler.mn_menu_lock != FALSE)  st_handler.mn_menu_lock = FALSE;	// 메뉴 사용 가능 상태 플래그 설정
	
	::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 1);  // 메인 화면 전환 
}

void CScreen_Initial::OnBtnInitRetry() 
{
	int n_response;  // 대화 상자에 대한 리턴 값 저장 변수
	
	CDialog_Select select_dlg;
	
	st_msg.mstr_confirm_msg = _T("Initialization work progress");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		if (st_handler.mn_initial != FALSE)  st_handler.mn_initial = FALSE;  // 초기화 작업 완료 여부 초기화 
		
		st_handler.mn_manual = CTL_YES;
		mn_pos_step = 0;					// 프로그레서 위치 정보 초기화
		
		OnInitial_Controls_Enable(FALSE);	// 초기화 화면에 대한 버튼 컨트롤 Enabled/Disabled 설정 함수
		
		OnInitial_Change_Status(RBTN_ALL_OFF);			// 초기화 화면 이니셜 진행 버튼 상태 변경 함수
		
		OnInitial_Step_Clear();				// 초기화 작업: Step 데이터 저장 변수 초기화 함수
		
		OnInitial_Initial_Ready();			// 초기화 준비 작업 설정 함수
	}
	else if (n_response == IDCANCEL)
	{
		
	}	
}

int CScreen_Initial::OnLevelCheck()
{
	st_handler.mstr_pass_level = "LevelChange";

	CDialog_Pass_Check pass_dlg;
	if( pass_dlg.DoModal() == IDOK )
		return 1;

	return -1;
}

bool CScreen_Initial::ChkInitReady()
{
	if (g_ioMgr.Get_In_Bit(st_io.i_mc1_chk) == CTL_NO || g_ioMgr.Get_In_Bit(st_io.i_mc2_chk) == CTL_NO)
	{
		st_handler.mstrSelectMessage = "Check MC1 or MC2.";
		st_handler.mnSelectMessage = 0;
		::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
		
		return false;
	}

	//hifix
	if( g_ioMgr.Get_Out_Bit( st_io.o_testsite1_hifix_clamp ) == IO_ON ||
		g_ioMgr.Get_In_Bit( st_io.i_testsite1_hifix_clamp_chk ) == IO_OFF )
	{
		st_handler.mstrSelectMessage = "Check TestSite 1 HI-FIX Clamp.";
		st_handler.mnSelectMessage = 0;
		::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
		
		return false;
	}

	if( g_ioMgr.Get_Out_Bit( st_io.o_testsite2_hifix_clamp ) == IO_ON ||
		g_ioMgr.Get_In_Bit( st_io.i_testsite2_hifix_clamp_chk ) == IO_OFF )
	{
		st_handler.mstrSelectMessage = "Check TestSite 2 HI-FIX Clamp.";
		st_handler.mnSelectMessage = 0;
		::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
		
		return false;
	}

	//tray
	if( g_ioMgr.Get_In_Bit( st_io.i_bin_tray1_in_chk ) == IO_ON || 
		g_ioMgr.Get_In_Bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON ||
		g_ioMgr.Get_In_Bit( st_io.i_bin_stacker1_tray_chk ) == IO_ON || 
		g_ioMgr.Get_In_Bit( st_io.i_bin_tray2_stop_position_chk ) == IO_ON ||
		g_ioMgr.Get_In_Bit( st_io.i_bin_stacker2_tray_chk ) == IO_ON || 
		g_ioMgr.Get_In_Bit( st_io.i_bin_tray3_stop_position_chk ) == IO_ON ||
		g_ioMgr.Get_In_Bit( st_io.i_bin_stacker3_tray_chk ) == IO_ON || 
		g_ioMgr.Get_In_Bit( st_io.i_bin_tray3_out_chk ) == IO_ON )
	{
		if     (g_ioMgr.Get_In_Bit( st_io.i_bin_tray1_in_chk ) == IO_ON)            st_handler.mstrSelectMessage = "Check No Tray#1(PS0200).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_tray3_out_chk ) == IO_ON)           st_handler.mstrSelectMessage = "Check No Tray#3(PS0402).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_stacker1_tray_chk ) == IO_ON)	    st_handler.mstrSelectMessage = "Check No Tray#1(PS0106).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_stacker2_tray_chk ) == IO_ON)	    st_handler.mstrSelectMessage = "Check No Tray#2(PS0111).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_stacker3_tray_chk ) == IO_ON)	    st_handler.mstrSelectMessage = "Check No Tray#3(PS0114).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON)	st_handler.mstrSelectMessage = "Check No Tray#1 stop posion chk(PS0202).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_tray2_stop_position_chk ) == IO_ON)	st_handler.mstrSelectMessage = "Check No Tray#2 stop posion chk(PS0301).";
		else if(g_ioMgr.Get_In_Bit( st_io.i_bin_tray3_stop_position_chk ) == IO_ON)	st_handler.mstrSelectMessage = "Check No Tray#3 stop posion chk(PS0401).";
		st_handler.mnSelectMessage = 0;
		::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
		
		return false;
	}

	return true;
}

void CScreen_Initial::EIS_Start()
{
	if (st_handler.mn_motor_init_check == CTL_YES)
	{
		if (ChkInitReady() == false )
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
			
			mn_pos_step = 0;
			ml_init_step = EIS_ERRMSG;
			return;
		}
	}
	
	mcls_m_basic.OnBasic_Data_Load();
	
	mn_pos_step = 9;
	ml_init_step = EIS_IO;
}

void CScreen_Initial::EIS_ErrMsg()
{
	if (st_handler.mnSelectMessage == 1 )					// YES
	{
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
		ml_init_step = EIS_START;
		mn_pos_step = 0;
	}
	else if( st_handler.mnSelectMessage == 2 )
	{
		OnBtnInitSkip();
	}
}

void CScreen_Initial::EIS_InitIO()
{
	bool Ret = g_ioMgr.GetIOBoardInit();		// I/O 보드 초기화 여부 검사한다
	
	if (Ret != TRUE)  
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "EIIB00");
		CTL_Lib.Alarm_Error_Occurrence(700, CTL_dWARNING, "390300");
		
		ml_init_step = EIS_START;						// 초기화 진행 스텝 정보 초기화 
		mn_init_flag = RET_ERROR;				// 초기화 작업 에러 플래그 설정
	}
	else
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "I/O Board Initialized...");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		
		g_ioMgr.OnSet_IO_Out_Port_Clear();
		
		mn_pos_step += 8;
		
		OnInitial_Change_Status(RBTN_IO);				// I/O보드 초기화 완료.
		
		ml_init_step = EIS_MOTOR;
	}
}

void CScreen_Initial::EIS_Motor()
{
	int Ret = g_comiMgr.GetMotBoardInit();
	
	if (Ret != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "EIMB00");
		CTL_Lib.Alarm_Error_Occurrence(701, CTL_dWARNING, "390400");
		
		ml_init_step = EIS_START;						// 초기화 진행 스텝 정보 초기화 
		mn_init_flag = RET_ERROR;				// 초기화 작업 에러 플래그 설정 
	}
	else
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Motor Board Open Success!");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		
		Func.OnMot_Speed_Setting();					// MOTOR 보드가 정상적으로 OPEN이 된 후에 속도를 세팅한다. 2K5/03/24/ViboX
		
		mn_pos_step += 8;
		
		OnInitial_Change_Status(RBTN_MOTOR);					// MOTOR 보드 초기화 완료.
		
		ml_init_step = EIS_CHK_STATE;
	}
}

void CScreen_Initial::EIS_Chk_State()
{
	for (int i=0; i<MAXSITE; i++)
	{
		st_handler.mn_init_state[i] = CTL_NO;
	}
	
	if (st_handler.mn_motor_init_check == CTL_YES)			// 모터 초기화를 할것인지 말것인지 결정한다. 2K4/11/16/ViboX
	{
		if (st_handler.mn_manual == CTL_NO)
		{
			ml_init_step = EIS_RECOVERY;
		}
		else if (st_handler.mn_manual == CTL_YES)
		{	
			if (m_thread[THREAD_ROBOT] != NULL)
			{
				ml_init_step = EIS_CHK_ALL_FINISH;
			}
			else
			{
				ml_init_step = EIS_RECOVERY;
			}
		}
	}
	else// if (st_handler.mn_motor_init_check == CTL_NO)
	{
		EIS_Create_Thread();
		ml_init_step = EIS_FINISH;
		
	}
}

void CScreen_Initial::EIS_Recovery()
{
	if( bRecoveryInit )
	{
		COMI.mn_run_status = dSTOP;
		for( int i=0; i<MOTOR_COUNT; i++ )
		{
			COMI.mn_homechk_flag[i] = BD_YES;
			COMI.Set_MotPower( i, TRUE );
		}
		
		// 각 축에 대하여 초기화가 완료된 것으로 설정.
		// 			for (i = 0; i < MAXSITE; i++)
		// 			{
		// 				st_handler.mn_init_state[i] = CTL_YES;
		//			}
		
		// 화면 정보도 갱신한다.
		// 			if(st_handler.cwnd_main != NULL)
		// 			{
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_TRAY_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_BUFFER_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOB_TRAY_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_PICKER_INFO_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_REJECT_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_CUSTOMER_DSP, 0);
		// 				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_RETEST_DSP, 0);
		// 			}
		// 			
		// 			if(st_handler.cwnd_title != NULL)
		// 			{
		// 				st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, 0);
		// 				st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, DEVICE_MODE, 0);
		// 				st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, LEVEL_MODE, 0);
		// 			}
		
	}
	ml_init_step = EIS_CREATE_THREAD;
}

void CScreen_Initial::EIS_Create_Thread()
{
	m_thread[THREAD_ROBOT]=AfxBeginThread(OnThread_Robot, this);
	if (m_thread[THREAD_ROBOT] != NULL) 	
		hThrHandle[THREAD_ROBOT] = m_thread[THREAD_ROBOT]->m_hThread;

	m_thread[THREAD_REJECT_TRAY_TRANSFER] = AfxBeginThread(OnThread_Reject_Tray_Transfer, this);
	if (m_thread[THREAD_REJECT_TRAY_TRANSFER] != NULL) 	
		hThrHandle[THREAD_REJECT_TRAY_TRANSFER] = m_thread[THREAD_REJECT_TRAY_TRANSFER]->m_hThread;

	m_thread[THREAD_REJECT_STACKER] = AfxBeginThread(OnThread_Reject_Stacker, this);
	if (m_thread[THREAD_REJECT_STACKER] != NULL) 	
		hThrHandle[THREAD_REJECT_STACKER] = m_thread[THREAD_REJECT_STACKER]->m_hThread;

	m_thread[THREAD_COK_ROBOT] = AfxBeginThread(OnThread_COK_Robot, this);
	if (m_thread[THREAD_COK_ROBOT] != NULL) 	
		hThrHandle[THREAD_COK_ROBOT] = m_thread[THREAD_COK_ROBOT]->m_hThread;

		
	Run_Test_Site[0].SetMotID( MOTOR_TESTSITE_1_Z );
	Run_Test_Site[0].SetSiteID( INIT_TEST_SITE_1 );
	m_thread[THREAD_TEST_SITE_1] = AfxBeginThread(OnThread_Test_Site_1, this);
	if (m_thread[THREAD_TEST_SITE_1] != NULL) 	
		hThrHandle[THREAD_TEST_SITE_1] = m_thread[THREAD_TEST_SITE_1]->m_hThread;

		
	Run_Test_Site[1].SetMotID( MOTOR_TESTSITE_2_Z );
	Run_Test_Site[1].SetSiteID( INIT_TEST_SITE_2 );
	m_thread[THREAD_TEST_SITE_2] = AfxBeginThread(OnThread_Test_Site_2, this);
	if (m_thread[THREAD_TEST_SITE_2] != NULL) 	
		hThrHandle[THREAD_TEST_SITE_2] = m_thread[THREAD_TEST_SITE_2]->m_hThread;

	Run_Bin_Stacker[0].SetMotID( MOTOR_BIN_1_Z, MOTOR_BIN_1_R );
	Run_Bin_Stacker[0].SetSiteID( INIT_BIN_STACKER_1 );
	m_thread[THREAD_BIN_STACKER_1] = AfxBeginThread(OnThread_Bin_Stacker_1, this);
	if (m_thread[THREAD_BIN_STACKER_1] != NULL) 	
		hThrHandle[THREAD_BIN_STACKER_1] = m_thread[THREAD_BIN_STACKER_1]->m_hThread;

	Run_Bin_Stacker[1].SetMotID( MOTOR_BIN_2_Z, MOTOR_BIN_2_R );
	Run_Bin_Stacker[1].SetSiteID( INIT_BIN_STACKER_2 );
	m_thread[THREAD_BIN_STACKER_2] = AfxBeginThread(OnThread_Bin_Stacker_2, this);
	if (m_thread[THREAD_BIN_STACKER_2] != NULL) 	
		hThrHandle[THREAD_BIN_STACKER_2] = m_thread[THREAD_BIN_STACKER_2]->m_hThread;

//	Run_Bin_Stacker[2].SetMotID( MOTOR_REJECT_Z, MOTOR_REJECT_R );
//	Run_Bin_Stacker[2].SetSiteID( INIT_BIN_STACKER_3 );
//	m_thread[THREAD_BIN_STACKER_3] = AfxBeginThread(OnThread_Bin_Stacker_3, this);
//	if (m_thread[THREAD_BIN_STACKER_3] != NULL) 	
//		hThrHandle[THREAD_BIN_STACKER_3] = m_thread[THREAD_BIN_STACKER_3]->m_hThread;

//	Run_Bin_Stacker[3].SetMotID( MOTOR_REJECT_Z, MOTOR_REJECT_R );
//	Run_Bin_Stacker[3].SetSiteID( INIT_BIN_STACKER_REJECT );
	Run_Bin_Stacker[2].SetMotID( MOTOR_REJECT_Z, MOTOR_REJECT_R );
//	Run_Bin_Stacker[2].SetSiteID( INIT_BIN_STACKER_REJECT );
	Run_Bin_Stacker[2].SetSiteID( INIT_BIN_STACKER_3 );
	m_thread[THREAD_BIN_STACKER_REJECT] = AfxBeginThread(OnThread_Bin_Stacker_Reject, this);
	if (m_thread[THREAD_BIN_STACKER_REJECT] != NULL) 	
		hThrHandle[THREAD_BIN_STACKER_REJECT] = m_thread[THREAD_BIN_STACKER_REJECT]->m_hThread;

	ml_init_step = EIS_SET_THREAD_STEP;

	//2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		st_handler.mn_robot_motorX_safety = CTL_NO;
		st_handler.mn_robot_motorRej_safety = CTL_NO;
	}
}

void CScreen_Initial::EIS_Set_Thread_Step()
{
	Run_Robot.mn_InitStep = ERI_START;
	Run_Reject_Tray_Transfer.mn_InitStep = ETI_START;
	Run_Reject_Stacker.mn_InitStep = ERSI_START;
	Run_COK_Robot.mn_InitStep = ECI_START;
	Run_Test_Site[0].mn_InitStep = ETSI_START;
	Run_Test_Site[1].mn_InitStep = ETSI_START;


	ml_init_step = EIS_CHK_ALL_FINISH;
}

void CScreen_Initial::EIS_Chk_All_Finish()
{
	int j = 0;
	for (int i = 0; i < MAXSITE; i++)
	{
		if (st_handler.mn_init_state[i] == CTL_YES)
		{
			mn_pos_step += 15;
			
			//OnInitial_Change_Status(i + 3);
			
			st_handler.mn_init_state[i] = CTL_READY;
		}
	}
	
	for (i = 0; i < MAXSITE; i++)
	{
		if (st_handler.mn_init_state[i] == CTL_READY)
		{
			j++;
		}
	}
	
	// 전부 홈체크가 끝났으면 초기화 끝~~~~ ^_^
	if (j == MAXSITE)
	{
		for (i = 0; i < MAXSITE; i++)
		{
			st_handler.mn_init_state[i] = CTL_YES;
		}
		
		ml_init_step = EIS_FINISH;
	}
}

void CScreen_Initial::EIS_Finish()
{
	mn_pos_step = 100;
	
	ml_init_step = EIS_START; 
	mn_init_flag = RET_GOOD;
}

LRESULT CScreen_Initial::OnInitBtn(WPARAM wParam,LPARAM lParam)
{
	OnInitial_Change_Status( (int)wParam );

	return 0;
}
