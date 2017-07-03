// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Handler.h"

#include "MainFrm.h"

// ******************************************************************************
// ��ȭ ���� �� �ܺ� Ŭ���� �߰�                                                 
// ******************************************************************************
#include "MenuHead.h"		// ȭ�鿡 ���� ��� ���ǵ� Ŭ���� �߰� 
#include "Dialog_Menu.h"	// �޴� ���� ��ȭ ���� Ŭ���� �߰� 

#include "Dialog_About.h"	// ���α׷� About ��ȭ ���� Ŭ���� �߰� 
#include "Dialog_Exit.h"	// ���� ���α׷� ���� ȭ�� Ŭ���� �߰� 
#include "Dialog_Pass_Check.h"
#include "Dialog_Infor.h"
#include "Dialog_Select.h"
#include "Dialog_Message.h"
#include "Dialog_KeyBoard.h"
#include "Dialog_Pass_Check.h"
#include "Public_Function.h"	// ���� ���� �� I/O ���� ���� Ŭ����
#include "RUN_IO_ReadWrite.h"
#include "Screen_Main.h"
#include "ComizoaPublic.h"
#include "Cmmsdk.h"

#include "AVersion.h"

#include "io.h" 
#include "FAS_HSSI.h"
#include "FastechPublic_IO.h"

#include "SrcPart/APartShortkey.h"
#include "SrcPart/APartDatabase.h"
#include "SrcPart//PartFunction.h"

#include "IO_Manager.h"
#include "COMI_Manager.h"

#include "AMTRegistry.h"
#include "Srcbase/ALocalization.h"

#include "AMTLotManager.h"
#include "Dialog_XgemInterface.h"
#include "AClient_Xgem.h"

#include "SrcPart/APartNetworkManager.h"

#include "FtpClient.h"//2015.0604
//2015.0914
#include "SrcBase\ACriticalSection.h"
#include "Run_Test_Site.h"
#include "SrcPart/APartTestSite.h"
#include "SrcPart/APartTray.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

st_handler_param			st_handler;

st_setting_param			st_set;
st_message_param			st_msg;
st_message_param_ftp		st_ftp;//2015.0604
st_time_param				st_time;
st_lamp_param				st_lamp;
st_basic_param				st_basic;
st_alarm_param				alarm;
st_dsp_alarm_param			st_alarm;
st_work_param				st_work;
st_modulemap_size			st_modulemap;
st_motor_param				st_motor[MOTOR_COUNT];
st_path_param				st_path;
st_nw_param					st_NW;
st_sync_param				st_sync;
st_ctlbd_param				st_ctlbd;
st_ctlbd_param				st_reco_ctlbd;  
st_serial_param				st_serial;
st_linearmotion_param		st_linearmot[8];
st_coordmotion_param		st_coordmot[16];
//2015.0914
st_DB_time_param			st_DB_time; 
st_dbtime_database          st_dbtime;
st_Buffer_DB_database       st_BufferTemp;
st_dbTimeTmp_database       *st_dbTimeTmp;
st_dbTimeTmp_database       *st_dbTimeDate;
///////////////

st_time_database			st_timedb;
st_alarm_database			st_alarmdb;
st_ground_database			st_grounddb;

st_buffer_database			st_bufferdb;

st_part_motor_info			st_motor_info;
st_part_info				st_part;
st_buffer_tray_param		st_buffer_tray[MAX_SITE_INFO];

st_boat_database			st_boatdb;
st_xgemdata					st_xgem;

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONDBLCLK()
	ON_WM_NCRBUTTONDOWN()
	ON_COMMAND(ID_MAIN, OnMain)
	ON_COMMAND(ID_BASIC, OnBasic)
	ON_COMMAND(ID_SETTING, OnSetting)
	ON_COMMAND(ID_WAIT, OnWait)
	ON_COMMAND(ID_MOTOR, OnMotor)
	ON_COMMAND(ID_IO, OnIo)
	ON_COMMAND(ID_LIST, OnList)
	ON_COMMAND(ID_LEVEL, OnLevel)
	ON_COMMAND(ID_SAVE, OnSave)
	ON_COMMAND(ID_ALARM, OnAlarm)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_RESET, OnReset)
	ON_COMMAND(ID_LOCK, OnLock)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_MOT_TEACH, OnMotTeach)
	ON_WM_TIMER()
	ON_COMMAND(ID_MOT_SPEED, OnMotSpeed)
	ON_COMMAND(ID_SET_INTERFACE, OnSetInterface)
	ON_COMMAND(ID_SET_MAINTENANCE, OnSetMaintenance)
	ON_COMMAND(ID_ADMIN, OnAdmin)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_LIST_OPER, OnListOper)
	ON_COMMAND(ID_LIST_DATAVIEW, OnListDataview)
	ON_COMMAND(ID_FILE_EDITOR, OnFileEditor)
	ON_COMMAND(IDD_SCREEN_MOTOR_WITH_IO, OnMotWithIO)
	ON_WM_NCMBUTTONDBLCLK()
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_FORM_CHANGE, OnViewChangeMode)					// Post Message�� ���� ȭ�� ��ȯ ����� ����� ���� �޽��� ����
	ON_MESSAGE(WM_MAINFRAME_WORK, OnMainframe_Work)
	ON_MESSAGE(WM_MAIN_EVENT, OnMainMessageEvent)					// �̺�Ʈ ���� �޽���
	ON_MESSAGE(WM_COMM_DATA, OnCommunication)						// RS-232C �ø��� ��Ʈ ���� �޽���
	ON_MESSAGE(WM_COMM_EVENT, OnCommunicationEvent)					// RS-232C �ø��� ��Ʈ �̺�Ʈ ���� �޽���
	ON_MESSAGE(WM_DATA_SEND, OnDataSend)							// RS-232C �ø��� ��Ʈ ���� ������ �۽� �޽���
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	g_db.ChangeSection( ST_STOP );
	st_timedb.mnSectionBuffer = dSTOP;

//	FAS_IO.mn_simulation_mode = 0;
	COMI.mn_simulation_mode = 0;

	OnMain_Var_Default_Set();

	MyJamData.On_Alarm_Info_Load();



	m_timeLastDay = AMTRegistry::RegReadTime();
}

CMainFrame::~CMainFrame()
{
	if(mp_msg_dlg != NULL)   delete mp_msg_dlg;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CString str_temp;
	int i = 0, j = 0;

	st_handler.hWnd = GetSafeHwnd() ;  // ���� �����ӿ� ���� �ڵ� ���� ����

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_wndToolBar.SetBorders(1, 1, 1, 1);

	if (!m_wndToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);

	m_wndToolBar.LoadTrueColorToolBar(TOOLBAR_DRAW_BUTTON_WIDTH,
								   IDB_TOOLBAR_DRAW,
		                           IDB_TOOLBAR_DRAW_HOT,
								   IDB_TOOLBAR_DRAW_DISABLED);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP);
//	DockControlBar(&m_wndToolBar);					// Docking ����� �ƴϴ�.

	// **************************************************************************
    // Text ���� Icon ������ ����
    // **************************************************************************
	SIZE size1={32, 32}, size2={62, 49};
	m_wndToolBar.SetSizes(size2, size1);
	// **************************************************************************

	// **************************************************************************
    // Ÿ��Ʋ ���� ��� Initial Dialog bar ����
    // **************************************************************************
	if (!m_wndTitleBar.Create(this, IDD_DIALOG_TITLE, CBRS_ALIGN_TOP, IDD_DIALOG_TITLE))
	{
		TRACE0("Failed to create my init bar\n");
		return -1;      // fail to create
	}
	st_handler.cwnd_title = &m_wndTitleBar;
	// **************************************************************************
	
	mcls_m_basic.OnAdmin_Data_Load();
	mcls_m_basic.OnMotorSpeed_Set_Data_Load();
	mcls_m_basic.OnBasic_Data_Load();							// ������ ���� ���� ���� ������ �ε��ϴ� �Լ�
	mcls_m_basic.OnWaitTime_Data_Load();
	mcls_m_basic.OnMaintenance_Data_Load();

	Func.Load_scrap_code_Data();//2015.05.18

	// **************************************************************************
    // ����Ʈ ���� ��� Initial Dialog bar ����
	// -> ���� �� �ؿ� ��ġ�ϵ��� �Ϸ��� �� �κ��� ���� �� ���� �κ� ���� �ø���.
    // **************************************************************************
	if (!m_wndListBar.Create(this, IDD_DIALOG_LIST, CBRS_ALIGN_BOTTOM, IDD_DIALOG_LIST))
	{
		TRACE0("Failed to create my information bar\n");
		return -1;      // fail to create
	}
	st_handler.cwnd_list = &m_wndListBar;
	// **************************************************************************

	g_ver.SetVersion();
	
	// **************************************************************************
    // ���� ���α׷��� ���� Ÿ��Ʋ ���� ���
    // ************************************************************************** 
	SetWindowText( g_ver.GetStrVersion() );	// Ÿ��Ʋ ���� ��� 
	CenterWindow();					// ���� ���� ���α׷��� �� �߾ӿ� ��ġ
	// **************************************************************************

	// **************************************************************************
    // ���� ���� �� I/O ���� �ʱ�ȭ �� ��� ��Ʈ Ŭ���� �۾� �����Ѵ�        
    // **************************************************************************
	g_ioMgr.IO_Board_Initialize();				// IO ���� ���̺귯�� �ʱ�ȭ �Լ�
	RUN_IO_ReadWrite.Init_Total_IORead(0);
	g_ioMgr.OnSet_IO_Out_Port_Clear();			// I/O ������ ��� ��Ʈ �ʱ�ȭ �Լ�
	
	g_comiMgr.SetMotBoardInit( COMI.Initialize_MotBoard(st_path.mstr_motor) );	// MOTOR ���� �ʱ�ȭ ���� �˻��Ѵ�
	if( st_handler.mn_virtual_mode )
		g_comiMgr.SetMotBoardInit( BD_GOOD );

	if (st_handler.cwnd_list != NULL)
	{
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
	}

	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, dSTOP); 
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, DEVICE_MODE, 0);
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, LEVEL_MODE, 0);
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, ONLINE_MODE, 0);
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, AOTO_TIMEOUT_MODE, 0);
	}

	g_comiMgr.OnMain_Motor_Setting();
	
	OnMain_Thread_Creating();
	
	OnMain_Port_Create();					// Serial Port�� �����Ѵ�.

	// **************************************************************************
    // �ʱ�ȭ ���� ȭ������ ��ȯ�Ͽ� ��� �ʱ�ȭ �۾� �����Ѵ�                   
    // **************************************************************************
	::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 3);  // �ʱ�ȭ ���� ȭ�� ��ȯ 
	// **************************************************************************

	SetTimer(TMR_MAIN_REFRESH, 500, NULL);  // ����Ʈ ���� ���� Ÿ�̸�
	SetTimer(TMR_FILE_CREATE, 1000, NULL);  // ����Ʈ ���� ���� Ÿ�̸�

	// �׽�Ʈ��.
	st_handler.mn_level_teach =  TRUE;

	if( st_handler.cwnd_title )
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, LEVEL_MODE, 0);

	st_handler.cwnd_data_lot = NULL;
	st_handler.cwnd_data_testsite = NULL;

	::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1002, 0);  // �ʱ�ȭ ���� ȭ�� ��ȯ 

// 	if( st_basic.mn_mode_xgem == CTL_YES )
// 	{
// 		CWnd* m_pWnd = CWnd::FindWindow(NULL,"XgemInterface");
// 		if(m_pWnd){
// 			//������ Program�� Ȱ��ȭ ��Ų��.
// 			m_pWnd->SetForegroundWindow(); 	
// 			m_pWnd->ShowWindow(SW_RESTORE);		
// 		}
// 		else{
// 
// 			WinExec("C:\\AMT8520\\bin\\XgemInterface.exe",SW_SHOW);
// 		}
// 
// 		::Sleep(1000);
// 		g_client_xgem.SetGemInit();
// 		::Sleep(500);
// 		Func.OnSavePPID();//2013,1216
// 	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if ( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_SYSMENU | WS_MAXIMIZEBOX;
	cs.style &= ~FWS_ADDTOTITLE;								// ȭ�� Ÿ��Ʋ ���� ���� �����ϵ��� ��Ÿ�� ���� 
	cs.lpszClass=((CHandlerApp*)AfxGetApp())->GetClassName();	// ���� ���� ���α׷��� ���� Ŭ���� �̸��� ��´�(Need for Single Instance)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	//Ÿ��Ʋ�ٸ� Ŭ�� ������ ���Ͻ�Ų��.
	if (nHitTest == HTCAPTION) return;
	
	CFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}

void CMainFrame::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	//Ÿ��Ʋ�ٸ� Ŭ�� ������ ���Ͻ�Ų��.
	if (nHitTest == HTCAPTION) return;
	
	CFrameWnd::OnNcLButtonDown(nHitTest, point);
}

void CMainFrame::OnNcRButtonDblClk(UINT nHitTest, CPoint point) 
{
	//Ÿ��Ʋ�ٸ� Ŭ�� ������ ���Ͻ�Ų��.
	if (nHitTest == HTCAPTION) return;
	
	CFrameWnd::OnNcRButtonDblClk(nHitTest, point);
}

void CMainFrame::OnNcRButtonDown(UINT nHitTest, CPoint point) 
{
	//Ÿ��Ʋ�ٸ� Ŭ�� ������ ���Ͻ�Ų��.
	if (nHitTest == HTCAPTION) return;
	
	CFrameWnd::OnNcRButtonDown(nHitTest, point);
}

void CMainFrame::OnNcMButtonDblClk(UINT nHitTest, CPoint point) 
{
	if(nHitTest == HTCAPTION)		// Ÿ��Ʋ�� ����Ŭ���̸� ����
		return;
	
	//     if(nHitTest == HTMAXBUTTON)    // MAX��ư Ŭ���̸� ����
	//         return;		
	
	CFrameWnd::OnNcMButtonDblClk(nHitTest, point);
}

//SysCommand�� �ִ�ȭ�� ���ƾ� ..
// void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
// {	
//     if(nID == SC_MAXIMIZE)    // �ִ�ȭ ����̸� ����		
//         return;
// 	
//     CMDIFrameWnd::OnSysCommand(nID, lParam);
// 	
// }


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->hwnd == m_wndToolBar.m_hWnd)
	{
		if (m_wndToolBar.IsFloating())  // ���ٰ� �÷��� �� ��쿡 �ٽ� ȭ�� ��ܿ� ���� 
		{
			m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
			EnableDocking(CBRS_ALIGN_TOP);
			DockControlBar(&m_wndToolBar);
		}
	}


	g_ShortKey.CheckKeyboard( pMsg );
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnMain() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	// int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	// if (nmenu_chk != TRUE)  return;
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Main)))   return;
	OnSwitchToForm(IDW_SCREEN_MAIN);
}

void CMainFrame::OnBasic() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Basic)))   return;
	OnSwitchToForm(IDW_SCREEN_BASIC);
}

void CMainFrame::OnSetting() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Set_Maintenance)))   return;
	OnSwitchToForm(IDW_SCREEN_SET_MAINTENANCE);	
}

void CMainFrame::OnWait() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Wait_Time)))   return;
	OnSwitchToForm(IDW_SCREEN_WAIT_TIME);
}

void CMainFrame::OnMotor() 
{
	int nRet, nRet2;

	CDialog_Menu menu_dlg;
	
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************

	nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
	nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);
	
	if (nRet == IO_ON && nRet2 == IO_ON)
	{			
		return;
	}
	
	st_msg.mstr_parent_menu = "Motor Screen";  // ���� ���õ� �� �޴� ���� ����
	int nResponse = menu_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
}

void CMainFrame::OnIo() 
{
	int nRet, nRet2;
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
	nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);
	
	if (nRet == IO_ON && nRet2 == IO_ON)
	{			
		return;
	}

	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_IO)))   return;
	OnSwitchToForm(IDW_SCREEN_IO);
}

void CMainFrame::OnList() 
{
	CDialog_Menu menu_dlg;
	
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// ������ ���� �ְ� ����. -�̻�� �븮- 2K4/11/05/ViboX
	/* ************************************************************************** */
	/* �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                               */
	/* -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                       */
	/* ************************************************************************** */
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	/* ************************************************************************** */
	
	st_msg.mstr_parent_menu = "List Screen";  // ���� ���õ� �� �޴� ���� ����
	int nResponse = menu_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
}

void CMainFrame::OnLevel() 
{
	int n_response;  // ��ȭ ���ڿ� ���� ���� �� ���� ���� ���� 
	
	/* ************************************************************************** */
    /* ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                       */
    /* ************************************************************************** */
	if (st_handler.mn_menu_lock != FALSE)  return;
	if (st_handler.mn_system_lock != FALSE)  return;  // ���� �ý��� Lock ������ ��� ���� 
	
	st_handler.mstr_pass_level = "LevelChange";
	
	CDialog_Pass_Check pass_dlg;
	
	n_response = pass_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		
	}
	else if (n_response == IDCANCEL)
	{
		
	}
}

void CMainFrame::OnSave() 
{
	int n_response;  // ��ȭ ���� ���� �÷���
	
	/* ************************************************************************** */
    /* ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                       */
    /* ************************************************************************** */
	if (st_handler.mn_menu_lock != FALSE)  return;
	if (st_handler.mn_system_lock != FALSE)  return;  // ���� �ý��� Lock ������ ��� ���� 
	
	CDialog_Select  select_dlg;
	
	st_msg.mstr_confirm_msg = _T("Main : All Setting Data Save?");
	
	n_response = select_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		st_handler.mn_menu_lock = TRUE;
		
		OnExitData_Saving();					// ȭ�鿡 ������ ��� ������ ���� �Լ�
		
		st_handler.mn_menu_lock = FALSE;
	}
	else if (n_response == IDCANCEL)
	{
		
	}
}

void CMainFrame::OnAlarm() 
{
	/* ************************************************************************** */
    /* ��޸��� �˶� ȭ�� ����Ѵ�                                                */
	/* -> ���� �˶� ȭ�� ��µ� ��쿡�� �ش� ȭ�鿡 ��Ŀ���� ����                */
    /* ************************************************************************** */
	if (mp_alarm_dlg != NULL)  mp_alarm_dlg->SetFocus();
	else
	{
		mp_alarm_dlg = new CDialog_Alarm;
		mp_alarm_dlg->Create(this);
		mp_alarm_dlg->ShowWindow(SW_SHOW);
	}
	/* ************************************************************************** */
}

void CMainFrame::OnAbout() 
{
	CDialog_About about_dlg;
	//2012,1015 ybs
	st_handler.mn_menu_lock = FALSE;
	st_handler.mn_system_lock = FALSE;
	
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.
    // **************************************************************************
	if (st_handler.mn_system_lock != FALSE)  return;  // ���� �ý��� Lock ������ ��� ���� 
	// **************************************************************************
	
	int nResponse = about_dlg.DoModal();
	
	if (nResponse == IDOK)
	{
		ShellExecute( NULL, NULL, TEXT("UpdateList.txt"), NULL, TEXT("ReadMe"), SW_SHOW );
	}
}

void CMainFrame::OnReset() 
{
	// TODO: Add your command handler code here
	
}

void CMainFrame::OnLock() 
{
	int n_response;  // ��ȭ ���� ���� �÷���
	
	CDialog_Pass_Check  pass_chk_dlg;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	st_handler.mstr_pass_level=_T("SystemLock");  // ��ȣ ���� ����
	
	n_response = pass_chk_dlg.DoModal();
	
	if (n_response == IDOK)
	{
		::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, FORM_NAME, 2);  // ���� �����ӿ� SYSTEM LOCK ȭ�� ��� ��û
		
		if (st_handler.cwnd_list != NULL)
		{
//			st_msg.mstr_normal_msg = "System Locking.";
			sprintf(st_msg.c_normal_msg, "System Locking.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
	}
	else if (n_response == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}

void CMainFrame::OnExit() 
{
	int mn_response;  // ��ȭ ���ڿ� ���� ���� �� ���� ���� ���� 
	
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	if (st_handler.mn_menu_lock != FALSE)	return;
	if (st_handler.mn_system_lock != FALSE)  return;  // ���� �ý��� Lock ������ ��� ���� 
	
	CDialog_Message msg_dlg;

	// **************************************************************************
	// ���� ��� ���°� STOP �������� �˻�                                       
	// **************************************************************************
	if (COMI.mn_run_status != dSTOP)
	{
		st_msg.str_fallacy_msg = _T("SSD Interface handler is active.");
		mn_response = msg_dlg.DoModal();
		if (mn_response == IDOK)  return ;
	} 
	// **************************************************************************

	st_handler.mn_level_teach =  FALSE;
	st_handler.mn_level_maint =  FALSE;
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
		
	// **************************************************************************
    // ���� ���α׷� ���� ȭ�� ���                                              
    // **************************************************************************
	CDialog_Exit  dlg;
	
	mn_response = dlg.DoModal();
	
	if (mn_response == IDOK)     
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_normal_msg, "[Exit OK]");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}


		st_timedb.mnTime_Section	= dSTOP;		
		st_timedb.mole_date			= COleDateTime::GetCurrentTime();
		
		//g_db.DB_Write_Time(st_timedb);

		All_Stop = 1 ;				// ������ ���� ���� ���� ���� �ʱ�ȭ
		OnMain_Thread_Destroy();	// ������ ���� �Լ�

		OnMain_Port_Close();			// ������ �ø��� ��Ʈ ���� �Լ�
		OnMain_Motor_Destroy();						//��� ���� ���� ����.
		mcls_m_basic.OnDaily_Count_Save();			// Daily Data Save 2K4/08/20/ViboX
		
		OnExitData_Saving();						// ȭ�鿡 ������ ��� ������ ���� �Լ�
		mcls_m_basic.OnDaily_Count_Save();			// Daily Data Save 2K4/08/20/ViboX
		
		// ��� ��ٴ�.
		g_ioMgr.set_out_bit(st_io.o_door_lock,					IO_OFF);

		CView* pOldActiveView = GetActiveView();	// ���� ȭ�鿡 ��µ� �� ���� ���� (save old view)
		
		pOldActiveView->ShowWindow(SW_HIDE);		// ������ ��µ� �並 ����� (hide the old)
		pOldActiveView->DestroyWindow();			// ������ ��µ� �並 �ı���Ų�� (Old View Destroy ��Ű�� ���Ͽ� �߰�)
		pOldActiveView = NULL;						// ������ ��µ� �信 ���� ���� ���� ���� �ʱ�ȭ 
		
		delete	pOldActiveView;						// ������ ��µ� �� ������ �����Ѵ� (kill old view)

		CFrameWnd::OnClose();
	}
}

int CMainFrame::OnMenu_Change_Checking()
{
	int nRet;
	
	nRet = TRUE;  // �ʱ� ���� �� ���� 
	
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	if (st_handler.mn_menu_lock != FALSE)	nRet = FALSE;
	if (st_handler.mn_system_lock != FALSE)  nRet = FALSE;  // ���� �ý��� Lock ������ ��� ���� 
	
	// **************************************************************************
	// ���� ��� ���°� STOP �������� �˻�                                       
	// **************************************************************************
	if (COMI.mn_run_status != dSTOP && COMI.mn_run_status != dLOCK)
	{
//		st_msg.mstr_normal_msg = _T("Handler is active. Stop first.");
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_normal_msg, "Handler is active. Stop first.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		
		nRet = FALSE;
	}
	// **************************************************************************
	
	return nRet;
}

void CMainFrame::OnSwitchToForm(int nForm)
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	if (st_handler.mn_menu_lock != FALSE)	return;
	if (st_handler.mn_system_lock != FALSE)  return;		// ���� �ý��� Lock ������ ��� ���� 

	CView* pOldActiveView = GetActiveView();			// ���� ȭ�鿡 ��µ� �� ���� ���� (save old view)
	CView* pNewActiveView = (CView*)GetDlgItem(nForm);  // ���� ����� �� ���� ���� (get new view)

	if (pNewActiveView == NULL)							// ���� ����� �䰡 ���� ��µ��� �ʾҴ��� �˻� (if it hasn't been created yet)
	{
		switch (nForm)			// �信 ������ ID ���� �� (these IDs are the dialog IDs of the view but can use anything)
		{
		case IDW_SCREEN_MAIN:				// ���� ȭ�� 
			pNewActiveView = (CView*)new CScreen_Main;
			break;
		case IDW_SCREEN_LOCK:				// System Lock ȭ��
			pNewActiveView = (CView*)new CScreen_Lock;
			break;
		case IDW_SCREEN_INIT:				// �ʱ�ȭ ȭ�� 
			pNewActiveView = (CView*)new CScreen_Initial;
			break;
		case IDW_SCREEN_BASIC:				// Basic ȭ��
			pNewActiveView = (CView*)new CScreen_Basic;
			break;
		case IDW_SCREEN_SET_MAINTENANCE:	// ����Ʈ ȭ�� 
			pNewActiveView = (CView*)new CScreen_Set_Maintenance;
			break;
 		case IDW_SCREEN_WAIT_TIME:			// ��� �ð� ȭ�� 
 			pNewActiveView = (CView*)new CScreen_Wait_Time;
 			break;
		case IDW_SCREEN_MOTOR_TEACH:		// Motor Teach ȭ��
			pNewActiveView = (CView*)new CScreen_Motor;
			break;
		case IDW_SCREEN_MOTOR_SPEED:		// Motor Speed ȭ�� 
			pNewActiveView = (CView*)new CScreen_Motor_Speed;
			break;

		case IDW_SCREEN_MOTOR_WITH_IO:
			pNewActiveView = (CView*)new CScreen_Motor_With_IO;
			break;

		case IDW_SCREEN_LIST_OPERATION:		// ���۷��̼� ���� ��� ȭ�� 
			pNewActiveView = (CView*)new CScreen_List_Operation;
			break;
		case IDW_SCREEN_LIST_DATA_VIEW:			// ������ ���� ���� ��� ȭ�� 
			pNewActiveView = (CView*)new CScreen_List_Data_View;
			break;
		case IDW_SCREEN_IO:					// I/O ��� ȭ�� 
			pNewActiveView = (CView*)new CScreen_IO;
			break;

		case IDW_SCREEN_ADMINISTRATOR:		// ADMINISTRATOR ��� ȭ�� 
			pNewActiveView = (CView*)new CScreen_Admin;
			break;
		case IDW_SCREEN_FILE_EDITOR:		// File Editor ��� ȭ�� 
			pNewActiveView = (CView*)new CScreen_File_Editor;
			break;
		case IDW_SCREEN_PGM_INFO:
			pNewActiveView = (CView*)new CScreen_Pgm_Info;
			break;
		}

		CCreateContext context;  // ���ο� �信 ��ť��Ʈ Attach ( attach the document to the new view)
		context.m_pCurrentDoc = pOldActiveView->GetDocument();

		pNewActiveView->Create(NULL, NULL, 0L, CFrameWnd::rectDefault, this, nForm, &context);
		pNewActiveView->OnInitialUpdate();       // ���� ������ ����� ������ �� �κ��� ���ش�.
	}

//  ���� ������ ���.
//	pOldActiveView->ShowWindow(SW_HIDE);	// ������ ��µ� �並 ����� (hide the old)
//	pOldActiveView->DestroyWindow();		// ������ ��µ� �並 �ı���Ų�� (Old View Destroy ��Ű�� ���Ͽ� �߰�)
//	pOldActiveView = NULL;					// ������ ��µ� �信 ���� ���� ���� ���� �ʱ�ȭ 
//	
//	pNewActiveView->OnInitialUpdate();      // �ı��Ǵ� ������ �ٲ� ������ ���� ��µǴ� �信�� ���� ���ؼ� �̸��� �� ����.
//	SetActiveView(pNewActiveView);			// ���� ��µ� �並 ��Ƽ�� ��� ���� (change the active view)
//	pNewActiveView->ShowWindow(SW_SHOW);	// ���ο� �並 ��� (show the new window)
//
//	::SetWindowLong(pNewActiveView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);  // gotta have it
//	RecalcLayout();							// �������� �����Ѵ� (adjust frame)
//	delete	pOldActiveView;					// ������ ��µ� �� ������ �����Ѵ� (kill old view)
//
//  ���� ���.
	SetActiveView(pNewActiveView);			// ���� ��µ� �並 ��Ƽ�� ��� ���� (change the active view)
	pNewActiveView->ShowWindow(SW_SHOW);	// ���ο� �並 ��� (show the new window)

	pOldActiveView->ShowWindow(SW_HIDE);	// ������ ��µ� �並 ����� (hide the old)
	pOldActiveView->DestroyWindow();		// ������ ��µ� �並 �ı���Ų�� (Old View Destroy ��Ű�� ���Ͽ� �߰�)
	pOldActiveView = NULL;					// ������ ��µ� �信 ���� ���� ���� ���� �ʱ�ȭ 

	::SetWindowLong(pNewActiveView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);  // gotta have it
	RecalcLayout();							// �������� �����Ѵ� (adjust frame)
	delete	pOldActiveView;					// ������ ��µ� �� ������ �����Ѵ� (kill old view)
	
	st_handler.mn_menu_num = nForm; 

	// **************************************************************************
	// ���� ȭ�� ���� Ÿ��Ʋ�ٿ� ����Ѵ�.                                       
	// **************************************************************************
	if (st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, FORM_NAME, nForm);
	// **************************************************************************
}

// ******************************************************************************
// ����� ���� �޽����� ���� �� ȭ�� ��ȯ �Լ�                                   
// ******************************************************************************
LRESULT CMainFrame::OnViewChangeMode(WPARAM wParam,LPARAM lParam)
{

	if( GetActiveView() == NULL )
	{
		int a = 1;
		return 0;
	}
	int mn_chg_num = wParam;  // �� ȭ�鿡 ���� ID �� ���� 
	
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
	// -> �˶� ȭ���� ������ ȭ�鿡 ���ؼ��� �޴� ��� ���� ���� �˻�            
    // **************************************************************************
	if (mn_chg_num != 9)  
	{
		if (st_handler.mn_menu_lock != FALSE)  return 0;
		if (st_handler.mn_system_lock != FALSE)  return 0;  // ���� �ý��� Lock ������ ��� ���� 
	}
	// **************************************************************************
	
	switch(mn_chg_num)
	{
	case 1: 
		if (lParam==1)  OnMain();					// ���� ȭ�� ��ȯ 
		else if (lParam==2)							// System Locking ȭ�� ��ȯ 
		{
			if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Lock)))   return 0;
			OnSwitchToForm(IDW_SCREEN_LOCK) ;
		}
		else if (lParam==3)							// �ʱ�ȭ ȭ�� ��ȯ 
		{
			if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Initial)))   return 0;
			OnSwitchToForm(IDW_SCREEN_INIT) ;
		}
		break;
	case 2: 
		OnBasic();									// ������ ȭ�� ��ȯ 
		break;
 	case 3:
 		if (lParam==1)       OnSetMaintenance();	// ����Ʈ ȭ�� ��ȯ 
 		else if (lParam==2)  OnSetInterface();		// Interface ���� ���� ȭ�� ��ȯ
 		break;
 	case 4 :
 		OnWait();
 		break;
 	case 5 : 
 		if (lParam==1)       OnMotTeach();			// ���� ��ġ ȭ�� ��ȯ 
 		else if (lParam==2)	 OnMotSpeed();			// ���� �ӵ� ȭ�� ��ȯ 
		else if( lParam==3)	 OnMotWithIO();
 		break;
	case 6 :
		if (lParam==1)       OnListOper();			// Operation ��� ����Ʈ ȭ�� ��ȯ 
		else if (lParam == 2)OnListDataview();
		break;

	case 7 : 
		if (lParam==1)       OnAlarm();				// �˶� ȭ�� ��ȯ 
		else if (lParam==2)  OnAlarm_Destroy();		// �˶� ȭ�� ���� �Լ�
		break;
	case 8 : 
		if (lParam==1)       OnAdmin();				// Administrator ȭ�� ��ȯ 
		else if (lParam==2)  OnFileEditor();		// File Editor ȭ�� ��ȯ.
		break;
	default:
		break;
	}
	
	return 0 ;
}
// ******************************************************************************

void CMainFrame::OnMain_Path_Set()
{
	CString strTempPath;

// 	CString mstr_basic;			// �⺻ ���� ���� ���� ����+���ϸ� ���� ����
// 	
// 	CString mstr_path_dvc;		// ����̽��� ƼĪ ���� ���� ���� ���� ���� ���� ����
// 	CString mstr_socket_lot;	// ���Ϻ�...
// 	CString mstr_socket_day;	// ���Ϻ�...
// 	CString mstr_socket_month;	// ���Ϻ�...
// 	CString mstr_socket_shift;	// ���Ϻ�...
// 	CString mstr_path_alarm;	// ��� �˶� ���� ���� ���� ���� ����
// 	CString mstr_path_op;		// ���۷����� ���� ���� ���� ���� ����
// 	CString mstr_path_tray;		// Tray�� ���� ���� ���� ���� ���� ���� ����
// 	CString mstr_path_serial;	// Socket ���� ���� ���� ���� ���� ���� ����
// 	
// 	CString mstr_path_file;		// ���� ���� ���� ���� ����
// 	CString mstr_path_back;		// BACKUP ���� ���� ���� ���� ����
// 	
// 	CString mstr_file_basic;	// �⺻ ���� ���� ���� ���ϸ� ���� ����
// 	CString mstr_file_wait;		// ��� �ð� ���� ���� ���ϸ� ���� ����
// 	
// 	CString mstr_operation;		// Operation Log ���� ���� ����.
// 	CString mstr_interface;		// Interface Log ���� ���� ����.
// 	CString mstr_machine;		// Machine Log ���� ���� ����.
// 	CString mstr_barcode;		// Barcode Log ���� ���� ����.
// 	CString mstr_gpib;			// Total Log ���� ���� ����.
// 	CString mstr_total;			// Total Log ���� ���� ����.
// 	
// 	CString mstr_io_map;		// IO MAP ���� ��ġ ����.
// 	
// 	CString mstr_handler;

// [Folder_Data]
// Alarm=C:\\AMT8500\\Alarm\\
// Bmp=C:\\AMT8500\\Bmp\\
// Data=C:\\AMT8500\\Data\\
// Device=C:\\AMT8500\\Device\\
// Log=C:\\AMT8500\\Log\\
// Setting=C:\\AMT8500\\Setting\\;

	CString strAlarmPath;
	CString strBmpPath;
	CString strDataPath;
	CString strDevicePath;
	CString strLogPath;
	CString strSettingPath;
	char chr_data[100];

	CString str_LoadFile;       // �ӽ� ���� ����
	
	FILE    *fp ;
	int     existence;
	
	str_LoadFile = "..\\Setting\\PathInfo.TXT";
// 	str_LoadFile = "C:\\AMT8520\\Setting\\PathInfo.TXT";
	
	existence = access(str_LoadFile, 0);
	
	if (!existence)
	{
		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return;
	}

	:: GetPrivateProfileString("Folder_Data", "Alarm",		"C:\\AMT8520\\Alarm\\", chr_data, 100, ".\\PathInfo.TXT");
	strAlarmPath = chr_data;
	strAlarmPath.TrimLeft(' ');               
	strAlarmPath.TrimRight(' ');

	:: GetPrivateProfileString("Folder_Data", "Bmp",		"C:\\AMT8520\\Bmp\\", chr_data, 100, ".\\PathInfo.TXT");
	strBmpPath = chr_data;
	strBmpPath.TrimLeft(' ');               
	strBmpPath.TrimRight(' ');

	:: GetPrivateProfileString("Folder_Data", "Data",		"C:\\AMT8520\\Data\\", chr_data, 100, ".\\PathInfo.TXT");
	strDataPath = chr_data;
	strDataPath.TrimLeft(' ');               
	strDataPath.TrimRight(' ');

	:: GetPrivateProfileString("Folder_Data", "Device",		"C:\\AMT8520\\Device\\", chr_data, 100, ".\\PathInfo.TXT");
	strDevicePath = chr_data;
	strDevicePath.TrimLeft(' ');               
	strDevicePath.TrimRight(' ');

	:: GetPrivateProfileString("Folder_Data", "Log",		"C:\\AMT8520\\Log\\", chr_data, 100, ".\\PathInfo.TXT");
	strLogPath = chr_data;
	strLogPath.TrimLeft(' ');               
	strLogPath.TrimRight(' ');

	:: GetPrivateProfileString("Folder_Data", "Setting",	"C:\\AMT8520\\Setting\\", chr_data, 100, ".\\PathInfo.TXT");
	strSettingPath = chr_data;
	strSettingPath.TrimLeft(' ');               
	strSettingPath.TrimRight(' ');


	st_path.mstr_basic =			_T(strSettingPath + "BasicTerm.TXT");		// ȯ�� ���� ���� ���� ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_basic);

	st_path.mstr_basic_folder =		_T(strSettingPath);					// ȯ�� ���� ���� ���� ���Ͽ� ���� ���� ���� 

	////2015.0604
	st_path.mstr_scrap_code =			_T(strSettingPath + "RETEST_RULE.TXT");		// ȯ�� ���� ���� ���� ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_scrap_code);
	////
	st_path.mstr_recipe_rule =			_T(strSettingPath + "RECIPE_RULE.TXT");		// ȯ�� ���� ���� ���� ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_recipe_rule);


	st_path.mstr_path_dvc =		_T(strDevicePath);						// ����̽��� ƼĪ ���� ���� ���� ���� ���� ����
	Func.CreateFolder(st_path.mstr_path_dvc);

	st_path.mstr_motor =			_T(strSettingPath + "AMT8520.cme2");

	st_path.mstr_file_basic =		_T("BasicTerm");								// �⺻ ���� ���� ���� ���ϸ� ����
	st_path.mstr_file_wait =		_T("WaitTime");									// ��� �ð� ���� ���� ���ϸ� ����


	//2016.0709
	st_path.mstr_socket_skip = _T(strSettingPath + "SOCKETSKIP_RULE.TXT");

	CString FolderPath;
	
	st_path.mstr_path_alarm =		_T(strBmpPath);									// ��� �˶� ���� ���� ���� ����
	Func.CreateFolder(st_path.mstr_path_alarm);
	
	st_path.mstr_path_file =		_T("C:\\AMT8520\\");							// ���� ���� ���� ����
	st_path.mstr_path_back =		_T("c:\\BackUp\\");									// BACKUP ���� ���� ���� ����
	Func.CreateFolder(st_path.mstr_path_back);
	
	st_path.mstr_operation =		_T(strLogPath + "Operation\\");			// Operation Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_operation);
	
	st_path.mstr_interface =		_T(strLogPath + "Interface\\");			// Interface Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_interface);

	st_path.mstr_tcpip =		_T(strLogPath + "Tcpip\\");			// Interface Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_tcpip);	
	
	st_path.mstr_machine   =		_T(strLogPath + "Machine\\");				// Machine Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_machine);
	
	st_path.mstr_barcode   =		_T(strLogPath + "Barcode\\");				// Machine Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_barcode);
	
	st_path.mstr_total     =		_T(strLogPath + "Total\\");				// Total Log ���Ͽ� ���� ���� ���� 
	Func.CreateFolder(st_path.mstr_total);

	st_path.str_log_path	=		_T(strLogPath + "EqLog\\");	
	Func.CreateFolder(st_path.str_log_path);

	st_path.str_lot_data_path	=	_T(strDataPath);	
	Func.CreateFolder(st_path.str_lot_data_path);

	Func.CreateFolder(strLogPath + "AlarmCodeDebug\\");
	Func.CreateFolder(strAlarmPath + "Daily\\");
	//2013,0306
	Func.CreateFolder(strAlarmPath + "UnderDaily\\");

	st_path.mstr_io_map				= _T(strSettingPath + "AMT8520_IO_MAP.xls");
	st_path.mstr_motor_axis_map		= _T(strSettingPath + "AMT8520_MOTOR_AXIS_MAP.xls");
	if ( g_local.GetLocalType() == LOCAL_ENG ) st_path.mstr_motor_axis_map		= _T(strSettingPath + "AMT8520_MOTOR_AXIS_MAP_ENG.xls");
	st_path.mstr_motor_part_map		= _T(strSettingPath + "AMT8520_MOTOR_PART_MAP.xls");
	if ( g_local.GetLocalType() == LOCAL_ENG ) st_path.mstr_motor_part_map		= _T(strSettingPath + "AMT8520_MOTOR_PART_MAP_ENG.xls");
	st_path.mstr_io_part_map		= _T(strSettingPath + "AMT8520_IO_PART_MAP.xls");
	st_path.mstr_wait_time_map		= _T(strSettingPath + "AMT8520_WAITTIME_MAP.xls");
	if ( g_local.GetLocalType() == LOCAL_ENG ) st_path.mstr_wait_time_map		= _T(strSettingPath + "AMT8520_WAITTIME_MAP_ENG.xls");

	//2017.0602
	if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
	{
		st_path.mstr_recipe_path = _T("C:\\XGEM\\AMT8520_1\\AMT8520\\XWork\\Recipe\\");
	}
	else
	{
		st_path.mstr_recipe_path = _T("C:\\XGEM\\AMT8520\\AMT8520\\XWork\\Recipe\\");
	}
	//2013,1105
	fclose(fp);
}

void CMainFrame::OnMain_Recipe_Data()
{
	char chr_data[100];

	CString str_LoadFile;       // �ӽ� ���� ����
	CString strRecipe;

	int nTotal = 0;
	
	FILE    *fp ;
	int     existence;
	
	str_LoadFile = "C:\\AMT8520\\Setting\\Recipe.ini";
	
	existence = access(str_LoadFile, 0);
	
	if (!existence)
	{
		if ((fp=fopen(str_LoadFile,"rb")) == NULL)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[%s] file open error.", str_LoadFile);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
			}
			st_handler.m_nTotal = 0;
			return ;
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "[%s] file is not exist.", str_LoadFile);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		st_handler.m_nTotal = 0;
		return;
	}

	:: GetPrivateProfileString("Recipe_Data", "Total", "0", chr_data, 100, str_LoadFile);
	nTotal = atoi(chr_data);

	st_handler.m_nTotal = nTotal;

	CString str;
	for (int i = 0; i < nTotal; i++)
	{
		str.Format("%d",i+1);
		:: GetPrivateProfileString("Recipe_Data", str, "0", chr_data, 100, str_LoadFile);
		strRecipe = chr_data;
		strRecipe.TrimLeft(' ');               
		strRecipe.TrimRight(' ');
		if(strRecipe == "") st_handler.m_strRecipe[i] = "AMT8520";
		else                st_handler.m_strRecipe[i] = strRecipe;
	}

	fclose(fp);
}

void CMainFrame::OnMain_Var_Default_Set()
{
	int i = 0, j = 0, mn_chk = 0;
	CString mstr_temp;
	OnMain_Path_Set();
	OnMain_Recipe_Data();

	Func.Load_hifix_mode_Data();//2015.0604

	for (i = 0; i < MAXSITE; i++)
	{
		st_handler.mn_init_state[i] = CTL_NO;
	}

	mp_alarm_dlg = NULL;
	mp_msg_dlg = NULL;

	mn_new_lot_in_step  = 0;
	mn_lot_name_step = 0;

	for (i = 0; i < 3; i++)
	{
		st_handler.mstr_lot_name[i] = "";
	}

	nXgemStep = 0;
	nElectrostaticCheckStep = 0;					// ������ Ȯ�� ���� Ŭ����.
	nElectChannel = 1;						// ������ Ȯ�� ä�� 1~5

	st_ctlbd.n_motorbd_init_end = CTLBD_NO; 
	g_comiMgr.SetMotBoardInit( false );

	CString str_load_device, str_chk_ext;
	int mn_pos;
	char chr_data[50];
	char chr_buf[50];
	
	:: GetPrivateProfileString("BasicData", "Device_Type", "DEFAULT", chr_data, 50, st_path.mstr_basic);
	str_load_device = chr_data;
	str_load_device.TrimLeft(' ');               
	str_load_device.TrimRight(' ');
	
	mn_pos = str_load_device.Find(".");								// Ȯ���� ��ġ �˻�
	if (mn_pos == -1)
		st_basic.mstr_device_name = str_load_device + _T(".TXT");	// Ȯ���� �߰�
	else 
	{
		str_chk_ext = str_load_device.Mid(mn_pos);					// ���� Ȯ���� ����
		if (str_chk_ext != _T(".TXT"))  
			st_basic.mstr_device_name = _T("DEFAULT.TXT");			// �ʱ� ����̽� ���ϸ� ����
		else 
			st_basic.mstr_device_name = str_load_device;				// ����̽� ���ϸ� ����
	}

	:: GetPrivateProfileString("BasicData", "Virtual_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_virtual_mode = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_virtual_mode, chr_buf, 10)) ;
		:: WritePrivateProfileString("BasicData", "Virtual_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_virtual_mode = mn_chk;

	//2013,1028
//	st_handler.mn_virtual_mode = 0;

	:: GetPrivateProfileString("BasicData", "Virtual_Supply", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_virtual_supply = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_virtual_supply, chr_buf, 10)) ;
		:: WritePrivateProfileString("BasicData", "Virtual_Supply", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_virtual_supply = mn_chk;
	
	//2013,1028
	st_handler.mn_virtual_supply = 0;

	:: GetPrivateProfileString("BasicData", "Buffer_In", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_buffer_in = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_buffer_in, chr_buf, 10)) ;
		:: WritePrivateProfileString("BasicData", "Buffer_In", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_buffer_in = mn_chk;

	//2013,1028
	st_handler.mn_buffer_in = 0;

	//2013,0715
	:: GetPrivateProfileString("BasicData", "FullInline_Mode", "0", chr_data, 10, st_path.mstr_basic);
	mn_chk = atoi(chr_data);
	if (mn_chk < 0 || mn_chk > 1)
	{
		st_handler.mn_fullinline_mode = 0;
		mstr_temp = LPCTSTR(_itoa(st_handler.mn_fullinline_mode, chr_buf, 10)) ;
		:: WritePrivateProfileString("BasicData", "FullInline_Mode", LPCTSTR(mstr_temp), st_path.mstr_basic);
	}
	else  st_handler.mn_fullinline_mode = mn_chk;

	//2013,1028
	st_handler.mn_fullinline_mode = 1;

	:: GetPrivateProfileString("BasicData", "PICKER_POSITION_0", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_picker_position[0] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "PICKER_POSITION_1", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_picker_position[1] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "PICKER_POSITION_2", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_picker_position[2] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "PICKER_POSITION_3", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_picker_position[3] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "TRAY_PLACE_OFFSET_0", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_tray_place_offset[0] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "TRAY_PLACE_OFFSET_1", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_tray_place_offset[1] = atof(chr_data);
	
	//2016.0329
	:: GetPrivateProfileString("BasicData", "ROBOT_X_SAFETY", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_safty_rbtx = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "ROBOT_Y_TRAY_SAFETY", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_safty_rbty[0] = atof(chr_data);

	:: GetPrivateProfileString("BasicData", "ROBOT_Y_REJECT_SAFETY", "0", chr_data, 10, st_path.mstr_basic);
	st_handler.md_safty_rbty[1] = atof(chr_data);


	//kwlee 2015.1005
	:: GetPrivateProfileString("FILLATIO", "DIR", "0", chr_data, 100, st_path.mstr_basic); 
	 st_handler.mstr_DirPath =  chr_data;
	st_handler.mstr_DirPath.TrimRight();
	st_handler.mstr_DirPath.TrimLeft();
	///////////////////////

	ReadBasicData_Int( st_handler.mn_retest_cnt, "RETEST_CNT", 1, 0, 9, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_multi_retest_cnt, "MULTI_RETEST_CNT", 1, 0, 9, st_path.mstr_basic );//2015.0518
	ReadBasicData_Int( st_handler.mn_beat_cnt, "BEAT_CNT", 0, 0, 3, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_AutoSocketOff_Cnt, "AUTO_SOCKET_OFF_CNT", 5, 0, 10, st_path.mstr_basic );

	ReadBasicData_Int( st_handler.mn_cokIndex[COKINDEX_GS1], "COKINDEX_GS1", -1, -1, 5, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_cokIndex[COKINDEX_GS2], "COKINDEX_GS2", -1, -1, 5, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_cokIndex[COKINDEX_MSATA], "COKINDEX_MSATA", -1, -1, 5, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_cokIndex[COKINDEX_25_REVERSE], "COKINDEX_25_REVERSE", -1, -1, 5, st_path.mstr_basic );
	ReadBasicData_Int( st_handler.mn_cokIndex[COKINDEX_25_FRONT], "COKINDEX_25_FRONT", -1, -1, 5, st_path.mstr_basic );

	for( i=0; i<COMMON_TEACHING_MAX; i++ )
	{
		CString strKeyName;
		strKeyName.Format( "COMMON_TEACHING_%02d", i );
		ReadBasicData_Double( st_handler.md_common_teaching[i], strKeyName, 0.0f, -5000.0f, 5000.0f, st_path.mstr_basic );
	}

	//2016.0328
	mcls_m_basic.OnBasic_Data_Load();
	mcls_m_basic.On_Teach_Data_Load();

	for (i = 0; i < 3; i++)
	{
		st_handler.mstr_lot_name[i] = "";
	}

////2015.0623
// 	//2014,1205
// 	if(st_motor[MOTOR_ROBOT_Y].d_pos[27] < 30 || st_motor[MOTOR_ROBOT_Y].d_pos[27] > 60)  
// 		st_motor[MOTOR_ROBOT_Y].d_pos[27] = 55;
// 	if(st_motor[MOTOR_ROBOT_Y].d_pos[28] < 30 || st_motor[MOTOR_ROBOT_Y].d_pos[28] > 65)  
// 		st_motor[MOTOR_ROBOT_Y].d_pos[28] = 55;
// 	
// 	st_handler.md_picker_position[0] = 0;
// 	st_handler.md_picker_position[1] = st_motor[MOTOR_ROBOT_Y].d_pos[27];
// 	st_handler.md_picker_position[2] = st_motor[MOTOR_ROBOT_Y].d_pos[27]*2;
// 	st_handler.md_picker_position[3] = st_motor[MOTOR_ROBOT_Y].d_pos[27]*3;
////

	st_NW.mb_buffer_in_received = true;

	st_basic.n_retry_cnt = 3;

	st_handler.mn_emptyout_cnt = 0; //ybs
	st_handler.mn_emptynext_cnt = 0;//2012,1128
	st_handler.mn_emptyin_cnt = 0;//2012,1128
	st_handler.mb_rej_3stacker = FALSE;

	st_handler.m_nfront_smema = CTL_NO;
	st_handler.m_nfront_request = CTL_CLEAR;

	//2013,0307
	st_handler.mn_robot_motorX_safety = CTL_NO;

	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
	}

	//2013,0910
	st_work.m_bPLotDel = false;
	for (i = 0; i < MAX_PLOT; i++)
	{
		st_work.nPLotInputCount[i][0] = 0;
		st_work.nPLotPassCount[i][0] = 0;
		st_work.nPLotFailCount[i][0] = 0;
		st_work.fPLotYield[i][0] = 0;

		st_work.nPLotInputCount[i][1] = 0;
		st_work.nPLotPassCount[i][1] = 0;
		st_work.nPLotFailCount[i][1] = 0;
		st_work.fPLotYield[i][1] = 0;
		
		st_handler.m_tLotR[i] = 0;
	}

	alarm.mn_emo_alarm = FALSE;
	st_handler.m_nRemote = CTL_NO;
	//2016.0117
	st_handler.mn_xgem_connect = CTL_NO;

	//2016.0201
	st_handler.mstr_device_name = "DEFAULT.TXT";

	st_handler.mn_Abort = CTL_NO;



//	pSgem = new CDialog_XgemInterface();
//	ASSERT(pSgem);
//
//	if(!pSgem->Create(IDD_XGEMINTERFACE_DIALOG)){
//	}
//
	//2016.0709
	Func.LoadSocketSkipRule();
}	

void CMainFrame::OnMotTeach() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Motor)))   return;
	OnSwitchToForm(IDW_SCREEN_MOTOR_TEACH);
}

LRESULT CMainFrame::OnMainframe_Work(WPARAM wParam, LPARAM lParam)
{	
	int mn_command_num = wParam;  // ��Ʈ��ũ �۾��� �� ���� ����
	
	switch (mn_command_num)
	{		
	case 2:		// ������ ȭ�鿡�� ������ Ȯ��
		SetTimer(TMR_MAIN_ELEC_CHK, 100, NULL);
		break;
				
	case 1001:
		OnMainFrame_SelectMessageDisplay();
		break;

	case 1002:
		SetTimer(TMR_XGEM_CHK, 500, 0);
		break;

	case 1003:
		FtpConnect();
		FtpFileDown();
		Func.Load_scrap_code_Data();
		break;

	case 1004:
		FtpConnect();
		RecipeRuleFileDown();
		break;
	}
	
	return 0;
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	CString str_display_time;
	int i = 0, n_hour, n_minute, n_second;
	int n_min_temp;	// �� �ð� [��] ���� ���� 
	int n_year, n_month, n_day;				// ��, ��, �� ���� ����

	int nRet;

	int nIndexMin = 0, nIndexSec = 0;
	int nCyclexMin = 0, nCycleSec = 0;

	CString strTimeLog;
	CString mstr_temp;

	CString mStartTime, mEndTime; 
//	char cStartTime[60], cEndTime[60];

	COleDateTime cur_time;
	CTime bin_time;

	if (nIDEvent == TMR_MAIN_REFRESH)
	{
		CTime cur = CTime::GetCurrentTime();
		CTimeSpan diff = cur - st_handler.m_tRef;

		//2015.0915
		st_handler.m_tRef = cur;

		st_handler.m_tDRef = st_handler.m_tDRef + diff;
		for (int i =0; i<TSITE_SOCKET_CNT;i++)
		{
			st_DB_time.n_Db_time[REF_TIME][i] = st_handler.m_tDRef;	
		}

		switch(COMI.mn_run_status)
		{			
		case dRUN://2013,0215
			{
				if(st_work.mn_tl_status == dSELFCHECK)
				{//dSTOP
					st_handler.m_tS = st_handler.m_tS + diff;
					st_handler.m_tDS = st_handler.m_tDS + diff;
					
					////2015.0914
					for (i =0; i<TSITE_SOCKET_CNT;i++)
					{	
						st_handler.mn_InstandyBy_1[i] = CTL_NO;
					}
					/////////////
				}
				else
				{
					st_handler.m_tR = st_handler.m_tR + diff;
					st_handler.m_tDR = st_handler.m_tDR + diff;
					
					for ( i = 0; i < g_lotMgr.GetLotCount(); i++ )//2013,1105
					{
						st_handler.m_tLotR[i] = st_handler.m_tLotR[i] + diff;
					}
					////2015.0914
					for (int i =0; i<TSITE_SOCKET_CNT;i++)
					{
						st_DB_time.n_Db_time[RUN_TIME][i] = st_handler.m_tDR;	
					}
					///////
				}

				////2015.0914
				for (i =0; i<TSITE_INSERT_CNT;i++)
				{	
					if ( /*g_site.GetEnableSocket(i) == true*/
						g_site.GetEnableSocket(i) > 0 && st_handler.mn_InstandyBy_1[i] != CTL_YES && 
						st_handler.isTestRun[i] != CTL_YES &&  Run_Test_Site[0].isOutStandBy[i] != CTL_YES  && g_site.GetModule(i).IsExist() == false)
					{
						st_handler.m_tDRdown[i] = st_handler.m_tDRdown[i] + diff;	
						
						st_DB_time.n_Db_time[RUN_DOWN][i] = st_handler.m_tDRdown[i];
						Run_Test_Site[0].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i] = CTL_NO;
						
					}
					
					if ( /*g_site.GetEnableSocket(i+ 10) == true*/
						g_site.GetEnableSocket(i+ 10) > 0 && st_handler.mn_InstandyBy_1[i + 10] != CTL_YES && 
						st_handler.isTestRun[i + 10] != CTL_YES && Run_Test_Site[1].isOutStandBy[i] != CTL_YES  && g_site.GetModule(i + 10).IsExist() == false)
					{
						st_handler.m_tDRdown[i + 10] = st_handler.m_tDRdown[i + 10] + diff;	
						st_DB_time.n_Db_time[RUN_DOWN][i + 10] = st_handler.m_tDRdown[i + 10];
						
						Run_Test_Site[1].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i + 10] = CTL_NO;
					}
					
 				}
				///////
			}
		break;

		case dWARNING:
		case dJAM:		
			st_handler.m_tJ = st_handler.m_tJ + diff;
			st_handler.m_tDJ = st_handler.m_tDJ + diff;
			//2015.0914
			for (i =0; i<TSITE_INSERT_CNT;i++)
			{	
				if(st_handler.isTestRun[i] != CTL_YES && g_site.GetEnableSocket(i) > 0/*g_site.GetEnableSocket(i) == true*/)
				{
					st_handler.m_tDStop[i] = st_handler.m_tDStop[i] + diff;	
					st_DB_time.n_Db_time[STOP][i] = st_handler.m_tDStop[i];
					
					Run_Test_Site[0].isOutStandBy[i] = CTL_NO;
					st_handler.mn_InstandyBy_1[i] = CTL_NO;
				}
				
				if(st_handler.isTestRun[i + 10] != CTL_YES && g_site.GetEnableSocket(i + 10) > 0 /*g_site.GetEnableSocket(i + 10) == true*/)
				{
					st_handler.m_tDStop[i + 10] = st_handler.m_tDStop[i + 10] + diff;	
					st_DB_time.n_Db_time[STOP][i + 10] = st_handler.m_tDStop[i + 10];
					
					Run_Test_Site[1].isOutStandBy[i] = CTL_NO;
					st_handler.mn_InstandyBy_1[i + 10] = CTL_NO;
				}
			}
			/////////////////////
			break;

		case dLOCK:
			st_handler.m_tM = st_handler.m_tM + diff;
			st_handler.m_tDM = st_handler.m_tDM + diff;
			break;
		
		case dLOTEND:												// Lot End �ÿ��� Stop���� �ð��� �ø���.
		case dSTOP:
			st_handler.m_tS = st_handler.m_tS + diff;
			st_handler.m_tDS = st_handler.m_tDS + diff;

			//2015.0914
			if(st_handler.mn_userstop  == CTL_YES)
			{
				for (i =0; i<TSITE_INSERT_CNT;i++)
				{	
					
					if(st_handler.isTestRun[i] != CTL_YES && g_site.GetEnableSocket(i) > 0/*g_site.GetEnableSocket(i) == true*/)
					{
						st_handler.m_tDUserStop[i] = st_handler.m_tDUserStop[i] + diff;	
						st_DB_time.n_Db_time[USERSTOP][i] = st_handler.m_tDUserStop[i];
						
						Run_Test_Site[0].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i] = CTL_NO;	
					}
					
					if(st_handler.isTestRun[i + 10] != CTL_YES && g_site.GetEnableSocket(i + 10) > 0/*g_site.GetEnableSocket(i + 10) == true*/)
					{
						st_handler.m_tDUserStop[i + 10] = st_handler.m_tDUserStop[i + 10] + diff;	
						st_DB_time.n_Db_time[USERSTOP][i + 10] = st_handler.m_tDUserStop[i + 10];
						
						Run_Test_Site[1].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i + 10] = CTL_NO;	
					}
				}
			}
			else
			{
				for (i =0; i<TSITE_INSERT_CNT;i++)
				{	
					
					if(st_handler.isTestRun[i] != CTL_YES && g_site.GetEnableSocket(i) > 0/*g_site.GetEnableSocket(i) == true*/)
					{
						st_handler.m_tDStop[i] = st_handler.m_tDStop[i] + diff;	
						st_DB_time.n_Db_time[STOP][i] = st_handler.m_tDStop[i];
						
						Run_Test_Site[0].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i] = CTL_NO;
					}
					
					if(st_handler.isTestRun[i + 10] != CTL_YES && g_site.GetEnableSocket(i + 10) > 0/*g_site.GetEnableSocket(i + 10) == true*/)
					{
						st_handler.m_tDStop[i + 10] = st_handler.m_tDStop[i + 10] + diff;	
						st_DB_time.n_Db_time[STOP][i + 10] = st_handler.m_tDStop[i + 10];
						
						Run_Test_Site[1].isOutStandBy[i] = CTL_NO;
						st_handler.mn_InstandyBy_1[i + 10] = CTL_NO;
					}
				}
			}
			/////////////////////
			break;
		}

		///////2015.0914
		for ( int k = 0; k < TSITE_INSERT_CNT; k++ )
		{
			st_handler.mn_Pass[k] = Run_Test_Site[0].mn_Pass[k];
			st_handler.mn_Pass[10+k] = Run_Test_Site[1].mn_Pass[k];
			st_handler.mn_Fail[k] = Run_Test_Site[0].mn_Fail[k];
			st_handler.mn_Fail[10+k] = Run_Test_Site[1].mn_Fail[k];
			st_handler.mn_Input[k] = Run_Test_Site[0].mn_Input[k];
			st_handler.mn_Input[10+k] = Run_Test_Site[1].mn_Input[k];

			if(st_handler.isTestRun[k] == CTL_YES && g_site.GetEnableSocket(k) > 0/*g_site.GetEnableSocket(k) == true*/)
			{
				
				st_handler.m_tDtest[0][k] = st_handler.m_tDtest[0][k] + diff;
				st_DB_time.n_Db_time[TEST_TIME][k] = st_handler.m_tDtest[0][k];
				st_handler.mn_InstandyBy_1[k] = CTL_NO;
			}
			
			
			if(st_handler.isTestRun[k + 10] == CTL_YES && g_site.GetEnableSocket(k+ 10) > 0/*g_site.GetEnableSocket(k+ 10) == true*/)
			{
				st_handler.m_tDtest[1][k] = st_handler.m_tDtest[1][k] + diff;
				st_DB_time.n_Db_time[TEST_TIME][10+k] = st_handler.m_tDtest[1][k];
				st_handler.mn_InstandyBy_1[10+k] = CTL_NO;
			}
			
			if(Run_Test_Site[0].isOutStandBy[k] == CTL_YES && g_site.GetEnableSocket(k) > 0/*g_site.GetEnableSocket(k) == true*/ && st_handler.isTestRun[k] != CTL_YES )
			{
				st_handler.m_tDOutStandby[0][k] = st_handler.m_tDOutStandby[0][k] + diff;
				st_DB_time.n_Db_time[OUTREADY][k] = st_handler.m_tDOutStandby[0][k];
				st_handler.mn_InstandyBy_1[k] = CTL_NO;
			}
			if(Run_Test_Site[1].isOutStandBy[k] == CTL_YES && g_site.GetEnableSocket(k+ 10) > 0/*g_site.GetEnableSocket(k+ 10) == true*/ && st_handler.isTestRun[k + 10] != CTL_YES)
			{
				st_handler.m_tDOutStandby[1][k] = st_handler.m_tDOutStandby[1][k] + diff;
				st_DB_time.n_Db_time[OUTREADY][10+k] = st_handler.m_tDOutStandby[1][k];
				st_handler.mn_InstandyBy_1[10+k] = CTL_NO;
			}
		}
		
		for( int j=0; j<TSITE_SOCKET_CNT; j++ )
		{
			int nSite1 = j/TSITE_INSERT_CNT;
			int nSite2 = j%TSITE_INSERT_CNT;
			
			if( g_site.GetEnableSocket(j) <= 0 /*g_site.GetEnableSocket(j) == false*/)
			{
				st_handler.m_tDSokOff[nSite1][nSite2] = st_handler.m_tDSokOff[nSite1][nSite2] + diff;
				st_DB_time.n_Db_time[SOCKETOFF][j] = st_handler.m_tDSokOff[nSite1][nSite2];
				
				if(st_handler.mn_InstandyBy_1[j] == CTL_YES)
					st_handler.mn_InstandyBy_1[j] = CTL_NO;
			}
			else
			{
				if(st_handler.mn_InstandyBy_1[j] == CTL_YES  && st_handler.isTestRun[j] != CTL_YES)
				{
					st_handler.m_tDInStandby[j] = st_handler.m_tDInStandby[j] + diff;
					st_DB_time.n_Db_time[INSTANBY][j] = st_handler.m_tDInStandby[j];
				}
			}
		}
		IntervalCheck(); //kwlee 2015.1005
		
		if (st_handler.cwnd_main != NULL)
		{//2013,0215
			if(st_work.mn_tl_status == dSELFCHECK)
			{
				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_TIMEINFO, dSTOP);
			}
			else
			{
				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_TIMEINFO, COMI.mn_run_status);
			}
		}
	}
	else if (nIDEvent == TMR_FILE_CREATE)
	{
		// **********************************************************************
		// ���� ���� �� �ʿ��� �ð� �����Ѵ�                                     
		// **********************************************************************
		cur_time = COleDateTime::GetCurrentTime();

		COleDateTimeSpan span( 0, 22, 0, 0 ); // �Ϸ�.
		COleDateTime timeLastDay( m_timeLastDay );
		COleDateTime timeSpan = COleDateTime::GetCurrentTime() - span;
		if( timeSpan >= timeLastDay)
		{
			Func.OnDailyDataSave();
			Func.OnDayDataReset();

			//2015.0914
			st_dbtime.mole_date = COleDateTime::GetCurrentTime();
	
			for (int i =0; i<TSITE_SOCKET_CNT; i++)
			{
				st_dbtime.mn_Ref_time[i] = st_DB_time.n_Db_time[REF_TIME][i];//_DB_time.n_Db_time[REF_TIME][i];
				st_dbtime.mn_run_time[i] =  st_DB_time.n_Db_time[RUN_TIME][i];
				st_dbtime.mn_test_time[i] = st_DB_time.n_Db_time[TEST_TIME][i];
				st_dbtime.mn_lt_Rundown[i] = st_DB_time.n_Db_time[RUN_DOWN][i];
				st_dbtime.mn_lt_Socketoff[i] = st_DB_time.n_Db_time[SOCKETOFF][i];
				st_dbtime.mn_lt_instandby[i] = st_DB_time.n_Db_time[INSTANBY][i];
				st_dbtime.mn_lt_outReady[i] = st_DB_time.n_Db_time[OUTREADY][i];
				st_dbtime.mn_lt_stop[i] = st_DB_time.n_Db_time[STOP][i];
				st_dbtime.mn_lt_User_stop[i] = st_DB_time.n_Db_time[USERSTOP][i];//2015.0723
				
				st_dbtime.mn_Pass[i] = st_handler.mn_Pass[i];
				///
				if(st_handler.mn_Input[i] <= 0)
				{
					st_dbtime.mn_Output[i] = 0; //kwlee 2015.0820
					st_dbtime.md_Yield[i] = 0;
				}
				else
				{	
					st_dbtime.mn_Output[i]	= st_handler.mn_Input[i];//kwlee 2015.0820
					st_dbtime.md_Yield[i] = (double)st_handler.mn_Pass[i]/(double)st_handler.mn_Input[i] * 100.0f;
				}
				
				//Func.DB_Time_Write(st_dbtime,i);	
				
			}
			//////////

			COleDateTimeSpan span2( 1, 0, 0, 0 );
			timeSpan += span2;
			timeSpan.GetAsSystemTime( m_timeLastDay );
			m_timeLastDay.wHour = 0;
			m_timeLastDay.wMinute = 0;
			m_timeLastDay.wSecond = 0;
			m_timeLastDay.wMilliseconds = 0;

			AMTRegistry::RegWriteTime( m_timeLastDay );
		}
		
		n_hour = cur_time.GetHour();
		n_minute = cur_time.GetMinute();
		n_second = cur_time.GetSecond();

		n_year = cur_time.GetYear();  
		n_month = cur_time.GetMonth();  
		n_day = cur_time.GetDay();  

// 		if (n_hour == 21 && n_minute >= 59 && n_second >= 59)
// 		{
// 			Func.OnDailyDataSave();
// 			Func.OnDayDataReset();
// 		}

		n_min_temp = n_hour * 60 + n_minute;

		// ������ Ȯ���� ���� �ð� Ȯ��.
		if (n_second == 0)
		{
			if (n_min_temp % st_basic.mn_electrostatic_check_gap == 0)
			{
				SetTimer(TMR_MAIN_ELEC_CHK, 200, NULL);
			}
		}
	}
	else if (nIDEvent == TMR_MAIN_ELEC_CHK)
	{
		nRet = OnElectrostatic_Check();

		if (nRet == CTLBD_RET_GOOD)
		{
			KillTimer(TMR_MAIN_ELEC_CHK);
		}
		else if (nRet == CTLBD_RET_ERROR)
		{
			KillTimer(TMR_MAIN_ELEC_CHK);
		}
	}
	else if (nIDEvent == TMR_XGEM_CHK)
	{
		nRet = OnXgmInterface();
		if (nRet == CTLBD_RET_GOOD)
		{
			KillTimer(TMR_XGEM_CHK);
		}
		else if (nRet == CTLBD_RET_ERROR)
		{
			KillTimer(TMR_XGEM_CHK);
		}
	}	
	CFrameWnd::OnTimer(nIDEvent);
}

int CMainFrame::OnElectrostatic_Check()
{
	int nFunRet = CTLBD_RET_PROCEED;
	int i;

	// �ø��� ��Ʈ�� �������� �ʾҴٸ� �׳� �н�...
	if (st_handler.mnSerial_Port_Creating[GMS_PORT-1] != CTL_YES)
	{
		return CTLBD_RET_GOOD;
	}
	
	switch (nElectrostaticCheckStep)
	{
	case 0:
		strElectCommand.Format("%c%cP%-8sCH%d %c", 0x10, 0x02, "SAMSUNG1", nElectChannel, 0x03);
		st_serial.mstr_snd[GMS_PORT-1] = strElectCommand;
		st_work.mn_elec_serial_work = CTL_READY;
		lElectSerialTime[0] = GetCurrentTime();
		OnDataSend(GMS_PORT, 0);
		nElectrostaticCheckStep = 100;
		break;
		
	case 100:
		if (st_work.mn_elec_serial_work == CTL_YES)				// ���� �Ϸ�
		{
			nElectrostaticCheckStep = 200;
		}
		else if (st_work.mn_elec_serial_work == CTL_NO)			// ������ �Ϸ�
		{
			nFunRet = CTLBD_RET_ERROR;
			nElectrostaticCheckStep = 0;
		}
		else													// �̵����� �ƴҶ�...
		{
			lElectSerialTime[1] = GetCurrentTime();
			lElectSerialTime[2] = lElectSerialTime[1] - lElectSerialTime[0];
			
			if (lElectSerialTime[2] > 100000)					// 10�� ���
			{
				nFunRet = CTLBD_RET_ERROR;
				nElectrostaticCheckStep = 0;
			}
			else if(lElectSerialTime[2] < 0)//2013,0608
			{
				lElectSerialTime[0] = GetCurrentTime();
			}
		}
		break;
		
	case 200:
		if (nElectChannel == 5)									// ������ ä�α��� ������...
		{
			nElectChannel = 1;									// �����ϰ�.
			nElectrostaticCheckStep = 300;
		}
		else if (nElectChannel < 5)
		{
			nElectChannel++;
			nElectrostaticCheckStep = 0;
		}
		break;
		
	case 300:
		st_grounddb.mole_date = COleDateTime::GetCurrentTime();
		//Func.DB_Write_Ground(st_grounddb);			// �����Ѵ�.
		
		for (i = 0; i < 5; i++)
		{
			if (st_work.mdReadElectData[i]  > st_basic.md_electrostatic_value)		// ������ + ������ �⺻�� ���� ũ��
			{
				nFunRet = CTLBD_RET_ERROR;
				nElectrostaticCheckStep = 0;
				break;
			}
		}
		
		// �����̴ϱ� ����.
		for (i = 0; i < 5; i++)
		{
			st_work.mdReadElectData[i] = 0;
			st_grounddb.mstrChannel[i] = "";
		}
		
		nElectrostaticCheckStep = 0;
		nFunRet = CTLBD_RET_GOOD;
		break;
	}
	
	return nFunRet;
}

void CMainFrame::OnMotSpeed() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Motor_Speed)))   return;
	OnSwitchToForm(IDW_SCREEN_MOTOR_SPEED);	
}

void CMainFrame::OnSetInterface() 
{
// 	// **************************************************************************
//     // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
//     // **************************************************************************
// 	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
// 	if (nmenu_chk != TRUE)  return;
// 	
// 	// **************************************************************************
// 	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
// 	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
// 	// **************************************************************************
// 	if (st_handler.mn_level_teach !=  TRUE)
// 	{
// 		if (st_handler.mn_level_maint !=  TRUE) 
// 		{
// 			OnLevel();
// 			if (st_handler.mn_level_teach !=  TRUE)
// 			{
// 				if (st_handler.mn_level_maint !=  TRUE)  return;
// 			}
// 		}
// 	}
// 	// **************************************************************************
// 	
// 	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Set_Interface)))   return;
// 	OnSwitchToForm(IDW_SCREEN_SET_INTERFACE);	
}

void CMainFrame::OnSetMaintenance() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Set_Maintenance)))   return;
	OnSwitchToForm(IDW_SCREEN_SET_MAINTENANCE);	
}

void CMainFrame::OnAdmin() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Admin)))   return;
	OnSwitchToForm(IDW_SCREEN_ADMINISTRATOR);
}



void CMainFrame::OnMain_Thread_Creating()
{
	// **********************************************************************
	// ����ġ �˻� ���� ���� ������ �����Ѵ�                                 
	// **********************************************************************
	m_thread[THREAD_HANDLER_CHK]=AfxBeginThread(OnThread_Handler_Check, this);
	if (m_thread[THREAD_HANDLER_CHK] != NULL) 	
		hThrHandle[THREAD_HANDLER_CHK] = m_thread[THREAD_HANDLER_CHK]->m_hThread;
	// **********************************************************************

	// **********************************************************************
	// Ÿ�� ���� ��� ���� ���� ������ �����Ѵ�                              
	// **********************************************************************
	m_thread[THREAD_TOWER_LAMP_CHK]=AfxBeginThread(OnThread_Tower_Lamp_Check, this);
	if (m_thread[THREAD_TOWER_LAMP_CHK] != NULL) 	
		hThrHandle[THREAD_TOWER_LAMP_CHK] = m_thread[THREAD_TOWER_LAMP_CHK]->m_hThread;
	// **********************************************************************
	
	// **********************************************************************
	// ��� ���� �� �߻��� �˶� ����ϱ� ���� ������ �����Ѵ�                
	// **********************************************************************
	m_thread[THREAD_ALARM_DISPLAY]=AfxBeginThread(OnThread_Alarm_Display, this);
	if (m_thread[THREAD_ALARM_DISPLAY] != NULL) 	
		hThrHandle[THREAD_ALARM_DISPLAY] = m_thread[THREAD_ALARM_DISPLAY]->m_hThread;
	// **********************************************************************	

	// **********************************************************************
	//  IO ������ ���� �����带 �����Ѵ�.                         
	// **********************************************************************
	m_thread[THREAD_IO]=AfxBeginThread(OnThread_IO,this);
	if (m_thread[THREAD_IO] != NULL) 	
		hThrHandle[THREAD_IO] = m_thread[THREAD_IO]->m_hThread;
	// **********************************************************************

	m_thread[THREAD_NETWORK] = AfxBeginThread( OnThread_Network, this );
	if( m_thread[THREAD_NETWORK] != NULL )
		hThrHandle[THREAD_NETWORK] = m_thread[THREAD_NETWORK]->m_hThread;

	m_thread[THREAD_MOTORS] = AfxBeginThread( OnThread_Motors, this );
	if( m_thread[THREAD_MOTORS] != NULL )
		hThrHandle[THREAD_MOTORS] = m_thread[THREAD_MOTORS]->m_hThread;

}

void CMainFrame::OnMain_Thread_Destroy()
{
	for( int i=0; i<THREAD_COUNT; i++ )
	{
		if (hThrHandle[i])  // ������ ����
		::WaitForSingleObject(hThrHandle[i], 1500);
	}
}

void CMainFrame::OnMain_Port_Create()
{
	int i;
	DWORD dwCommEvents[MAX_PORT];

	for (i = 0; i < MAX_PORT; i++)
	{		
		if (st_handler.mnSerial_Port_Creating[i] == CTL_NO)		// �����Ǿ� ���� ��������, �����Ѵ�.
		{
			// **************************************************************************
			//  ��Ʈ �����Ѵ�                                                    
			// **************************************************************************
			dwCommEvents[i] = m_ports[i].MmdSerialGetCommEvents();  // �ø��� �̺�Ʈ ����
			dwCommEvents[i] = 0;
			dwCommEvents[i] |= EV_CTS;
			dwCommEvents[i] |= EV_RXCHAR;
			
			if (m_ports[i].MmdSerialInitializePort(this, i + 1, 9600, 78, 8, 1, 1024, dwCommEvents[i])) 
				m_ports[i].MmdSerialStartMonitoring();
			else 
				st_serial.mstr_port_chk[i] = "NOT FOUND";
			
			mstr_received[i].Empty();  // ���� �޽��� �ӽ� ���� ���� �ʱ�ȭ
			// **************************************************************************
			
			if (st_serial.mstr_port_chk[i] == "NOT FOUND")
			{
				st_handler.mnSerial_Port_Creating[i] = CTL_READY;
			}
			else
			{
				st_handler.mnSerial_Port_Creating[i] = CTL_YES;
			}
		}
	}
}

void CMainFrame::OnExitData_Saving()
{
	st_handler.mn_menu_lock = TRUE;
	
	if (st_handler.cwnd_basic != NULL)				// BASIC ȭ�� ����
	{
		st_handler.cwnd_basic->SendMessage(WM_BASIC_APPLY, 0, 0);					// BASIC DATA APPLY ��û
	}
	else if (st_handler.cwnd_maintenance != NULL)	// Tower Lamp ȭ�� ����
	{
		st_handler.cwnd_maintenance->SendMessage(WM_MAINTENANCE_APPLY, 0, 0);		// Tower Lamp DATA APPLY ��û
	}
	else if (st_handler.cwnd_testreference != NULL)	// Test Reference ȭ�� ����
	{
		st_handler.cwnd_testreference->SendMessage(WM_TESTREFERENCE_APPLY, 0, 0);	// Test Reference DATA APPLY ��û
	}
	else if (st_handler.cwnd_waittime != NULL)		// Wait Time ȭ�� ����
	{
		st_handler.cwnd_waittime->SendMessage(WM_WAITTIME_APPLY, 0, 0);				// Wait Time DATA APPLY ��û
	}
	else if (st_handler.cwnd_motorspeed != NULL)	// Motor Speed ȭ�� ����
	{
		st_handler.cwnd_motorspeed->SendMessage(WM_MOTORSPEED_APPLY, 0, 0);			// Motor Speed DATA APPLY ��û
	}
	
	/* ************************************************************************** */
    /* ȭ�鿡 ������ ��� ������ ���Ͽ� �����Ѵ�                                  */
	/* -> ���α׷� ���� �������� ���۷����Ͱ� ������ ���� ���� �����ϴ� ��� ��� */
	/* -> ��� ���� �߿� ���� �����Ǵ��� ���� ���� ���� �����ȴ�                */
    /* ************************************************************************** */
	mcls_m_basic.OnBasic_Data_Save();				// ���� ������ ����� Basic ���� ������ ���Ͽ� �����ϴ� �Լ�
	mcls_m_basic.OnWaitTime_Data_Save();			// ���� ������ ����� Wait Time ���� ������ ���Ͽ� �����ϴ� �Լ�
	mcls_m_basic.OnMotorSpeed_Set_Data_Save();		// ���� ������ ����� Motor Speed ���� ������ ���Ͽ� �����ϴ� �Լ�.
	mcls_m_basic.On_Teach_Data_Save();
	mcls_m_basic.OnMaintenance_Data_Save();

	////2015.0623
	st_handler.md_picker_position[0] = 0;//Robot Picker Position -> Tray Pitch
	st_handler.md_picker_position[1] = st_motor[MOTOR_ROBOT_Y].d_pos[27];
	st_handler.md_picker_position[2] = st_motor[MOTOR_ROBOT_Y].d_pos[27]*2;
	st_handler.md_picker_position[3] = st_motor[MOTOR_ROBOT_Y].d_pos[27]*3;
	
	CString strKey;
	CString str_temp;
	int i;
	for(i=0; i<4; i++)
	{
		strKey.Format( "PICKER_POSITION_%d", i );
		str_temp.Format( "%.03f", st_handler.md_picker_position[i] );
		:: WritePrivateProfileString("BasicData", strKey, LPCTSTR(str_temp), st_path.mstr_basic);
	}
	////

	st_handler.mn_menu_lock = FALSE;
}

void CMainFrame::OnMain_Port_Close()
{

}

void CMainFrame::OnMain_Motor_Destroy()
{

}

LRESULT CMainFrame::OnMainMessageEvent(WPARAM wParam, LPARAM lParam)
{
	int i = 0;
	CString strTemp;
	
	if (wParam == CTL_YES)
	{
		if (mp_msg_dlg != NULL && IsWindow(mp_msg_dlg->m_hWnd))
		{
			mp_msg_dlg->SetFocus();	// ��ȭ���ڸ� Ȱ��ȭ
			mp_msg_dlg->OnEventMsg_Text_Set();
		}
		else
		{
			mp_msg_dlg = new CDialog_Event_Msg( st_msg.mstr_event_msg[0], st_msg.mstr_event_msg[1], st_msg.mstr_event_msg[2]);
			//2017.0201
			//mp_msg_dlg = new CDialog_Event_Msg;
			mp_msg_dlg->Create();
			mp_msg_dlg->ShowWindow(SW_SHOW);
		}
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);

	}
	else if (wParam == CTL_NO)
	{
		for (i = 0; i < 3; i++)
		{
			st_msg.mstr_event_msg[i] = "";
		}
		
		if (mp_msg_dlg != NULL && IsWindow(mp_msg_dlg->m_hWnd))
		{
			mp_msg_dlg->ShowWindow(SW_HIDE);
			mp_msg_dlg->DestroyWindow();
			delete mp_msg_dlg;
			mp_msg_dlg = NULL;
		}
		
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, CTL_OFF);
	}
	
	return 0;
}

void CMainFrame::OnMainFrame_SelectMessageDisplay()
{
	int ReturnVal;
	
	CDialog_Select select_dlg;
	
	st_msg.mstr_confirm_msg = _T(st_handler.mstrSelectMessage);
	ReturnVal = select_dlg.DoModal();
	
	if (ReturnVal == IDOK)
	{
		st_handler.mnSelectMessage = 1;
	}
	else
	{
		st_handler.mnSelectMessage = 2;
	}
}

// ******************************************************************************
// RS-232C �ø��� ��Ʈ ���� �Լ�                                                 
// ******************************************************************************
LONG CMainFrame::OnCommunication(WPARAM port, LPARAM ch)
{
	// **************************************************************************
    // RS-232C �ø��� ��Ʈ �˻��Ѵ�                                              
    // **************************************************************************
	if (port <= 0 || port > MAX_PORT)  // COMM ��Ʈ �˻�
		return -1;
	// **************************************************************************
    
	if (ch == -1)  // �б� ����
	{
		if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			st_serial.mcomm_err[port-1] = _T("[���� ������] �ø��� ��Ʈ �б� ���� �߻�");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, port, RECEIVE_MSG);  // ���� �޽��� ��� ��û
		}

		st_serial.mn_rec_chk[port-1] = COM_ERROR;  // ���� ���� �÷��� ����
		return 0;
	}
	else if (ch == -2)  // ���� ����
	{
		if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			st_serial.mcomm_err[port-1] = _T("[���� ������] �ø��� ��Ʈ ���� ���� �߻�");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, port, SEND_MSG);  // ���� �޽��� ��� ��û
		}

		st_serial.mn_rec_chk[port-1] = COM_ERROR;  // ���� ���� �÷��� ����
		return 0;
	}

	if (port == GMS_PORT)
	{
		OnMain_Rcv_GMS(port, ch);
	}

	return 0;
}
// ******************************************************************************

// ******************************************************************************
// RS-232C �ø��� ��Ʈ �̺�Ʈ ���� �Լ�                                          
// ******************************************************************************
LONG CMainFrame::OnCommunicationEvent(WPARAM port, LPARAM event)
{
	// **************************************************************************
    // �̺�Ʈ ������ �ø��� ��Ʈ ��Ʈ �˻��Ѵ�                                   
    // **************************************************************************
	if (port <= 0 || port > MAX_PORT)  // COMM ��Ʈ �˻�
		return -1;
	// **************************************************************************

	if (event & EV_BREAK)
		Func.MsgLog ("EV_BREAK event");
	else if (event & EV_CTS)
		Func.MsgLog ("EV_CTS event");
	else if (event & EV_DSR)
		Func.MsgLog ("EV_DSR event");
	else if (event & EV_ERR)
		Func.MsgLog ("EV_ERR event");
	else if (event & EV_RING)
		Func.MsgLog ("EV_RING event");
	else if (event & EV_RLSD)
		Func.MsgLog ("EV_RLSD event");
	else if (event & EV_RXCHAR)
		Func.MsgLog ("EV_RXCHAR event");
	else if (event & EV_RXFLAG)
		Func.MsgLog ("EV_RXFLAG event");
	else if (event & EV_TXEMPTY)
		Func.MsgLog ("EV_TXEMPTY event");

	return 0;
}
// ******************************************************************************

// ******************************************************************************
// RS-232C �ø��� ��Ʈ�� ���� �޽��� ���� �Լ�                                   
// -> wParam : �޽��� ������ ��Ʈ �ѹ�                                           
// ******************************************************************************
LRESULT CMainFrame::OnDataSend(WPARAM wParam, LPARAM lParam)
{	
	int port = wParam; 
	int pos = lParam;
	int i = 0;
	CString data, temp;
	
	switch(port)
	{
		case GMS_PORT:
			OnMain_Snd_Serial(wParam, lParam);
			break;
	}
	return 0;
}

void CMainFrame::OnMain_Snd_Serial(WPARAM wParam, LPARAM lParam)
{
	int n_serial_chk;  // ������ �۽� �÷���
	
	char buf[1024];
	
	// **************************************************************************
	// �۽� ������ �ӽ� ���� ���� �ʱ�ȭ                                         
	// **************************************************************************
	memset(&buf, 0, sizeof(buf));
	//buf[0] = '\0';
	// **************************************************************************
	
	// **************************************************************************
	// �۽� ������ �� �������� �� ����/���� ���� ���� �����Ѵ�                 
	// **************************************************************************
	//(st_serial.str_snd[wParam-1]).TrimLeft(' ');
	//(st_serial.str_snd[wParam-1]).TrimRight(' ');
	// **************************************************************************
	
	// **************************************************************************
	// �۽� ������ ������ ���� �˻��Ѵ�                                          
	// **************************************************************************
	if (st_serial.mstr_snd[wParam-1] == _T(""))  // �۽� ������ ���� �˻�
	{
		if (st_handler.cwnd_list != NULL)       // ����Ʈ �� ȭ�� ����
		{
			st_serial.mcomm_err[wParam-1] = _T("[�۽� ������] ���� ������ ���� ���� �߻�");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, wParam, SEND_MSG);  // ���� �޽��� ��� ��û
		}
		
		st_serial.mn_snd_chk[wParam-1] = COM_ERROR;  // �۽� ���� �÷��� ����
		return;
	}
	// **************************************************************************
	
	st_serial.mcomm_snd[wParam-1] = st_serial.mstr_snd[wParam-1];  
	sprintf(buf, st_serial.mstr_snd[wParam-1]);  // �۽� ������ ����
	
	// **************************************************************************
	// �Էµ� �۽� ������ �ø��� ��Ʈ�� ���� �����Ѵ�                            
	// **************************************************************************
	n_serial_chk = (m_ports[wParam-1]).MmdSerialWriteData(buf, strlen(buf), SERIAL_WAIT, SERIAL_TIME_INFINITE);
	
	if (n_serial_chk == 0)        // ���� �۽�
	{
		//if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		//    st_handler.cwnd_list->PostMessage(WM_LIST_DATA, wParam, 2);  // �۽� �޽��� ��� ��û
		
		st_serial.mn_snd_chk[wParam-1] = CTL_YES;
	}
	else if (n_serial_chk == -1)  // ���� ����
	{
		if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			st_serial.mcomm_err[wParam-1] = _T("[�۽� ������] �ø��� ��Ʈ ���� ���� �߻�");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, wParam, SEND_MSG);  // ���� �޽��� ��� ��û
		}
		
		st_serial.mn_snd_chk[wParam-1] = -1;  
	}
	else if (n_serial_chk == -2)  // TIME OUT
	{
		if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
		{
			st_serial.mcomm_err[wParam-1] = _T("[�۽� ������] �ø��� ��Ʈ TIME OUT �߻�");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, wParam, SEND_MSG);  // ���� �޽��� ��� ��û
		}
		
		st_serial.mn_snd_chk[wParam-1] = -2;
	}
	// **************************************************************************
}


void CMainFrame::OnMain_Rcv_GMS(WPARAM port, LPARAM ch)
{
	CString str_check;
	int nRet;
	
	if (ch == 0x03)  // ������ ������ �˻�
	{
		mstr_received[port-1] += (char)ch;
		mstr_received[port-1].TrimLeft(' ');
		mstr_received[port-1].TrimRight(' ');
		
		st_serial.mstr_rec[port-1] = mstr_received[port-1];
		
//		st_msg.mstr_normal_msg = mstr_received[port-1];
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_normal_msg, mstr_received[port-1]);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}
		
		(mstr_received[port-1]).Empty();
		
		nRet = Func.OnElect_CheckData(st_serial.mstr_rec[port-1]);
		
		if (nRet == CTLBD_RET_GOOD)
		{
			st_work.mn_elec_serial_work = CTL_YES;		// �������� �亯�� �Դ�.
			st_serial.mn_rec_chk[port-1] = TRUE;
		}
		else if (nRet == CTLBD_RET_ERROR)			// ���̰� �ȸ¾Ƽ� ����.
		{
			st_work.mn_elec_serial_work = CTL_NO;		// ���������� �亯�� �Դ�.
			st_serial.mn_rec_chk[port-1] = FALSE;
		}
		
		st_serial.mcomm_rec[port-1] = st_serial.mstr_rec[port-1];  // ����Ʈ ȭ�鿡 ����� ���� ������ ����
	}
	else
	{
		mstr_received[port-1] += (char)ch;
	}
}	

void CMainFrame::OnClose() 
{
	OnExit();
}

void CMainFrame::OnDestroy() 
{
	/* ************************************************************************** */
	/* ���� ��޸��� �˶� ȭ�� �������� �ʾҴٸ� �� ���� �۾�                     */
	/* -> mp_modal_dlg : �˶� ȭ�鿡 ���� ������ ���� ����                        */
	/* ************************************************************************** */
	if (mp_alarm_dlg)
	{
		OnAlarm_Destroy();			// �˶� ȭ�� ���� �Լ� */
		delete this;
	}
	/* ************************************************************************** */
	
	OnMain_Port_Close();			// ������ �ø��� ��Ʈ ���� �Լ�
	
	KillTimer(TMR_MAIN_REFRESH);	// Time ��� Ÿ�̸� ����
	KillTimer(TMR_FILE_CREATE);

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnAlarm_Destroy()
{
	if (mp_alarm_dlg)
	{
		if (mp_alarm_dlg->IsWindowVisible())
		{
			mp_alarm_dlg->DestroyWindow();
		}
		
		if (st_handler.mn_alarm_screen != FALSE)  // �˶� ȭ�� ���
			st_handler.mn_alarm_screen = FALSE;
	}
}


void CMainFrame::OnListOper() 
{
	/* ************************************************************************** */
    /* ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                       */
    /* ************************************************************************** */
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	/* ************************************************************************** */
	/* �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                               */
	/* -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                       */
	/* ************************************************************************** */
	//	if (st_ICT.n_level_teach !=  TRUE)
	//	{
	//		if (st_ICT.n_level_maint !=  TRUE)  return;
	//	}
	/* ************************************************************************** */
	
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_List_Operation)))   return;
	OnSwitchToForm(IDW_SCREEN_LIST_OPERATION);
}

void CMainFrame::OnListDataview() 
{
	/* ************************************************************************** */
    /* ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                       */
    /* ************************************************************************** */
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	/* ************************************************************************** */
	/* �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                               */
	/* -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                       */
	/* ************************************************************************** */
	//	if (st_ICT.n_level_teach !=  TRUE)
	//	{
	//		if (st_ICT.n_level_maint !=  TRUE)  return;
	//	}
	/* ************************************************************************** */
	
	if(GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_List_Data_View)))   return;
	OnSwitchToForm(IDW_SCREEN_LIST_DATA_VIEW);
}

void CMainFrame::OnFileEditor() 
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_File_Editor)))   return;
	OnSwitchToForm(IDW_SCREEN_FILE_EDITOR);	
}

void CMainFrame::OnMotWithIO()
{
	// **************************************************************************
    // ȭ�� �� ��ȯ �Ұ����� ���� �˻��Ѵ�.                                      
    // **************************************************************************
	int nmenu_chk = OnMenu_Change_Checking(); // �޴� ��� ���� ���� �˻� �Լ�
	if (nmenu_chk != TRUE)  return;
	
	// **************************************************************************
	// �޴� ��� �Ұ����� ��쿡 ���� ���� �˻��Ѵ�                              
	// -> ����Ʈ ��� �Ǵ� ƼĪ ��尡 �����Ǿ� �ִ��� �˻�                      
	// **************************************************************************
	if (st_handler.mn_level_teach !=  TRUE)
	{
		if (st_handler.mn_level_maint !=  TRUE) 
		{
			OnLevel();
			if (st_handler.mn_level_teach !=  TRUE)
			{
				if (st_handler.mn_level_maint !=  TRUE)  return;
			}
		}
	}
	// **************************************************************************
	
	if (GetActiveView()->IsKindOf(RUNTIME_CLASS(CScreen_Motor_With_IO)))   return;
	OnSwitchToForm(IDW_SCREEN_MOTOR_WITH_IO);	
}


int CMainFrame::OnXgmInterface()
{
	int nFunRet = CTLBD_RET_PROCEED;

	switch(nXgemStep)
	{
	case 0:
		nXgemStep = 100;
		break;

	case 100:
		if( st_basic.mn_mode_xgem == CTL_YES )
		{
			CWnd* m_pWnd = CWnd::FindWindow(NULL,"1");
			CWnd* m_pWnd1 = CWnd::FindWindow(NULL,"XgemInterface");
			if(m_pWnd){
				//������ Program�� Ȱ��ȭ ��Ų��.
				m_pWnd->SetForegroundWindow(); 	
				m_pWnd->ShowWindow(SW_RESTORE);	
//				nFunRet = CTLBD_RET_GOOD;
//				lXgemTime[0] = GetCurrentTime();
//				nXgemStep = 200;
				//2016.0117
				lXgemTime[0] = GetCurrentTime();
				nXgemStep = 500;
			}
			else if(m_pWnd1){
				//������ Program�� Ȱ��ȭ ��Ų��.
				m_pWnd1->SetForegroundWindow(); 	
				m_pWnd1->ShowWindow(SW_RESTORE);	
//				nFunRet = CTLBD_RET_GOOD;
//				lXgemTime[0] = GetCurrentTime();
//				nXgemStep = 200;
				//2016.0117
				lXgemTime[0] = GetCurrentTime();
				nXgemStep = 500;
			}
			else{
				
				//WinExec("C:\\AMT8520\\bin\\XgemInterface.exe",SW_SHOW);
				::ShellExecute(NULL, NULL, "C:\\AMT8520\\bin\\XgemInterface.exe", NULL, NULL, SW_SHOWNORMAL);
				//2016.0117
				lXgemTime[0] = GetCurrentTime();
				nXgemStep = 200;
			}
			//2016.0117
// 			lXgemTime[0] = GetCurrentTime();
// 			nXgemStep = 200;
		}
		else
		{
			nFunRet = CTLBD_RET_ERROR;
			nXgemStep = 0;
		}
		break;
		

	case 200:
		if(GetCurrentTime() - lXgemTime[0] > 7000) nXgemStep = 500;
		else if(GetCurrentTime() - lXgemTime[0] < 0 ) lXgemTime[0] = GetCurrentTime();

		break;

	case 300:
		g_client_xgem.SetGemInit();
		lXgemTime[0] = GetCurrentTime();
		nXgemStep = 400;
		break;

	case 400:
		if(GetCurrentTime() - lXgemTime[0] > 2000) nXgemStep = 500;
		else if(GetCurrentTime() - lXgemTime[0] < 0 ) lXgemTime[0] = GetCurrentTime();
		break;

	case 500:
		Func.OnSavePPID();
		lXgemTime[0] = GetCurrentTime();
		nXgemStep = 600;
		break;

	case 600:
		if(GetCurrentTime() - lXgemTime[0] > 2000) nXgemStep = 1000;
		else if(GetCurrentTime() - lXgemTime[0] < 0 ) lXgemTime[0] = GetCurrentTime();
		break;
		
	case 1000:
		g_client_ec.OnSendToolInform(true);
		nFunRet = CTLBD_RET_GOOD;
		nXgemStep = 0;
		break;
	}


	return nFunRet;

}

//2015.0914
BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	///// 3-30
	COPYDATASTRUCT MyCDS;
	st_dbtime_database st_dbtime;
	

	int year_s, year_e, month_s, month_e, day_s, day_e;
	int hour_s, hour_e, min_s, min_e, sec_s, sec_e;

	int ns_year, ne_year, ns_month, ne_month, ns_day, ne_day;
	int ns_hour, /*ne_hour,*/ ns_min, /*ne_min,*/ ns_sec/*, ne_sec*/;

	CString csYear_s,csMonth_s,csDay_s;
	CString csYear_e,csMonth_e,csDay_e;
	CString strRecvLog;
	int count;
	int nStart,nEnd;

//	int nInterval;
	int i;//, j;
	COleDateTime nTime[2];
	
	CString strStartDate,strEndDate,strDate;
	
	COleDateTime time;
	COleDateTime Stime;
	COleDateTime Etime;
	COleDateTime Day;
	time = COleDateTime::GetCurrentTime();
	

	switch( pCopyDataStruct->dwData)
	{ 

		case WM_DAY3_DATA:
			{
				g_cs.EnterCriticalSection();

				if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
				{
					Stime = time + COleDateTimeSpan(-3, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
				}
				else
				{
					Stime = time + COleDateTimeSpan(-3, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
				}

				year_s		= Stime.GetYear();
				month_s		= Stime.GetMonth();
				day_s		= Stime.GetDay();
				
				hour_s		= Stime.GetHour();
				min_s		= Stime.GetMinute();
				sec_s		= Stime.GetSecond();
				
				year_e		= Etime.GetYear();
				month_e		= Etime.GetMonth();
				day_e		=Etime.GetDay();
				
				hour_e		= Etime.GetHour();
				min_e		= Etime.GetMinute();
				sec_e		= Etime.GetSecond();
				
				nTime[0].SetDateTime(year_s,month_s,day_s,22,0,0);
				nTime[1].SetDateTime(year_e,month_e,day_e,22,0,0);

				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
						count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
						
					}
				}

				st_dbTimeTmp = new st_dbTimeTmp_database[count];
	
				for (i = 0; i<count; i++)
				{
					DB_Data_Read(i);	
				}
			
				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}

				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");

				//2016.0425
				strRecvLog = _T("FILL RATIO UI");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);


				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}

				delete[] st_dbTimeTmp; 

				Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}
		
			break;

		case WM_DAY7_DATA:
			{
				g_cs.EnterCriticalSection();
				
				if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
				{
					Stime = time + COleDateTimeSpan(-7, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
				}
				else
				{
					Stime = time + COleDateTimeSpan(-7, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
				}
				
				year_s		= Stime.GetYear();
				month_s		= Stime.GetMonth();
				day_s		= Stime.GetDay();
				
				hour_s		= Stime.GetHour();
				min_s		= Stime.GetMinute();
				sec_s		= Stime.GetSecond();
				
				year_e		= Etime.GetYear();
				month_e		= Etime.GetMonth();
				day_e		= Etime.GetDay();
				
				hour_e		= Etime.GetHour();
				min_e		= Etime.GetMinute();
				sec_e		= Etime.GetSecond();
				
				nTime[0].SetDateTime(year_s,month_s,day_s,22,0,0);
				nTime[1].SetDateTime(year_e,month_e,day_e,22,0,0);
				
				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
						count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
						
					}
				}
				
				st_dbTimeTmp = new st_dbTimeTmp_database[count];
				

				
				for (i = 0; i<count; i++)
				{
					DB_Data_Read(i);	
				}
				
				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}
			
				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI2");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
			
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp; 
				
				Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}
			break;

		case WM_MONTH1_DATA:
			{

				g_cs.EnterCriticalSection();
				
				if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
				{
					Stime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
				}
				else
				{
					Stime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
				}
				
				year_s		= Stime.GetYear();
				month_s		= Stime.GetMonth();
				day_s		= Stime.GetDay();
				
				hour_s		= Stime.GetHour();
				min_s		= Stime.GetMinute();
				sec_s		= Stime.GetSecond();
				
				year_e		= Etime.GetYear();
				month_e		= Etime.GetMonth();
				day_e		=Etime.GetDay();
				
				hour_e		= Etime.GetHour();
				min_e		= Etime.GetMinute();
				sec_e		= Etime.GetSecond();
				
				nTime[0].SetDateTime(year_s,month_s -1,day_s,22,0,0);
				nTime[1].SetDateTime(year_e,month_e,day_e,22,0,0);
				
				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
						count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
						
					}
				}
				
				st_dbTimeTmp = new st_dbTimeTmp_database[count];
				

				
				for (i = 0; i<count; i++)
				{
					DB_Data_Read(i);	
				}
				
				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}
				
				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI3");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
			
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp; 
				
			Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}	
			break;

		case WM_MONTH2_DATA:
			{
				g_cs.EnterCriticalSection();
				
				if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
				{
					Stime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
				}
				else
				{
					Stime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
				}
				
				year_s		= Stime.GetYear();
				month_s		= Stime.GetMonth();
				day_s		= Stime.GetDay();
				
				hour_s		= Stime.GetHour();
				min_s		= Stime.GetMinute();
				sec_s		= Stime.GetSecond();
				
				year_e		= Etime.GetYear();
				month_e		= Etime.GetMonth();
				day_e		=Etime.GetDay();
				
				hour_e		= Etime.GetHour();
				min_e		= Etime.GetMinute();
				sec_e		= Etime.GetSecond();
				
				nTime[0].SetDateTime(year_s,month_s -2,day_s,22,0,0);
				nTime[1].SetDateTime(year_e,month_e,day_e,22,0,0);
				
				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
						count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
						
					}
				}
				
				st_dbTimeTmp = new st_dbTimeTmp_database[count];
				

				
				for (i = 0; i<count; i++)
				{
					DB_Data_Read(i);	
				}
				
				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}
				
				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI4");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
			
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp; 
				
				Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}
				break;
			
		case WM_MONTH3_DATA:
			{
				g_cs.EnterCriticalSection();
				
				if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
				{
					Stime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
				}
				else
				{
					Stime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
					Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
				}
				
				year_s		= Stime.GetYear();
				month_s		= Stime.GetMonth();
				day_s		= Stime.GetDay();
				
				hour_s		= Stime.GetHour();
				min_s		= Stime.GetMinute();
				sec_s		= Stime.GetSecond();
				
				year_e		= Etime.GetYear();
				month_e		= Etime.GetMonth();
				day_e		= Etime.GetDay();
				
				hour_e		= Etime.GetHour();
				min_e		= Etime.GetMinute();
				sec_e		= Etime.GetSecond();
				
				nTime[0].SetDateTime(year_s,month_s -3,day_s,22,0,0);
				nTime[1].SetDateTime(year_e,month_e,day_e,22,0,0);
				
				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
						count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
						
					}
				}
				
				st_dbTimeTmp = new st_dbTimeTmp_database[count];
				
				//st_dbTimeTmp[0].mn_Cnt = count;

				
				for (i = 0; i<count; i++)
				{
					DB_Data_Read(i);	
				}
				
				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}
			
				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI5");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
			
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp; 
				
				Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}
		break;
		case WM_MONTHCAL_DATA:
			{
				
					g_cs.EnterCriticalSection();
					
					if (time.GetHour() >= 22)			// ���� �ð��� 22���� ũ��...
					{
						Stime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
						Etime = time + COleDateTimeSpan(-1, 0, 0, 0);				// ����
					}
					else
					{
						Stime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
						Etime = time + COleDateTimeSpan(0, 0, 0, 0);				// ����
					}


				    
					st_dbTimeDate =  (st_dbTimeTmp_database*)pCopyDataStruct->lpData;
					
					strStartDate.Format("%s",st_dbTimeDate[0].str_startDate);
					strEndDate.Format("%s",st_dbTimeDate[0].str_EndDate);
					
		

					strStartDate.TrimLeft();
					strStartDate.TrimRight();

					strEndDate.TrimRight();
					strEndDate.TrimLeft();

					nStart = strStartDate.GetLength();
					nEnd = strEndDate.GetLength();
	
					ns_year		= Stime.GetYear();
					ns_month	= Stime.GetMonth();
					ns_day		= Stime.GetDay();
					
					ns_hour		= Stime.GetHour();
					ns_min		= Stime.GetMinute();
					ns_sec		= Stime.GetSecond();
					
					ne_year		= Etime.GetYear();
					ne_month	= Etime.GetMonth();
					ne_day		= Etime.GetDay();
					
					strDate.Format("%04d%02d%02d",ns_year,ns_month,ns_day);
					strDate.TrimRight();
					strDate.TrimLeft();


				if (nStart >0 && nEnd >0 )
				{
				
					csYear_s = strStartDate.Left(4);
					csYear_e = strEndDate.Left(4);

					csMonth_s = strStartDate.Mid(4,2);
					csMonth_e = strEndDate.Mid(4,2);
					
					csDay_s = strStartDate.Right(2);
					csDay_e = strEndDate.Right(2);

					year_s = atoi(csYear_s);
					year_e = atoi(csYear_e);
	
					month_s = atoi(csMonth_s);
					month_e = atoi(csMonth_e);

					day_s = atoi(csDay_s);
					day_e = atoi(csDay_e);

					
					

				}
				else
				{
					break;

				}
 				
				
 				if (strStartDate == strEndDate)			
 				{		
					Day.SetDateTime(year_s,month_s,day_s ,22,0,0);
					nTime[0] = Day - COleDateTimeSpan(+1,0,0,0);
  					nTime[1].SetDateTime(year_e,month_e,day_e ,22,0,0);		
				}
				else
 				{
 					nTime[0].SetDateTime(year_s,month_s,day_s,22,0,0);
 					nTime[1].SetDateTime(year_e,month_e,day_e,22,0,1);				
				}

				if (Mysql.MySql_Open("localhost", "root", "1111", 3306))
				{
					if (Mysql.Database_Select("AMT8520"))
					{	
					
					   count = Mysql.Record_Date_Search("FILLRATIO_DATA", "DATE", "<=>", nTime);
					   mn_Db_Cnt = count;
					}
				}
				
				
				
				if (strEndDate ==  strDate && strStartDate != strDate)
				{
					DB_Data_Init();
					//mb_SameLastDate = true;
					for (i = 0; i<count; i++)
					{
						DB_MonthCal_Data_Read(i);	
					}
						count = 20;
						st_dbTimeTmp = new st_dbTimeTmp_database[count];

						DB_MonthCal_Data_Add();

				}
				else
				{
					st_dbTimeTmp = new st_dbTimeTmp_database[count];

					
					for (i = 0; i<count; i++)
					{
						DB_Data_Read(i);	
					}
				}

				if (count ==0)
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database));
					MyCDS.lpData = st_dbTimeTmp;
				}
				else
				{
					st_dbTimeTmp[0].mn_Cnt = count;
					
					MyCDS.dwData = WM_FILLRATIO_DATA;
					MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
					MyCDS.lpData = st_dbTimeTmp;
				}

				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI6");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp;
			
				Mysql.MySql_Close();
				g_cs.LeaveCriticalSection();
			}
			break;
		case WM_NOW_DATA:
			{	
				int Temp =0 ;
				
				g_cs.EnterCriticalSection();
				
				CString strTmp[20];
			/*	st_dbTimeTmp =  (st_dbTimeTmp_database*)pCopyDataStruct->lpData;*/
				count = 20;

				for (i =0; i<count; i++)
				{
					strTmp[i].Format("%d",0);
				}
				st_dbTimeTmp = new st_dbTimeTmp_database[count];

	
				st_dbTimeTmp[0].mn_Cnt = count;

				DB_Now_Data();

				MyCDS.dwData = WM_FILLRATIO_DATA;
				MyCDS.cbData = (sizeof(st_dbTimeTmp_database))*count;
				MyCDS.lpData = st_dbTimeTmp;
				
				HWND hwnd = ::FindWindow(NULL,"FILL RATIO UI");
				//2016.0425
				strRecvLog = _T("FILL RATIO UI7");				
				sprintf(st_msg.c_msg, (LPCSTR)strRecvLog);
				Func.On_LogFile_Add(99, st_msg.c_msg);
				if (hwnd!= NULL)
				{
					HANDLE handle = AfxGetInstanceHandle();
					::SendMessage(hwnd,WM_COPYDATA,(WPARAM)handle,(LPARAM)&MyCDS);
				}
				else
				{
					AfxMessageBox(_T("ã���� �ϴ� ���� �����ϴ�."));	 
				}
				
				delete[] st_dbTimeTmp; 
			
				
				g_cs.LeaveCriticalSection();
			}
			break;

		default:
			break;
	}
	return CFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

void CMainFrame::DB_Data_Read(int nCount) 
{

	int i;
	int length;
	int pos[2];
	
	CString strData, strTmp[22];

	 strData = Mysql.Record_Read();
	// lot_no
 	length = strData.GetLength();
	pos[0] = strData.Find(",", 0);
	

	for (i = 0; i < Mysql.mn_dbtime_table_cnt; i++)
	{	
		pos[1] = strData.Find(",", pos[0]+1);
		
		if (pos[1] > 0)
		{
			strTmp[i]= strData.Mid(pos[0]+1, (pos[1] - pos[0])-1);
			pos[0] = pos[1];
		}
		else
		{
			strTmp[i] = strData.Mid(pos[0]+1, (length - pos[1])-1);
			
		}
	}
 	sprintf(st_dbTimeTmp[nCount].str_slot,"%s",strTmp[1]);
  	st_dbTimeTmp[nCount].mn_Ref_time = atoi(strTmp[2]);
  	st_dbTimeTmp[nCount].mn_run_time = atoi(strTmp[3]);
  	st_dbTimeTmp[nCount].mn_test_time = atoi(strTmp[4]);
  	st_dbTimeTmp[nCount].mn_lt_Rundown = atoi(strTmp[5]);	
  	st_dbTimeTmp[nCount].mn_lt_Socketoff = atoi(strTmp[6]);
  	st_dbTimeTmp[nCount].mn_lt_instandby = atoi(strTmp[7]);
  	st_dbTimeTmp[nCount].mn_lt_outReady = atoi(strTmp[8]);
 	st_dbTimeTmp[nCount].mn_lt_stop = atoi(strTmp[9]);
  	st_dbTimeTmp[nCount].mn_lt_User_stop = atoi(strTmp[10]);
  	st_dbTimeTmp[nCount].mn_Output = atoi(strTmp[11]);
  	st_dbTimeTmp[nCount].md_Yield = atof(strTmp[12]);
	st_dbTimeTmp[nCount].mn_Pass = atof(strTmp[13]);

}


void CMainFrame::DB_MonthCal_Data_Read(int nCount) 
{
	
	int i;
	int length;
	int pos[2];
	
	CString strData, strTmp[22];
	
	strData = Mysql.Record_Read();
	// lot_no
	length = strData.GetLength();
	pos[0] = strData.Find(",", 0);
	

	
	for (i = 0; i < Mysql.mn_dbtime_table_cnt; i++)
	{	
		pos[1] = strData.Find(",", pos[0]+1);
		
		if (pos[1] > 0)
		{
			strTmp[i]= strData.Mid(pos[0]+1, (pos[1] - pos[0])-1);
			pos[0] = pos[1];
		}
		else
		{
			strTmp[i] = strData.Mid(pos[0]+1, (length - pos[1])-1);
			
		}
	}
	
	if (nCount%20 == 0)
	{
		mn_Count = 0;
		
	}
	//sprintf(st_BufferTemp.str_slot[mn_Count],"%s",strTmp[1]);
	st_BufferTemp.mn_Ref_time[mn_Count] += atoi(strTmp[2]);
	st_BufferTemp.mn_run_time[mn_Count] += atoi(strTmp[3]);
	st_BufferTemp.mn_test_time[mn_Count] += atoi(strTmp[4]);
	st_BufferTemp.mn_lt_Rundown[mn_Count] += atoi(strTmp[5]);
	st_BufferTemp.mn_lt_Socketoff[mn_Count] += atoi(strTmp[6]);
	st_BufferTemp.mn_lt_instandby[mn_Count] += atoi(strTmp[7]);
	st_BufferTemp.mn_lt_outReady[mn_Count] += atoi(strTmp[8]);
	st_BufferTemp.mn_lt_stop[mn_Count] += atoi(strTmp[9]);
	st_BufferTemp.mn_lt_User_stop[mn_Count] += atoi(strTmp[10]);
	st_BufferTemp.mn_Output[mn_Count] += atoi(strTmp[11]);
	st_BufferTemp.md_Yield[mn_Count] += atoi(strTmp[12]);
	st_BufferTemp.mn_Pass[mn_Count] += atoi(strTmp[13]);

	mn_Count ++;
	
}
void CMainFrame::DB_MonthCal_Data_Add() 
{
	
	int i;
//	int length;
//	int pos[2];
	double dTemp[20];
	CString strData, strTmp[20];
	
	for (i = 0; i<20; i++)
	{
		dTemp[i] = 0;
	}
	
	for (i = 0; i<20; i++)
	{	
		
		st_BufferTemp.mn_Ref_time[i] += st_handler.m_tDRef.GetHours()*3600 +st_handler.m_tDRef.GetMinutes()*60 +st_handler.m_tDRef.GetSeconds();
		st_BufferTemp.mn_run_time[i] += st_handler.m_tDR.GetHours()*3600 +st_handler.m_tDR.GetMinutes()*60 +st_handler.m_tDR.GetSeconds();
		st_BufferTemp.mn_test_time[i] += st_handler.m_tDtest[i/10][i%10].GetHours()*3600 +st_handler.m_tDtest[i/10][i%10].GetMinutes()*60 +st_handler.m_tDtest[i/10][i%10].GetSeconds();
		st_BufferTemp.mn_lt_Rundown[i] += st_handler.m_tDRdown[i].GetHours()*3600 + st_handler.m_tDRdown[i].GetMinutes()*60 +st_handler.m_tDRdown[i].GetSeconds();
		st_BufferTemp.mn_lt_Socketoff[i] += st_handler.m_tDSokOff[i/10][i%10].GetHours()*3600 +st_handler.m_tDSokOff[i/10][i%10].GetMinutes()*60 +st_handler.m_tDSokOff[i/10][i%10].GetSeconds();
		st_BufferTemp.mn_lt_instandby[i] += st_handler.m_tDInStandby[i].GetHours()*3600 +st_handler.m_tDInStandby[i].GetMinutes()*60 +st_handler.m_tDInStandby[i].GetSeconds();
		st_BufferTemp.mn_lt_outReady[i] += st_handler.m_tDOutStandby[i/10][i%10].GetHours()*3600 +st_handler.m_tDOutStandby[i/10][i%10].GetMinutes()*60 +st_handler.m_tDOutStandby[i/10][i%10].GetSeconds();
		st_BufferTemp.mn_lt_stop[i] += st_handler.m_tDStop[i].GetHours()*3600 +st_handler.m_tDStop[i].GetMinutes()*60 +st_handler.m_tDStop[i].GetSeconds();//2015.0723
		st_BufferTemp.mn_lt_User_stop[i] += st_handler.m_tDUserStop[i].GetHours()*3600 +st_handler.m_tDUserStop[i].GetMinutes()*60 +st_handler.m_tDUserStop[i].GetSeconds();//2015.0723					
		st_BufferTemp.mn_Pass[i] += st_handler.mn_Pass[i];

 		if(st_handler.mn_Input[i] <= 0) 
 		{

			dTemp[i] = st_BufferTemp.md_Yield[i]/(mn_Db_Cnt/20);
			st_BufferTemp.md_Yield[i] = dTemp[i];
			st_BufferTemp.mn_Output[i] += 0;
			st_BufferTemp.md_Yield[i] += 0;
			

 		}
 		else
 		{
			
			dTemp[i] = st_BufferTemp.md_Yield[i]/(mn_Db_Cnt/20);
			dTemp[i] += (double)st_handler.mn_Pass[i]/(double)st_handler.mn_Input[i] * 100.0f;
			st_BufferTemp.md_Yield[i] = dTemp[i]/2;
			st_BufferTemp.mn_Output[i] += st_handler.mn_Input[i];

		}

		st_dbTimeTmp[i].mn_Ref_time = st_BufferTemp.mn_Ref_time[i];
		st_dbTimeTmp[i].mn_run_time = st_BufferTemp.mn_run_time[i];
		st_dbTimeTmp[i].mn_test_time = st_BufferTemp.mn_test_time[i];
		st_dbTimeTmp[i].mn_lt_Rundown = st_BufferTemp.mn_lt_Rundown[i];
		st_dbTimeTmp[i].mn_lt_Socketoff = st_BufferTemp.mn_lt_Socketoff[i];
		st_dbTimeTmp[i].mn_lt_instandby = st_BufferTemp.mn_lt_instandby[i];
		st_dbTimeTmp[i].mn_lt_outReady = st_BufferTemp.mn_lt_outReady[i];
		st_dbTimeTmp[i].mn_lt_stop = st_BufferTemp.mn_lt_stop[i];
		st_dbTimeTmp[i].mn_lt_User_stop = st_BufferTemp.mn_lt_User_stop[i];
		st_dbTimeTmp[i].mn_Pass = st_BufferTemp.mn_Pass[i];
		st_dbTimeTmp[i].mn_Output = st_BufferTemp.mn_Output[i];
		st_dbTimeTmp[i].md_Yield = st_BufferTemp.md_Yield[i];

	}
	
	
}

void CMainFrame::DB_Now_Data() 
{
	
	int i;
//	int length;
//	int pos[2];
	
	CString strData, strTmp[22];
	
	for (i = 0; i<20; i++)
	{	
		strTmp[1].Format("Slot_%d",i+1);	
		sprintf(st_dbTimeTmp[i].str_slot,"%s",strTmp[1]);
		st_dbTimeTmp[i].mn_Ref_time = st_handler.m_tDRef.GetHours()*3600 +st_handler.m_tDRef.GetMinutes()*60 +st_handler.m_tDRef.GetSeconds();
		st_dbTimeTmp[i].mn_run_time = st_handler.m_tDR.GetHours()*3600 +st_handler.m_tDR.GetMinutes()*60 +st_handler.m_tDR.GetSeconds();
		st_dbTimeTmp[i].mn_test_time = st_handler.m_tDtest[i/10][i%10].GetHours()*3600 +st_handler.m_tDtest[i/10][i%10].GetMinutes()*60 +st_handler.m_tDtest[i/10][i%10].GetSeconds();
		st_dbTimeTmp[i].mn_lt_Rundown = st_handler.m_tDRdown[i].GetHours()*3600 + st_handler.m_tDRdown[i].GetMinutes()*60 +st_handler.m_tDRdown[i].GetSeconds();
		st_dbTimeTmp[i].mn_lt_Socketoff = st_handler.m_tDSokOff[i/10][i%10].GetHours()*3600 +st_handler.m_tDSokOff[i/10][i%10].GetMinutes()*60 +st_handler.m_tDSokOff[i/10][i%10].GetSeconds();
		st_dbTimeTmp[i].mn_lt_instandby = st_handler.m_tDInStandby[i].GetHours()*3600 +st_handler.m_tDInStandby[i].GetMinutes()*60 +st_handler.m_tDInStandby[i].GetSeconds();
		st_dbTimeTmp[i].mn_lt_outReady = st_handler.m_tDOutStandby[i/10][i%10].GetHours()*3600 +st_handler.m_tDOutStandby[i/10][i%10].GetMinutes()*60 +st_handler.m_tDOutStandby[i/10][i%10].GetSeconds();
		
		st_dbTimeTmp[i].mn_lt_stop = st_handler.m_tDStop[i].GetHours()*3600 +st_handler.m_tDStop[i].GetMinutes()*60 +st_handler.m_tDStop[i].GetSeconds();//2015.0723
		st_dbTimeTmp[i].mn_lt_User_stop = st_handler.m_tDUserStop[i].GetHours()*3600 +st_handler.m_tDUserStop[i].GetMinutes()*60 +st_handler.m_tDUserStop[i].GetSeconds();//2015.0723					

		st_dbTimeTmp[i].mn_Pass = st_handler.mn_Pass[i];
		
		if(st_handler.mn_Input[i] <= 0)
		{
			st_dbTimeTmp[i].md_Yield = 0;
			st_dbTimeTmp[i].mn_Output = 0;
		}
		else
		{
			st_dbTimeTmp[i].md_Yield = (double)st_handler.mn_Pass[i]/(double)st_handler.mn_Input[i] * 100.0f;
			st_dbTimeTmp[i].mn_Output = st_handler.mn_Input[i];
		}
	}

}
void CMainFrame::DB_Data_Init() 
{
	
	int i;
	//	int length;
	//	int pos[2];

	for (i = 0; i<20; i++)
	{	
		st_BufferTemp.mn_Ref_time[i] = 0;
		st_BufferTemp.mn_run_time[i] = 0;
		st_BufferTemp.mn_test_time[i] = 0;
		st_BufferTemp.mn_lt_Rundown[i] = 0;
		st_BufferTemp.mn_lt_Socketoff[i] = 0;
		st_BufferTemp.mn_lt_instandby[i] = 0;
		st_BufferTemp.mn_lt_outReady[i] = 0;
		st_BufferTemp.mn_lt_stop[i] = 0;
		st_BufferTemp.mn_lt_User_stop[i] = 0;
		st_BufferTemp.mn_Output[i] = 0;
		st_BufferTemp.md_Yield[i] = 0;
		st_BufferTemp.mn_Pass[i] = 0;
	}
}
////////////////////

void CMainFrame::CSV_File_Save()
{
	CString mstr_content,mstr_list_name,mstr_create_file,Temp;   
	
	int mn_existence;      
	double nMin[21][21];
	double nTotal[21];
	double nPass[21];
	double dRatioData[21][21];
	double dAvr_Yield;
	int gapTime[21];
	int i,j;
	CString csData[21][21];
	CString csAvr[21];
	CString str_Temp,str_Title,str_Avr,str_data;

//	char strValue[1000];
	char fileName[256];        
	
	CString str_FilePath;
	CString strSaveString;
	CString strTmp,strTitle;
	
	int n_cur_year, n_cur_month, n_cur_day;	
	int n_cur_hour, n_cur_minute, n_cur_second;	

	COleDateTime mtime_cur;						
	
	CString str_time;
	
	FILE  *fp ;

	mtime_cur = COleDateTime::GetCurrentTime();
	
	for (i = 0; i<21; i++)
	{
		nTotal[i] = 0;
		nPass[i]  =0;
		for (j =0; j<21; j++)
		{
			nMin[i][j] = 0;
			dRatioData[i][j] = 0;
		}
	}

	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  
	
	n_cur_hour = mtime_cur.GetHour();
	n_cur_minute = mtime_cur.GetMinute();
	n_cur_second = mtime_cur.GetSecond();
	
	//str_FilePath.Format("FILLRATIO_%04d%02d%02d%02d%02d%02d", n_cur_year, n_cur_month, n_cur_day,n_cur_hour,n_cur_minute,n_cur_second);
	if (mb_intervalCheck == true)
 	{
		if (mb_Ten == true)
		{
			str_FilePath.Format("%s_%04d%02d%02d%02d%02d%02d",st_lamp.mstr_equip_id, n_cur_year, n_cur_month, n_cur_day,22,0,0);
		}
		else
		{
			str_FilePath.Format("%s_%04d%02d%02d%02d%02d%02d",st_lamp.mstr_equip_id, n_cur_year, n_cur_month, n_cur_day,n_cur_hour,n_cur_minute,00);
		}
		mb_Ten = false;
		mb_intervalCheck = false;
	}
	else
	{
		str_FilePath.Format("%s_%04d%02d%02d%02d%02d%02d",st_lamp.mstr_equip_id, n_cur_year, n_cur_month, n_cur_day,n_cur_hour,n_cur_minute,n_cur_second);
	}
	
	///////////////////////////////////////////////
	
	//str_FilePath.Format("%s_%04d%02d%02d%02d%02d%02d",st_lamp.mstr_equip_id, n_cur_year, n_cur_month, n_cur_day,n_cur_hour,n_cur_minute,00);

	mstr_create_file =  st_handler.mstr_DirPath;
	mstr_create_file += str_FilePath;
	mstr_create_file += ".CSV";
	
	/*sprintf(fileName, "%s", mstr_create_file);*/
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* ���� ������ */
	{
		mstr_create_file =  st_handler.mstr_DirPath;
		mstr_create_file += str_FilePath;
		mstr_create_file += ".CSV";
		
	}
	
	if ((fp=fopen(mstr_create_file,"w"))==NULL)  
	{	
		return;
	}

	for(i = 0; i<20; i++)
	{						
		csData[2][i].Format("%d",st_dbtime.mn_Ref_time[i]);
		csData[3][i].Format("%d",st_dbtime.mn_run_time[i]);
		csData[4][i].Format("%d",st_dbtime.mn_test_time[i]);

		csData[7][i].Format("%d",st_dbtime.mn_lt_Rundown[i]);
		csData[8][i].Format("%d",st_dbtime.mn_lt_Socketoff[i]);
		csData[9][i].Format("%d",st_dbtime.mn_lt_instandby[i]);
		csData[10][i].Format("%d",st_dbtime.mn_lt_outReady[i]);
		csData[11][i].Format("%d",st_dbtime.mn_lt_stop[i]);
		csData[12][i].Format("%d",st_dbtime.mn_lt_User_stop[i]);
		
		mstr_Data[19][i].Format("%d",st_dbtime.mn_Output[i]);
		//mstr_Data[20][i].Format("%3.2f",st_dbtime.md_Yield[i]/(mn_Cnt/20));
		mstr_Data[20][i].Format("%3.2f",st_dbtime.md_Yield[i]);
		//kwlee 2015.0902
		gapTime[i] = atoi(csData[2][i]) - (atoi(csData[4][i]) + atoi(csData[7][i]) + atoi(csData[8][i]) + atoi(csData[9][i]) + atoi(csData[10][i]) + atoi(csData[11][i]) + atoi(csData[12][i]));
		//kwlee 2015.1112
		if (gapTime[i] < 0)
		{
			gapTime[i] = 0;
		}
		////
		nMin[2][i] = atoi(csData[2][i]);
		nMin[3][i] = atoi(csData[3][i]);
		//kwlee 2015.0902
		nMin[4][i] = atoi(csData[4][i]) + gapTime[i];
	//	nMin[4][i] = atoi(csData[4][i]);
		//nMin[4][i] = atoi(csData[4][i]);
		//
		nMin[5][i] = atof(mstr_Data[5][i]);
		nMin[6][i] = atof(mstr_Data[6][i]);
		nMin[7][i] = atoi(csData[7][i]);
		nMin[8][i] = atoi(csData[8][i]);
		nMin[9][i] = atoi(csData[9][i]);
		nMin[10][i] = atoi(csData[10][i]);
		nMin[11][i] = atoi(csData[11][i]);
		nMin[12][i] = atoi(csData[12][i]);

		nMin[19][i] = atoi(mstr_Data[19][i]);
		nMin[20][i] = atof(mstr_Data[20][i]);

		//////////////////////////////////////////////////////////////////////////
		//RATIO ȭ�� ǥ��
		dRatioData[2][i] = nMin[3][i]/nMin[2][i]*100; //RUN RATIO
		mstr_Data[5][i].Format("%3.2f",(float)dRatioData[2][i]);
		
		dRatioData[3][i] = nMin[4][i]/nMin[2][i]*100; //TEST RATIO
		mstr_Data[6][i].Format("%3.2f",(float)dRatioData[3][i]);
		
		dRatioData[4][i] = nMin[7][i]/nMin[2][i]*100; //RUNDOWN RATIO
		mstr_Data[13][i].Format("%3.2f",(float)dRatioData[4][i]);
		
		dRatioData[5][i] = nMin[8][i]/nMin[2][i]*100; //SOCKET OFF RATIO
		mstr_Data[14][i].Format("%3.2f",(float)dRatioData[5][i]);

		dRatioData[6][i] = nMin[9][i]/nMin[2][i]*100; //INSTANDBY RATIO
		mstr_Data[15][i].Format("%3.2f",(float)dRatioData[6][i]);
		
		dRatioData[7][i] = nMin[10][i]/nMin[2][i]*100; //OUTREADY RATIO
		mstr_Data[16][i].Format("%3.2f",(float)dRatioData[7][i]);
		
		dRatioData[8][i] = nMin[11][i]/nMin[2][i]*100; //STOP RATIO
		mstr_Data[17][i].Format("%3.2f",(float)dRatioData[8][i]);
		
		dRatioData[9][i] = nMin[12][i]/nMin[2][i]*100; //USER STOP RAITO
		mstr_Data[18][i].Format("%3.2f",(float)dRatioData[9][i]);
		////////////////////////////////////////////////////////////////////
		//LOS TIME(MIN)
		mstr_Data[1][i].Format("Slot_%d",i +1);
		mstr_Data[2][i].Format("%d",(int)nMin[2][i]/60);
		mstr_Data[3][i].Format("%d",(int)nMin[3][i]/60);
		mstr_Data[4][i].Format("%d",(int)nMin[4][i]/60);
		
		mstr_Data[7][i].Format("%d",(int)nMin[7][i]/60);
		mstr_Data[8][i].Format("%d",(int)nMin[8][i]/60);
		mstr_Data[9][i].Format("%d",(int)nMin[9][i]/60);
		mstr_Data[10][i].Format("%d",(int)nMin[10][i]/60);
		mstr_Data[11][i].Format("%d",(int)nMin[11][i]/60);
		mstr_Data[12][i].Format("%d",(int)nMin[12][i]/60);

		//////////////////////////////////////////////////////////
		//TOTAL AVR
		csAvr[1].Format("%s","Zone-AVR");
		csAvr[2].Format("%d",(int)((nTotal[2] += nMin[2][i]) /20) /60);
		csAvr[3].Format("%d",(int)((nTotal[3] += nMin[3][i]) /20) /60);
		csAvr[4].Format("%d",(int)((nTotal[4] += nMin[4][i]) /20) /60);

		csAvr[5].Format("%3.2f",(nTotal[5] = nTotal[3]/nTotal[2] *100));
		csAvr[6].Format("%3.2f",(nTotal[6] = nTotal[4]/nTotal[2] *100));

		csAvr[7].Format("%d",(int)((nTotal[7] += nMin[7][i]) /20) /60);
		csAvr[8].Format("%d",(int)((nTotal[8] += nMin[8][i]) /20) /60);
		csAvr[9].Format("%d",(int)((nTotal[9] += nMin[9][i]) /20) /60);
		csAvr[10].Format("%d",(int)((nTotal[10] += nMin[10][i]) /20) /60);
		csAvr[11].Format("%d",(int)((nTotal[11] += nMin[11][i]) /20) /60);
		csAvr[12].Format("%d",(int)((nTotal[12] += nMin[12][i]) /20) /60);
		
		csAvr[13].Format("%3.2f",(nTotal[13] = nTotal[7]/nTotal[2] *100) );
		csAvr[14].Format("%3.2f",(nTotal[14] = nTotal[8]/nTotal[2] *100) );
		csAvr[15].Format("%3.2f",(nTotal[15] = nTotal[9]/nTotal[2] *100) );
		csAvr[16].Format("%3.2f",(nTotal[16] = nTotal[10]/nTotal[2] *100));
		csAvr[17].Format("%3.2f",(nTotal[17] = nTotal[11]/nTotal[2] *100));
		csAvr[18].Format("%3.2f",(nTotal[18] = nTotal[12]/nTotal[2] *100));

		//////////////
		csAvr[19].Format("%d",(int)(nTotal[19] += nMin[19][i]));
		//kwlee 2015.0824
		//csAvr[20].Format("%3.2f",(nTotal[20] += nMin[20][i])/20);
		nTotal[20] += st_dbtime.mn_Pass[i];
		dAvr_Yield = (double)nTotal[19]/(double)nTotal[20] * 100;

		if (dAvr_Yield<0)
		{
			dAvr_Yield = 0;
		}
		//csAvr[20].Format("%3.2f",(double)nTotal[19]/(double)nTotal[20] * 100);
		csAvr[20].Format("%3.2f",dAvr_Yield);
	}

	strTmp = " ,Zone-Slot, REF TIME, RUN TIME, TEST TIME, FillRatio (RUN), FillRatio (TEST), L/T RD, L/T SocketOff, L/T InStandby, L/T OutReady, L/T STOP, L/T UserStop, L/R RD, L/R SocketOff, L/R Instandby, L/R OutReady, L/R STOP, L/R UserStop, Output, Yield";
	strTmp +="\n";			

	for (i = 0; i<21; i++)
	{
		strTmp += csAvr[i];
		strTmp += ",";
	}
	 strTmp +="\n";

	for (i = 0; i<20; i++)
	{
		for (j = 0; j<21; j++)
		{
			strTmp +=  mstr_Data[j][i];
			strTmp +=  ",";	
		}
		strTmp += "\n";
	}		

	
	fprintf(fp,"%s", strTmp);

	fclose(fp); 
}


void CMainFrame::IntervalCheck()
{
	int i = 0, n_hour, n_minute, n_second;
	int nInterval = 0;
	COleDateTime cur_time;
	CTime cur = CTime::GetCurrentTime();
	char chr_data[100];
	
	n_hour = cur.GetHour();
	n_minute = cur.GetMinute();
	n_second = cur.GetSecond();
	
	:: GetPrivateProfileString("FILLATIO", "INTERVAL", "0", chr_data, 100, st_path.mstr_basic); 
	nInterval = atoi(chr_data);
	if(nInterval<=0)
	{
		nInterval = 1;
	}

	///////////////////////////////////////////
	//kwlee 2015.1005
	if (n_hour%nInterval == 0&& n_minute% 60 == 0 && n_second == 0|| 
	n_hour == 21 && n_minute== 59 && n_second == 50 || 
	n_hour == 0 && n_minute== 0 && n_second == 0)
	{	
		//2015.0914
		mb_intervalCheck = true;
		st_dbtime.mole_date = COleDateTime::GetCurrentTime();
		
		for (int i =0; i<TSITE_SOCKET_CNT; i++)
		{
			//kwlee 2015.1117
// 			st_dbtime.mn_Ref_time[i] = st_DB_time.n_Db_time[REF_TIME][i];//_DB_time.n_Db_time[REF_TIME][i];
// 			st_dbtime.mn_run_time[i] =  st_DB_time.n_Db_time[RUN_TIME][i];
// 			st_dbtime.mn_test_time[i] = st_DB_time.n_Db_time[TEST_TIME][i];
// 			st_dbtime.mn_lt_Rundown[i] = st_DB_time.n_Db_time[RUN_DOWN][i];
// 			st_dbtime.mn_lt_Socketoff[i] = st_DB_time.n_Db_time[SOCKETOFF][i];
// 			st_dbtime.mn_lt_instandby[i] = st_DB_time.n_Db_time[INSTANBY][i];
// 			st_dbtime.mn_lt_outReady[i] = st_DB_time.n_Db_time[OUTREADY][i];
// 			st_dbtime.mn_lt_stop[i] = st_DB_time.n_Db_time[STOP][i];
// 			st_dbtime.mn_lt_User_stop[i] = st_DB_time.n_Db_time[USERSTOP][i];//2015.0723
			
 			st_dbtime.mn_Ref_time[i] = st_handler.m_tDRef.GetHours()*3600 +st_handler.m_tDRef.GetMinutes()*60 +st_handler.m_tDRef.GetSeconds();
 			st_dbtime.mn_run_time[i] = st_handler.m_tDR.GetHours()*3600 +st_handler.m_tDR.GetMinutes()*60 +st_handler.m_tDR.GetSeconds();
 			st_dbtime.mn_test_time[i] = st_handler.m_tDtest[i/10][i%10].GetHours()*3600 +st_handler.m_tDtest[i/10][i%10].GetMinutes()*60 +st_handler.m_tDtest[i/10][i%10].GetSeconds();
 			st_dbtime.mn_lt_Rundown[i] = st_handler.m_tDRdown[i].GetHours()*3600 + st_handler.m_tDRdown[i].GetMinutes()*60 +st_handler.m_tDRdown[i].GetSeconds();
			st_dbtime.mn_lt_Socketoff[i] = st_handler.m_tDSokOff[i/10][i%10].GetHours()*3600 +st_handler.m_tDSokOff[i/10][i%10].GetMinutes()*60 +st_handler.m_tDSokOff[i/10][i%10].GetSeconds();
 			st_dbtime.mn_lt_instandby[i] = st_handler.m_tDInStandby[i].GetHours()*3600 +st_handler.m_tDInStandby[i].GetMinutes()*60 +st_handler.m_tDInStandby[i].GetSeconds();
 			st_dbtime.mn_lt_outReady[i] = st_handler.m_tDOutStandby[i/10][i%10].GetHours()*3600 +st_handler.m_tDOutStandby[i/10][i%10].GetMinutes()*60 +st_handler.m_tDOutStandby[i/10][i%10].GetSeconds();
 			st_dbtime.mn_lt_stop[i] = st_handler.m_tDStop[i].GetHours()*3600 +st_handler.m_tDStop[i].GetMinutes()*60 +st_handler.m_tDStop[i].GetSeconds();//2015.0723
 			st_dbtime.mn_lt_User_stop[i] = st_handler.m_tDUserStop[i].GetHours()*3600 +st_handler.m_tDUserStop[i].GetMinutes()*60 +st_handler.m_tDUserStop[i].GetSeconds();//2015.0723					
			/////////

			st_dbtime.mn_Pass[i] = st_handler.mn_Pass[i];
			///
			if(st_handler.mn_Input[i] <= 0)
			{
				st_dbtime.mn_Output[i] = 0; //kwlee 2015.0820
				st_dbtime.md_Yield[i] = 0;
			}
			else
			{	
				st_dbtime.mn_Output[i]	= st_handler.mn_Input[i];//kwlee 2015.0820
				st_dbtime.md_Yield[i] = (double)st_handler.mn_Pass[i]/(double)st_handler.mn_Input[i] * 100.0f;
			}
				
			//Func.DB_perTime_Write(st_dbtime,i);
		}

	 	if (n_hour == 22 && n_minute== 0 && n_second == 0)
 		{
 			
 		}
		else
		{
			if (n_hour == 21 && n_minute== 59 && n_second == 50)
		 	{
		 		mb_Ten = true;
			}
			CSV_File_Save();
		}
	}
}

///////////////////////////////////////////
////2015.0604
void CMainFrame::FtpConnect() 
{
	
	// TODO: Add your control notification handler code here
	CString strSend;
	CString strFtp;
	
	//�����ϰ����ϴ� FTP ������ IP �ּҿ� ����ڸ�, ��ȣ �Է�
	if( m_pFtp.Open(st_lamp.mstr_ftp_ip, st_lamp.mstr_ftp_user, st_lamp.mstr_ftp_pw, st_lamp.mn_ftp_port) )
	{
		OnUpdateRemote(st_lamp.mstr_ftp_path);
		
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			strFtp = "Ftp Connection Complete";
			// 			sprintf(st_msg.c_normal_msg, "%s", strFtp);
			// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			strFtp = "Ftp Connection Fail";
			// 			sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
			// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
	}
}

void CMainFrame::OnUpdateRemote(CString strPath)
{
	CString strTemp;
	CString strDirectory;
	
	BOOL bContinue = TRUE;
	
	unsigned long size;
	
	CTime time;
	
	int bWorking;
	
	if(strPath != _T("/"))
	{
		strTemp = _T("[..]");
		// �߰����� �߰��� index �� �޾ƿ�
	}
	//ã�� ȭ�� ����
	CFTPFileFind	finder(m_pFtp.m_pConnection);
	bWorking = finder.FindFile(strPath);
	
	while(bWorking)
	{
		//FTP ������ ���ϸ��� ����
		bWorking = finder.FindNextFile();		// ȭ���ϳ� ã��
		
		// ȭ���̸� �߰�
		strTemp = finder.GetFileName();
		if(strTemp == _T(".")) continue;	// "." �̸� �������� �Ѿ
		
		if(finder.IsDirectory())
		{
			//���丮�� [ ] �� ����	
			strDirectory.Format(_T("[%s]"), strTemp);
		}
		else
		{
			strDirectory = strTemp;
		}
		// �߰����� �߰��� index �� �޾ƿ�
		
		// ȭ��ũ�� �߰�
		size = finder.GetLength();
		strTemp.Format(_T("%d"),size);
		
		// ȭ�ϳ�¥ �߰�
		finder.GetLastWriteTime(time);
		strTemp = time.Format("%y%m%d %H:%M");		
	}
	
	UpdateData(FALSE);	// ȭ�鿡 �ִ°�����.....
}

void CMainFrame::FtpFileDown()
{
	CString strData;
	CString strFtp;
	int mn_response;  // ��ȭ ���ڿ� ���� ���� �� ���� ����
	int nRet;
	char chr_file_name[256];

	CDialog_Infor info_dlg;


	int n_existence;
	
	
	sprintf(chr_file_name, "%s", st_path.mstr_scrap_code);
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)
	{
		DeleteFile(chr_file_name);//C:\AMT8520\Setting\RETEST_RULE.TXT ���� �ٿ� ���� ��ġ
	}
	
	if( m_pFtp.Open(st_lamp.mstr_ftp_ip, st_lamp.mstr_ftp_user, st_lamp.mstr_ftp_pw, st_lamp.mn_ftp_port) )
	{ 
		strData.Format(_T("%s/%s"), st_lamp.mstr_ftp_path, st_lamp.mstr_ftp_scrap_code_name);// /Users/MDL/MDL_HNDL/WP_SKEL_SSD/RETEST_RULE, RETEST_RULE.TXT -> FTP���� ���, FTP���� �ٿ� ���� ����
		
		nRet = m_pFtp.DownLoad(strData, st_path.mstr_scrap_code);
		m_pFtp.Close();

		if(nRet == CTL_YES)
		{
			st_basic.mn_mode_ftp = 1;
			st_basic.mstr_mode_ftp = "FTP";

			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				strFtp = "Ftp File Down Complete";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}	
		}
		else
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				strFtp = "Ftp File Down Fail";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}
			
			if (COMI.mn_run_status != dSTOP)
			{
				g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
				Func.OnSet_IO_Port_Stop(); // ��� ���� : ���� ������ ��� I/O ��� �������� �Լ�
				
				if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
			}
		
			st_msg.mstr_confirm_msg = _T("Scrap Code�� Local Mode�� ��� �Ͻðڽ��ϱ�?");
			if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_confirm_msg = "Do you want to use to Scrap Code for Local Mode.";
			
			mn_response = info_dlg.DoModal();
			
			if (mn_response == IDOK)
			{
				st_basic.mn_mode_ftp = 2;
				st_basic.mstr_mode_ftp = "Local";
			}
			else if (mn_response == IDCANCEL)
			{
				st_basic.mn_mode_ftp = 0;
				st_basic.mstr_mode_ftp = "not use";
			}

			if (st_handler.cwnd_list != NULL)
			{
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
			}

		}
	
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
		{
			strFtp = "Ftp File Down Fail";
			Func.On_LogFile_Add(LOG_TOTAL, strFtp);
		}

		if (COMI.mn_run_status != dSTOP)
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
			Func.OnSet_IO_Port_Stop(); // ��� ���� : ���� ������ ��� I/O ��� �������� �Լ�

			if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
		}
		
		st_msg.mstr_confirm_msg = _T("Scrap Code�� Local Mode�� ��� �Ͻðڽ��ϱ�?");
		if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_confirm_msg = "Do you want to use to Scrap Code for Local Mode.";

		mn_response = info_dlg.DoModal();
		
		if (mn_response == IDOK)
		{
			st_basic.mn_mode_ftp = 2;
			st_basic.mstr_mode_ftp = "Local";
		}
		else if (mn_response == IDCANCEL)
		{
			st_basic.mn_mode_ftp = 0;
			st_basic.mstr_mode_ftp = "not use";
		}				
	}
	
	m_pFtp.Close();	
	
}

void CMainFrame::RecipeRuleFileDown()
{
	CString strData;
	CString strFtp;
	int mn_response, nRet;

	char chr_file_name[256];
	int n_existence;
	CDialog_Infor info_dlg;
	
	
	sprintf(chr_file_name, "%s", st_path.mstr_recipe_rule);
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)
	{
		DeleteFile(chr_file_name);//C:\AMT8520\Setting\RECIPE_RULE.TXT
	}

	if( m_pFtp.Open(st_lamp.mstr_ftp_ip, st_lamp.mstr_ftp_user, st_lamp.mstr_ftp_pw, st_lamp.mn_ftp_port) )
	{
		strData.Format(_T("%s/%s"), st_lamp.mstr_ftp_path, st_lamp.mstr_ftp_recipe_rule_name);

		nRet = m_pFtp.DownLoad(strData, st_path.mstr_recipe_rule);
		m_pFtp.Close();

		if( nRet == CTL_YES )
		{
			st_basic.mn_mode_ftp = 1;
			st_basic.mstr_mode_ftp = "FTP";

			if( st_handler.cwnd_list != NULL)
			{
				strFtp = "Ftp recipe rule down complete";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}
		}
		else
		{
			if (st_handler.cwnd_list != NULL)	// ����Ʈ �� ȭ�� ����
			{
				strFtp = "Ftp recipe rule down fail";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}
			
			if (COMI.mn_run_status != dSTOP)
			{
				g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
				Func.OnSet_IO_Port_Stop(); // ��� ���� : ���� ������ ��� I/O ��� �������� �Լ�
				
				if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
			}
			
			st_msg.mstr_confirm_msg = _T("Recipe Rule�� Local Mode�� ��� �Ͻðڽ��ϱ�?");
			if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_confirm_msg = "Do you want to use to recipe rule for Local Mode.";
			
			mn_response = info_dlg.DoModal();
			
			if (mn_response == IDOK)
			{
				st_basic.mn_mode_ftp = 2;
				st_basic.mstr_mode_ftp = "Local";
			}
			else if (mn_response == IDCANCEL)
			{
				st_basic.mn_mode_ftp = 0;
				st_basic.mstr_mode_ftp = "not use";
			}
			
			if (st_handler.cwnd_list != NULL)
			{
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
			}
		}
	}
}