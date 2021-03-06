// Screen_Main.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Screen_Main.h"
#include "MainFrm.h"

#include "Dialog_KeyBoard.h"
#include "Dialog_Keypad.h"
#include "Dialog_Message.h"
#include "FastechPublic_IO.h"

#include "Dialog_Select.h"
#include "Dialog_Buffer_Status.h"
#include "Dialog_Data_Lot.h"
#include "Dialog_Data_TestSite.h"
#include "Dialog_Data_Cok.h"
#include "Dialog_Data_Module.h"
#include "io.h" 

#include "CtlBd_Function.h"
#include "GridControlAlg.h"

#include "CtlBd_Library.h"
#include "SrcBase\ALocalization.h"

#include "IO_Manager.h"
#include "ComizoaPublic.h"

#include "AMTLotManager.h"
#include "SrcPart/APartHandler_COK.h"
#include "SrcPart/PartFunction.h"
#include "SrcPart/APartTray.h"

#include "SrcPart/APartNetworkManager.h"
#include "SrcPart/APartTestSite.h"
#include "SrcPart/APartRobot.h"
#include "SrcPart/APartHandler.h"

#include "AMTRegistry.h"
#include "SrcPart/APartShortkey.h"

#include "Dialog_XgemInterface.h"

#include "Dialog_Infor.h"//2015.0604
#include "Dialog_MgrSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Main

IMPLEMENT_DYNCREATE(CScreen_Main, CFormView)

CScreen_Main::CScreen_Main()
	: CFormView(CScreen_Main::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Main)
	//}}AFX_DATA_INIT
	m_emvt_ts = EMVT_SERIAL;
	m_emvt_mdl = EMVT_SERIAL;
}

CScreen_Main::~CScreen_Main()
{
}

void CScreen_Main::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Main)
	DDX_Control(pDX, IDC_BTN_ABORT, m_btn_abort);
	DDX_Control(pDX, IDC_BTN_TEST1, m_btnBufferIn);
	DDX_Control(pDX, IDC_BTN_STACKER_OFF, m_btn_stacker_off);
	DDX_Control(pDX, IDC_GROUP_P_LOT_INFO, m_group_p_lot_info);	// P_LOT 2013,0910
	DDX_Control(pDX, IDC_MSG_P_LOT_INFO, m_msg_p_lot_info);	// P_LOT 2013,0910
	DDX_Control(pDX, IDC_STATIC_WORK3, m_group_work3);
	DDX_Control(pDX, IDC_STATIC_WORK2, m_group_work2);
	DDX_Control(pDX, IDC_STATIC_WORK1, m_group_work1);
	DDX_Control(pDX, IDC_STATIC_CONV3, m_group_conv3);
	DDX_Control(pDX, IDC_STATIC_CONV2, m_group_conv2);
	DDX_Control(pDX, IDC_STATIC_CONV1, m_group_conv1);
	DDX_Control(pDX, IDC_GROUP_BIN_TRAY, m_group_bin_tray);
	DDX_Control(pDX, IDC_GROUP_ROBOT, m_group_robot);
	DDX_Control(pDX, IDC_GROUP_REFERENCE, m_group_reference);
	DDX_Control(pDX, IDC_GROUP_SITE, m_group_site);
	DDX_Control(pDX, IDC_GROUP_TRAY_MOVE_REJ, m_group_tray_move_rej);
	DDX_Control(pDX, IDC_GROUP_TRAY_MOVE, m_group_tray_move);
	DDX_Control(pDX, IDC_GROUP_REJ_STACKER, m_group_rej_stacker);
	DDX_Control(pDX, IDC_GROUP_SELECTED_TRAY, m_group_selected_tray);
	DDX_Control(pDX, IDC_GROUP_SSD_AUTO_SORTER, m_group_ssd_auto_sorter);
	DDX_Control(pDX, IDC_GROUP_BIN_INFO, m_group_bin_info);
	DDX_Control(pDX, IDC_BTN_DOOR_OPEN, m_btn_door_open);
	DDX_Control(pDX, IDC_GROUP_B_LOT_INFO, m_group_b_lot_info);
	DDX_Control(pDX, IDC_DGT_ALARM, m_dgt_alarm);
	DDX_Control(pDX, IDC_MSG_STOP, m_msg_stop);
	DDX_Control(pDX, IDC_MSG_RUN, m_msg_run);
	DDX_Control(pDX, IDC_MSG_ALARM, m_msg_alarm);
	DDX_Control(pDX, IDC_MSG_MTBI, m_msg_mtbi);
	DDX_Control(pDX, IDC_GROUP_DAILY_YIELD_INFO, m_group_daily_yield_info);
	DDX_Control(pDX, IDC_GROUP_TIME_INFO, m_group_time_info);
	DDX_Control(pDX, IDC_GROUP_LOT_INFO, m_group_lot_info);
	DDX_Control(pDX, IDC_GROUP_LOT_YIELD_INFO, m_group_lot_yield_info);
	DDX_Control(pDX, IDC_DGT_STOP, m_dgt_stop);
	DDX_Control(pDX, IDC_DGT_RUN, m_dgt_run);
	DDX_Control(pDX, IDC_DGT_MTBI, m_dgt_mtbi);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreen_Main, CFormView)
	//{{AFX_MSG_MAP(CScreen_Main)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DOOR_OPEN, OnBtnDoorOpen)
	ON_BN_CLICKED(IDC_BTN_TEST1, OnBtnTest1)
	ON_BN_CLICKED(IDC_BTN_TEST2, OnBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, OnBtnTest3)
	ON_BN_CLICKED(IDC_BTN_TEST4, OnBtnTest4)
	ON_BN_CLICKED(IDC_BTN_TEST5, OnBtnTest5)
	ON_BN_CLICKED(IDC_BTN_TEST6, OnBtnTest6)
	ON_BN_CLICKED(IDC_BTN_TRAYMINUS, OnBtnTrayminus)
	ON_BN_CLICKED(IDC_MSG_P_LOT_INFO, OnMsgPLotInfo)
	ON_BN_CLICKED(IDC_BTN_STACKER_OFF, OnBtnStackerOff)
	ON_BN_CLICKED(IDC_BUTTON_XGEM, OnButtonXgem)
	ON_BN_CLICKED(IDC_BUTTON_FILLRATIO, OnButtonFillratio)
	ON_BN_CLICKED(IDC_BTN_ABORT, OnBtnAbort)
	ON_BN_DOUBLECLICKED(IDC_BTN_DOOR_OPEN, OnDoubleclickedBtnDoorOpen)
	ON_BN_CLICKED(IDC_BTN_MANAGER_SOCKET, OnBtnManagerSocket)
	//}}AFX_MSG_MAP
 	ON_MESSAGE(WM_WORK_END, OnMain_Work_Info_Display)  // 테스트 결과 정보 화면에 출력하기 위한 사용자 정의 메시지 추가 
	ON_MESSAGE( WM_DRAW_UI_MAIN, OnMain_Draw_UI)
	ON_MESSAGE( WM_DRAW_DATA_MAIN, OnMain_Draw_Data)
 	ON_MESSAGE(SSM_CLICK, OnCellClick)

	ON_MESSAGE( WM_CHANGE_VIEW_TYPE, OnMain_Change_View )
// 	ON_MESSAGE(WM_RECEIVED_NEW_LOT, OnReceivedNewLot)  // 새로운 Lot을 받았다.
// 	ON_MESSAGE(WM_DRAW_LOT_TIME, OnDrawLotTime)
// 	ON_MESSAGE(WM_DRAW_WORK_SITE, OnDrawWorkSite)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Main diagnostics

#ifdef _DEBUG
void CScreen_Main::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Main::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Main message handlers

void CScreen_Main::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();

	int i = 0;

	// ************************************************************************** 
	// 컨트롤에 설정할 폰트 생성한다                                              
	// ************************************************************************** 
// 	mp_main_font = NULL;
// 	mp_main_font = new CFont;
// 	mp_main_font->CreateFont(15,0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");

// 	mp_main_big_font = NULL;
// 	mp_main_big_font = new CFont;
// 	mp_main_big_font->CreateFont(21,0,0,0,0,0,0,0,0,0,0,DEFAULT_QUALITY,0,"2002L");
	// ************************************************************************** 
	OnMain_GroupBox_Set();
	OnMain_Label_Set();
	OnMain_Digital_Count_Set();
	OnMain_Init_Lot_Yield_info();
	OnMain_Init_Daily_Yield_info();
	OnMain_Init_Lot_Info();
	OnMain_Init_B_Lot_Info();
	OnMain_Init_Bin_Info();

	OnMain_Init_COK_TSITE();
	OnMain_Init_COK_CNT();
	OnMain_Init_TrayMove();
	OnMain_Init_Reject_Floor();
	OnMain_Change_Reject_Floor();
	OnMain_Init_SelectedTray();
	OnMain_Change_Selected_Tray();
	OnMain_Init_TestSite( 0 );
	OnMain_Init_TestSite( 1 );
	OnMain_Display_TestSite();

	OnMain_Init_COK_Robot();
	OnMain_Init_Robot_Module();
	OnMain_Init_BinTray(0);
	OnMain_Init_BinTray(1);
	OnMain_Init_BinTray(2);//ybs
	OnMain_Init_NetUsable();

	OnMain_Init_Door();
	OnMain_Init_SMema();
	OnMain_Init_Bypass();
	OnMain_Init_RecvNext();

	OnMain_Display_Lot_Info();
	OnMain_Display_Lot_Yield_info();
	OnMain_Display_Daily_Yield_info();
	OnMain_Display_NetUsable();
	OnMain_Display_Bin_Info();

	OnMain_Change_BinTray(0);
	OnMain_Change_BinTray(1);
	OnMain_Change_BinTray(2);

	OnMain_Draw_UI( 0, 0 );
	OnDrawVideo();
	OnMain_display_RobotPicker();

	OnMain_Init_Ref_Color();

	OnMain_Init_P_Lot_Info(); //2013,0913
	OnMain_Display_P_Lot_Info();

	SetTimer(1234, 500, 0);

	mn_time_select = 0;	
	
	st_handler.cwnd_main = this;		// MAIN 화면 핸들 설정


	m_iClick = -1;
	m_dwTimeClicked = -1;
	m_iClickCnt = 0;

	m_emvt_ts = EMVT_SERIAL;
	m_emvt_mdl = EMVT_SERIAL;

	//2013,1001
	/*if(CHINA_VER == 1)*/	m_btnBufferIn.ShowWindow(false);
	m_btn_abort.ShowWindow(false);
}

void CScreen_Main::OnDestroy() 
{
	st_handler.cwnd_main = NULL;		// MAIN 화면 핸들 초기화
	
	// **************************************************************************
	// 생성한 폰트 정보 삭제한다
	// **************************************************************************
	
//   	delete mp_main_font;
// 	mp_main_font = NULL;

// 	delete mp_main_big_font;
// 	mp_main_big_font = NULL;
	// **************************************************************************

	CFormView::OnDestroy();
}

void CScreen_Main::OnTimer(UINT nIDEvent) 
{
	int nResponse = CTLBD_RET_GOOD;
	
	switch(nIDEvent)
	{
	case 1234:
		{
			OnMain_Display_Door();
			OnMain_Display_Smema();
			OnMain_Display_Bypass();
			OnMain_Display_RecvNext();
		}
		break;
	case 200:

		if (nResponse == CTLBD_RET_GOOD)
		{
			KillTimer(200);
		}
		break;
	}

	CFormView::OnTimer(nIDEvent);
}

void CScreen_Main::OnMain_GroupBox_Set()
{
	CSxLogFont main_font(13, FW_THIN, false, "Bitstream Vera Sans Mono");

	m_group_lot_info.SetFont(main_font);
	m_group_lot_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_lot_info.SetFontBold(TRUE);

	m_group_b_lot_info.SetFont(main_font);
	m_group_b_lot_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_b_lot_info.SetFontBold(TRUE);
	
	m_group_daily_yield_info.SetFont(main_font);
	m_group_daily_yield_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_daily_yield_info.SetFontBold(TRUE);
	
	m_group_lot_yield_info.SetFont(main_font);
	m_group_lot_yield_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_lot_yield_info.SetFontBold(TRUE);
		
	m_group_time_info.SetFont(main_font);
	m_group_time_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_time_info.SetFontBold(TRUE);

	m_group_bin_info.SetFont(main_font);
	m_group_bin_info.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_bin_info.SetFontBold(TRUE);

	m_group_ssd_auto_sorter.SetFont(main_font);
	m_group_ssd_auto_sorter.SetCatptionTextColor(RGB(145, 25, 0));
	m_group_ssd_auto_sorter.SetFontBold(TRUE);

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		m_group_work1.SetWindowText(_T("Work 3"));
		m_group_conv1.SetWindowText(_T("Conv 3"));
		m_group_work3.SetWindowText(_T("Work 1"));
		m_group_conv3.SetWindowText(_T("Conv 1"));
	}
	else
	{
		m_group_work1.SetWindowText(_T("Work 1"));
		m_group_conv1.SetWindowText(_T("Conv 1"));
		m_group_work3.SetWindowText(_T("Work 3"));
		m_group_conv3.SetWindowText(_T("Conv 3"));
	}

	m_group_p_lot_info.SetFont(main_font);	// P_LOT 2013,0910
	m_group_p_lot_info.SetCatptionTextColor(RGB(145, 25, 0));	// P_LOT 2013,0910
	m_group_p_lot_info.SetFontBold(TRUE);	// P_LOT 2013,0910

}

void CScreen_Main::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	Grid->SetColor(col, row, bk, tk);
	Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::GridControl(UINT nID, int type, int row, int col, int pos)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	SS_CELLTYPE CellType;
	SS_DATEFORMAT dateFormat = { TRUE, '/', IDF_DDMMYY, FALSE };
	SS_TIMEFORMAT tmFormat;
	CString strList;
	strList = "combo1\tcombo2\tcombo3\tcombo4\tcombo5\tcombo6";
	double a = 0.5;

	switch (type)
	{
		case DATE:			
			Grid->SetTypeDate(&CellType, 0 , &dateFormat, NULL, NULL);
			break;

		case EDIT:
			Grid->SetTypeEdit(&CellType,ES_CENTER,60,SS_CHRSET_CHR,SS_CASE_NOCASE);
			break;

		case COMBO:
			// SetValue(col, row, data); data = ""0"(1선택) data = "1"(2선택) data = "2"(3선택)
			Grid->SetTypeComboBox(&CellType, 0, strList);
			break;

		case BUTTON:
			if (pos == 0)
			{
				Grid->SetTypeButton(&CellType, SBS_DEFPUSHBUTTON, "Go", NULL, SUPERBTN_PICT_NONE,
					NULL, SUPERBTN_PICT_NONE, SUPERBTN_TYPE_NORMAL, 2, NULL);
			}
			else
			{
				Grid->SetTypeButton(&CellType, SBS_DEFPUSHBUTTON, "Read", NULL, SUPERBTN_PICT_NONE,
					NULL, SUPERBTN_PICT_NONE,SUPERBTN_TYPE_NORMAL, 2, NULL);
			}
			break;

		case STATIC:
			Grid->SetTypeStaticText(&CellType, SS_TEXT_CENTER | SS_TEXT_VCENTER);
			break;

		case TIME:
			tmFormat.b24Hour=TRUE;
			tmFormat.bSpin=FALSE;
			tmFormat.bSeconds=FALSE;
			tmFormat.cSeparator=':';
			Grid->SetTypeTime(&CellType, 0, &tmFormat, NULL, NULL);
			break;

		case PERCENT:
			Grid->SetTypePercentEx(&CellType,ES_LEFTALIGN,2,.01,100,".",0,0,0,0,1.001);
			break;

		case CHECK1:
			Grid->SetTypeCheckBox(&CellType,BS_CENTER, "", 
				"CheckUp", BT_BITMAP,
				"CheckDown", BT_BITMAP,
				"CheckFocusUp", BT_BITMAP,
				"CheckFocusDown", BT_BITMAP,
				"CheckDisabledUp", BT_BITMAP,
				"CheckDisabledDown", BT_BITMAP);
			break;

		case NUMBER:
			Grid->SetTypeNumberEx(&CellType,0,pos, 0, 10000,0,".",",",0,0,1,0,1.001);
			break;
	}

	Grid->SetCellType(col,row,&CellType);

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Label_Set()
{
	m_msg_alarm.SetFont(Func.mp_main_font);
	m_msg_alarm.SetWindowText(_T("Alarm"));
	m_msg_alarm.SetCenterText();
	m_msg_alarm.SetColor(RGB(0, 0, 0));
	m_msg_alarm.SetGradientColor(RGB(255, 255, 0));
	m_msg_alarm.SetTextColor(RGB(255, 255, 255));
	
	m_msg_run.SetFont(Func.mp_main_font);
	m_msg_run.SetWindowText(_T("Run"));
	m_msg_run.SetCenterText();
	m_msg_run.SetColor(RGB(0, 0, 0));
	m_msg_run.SetGradientColor(RGB(0, 0, 255));
	m_msg_run.SetTextColor(RGB(255, 255, 255));
	
	m_msg_stop.SetFont(Func.mp_main_font);
	m_msg_stop.SetWindowText(_T("Stop"));
	m_msg_stop.SetCenterText();
	m_msg_stop.SetColor(RGB(0, 0, 0));
	m_msg_stop.SetGradientColor(RGB(255, 0, 0));
	m_msg_stop.SetTextColor(RGB(255, 255, 255));

	m_msg_mtbi.SetFont(Func.mp_main_font);
	m_msg_mtbi.SetWindowText(_T("MTBI"));
	m_msg_mtbi.SetCenterText();
	m_msg_mtbi.SetColor(RGB(0, 0, 0));
	m_msg_mtbi.SetGradientColor(ORANGE_C);
	m_msg_mtbi.SetTextColor(RGB(255, 255, 255));

	m_msg_p_lot_info.SetFont(Func.mp_main_font);			// P_LOT 2013,0910
	m_msg_p_lot_info.SetWindowText(_T("P LOT INFO"));
	m_msg_p_lot_info.SetCenterText();
	m_msg_p_lot_info.SetColor(RGB(0, 0, 0));
	m_msg_p_lot_info.SetGradientColor(RGB(0, 255, 0));
	m_msg_p_lot_info.SetTextColor(RGB(255, 255, 255));			// P_LOT 2013,0910

}

void CScreen_Main::OnMain_Digital_Count_Set()
{
	m_dgt_alarm.SetStyle(CDigit::DS_TIMER, NULL, CDigit::DC_YELLOW, CDigit::DC_CYAN);
	m_dgt_run.SetStyle(CDigit::DS_TIMER, NULL, CDigit::DC_YELLOW, CDigit::DC_CYAN);
	m_dgt_stop.SetStyle(CDigit::DS_TIMER, NULL, CDigit::DC_YELLOW, CDigit::DC_CYAN);
	m_dgt_mtbi.SetStyle(CDigit::DS_TIMER, NULL, CDigit::DC_YELLOW, CDigit::DC_CYAN);
}

void CScreen_Main::OnMain_Init_Lot_Yield_info()
{
	TSpread *Grid;
	CString str_pos;
	int row = 5;
	int col = 3;
	int i, j;

	CString str_tmp[4] = {"Input", "Pass", "Fail", "Yield"};
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_LOT_YIELD_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for (i = 1; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 20);

			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 42);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 56);
			}

			if (i == 1 && j == 1)
			{
				GridColor(IDC_CUSTOM_LOT_YIELD_INFO, i, j, ORANGE_C, WHITE_C);
			}
			else if (i == 1 || j == 1)
			{
				GridColor(IDC_CUSTOM_LOT_YIELD_INFO, i, j, PINK_D, BLACK_C);
			}
			else
			{
				if (j == 4)
				{
					GridColor(IDC_CUSTOM_LOT_YIELD_INFO, i, j, BLACK_C, RED_C);
				}
				else
				{
					GridColor(IDC_CUSTOM_LOT_YIELD_INFO, i, j, BLACK_C, GREEN_B);
				}
			}

			GridControl(IDC_CUSTOM_LOT_YIELD_INFO, STATIC, i, j, 0);
			GridFont(IDC_CUSTOM_LOT_YIELD_INFO, i, j, 15);
		}
	}

	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 1, 1, " ");
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 1, 2, "Prime");
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 1, 3, "Cum");

	for (i = 2; i < row + 1; i++)
	{
		GridData(IDC_CUSTOM_LOT_YIELD_INFO, i, 1, str_tmp[i - 2]);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_Daily_Yield_info()
{
	TSpread *Grid;
	CString str_pos;
	int row = 5;
	int col = 3;
	int i, j;

	CString str_tmp[4] = {"Input", "Pass", "Fail", "Yield"};
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_DAILY_YIELD_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for (i = 1; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 20);
			
			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 42);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 56);
			}
			
			if (i == 1 && j == 1)
			{
				GridColor(IDC_CUSTOM_DAILY_YIELD_INFO, i, j, ORANGE_C, WHITE_C);
			}
			else if (i == 1 || j == 1)
			{
				GridColor(IDC_CUSTOM_DAILY_YIELD_INFO, i, j, PINK_D, BLACK_C);
			}
			else
			{
				if (j == 4)
				{
					GridColor(IDC_CUSTOM_DAILY_YIELD_INFO, i, j, BLACK_C, RED_C);
				}
				else
				{
					GridColor(IDC_CUSTOM_DAILY_YIELD_INFO, i, j, BLACK_C, GREEN_B);
				}
			}
			GridControl(IDC_CUSTOM_DAILY_YIELD_INFO, STATIC, i, j, 0);
			GridFont(IDC_CUSTOM_DAILY_YIELD_INFO, i, j, 15);
		}
	}

	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 1, 1, " ");
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 1, 2, "Prime");
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 1, 3, "Cum");
	
	for (i = 2; i < row + 1; i++)
	{
		GridData(IDC_CUSTOM_DAILY_YIELD_INFO, i, 1, str_tmp[i - 2]);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_Lot_Info()
{
	TSpread *Grid;
	CString str_pos;
	int row = 5;
	int col = 2;
	int i, j;

//	CString str_tmp[4] = {"Lot No", "Part No", "Last Mdl", "ByPass"};
	//2013,0910
	CString str_tmp[5] = {"Lot No", "Part No", "Last Mdl", "R_PryEnd", "ByPass"};
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_LOT_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for (i = 0; i <= row; i++)
	{
		for (j = 0; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 20);
  			
			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 50);
				GridColor(IDC_CUSTOM_LOT_INFO, i, j, YELLOW_L, BLACK_C);

				GridControl(IDC_CUSTOM_LOT_INFO, STATIC, i, j, 0);
				GridFont(IDC_CUSTOM_LOT_INFO, i, j, 14);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 105);
				GridColor(IDC_CUSTOM_LOT_INFO, i, j, BLACK_C, WHITE_C);

				GridControl(IDC_CUSTOM_LOT_INFO, STATIC, i, j, 0);

				if (i == 2)
				{
					GridFont(IDC_CUSTOM_LOT_INFO, i, j, 10);
				}
				else
				{
					GridFont(IDC_CUSTOM_LOT_INFO, i, j, 15);
				}
			}
		}
	}

	for (i = 1; i < row + 1; i++)
	{
		GridData(IDC_CUSTOM_LOT_INFO, i, 1, str_tmp[i - 1]);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_B_Lot_Info()
{
	TSpread *Grid;
	CString str_pos;
	int row = 2;
	int col = 2;
	int i, j;
	
	CString str_tmp[2] = {"Lot No", "Part No"};
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_B_LOT_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for (i = 0; i <= row; i++)
	{
		for (j = 0; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 24);
			
			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 50);
				GridColor(IDC_CUSTOM_B_LOT_INFO, i, j, YELLOW_L, BLACK_C);
				
				GridControl(IDC_CUSTOM_B_LOT_INFO, STATIC, i, j, 0);
				GridFont(IDC_CUSTOM_B_LOT_INFO, i, j, 15);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 105);
				GridColor(IDC_CUSTOM_B_LOT_INFO, i, j, BLACK_C, WHITE_C);
				
				GridControl(IDC_CUSTOM_B_LOT_INFO, STATIC, i, j, 0);
				
				if (i == 2)
				{
					GridFont(IDC_CUSTOM_B_LOT_INFO, i, j, 13);
				}
				else
				{
					GridFont(IDC_CUSTOM_B_LOT_INFO, i, j, 15);
				}
			}
		}
	}
	
	for (i = 1; i < row + 1; i++)
	{
		GridData(IDC_CUSTOM_B_LOT_INFO, i, 1, str_tmp[i - 1]);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Time_Display(int n_state)
{
	m_dgt_run.ActivateTimer(FALSE);
	m_dgt_stop.ActivateTimer(FALSE);
	m_dgt_alarm.ActivateTimer(FALSE);
	m_dgt_mtbi.ActivateTimer(FALSE);
	
	mn_time_select = 0;
	switch(n_state)
	{
	case dRUN:
		m_dgt_run.ActivateTimer();
		m_dgt_mtbi.ActivateTimer();
		if (mn_time_select == 0)
		{
			m_dgt_run.SetTime(st_handler.m_tDR.GetTotalHours(), st_handler.m_tDR.GetMinutes(), st_handler.m_tDR.GetSeconds());
			int nSec = st_handler.m_tDR.GetTotalHours() * 3600 + st_handler.m_tDR.GetMinutes() * 60 + st_handler.m_tDR.GetSeconds();
			if( g_handler.GetAlarmCnt() == 0 )
				nSec = nSec;
			else
			{
				nSec /= g_handler.GetAlarmCnt();
			}
			
			m_dgt_mtbi.SetTime( nSec / 3600, (nSec % 3600) / 60, nSec % 60 );
		}
		else
		{
			m_dgt_run.SetTime(st_handler.m_tR.GetTotalHours(), st_handler.m_tR.GetMinutes(), st_handler.m_tR.GetSeconds());
			int nSec = st_handler.m_tR.GetTotalHours() * 3600 + st_handler.m_tR.GetMinutes() * 60 + st_handler.m_tR.GetSeconds();
			if( g_handler.GetAlarmCnt() == 0 )
				nSec = nSec;
			else
			{
				nSec /= g_handler.GetAlarmCnt();
			}

			m_dgt_mtbi.SetTime( nSec / 3600, (nSec % 3600) / 60, nSec % 60 );
		}
		break;
		
	case dWARNING:
	case dJAM:
		m_dgt_alarm.ActivateTimer();
		if (mn_time_select == 0)
		{
			m_dgt_alarm.SetTime(st_handler.m_tDJ.GetTotalHours(), st_handler.m_tDJ.GetMinutes(), st_handler.m_tDJ.GetSeconds());
		}
		else
		{
			m_dgt_alarm.SetTime(st_handler.m_tJ.GetTotalHours(), st_handler.m_tJ.GetMinutes(), st_handler.m_tJ.GetSeconds());
		}
		break;
		
	case dLOCK:
	case dSTOP:
		m_dgt_stop.ActivateTimer();
		if (mn_time_select == 0)
		{
			m_dgt_stop.SetTime(st_handler.m_tDS.GetTotalHours(), st_handler.m_tDS.GetMinutes(), st_handler.m_tDS.GetSeconds());
		}
		else
		{
			m_dgt_stop.SetTime(st_handler.m_tS.GetTotalHours(), st_handler.m_tS.GetMinutes(), st_handler.m_tS.GetSeconds());
		}
		break;
	}
	m_dgt_run.UpdateTimer();
	m_dgt_stop.UpdateTimer();
	m_dgt_alarm.UpdateTimer();
	m_dgt_mtbi.UpdateTimer();
}

void CScreen_Main::OnMain_TimeInfo_Display()
{
	m_dgt_run.ActivateTimer(FALSE);
	m_dgt_stop.ActivateTimer(FALSE);
	m_dgt_alarm.ActivateTimer(FALSE);
	m_dgt_mtbi.ActivateTimer(FALSE);

	
	if (mn_time_select == 0)
	{
		m_dgt_run.SetTime(st_handler.m_tDR.GetTotalHours(), st_handler.m_tDR.GetMinutes(), st_handler.m_tDR.GetSeconds());
		m_dgt_stop.SetTime(st_handler.m_tDS.GetTotalHours(), st_handler.m_tDS.GetMinutes(), st_handler.m_tDS.GetSeconds());
		m_dgt_alarm.SetTime(st_handler.m_tDJ.GetTotalHours(), st_handler.m_tDJ.GetMinutes(), st_handler.m_tDJ.GetSeconds());

		int nSec = st_handler.m_tDR.GetTotalHours() * 3600 + st_handler.m_tDR.GetMinutes() * 60 + st_handler.m_tDR.GetSeconds();
		if( g_handler.GetAlarmCnt() == 0 )
			nSec = nSec;
		else
		{
			nSec /= g_handler.GetAlarmCnt();
		}
			
		m_dgt_mtbi.SetTime( nSec / 3600, (nSec % 3600) / 60, nSec % 60 );
	}
	else
	{
		m_dgt_run.SetTime(st_handler.m_tR.GetTotalHours(), st_handler.m_tR.GetMinutes(), st_handler.m_tR.GetSeconds());
		m_dgt_stop.SetTime(st_handler.m_tS.GetTotalHours(), st_handler.m_tS.GetMinutes(), st_handler.m_tS.GetSeconds());
		m_dgt_alarm.SetTime(st_handler.m_tJ.GetTotalHours(), st_handler.m_tJ.GetMinutes(), st_handler.m_tJ.GetSeconds());

		int nSec = st_handler.m_tR.GetTotalHours() * 3600 + st_handler.m_tR.GetMinutes() * 60 + st_handler.m_tR.GetSeconds();
		if( g_handler.GetAlarmCnt() == 0 )
			nSec = nSec;
		else
		{
			nSec /= g_handler.GetAlarmCnt();
		}

		m_dgt_mtbi.SetTime( nSec / 3600, (nSec % 3600) / 60, nSec % 60 );
	}
	
	m_dgt_run.UpdateTimer();
	m_dgt_stop.UpdateTimer();
	m_dgt_alarm.UpdateTimer();
	m_dgt_mtbi.UpdateTimer();
}

LRESULT CScreen_Main::OnMain_Work_Info_Display(WPARAM wParam,LPARAM lParam) 
{
	int mn_state = wParam;
	int nOpt = lParam;
	
	switch(mn_state)
	{	
	case MAIN_TIMEINFO:
		OnMain_Time_Display(lParam);
		break;

	case MAIN_COUNTINFO:
		OnMain_Display_Lot_Yield_info();
		OnMain_Display_Daily_Yield_info();
		OnMain_Display_Bin_Info();
		break;

	case MAIN_LOTINFO:
		OnMain_Display_Lot_Info();
		OnMain_Display_P_Lot_Info();
		break;

	case MAIN_CYCLETIME:
		break;
		
	////2015.0604
	case MAIN_FTPDOWN:
		FtpConnect();
		FtpFileDown();
		Func.Load_scrap_code_Data();
		break;
	////
	default:
		break;
	}

	return 0;
}

LRESULT CScreen_Main::OnMain_Draw_UI(WPARAM wParam,LPARAM lParam)
{
	m_btn_door_open.SetWindowText( g_local.Get( "L_U_0000") );
	return 0;
}

LRESULT CScreen_Main::OnMain_Draw_Data(WPARAM wParam,LPARAM lParam)
{
	switch( wParam )
	{
	case EDDC_SITE:			OnDrawCOKSite( lParam );		break;
	case EDDC_COKSTACKER:	OnDrawCokStacker( lParam );		break;

	case EDDT_INIT:			OnMain_Init_TrayMove( lParam );	break;

	case EDDT_REJ_FLOOR:	OnMain_Change_Reject_Floor();	break;
	case EDDT_SEL_TRAY:		OnMain_Change_Selected_Tray();	break;
	case EDDT_BINTRAY_0:	OnMain_Change_BinTray(0);		break;
	case EDDT_BINTRAY_1:	OnMain_Change_BinTray(1);		break;
	case EDDT_BINTRAY_2:	OnMain_Change_BinTray(2);		break;//ybs

	case EDDR_PICKER:		OnMain_display_RobotPicker();	break;
	case EDDS_TESTSITE:		OnMain_Display_TestSite();		break;
	case EDDS_VIDEO:		OnDrawVideo();					break;

	case EDDN_NETUSABLE:	OnMain_Display_NetUsable();		break;
	}

	return 0;
}

void CScreen_Main::OnBtnDoorOpen() 
{
	int nRet, nRet2;
	
	nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
	nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);

	if (nRet == IO_ON && nRet2 == IO_ON)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "Manual Mode가 아닙니다.");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "The Machine is not manual mode.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return;
	}

	if (COMI.mn_run_status != dSTOP)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "장비가 Stop 상태가 아닙니다..");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "The Machine is not Stop.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return ;
	} 
	
	g_ioMgr.set_out_bit(st_io.o_door_lock, IO_OFF);
}

void CScreen_Main::OnMain_Display_Lot_Info()
{
	TSpread *Grid;	// P_LOT 2013,0910
	TSpread *Grid2;	// P_LOT 2013,0910
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_PLOT_INFO);	// P_LOT 2013,0910
	Grid2 = (TSpread*)GetDlgItem(IDC_CUSTOM_BIN_INFO);	// P_LOT 2013,0910

	if( g_lotMgr.GetLotCount() > 0 )
	{
		GridData(IDC_CUSTOM_LOT_INFO, 1, 2, g_lotMgr.GetLotIDAt( 0 ) );
		GridData(IDC_CUSTOM_LOT_INFO, 2, 2, g_lotMgr.GetLotAt(0).GetPartID() );
		GridData(IDC_CUSTOM_LOT_INFO, 3, 2, g_lotMgr.GetLotAt(0).GetStrLastModule());
		GridData(IDC_CUSTOM_LOT_INFO, 4, 2, g_lotMgr.GetLotAt(0).GetStrRProtyModule());
//		GridData(IDC_CUSTOM_LOT_INFO, 4, 2, g_lotMgr.GetLotAt(0).GetStrBypass());
		GridData(IDC_CUSTOM_LOT_INFO, 5, 2, g_lotMgr.GetLotAt(0).GetStrBypass());//2013,0910

	}
	else
	{
		GridData(IDC_CUSTOM_LOT_INFO, 1, 2, "" );
		GridData(IDC_CUSTOM_LOT_INFO, 2, 2, "" );
		GridData(IDC_CUSTOM_LOT_INFO, 3, 2, "");
		GridData(IDC_CUSTOM_LOT_INFO, 4, 2, "");
		GridData(IDC_CUSTOM_LOT_INFO, 5, 2, "");
	}

	if( g_lotMgr.GetLotCount() > 1 )
	{
		GridData( IDC_CUSTOM_B_LOT_INFO, 1, 2, g_lotMgr.GetLotIDAt( 1 ) );
		GridData( IDC_CUSTOM_B_LOT_INFO, 2, 2, g_lotMgr.GetLotAt( 1 ).GetPartID() );
	}
	else
	{
		GridData( IDC_CUSTOM_B_LOT_INFO, 1, 2, "" );
		GridData( IDC_CUSTOM_B_LOT_INFO, 2, 2, "" );
	}

	if ( st_handler.m_iPLotInfo == CTL_YES )	// P_LOT 2013,0910
	{
		Grid->ShowWindow( TRUE );
		Grid2->ShowWindow( FALSE );
		
		m_group_p_lot_info.ShowWindow( TRUE );
		m_group_lot_info.ShowWindow( FALSE );
		m_group_b_lot_info.ShowWindow( FALSE );
		m_group_daily_yield_info.ShowWindow( FALSE );
		m_group_lot_yield_info.ShowWindow( FALSE );
		
		m_msg_p_lot_info.SetWindowText(_T("P LOT INFO"));
		m_msg_p_lot_info.SetGradientColor(RGB(255, 0, 0));
	}
	else	// P_LOT  2013,0910
	{	
		Grid->ShowWindow( FALSE );
		Grid2->ShowWindow( TRUE );
		
		m_group_p_lot_info.ShowWindow( FALSE );
		m_group_lot_info.ShowWindow( TRUE );
		m_group_b_lot_info.ShowWindow( TRUE );
		m_group_daily_yield_info.ShowWindow( TRUE );
		m_group_lot_yield_info.ShowWindow( TRUE );
		
		m_msg_p_lot_info.SetWindowText(_T("CURRENT LOT INFO"));
		m_msg_p_lot_info.SetGradientColor(RGB(0, 0, 255));
	}
}

void CScreen_Main::OnMain_Display_Lot_Yield_info()
{	//st_work.nMdlInputCount
	CString strTemp;
	int nLotNum = -1;
	
	// Prime
	strTemp.Format( "%06d", g_lotMgr.GetLotAt(0).GetInputCnt(PRIME) );
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 2, 2, strTemp);		// Input
	strTemp.Format( "%06d", g_lotMgr.GetLotAt(0).GetPassCnt(PRIME) );
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 3, 2, strTemp);		// Pass
	strTemp.Format( "%06d", g_lotMgr.GetLotAt(0).GetFailCnt(PRIME) );
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 4, 2, strTemp);		// Reject

	double dYield = 0;
	if (g_lotMgr.GetLotAt(0).GetInputCnt(PRIME) > 0)
	{
		dYield = (double)g_lotMgr.GetLotAt(0).GetPassCnt(PRIME) / (double)g_lotMgr.GetLotAt(0).GetInputCnt(PRIME) * 100.0f;
	}

	strTemp.Format("%3.2f%%", dYield);
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 5, 2, strTemp);		// Yield

	//2013,0913
	if(st_handler.m_strProcessID == "SMFA")
	{
		for (int i = 0; i < g_lotMgr.GetLotCount(); i++)
		{
			nLotNum = g_lotMgr.IsGetPLotID(g_lotMgr.GetLotIDAt(i));
			if( nLotNum >= 0 && nLotNum < MAX_PLOT)
			{
				st_work.nPLotInputCount[nLotNum][PRIME] = g_lotMgr.GetLotAt(i).GetInputCnt(PRIME);
				st_work.nPLotPassCount[nLotNum][PRIME] = g_lotMgr.GetLotAt(i).GetPassCnt(PRIME);
				st_work.nPLotFailCount[nLotNum][PRIME] = g_lotMgr.GetLotAt(i).GetFailCnt(PRIME);
				st_work.fPLotYield[nLotNum][PRIME] = dYield;
			}
		}
	}


	// Cum
	strTemp.Format("%06d", g_lotMgr.GetLotAt(0).GetInputCnt(CUM));
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 2, 3, strTemp);		// Input
	strTemp.Format("%06d", g_lotMgr.GetLotAt(0).GetPassCnt(CUM));
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 3, 3, strTemp);		// Pass
	strTemp.Format("%06d", g_lotMgr.GetLotAt(0).GetFailCnt(CUM));
	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 4, 3, strTemp);		// Reject

	dYield = 0;
	if (g_lotMgr.GetLotAt(0).GetInputCnt(CUM) > 0)
	{
		dYield = (double)g_lotMgr.GetLotAt(0).GetPassCnt(CUM) / (double)g_lotMgr.GetLotAt(0).GetInputCnt(CUM) * 100.0f;
	}
	
	strTemp.Format("%3.2f%%", dYield);

	GridData(IDC_CUSTOM_LOT_YIELD_INFO, 5, 3, strTemp);		// Yield

	//2013,0913
	if(st_handler.m_strProcessID == "SMFA")
	{
		for (int i = 0; i < g_lotMgr.GetLotCount(); i++)
		{
			nLotNum = g_lotMgr.IsGetPLotID(g_lotMgr.GetLotIDAt(i));
			if( nLotNum >= 0 && nLotNum < MAX_PLOT)
			{
				st_work.nPLotInputCount[nLotNum][CUM] = g_lotMgr.GetLotAt(i).GetInputCnt(CUM);
				st_work.nPLotPassCount[nLotNum][CUM] = g_lotMgr.GetLotAt(i).GetPassCnt(CUM);
				st_work.nPLotFailCount[nLotNum][CUM] = g_lotMgr.GetLotAt(i).GetFailCnt(CUM);
				st_work.fPLotYield[nLotNum][CUM] = dYield;
			}
		}
	}
}	

void CScreen_Main::OnMain_Display_Daily_Yield_info()
{
	CString strTemp;
	// Prime
	strTemp.Format( "%06d", g_lotMgr.GetInputCnt(PRIME) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 2, 2, strTemp);		// Input
	strTemp.Format( "%06d", g_lotMgr.GetPassCnt(PRIME) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 3, 2, strTemp);		// Pass
	strTemp.Format( "%06d", g_lotMgr.GetFailCnt(PRIME) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 4, 2, strTemp);		// Reject
	
	double dYield = 0;
	if (g_lotMgr.GetInputCnt(PRIME) > 0)
	{
		dYield = (double)g_lotMgr.GetPassCnt(PRIME) / (double)g_lotMgr.GetInputCnt(PRIME) * 100.0f;
	}
	strTemp.Format("%3.2f%%", dYield);
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 5, 2, strTemp);		// Yield
	
	// Retest
	strTemp.Format( "%06d", g_lotMgr.GetInputCnt(CUM) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 2, 3, strTemp);		// Input
	strTemp.Format( "%06d", g_lotMgr.GetPassCnt(CUM) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 3, 3, strTemp);		// Pass
	strTemp.Format( "%06d", g_lotMgr.GetFailCnt(CUM) );
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 4, 3, strTemp);		// Reject

	dYield = 0;
	if (g_lotMgr.GetInputCnt(CUM) > 0)
	{
		dYield = (double)g_lotMgr.GetPassCnt(CUM) / (double)g_lotMgr.GetInputCnt(CUM) * 100.0f;
	}

	strTemp.Format("%3.2f%%", dYield);
	GridData(IDC_CUSTOM_DAILY_YIELD_INFO, 5, 3, strTemp);		// Yield
}

void CScreen_Main::OnMain_Init_Bin_Info()
{
	TSpread *Grid;
	CString str_pos, str_temp;
	int row = 9;
	int col = 3;
	int i, j;
	
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_BIN_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, TRUE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, TRUE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for (i = 0; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 20);
			
			if (j == 1)
			{
				Grid -> SetColWidthInPixels(j, 32);
			}
			else
			{
				Grid -> SetColWidthInPixels(j, 52);
			}
			
			if (i == 0 && j == 1)
			{
				GridColor(IDC_CUSTOM_BIN_INFO, i, j, ORANGE_C, WHITE_C);
			}
			else if (i == 0 || j == 1)
			{
				GridColor(IDC_CUSTOM_BIN_INFO, i, j, PINK_D, BLACK_C);
			}
			else
			{
				if (j == 4)
				{
					GridColor(IDC_CUSTOM_BIN_INFO, i, j, BLACK_C, RED_C);
				}
				else
				{
					GridColor(IDC_CUSTOM_BIN_INFO, i, j, BLACK_C, GREEN_B);
				}
			}
			GridControl(IDC_CUSTOM_BIN_INFO, STATIC, i, j, 0);
			GridFont(IDC_CUSTOM_BIN_INFO, i, j, 15);
		}
	}
	
	GridData(IDC_CUSTOM_BIN_INFO, 0, 1, "Bin");
	GridData(IDC_CUSTOM_BIN_INFO, 0, 2, "Daily");
	GridData(IDC_CUSTOM_BIN_INFO, 0, 3, "Lot");
	
	for (i = 1; i < row + 1; i++)
	{
		str_temp.Format("%d", i);
		GridData(IDC_CUSTOM_BIN_INFO, i, 1, str_temp);
	}
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_COK_TSITE()
{
	TSpread *Grid;
	int row = 21;
	int col = 1;
	int i;

	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_COK_TSITE);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 54);
	for (i = 1; i <= row; i++)
	{
		Grid -> SetRowHeightInPixels(i, 13.5);
		if( i > 1 )
			GridColor(IDC_CUSTOM_COK_TSITE, i, 1, GetCokCol( g_handler_cok.GetSiteCok( i - 2 )), BLACK_C);

		GridControl(IDC_CUSTOM_COK_TSITE, STATIC, i, 1, 0);
		GridFont(IDC_CUSTOM_COK_TSITE, i, 1, 15);
	}
	GridColor(IDC_CUSTOM_COK_TSITE, 1, 1, BLACK, WHITE_C);
	GridData(IDC_CUSTOM_COK_TSITE, 1, 1, "T-SITE");

	for (i = 2; i <= row; i++)
	{
		CString strSiteName;
		strSiteName.Format( "S-%d [%02d]", (i-2) / 10 + 1, (i-2) % 10 + 1);
		GridData(IDC_CUSTOM_COK_TSITE, i, 1, strSiteName );
		
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_COK_CNT()
{
	TSpread *Grid;
	int row = 7;
	int col = 1;
	int i;

	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_COK_CNT);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 37);
	for (i = 1; i <= row; i++)
	{
		Grid -> SetRowHeightInPixels(i, 17);
		if( i > 1 )	
		{
			CString strData;
			strData.Format( "%d", g_handler_cok.GetCokCnt( i - 2) );
			GridColor(IDC_CUSTOM_COK_CNT, i, 1, GetCokCol( i - 2 ), BLACK);
			GridData(IDC_CUSTOM_COK_CNT, i, 1, strData );
		}
		GridControl(IDC_CUSTOM_COK_CNT, STATIC, i, 1, 0);
	}
	GridFont(IDC_CUSTOM_COK_CNT, 1, 1, 15);
	GridMerge( IDC_CUSTOM_COK_CNT, 1, 1, 1, 2 );
	GridColor(IDC_CUSTOM_COK_CNT, 1, 1, BLACK, WHITE_C);
	GridData(IDC_CUSTOM_COK_CNT, 1, 1, "CNT");

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnDrawCOKSite( int iIdx )
{
	GridColor(IDC_CUSTOM_COK_TSITE, iIdx + 2, 1, GetCokCol( g_handler_cok.GetSiteCok( iIdx )), BLACK );
	GridColor( IDC_CUSTOM_COK_ROBOT, 1, 1, GetCokCol( Run_Robot.GetCokID() ), BLACK );
}

void CScreen_Main::OnDrawCokStacker( int iIdx )
{
	CString strData;
			strData.Format( "%d", g_handler_cok.GetCokCnt( iIdx ) );
	GridData(IDC_CUSTOM_COK_CNT, iIdx + 2, 1, strData);
	GridColor( IDC_CUSTOM_COK_ROBOT, 1, 1, GetCokCol( Run_Robot.GetCokID() ), BLACK );
}

void CScreen_Main::OnBtnTest1() 
{
//	if( g_lotMgr.GetLotCount() == 0 )
//		return;

	//ybs 2012,1128
	if(st_handler.mn_buffer_in != 1)
	{
		CString strErr;
		strErr.Format( "Admin 화면: Buffer In 세팅을 해 주세요" );
		st_msg.mstr_event_msg[0] = strErr;
		if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_event_msg[0] = "Admin screen: Buffer In the settings, please.";//20130930

		::SendMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		return;
	}

	static int nstBufferNo = 0;

//	if( g_handler.GetControlConv( 0 ) == EC_NONE )
//	{
////		g_handler.ControlConv(0, EC_RECEIVE );
//	}
//	else
//	{
		int nCnt = st_basic.n_tray_x * st_basic.n_tray_y;
		CString strBufferIn;
		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=SRS-01 LOTID=%s BUFFER_NO=%02d LAST_MDL=NO EMPTY_TRAY=NO POCKET_CNT=%d ", g_lotMgr.GetLotIDAt(0),
			nstBufferNo + 1, nCnt );
// 		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=PS-GL LOTID=Y001000130 BUFFER_NO=%02d LAST_MDL=NO EMPTY_TRAY=YES POCKET_CNT=%d ", g_lotMgr.GetLotIDAt(0),
// 			nstBufferNo + 1, nCnt );
		for( int i=1; i<=nCnt; i++ )
		{
			int iIdx = i + nCnt * nstBufferNo;
			CString strTemp;
			strTemp.Format( " POCKET_%02d=(ARRAYSN=MZCPA2561127000%05d_3 SERIAL=S11ZNYAC5%05d PPID=KR0NMY6F018511CJ02%05d WWN= BIN=)", i,
				iIdx, iIdx, iIdx );
// 			strTemp.Format( " POCKET_%02d=NONE", i);
			strBufferIn += strTemp;
		}
		
		g_server_front.OnReceived_BufferIn( strBufferIn ); 
		
		nstBufferNo++;

		//2013,0330
		CString mstr_temp;		
		st_handler.mn_buffer_in = 0;		
		mstr_temp.Format("%d", st_handler.mn_buffer_in);
		:: WritePrivateProfileString("BasicData", "Buffer_In", LPCTSTR(mstr_temp), st_path.mstr_basic);

//	}
}

void CScreen_Main::OnMain_Init_TrayMove()
{
	for( int i=0; i<16; i++ )
	{
		RECT rt;
		rt.left = 0;
		rt.top = 0;
		rt.right = 0;
		rt.bottom = 0;
		GetDlgItem( GetCustom_TrayMove(i) )->MoveWindow( &rt );	
	}

	for( i=0; i<MAX_TRAY; i++ )
	{
		OnMain_Init_TrayMove(i);
	}
}

void CScreen_Main::OnMain_Init_TrayMove( int iIdx )
{
	if( iIdx < 0 )	
		return;

	if( g_Tray.GetTray(iIdx).GetPos() == TPOS_NONE )
	{
		SetTrayPos( iIdx );
		return;
	}

	TSpread *Grid;

	int col = st_basic.n_tray_x;
	int row = st_basic.n_tray_y;

	bool bTurn = (g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN1 ||
		g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN2 ||
		g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN3 ||
		g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT ||
		g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN_TO_REJECT ||
		g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT_TO_BIN );
	
	if( bTurn && 
		(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		row = st_basic.n_tray_x;
		col = st_basic.n_tray_y;
	}
	
	int iGrid = GetCustom_TrayMove(iIdx);
	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	int iw=0,ih=0;
	//2013,0306
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( iw=row-1; iw>=0; iw-- )
		{
			Grid -> SetRowHeightInPixels(iw + 1, 120 / row );
			for( ih=0; ih<col; ih++ )
			{
				if( iw == 0 )
					Grid -> SetColWidthInPixels(ih + 1, 120 / col );
				
				GridFont( iGrid, iw + 1, ih + 1, 15);
				GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
				GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn );
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
				
				if( g_Tray.GetTray(iIdx).IsExistModule(iMdlIdx - 1) )
				{
					GridColor( iGrid, iw + 1, ih + 1, CalcModuleColor( g_Tray.GetTray(iIdx).GetModule(iMdlIdx - 1) ), BLACK_C);
				}
			}
		}
	}
	else
	{
		for( iw=0; iw<row; iw++ )
		{
			Grid -> SetRowHeightInPixels(iw + 1, 120 / row );
			for( ih=0; ih<col; ih++ )
			{
				if( iw == 0 )
					Grid -> SetColWidthInPixels(ih + 1, 120 / col );
				
				GridFont( iGrid, iw + 1, ih + 1, 15);
				GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
				GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn );
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
				
				if( g_Tray.GetTray(iIdx).IsExistModule(iMdlIdx - 1) )
				{
					GridColor( iGrid, iw + 1, ih + 1, CalcModuleColor( g_Tray.GetTray(iIdx).GetModule(iMdlIdx - 1) ), BLACK_C);
				}
			}
		}
	}	
	
	Grid = NULL;
	delete Grid;

	SetTrayPos( iIdx );
}

int CScreen_Main::GetCustom_TrayMove( int iIdx )
{
	switch( iIdx )
	{
	case 0: return IDC_CUSTOM_TM_1;
	case 1: return IDC_CUSTOM_TM_2;
	case 2: return IDC_CUSTOM_TM_3;
	case 3: return IDC_CUSTOM_TM_4;
	case 4: return IDC_CUSTOM_TM_5;
	case 5: return IDC_CUSTOM_TM_6;
	case 6: return IDC_CUSTOM_TM_7;
	case 7: return IDC_CUSTOM_TM_8;
	case 8: return IDC_CUSTOM_TM_9;
	case 9: return IDC_CUSTOM_TM_10;
	case 10: return IDC_CUSTOM_TM_11;
	case 11: return IDC_CUSTOM_TM_12;
	case 12: return IDC_CUSTOM_TM_13;
	case 13: return IDC_CUSTOM_TM_14;
	case 14: return IDC_CUSTOM_TM_15;
	case 15: return IDC_CUSTOM_TM_16;
	}

	return IDC_CUSTOM_TM_1;
}

void CScreen_Main::SetTrayPos( int iIdx )
{
	RECT rt;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		switch( g_Tray.GetTray(iIdx).GetPos() )
		{
		case TPOS_NONE:					rt.left = 0;	rt.top = 0;	rt.right = 0;			rt.bottom = 0;			break;
		case TPOS_CONV1:				rt.left = 469;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV2:				rt.left = 341;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV3:				rt.left = 213;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_FRONT_TO_CONV1:		rt.left = 485;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV1_TO_CONV2:		rt.left = 405;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV2_TO_CONV3:		rt.left = 277;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV3_TO_NEXT:		rt.left = 190;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN1:			rt.left = 469;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN2:			rt.left = 341;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN3:			rt.left = 213;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV1:			rt.left = 469;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV2:			rt.left = 341;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV3:			rt.left = 213;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN1:					rt.left = 469;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN2:					rt.left = 341;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN3:					rt.left = 213;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_REJECT:		rt.left = 469;	rt.top = 438;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_REJECT_TO_BIN:		rt.left = 469;	rt.top = 438;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_REJECT:				
			if( g_Tray.GetRejSelectedTray() == iIdx )
			{
				rt.left = 630;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
			}
			else
			{
				rt.left = 0;	rt.top = 0;		rt.right = 0;				rt.bottom = 0;
			}
			break;
			
		default:						rt.left = 0;	rt.top = 0;		rt.right = 120;				rt.bottom = 120;			break;	
		}		
	}
	else
	{
		switch( g_Tray.GetTray(iIdx).GetPos() )
		{
		case TPOS_NONE:					rt.left = 0;	rt.top = 0;		rt.right = 0;				rt.bottom = 0;				break;
		case TPOS_CONV1:				rt.left = 213;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV2:				rt.left = 341;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV3:				rt.left = 469;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_FRONT_TO_CONV1:		rt.left = 190;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV1_TO_CONV2:		rt.left = 277;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV2_TO_CONV3:		rt.left = 405;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV3_TO_NEXT:		rt.left = 485;	rt.top = 648;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN1:			rt.left = 213;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN2:			rt.left = 341;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_CONV_TO_BIN3:			rt.left = 469;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV1:			rt.left = 213;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV2:			rt.left = 341;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_CONV3:			rt.left = 469;	rt.top = 578;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN1:					rt.left = 213;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN2:					rt.left = 341;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN3:					rt.left = 469;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_BIN_TO_REJECT:		rt.left = 469;	rt.top = 438;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_REJECT_TO_BIN:		rt.left = 469;	rt.top = 438;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
		case TPOS_REJECT:				
			if( g_Tray.GetRejSelectedTray() == iIdx )
			{
				rt.left = 630;	rt.top = 508;	rt.right = rt.left + 120;	rt.bottom = rt.top + 120;	break;
			}
			else
			{
				rt.left = 0;	rt.top = 0;		rt.right = 0;				rt.bottom = 0;
			}
			break;
			
		default:						rt.left = 0;	rt.top = 0;		rt.right = 120;				rt.bottom = 120;			break;	
		}
	}

	GetDlgItem( GetCustom_TrayMove(iIdx) )->MoveWindow( &rt );
}

void CScreen_Main::OnMain_Init_Reject_Floor()
{
	TSpread* Grid;
	int row = 5;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_REJ_FLOOR );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 120 / row );
		for( int ih=0; ih<col; ih++ )
		{
			if( iw == 0 )
				Grid -> SetColWidthInPixels(ih + 1, 120 );
			
			GridFont( IDC_CUSTOM_REJ_FLOOR, iw + 1, ih + 1, 15);
			GridControl(IDC_CUSTOM_REJ_FLOOR, STATIC, iw + 1, ih + 1, 0);
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnCellClick( WPARAM wParam, LPARAM lParam )
{
	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	if( lpcc->Col == 1 && lpcc->Row == 1 )
	{
		CalcPopupMenuByClick( wParam );
	}

	if(wParam==IDC_CUSTOM_REJ_FLOOR )
	{
		int iSelectedIdx = 5 - lpcc->Row;
		int iSelectedTray_Old = g_Tray.GetRejSelectedTray();
		g_Tray.SetRejSelected( iSelectedIdx );
		int iSelectedTray = g_Tray.GetRejSelectedTray();

		if( iSelectedTray_Old != iSelectedTray )
		{
			OnMain_Change_Reject_Floor();
			OnMain_Init_TrayMove( iSelectedTray_Old );
			OnMain_Init_TrayMove( iSelectedTray );
		}
		return;
	}

	if( wParam == IDC_CUSTOM_SITE_1 )
	{
		if( lpcc->Row > 2 )
		{
			if( (g_ShortKey.IsPressed( VK_CONTROL ) || g_ShortKey.IsPressed( VK_LCONTROL ) ) &&
				m_emvt_ts == EMVT_TS_TESTCNT )
			{
				g_ShortKey.SetPressed( VK_CONTROL, false );
				g_ShortKey.SetPressed( VK_LCONTROL, false );

				CString strQuestion;
				strQuestion.Format( "%02d 소켓 사용 카운트를 초기화 하시겠습니까?", lpcc->Row - 2 );
				if ( g_local.GetLocalType() == LOCAL_ENG ) strQuestion.Format( "%02d Socket Do you want to reset test count?", lpcc->Row - 2 );
				if( DoModal_Select( strQuestion ) == IDOK )
				{
					g_site.ClearTestCount( lpcc->Row - 3 );
					OnMain_Display_TestSite();
					return;
				}
			}
		}
		return;
	}

	if( wParam == IDC_CUSTOM_SITE_2 )
	{
		if( lpcc->Row > 2 )
		{
			if( (g_ShortKey.IsPressed( VK_CONTROL ) || g_ShortKey.IsPressed( VK_LCONTROL ) ) &&
				m_emvt_ts == EMVT_TS_TESTCNT )
			{
				g_ShortKey.SetPressed( VK_CONTROL, false );
				g_ShortKey.SetPressed( VK_LCONTROL, false );

				CString strQuestion;
				strQuestion.Format( "%02d 소켓 사용 카운트를 초기화 하시겠습니까?", 10 + lpcc->Row - 2 );
				if ( g_local.GetLocalType() == LOCAL_ENG ) strQuestion.Format( "%02d Socket Do you want to reset test count?", lpcc->Row - 2 );
				if( DoModal_Select( strQuestion ) == IDOK )
				{
					g_site.ClearTestCount( 10 + lpcc->Row - 3 );
					OnMain_Display_TestSite();
					return;
				}
			}
		}
		return;
	}

	if( wParam == IDC_CUSTOM_NET_USABLE )
	{
		if( lpcc->Row == 2 && g_client_bpc.GetUsable() == false )
		{
			g_client_bpc.OnClientClose();//2013,1031
			g_client_bpc.ClearClient();
			g_client_bpc.SetUsable( true );
			OnMain_Display_NetUsable();
		}//2013,0718
		else if( lpcc->Row == 3 && g_client_ec.GetUsable() == false )
		{
			g_client_ec.OnClientClose();//2013,1031
			g_client_ec.ClearClient();
			g_client_ec.SetUsable( true );
			OnMain_Display_NetUsable();
		}
		else if( lpcc->Row == 4 && g_client_front.GetUsable() == false )
		{
			g_client_front.OnClientClose();//2013,1031
			g_client_front.ClearClient();
			g_client_front.SetUsable( true );
			g_client_front.PushSendMsg("Front Server Test");
			OnMain_Display_NetUsable();
		}
		else if( lpcc->Row == 5 && g_client_next.GetUsable() == false )
		{
			g_client_next.OnClientClose();//2013,1031
			g_client_next.ClearClient();
			g_client_next.PushSendMsg( "NEXT Client Test" );
			g_client_next.SetUsable( true );
			OnMain_Display_NetUsable();
		}
		else if( lpcc->Row == 6 && g_client_xgem.GetUsable() == false )
		{
			g_client_xgem.OnClientClose();
			g_client_xgem.ClearClient();
			g_client_xgem.PushSendMsg( "XGEM Client Test" );
			g_client_xgem.SetUsable( true );
			OnMain_Display_NetUsable();
		}
		return;
	}


	if( wParam == IDC_CUSTOM_TM_1 )		{ g_Tray.SetSelectedTray(0);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_2 )		{ g_Tray.SetSelectedTray(1);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_3 )		{ g_Tray.SetSelectedTray(2);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_4 )		{ g_Tray.SetSelectedTray(3);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_5 )		{ g_Tray.SetSelectedTray(4);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_6 )		{ g_Tray.SetSelectedTray(5);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_7 )		{ g_Tray.SetSelectedTray(6);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_8 )		{ g_Tray.SetSelectedTray(7);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_9 )		{ g_Tray.SetSelectedTray(8);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_10 )	{ g_Tray.SetSelectedTray(9);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_11 )	{ g_Tray.SetSelectedTray(10);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_12 )	{ g_Tray.SetSelectedTray(11);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_13 )	{ g_Tray.SetSelectedTray(12);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_14 )	{ g_Tray.SetSelectedTray(13);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_15 )	{ g_Tray.SetSelectedTray(14);	OnMain_Change_Selected_Tray(); return; } 
	if( wParam == IDC_CUSTOM_TM_16 )	{ g_Tray.SetSelectedTray(15);	OnMain_Change_Selected_Tray(); return; } 
}

void CScreen_Main::OnMain_Change_Reject_Floor()
{
	int iRejCnt = g_Tray.GetStopRejCnt();
	int iRejSelectedIdx = g_Tray.GetRejSelected();

	for( int iw=0; iw<5; iw++ )
	{
		COLORREF col = BLACK_L;
		CString strData = "";
		if( 5 - iw <= iRejCnt )						{col = BLUE_L;}
		if( 5 - iw - 1 == iRejSelectedIdx )			{col = YELLOW_L;	strData = "Selected";}
		GridColor( IDC_CUSTOM_REJ_FLOOR, iw + 1, 1, col, BLACK_C);
		GridData( IDC_CUSTOM_REJ_FLOOR, iw + 1, 1, strData );
	}
}

void CScreen_Main::OnMain_Init_SelectedTray()
{
	TSpread* Grid;
	int col = 2;
	int row = 27;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_SELECTED_TRAY );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	Grid -> SetColWidthInPixels(1, 20 );
	Grid -> SetColWidthInPixels(2, 248 );
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 13.5 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( IDC_CUSTOM_SELECTED_TRAY, iy + 1, ix + 1, 13.5);
			GridControl(IDC_CUSTOM_SELECTED_TRAY, STATIC, iy + 1, ix + 1, 0);

			if( iy >= iMdlCnt + 2 )
				GridColor( IDC_CUSTOM_SELECTED_TRAY, iy + 1, ix + 1, BLACK, BLACK );
			else
				GridColor( IDC_CUSTOM_SELECTED_TRAY, iy + 1, ix + 1, WHITE_C, BLACK );
		}

		if( iy >= 2 )
		{
			CString strIdx;
			strIdx.Format("%02d", iy - 1 );
			GridData( IDC_CUSTOM_SELECTED_TRAY, iy + 1, 1, strIdx );
		}
	}

	GridColor( IDC_CUSTOM_SELECTED_TRAY, 1, 1, BLACK_GC, WHITE_C );
	GridMerge( IDC_CUSTOM_SELECTED_TRAY, 1, 1, 1, 2 );
	GridColor( IDC_CUSTOM_SELECTED_TRAY, 2, 1, GRAY_C, WHITE_C );
	GridColor( IDC_CUSTOM_SELECTED_TRAY, 2, 2, YELLOW_D, WHITE_C );

	GridData( IDC_CUSTOM_SELECTED_TRAY, 2, 2, "SERIAL" );
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Change_Selected_Tray()
{
	int iSelected = g_Tray.GetSelectedTray();
	ATray tray;
	if( iSelected >= 0 )
	{
		tray = g_Tray.GetTray(iSelected);	
	}
	
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i<iMdlCnt; i++ )
	{
		if( tray.GetModule(i).IsExist() )
		{
			GridColor( IDC_CUSTOM_SELECTED_TRAY, 3 + i, 2, CalcModuleColor( tray.GetModule(i) ), BLACK_C );

			CString strData;
			switch( m_emvt_mdl )
			{
			case EMVT_SERIAL:		strData = tray.GetModule(i).GetSerial();					break;
			case EMVT_LOTNO:		strData = tray.GetModule(i).GetLotNo();						break;
			case EMVT_SN:			strData = tray.GetModule(i).GetSN();						break;
			case EMVT_PPID:			strData = tray.GetModule(i).GetPPID();						break;
			case EMVT_WWN:			strData = tray.GetModule(i).GetWWN();						break;
			case EMVT_CSN:			strData = tray.GetModule(i).GetCSN();						break;
			case EMVT_PSID:			strData = tray.GetModule(i).GetPSID();						break;
			case EMVT_BIN:			strData = tray.GetModule(i).GetBin();						break;
			case EMVT_SCRAPCODE:	strData = tray.GetModule(i).GetScrapCode();					break;
			case EMVT_TESTCNT:		strData.Format( "%d", tray.GetModule(i).GetTestCnt() );		break;
			case EMVT_TESTEDPOS:
				{
					strData = "";
					int nTestCnt = tray.GetModule(i).GetTestCnt();
					if( nTestCnt > 0 )
					{
						strData.Format( "%d", tray.GetModule(i).GetTestedPos( nTestCnt - 1 ) + 1 );
					}
				}
				break;
			}
			GridData( IDC_CUSTOM_SELECTED_TRAY, 3 + i, 2, strData );
		}
		else
		{
			GridColor( IDC_CUSTOM_SELECTED_TRAY, 3 + i, 2, WHITE_C, BLACK_C );
			GridData( IDC_CUSTOM_SELECTED_TRAY, 3 + i, 2, "" );
		}
	}

	CString strTrayInfo;
	if( tray.GetPos() == TPOS_NONE )
	{
		strTrayInfo = "NONE";
	}
	else
	{
		strTrayInfo.Format( "[%s] ->[%s] {%s}", 
		g_Tray.GetStrPos(tray.GetPos() ), g_Tray.GetStrPos( tray.GetTargetPos() ), tray.GetStrLotID() );
	}
	GridData( IDC_CUSTOM_SELECTED_TRAY, 1, 1, strTrayInfo );
	GridData( IDC_CUSTOM_SELECTED_TRAY, 2, 2, GetStringEMVT( m_emvt_mdl ) );
}

void CScreen_Main::OnBtnTest2() 
{
// 
// 	g_handler.ControlConv(0, EC_RECEIVE );
// 
// 	int nCnt = st_basic.n_tray_x * st_basic.n_tray_y;
// 	CString strBufferIn;
// 	strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=SRS-01 LOTID=AAA0001 BUFFER_NO=01 LAST_MDL=NO EMPTY_TRAY=YES POCKET_CNT=%d ", 0);
// 
// 	g_server_front.OnReceived_BufferIn( strBufferIn );

	if( g_lotMgr.GetLotCount() > 0 )
	{
		// 뒷 설비로 전달.
		g_client_next.SetNewLotInID( g_lotMgr.GetLotIDAt(0) );
		
		// bpc로 전달.
		g_client_bpc.SetNewLotInID( g_lotMgr.GetLotIDAt(0) );
	}
}

void CScreen_Main::OnMain_Init_TestSite( int iIdx )
{
	int iGrid = IDC_CUSTOM_SITE_1;
	CString strTitle = "TEST SITE 1";
	if( iIdx == 1 )
	{
		iGrid = IDC_CUSTOM_SITE_2;
		strTitle = "TEST SITE 2";
	}

	TSpread* Grid;
	int col = 2;
	int row = 10 + 2;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid -> SetColWidthInPixels(1, 30 );
	Grid -> SetColWidthInPixels(2, 141 );
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 30 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 15);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );
		}

		if( iy >= 2 )
		{
			CString strIdx;
			strIdx.Format("%02d", iy - 1 );
			GridData( iGrid, iy + 1, 1, strIdx );
		}
	}

	GridMerge( iGrid, 1, 1, 1, 2 );
	GridMerge( iGrid, 2, 1, 1, 2 );
	GridData( iGrid, 1, 1, strTitle );

	GridColor( iGrid, 1, 1, BLACK_GC, WHITE_C );

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Init_COK_Robot()
{
	TSpread* Grid;
	int row = 1;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_COK_ROBOT );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetRowHeightInPixels(1, 23 );
	Grid -> SetColWidthInPixels(1, 82 );
	GridFont( IDC_CUSTOM_COK_ROBOT, 1, 1, 15);
	GridControl(IDC_CUSTOM_COK_ROBOT, STATIC, 1, 1, 0);
	GridData( IDC_CUSTOM_COK_ROBOT, 1, 1, "COK" );
	
	Grid = NULL;
	delete Grid;

	GridColor( IDC_CUSTOM_COK_ROBOT, 1, 1, GetCokCol( Run_Robot.GetCokID() ), BLACK );
}

void CScreen_Main::OnMain_Init_Robot_Module()
{
	TSpread* Grid;
	int row = 5;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_ROBOT_MODULE );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 20);
		Grid -> SetColWidthInPixels(1, 82 );
			
		GridFont( IDC_CUSTOM_ROBOT_MODULE, iw + 1, 1, 15);
		GridControl(IDC_CUSTOM_ROBOT_MODULE, STATIC, iw + 1, 1, 0);
	}

	GridColor( IDC_CUSTOM_ROBOT_MODULE, 1, 1, BLACK, WHITE_C );
	GridData( IDC_CUSTOM_ROBOT_MODULE, 1, 1, "Serial" );
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_display_RobotPicker()
{
	for( int i=0; i<PICKER_CNT; i++ )
	{
		if( g_robot.GetPicker(i).GetModule().IsExist() )
		{
			GridColor( IDC_CUSTOM_ROBOT_MODULE, 2 + i, 1, CalcModuleColor( g_robot.GetPicker(i).GetModule() ), BLACK_C );

			CString strData;
			switch( m_emvt_mdl )
			{
			case EMVT_SERIAL:		strData = g_robot.GetPicker(i).GetModule().GetSerial();						break;
			case EMVT_LOTNO:		strData = g_robot.GetPicker(i).GetModule().GetLotNo();						break;
			case EMVT_SN:			strData = g_robot.GetPicker(i).GetModule().GetSN();							break;
			case EMVT_PPID:			strData = g_robot.GetPicker(i).GetModule().GetPPID();						break;
			case EMVT_WWN:			strData = g_robot.GetPicker(i).GetModule().GetWWN();						break;
			case EMVT_CSN:			strData = g_robot.GetPicker(i).GetModule().GetCSN();							break;
			case EMVT_PSID:			strData = g_robot.GetPicker(i).GetModule().GetPSID();						break;
			case EMVT_BIN:			strData = g_robot.GetPicker(i).GetModule().GetBin();						break;
			case EMVT_SCRAPCODE:	strData = g_robot.GetPicker(i).GetModule().GetScrapCode();					break;
			case EMVT_TESTCNT:		strData.Format( "%d", g_robot.GetPicker(i).GetModule().GetTestCnt() );		break;
			case EMVT_TESTEDPOS:
				{
					strData = "";
					int nTestCnt = g_robot.GetPicker(i).GetModule().GetTestCnt();
					if( nTestCnt > 0 )
					{
						strData.Format( "%d", g_robot.GetPicker(i).GetModule().GetTestedPos( nTestCnt - 1 ) + 1 );
					}
				}
				break;
			}
			GridData( IDC_CUSTOM_ROBOT_MODULE, 2 + i, 1, strData );
		}
		else
		{
			GridColor( IDC_CUSTOM_ROBOT_MODULE, 2 + i, 1, WHITE_C, BLACK_C );
			GridData( IDC_CUSTOM_ROBOT_MODULE, 2 + i, 1, "" );
		}
	}
}

void CScreen_Main::OnMain_Display_TestSite()
{
	GridData( IDC_CUSTOM_SITE_1, 2, 1, GetStringEMVT( m_emvt_ts ) );
	GridData( IDC_CUSTOM_SITE_2, 2, 1, GetStringEMVT( m_emvt_ts ) );
	for( int i=0; i<TSITE_SOCKET_CNT; i++ )
	{
		int iGrid = IDC_CUSTOM_SITE_1;
		int x = 3 + i;
		int nVal = 0;

		if( i >= 10 )
		{
			iGrid = IDC_CUSTOM_SITE_2;
			x = 3 + i - 10;
		}

		//2016.0711
		COLORREF col = GRAY_C;
		if( g_site.GetEnableSocket(i) > 0  /*g_site.GetEnableSocket(i)*/ )
			col = CalcModuleColor( g_site.GetModule(i));
		else if( g_site.GetEnableSocket(i) < 0) //SOCKET MAX_COUNT OVER : -1 CONTINUE FAIL : -2 DPS : -3 REPAIR  : -4
		{
			if( g_site.GetEnableSocket(i) == -1 )
				col = CalcModuleColor(EMS_MAXCOUNT);
			else if( g_site.GetEnableSocket(i) == -2 )//CONTINUE FAIL : -2
				col = CalcModuleColor(EMS_COUNTINUE);
			else if( g_site.GetEnableSocket(i) == -3 )//DPS : -3
				col = CalcModuleColor(EMS_OPS);
			else //REPAIR  : -4
				col = CalcModuleColor(EMS_REPAIR);

		}
		
		GridColor( iGrid, x, 2, col, BLACK_C );
		
		if( g_site.GetModule(i).IsExist() || m_emvt_ts >= EMVT_USABLE || g_site.GetEnableSocket(i) <= 0/*g_site.GetEnableSocket(i) == false*/ )
		{
			CString strData;
			switch( m_emvt_ts )
			{
			case EMVT_SERIAL:		strData = g_site.GetModule(i).GetSerial();						break;
			case EMVT_LOTNO:		strData = g_site.GetModule(i).GetLotNo();						break;
			case EMVT_SN:			strData = g_site.GetModule(i).GetSN();							break;
			case EMVT_PPID:			strData = g_site.GetModule(i).GetPPID();						break;
			case EMVT_WWN:			strData = g_site.GetModule(i).GetWWN();							break;
			case EMVT_CSN:			strData = g_site.GetModule(i).GetCSN();							break;
			case EMVT_PSID:			strData = g_site.GetModule(i).GetPSID();						break;
			case EMVT_BIN:			strData = g_site.GetModule(i).GetBin();							break;
			case EMVT_SCRAPCODE:	strData = g_site.GetModule(i).GetScrapCode();					break;
			case EMVT_TESTCNT:		/*strData.Format( "%d", g_site.GetModule(i).GetTestCnt() );*/		break;
			case EMVT_TESTEDPOS:
				{
					strData = "";
// 					int nTestCnt = g_site.GetModule(i).GetTestCnt();
// 					if( nTestCnt > 0 )
// 					{
// 						strData.Format( "%d", g_site.GetModule(i).GetTestedPos( nTestCnt - 1 ) + 1 );
// 					}
				}
				break;
						
			case EMVT_USABLE:
				//strData = g_site.GetEnableSocket(i) ? "ENABLE" : "DISABLE";		break;
				nVal = g_site.GetEnableSocket(i);//2016.0709
				if(nVal <= 0 )  strData = "DISABLE";
				else            strData = "ENABLE";
				break;

			case EMVT_BOARDOK:		strData = g_site.GetBoardOn(i) ? "ON" : "OFF";					break;
////2015.0325
// 			case EMVT_TS_TESTCNT:	/*strData.Format( "%d", g_site.GetTestCount(i) );*/					break;
// 			case EMVT_BINHISTORY:	/*strData = g_site.GetBinHistory(i);*/								break;
			case EMVT_TS_TESTCNT:
				{
					strData = "";
					strData.Format( "%d", g_site.GetTestCount(i) );
				}
				break;
			case EMVT_BINHISTORY:	
				{
					strData.Format( "%s", g_site.GetBinHistory(i) );
					if( (strData.GetAt(0) >= '0' && strData.GetAt(0) <= '9') || strData.GetAt(0) == '/' )
					{
					}
					else
					{
						strData = "";
					}
				}	
				break;
////
			}
			strData.Format( "%s", strData );

			GridData( iGrid, x, 2, strData );
		}
		else
		{
			GridColor( iGrid, x, 2, WHITE_C, BLACK_C );
			GridData( iGrid, x, 2, "" );
		}
	}
}

void CScreen_Main::OnMain_Init_BinTray( int iIdx )
{
	int iGrid;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		iGrid = IDC_CUSTOM_BIN_TRAY_3;
		if( iIdx == 1 )
			iGrid = IDC_CUSTOM_BIN_TRAY_2;
		if( iIdx == 2 )//ybs
			iGrid = IDC_CUSTOM_BIN_TRAY_1;
	}
	else
	{
		iGrid = IDC_CUSTOM_BIN_TRAY_1;
		if( iIdx == 1 )
			iGrid = IDC_CUSTOM_BIN_TRAY_2;
		if( iIdx == 2 )//ybs
			iGrid = IDC_CUSTOM_BIN_TRAY_3;
	}
	
	TSpread* Grid;
	int col = 2;
	int row = 27;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	Grid -> SetColWidthInPixels(1, 20 );
	Grid -> SetColWidthInPixels(2, 176 );
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 13.5 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 13);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			if( iy >= iMdlCnt + 2 )
				GridColor( iGrid, iy + 1, ix + 1, BLACK, BLACK );
			else
				GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );
		}

		if( iy >= 2 )
		{
			CString strIdx;
			strIdx.Format("%02d", iy - 1 );
			GridData( iGrid, iy + 1, 1, strIdx );
		}
	}

	GridColor( iGrid, 1, 1, BLACK_GC, WHITE_C );
	GridMerge( iGrid, 1, 1, 1, 2 );
	GridColor( iGrid, 2, 1, GRAY_C, WHITE_C );
	GridColor( iGrid, 2, 2, YELLOW_D, WHITE_C );

	CString strTitle;
	strTitle.Format( "Work %d", iIdx + 1 );
	GridData( iGrid, 1, 1, strTitle );
	GridData( iGrid, 2, 2, "SERIAL" );
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Change_BinTray( int iIdx )
{
	int iGrid;
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		iGrid = IDC_CUSTOM_BIN_TRAY_3;
		if( iIdx == 1 )
			iGrid = IDC_CUSTOM_BIN_TRAY_2;
		if( iIdx == 2 )//ybs
			iGrid = IDC_CUSTOM_BIN_TRAY_1;
	}
	else
	{
		iGrid = IDC_CUSTOM_BIN_TRAY_1;
		if( iIdx == 1 )
			iGrid = IDC_CUSTOM_BIN_TRAY_2;
		if( iIdx == 2 )//ybs
			iGrid = IDC_CUSTOM_BIN_TRAY_3;
	}

	int iTray = g_Tray.GetIdxByPos( (ETRAY_POS)(TPOS_BIN1 + iIdx) );
	
	ATray tray;
	if( iTray >= 0 )
	{
		tray = g_Tray.GetTray( iTray );
	}
	else
	{
		return;
	}

	int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	for( int i=0; i<iMdlCnt; i++ )
	{
		if( tray.GetModule(i).IsExist() )
		{
			GridColor( iGrid, 3 + i, 2, CalcModuleColor( tray.GetModule(i) ), BLACK_C );

			CString strData;
			switch( m_emvt_mdl )
			{
			case EMVT_SERIAL:		strData = tray.GetModule(i).GetSerial();					break;
			case EMVT_LOTNO:		strData = tray.GetModule(i).GetLotNo();						break;
			case EMVT_SN:			strData = tray.GetModule(i).GetSN();						break;
			case EMVT_PPID:			strData = tray.GetModule(i).GetPPID();						break;
			case EMVT_WWN:			strData = tray.GetModule(i).GetWWN();						break;
			case EMVT_CSN:			strData = tray.GetModule(i).GetCSN();						break;
			case EMVT_PSID:			strData = tray.GetModule(i).GetPSID();						break;
			case EMVT_BIN:			strData = tray.GetModule(i).GetBin();						break;
			case EMVT_SCRAPCODE:	strData = tray.GetModule(i).GetScrapCode();					break;
			case EMVT_TESTCNT:		strData.Format( "%d", tray.GetModule(i).GetTestCnt() );		break;
			case EMVT_TESTEDPOS:
				{
					strData = "";
					int nTestCnt = tray.GetModule(i).GetTestCnt();
					if( nTestCnt > 0 )
					{
						strData.Format( "%d", tray.GetModule(i).GetTestedPos( nTestCnt  - 1 ) + 1 );
					}
				}
				break;
			}
			GridData( iGrid, 3 + i, 2, strData );
		}
		else
		{
			GridColor( iGrid, 3 + i, 2, WHITE_C, BLACK_C );
			GridData( iGrid, 3 + i, 2, "" );
		}
	}

	CString strTrayInfo;
	if( tray.GetPos() == TPOS_NONE )
	{
		strTrayInfo.Format( "WORK %d", iIdx + 1);
	}
	else
	{
		strTrayInfo.Format( "WORK %d [%s]", iIdx + 1, tray.GetStrLotID() );
	}
	GridData( iGrid, 1, 1, strTrayInfo );

	GridData( iGrid, 2, 2, GetStringEMVT( m_emvt_mdl ) );
}

void CScreen_Main::CalcPopupMenuByClick( int iCustom )
{
	if( iCustom != IDC_CUSTOM_LOT_INFO &&
		iCustom != IDC_CUSTOM_ROBOT_MODULE &&
		iCustom != IDC_CUSTOM_SITE_1 && 
		iCustom != IDC_CUSTOM_SITE_2 &&
		iCustom != IDC_CUSTOM_COK_ROBOT &&
		iCustom != IDC_CUSTOM_ROBOT_MODULE &&
		iCustom != IDC_CUSTOM_BYPASS &&
		iCustom != IDC_CUSTOM_RECV_NEXT )
	{
		return;
	}

	if( iCustom == m_iClick && 
		GetCurrentTime() - m_dwTimeClicked < 1000 )
	{
		m_iClickCnt++;	
	}
	else
	{
		m_iClickCnt = 1;
	}

	if( m_iClickCnt >= 3 )
	{
		switch( iCustom )
		{
		case IDC_CUSTOM_LOT_INFO:	
			{
				CDialog_Data_Lot dlgDataLot;
				dlgDataLot.DoModal();
			}
			break;

		case IDC_CUSTOM_SITE_1:
		case IDC_CUSTOM_SITE_2:
			{
				CDialog_Data_TestSite dlgDataTestSite;
				dlgDataTestSite.DoModal();
			}
			break;

		case IDC_CUSTOM_COK_ROBOT:
			{
				CDialog_Data_Cok dlgDataCok;
				dlgDataCok.DoModal();
			}
			break;

		case IDC_CUSTOM_ROBOT_MODULE:
			{
				CDialog_Data_Module dlgDataModule;
				dlgDataModule.DoModal();
			}
			break;
		case IDC_CUSTOM_BYPASS:
			{
				m_strTempMSG = "테스트 완료 바이패스 상태를 바꾸시겠습니까?";
				if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Do you want to change TEST Complete BYPASS Status?");
				if( DoModal_Select( m_strTempMSG ) == IDOK )
				{
					g_handler.SetReadySendNext( !g_handler.GetReadySendNext() );
				}
			}

			break;
		case IDC_CUSTOM_RECV_NEXT:
			{
				m_strTempMSG = "트레이 받는 상태를 바꾸시겠습니까?";
				if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Do you want to change Received Tray Status?");
				if( DoModal_Select( m_strTempMSG ) == IDOK )
				{
					g_handler.SetRecvStateNextMachine( !g_handler.GetRecvStateNextMachine() );
				}
			}
			break;
		}
		m_iClickCnt = 0;
	}

	m_iClick = iCustom;
	m_dwTimeClicked = GetCurrentTime();
}

void CScreen_Main::OnDrawVideo()
{
	for( int i=0; i<SITE_COK_CNT; i++ )
	{
		GridColor( IDC_CUSTOM_SITE_1, i + 3, 1, WHITE_C, BLACK_C );
		GridColor( IDC_CUSTOM_SITE_2, i + 3, 1, WHITE_C, BLACK_C );
	}

	int nVideo = g_handler.GetVideo();
	int iGrid = IDC_CUSTOM_SITE_1;
	if( nVideo >= 10 )
		iGrid = IDC_CUSTOM_SITE_2;

	if( nVideo >= 0 )
	{
		GridColor( iGrid, (nVideo % 10) + 3, 1, RED_L, BLACK_C );
	}
}

void CScreen_Main::OnMain_Init_NetUsable()
{
	TSpread *Grid;
	int row = 6;
	int col = 1;
	int i;

	int iGrid = IDC_CUSTOM_NET_USABLE;
	Grid = (TSpread*)GetDlgItem(iGrid);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 120);
	for (i = 1; i <= row; i++)
	{
		Grid -> SetRowHeightInPixels(i, 20);
		GridControl(iGrid, STATIC, i, 1, 0);
		GridFont(iGrid, i, 1, 15);
		GridColor(iGrid, i, 1, BLUE_L, BLACK_C);
	}
	GridData(iGrid, 1, 1, "R Sorter");
	GridData(iGrid, 2, 1, "TESTER");
	GridData(iGrid, 3, 1, "EC Server");
	GridData(iGrid, 4, 1, "Front Machine");
	GridData(iGrid, 5, 1, "Next Machine");
	GridData(iGrid, 6, 1, "XGEM");

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_NetUsable()
{
	for (int i = 1; i <= 6; i++)
	{
		GridColor(IDC_CUSTOM_NET_USABLE, i, 1, BLUE_L, BLACK_C);
	}

	if( g_client_router.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 1, 1, RED_C, BLACK_C );

	if( g_client_bpc.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 2, 1, RED_C, BLACK_C );

	if( g_client_ec.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 3, 1, RED_C, BLACK_C );

	if( g_client_front.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 4, 1, RED_C, BLACK_C );

	if( g_client_next.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 5, 1, RED_C, BLACK_C );

	//2013,1028
	if( g_client_xgem.GetUsable() == false )
		GridColor( IDC_CUSTOM_NET_USABLE, 6, 1, RED_C, BLACK_C );

}

void CScreen_Main::OnBtnTest3() 
{
	if( COMI.mn_run_status != dSTOP )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Is not a stationary state.");//20130930
		}
		else
		{
			DoModal_Msg( "정지상태가 아닙니다.");
		}
		return;
	}

	// 트레이 빈 공간이 없어야 한다.
	if( g_Tray.GetRejCnt() > 0 && g_Tray.GetVecRejEmptyCell().size() < (st_basic.n_tray_x * st_basic.n_tray_y) )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Is not empty tray on top.");//20130930
		}
		else
		{
			DoModal_Msg( "최상위에 빈트레이가 아닙니다.");
		}
		return;
	}

	// 5장 넘으면 안된다.
	if( g_Tray.GetRejCnt() >= 5 )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Reject stacker tray can not exceed 5..");//20130930
		}
		else
		{
			DoModal_Msg( "리젝트 스태커에는 트레이가 5장을 넘을 수 없습니다.");
		}
		
		return;
	}
	
	ATray tray;
	tray.SetTargetPos( TPOS_REJECT );
	if( g_lotMgr.GetLotCount() > 0 )
		tray.SetLotID( ""/*g_lotMgr.GetLotIDAt(0)*/ );

	g_Tray.AddTray( tray );
	g_Tray.TrayMoved( TPOS_NONE, TPOS_REJECT );
}

void CScreen_Main::OnMain_Display_Bin_Info()
{
	for( int i=0; i<BIN_MAX; i++ )
	{
		// LOT
		CString strBinCnt;
		strBinCnt.Format( "%04d", g_lotMgr.GetLotAt(0).GetBinCnt(i));
		GridData( IDC_CUSTOM_BIN_INFO, i + 1, 3, strBinCnt );

		// Daily
		strBinCnt.Format( "%04d", g_lotMgr.GetBinCnt(i) );
		GridData( IDC_CUSTOM_BIN_INFO, i + 1, 2, strBinCnt );
	}
}

void CScreen_Main::OnBtnTest4() 
{
	SYSTEMTIME st;
	st.wYear = 2012;
	st.wMonth = 1;
	st.wDay = 2;

	AMTRegistry::RegWriteTime( st );
}

void CScreen_Main::OnBtnTest5() 
{
// 	SYSTEMTIME st = AMTRegistry::RegReadTime();
// 	SYSTEMTIME st2;
// 	COleDateTime::GetAsSystemTime( st2 );
// 
// 	SYSTEMTIME st3 = st2 - st;
}

LRESULT CScreen_Main::OnMain_Change_View( WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case ECVT_TESTSITE:
		m_emvt_ts = (ENUM_MAIN_VIEW_TYPE)(m_emvt_ts + 1);
		if( m_emvt_ts <= EMVT_BOARDOK || m_emvt_ts >= EMVT_MAX )
			m_emvt_ts = EMVT_BOARDOK;

		OnMain_Display_TestSite();
		break;

	case ECVT_MODULE:
		m_emvt_mdl = (ENUM_MAIN_VIEW_TYPE)(m_emvt_mdl + 1);
		if( m_emvt_mdl >= EMVT_USABLE )
			m_emvt_mdl = EMVT_SERIAL;

		m_emvt_ts = m_emvt_mdl;

		OnMain_Display_TestSite();
		OnMain_display_RobotPicker();
		OnMain_Change_BinTray(0);
		OnMain_Change_BinTray(1);
		OnMain_Change_Selected_Tray();
		break;
	}

	return 0;
}

CString CScreen_Main::GetStringEMVT( ENUM_MAIN_VIEW_TYPE emvt )
{
	switch( emvt )
	{
	case EMVT_SERIAL:		return "SERIAL";
	case EMVT_LOTNO:		return "LOT NO";
	case EMVT_SN:			return "SN";
	case EMVT_PPID:			return "PPID";
	case EMVT_WWN:			return "WWN";
	case EMVT_CSN:			return "C_SN";
	case EMVT_PSID:			return "PSID";
	case EMVT_BIN:			return "BIN";
	case EMVT_SCRAPCODE:	return "SCRAP CODE";
	case EMVT_TESTCNT:		return "TEST COUNT";
	case EMVT_TESTEDPOS:	return "TESTED POSITION";
	
	case EMVT_USABLE:		return "ENABLE/DISABLE";
	case EMVT_BOARDOK:		return "BOARD ON/OFF";
	case EMVT_TS_TESTCNT:	return "SOCKET TEST COUNT";
	case EMVT_BINHISTORY:	return "BIN HISTORY";
	
	//EMVT_MAX,
	}

	return "";
}

void CScreen_Main::OnMain_Init_Ref_Color()
{
	TSpread* Grid;
	int row = 7;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_REF_COLOR );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 16.5);
		Grid -> SetColWidthInPixels(1, 82 );
			
		GridFont( IDC_CUSTOM_REF_COLOR, iw + 1, 1, 14);
		GridControl(IDC_CUSTOM_REF_COLOR, STATIC, iw + 1, 1, 0);
	}

	GridColor( IDC_CUSTOM_REF_COLOR, 1, 1, CalcModuleColor( EMS_LOAD ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 1, 1, "LOAD" );

	GridColor( IDC_CUSTOM_REF_COLOR, 2, 1, CalcModuleColor( EMS_READY ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 2, 1, "READY" );

	GridColor( IDC_CUSTOM_REF_COLOR, 3, 1, CalcModuleColor( EMS_TEST ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 3, 1, "TEST" );

	GridColor( IDC_CUSTOM_REF_COLOR, 4, 1, CalcModuleColor( EMS_ABORT ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 4, 1, "ABORT" );

	GridColor( IDC_CUSTOM_REF_COLOR, 5, 1, CalcModuleColor( EMS_GOOD ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 5, 1, "GOOD" );

	GridColor( IDC_CUSTOM_REF_COLOR, 6, 1, CalcModuleColor( EMS_FAIL ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 6, 1, "FAIL" );

	GridColor( IDC_CUSTOM_REF_COLOR, 7, 1, CalcModuleColor( EMS_RELOAD ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR, 7, 1, "RELOAD" );
	
	Grid = NULL;
	delete Grid;	

	TSpread* Grid1;
	row = 4;
	col = 1;
	
	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_REF_COLOR2 );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for( iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 14);
		Grid -> SetColWidthInPixels(1, 82 );
		
		GridFont( IDC_CUSTOM_REF_COLOR2, iw + 1, 1, 12);
		GridControl(IDC_CUSTOM_REF_COLOR2, STATIC, iw + 1, 1, 0);
	}
	
	GridColor( IDC_CUSTOM_REF_COLOR2, 1, 1, CalcModuleColor( EMS_MAXCOUNT ), WHITE_C );
	GridData( IDC_CUSTOM_REF_COLOR2, 1, 1, "MAX COUNT" );
	
	GridColor( IDC_CUSTOM_REF_COLOR2, 2, 1, CalcModuleColor( EMS_COUNTINUE ), WHITE_C );
	GridData( IDC_CUSTOM_REF_COLOR2, 2, 1, "CONTINUE FAIL" );
	
	GridColor( IDC_CUSTOM_REF_COLOR2, 3, 1, CalcModuleColor( EMS_OPS ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR2, 3, 1, "OPS" );
	
	GridColor( IDC_CUSTOM_REF_COLOR2, 4, 1, CalcModuleColor( EMS_REPAIR ), BLACK_C );
	GridData( IDC_CUSTOM_REF_COLOR2, 4, 1, "REPAIR" );
	
	Grid1 = NULL;
	delete Grid1;
}

void CScreen_Main::OnBtnTest6() 
{
	// TODO: Add your control notification handler code here

	return;
	g_Tray.SetSendNextFromRejForce( true );
	g_handler.CalcTrayMoveNext_fromReject();
	g_Tray.SetSendNextFromRejForce( false );
}

void CScreen_Main::OnMain_Init_Door()
{
	TSpread* Grid;
	int row = 3;
	int col = 2;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_DOOR );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 18);

		for( int iw = 0; iw<col; iw++ )
		{
			Grid -> SetColWidthInPixels(iw + 1, 93 );
			
			GridFont( IDC_CUSTOM_DOOR, ih + 1, iw + 1, 15);
			GridControl(IDC_CUSTOM_DOOR, STATIC, ih + 1, iw + 1, 0);
		}
	}

	GridData( IDC_CUSTOM_DOOR, 1, 1, "DOOR 4" );
	GridData( IDC_CUSTOM_DOOR, 1, 2, "DOOR 3" );
	GridData( IDC_CUSTOM_DOOR, 2, 1, "DOOR 1" );
	GridData( IDC_CUSTOM_DOOR, 2, 2, "DOOR 2" );
	
	GridData( IDC_CUSTOM_DOOR, 3, 1, "DOOR 5" );
	GridData( IDC_CUSTOM_DOOR, 3, 2, "DOOR 6" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_Door()
{
	COLORREF col = RED_C;
	if( g_ioMgr.get_in_bit( st_io.i_door4_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 1, 1, col, BLACK_C );

	col = RED_C;	if( g_ioMgr.get_in_bit( st_io.i_door3_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 1, 2, col, BLACK_C );

	col = RED_C;	if( g_ioMgr.get_in_bit( st_io.i_door1_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 2, 1, col, BLACK_C );

	col = RED_C;	if( g_ioMgr.get_in_bit( st_io.i_door2_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 2, 2, col, BLACK_C );

	col = RED_C;	if( g_ioMgr.get_in_bit( st_io.i_door5_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 3, 1, col, BLACK_C );

	col = RED_C;	if( g_ioMgr.get_in_bit( st_io.i_door6_chk ) == IO_ON )	col = BLUE_L;
	GridColor( IDC_CUSTOM_DOOR, 3, 2, col, BLACK_C );
}

void CScreen_Main::OnMain_Init_SMema()
{
	// Front
	TSpread* Grid;
	int row = 2;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_SMEMA_FRONT );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid->SetColWidthInPixels( 1, 90 );
	for( int ih=0; ih<row; ih++ )
	{
		Grid->SetRowHeightInPixels(ih + 1, 15);

		GridFont( IDC_CUSTOM_SMEMA_FRONT, ih + 1, 1, 15);
		GridControl(IDC_CUSTOM_SMEMA_FRONT, STATIC, ih + 1, 1, 0);
	}

	GridData( IDC_CUSTOM_SMEMA_FRONT, 1, 1, "F Tran" );
	GridData( IDC_CUSTOM_SMEMA_FRONT, 2, 1, "F Req" );

	Grid = NULL;
	delete Grid;


	// rear
	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_SMEMA_REAR );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid->SetColWidthInPixels( 1, 90 );
	for( ih=0; ih<row; ih++ )
	{
		Grid->SetRowHeightInPixels(ih + 1, 15);

		GridFont( IDC_CUSTOM_SMEMA_REAR, ih + 1, 1, 15);
		GridControl(IDC_CUSTOM_SMEMA_REAR, STATIC, ih + 1, 1, 0);
	}

	GridData( IDC_CUSTOM_SMEMA_REAR, 1, 1, "R Tran" );
	GridData( IDC_CUSTOM_SMEMA_REAR, 2, 1, "R Req" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_Smema()
{
	// front
	COLORREF col = GREEN_C;	if( g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk ) == IO_OFF )		col = RGB(0,64,0);
	GridColor( IDC_CUSTOM_SMEMA_FRONT, 1, 1, col, BLACK_C );

	col = RGB(255,128,64);				if( g_ioMgr.get_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ) == IO_OFF )	col = RGB(64,32,0);
	GridColor( IDC_CUSTOM_SMEMA_FRONT, 2, 1, col, BLACK_C );


	// rear
	col = RGB(255,128,64);				if( g_ioMgr.get_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_ON ) == IO_OFF )	col = RGB(64,32,0);
	GridColor( IDC_CUSTOM_SMEMA_REAR, 1, 1, col, BLACK_C );

	col = GREEN_C;				if( g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF )		col = RGB(0,64,0);
	GridColor( IDC_CUSTOM_SMEMA_REAR, 2, 1, col, BLACK_C );
}

void CScreen_Main::OnMain_Init_Bypass()
{
	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		GetDlgItem( IDC_CUSTOM_BYPASS )->MoveWindow( 0, 0, 0, 0 );		
		return;
	}

	TSpread* Grid;
	int row = 1;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_BYPASS );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid->SetColWidthInPixels( 1, 90 );
	Grid->SetRowHeightInPixels( 1, 30);

	GridFont( IDC_CUSTOM_BYPASS, 1, 1, 15);
	GridControl(IDC_CUSTOM_BYPASS, STATIC, 1, 1, 0);

	GridData( IDC_CUSTOM_BYPASS, 1, 1, "NEXT BYPASS" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_Bypass()
{
	if( g_handler.GetMachinePos() == EMPOS_FRONT )
		return;

	COLORREF col = YELLOW_C;	if( g_handler.GetReadySendNext() == false )		col = YELLOW_DD;
	GridColor( IDC_CUSTOM_BYPASS, 1, 1, col, BLACK_C );
}

void CScreen_Main::OnMain_Init_RecvNext()
{
	if( g_handler.GetMachinePos() == EMPOS_REAR )
	{
		GetDlgItem( IDC_CUSTOM_RECV_NEXT )->MoveWindow( 0, 0, 0, 0 );		
		return;
	}

	TSpread* Grid;
	int row = 1;
	int col = 1;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_RECV_NEXT );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid->SetColWidthInPixels( 1, 90 );
	Grid->SetRowHeightInPixels( 1, 30);

	GridFont( IDC_CUSTOM_RECV_NEXT, 1, 1, 15);
	GridControl(IDC_CUSTOM_RECV_NEXT, STATIC, 1, 1, 0);

	GridData( IDC_CUSTOM_RECV_NEXT, 1, 1, "RECV NEXT" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_RecvNext()
{
	if( g_handler.GetMachinePos() == EMPOS_REAR )
		return;

	COLORREF col = YELLOW_C;	if( g_handler.GetRecvStateNextMachine() == false )		col = YELLOW_DD;
	GridColor( IDC_CUSTOM_RECV_NEXT, 1, 1, col, BLACK_C );
}

void CScreen_Main::OnBtnTrayminus() 
{
	if( COMI.mn_run_status != dSTOP )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Is not a stationary state.");//20130930
		}
		else
		{
			DoModal_Msg( "정지상태가 아닙니다.");
		}

		return;
	}
	
	// 트레이 빈 공간이 없어야 한다.
	if( g_Tray.GetRejCnt() > 0 && g_Tray.GetVecRejEmptyCell().size() < (st_basic.n_tray_x * st_basic.n_tray_y) )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Is not empty tray on top.");//20130930
		}
		else
		{
			DoModal_Msg( "최상위에 빈트레이가 아닙니다.");
		}
		return;
	}
	
	if( g_Tray.GetRejCnt() <=0 )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG ) 
		{
			DoModal_Msg( "Not reject stacker tray.");//20130930
		}
		else
		{
			DoModal_Msg( "리젝트 스태커에는 트레이가 없습니다.");
		}

		return;
	}
	
	g_Tray.TrayMinus();	
}

void CScreen_Main::OnMain_Init_P_Lot_Info()				// P_LOT 20130910
{
	TSpread *Grid;
	CString str_pos;
	int row = 42;
	int col = 3;
	int i, j;
	
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_PLOT_INFO);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for (i = 0; i <= row; i++)
	{
		for (j = 0; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 15);
			Grid -> SetColWidthInPixels(j, 55);
			
			if ( i == 1 || i == 8 || i == 15 || i == 22 || i == 29  || i == 36)
			{
				GridMerge( IDC_CUSTOM_PLOT_INFO, i , 1, 2, 1);
				GridData( IDC_CUSTOM_PLOT_INFO, i, 1, "-");
				GridMerge( IDC_CUSTOM_PLOT_INFO, i, 2, 1, 2);
				GridData( IDC_CUSTOM_PLOT_INFO, i, 2, "-");
				GridData( IDC_CUSTOM_PLOT_INFO, i+1, 2, "Prime");
				GridData( IDC_CUSTOM_PLOT_INFO, i+1, 3, "Cum");
				GridColor(IDC_CUSTOM_PLOT_INFO, i, j, YELLOW_C, BLACK_C);
			}
			else if ( i == 7 || i == 14 || i == 21 || i == 28 || i == 35 )
			{
				GridColor(IDC_CUSTOM_PLOT_INFO, i, j, GRAY_C, BLACK_C);
			}
			else if ( i == 41 )
			{
				GridColor(IDC_CUSTOM_PLOT_INFO, i, j, ORANGE_C, BLACK_C);
			}
			else
			{
				GridColor(IDC_CUSTOM_PLOT_INFO, i, j, WHITE_C, BLACK_C);
			}
			
			GridControl(IDC_CUSTOM_PLOT_INFO, STATIC, i, j, 0);
			GridFont(IDC_CUSTOM_PLOT_INFO, i, j, 15);
		}
	}
	
	GridMerge(IDC_CUSTOM_PLOT_INFO, 7, 1, 1, 2);
	GridMerge(IDC_CUSTOM_PLOT_INFO, 14, 1, 1, 2);
	GridMerge(IDC_CUSTOM_PLOT_INFO, 21, 1, 1, 2);
	GridMerge(IDC_CUSTOM_PLOT_INFO, 28, 1, 1, 2);
	GridMerge(IDC_CUSTOM_PLOT_INFO, 35, 1, 1, 2);	
	GridMerge(IDC_CUSTOM_PLOT_INFO, 42, 1, 1, 2);
	
	for (i = 1; i < row + 1; i++)
	{
		if ( i == 3 || i == 10 || i == 17 || i == 24 || i == 31 || i == 38 )
		{
			GridData(IDC_CUSTOM_PLOT_INFO, i, 1, "Input" );
		}
		else if ( i == 4 || i == 11 || i == 18 || i == 25 || i == 32 || i == 39 )
		{
			GridData(IDC_CUSTOM_PLOT_INFO, i, 1, "Pass" );
		}
		else if ( i == 5 || i == 12 || i == 19 || i == 26 || i == 33 || i == 40 )
		{
			GridData(IDC_CUSTOM_PLOT_INFO, i, 1, "Fail" );
		}
		else if ( i == 6 || i == 13 || i == 20 || i == 27 || i == 34 || i == 41 )
		{
			GridData(IDC_CUSTOM_PLOT_INFO, i, 1, "Yield" );
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Main::OnMain_Display_P_Lot_Info()				// P_LOT 20130910
{
	int i = 0;

	CString strPLotName;
	CString strCLotName[MAX_PLOT];
	
	CString strPLotInputCount[MAX_PLOT][3];
	CString strPLotPassCount[MAX_PLOT][3];
	CString strPLotFailCount[MAX_PLOT][3];
	CString strPLotYield[MAX_PLOT][3];
	
	int nPLotInputTotal[3] = {0,};
	int nPLotPassTotal[3] = {0,};
	int nPLotFailTotal[3] = {0,};
	double fPLotYieldTotal[3] = {0,};
	
	CString strPLotInputTotal[3];
	CString strPLotPassTotal[3];
	CString strPLotFailTotal[3];
	CString strPLotYieldTotal[3];

	for ( i = 0; i < 5; i++)
	{
		if(i < g_lotMgr.GetPLotCount())
		{
			if	   ( i == 0 )	strPLotName = strCLotName[0] = g_lotMgr.GetPLotAt(0).GetLotID();
			else if( i == 1 )	strCLotName[1] = g_lotMgr.GetPLotAt(1).GetLotID();
			else if( i == 2 )	strCLotName[2] = g_lotMgr.GetPLotAt(2).GetLotID();
			else if( i == 3 )	strCLotName[3] = g_lotMgr.GetPLotAt(3).GetLotID();
			else if( i == 4 )	strCLotName[4] = g_lotMgr.GetPLotAt(4).GetLotID();

			strPLotInputCount[i][PRIME].Format("%d", st_work.nPLotInputCount[i][PRIME]);
			strPLotPassCount[i][PRIME].Format("%d", st_work.nPLotPassCount[i][PRIME]);
			strPLotFailCount[i][PRIME].Format("%d", st_work.nPLotFailCount[i][PRIME]);
			strPLotYield[i][PRIME].Format("%3.2f%%", st_work.fPLotYield[i][PRIME]);

			strPLotInputCount[i][CUM].Format("%d", st_work.nPLotInputCount[i][CUM]);
			strPLotPassCount[i][CUM].Format("%d", st_work.nPLotPassCount[i][CUM]);
			strPLotFailCount[i][CUM].Format("%d", st_work.nPLotFailCount[i][CUM]);
			strPLotYield[i][CUM].Format("%3.2f%%", st_work.fPLotYield[i][CUM]);
			

			nPLotInputTotal[PRIME] += st_work.nPLotInputCount[i][PRIME];
			nPLotPassTotal[PRIME] += st_work.nPLotPassCount[i][PRIME];
			nPLotFailTotal[PRIME] += st_work.nPLotFailCount[i][PRIME];

			nPLotInputTotal[CUM] += st_work.nPLotInputCount[i][CUM];
			nPLotPassTotal[CUM] += st_work.nPLotPassCount[i][CUM];
			nPLotFailTotal[CUM] += st_work.nPLotFailCount[i][CUM];

		}
		else
		{
			strCLotName[i] = "";
			strPLotInputCount[i][PRIME] = "";
			strPLotPassCount[i][PRIME] = "";
			strPLotFailCount[i][PRIME] = "";
			strPLotYield[i][PRIME] = "";

			strPLotInputCount[i][CUM] = "";
			strPLotPassCount[i][CUM] = "";
			strPLotFailCount[i][CUM] = "";
			strPLotYield[i][CUM] = "";
		}
	}	

	for ( i = 0; i < 2; i++)
	{
		if ( nPLotPassTotal[i] + nPLotFailTotal[i] > 0 )
		{
			fPLotYieldTotal[i] = (double)( (double)nPLotPassTotal[i] / (double)( (double)nPLotPassTotal[i] + (double)nPLotFailTotal[i] ) ) * (double)100;
		}
		else
		{
			fPLotYieldTotal[i] = 0;
		}

		strPLotYieldTotal[i].Format("%3.2f%%", fPLotYieldTotal[i]);
	}

	strPLotInputTotal[PRIME].Format("%d", nPLotInputTotal[PRIME]);
	strPLotPassTotal[PRIME].Format("%d", nPLotPassTotal[PRIME]);
	strPLotFailTotal[PRIME].Format("%d", nPLotFailTotal[PRIME]);

	strPLotInputTotal[CUM].Format("%d", nPLotInputTotal[CUM]);
	strPLotPassTotal[CUM].Format("%d", nPLotPassTotal[CUM]);
	strPLotFailTotal[CUM].Format("%d", nPLotFailTotal[CUM]);

	
	// Lot Name
	GridData(IDC_CUSTOM_PLOT_INFO, 1, 1, strPLotName );
	GridData(IDC_CUSTOM_PLOT_INFO, 1, 2, strCLotName[0] );

	GridData(IDC_CUSTOM_PLOT_INFO, 8, 1, strPLotName );
	GridData(IDC_CUSTOM_PLOT_INFO, 8, 2, strCLotName[1] );

	GridData(IDC_CUSTOM_PLOT_INFO, 15, 1, strPLotName );
	GridData(IDC_CUSTOM_PLOT_INFO, 15, 2, strCLotName[2] );

	GridData(IDC_CUSTOM_PLOT_INFO, 22, 1, strPLotName );
	GridData(IDC_CUSTOM_PLOT_INFO, 22, 2, strCLotName[3] );

	GridData(IDC_CUSTOM_PLOT_INFO, 29, 1, strPLotName );
	GridData(IDC_CUSTOM_PLOT_INFO, 29, 2, strCLotName[4] );

	GridData(IDC_CUSTOM_PLOT_INFO, 42, 1, strPLotName );

	// Input Count
	GridData(IDC_CUSTOM_PLOT_INFO, 3, 2, strPLotInputCount[0][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 10, 2, strPLotInputCount[1][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 17, 2, strPLotInputCount[2][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 24, 2, strPLotInputCount[3][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 31, 2, strPLotInputCount[4][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 38, 2, strPLotInputTotal[PRIME] );

	// Pass Count
	GridData(IDC_CUSTOM_PLOT_INFO, 4, 2, strPLotPassCount[0][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 11, 2, strPLotPassCount[1][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 18, 2, strPLotPassCount[2][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 25, 2, strPLotPassCount[3][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 32, 2, strPLotPassCount[4] [PRIME]);
	GridData(IDC_CUSTOM_PLOT_INFO, 39, 2, strPLotPassTotal[PRIME] );

	// Fail Count
	GridData(IDC_CUSTOM_PLOT_INFO, 5, 2, strPLotFailCount[0][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 12, 2, strPLotFailCount[1][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 19, 2, strPLotFailCount[2][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 26, 2, strPLotFailCount[3][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 33, 2, strPLotFailCount[4][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 40, 2, strPLotFailTotal[PRIME] );

	// Yield
	GridData(IDC_CUSTOM_PLOT_INFO, 6, 2, strPLotYield[0][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 13, 2, strPLotYield[1][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 20, 2, strPLotYield[2][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 27, 2, strPLotYield[3][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 34, 2, strPLotYield[4][PRIME] );
	GridData(IDC_CUSTOM_PLOT_INFO, 41, 2, strPLotYieldTotal[PRIME] );

	// Input Count
	GridData(IDC_CUSTOM_PLOT_INFO, 3, 3, strPLotInputCount[0][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 10, 3, strPLotInputCount[1][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 17, 3, strPLotInputCount[2][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 24, 3, strPLotInputCount[3][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 31, 3, strPLotInputCount[4][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 38, 3, strPLotInputTotal[CUM] );

	// Pass Count
	GridData(IDC_CUSTOM_PLOT_INFO, 4, 3, strPLotPassCount[0][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 11, 3, strPLotPassCount[1][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 18, 3, strPLotPassCount[2][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 25, 3, strPLotPassCount[3][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 32, 3, strPLotPassCount[4] [CUM]);
	GridData(IDC_CUSTOM_PLOT_INFO, 39, 3, strPLotPassTotal[CUM] );

	// Fail Count
	GridData(IDC_CUSTOM_PLOT_INFO, 5, 3, strPLotFailCount[0][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 12, 3, strPLotFailCount[1][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 19, 3, strPLotFailCount[2][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 26, 3, strPLotFailCount[3][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 33, 3, strPLotFailCount[4][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 40, 3, strPLotFailTotal[CUM] );

	// Yield
	GridData(IDC_CUSTOM_PLOT_INFO, 6, 3, strPLotYield[0][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 13, 3, strPLotYield[1][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 20, 3, strPLotYield[2][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 27, 3, strPLotYield[3][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 34, 3, strPLotYield[4][CUM] );
	GridData(IDC_CUSTOM_PLOT_INFO, 41, 3, strPLotYieldTotal[CUM] );	
}
void CScreen_Main::OnMsgPLotInfo() 
{
 	st_handler.m_iPLotInfo = !st_handler.m_iPLotInfo;
 
 	OnMain_Display_Lot_Info();
	
}

void CScreen_Main::OnBtnStackerOff() 
{
	CString str_Temp;
	if (COMI.mn_run_status != dSTOP)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "장비가 Stop 상태가 아닙니다.(reject stacker guide off)..");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "The Machine is not Stop.(reject stacker guide off)..");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return ;
	}

	g_ioMgr.set_out_bit(st_io.o_reject_stacker_guide, IO_OFF);
	
}

void CScreen_Main::OnButtonXgem() 
{
	if( COMI.mn_run_status != dSTOP )
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "장비가 Stop 상태가 아닙니다..");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "The Machine is not Stop.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return;
	}


	CDialog_XgemInterface dlgxgem;
	dlgxgem.DoModal();

	CWnd* m_pWnd = CWnd::FindWindow(NULL,"XgemInterface");
	if(m_pWnd){
		//기존의 Program을 활성화 시킨다.
		m_pWnd->SetForegroundWindow(); 	
		m_pWnd->ShowWindow(SW_RESTORE);		
	}
	else{

		WinExec("C:\\AMT8520\\bin\\XgemInterface.exe",SW_SHOW);
	}


}

////2015.0604
void CScreen_Main::FtpConnect() 
{
	
	// TODO: Add your control notification handler code here
	CString strSend;
	CString strFtp;

	//연결하고자하는 FTP 서버의 IP 주소와 사용자명, 암호 입력
	if( m_pFtp.Open(st_lamp.mstr_ftp_ip, st_lamp.mstr_ftp_user, st_lamp.mstr_ftp_pw, st_lamp.mn_ftp_port) )
	{
		OnUpdateRemote(st_lamp.mstr_ftp_path);
		
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			strFtp = "Ftp Connection Complete";
// 			sprintf(st_msg.c_normal_msg, "%s", strFtp);
// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}
	}
	else
	{
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			strFtp = "Ftp Connection Fail";
// 			sprintf(st_msg.c_abnormal_msg, "%s", strFtp);
// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
	}
}

void CScreen_Main::OnUpdateRemote(CString strPath)
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
		// 추가한후 추가된 index 를 받아옴
	}
	//찾을 화일 지정
	CFTPFileFind	finder(m_pFtp.m_pConnection);
	bWorking = finder.FindFile(strPath);
	
	while(bWorking)
	{
		//FTP 서버의 파일명을 얻어옴
		bWorking = finder.FindNextFile();		// 화일하나 찾음
		
		// 화일이름 추가
		strTemp = finder.GetFileName();
		if(strTemp == _T(".")) continue;	// "." 이면 다음으로 넘어감
		
		if(finder.IsDirectory())
		{
			//디렉토리면 [ ] 로 감쌈	
			strDirectory.Format(_T("[%s]"), strTemp);
		}
		else
		{
			strDirectory = strTemp;
		}
		// 추가한후 추가된 index 를 받아옴
		
		// 화일크기 추가
		size = finder.GetLength();
		strTemp.Format(_T("%d"),size);
		
		// 화일날짜 추가
		finder.GetLastWriteTime(time);
		strTemp = time.Format("%y%m%d %H:%M");		
	}
	
	UpdateData(FALSE);	// 화면에 있는값으로.....
}

void CScreen_Main::FtpFileDown()
{
	CString strData;
	CString strFtp;
	int mn_response;  // 대화 상자에 대한 리턴 값 저장 변수
	int nRet;
	char chr_file_name[256];

	CDialog_Infor info_dlg;


	int n_existence;
	
	
	sprintf(chr_file_name, "%s", st_path.mstr_scrap_code);
	n_existence = access(chr_file_name, 0) ;
	
	if (n_existence != -1)
	{
		DeleteFile(chr_file_name);//C:\AMT8520\Setting\RETEST_RULE.TXT 설비에 다운 받을 위치
	}
	
	if( m_pFtp.Open(st_lamp.mstr_ftp_ip, st_lamp.mstr_ftp_user, st_lamp.mstr_ftp_pw, st_lamp.mn_ftp_port) )
	{ 
		strData.Format(_T("%s/%s"), st_lamp.mstr_ftp_path, st_lamp.mstr_ftp_scrap_code_name);// /Users/MDL/MDL_HNDL/WP_SKEL_SSD/RETEST_RULE, RETEST_RULE.TXT -> FTP서버 경로, FTP서버 다운 받을 파일
		
		nRet = m_pFtp.DownLoad(strData, st_path.mstr_scrap_code);
		m_pFtp.Close();

		if(nRet == CTL_YES)
		{
			st_basic.mn_mode_ftp = 1;
			st_basic.mstr_mode_ftp = "FTP";

			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
			{
				strFtp = "Ftp File Down Complete";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}	
		}
		else
		{
			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
			{
				strFtp = "Ftp File Down Fail";
				Func.On_LogFile_Add(LOG_TOTAL, strFtp);
			}
			
			if (COMI.mn_run_status != dSTOP)
			{
				g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
				Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수
				
				if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
			}
		
			st_msg.mstr_confirm_msg = _T("Scrap Code를 Local Mode로 사용 하시겠습니까?");
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
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			strFtp = "Ftp File Down Fail";
			Func.On_LogFile_Add(LOG_TOTAL, strFtp);
		}

		if (COMI.mn_run_status != dSTOP)
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
			Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수

			if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
		}
		
		st_msg.mstr_confirm_msg = _T("Scrap Code를 Local Mode로 사용 하시겠습니까?");
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
////

void CScreen_Main::OnButtonFillratio() 
{
	
	if( COMI.mn_run_status != dSTOP )
	{
		// 		sprintf(st_msg.c_abnormal_msg, "장비가 Stop 상태가 아닙니다..");
		// 		if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "The Machine is not Stop.");
		// 		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		// 		return;
	}
	
	CWnd* m_pWnd = CWnd::FindWindow(NULL,"FILL RATIO UI");
	if(m_pWnd){
		//기존의 Program을 활성화 시킨다.
		m_pWnd->SetForegroundWindow(); 	
		m_pWnd->ShowWindow(SW_RESTORE);		
	}
	else{
		
		WinExec("C:\\AMT8520\\bin\\FILL RATIO UI.exe",SW_SHOW);
	}		
}


void CScreen_Main::OnBtnAbort() 
{
	CDialog_Select select_dlg;

	st_msg.mstr_confirm_msg = _T("Do you want to abort with tester?");
				
	int mn_response = select_dlg.DoModal();

	if (mn_response == IDOK)
	{
		st_handler.mn_Abort = CTL_YES;
	}	
}

void CScreen_Main::OnDoubleclickedBtnDoorOpen() 
{
	m_btn_abort.ShowWindow(true);
	
}

void CScreen_Main::OnBtnManagerSocket() 
{
// 	CDialog_MgrSocket dlg;
// 	dlg.DoModal();

/*	CString strErr;

	strErr.Format( "모든 Testet socket을 사용할 수 없습니다. 테스터를 체크해 주세요");
	if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "It can't use Tester socket. Check tester plaese.");
	st_msg.mstr_event_msg[0] = strErr;
//	st_msg.mstr_event_msg[1] = strErr;
//	st_msg.mstr_event_msg[2] = strErr;	
	::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

	return;
*/

	::ShellExecute(NULL, NULL, "C:\\AMT8520\\bin\\ManagerSocket.exe", NULL, NULL, SW_SHOWNORMAL);


/// 	CString str = g_site.GetModule( 12 ).GetScrapCode();
// 	n = g_site.GetModule( 13 ).GetScrapeCnt();
// 	g_site.GetModule( 12 ).SetScrapeCnt(st_handler.mn_MultiLot_cnt[21]);
// 	n = g_site.GetModule( 12 ).GetScrapeCnt();

// 	n = st_handler.m_bBackupEnable2[0][2];
// 	n = st_handler.m_bBackupEnable[0][2];
	//2016.071112
//     Func.LoadSocketSkipRule();
// 	g_site.SetEnableSocket( 13, -1);
// 	g_site.SetEnableSocket( 14, -2);
// 	g_site.SetEnableSocket( 15, -3);
// 	g_site.SetEnableSocket( 16, -4);
}
