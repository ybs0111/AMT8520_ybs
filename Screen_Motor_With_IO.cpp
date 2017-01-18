// Screen_Motor_With_IO.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Screen_Motor_With_IO.h"
#include "IO_Manager.h"
#include "ComizoaPublic.h"
#include "SrcPart/PartFunction.h"
#include "SrcBase/ALocalization.h"
#include "Run_Motors.h"
#include "SrcPart/APartRobot.h"
#include "SrcPart/APartHandler.h"
#include "Cmmsdk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScreen_Motor_With_IO

IMPLEMENT_DYNCREATE(CScreen_Motor_With_IO, CFormView)

CScreen_Motor_With_IO::CScreen_Motor_With_IO()
	: CFormView(CScreen_Motor_With_IO::IDD)
{
	//{{AFX_DATA_INIT(CScreen_Motor_With_IO)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CScreen_Motor_With_IO::~CScreen_Motor_With_IO()
{
}

void CScreen_Motor_With_IO::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScreen_Motor_With_IO)
	DDX_Control(pDX, IDC_CHK_PICK, m_chk_pick);
	DDX_Control(pDX, IDC_BTN_DOWN, m_btn_down);
	DDX_Control(pDX, IDC_BTN_PLUS, m_btn_plus);
	DDX_Control(pDX, IDC_BTN_MINUS, m_btn_minus);
	DDX_Control(pDX, IDC_BTN_ROBOT_GO, m_btn_go);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScreen_Motor_With_IO, CFormView)
	//{{AFX_MSG_MAP(CScreen_Motor_With_IO)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_ROBOT_GO, OnBtnRobotGo)
	ON_BN_CLICKED(IDC_BTN_HOME, OnBtnHome)
	ON_BN_CLICKED(IDC_BTN_EMG_STOP, OnBtnEmgStop)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_START, OnBtnTeachS1Start)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_END, OnBtnTeachS1End)
	ON_BN_CLICKED(IDC_BTN_TEACH_S2_START, OnBtnTeachS2Start)
	ON_BN_CLICKED(IDC_BTN_TEACH_S2_END, OnBtnTeachS2End)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN1_START, OnBtnTeachBin1Start)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN1_END, OnBtnTeachBin1End)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN2_START, OnBtnTeachBin2Start)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN2_END, OnBtnTeachBin2End)
	ON_BN_CLICKED(IDC_BTN_TEACH_REJ_START, OnBtnTeachRejStart)
	ON_BN_CLICKED(IDC_BTN_TEACH_REJ_END, OnBtnTeachRejEnd)
	ON_BN_CLICKED(IDC_BTN_TEACH_COKST_START, OnBtnTeachCokstStart)
	ON_BN_CLICKED(IDC_BTN_TEACH_COKST_END, OnBtnTeachCokstEnd)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_COK, OnBtnTeachS1Cok)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_PLACE, OnBtnTeachS1Place)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN1_PLACE, OnBtnTeachBin1Place)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_COK_PLACE, OnBtnTeachS1CokPlace)
	ON_BN_CLICKED(IDC_BTN_TEACH_COKST_PLACE, OnBtnTeachCokstPlace)
	ON_BN_CLICKED(IDC_BTN_TEACH_S1_CALC, OnBtnTeachS1Calc)
	ON_BN_CLICKED(IDC_BTN_TEACH_S2_CALC, OnBtnTeachS2Calc)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN3_START, OnBtnTeachBin3Start)
	ON_BN_CLICKED(IDC_BTN_TEACH_BIN3_END, OnBtnTeachBin3End)
	ON_BN_CLICKED(IDC_BTN_DOOR_OPEN, OnBtnDoorOpen)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)
	ON_MESSAGE(WMU_CHILD_LBUTTON_DOWN, OnUserLButtonDown)
	ON_MESSAGE(WMU_CHILD_LBUTTON_UP, OnUserLButtonUp)
	ON_MESSAGE( WM_MOTOR_COMPLETION, OnMotorCompletion)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScreen_Motor_With_IO diagnostics

#ifdef _DEBUG
void CScreen_Motor_With_IO::AssertValid() const
{
	CFormView::AssertValid();
}

void CScreen_Motor_With_IO::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CScreen_Motor_With_IO message handlers

void CScreen_Motor_With_IO::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	OnInitMotor();
	OnInitIO();
	OnInitPOGOIO();
	OnInitTarget_TestSite();
	OnInitTarget_Robot();
	OnInitTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
	OnInitTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
	OnInitTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
	OnInitTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
	OnInitTarget_cokStacker();

	OnInitMotorInfo();

	m_nGo_col = -1;
	m_nGo_row = -1;

	SetTimer(TMR_WITH_IO_READ, 1000, NULL);

	st_handler.cwnd_motor_with_io = this;

	m_nRobot = 0;
	m_nTarget = -1;
	m_nAxis = -1;

	GridColor( IDC_CUSTOM_TARGET_ROBOT, 1, 2, RED_C, BLACK_C );

	m_btn_minus.SetTag(IDC_BTN_MINUS);
	m_btn_plus.SetTag( IDC_BTN_PLUS );
}

void CScreen_Motor_With_IO::OnInitIO()
{
	TSpread* Grid;
	int col = 14;
	int row = 20;
	int i=0;

	int iGrid = IDC_CUSTOM_IO;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for( i=0; i<col; i++ )
	{
		if( i % 2 == 0)	Grid->SetColWidthInPixels(i + 1, 120);
		else			Grid->SetColWidthInPixels(i + 1, 20);
	}
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 17 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 15);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );
		}
	}

	if ( g_local.GetLocalType() == LOCAL_ENG )
	{
		// BIN
		GridMerge( iGrid, 1, 1, 2, 5 );			GridData( iGrid, 1, 1, "RAIL" );			GridData( iGrid, 1, 6, "F" );	GridData( iGrid, 2, 6, "B" );
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			GridMerge( iGrid, 3, 1, 2, 1 );			GridData( iGrid, 3, 1, "Guide 3");			GridData( iGrid, 3, 2, "F" );	GridData( iGrid, 4, 2, "B" );
			GridMerge( iGrid, 3, 3, 2, 1 );			GridData( iGrid, 3, 3, "Guide 2");			GridData( iGrid, 3, 4, "F" );	GridData( iGrid, 4, 4, "B" );
			GridMerge( iGrid, 3, 5, 2, 1 );			GridData( iGrid, 3, 5, "Guide 1");			GridData( iGrid, 3, 6, "F" );	GridData( iGrid, 4, 6, "B" );
			GridMerge( iGrid, 5, 1, 2, 1 );			GridData( iGrid, 5, 1, "Pusher 3");			GridData( iGrid, 5, 2, "F" );	GridData( iGrid, 6, 2, "B" );
			GridMerge( iGrid, 5, 3, 2, 1 );			GridData( iGrid, 5, 3, "Pusher 2");			GridData( iGrid, 5, 4, "F" );	GridData( iGrid, 6, 4, "B" );
			GridMerge( iGrid, 5, 5, 2, 1 );			GridData( iGrid, 5, 5, "Pusher 1");			GridData( iGrid, 5, 6, "F" );	GridData( iGrid, 6, 6, "B" );
			GridMerge( iGrid, 7, 1, 2, 1 );			GridData( iGrid, 7, 1, "Align 3");			GridData( iGrid, 7, 2, "F" );	GridData( iGrid, 8, 2, "B" );
			GridMerge( iGrid, 7, 3, 2, 1 );			GridData( iGrid, 7, 3, "Align 2");			GridData( iGrid, 7, 4, "F" );	GridData( iGrid, 8, 4, "B" );
			GridMerge( iGrid, 7, 5, 2, 1 );			GridData( iGrid, 7, 5, "Align 1");			GridData( iGrid, 7, 6, "F" );	GridData( iGrid, 8, 6, "B" );
			GridMerge( iGrid, 9, 1, 2, 1 );			GridData( iGrid, 9, 1, "Stopper 3");		GridData( iGrid, 9, 2, "UP" );	GridData( iGrid, 10, 2, "DN" );
			GridMerge( iGrid, 9, 3, 2, 1 );			GridData( iGrid, 9, 3, "Stopper 2");		GridData( iGrid, 9, 4, "UP" );	GridData( iGrid, 10, 4, "DN" );
			GridMerge( iGrid, 9, 5, 2, 1 );			GridData( iGrid, 9, 5, "Stopper 1");		GridData( iGrid, 9, 6, "UP" );	GridData( iGrid, 10, 6, "DN" );
			GridMerge( iGrid, 11, 1, 2, 1 );		GridData( iGrid, 11, 1, "Clamp 3");		GridMerge( iGrid, 11,2,2,1 );	GridData( iGrid, 11, 2, "C" );
			GridMerge( iGrid, 11, 3, 2, 1 );		GridData( iGrid, 11, 3, "Clamp 2");		GridMerge( iGrid, 11,4,2,1 );	GridData( iGrid, 11, 4, "C" );
			GridMerge( iGrid, 11, 5, 2, 1 );		GridData( iGrid, 11, 5, "Clamp 1");		GridMerge( iGrid, 11,6,2,1 );	GridData( iGrid, 11, 6, "C" );
			GridMerge( iGrid, 13, 1, 1, 2 );		GridData( iGrid, 13, 1, "Tray CHK 3");
			GridMerge( iGrid, 13, 3, 1, 2 );		GridData( iGrid, 13, 3, "Tray CHK 2");
			GridMerge( iGrid, 13, 5, 1, 2 );		GridData( iGrid, 13, 5, "Tray CHK 1");
			GridMerge( iGrid, 14, 1, 1, 2 );		GridData( iGrid, 14, 1, "Stop Pos 3");
			GridMerge( iGrid, 14, 3, 1, 2 );		GridData( iGrid, 14, 3, "Stop Pos 2");
			GridMerge( iGrid, 14, 5, 1, 2 );		GridData( iGrid, 14, 5, "Stop Pos 1");
		}
		else
		{
			GridMerge( iGrid, 3, 1, 2, 1 );			GridData( iGrid, 3, 1, "Guide 1");			GridData( iGrid, 3, 2, "F" );	GridData( iGrid, 4, 2, "B" );
			GridMerge( iGrid, 3, 3, 2, 1 );			GridData( iGrid, 3, 3, "Guide 2");			GridData( iGrid, 3, 4, "F" );	GridData( iGrid, 4, 4, "B" );
			GridMerge( iGrid, 3, 5, 2, 1 );			GridData( iGrid, 3, 5, "Guide 3");			GridData( iGrid, 3, 6, "F" );	GridData( iGrid, 4, 6, "B" );
			GridMerge( iGrid, 5, 1, 2, 1 );			GridData( iGrid, 5, 1, "Pusher 1");			GridData( iGrid, 5, 2, "F" );	GridData( iGrid, 6, 2, "B" );
			GridMerge( iGrid, 5, 3, 2, 1 );			GridData( iGrid, 5, 3, "Pusher 2");			GridData( iGrid, 5, 4, "F" );	GridData( iGrid, 6, 4, "B" );
			GridMerge( iGrid, 5, 5, 2, 1 );			GridData( iGrid, 5, 5, "Pusher 3");			GridData( iGrid, 5, 6, "F" );	GridData( iGrid, 6, 6, "B" );
			GridMerge( iGrid, 7, 1, 2, 1 );			GridData( iGrid, 7, 1, "Align 1");			GridData( iGrid, 7, 2, "F" );	GridData( iGrid, 8, 2, "B" );
			GridMerge( iGrid, 7, 3, 2, 1 );			GridData( iGrid, 7, 3, "Align 2");			GridData( iGrid, 7, 4, "F" );	GridData( iGrid, 8, 4, "B" );
			GridMerge( iGrid, 7, 5, 2, 1 );			GridData( iGrid, 7, 5, "Align 3");			GridData( iGrid, 7, 6, "F" );	GridData( iGrid, 8, 6, "B" );
			GridMerge( iGrid, 9, 1, 2, 1 );			GridData( iGrid, 9, 1, "Stopper 1");			GridData( iGrid, 9, 2, "UP" );	GridData( iGrid, 10, 2, "DN" );
			GridMerge( iGrid, 9, 3, 2, 1 );			GridData( iGrid, 9, 3, "Stopper 2");			GridData( iGrid, 9, 4, "UP" );	GridData( iGrid, 10, 4, "DN" );
			GridMerge( iGrid, 9, 5, 2, 1 );			GridData( iGrid, 9, 5, "Stopper 3");			GridData( iGrid, 9, 6, "UP" );	GridData( iGrid, 10, 6, "DN" );
			GridMerge( iGrid, 11, 1, 2, 1 );		GridData( iGrid, 11, 1, "Clamp 1");		GridMerge( iGrid, 11,2,2,1 );	GridData( iGrid, 11, 2, "C" );
			GridMerge( iGrid, 11, 3, 2, 1 );		GridData( iGrid, 11, 3, "Clamp 2");		GridMerge( iGrid, 11,4,2,1 );	GridData( iGrid, 11, 4, "C" );
			GridMerge( iGrid, 11, 5, 2, 1 );		GridData( iGrid, 11, 5, "Clamp 3");		GridMerge( iGrid, 11,6,2,1 );	GridData( iGrid, 11, 6, "C" );
			GridMerge( iGrid, 13, 1, 1, 2 );		GridData( iGrid, 13, 1, "Tray CHK 1");
			GridMerge( iGrid, 13, 3, 1, 2 );		GridData( iGrid, 13, 3, "Tray CHK 2");
			GridMerge( iGrid, 13, 5, 1, 2 );		GridData( iGrid, 13, 5, "Tray CHK 3");
			GridMerge( iGrid, 14, 1, 1, 2 );		GridData( iGrid, 14, 1, "Stop Pos 1");
			GridMerge( iGrid, 14, 3, 1, 2 );		GridData( iGrid, 14, 3, "Stop Pos 2");
			GridMerge( iGrid, 14, 5, 1, 2 );		GridData( iGrid, 14, 5, "Stop Pos 3");
		}
		GridMerge( iGrid, 15, 1, 1, 2 );		GridData( iGrid, 15, 1, "Tray In Chk");
		GridMerge( iGrid, 16, 1, 1, 2 );		GridData( iGrid, 16, 1, "Tray Dir Chk");

		GridMerge( iGrid, 17, 1, 2, 1 );		GridData( iGrid, 17, 1, "Transfer DN");		GridData( iGrid, 17, 2, "UP" );	GridData( iGrid, 18, 2, "DN" );

		// ROBOT
		GridMerge( iGrid, 1, 7, 2, 1 );			GridData( iGrid, 1, 7, "Gripper DN" );		GridData( iGrid, 1, 8, "UP" ); GridData( iGrid, 2, 8, "DN" );
		GridMerge( iGrid, 3, 7, 2, 1 );			GridData( iGrid, 3, 7, "Gripper Grip" );		GridData( iGrid, 3, 8, "F" ); GridData( iGrid, 4, 8, "B" );
		GridMerge( iGrid, 5, 7, 2, 1 );			GridData( iGrid, 5, 7, "Picker4 DN" );		GridData( iGrid, 5, 8, "UP" ); GridData( iGrid, 6, 8, "DN" );
		GridMerge( iGrid, 7, 7, 2, 1 );			GridData( iGrid, 7, 7, "Picker3 DN" );		GridData( iGrid, 7, 8, "UP" ); GridData( iGrid, 8, 8, "DN" );
		GridMerge( iGrid, 9, 7, 2, 1 );			GridData( iGrid, 9, 7, "Picker2 DN" );		GridData( iGrid, 9, 8, "UP" ); GridData( iGrid, 10, 8, "DN" );
		GridMerge( iGrid, 11, 7, 2, 1 );		GridData( iGrid, 11, 7, "Picker1 DN" );	GridData( iGrid, 11, 8, "UP" ); GridData( iGrid, 12, 8, "DN" );
												GridData( iGrid, 13, 7, "Picker4 Vacuum");										GridData( iGrid, 13, 8, "V" );
												GridData( iGrid, 14, 7, "Picker3 Vacuum");										GridData( iGrid, 14, 8, "V" );
												GridData( iGrid, 15, 7, "Picker2 Vacuum");										GridData( iGrid, 15, 8, "V" );
												GridData( iGrid, 16, 7, "Picker1 Vacuum");										GridData( iGrid, 16, 8, "V" );
		GridMerge( iGrid, 17, 7, 1, 2 );		GridData( iGrid, 17, 7, "Picker4 Emit");
		GridMerge( iGrid, 18, 7, 1, 2 );		GridData( iGrid, 18, 7, "Picker3 Emit");
		GridMerge( iGrid, 19, 7, 1, 2 );		GridData( iGrid, 19, 7, "Picker2 Emit");
		GridMerge( iGrid, 20, 7, 1, 2 );		GridData( iGrid, 20, 7, "Picker1 Emit");

		// TEST SITE 1
		GridMerge( iGrid, 1, 9, 2, 1 );			GridData( iGrid, 1, 9, "Site 1 Playe");			GridData( iGrid, 1, 10, "F" );	GridData( iGrid, 2, 10, "B" );
		GridMerge( iGrid, 3, 9, 2, 1 );			GridData( iGrid, 3, 9, "COK LOCK" );					GridData( iGrid, 3, 10, "L" );	GridData( iGrid, 4, 10, "UL" );
		GridMerge( iGrid, 5, 9, 2, 1 );			GridData( iGrid, 5, 9, "INSERT 1" );					GridData( iGrid, 5, 10, "F" );	GridData( iGrid, 6, 10, "B" );
		GridMerge( iGrid, 7, 9, 2, 1 );			GridData( iGrid, 7, 9, "INSERT 2" );					GridData( iGrid, 7, 10, "F" );	GridData( iGrid, 8, 10, "B" );
		GridMerge( iGrid, 9, 9, 2, 1 );			GridData( iGrid, 9, 9, "INSERT 3" );					GridData( iGrid, 9, 10, "F" );	GridData( iGrid, 10, 10, "B" );
		GridMerge( iGrid, 11, 9, 2, 1 );		GridData( iGrid, 11, 9, "INSERT 4" );					GridData( iGrid, 11, 10, "F" );	GridData( iGrid, 12, 10, "B" );
		GridMerge( iGrid, 13, 9, 2, 1 );		GridData( iGrid, 13, 9, "INSERT 5" );					GridData( iGrid, 13, 10, "F" );	GridData( iGrid, 14, 10, "B" );
		GridMerge( iGrid, 13, 9, 2, 1 );		GridData( iGrid, 13, 9, "INSERT 5" );					GridData( iGrid, 13, 10, "F" );	GridData( iGrid, 14, 10, "B" );
												GridData( iGrid, 15, 9, "HI-FIX Clamp");				GridData( iGrid, 15, 10, "C" );
		GridMerge( iGrid, 16, 9, 1, 2 );		GridData( iGrid, 16, 9, "HI_FIX Chk");

		// TEST SITE 2
		GridMerge( iGrid, 1, 11, 2, 1 );			GridData( iGrid, 1, 11, "Site 2 Plate");			GridData( iGrid, 1, 12, "F" );	GridData( iGrid, 2, 12, "B" );
		GridMerge( iGrid, 3, 11, 2, 1 );			GridData( iGrid, 3, 11, "COK LOCK" );					GridData( iGrid, 3, 12, "L" );	GridData( iGrid, 4, 12, "UL" );
		GridMerge( iGrid, 5, 11, 2, 1 );			GridData( iGrid, 5, 11, "INSERT 1" );					GridData( iGrid, 5, 12, "F" );	GridData( iGrid, 6, 12, "B" );
		GridMerge( iGrid, 7, 11, 2, 1 );			GridData( iGrid, 7, 11, "INSERT 2" );					GridData( iGrid, 7, 12, "F" );	GridData( iGrid, 8, 12, "B" );
		GridMerge( iGrid, 9, 11, 2, 1 );			GridData( iGrid, 9, 11, "INSERT 3" );					GridData( iGrid, 9, 12, "F" );	GridData( iGrid, 10, 12, "B" );
		GridMerge( iGrid, 11, 11, 2, 1 );		GridData( iGrid, 11, 11, "INSERT 4" );					GridData( iGrid, 11, 12, "F" );	GridData( iGrid, 12, 12, "B" );
		GridMerge( iGrid, 13, 11, 2, 1 );		GridData( iGrid, 13, 11, "INSERT 5" );					GridData( iGrid, 13, 12, "F" );	GridData( iGrid, 14, 12, "B" );
		GridMerge( iGrid, 13, 11, 2, 1 );		GridData( iGrid, 13, 11, "INSERT 5" );					GridData( iGrid, 13, 12, "F" );	GridData( iGrid, 14, 12, "B" );
												GridData( iGrid, 15, 11, "HI-FIX Clamp");				GridData( iGrid, 15, 12, "C" );
		GridMerge( iGrid, 16, 11, 1, 2 );		GridData( iGrid, 16, 11, "HI_FIX Chk");

		// COK CHK
		GridData( iGrid, 1, 13, "TS1 COK 10 Chk");
		GridData( iGrid, 2, 13, "TS1 COK 09 Chk");
		GridData( iGrid, 3, 13, "TS1 COK 08 Chk");
		GridData( iGrid, 4, 13, "TS1 COK 07 Chk");
		GridData( iGrid, 5, 13, "TS1 COK 06 Chk");
		GridData( iGrid, 6, 13, "TS1 COK 05 Chk");
		GridData( iGrid, 7, 13, "TS1 COK 04 Chk");
		GridData( iGrid, 8, 13, "TS1 COK 03 Chk");
		GridData( iGrid, 9, 13, "TS1 COK 02 Chk");
		GridData( iGrid, 10, 13, "TS1 COK 01 Chk");

		GridData( iGrid, 11, 13, "TS2 COK 10 Chk");
		GridData( iGrid, 12, 13, "TS2 COK 09 Chk");
		GridData( iGrid, 13, 13, "TS2 COK 08 Chk");
		GridData( iGrid, 14, 13, "TS2 COK 07 Chk");
		GridData( iGrid, 15, 13, "TS2 COK 06 Chk");
		GridData( iGrid, 16, 13, "TS2 COK 05 Chk");
		GridData( iGrid, 17, 13, "TS2 COK 04 Chk");
		GridData( iGrid, 18, 13, "TS2 COK 03 Chk");
		GridData( iGrid, 19, 13, "TS2 COK 02 Chk");
		GridData( iGrid, 20, 13, "TS2 COK 01 Chk");

		// COK STACKER
		GridMerge( iGrid, 18, 9, 2, 1 );			GridData( iGrid, 18, 9, "COK Stacker LOCK" );					GridData( iGrid, 18, 10, "L" );	GridData( iGrid, 19, 10, "UL" );
		GridMerge( iGrid, 20, 9, 1, 2 );			GridData( iGrid, 20, 9, "Stacker Pos Chk" );

		// ������
		GridMerge( iGrid, 18, 11, 1, 2 );			GridData( iGrid, 18, 11, "Conveyor 1" );
		GridMerge( iGrid, 19, 11, 1, 2 );			GridData( iGrid, 19, 11, "Conveyor 2" );
		GridMerge( iGrid, 20, 11, 1, 2 );			GridData( iGrid, 20, 11, "Conveyor 3" );
	}
	else
	{
		// BIN
		GridMerge( iGrid, 1, 1, 2, 5 );			GridData( iGrid, 1, 1, "����" );			GridData( iGrid, 1, 6, "F" );	GridData( iGrid, 2, 6, "B" );
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			GridMerge( iGrid, 3, 1, 2, 1 );			GridData( iGrid, 3, 1, "���̵� 3");			GridData( iGrid, 3, 2, "F" );	GridData( iGrid, 4, 2, "B" );
			GridMerge( iGrid, 3, 3, 2, 1 );			GridData( iGrid, 3, 3, "���̵� 2");			GridData( iGrid, 3, 4, "F" );	GridData( iGrid, 4, 4, "B" );
			GridMerge( iGrid, 3, 5, 2, 1 );			GridData( iGrid, 3, 5, "���̵� 1");			GridData( iGrid, 3, 6, "F" );	GridData( iGrid, 4, 6, "B" );
			GridMerge( iGrid, 5, 1, 2, 1 );			GridData( iGrid, 5, 1, "Ǫ�� 3");			GridData( iGrid, 5, 2, "F" );	GridData( iGrid, 6, 2, "B" );
			GridMerge( iGrid, 5, 3, 2, 1 );			GridData( iGrid, 5, 3, "Ǫ�� 2");			GridData( iGrid, 5, 4, "F" );	GridData( iGrid, 6, 4, "B" );
			GridMerge( iGrid, 5, 5, 2, 1 );			GridData( iGrid, 5, 5, "Ǫ�� 1");			GridData( iGrid, 5, 6, "F" );	GridData( iGrid, 6, 6, "B" );
			GridMerge( iGrid, 7, 1, 2, 1 );			GridData( iGrid, 7, 1, "Align 3");			GridData( iGrid, 7, 2, "F" );	GridData( iGrid, 8, 2, "B" );
			GridMerge( iGrid, 7, 3, 2, 1 );			GridData( iGrid, 7, 3, "Align 2");			GridData( iGrid, 7, 4, "F" );	GridData( iGrid, 8, 4, "B" );
			GridMerge( iGrid, 7, 5, 2, 1 );			GridData( iGrid, 7, 5, "Align 1");			GridData( iGrid, 7, 6, "F" );	GridData( iGrid, 8, 6, "B" );
			GridMerge( iGrid, 9, 1, 2, 1 );			GridData( iGrid, 9, 1, "������ 3");			GridData( iGrid, 9, 2, "UP" );	GridData( iGrid, 10, 2, "DN" );
			GridMerge( iGrid, 9, 3, 2, 1 );			GridData( iGrid, 9, 3, "������ 2");			GridData( iGrid, 9, 4, "UP" );	GridData( iGrid, 10, 4, "DN" );
			GridMerge( iGrid, 9, 5, 2, 1 );			GridData( iGrid, 9, 5, "������ 1");			GridData( iGrid, 9, 6, "UP" );	GridData( iGrid, 10, 6, "DN" );
			GridMerge( iGrid, 11, 1, 2, 1 );		GridData( iGrid, 11, 1, "Ŭ���� 3");		GridMerge( iGrid, 11,2,2,1 );	GridData( iGrid, 11, 2, "C" );
			GridMerge( iGrid, 11, 3, 2, 1 );		GridData( iGrid, 11, 3, "Ŭ���� 2");		GridMerge( iGrid, 11,4,2,1 );	GridData( iGrid, 11, 4, "C" );
			GridMerge( iGrid, 11, 5, 2, 1 );		GridData( iGrid, 11, 5, "Ŭ���� 1");		GridMerge( iGrid, 11,6,2,1 );	GridData( iGrid, 11, 6, "C" );
			GridMerge( iGrid, 13, 1, 1, 2 );		GridData( iGrid, 13, 1, "Ʈ���� üũ 3");
			GridMerge( iGrid, 13, 3, 1, 2 );		GridData( iGrid, 13, 3, "Ʈ���� üũ 2");
			GridMerge( iGrid, 13, 5, 1, 2 );		GridData( iGrid, 13, 5, "Ʈ���� üũ 1");
			GridMerge( iGrid, 14, 1, 1, 2 );		GridData( iGrid, 14, 1, "���� ������ 3");
			GridMerge( iGrid, 14, 3, 1, 2 );		GridData( iGrid, 14, 3, "���� ������ 2");
			GridMerge( iGrid, 14, 5, 1, 2 );		GridData( iGrid, 14, 5, "���� ������ 1");
		}
		else
		{
			GridMerge( iGrid, 3, 1, 2, 1 );			GridData( iGrid, 3, 1, "���̵� 1");			GridData( iGrid, 3, 2, "F" );	GridData( iGrid, 4, 2, "B" );
			GridMerge( iGrid, 3, 3, 2, 1 );			GridData( iGrid, 3, 3, "���̵� 2");			GridData( iGrid, 3, 4, "F" );	GridData( iGrid, 4, 4, "B" );
			GridMerge( iGrid, 3, 5, 2, 1 );			GridData( iGrid, 3, 5, "���̵� 3");			GridData( iGrid, 3, 6, "F" );	GridData( iGrid, 4, 6, "B" );
			GridMerge( iGrid, 5, 1, 2, 1 );			GridData( iGrid, 5, 1, "Ǫ�� 1");			GridData( iGrid, 5, 2, "F" );	GridData( iGrid, 6, 2, "B" );
			GridMerge( iGrid, 5, 3, 2, 1 );			GridData( iGrid, 5, 3, "Ǫ�� 2");			GridData( iGrid, 5, 4, "F" );	GridData( iGrid, 6, 4, "B" );
			GridMerge( iGrid, 5, 5, 2, 1 );			GridData( iGrid, 5, 5, "Ǫ�� 3");			GridData( iGrid, 5, 6, "F" );	GridData( iGrid, 6, 6, "B" );
			GridMerge( iGrid, 7, 1, 2, 1 );			GridData( iGrid, 7, 1, "Align 1");			GridData( iGrid, 7, 2, "F" );	GridData( iGrid, 8, 2, "B" );
			GridMerge( iGrid, 7, 3, 2, 1 );			GridData( iGrid, 7, 3, "Align 2");			GridData( iGrid, 7, 4, "F" );	GridData( iGrid, 8, 4, "B" );
			GridMerge( iGrid, 7, 5, 2, 1 );			GridData( iGrid, 7, 5, "Align 3");			GridData( iGrid, 7, 6, "F" );	GridData( iGrid, 8, 6, "B" );
			GridMerge( iGrid, 9, 1, 2, 1 );			GridData( iGrid, 9, 1, "������ 1");			GridData( iGrid, 9, 2, "UP" );	GridData( iGrid, 10, 2, "DN" );
			GridMerge( iGrid, 9, 3, 2, 1 );			GridData( iGrid, 9, 3, "������ 2");			GridData( iGrid, 9, 4, "UP" );	GridData( iGrid, 10, 4, "DN" );
			GridMerge( iGrid, 9, 5, 2, 1 );			GridData( iGrid, 9, 5, "������ 3");			GridData( iGrid, 9, 6, "UP" );	GridData( iGrid, 10, 6, "DN" );
			GridMerge( iGrid, 11, 1, 2, 1 );		GridData( iGrid, 11, 1, "Ŭ���� 1");		GridMerge( iGrid, 11,2,2,1 );	GridData( iGrid, 11, 2, "C" );
			GridMerge( iGrid, 11, 3, 2, 1 );		GridData( iGrid, 11, 3, "Ŭ���� 2");		GridMerge( iGrid, 11,4,2,1 );	GridData( iGrid, 11, 4, "C" );
			GridMerge( iGrid, 11, 5, 2, 1 );		GridData( iGrid, 11, 5, "Ŭ���� 3");		GridMerge( iGrid, 11,6,2,1 );	GridData( iGrid, 11, 6, "C" );
			GridMerge( iGrid, 13, 1, 1, 2 );		GridData( iGrid, 13, 1, "Ʈ���� üũ 1");
			GridMerge( iGrid, 13, 3, 1, 2 );		GridData( iGrid, 13, 3, "Ʈ���� üũ 2");
			GridMerge( iGrid, 13, 5, 1, 2 );		GridData( iGrid, 13, 5, "Ʈ���� üũ 3");
			GridMerge( iGrid, 14, 1, 1, 2 );		GridData( iGrid, 14, 1, "���� ������ 1");
			GridMerge( iGrid, 14, 3, 1, 2 );		GridData( iGrid, 14, 3, "���� ������ 2");
			GridMerge( iGrid, 14, 5, 1, 2 );		GridData( iGrid, 14, 5, "���� ������ 3");
		}
		GridMerge( iGrid, 15, 1, 1, 2 );		GridData( iGrid, 15, 1, "Ʈ���� �� üũ");
		GridMerge( iGrid, 16, 1, 1, 2 );		GridData( iGrid, 16, 1, "Ʈ���� ���� üũ");

		GridMerge( iGrid, 17, 1, 2, 1 );		GridData( iGrid, 17, 1, "Ʈ������ �ٿ�");		GridData( iGrid, 17, 2, "UP" );	GridData( iGrid, 18, 2, "DN" );

		// ROBOT
		GridMerge( iGrid, 1, 7, 2, 1 );			GridData( iGrid, 1, 7, "�׸��� �ٿ�" );		GridData( iGrid, 1, 8, "UP" ); GridData( iGrid, 2, 8, "DN" );
		GridMerge( iGrid, 3, 7, 2, 1 );			GridData( iGrid, 3, 7, "�׸��� ���" );		GridData( iGrid, 3, 8, "F" ); GridData( iGrid, 4, 8, "B" );
		GridMerge( iGrid, 5, 7, 2, 1 );			GridData( iGrid, 5, 7, "��Ŀ 4 �ٿ�" );		GridData( iGrid, 5, 8, "UP" ); GridData( iGrid, 6, 8, "DN" );
		GridMerge( iGrid, 7, 7, 2, 1 );			GridData( iGrid, 7, 7, "��Ŀ 3 �ٿ�" );		GridData( iGrid, 7, 8, "UP" ); GridData( iGrid, 8, 8, "DN" );
		GridMerge( iGrid, 9, 7, 2, 1 );			GridData( iGrid, 9, 7, "��Ŀ 2 �ٿ�" );		GridData( iGrid, 9, 8, "UP" ); GridData( iGrid, 10, 8, "DN" );
		GridMerge( iGrid, 11, 7, 2, 1 );		GridData( iGrid, 11, 7, "��Ŀ 1 �ٿ�" );	GridData( iGrid, 11, 8, "UP" ); GridData( iGrid, 12, 8, "DN" );
												GridData( iGrid, 13, 7, "��Ŀ 4 ����");										GridData( iGrid, 13, 8, "V" );
												GridData( iGrid, 14, 7, "��Ŀ 3 ����");										GridData( iGrid, 14, 8, "V" );
												GridData( iGrid, 15, 7, "��Ŀ 2 ����");										GridData( iGrid, 15, 8, "V" );
												GridData( iGrid, 16, 7, "��Ŀ 1 ����");										GridData( iGrid, 16, 8, "V" );
		GridMerge( iGrid, 17, 7, 1, 2 );		GridData( iGrid, 17, 7, "��Ŀ 4 ����");
		GridMerge( iGrid, 18, 7, 1, 2 );		GridData( iGrid, 18, 7, "��Ŀ 3 ����");
		GridMerge( iGrid, 19, 7, 1, 2 );		GridData( iGrid, 19, 7, "��Ŀ 2 ����");
		GridMerge( iGrid, 20, 7, 1, 2 );		GridData( iGrid, 20, 7, "��Ŀ 1 ����");

		// TEST SITE 1
		GridMerge( iGrid, 1, 9, 2, 1 );			GridData( iGrid, 1, 9, "����Ʈ 1 �÷���Ʈ");			GridData( iGrid, 1, 10, "F" );	GridData( iGrid, 2, 10, "B" );
		GridMerge( iGrid, 3, 9, 2, 1 );			GridData( iGrid, 3, 9, "COK LOCK" );					GridData( iGrid, 3, 10, "L" );	GridData( iGrid, 4, 10, "UL" );
		GridMerge( iGrid, 5, 9, 2, 1 );			GridData( iGrid, 5, 9, "INSERT 1" );					GridData( iGrid, 5, 10, "F" );	GridData( iGrid, 6, 10, "B" );
		GridMerge( iGrid, 7, 9, 2, 1 );			GridData( iGrid, 7, 9, "INSERT 2" );					GridData( iGrid, 7, 10, "F" );	GridData( iGrid, 8, 10, "B" );
		GridMerge( iGrid, 9, 9, 2, 1 );			GridData( iGrid, 9, 9, "INSERT 3" );					GridData( iGrid, 9, 10, "F" );	GridData( iGrid, 10, 10, "B" );
		GridMerge( iGrid, 11, 9, 2, 1 );		GridData( iGrid, 11, 9, "INSERT 4" );					GridData( iGrid, 11, 10, "F" );	GridData( iGrid, 12, 10, "B" );
		GridMerge( iGrid, 13, 9, 2, 1 );		GridData( iGrid, 13, 9, "INSERT 5" );					GridData( iGrid, 13, 10, "F" );	GridData( iGrid, 14, 10, "B" );
		GridMerge( iGrid, 13, 9, 2, 1 );		GridData( iGrid, 13, 9, "INSERT 5" );					GridData( iGrid, 13, 10, "F" );	GridData( iGrid, 14, 10, "B" );
												GridData( iGrid, 15, 9, "HI-FIX Ŭ����");				GridData( iGrid, 15, 10, "C" );
		GridMerge( iGrid, 16, 9, 1, 2 );		GridData( iGrid, 16, 9, "HI_FIX üũ");

		// TEST SITE 2
		GridMerge( iGrid, 1, 11, 2, 1 );			GridData( iGrid, 1, 11, "����Ʈ 2 �÷���Ʈ");			GridData( iGrid, 1, 12, "F" );	GridData( iGrid, 2, 12, "B" );
		GridMerge( iGrid, 3, 11, 2, 1 );			GridData( iGrid, 3, 11, "COK LOCK" );					GridData( iGrid, 3, 12, "L" );	GridData( iGrid, 4, 12, "UL" );
		GridMerge( iGrid, 5, 11, 2, 1 );			GridData( iGrid, 5, 11, "INSERT 1" );					GridData( iGrid, 5, 12, "F" );	GridData( iGrid, 6, 12, "B" );
		GridMerge( iGrid, 7, 11, 2, 1 );			GridData( iGrid, 7, 11, "INSERT 2" );					GridData( iGrid, 7, 12, "F" );	GridData( iGrid, 8, 12, "B" );
		GridMerge( iGrid, 9, 11, 2, 1 );			GridData( iGrid, 9, 11, "INSERT 3" );					GridData( iGrid, 9, 12, "F" );	GridData( iGrid, 10, 12, "B" );
		GridMerge( iGrid, 11, 11, 2, 1 );		GridData( iGrid, 11, 11, "INSERT 4" );					GridData( iGrid, 11, 12, "F" );	GridData( iGrid, 12, 12, "B" );
		GridMerge( iGrid, 13, 11, 2, 1 );		GridData( iGrid, 13, 11, "INSERT 5" );					GridData( iGrid, 13, 12, "F" );	GridData( iGrid, 14, 12, "B" );
		GridMerge( iGrid, 13, 11, 2, 1 );		GridData( iGrid, 13, 11, "INSERT 5" );					GridData( iGrid, 13, 12, "F" );	GridData( iGrid, 14, 12, "B" );
												GridData( iGrid, 15, 11, "HI-FIX Ŭ����");				GridData( iGrid, 15, 12, "C" );
		GridMerge( iGrid, 16, 11, 1, 2 );		GridData( iGrid, 16, 11, "HI_FIX üũ");

		// COK CHK
		GridData( iGrid, 1, 13, "TS1 COK 10 üũ");
		GridData( iGrid, 2, 13, "TS1 COK 09 üũ");
		GridData( iGrid, 3, 13, "TS1 COK 08 üũ");
		GridData( iGrid, 4, 13, "TS1 COK 07 üũ");
		GridData( iGrid, 5, 13, "TS1 COK 06 üũ");
		GridData( iGrid, 6, 13, "TS1 COK 05 üũ");
		GridData( iGrid, 7, 13, "TS1 COK 04 üũ");
		GridData( iGrid, 8, 13, "TS1 COK 03 üũ");
		GridData( iGrid, 9, 13, "TS1 COK 02 üũ");
		GridData( iGrid, 10, 13, "TS1 COK 01 üũ");

		GridData( iGrid, 11, 13, "TS2 COK 10 üũ");
		GridData( iGrid, 12, 13, "TS2 COK 09 üũ");
		GridData( iGrid, 13, 13, "TS2 COK 08 üũ");
		GridData( iGrid, 14, 13, "TS2 COK 07 üũ");
		GridData( iGrid, 15, 13, "TS2 COK 06 üũ");
		GridData( iGrid, 16, 13, "TS2 COK 05 üũ");
		GridData( iGrid, 17, 13, "TS2 COK 04 üũ");
		GridData( iGrid, 18, 13, "TS2 COK 03 üũ");
		GridData( iGrid, 19, 13, "TS2 COK 02 üũ");
		GridData( iGrid, 20, 13, "TS2 COK 01 üũ");

		// COK STACKER
		GridMerge( iGrid, 18, 9, 2, 1 );			GridData( iGrid, 18, 9, "COK Stacker LOCK" );					GridData( iGrid, 18, 10, "L" );	GridData( iGrid, 19, 10, "UL" );
		GridMerge( iGrid, 20, 9, 1, 2 );			GridData( iGrid, 20, 9, "Stacker ������ üũ" );

		// ������
		GridMerge( iGrid, 18, 11, 1, 2 );			GridData( iGrid, 18, 11, "������ 1" );
		GridMerge( iGrid, 19, 11, 1, 2 );			GridData( iGrid, 19, 11, "������ 2" );
		GridMerge( iGrid, 20, 11, 1, 2 );			GridData( iGrid, 20, 11, "������ 3" );
	}


	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnInitPOGOIO()
{
	TSpread* Grid;
	int col = 4;
	int row = 22;
	int i=0;

	int iGrid = IDC_CUSTOM_POGO_IO;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for( i=0; i<col; i++ )
	{
		if( i % 2 == 0)	Grid->SetColWidthInPixels(i + 1, 25);
		else			Grid->SetColWidthInPixels(i + 1, 25);
	}
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 10 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 12);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );
		}
	}

	// TEST SITE 1
	GridMerge( iGrid, 1, 1, 2, 1 );			GridData( iGrid, 1, 1, "S1");	GridData( iGrid, 1, 2, "F" );	GridData( iGrid, 2, 2, "B" );
	GridMerge( iGrid, 3, 1, 2, 1 );			GridData( iGrid, 3, 1, "PO1" );	GridData( iGrid, 3, 2, "F" );	GridData( iGrid, 4, 2, "B" );
	GridMerge( iGrid, 5, 1, 2, 1 );			GridData( iGrid, 5, 1, "PO2" );				GridData( iGrid, 5, 2, "F" );	GridData( iGrid, 6, 2, "B" );
	GridMerge( iGrid, 7, 1, 2, 1 );			GridData( iGrid, 7, 1, "PO3" );				GridData( iGrid, 7, 2, "F" );	GridData( iGrid, 8, 2, "B" );
	GridMerge( iGrid, 9, 1, 2, 1 );			GridData( iGrid, 9, 1, "PO4" );				GridData( iGrid, 9, 2, "F" );	GridData( iGrid, 10, 2, "B" );
	GridMerge( iGrid, 11, 1, 2, 1 );		GridData( iGrid, 11, 1, "PO5" );				GridData( iGrid, 11, 2, "F" );	GridData( iGrid, 12, 2, "B" );
	GridMerge( iGrid, 13, 1, 2, 1 );		GridData( iGrid, 13, 1, "PO6" );				GridData( iGrid, 13, 2, "F" );	GridData( iGrid, 14, 2, "B" );
	GridMerge( iGrid, 15, 1, 2, 1 );		GridData( iGrid, 15, 1, "PO7" );				GridData( iGrid, 15, 2, "F" );	GridData( iGrid, 16, 2, "B" );
	GridMerge( iGrid, 17, 1, 2, 1 );		GridData( iGrid, 17, 1, "PO8" );				GridData( iGrid, 17, 2, "F" );	GridData( iGrid, 18, 2, "B" );
	GridMerge( iGrid, 19, 1, 2, 1 );		GridData( iGrid, 19, 1, "PO9" );				GridData( iGrid, 19, 2, "F" );	GridData( iGrid, 20, 2, "B" );
	GridMerge( iGrid, 21, 1, 2, 1 );		GridData( iGrid, 21, 1, "PO10" );			GridData( iGrid, 21, 2, "F" );	GridData( iGrid, 22, 2, "B" );

	// TEST SITE 2
	GridMerge( iGrid, 1, 3, 2, 1 );			GridData( iGrid, 1, 3, "S2");	GridData( iGrid, 1, 4, "F" );	GridData( iGrid, 2, 4, "B" );
	GridMerge( iGrid, 3, 3, 2, 1 );			GridData( iGrid, 3, 3, "PO1" );				GridData( iGrid, 3, 4, "F" );	GridData( iGrid, 4, 4, "B" );
	GridMerge( iGrid, 5, 3, 2, 1 );			GridData( iGrid, 5, 3, "PO2" );				GridData( iGrid, 5, 4, "F" );	GridData( iGrid, 6, 4, "B" );
	GridMerge( iGrid, 7, 3, 2, 1 );			GridData( iGrid, 7, 3, "PO3" );				GridData( iGrid, 7, 4, "F" );	GridData( iGrid, 8, 4, "B" );
	GridMerge( iGrid, 9, 3, 2, 1 );			GridData( iGrid, 9, 3, "PO4" );				GridData( iGrid, 9, 4, "F" );	GridData( iGrid, 10, 4, "B" );
	GridMerge( iGrid, 11, 3, 2, 1 );		GridData( iGrid, 11, 3, "PO5" );				GridData( iGrid, 11, 4, "F" );	GridData( iGrid, 12, 4, "B" );
	GridMerge( iGrid, 13, 3, 2, 1 );		GridData( iGrid, 13, 3, "PO6" );				GridData( iGrid, 13, 4, "F" );	GridData( iGrid, 14, 4, "B" );
	GridMerge( iGrid, 15, 3, 2, 1 );		GridData( iGrid, 15, 3, "PO7" );				GridData( iGrid, 15, 4, "F" );	GridData( iGrid, 16, 4, "B" );
	GridMerge( iGrid, 17, 3, 2, 1 );		GridData( iGrid, 17, 3, "PO8" );				GridData( iGrid, 17, 4, "F" );	GridData( iGrid, 18, 4, "B" );
	GridMerge( iGrid, 19, 3, 2, 1 );		GridData( iGrid, 19, 3, "PO9" );				GridData( iGrid, 19, 4, "F" );	GridData( iGrid, 20, 4, "B" );
	GridMerge( iGrid, 21, 3, 2, 1 );		GridData( iGrid, 21, 3, "PO10" );			GridData( iGrid, 21, 4, "F" );	GridData( iGrid, 22, 4, "B" );


	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	COLORREF bk_old, tk_old;
	Grid->GetColor(col, row, &bk_old, &tk_old);
	if( bk_old != bk || tk_old != tk )
	{
		Grid->SetColor(col, row, bk, tk);
		Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::GridControl(UINT nID, int type, int row, int col, int pos)
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
			// SetValue(col, row, data); data = ""0"(1����) data = "1"(2����) data = "2"(3����)
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

void CScreen_Motor_With_IO::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "����ü");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnCellClick( WPARAM wParam, LPARAM lParam )
{
	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	CString sTempMSG;
	int nRet_1, nRet_2, nRet_3, nRet_4;

	if( wParam == IDC_CUSTOM_POGO_IO )
	{
		if( lpcc->Col == 1 )
		{
			switch( lpcc->Row )
			{			
			case 3: case 4:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo10_clamp);
				break;
			case 5: case 6:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo9_clamp);
				break;
			case 7: case 8:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo8_clamp);
				break;
			case 9: case 10:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo7_clamp);
				break;
			case 11: case 12:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo6_clamp);
				break;
			case 13: case 14:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo5_clamp);
				break;
			case 15: case 16:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo4_clamp);
				break;
			case 17: case 18:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo3_clamp);
				break;
			case 19: case 20:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo2_clamp);
				break;
			case 21: case 22:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_pogo1_clamp);
				break;
			}
		}
		if( lpcc->Col == 3 )
		{
			switch( lpcc->Row )
			{			
			case 3: case 4:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo10_clamp);
				break;
			case 5: case 6:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo9_clamp);
				break;
			case 7: case 8:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo8_clamp);
				break;
			case 9: case 10:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo7_clamp);
				break;
			case 11: case 12:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo6_clamp);
				break;
			case 13: case 14:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo5_clamp);
				break;
			case 15: case 16:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo4_clamp);
				break;
			case 17: case 18:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo3_clamp);
				break;
			case 19: case 20:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo2_clamp);
				break;
			case 21: case 22:
				g_ioMgr.set_out_reverse( st_io.o_testsite2_pogo1_clamp);
				break;
			}
		}
		////2015.0321
		CString strText;
		strText.Format("IDC_CUSTOM_POGO_IO = %d %d", lpcc->Col, lpcc->Row);
		Func.On_LogFile_Add(99, strText);
		////

	}

	if( wParam == IDC_CUSTOM_IO )
	{
		if( lpcc->Col == 1 )
		{
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				switch( lpcc->Row )
				{
				case 1: case 2:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker_rail_variable_fwd, st_io.o_bin_stacker_rail_variable_bwd );	break;
				case 3:	case 4:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_guide_fwdbwd );											break;
				//case 5: case 6: g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_pusher_fwdbwd );											break;
				//2017.0104
				case 5: case 6: 
					g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_pusher_fwdbwd );
					g_ioMgr.set_out_reverse( st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd );
					break;
				case 7: case 8: g_ioMgr.set_out_reverse( st_io.o_bin_tray3_align_fwdbwd );													break;
				case 9: case 10: g_ioMgr.set_out_reverse( st_io.o_bin_stopper3_updn );														break;
				case 11: case 12: g_ioMgr.set_out_reverse( st_io.o_bin_elevator3_tray_clamp );												break;
				case 17: case 18:	g_ioMgr.set_out_reverse( st_io.o_reject_transfer_updn );												break;
				}
				
			}
			else
			{
				switch( lpcc->Row )
				{
				case 1: case 2:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker_rail_variable_fwd, st_io.o_bin_stacker_rail_variable_bwd );	break;
				case 3:	case 4:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_guide_fwdbwd );											break;
				//case 5: case 6:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_pusher_fwdbwd );											break;
				//2017.0104
				case 5: case 6:		
					g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_pusher_fwdbwd );	
					g_ioMgr.set_out_reverse( st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd );
					break;
				case 7: case 8:		g_ioMgr.set_out_reverse( st_io.o_bin_tray1_align_fwdbwd );													break;
				case 9: case 10:	g_ioMgr.set_out_reverse( st_io.o_bin_stopper1_updn );														break;
				case 11: case 12:	g_ioMgr.set_out_reverse( st_io.o_bin_elevator1_tray_clamp );												break;
				case 17: case 18:	g_ioMgr.set_out_reverse( st_io.o_reject_transfer_updn );												break;
				}
			}
		}
		else if( lpcc->Col == 3 )
		{
			switch( lpcc->Row )
			{
			case 1: case 2:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker_rail_variable_fwd, st_io.o_bin_stacker_rail_variable_bwd );	break;
			case 3:	case 4:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker2_rail_guide_fwdbwd );											break;
			//case 5: case 6: g_ioMgr.set_out_reverse( st_io.o_bin_stacker2_rail_pusher_fwdbwd );											break;
			//2017.0104
			case 5: case 6: 
				g_ioMgr.set_out_reverse( st_io.o_bin_stacker2_rail_pusher_fwdbwd );
				g_ioMgr.set_out_reverse( st_io.o_bin_rear_stacker2_rail_pusher_fwdbwd );
				break;
			case 7: case 8: g_ioMgr.set_out_reverse( st_io.o_bin_tray2_align_fwdbwd );													break;
			case 9: case 10: g_ioMgr.set_out_reverse( st_io.o_bin_stopper2_updn );														break;
			case 11: case 12: g_ioMgr.set_out_reverse( st_io.o_bin_elevator2_tray_clamp );												break;
			}
		}
		else if( lpcc->Col == 5 )
		{
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				switch( lpcc->Row )
				{
				case 1: case 2:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker_rail_variable_fwd, st_io.o_bin_stacker_rail_variable_bwd );	break;
				case 3:	case 4:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_guide_fwdbwd );											break;
				//case 5: case 6:		g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_pusher_fwdbwd );											break;
				//2017.0104
				case 5: case 6:		
					g_ioMgr.set_out_reverse( st_io.o_bin_stacker1_rail_pusher_fwdbwd );
					g_ioMgr.set_out_reverse( st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd );
					break;
				case 7: case 8:		g_ioMgr.set_out_reverse( st_io.o_bin_tray1_align_fwdbwd );													break;
				case 9: case 10:	g_ioMgr.set_out_reverse( st_io.o_bin_stopper1_updn );														break;
				case 11: case 12:	g_ioMgr.set_out_reverse( st_io.o_bin_elevator1_tray_clamp );												break;
				}
			}
			else
			{
				switch( lpcc->Row )
				{
				case 1: case 2:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker_rail_variable_fwd, st_io.o_bin_stacker_rail_variable_bwd );	break;
				case 3:	case 4:	g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_guide_fwdbwd );											break;
				//case 5: case 6: g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_pusher_fwdbwd );											break;
				//2017.0104
				case 5: case 6: 
					g_ioMgr.set_out_reverse( st_io.o_bin_stacker3_rail_pusher_fwdbwd );
					g_ioMgr.set_out_reverse( st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd );
					break;
				case 7: case 8: g_ioMgr.set_out_reverse( st_io.o_bin_tray3_align_fwdbwd );													break;
				case 9: case 10: g_ioMgr.set_out_reverse( st_io.o_bin_stopper3_updn );														break;
				case 11: case 12: g_ioMgr.set_out_reverse( st_io.o_bin_elevator3_tray_clamp );												break;
				}
			}
		}
		else if( lpcc->Col == 7 )
		{
			switch( lpcc->Row )
			{
			case 1:	case 2:	g_ioMgr.set_out_reverse( st_io.o_gripper_updn );	break;
			case 3: case 4:	g_ioMgr.set_out_reverse( st_io.o_gripper );			break;
			case 5: case 6:	g_ioMgr.set_out_reverse( st_io.o_picker4_updn );		break;
			case 7: case 8:	g_ioMgr.set_out_reverse( st_io.o_picker3_updn );		break;
			case 9: case 10:	g_ioMgr.set_out_reverse( st_io.o_picker2_updn );		break;
			case 11: case 12:	g_ioMgr.set_out_reverse( st_io.o_picker1_updn );		break;
			case 13:			g_ioMgr.set_out_reverse( st_io.o_picker4_vacuum );	break;
			case 14:			g_ioMgr.set_out_reverse( st_io.o_picker3_vacuum );	break;
			case 15:			g_ioMgr.set_out_reverse( st_io.o_picker2_vacuum );	break;
			case 16:			g_ioMgr.set_out_reverse( st_io.o_picker1_vacuum );	break;
			case 17:			g_ioMgr.set_out_reverse( st_io.o_picker4_eject );	break;
			case 18:			g_ioMgr.set_out_reverse( st_io.o_picker3_eject );	break;
			case 19:			g_ioMgr.set_out_reverse( st_io.o_picker2_eject );	break;
			case 20:			g_ioMgr.set_out_reverse( st_io.o_picker1_eject );	break;
			}
		}
		else if( lpcc->Col == 9 )
		{
			switch( lpcc->Row )
			{
// 			case 1: case 2:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_plate_fwd, st_io.o_testsite1_cok_plate_bwd );	break;
// 			case 3: case 4:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_lockunlock );		break;
// 			case 5: case 6:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert5_fwdbwd );	break;
// 			case 7: case 8:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert4_fwdbwd );	break;
// 			case 9: case 10:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert3_fwdbwd );	break;
// 			case 11: case 12:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert2_fwdbwd );	break;
// 			case 13: case 14:	g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert1_fwdbwd );	break;
// 			case 15:			g_ioMgr.set_out_reverse( st_io.o_testsite1_hifix_clamp );		break;
// 
// 			case 18: case 19:	g_ioMgr.set_out_reverse( st_io.o_cok_stacker_lockunlock );		break;

			case 1: case 2:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						if(g_ioMgr.get_out_bit(st_io.o_testsite1_pogo1_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite1_pogo2_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite1_pogo3_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite1_pogo4_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite1_pogo5_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite1_pogo6_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite1_pogo7_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite1_pogo8_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite1_pogo9_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite1_pogo10_clamp, IO_OFF) == IO_ON)
						{
							nRet_1 = IO_ON;
						}
						if(g_ioMgr.get_in_bit(st_io.i_testsite1_pogo1_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite1_pogo2_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite1_pogo3_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite1_pogo4_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite1_pogo5_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite1_pogo6_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite1_pogo7_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite1_pogo8_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite1_pogo9_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite1_pogo10_unclamp_chk) == IO_OFF)
						{
							nRet_2 = IO_OFF;
						}
						if( nRet_1 == IO_ON || nRet_2 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG = _T("TestSite1�� Clamp �Ǵ� UnClamp ������ ������ ���� �ʽ��ϴ�.");
								if ( g_local.GetLocalType() == LOCAL_ENG ) sTempMSG = _T("TestSite1 Clamp or UnClamp is not same with motion sensor.");//20130930
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_plate_fwd, st_io.o_testsite1_cok_plate_bwd );
				break;

			case 3: case 4:
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_lockunlock );		break;
			case 5: case 6:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo9_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo10_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo9_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo10_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test1Pogo1Clamp = %d, Test1Pogo2Clamp = %d, Test1Pogo2Clamp = %d, Test1Pogo1UnClampChk = %d, Test1Pogo2UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert5_fwdbwd );
				break;

			case 7: case 8:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo7_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo8_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo7_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo8_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test1Pogo3Clamp = %d, Test1Pogo4Clamp = %d, Test1Pogo3UnClampChk = %d, Test1Pogo4UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}				
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert4_fwdbwd );
				break;

			case 9: case 10:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo5_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo6_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo5_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo6_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_3 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test1Pogo5Clamp = %d, Test1Pogo6Clamp = %d, Test1Pogo5UnClampChk = %d, Test1Pogo6UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert3_fwdbwd );
				break;
				
			case 11: case 12:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo3_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo4_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo3_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo4_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test1Pogo7Clamp = %d, Test1Pogo8Clamp = %d, Test1Pogo7UnClampChk = %d, Test1Pogo8UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert2_fwdbwd );
				break;

			case 13: case 14:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo1_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite1_pogo2_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo1_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite1_pogo2_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test1Pogo9Clamp = %d, Test1Pogo10Clamp = %d, Test1Pogo9UnClampChk = %d, Test1Pogo10UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite1_cok_insert1_fwdbwd );	
				break;

			case 15:			g_ioMgr.set_out_reverse( st_io.o_testsite1_hifix_clamp );		break;

			case 18: case 19:	g_ioMgr.set_out_reverse( st_io.o_cok_stacker_lockunlock );		break;
			}
		}
		else if( lpcc->Col == 11 )
		{
			switch( lpcc->Row )
			{
// 			case 1: case 2:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_plate_fwd, st_io.o_testsite2_cok_plate_bwd );	break;
// 			case 3: case 4:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_lockunlock );		break;
// 			case 5: case 6:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert5_fwdbwd );	break;
// 			case 7: case 8:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert4_fwdbwd );	break;
// 			case 9: case 10:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert3_fwdbwd );	break;
// 			case 11: case 12:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert2_fwdbwd );	break;
// 			case 13: case 14:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert1_fwdbwd );	break;
// 			case 15:			g_ioMgr.set_out_reverse( st_io.o_testsite2_hifix_clamp );		break;
// 			
// 			case 18:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor1 );				break;
// 			case 19:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor2 );				break;
// 			case 20:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor3 );				break;
			case 1: case 2:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{

						if(g_ioMgr.get_out_bit(st_io.o_testsite2_pogo1_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite2_pogo2_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite2_pogo3_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite2_pogo4_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite2_pogo5_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite2_pogo6_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite2_pogo7_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite2_pogo8_clamp, IO_OFF) == IO_ON ||
							g_ioMgr.get_out_bit(st_io.o_testsite2_pogo9_clamp, IO_OFF) == IO_ON || g_ioMgr.get_out_bit(st_io.o_testsite2_pogo10_clamp, IO_OFF) == IO_ON)
						{
							nRet_1 = IO_ON;
						}
						if(g_ioMgr.get_in_bit(st_io.i_testsite2_pogo1_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite2_pogo2_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite2_pogo3_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite2_pogo4_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite2_pogo5_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite2_pogo6_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite2_pogo7_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite2_pogo8_unclamp_chk) == IO_OFF ||
							g_ioMgr.get_in_bit(st_io.i_testsite2_pogo9_unclamp_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_testsite2_pogo10_unclamp_chk) == IO_OFF)
						{
							nRet_2 = IO_OFF;
						}
						if( nRet_1 == IO_ON || nRet_2 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG = _T("TestSite2�� Clamp �Ǵ� UnClamp ������ ������ ���� �ʽ��ϴ�.");
								if ( g_local.GetLocalType() == LOCAL_ENG ) sTempMSG = _T("TestSite2 Clamp or UnClamp is not same with motion sensor.");
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_plate_fwd, st_io.o_testsite2_cok_plate_bwd );
				break;

			case 3: case 4:	g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_lockunlock );		break;
			case 5: case 6:	
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo9_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo10_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo9_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo10_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test2Pogo1Clamp = %d, Test2Pogo2Clamp = %d, Test2Pogo1UnClampChk = %d, Test2Pogo2UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert5_fwdbwd );
				break;

			case 7: case 8:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo7_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo8_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo7_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo8_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test2Pogo3Clamp = %d, Test2Pogo4Clamp = %d, Test2Pogo3UnClampChk = %d, Test2Pogo4UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert4_fwdbwd );
				break;

			case 9: case 10:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo5_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo6_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo5_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo6_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test2Pogo5Clamp = %d, Test2Pogo6Clamp = %d, Test2Pogo6Clamp = %d, Test2Pogo5UnClampChk = %d, Test2Pogo6UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert3_fwdbwd );
				break;

			case 11: case 12:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo3_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo4_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo3_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo4_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test2Pogo7Clamp = %d, Test2Pogo8Clamp = %d, Test2Pogo7UnClampChk = %d, Test2Pogo8UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert2_fwdbwd );
				break;

			case 13: case 14:
				if(st_handler.mn_pogo_type == CTL_YES)
				{
					if(st_basic.n_mode_pogo == CTL_YES)
					{
						nRet_1 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo1_clamp, IO_OFF);
						nRet_2 = g_ioMgr.get_out_bit(st_io.o_testsite2_pogo2_clamp, IO_OFF);
						nRet_3 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo1_unclamp_chk);
						nRet_4 = g_ioMgr.get_in_bit(st_io.i_testsite2_pogo2_unclamp_chk);
						if( nRet_1 == IO_ON || nRet_2 == IO_ON || nRet_3 == IO_OFF || nRet_4 == IO_OFF)
						{
							if (st_handler.cwnd_list != NULL)
							{
								sTempMSG.Format("Test2Pogo9Clamp = %d, Test2Pogo10Clamp = %d, Test2Pogo9UnClampChk = %d, Test2Pogo10UnClampChk = %d", nRet_1, nRet_2, nRet_3, nRet_4);
								sprintf(st_msg.c_normal_msg, "%s", sTempMSG);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
							}
							break;
						}
					}
				}
				g_ioMgr.set_out_reverse( st_io.o_testsite2_cok_insert1_fwdbwd );
				break;

			case 15:			g_ioMgr.set_out_reverse( st_io.o_testsite2_hifix_clamp );		break;
			
			case 18:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor1 );				break;
			case 19:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor2 );				break;
			case 20:			g_ioMgr.set_out_reverse( st_io.o_bin_conveyor3 );				break;			
			}
		}
		////2015.0321
		CString strText;
		strText.Format("IDC_CUSTOM_IO = %d %d", lpcc->Col, lpcc->Row);
		Func.On_LogFile_Add(99, strText);
		////

	}

	else if( wParam == IDC_CUSTOM_MOTOR )
	{
		int nAxis = -1;
		int nPos = -1;
		double dGotoPos = 0;

		

		if( lpcc->Col <= 3 )
		{
			switch( lpcc->Row )
			{
			case 1:	nAxis = MOTOR_BIN_1_Z;	dGotoPos = st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_TOP];		nPos = P_BIN_Z_TOP;			break;
			case 2: nAxis = MOTOR_BIN_1_Z;	dGotoPos = st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_WAIT];			nPos = P_BIN_Z_WAIT;	break;
			case 3: nAxis = MOTOR_BIN_1_Z;	dGotoPos = st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_ROTATE];		nPos = P_BIN_Z_ROTATE;	break;
			case 4: nAxis = MOTOR_BIN_1_Z;	dGotoPos = st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_PUT_ON];		nPos = P_BIN_Z_PUT_ON;	break;
			case 5: nAxis = MOTOR_BIN_1_Z;	dGotoPos = st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_BASE];			nPos = P_BIN_Z_BASE;	break;

			case 6: nAxis = MOTOR_BIN_1_R;	dGotoPos = st_motor[MOTOR_BIN_1_R].d_pos[P_BIN_R_BASE];		nPos = P_BIN_R_BASE;	break;
			case 7: nAxis = MOTOR_BIN_1_R;	dGotoPos = st_motor[MOTOR_BIN_1_R].d_pos[P_BIN_R_ROTATE];	nPos = P_BIN_R_ROTATE;	break;

			case 8:	nAxis = MOTOR_BIN_2_Z;	dGotoPos = st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_TOP];			nPos = P_BIN_Z_TOP;	break;
			case 9: nAxis = MOTOR_BIN_2_Z;	dGotoPos = st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_WAIT];			nPos = P_BIN_Z_WAIT;	break;
			case 10: nAxis = MOTOR_BIN_2_Z;	dGotoPos = st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_ROTATE];		nPos = P_BIN_Z_ROTATE;	break;
			case 11: nAxis = MOTOR_BIN_2_Z;	dGotoPos = st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_PUT_ON];		nPos = P_BIN_Z_PUT_ON;	break;
			case 12: nAxis = MOTOR_BIN_2_Z;	dGotoPos = st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_BASE];			nPos = P_BIN_Z_BASE;	break;

			case 13: nAxis = MOTOR_BIN_2_R;	dGotoPos = st_motor[MOTOR_BIN_2_R].d_pos[P_BIN_R_BASE];		nPos = P_BIN_R_BASE;	break;
			case 14: nAxis = MOTOR_BIN_2_R;	dGotoPos = st_motor[MOTOR_BIN_2_R].d_pos[P_BIN_R_ROTATE];	nPos = P_BIN_R_ROTATE;	break;
			}
		}
		else if( lpcc->Col <= 6 )
		{
			switch( lpcc->Row )
			{
			case 1: nAxis = MOTOR_TESTSITE_1_Z;	dGotoPos = st_handler.md_common_teaching[CT_TSITE_1_Z];			nPos = P_SITE_COK_CHANGE;	break;
			case 2: nAxis = MOTOR_TESTSITE_1_Z;	dGotoPos = st_motor[MOTOR_TESTSITE_1_Z].d_pos[P_SITE_SOCKET];	nPos = P_SITE_SOCKET;break;

			case 4: nAxis = MOTOR_TESTSITE_2_Z;	dGotoPos = st_handler.md_common_teaching[CT_TSITE_2_Z];			nPos = P_SITE_COK_CHANGE;	break;
			case 5: nAxis = MOTOR_TESTSITE_2_Z;	dGotoPos = st_motor[MOTOR_TESTSITE_2_Z].d_pos[P_SITE_SOCKET];	nPos = P_SITE_SOCKET;break;

			case 8: nAxis = MOTOR_REJECT_Z;	dGotoPos = st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_TOP];		nPos = P_BIN_Z_TOP;	break;
			case 9: nAxis = MOTOR_REJECT_Z;	dGotoPos = st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_WAIT];	nPos = P_BIN_Z_WAIT;	break;
			case 10: nAxis = MOTOR_REJECT_Z;	dGotoPos = st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_ROTATE];	nPos = P_BIN_Z_ROTATE;	break;
			case 11: nAxis = MOTOR_REJECT_Z;	dGotoPos = st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_PUT_ON];	nPos = P_BIN_Z_PUT_ON;	break;
			case 12: nAxis = MOTOR_REJECT_Z;	dGotoPos = st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_BASE];	nPos = P_BIN_Z_BASE;	break;

			case 13: nAxis = MOTOR_REJECT_R;	dGotoPos = st_motor[MOTOR_REJECT_R].d_pos[P_BIN_R_BASE];	nPos = P_BIN_R_BASE;	break;
			case 14: nAxis = MOTOR_REJECT_R;	dGotoPos = st_motor[MOTOR_REJECT_R].d_pos[P_BIN_R_ROTATE];	nPos = P_BIN_R_ROTATE;	break;
			}
		}
		else if( lpcc->Col <= 9 )
		{
			switch( lpcc->Row )
			{
 			case 1: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY];			nPos = P_Z_SAFETY;	break;
			case 2: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START];	nPos = P_Z_SITE1_PICK_START;	break;
			case 3: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END];	nPos = P_Z_SITE1_PICK_END;		break;
			case 4: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_START];	nPos = P_Z_SITE2_PICK_START;	break;
			case 5: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_END];	nPos = P_Z_SITE2_PICK_END;		break;
			case 6: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK];		nPos = P_Z_TRAY1_PICK;			break;
			case 7: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY2_PICK];		nPos = P_Z_TRAY2_PICK;			break;
			case 8: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE];		nPos = P_Z_REJ_PLACE;			break;
			case 9: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START];			nPos = P_Z_COK_SITE1_PICK_START;			break;
			case 10: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_START];			nPos = P_Z_COK_STACKER_PICK_START;			break;
			case 11: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_END];			nPos = P_Z_COK_STACKER_PICK_END;			break;
			case 12: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PLACE_START];			nPos = P_Z_SITE1_PLACE_START;			break;
			case 13: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PLACE];			nPos = P_Z_TRAY1_PLACE;			break;
			case 14: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PLACE_START];			nPos = P_Z_COK_SITE1_PLACE_START;			break;
			case 15: nAxis = MOTOR_ROBOT_Z;		dGotoPos = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PLACE_START];			nPos = P_Z_COK_STACKER_PLACE_START;			break;
			}
		}
		else if( lpcc->Col <= 12 )
		{
			switch( lpcc->Row )
			{
 			case 1: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SAFETY];			nPos = P_XY_SAFETY;break;
 			case 2: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START];		nPos = P_XY_SITE1_START;break;
 			case 3: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END];		nPos = P_XY_SITE1_END;break;
 			case 4: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START];		nPos = P_XY_SITE2_START;break;
 			case 5: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END];		nPos = P_XY_SITE2_END;break;
 			case 6: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_START];		nPos = P_XY_TRAY1_START;break;
 			case 7: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END];		nPos = P_XY_TRAY1_END;break;
 			case 8: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START];		nPos = P_XY_TRAY2_START;break;
 			case 9: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END];		nPos = P_XY_TRAY2_END;break;
 			case 10: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START];		nPos = P_XY_REJ_START;break;
 			case 11: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END];			nPos = P_XY_REJ_END;break;
 			case 12: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START];	nPos = P_XY_COK_SITE1_START;	break;
 			case 13: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_START];nPos = P_XY_COK_STACKER_START;		break;
 			case 14: nAxis = MOTOR_ROBOT_X;		dGotoPos = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_END];	nPos = P_XY_COK_STACKER_END;	break;
			}
		}
		else if( lpcc->Col <= 15 )
		{
			switch( lpcc->Row )
			{
			case 1: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SAFETY];			nPos = P_XY_SAFETY;break;
			case 2: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START];		nPos = P_XY_SITE1_START;break;
			case 3: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END];		nPos = P_XY_SITE1_END;break;
			case 4: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START];		nPos = P_XY_SITE2_START;break;
			case 5: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END];		nPos = P_XY_SITE2_END;break;
			case 6: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START];		nPos = P_XY_TRAY1_START;break;
			case 7: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END];		nPos = P_XY_TRAY1_END;break;
			case 8: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START];		nPos = P_XY_TRAY2_START;break;
			case 9: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END];		nPos = P_XY_TRAY2_END;break;
			case 10: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START];		nPos = P_XY_REJ_START;break;
			case 11: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];			nPos = P_XY_REJ_END;break;
			case 12: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START];	nPos = P_XY_COK_SITE1_START;	break;
			case 13: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_START];nPos = P_XY_COK_STACKER_START;		break;
			case 14: nAxis = MOTOR_ROBOT_Y;		dGotoPos = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END];	nPos = P_XY_COK_STACKER_END;	break;
			}
		}
		else if( lpcc->Col <= 18 )
		{
			switch( lpcc->Row )
			{
			case 1: nAxis = MOTOR_COK_ROBOT_Y;	dGotoPos = st_handler.md_common_teaching[CT_COK_Y_START];	nPos = P_CY_COK_0;break;
			case 2: nAxis = MOTOR_COK_ROBOT_Y;	dGotoPos = st_handler.md_common_teaching[CT_COK_Y_END];		nPos = P_CY_COK_5;break;
			
			case 4: nAxis = MOTOR_COK_ROBOT_Z;	dGotoPos = st_handler.md_common_teaching[CT_COK_Z_SAFETY];	nPos = P_CZ_SAFETY;break;
			case 5: nAxis = MOTOR_COK_ROBOT_Z;	dGotoPos = st_handler.md_common_teaching[CT_COK_Z_0];		nPos = P_CZ_COK_0;break;
			case 6: nAxis = MOTOR_COK_ROBOT_Z;	dGotoPos = st_handler.md_common_teaching[CT_COK_Z_19];		nPos = P_CZ_COK_19;break;

			case 8: nAxis = MOTOR_TRAY_STACKER;		dGotoPos = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];nPos = P_REJ_TRAY_1EA;	break;
			case 9: nAxis = MOTOR_TRAY_STACKER;		dGotoPos = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];nPos = P_REJ_TRAY_5EA;	break;

			case 11: nAxis = MOTOR_TRAY_TR_Y;	dGotoPos = st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ];	nPos = P_TRANSFER_Y_REJ;	break;
			case 12: nAxis = MOTOR_TRAY_TR_Y;	dGotoPos = st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN];	nPos = P_TRANSFER_Y_BIN;	break;
			
			case 14: nAxis = MOTOR_TRAY_TR_CLAMP;	dGotoPos = st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP];nPos = P_CLAMP_UNCLAMP;	break;
			case 15: nAxis = MOTOR_TRAY_TR_CLAMP;	dGotoPos = st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_CLAMP];nPos = P_CLAMP_CLAMP;	break;
			}
		}
		
		

		// Go ������...
		if( (lpcc->Col - 1) % 3 == 1 )
		{
			// ����.
			if( lpcc->Col == m_nGo_col && lpcc->Row == m_nGo_row )
			{
				// ����..
				GETMOTOR( (ENUM_MOTOR)nAxis ).SetStopReq( true );
				return;
			}
			
			On_GoMotor( nAxis, dGotoPos, lpcc->Row, lpcc->Col );
		}
		// Read ������.
		else if( (lpcc->Col - 1) % 3 == 2 )
		{
			double dvalue = g_comiMgr.Get_MotCurrentPos(nAxis);  // FASTECH ���� Ư�� ���� Ŀ��Ʈ ��ġ ���� �Լ� //
			
			int nResponse = Func.On_Encoder_Check(dvalue);
			if (nResponse == CTL_YES)
			{
				if( AMotor::CheckTeachSafety( nAxis, nPos, dvalue ) == false )
				{
					m_strTempMSG = "�� ƼĪ���� ������ ���� �ֽ��ϴ�. ����Ͻðڽ��ϱ�?";
					if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("It might dangerous this teaching value. Do you want to continue?");

					if( DoModal_Select( m_strTempMSG ) != IDOK )
					{
						g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
						m_nAxis = nAxis;
						return;
					}
				}
				st_motor[nAxis].d_pos[nPos] = dvalue;				
			}
		}
		
		m_nAxis = nAxis;
		////2015.0321
		CString strText;
		strText.Format("IDC_CUSTOM_MOTOR = %d %d", lpcc->Col, lpcc->Row);
		Func.On_LogFile_Add(99, strText);
		////
		
	}
	else if( wParam == IDC_CUSTOM_TARGET_SITE )
	{
		if( m_nRobot == -2 )
			return;

		int iSelect = (lpcc->Col - 1) * 10 + lpcc->Row - 1;
		if( m_nRobot == -1 )
		{
			m_nTarget = 20 + iSelect;
		}
		else
		{
			m_nTarget = iSelect / 2;
		}

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();
	}
	else if( wParam == IDC_CUSTOM_TARGET_ROBOT )
	{
		if( lpcc->Col == 1 && lpcc->Row < 5 )
		{
			m_nRobot = -1;
			if( m_nTarget < 100 && m_nTarget < 20 && m_nTarget > 0)
			{
				m_nTarget *= 2;
				m_nTarget += 20;
			}
		}
		else if( lpcc->Col == 2 && lpcc->Row < 5 ) 
		{
			m_nRobot = lpcc->Row - 1;
			if( m_nTarget < 100 && m_nTarget > 20 )
			{
				m_nTarget -= 20;
				m_nTarget /= 2;
			}
		}
		else
		{
			m_nRobot = -2;
		}


		if( (m_nTarget >= 100 && m_nTarget < 400 && m_nRobot == -1) ||
			(m_nTarget >= 400 && m_nRobot >= -1 ) ||
			(m_nTarget < 400 && m_nRobot == -2 ) )
			m_nTarget = -1;

		OnUpdateTarget_Robot();
		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();

		////2015.0321
		CString strText;
		strText.Format("IDC_CUSTOM_TARGET_ROBOT = %d %d", lpcc->Col, lpcc->Row);
		Func.On_LogFile_Add(99, strText);
		////
	}
	else if( wParam == IDC_CUSTOM_TARGET_REJECT )
	{
		if( m_nRobot < 0  )
			return;

		int x = st_basic.n_tray_x;
		int y = st_basic.n_tray_y;
		if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
		{
			x = st_basic.n_tray_y;
			y = st_basic.n_tray_x;
		}
		m_nTarget = MPOS_INDEX_REJ + GetMdlIdx(lpcc->Col - 1, lpcc->Row - 1, x, y, true ) - 1;

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();
	}
	else if( wParam == IDC_CUSTOM_TARGET_TRAY1 )
	{
		if( m_nRobot < 0  )
			return;

		int x = st_basic.n_tray_x;
		int y = st_basic.n_tray_y;
		if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
		{
			x = st_basic.n_tray_y;
			y = st_basic.n_tray_x;
		}
		m_nTarget = MPOS_INDEX_BIN1 + GetMdlIdx(lpcc->Col - 1, lpcc->Row - 1, x, y, true ) - 1;

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();
	}
	else if( wParam == IDC_CUSTOM_TARGET_TRAY2 )
	{
		if( m_nRobot < 0  )
			return;

		int x = st_basic.n_tray_x;
		int y = st_basic.n_tray_y;
		if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
		{
			x = st_basic.n_tray_y;
			y = st_basic.n_tray_x;
		}
		m_nTarget = MPOS_INDEX_BIN2 + GetMdlIdx(lpcc->Col - 1, lpcc->Row - 1, x, y, true ) - 1;

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();
	}
	else if( wParam == IDC_CUSTOM_TARGET_TRAY3 )//ybs
	{
		if( m_nRobot < 0  )
			return;

		int x = st_basic.n_tray_x;
		int y = st_basic.n_tray_y;
		if( st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 )
		{
			x = st_basic.n_tray_y;
			y = st_basic.n_tray_x;
		}
		m_nTarget = MPOS_INDEX_BIN3 + GetMdlIdx(lpcc->Col - 1, lpcc->Row - 1, x, y, true ) - 1;

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );
		OnUpdateTarget_cokStacker();
	}
	else if( wParam == IDC_CUSTOM_TARGET_COKSTACKER )
	{
		if( m_nRobot > -1  )
			return;

		int iSelect =  lpcc->Row - 1;
		m_nTarget = MPOS_INDEX_COK_STACKER + iSelect;

		OnUpdateTarget_testSite();
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_REJECT );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY1 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY2 );
		OnUpdateTarget_Tray( IDC_CUSTOM_TARGET_TRAY3 );//ybs
		OnUpdateTarget_cokStacker();
	}

	else if( wParam == IDC_CUSTOM_WITH_IO_MOTOR_INFO )
	{
		if( m_nAxis < 0 )
				return;

		if( lpcc->Col == 1 && lpcc->Row == 2 )
		{
			if( g_comiMgr.Get_MotPower( m_nAxis ) == -1 )
			{
				COMI.Set_MotPower( m_nAxis, IO_ON );
			}
			else
			{
				COMI.Set_MotPower( m_nAxis, IO_OFF );
			}
		}
		else if( lpcc->Col == 2 && lpcc->Row == 2 )
		{
			if( COMI.Get_MotAlarmStatus(m_nAxis) != 1 )
			{
				COMI.Set_MotAlarmClear(m_nAxis );
			}
		}
		////2015.0321
		CString strText;
		strText.Format("IDC_CUSTOM_WITH_IO_MOTOR_INFO = %s %s", lpcc->Col, lpcc->Row);
		Func.On_LogFile_Add(99, strText);
		////

	}
}

void CScreen_Motor_With_IO::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TMR_WITH_IO_READ:
		OnUpdateIO();
		OnUpdateMotorInfo();
		break;

	}
	CFormView::OnTimer(nIDEvent);
}

void CScreen_Motor_With_IO::OnUpdateIO()
{
	int iGrid = IDC_CUSTOM_IO;
	int iGrid2 = IDC_CUSTOM_POGO_IO;


	// BIN
	if( (g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_variable_fwd, IO_ON ) == IO_ON ) && (g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_variable_bwd,IO_OFF ) == IO_OFF ) )
		GridColor( iGrid, 1, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 1, 1, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_fwd_chk ) == IO_ON )
		GridColor( iGrid, 1, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 1, 6, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_bwd_chk ) == IO_ON )
		GridColor( iGrid, 2, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 2, 6, RGB(128,255,128), BLACK_C ); 

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		//guide
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 6, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 2, RGB(128,255,128), BLACK_C );

		// pusher
		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 5, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON ))
			GridColor( iGrid, 5, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 5, RGB(128,64,0), BLACK_C ); 		
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 6, RGB(128,255,128), BLACK_C );

		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 3, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON ))
			GridColor( iGrid, 5, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 4, RGB(128,255,128), BLACK_C );

		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 1, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON ))
			GridColor( iGrid, 5, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 2, RGB(128,255,128), BLACK_C );

		// Align
		if( g_ioMgr.get_out_bit( st_io.o_bin_tray1_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 6, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_tray2_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_tray3_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 2, RGB(128,255,128), BLACK_C );

		// stopper
		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper1_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper1_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper1_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 6, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper2_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper2_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper2_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper3_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper3_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper3_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 2, RGB(128,255,128), BLACK_C );

		// Clamp
		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator1_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator1_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 6, RGB(128,255,128), BLACK_C ); 

		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator2_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator2_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 4, RGB(128,255,128), BLACK_C ); 

		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator3_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator3_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 2, RGB(128,255,128), BLACK_C ); 

		// Ʈ���� üũ
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 5, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 5, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 3, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 3, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 1, RGB(128,255,128), BLACK_C ); 

		// ���� ������
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 5, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 5, RGB(128,255,128), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 3, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 3, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 1, RGB(128,255,128), BLACK_C ); 
	}
	else
	{
		//guide
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 2, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_guide_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 3, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_guide_fwd_chk ) == IO_ON )
			GridColor( iGrid, 3, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_guide_bwd_chk ) == IO_ON )
			GridColor( iGrid, 4, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 6, RGB(128,255,128), BLACK_C );

		// pusher
		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 1, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker1_rail_pusher_fwdbwd, IO_ON) == IO_ON ) )
			GridColor( iGrid, 5, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 2, RGB(128,255,128), BLACK_C );

		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 3, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker2_rail_pusher_fwdbwd, IO_ON) == IO_ON ))
			GridColor( iGrid, 5, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 4, RGB(128,255,128), BLACK_C );

		//if( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON )
		//	GridColor( iGrid, 5, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 5, RGB(128,64,0), BLACK_C ); 
		//2017.0104
		if( ( g_ioMgr.get_out_bit( st_io.o_bin_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON ) || ( g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker3_rail_pusher_fwdbwd, IO_ON) == IO_ON ))
			GridColor( iGrid, 5, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_pusher_fwd_chk ) == IO_ON )
			GridColor( iGrid, 5, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_rail_pusher_bwd_chk ) == IO_ON )
			GridColor( iGrid, 6, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 6, RGB(128,255,128), BLACK_C );

		// Align
		if( g_ioMgr.get_out_bit( st_io.o_bin_tray1_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 2, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_tray2_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_tray3_align_fwdbwd, IO_ON) == IO_ON )
			GridColor( iGrid, 7, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_align_fwd_chk ) == IO_ON )
			GridColor( iGrid, 7, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_align_bwd_chk ) == IO_ON )
			GridColor( iGrid, 8, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 6, RGB(128,255,128), BLACK_C );

		// stopper
		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper1_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper1_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 2, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper1_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 2, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper2_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper2_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 4, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper2_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 4, RGB(128,255,128), BLACK_C );

		if( g_ioMgr.get_out_bit( st_io.o_bin_stopper3_updn, IO_ON) == IO_ON )
			GridColor( iGrid, 9, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper3_up_chk ) == IO_ON )
			GridColor( iGrid, 9, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 6, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stopper3_dn_chk ) == IO_ON )
			GridColor( iGrid, 10, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 6, RGB(128,255,128), BLACK_C );

		// Clamp
		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator1_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 1, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator1_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 2, RGB(128,255,128), BLACK_C ); 

		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator2_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 3, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator2_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 4, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 4, RGB(128,255,128), BLACK_C ); 

		if( g_ioMgr.get_out_bit( st_io.o_bin_elevator3_tray_clamp, IO_ON) == IO_ON )
			GridColor( iGrid, 11, 5, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 5, RGB(128,64,0), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_elevator3_tray_clamp_chk ) == IO_ON )
			GridColor( iGrid, 11, 6, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 6, RGB(128,255,128), BLACK_C ); 

		// Ʈ���� üũ
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker1_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 1, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker2_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 3, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 3, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker3_tray_chk ) == IO_ON )
			GridColor( iGrid, 13, 5, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 5, RGB(128,255,128), BLACK_C ); 

		// ���� ������
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 1, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray2_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 3, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 3, RGB(128,255,128), BLACK_C ); 
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray3_stop_position_chk ) == IO_ON )
			GridColor( iGrid, 14, 5, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 5, RGB(128,255,128), BLACK_C );
	}

	// Ʈ���� ��üũ
	if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_in_chk ) == IO_ON )
		GridColor( iGrid, 15, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 15, 1, RGB(128,255,128), BLACK_C ); 

	// Ʈ���� ���� üũ
	if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_direction_chk ) == IO_ON )
		GridColor( iGrid, 16, 1, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 16, 1, RGB(128,255,128), BLACK_C );

	// Ʈ������ �ٿ�
	if( g_ioMgr.get_out_bit( st_io.o_reject_transfer_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 17, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 17, 1, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_reject_transfer_up_chk ) == IO_ON )
		GridColor( iGrid, 17, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 17, 2, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_reject_transfer_dn_chk ) == IO_ON )
		GridColor( iGrid, 18, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 18, 2, RGB(128,255,128), BLACK_C );

	// Robot �׸��� �ٿ�
	if( g_ioMgr.get_out_bit( st_io.o_gripper_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 1, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 1, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_gripper_up_chk ) == IO_ON )
		GridColor( iGrid, 1, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 1, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk ) == IO_ON )
		GridColor( iGrid, 2, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 2, 8, RGB(128,255,128), BLACK_C );

	// �׸��� ���
	if( g_ioMgr.get_out_bit( st_io.o_gripper, IO_ON) == IO_ON )
		GridColor( iGrid, 3, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_gripper_on_chk ) == IO_ON )
		GridColor( iGrid, 3, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_gripper_off_chk ) == IO_ON )
		GridColor( iGrid, 4, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 8, RGB(128,255,128), BLACK_C );

	// ��Ŀ 4 �ٿ�
	if( g_ioMgr.get_out_bit( st_io.o_picker4_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 5, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker4_up_chk ) == IO_ON )
		GridColor( iGrid, 5, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker4_dn_chk ) == IO_ON )
		GridColor( iGrid, 6, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker3_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 7, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker3_up_chk ) == IO_ON )
		GridColor( iGrid, 7, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker3_dn_chk ) == IO_ON )
		GridColor( iGrid, 8, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker2_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 9, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker2_up_chk ) == IO_ON )
		GridColor( iGrid, 9, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker2_dn_chk ) == IO_ON )
		GridColor( iGrid, 10, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker1_updn, IO_ON) == IO_ON )
		GridColor( iGrid, 11, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker1_up_chk ) == IO_ON )
		GridColor( iGrid, 11, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 8, RGB(128,255,128), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker1_dn_chk ) == IO_ON )
		GridColor( iGrid, 12, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 12, 8, RGB(128,255,128), BLACK_C );

	// ����
	if( g_ioMgr.get_out_bit( st_io.o_picker4_vacuum, IO_ON) == IO_ON )
		GridColor( iGrid, 13, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 13, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker4_vacuum_chk ) == IO_ON )
		GridColor( iGrid, 13, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker3_vacuum, IO_ON) == IO_ON )
		GridColor( iGrid, 14, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 14, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker3_vacuum_chk ) == IO_ON )
		GridColor( iGrid, 14, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker2_vacuum, IO_ON) == IO_ON )
		GridColor( iGrid, 15, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 15, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker2_vacuum_chk ) == IO_ON )
		GridColor( iGrid, 15, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 15, 8, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker1_vacuum, IO_ON) == IO_ON )
		GridColor( iGrid, 16, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 16, 7, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_picker1_vacuum_chk ) == IO_ON )
		GridColor( iGrid, 16, 8, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 16, 8, RGB(128,255,128), BLACK_C );

	//�����
	if( g_ioMgr.get_out_bit( st_io.o_picker4_eject, IO_ON) == IO_ON )
		GridColor( iGrid, 17, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 17, 7, RGB(128,64,0), BLACK_C ); 

	if( g_ioMgr.get_out_bit( st_io.o_picker3_eject, IO_ON) == IO_ON )
		GridColor( iGrid, 18, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 18, 7, RGB(128,64,0), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker2_eject, IO_ON) == IO_ON )
		GridColor( iGrid, 19, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 19, 7, RGB(128,64,0), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_picker1_eject, IO_ON) == IO_ON )
		GridColor( iGrid, 20, 7, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 20, 7, RGB(128,64,0), BLACK_C );

	// ����Ʈ 1 �÷���Ʈ
	if( (g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_fwd, IO_ON ) == IO_ON ) && (g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_bwd,IO_OFF ) == IO_OFF ) )
		GridColor( iGrid, 1, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 1, 9, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_fwd_chk ) == IO_ON )
		GridColor( iGrid, 1, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 1, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_bwd_chk ) == IO_ON )
		GridColor( iGrid, 2, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 2, 10, RGB(128,255,128), BLACK_C );

	// ����Ʈ 1 COK LOCK
	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_lockunlock, IO_ON) == IO_ON )
		GridColor( iGrid, 3, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_lock_chk ) == IO_ON )
		GridColor( iGrid, 3, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_unlock_chk ) == IO_ON )
		GridColor( iGrid, 4, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 10, RGB(128,255,128), BLACK_C );

	// Insert
	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_insert5_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 5, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert5_fwd_chk ) == IO_ON )
		GridColor( iGrid, 5, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert5_bwd_chk ) == IO_ON )
		GridColor( iGrid, 6, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_insert4_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 7, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert4_fwd_chk ) == IO_ON )
		GridColor( iGrid, 7, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert4_bwd_chk ) == IO_ON )
		GridColor( iGrid, 8, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_insert3_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 9, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert3_fwd_chk ) == IO_ON )
		GridColor( iGrid, 9, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert3_bwd_chk ) == IO_ON )
		GridColor( iGrid, 10, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_insert2_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 11, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert2_fwd_chk ) == IO_ON )
		GridColor( iGrid, 11, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert2_bwd_chk ) == IO_ON )
		GridColor( iGrid, 12, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 12, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_insert1_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 13, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 13, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert1_fwd_chk ) == IO_ON )
		GridColor( iGrid, 13, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_insert1_bwd_chk ) == IO_ON )
		GridColor( iGrid, 14, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 10, RGB(128,255,128), BLACK_C );

	// POGO ����Ʈ 1 �÷���Ʈ
	if( (g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_fwd, IO_ON ) == IO_ON ) && (g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_bwd,IO_OFF ) == IO_OFF ) )
		GridColor( iGrid2, 1, 1, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid2, 1, 1, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_fwd_chk ) == IO_ON )
		GridColor( iGrid2, 1, 2, RGB(0,128,0), WHITE_C);		    else	GridColor( iGrid2, 1, 2, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_bwd_chk ) == IO_ON )
		GridColor( iGrid2, 2, 2, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid2, 2, 2, RGB(128,255,128), BLACK_C );
	// POGO ����Ʈ 2 �÷���Ʈ
	if( (g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_fwd, IO_ON ) == IO_ON ) && (g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_bwd,IO_OFF ) == IO_OFF ) )
		GridColor( iGrid2, 1, 3, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid2, 1, 3, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_fwd_chk ) == IO_ON )
		GridColor( iGrid2, 1, 4, RGB(0,128,0), WHITE_C);			else	GridColor( iGrid2, 1, 4, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_bwd_chk ) == IO_ON )
		GridColor( iGrid2, 2, 4, RGB(0,128,0), WHITE_C);			else	GridColor( iGrid2, 2, 4, RGB(128,255,128), BLACK_C );

	// Pogo 2012,1225 unclamp_chk�� off �� ��� Clamp�� ON
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo10_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 3, 1, RGB(255,128,64), WHITE_C );
		else
			GridColor( iGrid2, 3, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo10_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 3, 2, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 4, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 3, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 4, 2, RGB(0,128,0), WHITE_C);
		}

		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo9_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 5, 1, RGB(255,128,64), WHITE_C );
		else
			GridColor( iGrid2, 5, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo9_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 5, 2, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 6, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 5, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 6, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo8_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 7, 1, RGB(255,128,64), WHITE_C );
		else	
			GridColor( iGrid2, 7, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo8_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 7, 2, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 8, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 7, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 8, 2, RGB(0,128,0), WHITE_C);
		}

		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo7_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 9, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 9, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo7_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 9, 2, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 10, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 9, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 10, 2, RGB(0,128,0), WHITE_C);
		}

		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo6_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 11, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 11, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo6_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 11, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 12, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 11, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 12, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo5_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 13, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 13, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo5_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 13, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 14, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 13, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 14, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo4_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 15, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 15, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo4_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 15, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 16, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 15, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 16, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo3_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 17, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 17, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo3_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 17, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 18, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 17, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 18, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo2_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 19, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 19, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo2_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 19, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 20, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 19, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 20, 2, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo1_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 21, 1, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 21, 1, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo1_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 21, 2, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 22, 2, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 21, 2, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 22, 2, RGB(0,128,0), WHITE_C);
		}


		//site2
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo10_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 3, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 3, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo10_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 3, 4, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 4, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 3, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 4, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo9_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 5, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 5, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo9_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 5, 4, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 6, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 5, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 6, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo8_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 7, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 7, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo8_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 7, 4, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 8, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 7, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 8, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo7_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 9, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 9, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo7_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 9, 4, RGB(0,128,0), WHITE_C);
			GridColor( iGrid2, 10, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 9, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 10, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo6_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 11, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 11, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo6_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 11, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 12, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 11, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 12, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo5_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 13, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 13, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo5_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 13, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 14, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 13, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 14, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo4_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 15, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 15, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo4_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 15, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 16, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 15, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 16, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo3_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 17, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 17, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo3_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 17, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 18, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 17, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 18, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo2_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 19, 3, RGB(255,128,64), WHITE_C );		
		else
			GridColor( iGrid2, 19, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo2_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 19, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 20, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 19, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 20, 4, RGB(0,128,0), WHITE_C);
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo1_clamp, IO_ON) == IO_ON)
			GridColor( iGrid2, 21, 3, RGB(255,128,64), WHITE_C );		
		else	
			GridColor( iGrid2, 21, 3, RGB(128,64,0), BLACK_C );
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo1_unclamp_chk) == IO_OFF)
		{
			GridColor( iGrid2, 21, 4, RGB(0,128,0), WHITE_C );
			GridColor( iGrid2, 22, 4, RGB(128,255,128), BLACK_C );
		}
		else
		{
			GridColor( iGrid2, 21, 4, RGB(128,255,128), BLACK_C );
			GridColor( iGrid2, 22, 4, RGB(0,128,0), WHITE_C);
		}
	}
	
	// hifix
	if( g_ioMgr.get_out_bit( st_io.o_testsite1_hifix_clamp, IO_ON) == IO_ON )
		GridColor( iGrid, 15, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 15, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_hifix_clamp_chk ) == IO_ON )
		GridColor( iGrid, 15, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 15, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_in_bit( st_io.i_testsite1_hifix_chk) == IO_ON )
		GridColor( iGrid, 16, 9, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 16, 9, RGB(128,255,128), BLACK_C );
	
	// ����Ʈ 1 �÷���Ʈ
	if( (g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_fwd, IO_ON ) == IO_ON ) && (g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_bwd,IO_OFF ) == IO_OFF ) )
		GridColor( iGrid, 1, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 1, 11, RGB(128,64,0), BLACK_C ); 
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_fwd_chk ) == IO_ON )
		GridColor( iGrid, 1, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 1, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_bwd_chk ) == IO_ON )
		GridColor( iGrid, 2, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 2, 12, RGB(128,255,128), BLACK_C );

	// ����Ʈ 2 COK LOCK
	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_lockunlock, IO_ON) == IO_ON )
		GridColor( iGrid, 3, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 3, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_lock_chk ) == IO_ON )
		GridColor( iGrid, 3, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_unlock_chk ) == IO_ON )
		GridColor( iGrid, 4, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 12, RGB(128,255,128), BLACK_C );

	// Insert
	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_insert5_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 5, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 5, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert5_fwd_chk ) == IO_ON )
		GridColor( iGrid, 5, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert5_bwd_chk ) == IO_ON )
		GridColor( iGrid, 6, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 12, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_insert4_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 7, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 7, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert4_fwd_chk ) == IO_ON )
		GridColor( iGrid, 7, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert4_bwd_chk ) == IO_ON )
		GridColor( iGrid, 8, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 12, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_insert3_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 9, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 9, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert3_fwd_chk ) == IO_ON )
		GridColor( iGrid, 9, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert3_bwd_chk ) == IO_ON )
		GridColor( iGrid, 10, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 12, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_insert2_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 11, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 11, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert2_fwd_chk ) == IO_ON )
		GridColor( iGrid, 11, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert2_bwd_chk ) == IO_ON )
		GridColor( iGrid, 12, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 12, 12, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_insert1_fwdbwd, IO_ON) == IO_ON )
		GridColor( iGrid, 13, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 13, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert1_fwd_chk ) == IO_ON )
		GridColor( iGrid, 13, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 12, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok_insert1_bwd_chk ) == IO_ON )
		GridColor( iGrid, 14, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 12, RGB(128,255,128), BLACK_C );


	//pogo testsite2
	if(st_handler.mn_pogo_type == CTL_YES)
	{
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo1_clamp, IO_ON) == IO_ON)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo1_unclamp_chk) == IO_OFF)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite2_pogo2_unclamp_chk) == IO_OFF)
		{
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_pogo2_clamp, IO_ON) == IO_ON)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo3_unclamp_chk) == IO_OFF)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo4_unclamp_chk) == IO_OFF)
		{
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo3_clamp, IO_ON) == IO_ON)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo5_unclamp_chk) == IO_OFF)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo6_unclamp_chk) == IO_OFF)
		{
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo4_clamp, IO_ON) == IO_ON)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo7_unclamp_chk) == IO_OFF)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo8_unclamp_chk) == IO_OFF)
		{
		}
		
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_pogo5_clamp, IO_ON) == IO_ON)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo9_unclamp_chk) == IO_OFF)
		{
		}
		if( g_ioMgr.get_in_bit( st_io.i_testsite1_pogo10_unclamp_chk) == IO_OFF)
		{
		}
	}

	// hifix
	if( g_ioMgr.get_out_bit( st_io.o_testsite2_hifix_clamp, IO_ON) == IO_ON )
		GridColor( iGrid, 15, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 15, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_hifix_clamp_chk ) == IO_ON )
		GridColor( iGrid, 15, 12, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 15, 12, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_in_bit( st_io.i_testsite2_hifix_chk) == IO_ON )
		GridColor( iGrid, 16, 11, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 16, 11, RGB(128,255,128), BLACK_C );


	// COK
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok10_chk ) == IO_ON )
		GridColor( iGrid, 1, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 1, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok9_chk ) == IO_ON )
		GridColor( iGrid, 2, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 2, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok8_chk ) == IO_ON )
		GridColor( iGrid, 3, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 3, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok7_chk ) == IO_ON )
		GridColor( iGrid, 4, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 4, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok6_chk ) == IO_ON )
		GridColor( iGrid, 5, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 5, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok5_chk ) == IO_ON )
		GridColor( iGrid, 6, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 6, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok4_chk ) == IO_ON )
		GridColor( iGrid, 7, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 7, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok3_chk ) == IO_ON )
		GridColor( iGrid, 8, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 8, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok2_chk ) == IO_ON )
		GridColor( iGrid, 9, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 9, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite1_cok1_chk ) == IO_ON )
		GridColor( iGrid, 10, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 10, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok10_chk ) == IO_ON )
		GridColor( iGrid, 11, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 11, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok9_chk ) == IO_ON )
		GridColor( iGrid, 12, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 12, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok8_chk ) == IO_ON )
		GridColor( iGrid, 13, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 13, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok7_chk ) == IO_ON )
		GridColor( iGrid, 14, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 14, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok6_chk ) == IO_ON )
		GridColor( iGrid, 15, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 15, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok5_chk ) == IO_ON )
		GridColor( iGrid, 16, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 16, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok4_chk ) == IO_ON )
		GridColor( iGrid, 17, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 17, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok3_chk ) == IO_ON )
		GridColor( iGrid, 18, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 18, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok2_chk ) == IO_ON )
		GridColor( iGrid, 19, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 19, 13, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_testsite2_cok1_chk ) == IO_ON )
		GridColor( iGrid, 20, 13, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 20, 13, RGB(128,255,128), BLACK_C );

	// COK STACKER
	if( g_ioMgr.get_out_bit( st_io.o_cok_stacker_lockunlock, IO_ON) == IO_ON )
		GridColor( iGrid, 18, 9, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 18, 9, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_cok_stacker_lock_chk ) == IO_ON )
		GridColor( iGrid, 18, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 18, 10, RGB(128,255,128), BLACK_C );
	if( g_ioMgr.get_in_bit( st_io.i_cok_stacker_unlock_chk ) == IO_ON )
		GridColor( iGrid, 19, 10, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 19, 10, RGB(128,255,128), BLACK_C );

	if( g_ioMgr.get_in_bit( st_io.i_cok_stacker_position_chk ) == IO_ON )
		GridColor( iGrid, 20, 9, RGB(0,128,0), WHITE_C);		else	GridColor( iGrid, 20, 9, RGB(128,255,128), BLACK_C );

	//������
	if( g_ioMgr.get_out_bit( st_io.o_bin_conveyor1, IO_ON) == IO_ON )
		GridColor( iGrid, 18, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 18, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_out_bit( st_io.o_bin_conveyor2, IO_ON) == IO_ON )
		GridColor( iGrid, 19, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 19, 11, RGB(128,64,0), BLACK_C );
	if( g_ioMgr.get_out_bit( st_io.o_bin_conveyor3, IO_ON) == IO_ON )
		GridColor( iGrid, 20, 11, RGB(255,128,64), WHITE_C );		else	GridColor( iGrid, 20, 11, RGB(128,64,0), BLACK_C );
}

void CScreen_Motor_With_IO::OnInitMotor()
{
	TSpread* Grid;
	int col = 18;
	int row = 15;
	int i=0;

	int iGrid = IDC_CUSTOM_MOTOR;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for( i=0; i<col; i++ )
	{
		if( i % 3 == 0)			Grid->SetColWidthInPixels(i + 1, 110);
		else if( i % 3 == 1)	Grid->SetColWidthInPixels(i + 1, 20);
		else					Grid->SetColWidthInPixels(i + 1, 30);
	}
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 25 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 15);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, BLUE_L, BLACK_C );

			if( ix % 3 == 1 )
				GridData( iGrid, iy + 1, ix + 1, "Go" );
			else if( ix % 3 == 2 )
				GridData( iGrid, iy + 1, ix + 1, "Read" );
		}
	}

	if ( g_local.GetLocalType() == LOCAL_ENG )
	{
		// BIN 1 Z
		GridData( iGrid, 1, 1, "BIN 1 Z Top");		GridColor( iGrid, 1, 1, SKY_C, BLACK_C );
		GridData( iGrid, 2, 1, "BIN 1 Z Wait");			GridColor( iGrid, 2, 1, SKY_C, BLACK_C );
		GridData( iGrid, 3, 1, "BIN 1 Z Rotate");			GridColor( iGrid, 3, 1, SKY_C, BLACK_C );
		GridData( iGrid, 4, 1, "BIN 1 Z Up");			GridColor( iGrid, 4, 1, SKY_C, BLACK_C );
		GridData( iGrid, 5, 1, "BIN 1 Z Btm");			GridColor( iGrid, 5, 1, SKY_C, BLACK_C );

		// BIN 1 R
		GridData( iGrid, 6, 1, "BIN 1 R Basic");			GridColor( iGrid, 6, 1, PINK_D, BLACK_C );
		GridData( iGrid, 7, 1, "BIN 1 R Rotate");			GridColor( iGrid, 7, 1, PINK_D, BLACK_C );

		// BIN 2 Z
		GridData( iGrid, 8, 1, "BIN 2 Z Top");		GridColor( iGrid, 8, 1, SKY_C, BLACK_C );
		GridData( iGrid, 9, 1, "BIN 2 Z Wait");			GridColor( iGrid, 9, 1, SKY_C, BLACK_C );
		GridData( iGrid, 10, 1, "BIN 2 Z Rotate");		GridColor( iGrid, 10, 1, SKY_C, BLACK_C );
		GridData( iGrid, 11, 1, "BIN 2 Z Up");		GridColor( iGrid, 11, 1, SKY_C, BLACK_C );
		GridData( iGrid, 12, 1, "BIN 2 Z Btm");		GridColor( iGrid, 12, 1, SKY_C, BLACK_C );

		// BIN 2 R
		GridData( iGrid, 13, 1, "BIN 2 R Basic");		GridColor( iGrid, 13, 1, PINK_D, BLACK_C );
		GridData( iGrid, 14, 1, "BIN 2 R Rotate");		GridColor( iGrid, 14, 1, PINK_D, BLACK_C );

		// BIN 3 Z
		GridData( iGrid, 8, 4, "BIN 3 Z Top");		GridColor( iGrid, 8, 4, SKY_C, BLACK_C );
		GridData( iGrid, 9, 4, "BIN 3 Z Wait");		GridColor( iGrid, 9, 4, SKY_C, BLACK_C );
		GridData( iGrid, 10, 4, "BIN 3 Z Rotate");		GridColor( iGrid, 10, 4, SKY_C, BLACK_C );
		GridData( iGrid, 11, 4, "BIN 3 Z Up");		GridColor( iGrid, 11, 4, SKY_C, BLACK_C );
		GridData( iGrid, 12, 4, "BIN 3 Z Btm");		GridColor( iGrid, 12, 4, SKY_C, BLACK_C );

		// BIN 3 R
		GridData( iGrid, 13, 4, "BIN 3 R Basic");		GridColor( iGrid, 13, 4, PINK_D, BLACK_C );
		GridData( iGrid, 14, 4, "BIN 3 R Rotate");		GridColor( iGrid, 14, 4, PINK_D, BLACK_C );

		// TEST SITE
		GridData( iGrid, 1, 4, "SITE 1 COK CHANGE");		GridColor( iGrid, 1, 4, SKY_C, BLACK_C );
		GridData( iGrid, 2, 4, "SITE 1 SOCKET");			GridColor( iGrid, 2, 4, SKY_C, BLACK_C );

		GridData( iGrid, 4, 4, "SITE 2 COK CHANGE");		GridColor( iGrid, 4, 4, SKY_C, BLACK_C );
		GridData( iGrid, 5, 4, "SITE 2 COK SOCKET");			GridColor( iGrid, 5, 4, SKY_C, BLACK_C );

		// ROBOT Z
		GridData( iGrid, 1, 7, "RBT Z Safety");		GridColor( iGrid, 1, 7, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 7, "SITE 1 PICK Start");		GridColor( iGrid, 2, 7, SKY_C, BLACK_C );
		GridData( iGrid, 3, 7, "SITE 1 PICK End");		GridColor( iGrid, 3, 7, SKY_C, BLACK_C );
		GridData( iGrid, 4, 7, "SITE 2 PICK Start");		GridColor( iGrid, 4, 7, SKY_C, BLACK_C );
		GridData( iGrid, 5, 7, "SITE 2 PICK End");		GridColor( iGrid, 5, 7, SKY_C, BLACK_C );
		GridData( iGrid, 6, 7, "TRAY 1 PICK");			GridColor( iGrid, 6, 7, SKY_C, BLACK_C );
		GridData( iGrid, 7, 7, "TRAY 2 PICK");			GridColor( iGrid, 7, 7, SKY_C, BLACK_C );
		GridData( iGrid, 8, 7, "REJ PICK");				GridColor( iGrid, 8, 7, SKY_C, BLACK_C );

		GridData( iGrid, 9, 7, "SITE 1 COK PICK Start");		GridColor( iGrid, 9, 7, PINK_D, BLACK_C );
		GridData( iGrid, 10, 7, "COK Stacker PICK Start");	GridColor( iGrid, 10, 7, PINK_D, BLACK_C );
		GridData( iGrid, 11, 7, "COK Stacker PICK End");		GridColor( iGrid, 11, 7, PINK_D, BLACK_C );

		GridData( iGrid, 12, 7, "SITE 1 PLACE Start");		GridColor( iGrid, 12, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 7, "TRAY 1 PLACE Start");		GridColor( iGrid, 13, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 14, 7, "SITE 1 COK PLACE Start");		GridColor( iGrid, 14, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 15, 7, "COK Stacker PLACE Start");		GridColor( iGrid, 15, 7, GREEN_L, BLACK_C );

		// ROBOT X
		GridData( iGrid, 1, 10, "RBT X Safety");		GridColor( iGrid, 1, 10, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 10, "RBT X SITE1 Start");		GridColor( iGrid, 2, 10, SKY_C, BLACK_C );
		GridData( iGrid, 3, 10, "RBT X SITE1 End");		GridColor( iGrid, 3, 10, SKY_C, BLACK_C );
		GridData( iGrid, 4, 10, "RBT X SITE2 Start");		GridColor( iGrid, 4, 10, SKY_C, BLACK_C );
		GridData( iGrid, 5, 10, "RBT X SITE2 End");		GridColor( iGrid, 5, 10, SKY_C, BLACK_C );
		GridData( iGrid, 6, 10, "RBT X TRAY1 Start");		GridColor( iGrid, 6, 10, PINK_D, BLACK_C );
		GridData( iGrid, 7, 10, "RBT X TRAY1 End");		GridColor( iGrid, 7, 10, PINK_D, BLACK_C );
		GridData( iGrid, 8, 10, "RBT X TRAY2 Start");		GridColor( iGrid, 8, 10, PINK_D, BLACK_C );
		GridData( iGrid, 9, 10, "RBT X TRAY2 End");		GridColor( iGrid, 9, 10, PINK_D, BLACK_C );
		GridData( iGrid, 10, 10, "RBT X REJ Start");		GridColor( iGrid, 10, 10, SKY_C, BLACK_C );
		GridData( iGrid, 11, 10, "RBT X REJ End");		GridColor( iGrid, 11, 10, SKY_C, BLACK_C );
		GridData( iGrid, 12, 10, "RBT X COK S1 Start");	GridColor( iGrid, 12, 10, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 10, "RBT X COK ST Start");	GridColor( iGrid, 13, 10, SKY_C, BLACK_C );
		GridData( iGrid, 14, 10, "RBT X COK ST End");		GridColor( iGrid, 14, 10, SKY_C, BLACK_C );

		// ROBOT Y
		GridData( iGrid, 1, 13, "RBT Y Safety");		GridColor( iGrid, 1, 13, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 13, "RBT Y SITE1 Start");		GridColor( iGrid, 2, 13, SKY_C, BLACK_C );
		GridData( iGrid, 3, 13, "RBT Y SITE1 End");		GridColor( iGrid, 3, 13, SKY_C, BLACK_C );
		GridData( iGrid, 4, 13, "RBT Y SITE2 Start");		GridColor( iGrid, 4, 13, SKY_C, BLACK_C );
		GridData( iGrid, 5, 13, "RBT Y SITE2 End");		GridColor( iGrid, 5, 13, SKY_C, BLACK_C );
		GridData( iGrid, 6, 13, "RBT Y TRAY1 Start");		GridColor( iGrid, 6, 13, PINK_D, BLACK_C );
		GridData( iGrid, 7, 13, "RBT Y TRAY1 End");		GridColor( iGrid, 7, 13, PINK_D, BLACK_C );
		GridData( iGrid, 8, 13, "RBT Y TRAY2 Start");		GridColor( iGrid, 8, 13, PINK_D, BLACK_C );
		GridData( iGrid, 9, 13, "RBT Y TRAY2 End");		GridColor( iGrid, 9, 13, PINK_D, BLACK_C );
		GridData( iGrid, 10, 13, "RBT Y REJ Start");		GridColor( iGrid, 10, 13, SKY_C, BLACK_C );
		GridData( iGrid, 11, 13, "RBT Y REJ End");		GridColor( iGrid, 11, 13, SKY_C, BLACK_C );
		GridData( iGrid, 12, 13, "RBT Y COK S1 Start");	GridColor( iGrid, 12, 13, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 13, "RBT Y COK ST Start");	GridColor( iGrid, 13, 13, SKY_C, BLACK_C );
		GridData( iGrid, 14, 13, "RBT Y COK ST End");		GridColor( iGrid, 14, 13, SKY_C, BLACK_C );

		
		// COK 
		GridData( iGrid, 1, 16, "COK ROBOT Y Start");			GridColor( iGrid, 1, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 2, 16, "COK ROBOT Y End");			GridColor( iGrid, 2, 16, GREEN_L, BLACK_C );
		
		GridData( iGrid, 4, 16, "COK ROBOT Z SAFETY");			GridColor( iGrid, 4, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 5, 16, "COK ROBOT Z 1EA");			GridColor( iGrid, 5, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 6, 16, "COK ROBOT Z 20EA");			GridColor( iGrid, 6, 16, GREEN_L, BLACK_C );

		// REJECT TRAY STACKER
		GridData( iGrid, 8, 16, "REJ 1");				GridColor( iGrid, 8, 16, PINK_D, BLACK_C );
		GridData( iGrid, 9, 16, "REJ 5");				GridColor( iGrid, 9, 16, PINK_D, BLACK_C );

		// TR Y
		GridData( iGrid, 11, 16, "TR Y Reject");		GridColor( iGrid, 11, 16, YELLOW_L, BLACK_C );
		GridData( iGrid, 12, 16, "TR Y BIN 3");			GridColor( iGrid, 12, 16, YELLOW_L, BLACK_C );
		
		// TR CLAMP
		GridData( iGrid, 14, 16, "TR Clamp Open");		GridColor( iGrid, 14,16, YELLOW_L, BLACK_C );
		GridData( iGrid, 15, 16, "TR Clamp Grip");		GridColor( iGrid, 15, 16, YELLOW_L, BLACK_C );
	}
	else
	{
		// BIN 1 Z
		GridData( iGrid, 1, 1, "BIN 1 Z ž");		GridColor( iGrid, 1, 1, SKY_C, BLACK_C );
		GridData( iGrid, 2, 1, "BIN 1 Z ���");			GridColor( iGrid, 2, 1, SKY_C, BLACK_C );
		GridData( iGrid, 3, 1, "BIN 1 Z ȸ��");			GridColor( iGrid, 3, 1, SKY_C, BLACK_C );
		GridData( iGrid, 4, 1, "BIN 1 Z ���");			GridColor( iGrid, 4, 1, SKY_C, BLACK_C );
		GridData( iGrid, 5, 1, "BIN 1 Z �ٴ�");			GridColor( iGrid, 5, 1, SKY_C, BLACK_C );

		// BIN 1 R
		GridData( iGrid, 6, 1, "BIN 1 R �⺻");			GridColor( iGrid, 6, 1, PINK_D, BLACK_C );
		GridData( iGrid, 7, 1, "BIN 1 R ȸ��");			GridColor( iGrid, 7, 1, PINK_D, BLACK_C );

		// BIN 2 Z
		GridData( iGrid, 8, 1, "BIN 2 Z ž");		GridColor( iGrid, 8, 1, SKY_C, BLACK_C );
		GridData( iGrid, 9, 1, "BIN 2 Z ���");			GridColor( iGrid, 9, 1, SKY_C, BLACK_C );
		GridData( iGrid, 10, 1, "BIN 2 Z ȸ��");		GridColor( iGrid, 10, 1, SKY_C, BLACK_C );
		GridData( iGrid, 11, 1, "BIN 2 Z ���");		GridColor( iGrid, 11, 1, SKY_C, BLACK_C );
		GridData( iGrid, 12, 1, "BIN 2 Z �ٴ�");		GridColor( iGrid, 12, 1, SKY_C, BLACK_C );

		// BIN 2 R
		GridData( iGrid, 13, 1, "BIN 2 R �⺻");		GridColor( iGrid, 13, 1, PINK_D, BLACK_C );
		GridData( iGrid, 14, 1, "BIN 2 R ȸ��");		GridColor( iGrid, 14, 1, PINK_D, BLACK_C );

		// BIN 3 Z
		GridData( iGrid, 8, 4, "BIN 3 Z ž");		GridColor( iGrid, 8, 4, SKY_C, BLACK_C );
		GridData( iGrid, 9, 4, "BIN 3 Z ���");		GridColor( iGrid, 9, 4, SKY_C, BLACK_C );
		GridData( iGrid, 10, 4, "BIN 3 Z ȸ��");		GridColor( iGrid, 10, 4, SKY_C, BLACK_C );
		GridData( iGrid, 11, 4, "BIN 3 Z ���");		GridColor( iGrid, 11, 4, SKY_C, BLACK_C );
		GridData( iGrid, 12, 4, "BIN 3 Z �ٴ�");		GridColor( iGrid, 12, 4, SKY_C, BLACK_C );

		// BIN 3 R
		GridData( iGrid, 13, 4, "BIN 3 R �⺻");		GridColor( iGrid, 13, 4, PINK_D, BLACK_C );
		GridData( iGrid, 14, 4, "BIN 3 R ȸ��");		GridColor( iGrid, 14, 4, PINK_D, BLACK_C );

		// TEST SITE
		GridData( iGrid, 1, 4, "SITE 1 COK CHANGE");		GridColor( iGrid, 1, 4, SKY_C, BLACK_C );
		GridData( iGrid, 2, 4, "SITE 1 SOCKET");			GridColor( iGrid, 2, 4, SKY_C, BLACK_C );

		GridData( iGrid, 4, 4, "SITE 2 COK CHANGE");		GridColor( iGrid, 4, 4, SKY_C, BLACK_C );
		GridData( iGrid, 5, 4, "SITE 2 COK SOCKET");			GridColor( iGrid, 5, 4, SKY_C, BLACK_C );

		// ROBOT Z
		GridData( iGrid, 1, 7, "�κ�Z ������Ƽ");		GridColor( iGrid, 1, 7, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 7, "SITE 1 PICK ����");		GridColor( iGrid, 2, 7, SKY_C, BLACK_C );
		GridData( iGrid, 3, 7, "SITE 1 PICK ��");		GridColor( iGrid, 3, 7, SKY_C, BLACK_C );
		GridData( iGrid, 4, 7, "SITE 2 PICK ����");		GridColor( iGrid, 4, 7, SKY_C, BLACK_C );
		GridData( iGrid, 5, 7, "SITE 2 PICK ��");		GridColor( iGrid, 5, 7, SKY_C, BLACK_C );
		GridData( iGrid, 6, 7, "TRAY 1 PICK");			GridColor( iGrid, 6, 7, SKY_C, BLACK_C );
		GridData( iGrid, 7, 7, "TRAY 2 PICK");			GridColor( iGrid, 7, 7, SKY_C, BLACK_C );
		GridData( iGrid, 8, 7, "REJ PICK");				GridColor( iGrid, 8, 7, SKY_C, BLACK_C );

		GridData( iGrid, 9, 7, "SITE 1 COK PICK ����");		GridColor( iGrid, 9, 7, PINK_D, BLACK_C );
		GridData( iGrid, 10, 7, "COK ����Ŀ PICK ����");	GridColor( iGrid, 10, 7, PINK_D, BLACK_C );
		GridData( iGrid, 11, 7, "COK ����Ŀ PICK ��");		GridColor( iGrid, 11, 7, PINK_D, BLACK_C );

		GridData( iGrid, 12, 7, "SITE 1 PLACE ����");		GridColor( iGrid, 12, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 7, "TRAY 1 PLACE ����");		GridColor( iGrid, 13, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 14, 7, "SITE 1 COK PLACE ����");		GridColor( iGrid, 14, 7, GREEN_L, BLACK_C );
		GridData( iGrid, 15, 7, "COK ����Ŀ PLACE ����");		GridColor( iGrid, 15, 7, GREEN_L, BLACK_C );

		// ROBOT X
		GridData( iGrid, 1, 10, "�κ�X ������Ƽ");		GridColor( iGrid, 1, 10, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 10, "�κ�X SITE1 ����");		GridColor( iGrid, 2, 10, SKY_C, BLACK_C );
		GridData( iGrid, 3, 10, "�κ�X SITE1 ��");		GridColor( iGrid, 3, 10, SKY_C, BLACK_C );
		GridData( iGrid, 4, 10, "�κ�X SITE2 ����");		GridColor( iGrid, 4, 10, SKY_C, BLACK_C );
		GridData( iGrid, 5, 10, "�κ�X SITE2 ��");		GridColor( iGrid, 5, 10, SKY_C, BLACK_C );
		GridData( iGrid, 6, 10, "�κ�X TRAY1 ����");		GridColor( iGrid, 6, 10, PINK_D, BLACK_C );
		GridData( iGrid, 7, 10, "�κ�X TRAY1 ��");		GridColor( iGrid, 7, 10, PINK_D, BLACK_C );
		GridData( iGrid, 8, 10, "�κ�X TRAY2 ����");		GridColor( iGrid, 8, 10, PINK_D, BLACK_C );
		GridData( iGrid, 9, 10, "�κ�X TRAY2 ��");		GridColor( iGrid, 9, 10, PINK_D, BLACK_C );
		GridData( iGrid, 10, 10, "�κ�X REJ ����");		GridColor( iGrid, 10, 10, SKY_C, BLACK_C );
		GridData( iGrid, 11, 10, "�κ�X REJ ��");		GridColor( iGrid, 11, 10, SKY_C, BLACK_C );
		GridData( iGrid, 12, 10, "�κ�X COK S1 ����");	GridColor( iGrid, 12, 10, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 10, "�κ�X COK ST ����");	GridColor( iGrid, 13, 10, SKY_C, BLACK_C );
		GridData( iGrid, 14, 10, "�κ�X COK ST ��");		GridColor( iGrid, 14, 10, SKY_C, BLACK_C );

		// ROBOT Y
		GridData( iGrid, 1, 13, "�κ�Y ������Ƽ");		GridColor( iGrid, 1, 13, YELLOW_L, BLACK_C );
		GridData( iGrid, 2, 13, "�κ�Y SITE1 ����");		GridColor( iGrid, 2, 13, SKY_C, BLACK_C );
		GridData( iGrid, 3, 13, "�κ�Y SITE1 ��");		GridColor( iGrid, 3, 13, SKY_C, BLACK_C );
		GridData( iGrid, 4, 13, "�κ�Y SITE2 ����");		GridColor( iGrid, 4, 13, SKY_C, BLACK_C );
		GridData( iGrid, 5, 13, "�κ�Y SITE2 ��");		GridColor( iGrid, 5, 13, SKY_C, BLACK_C );
		GridData( iGrid, 6, 13, "�κ�Y TRAY1 ����");		GridColor( iGrid, 6, 13, PINK_D, BLACK_C );
		GridData( iGrid, 7, 13, "�κ�Y TRAY1 ��");		GridColor( iGrid, 7, 13, PINK_D, BLACK_C );
		GridData( iGrid, 8, 13, "�κ�Y TRAY2 ����");		GridColor( iGrid, 8, 13, PINK_D, BLACK_C );
		GridData( iGrid, 9, 13, "�κ�Y TRAY2 ��");		GridColor( iGrid, 9, 13, PINK_D, BLACK_C );
		GridData( iGrid, 10, 13, "�κ�Y REJ ����");		GridColor( iGrid, 10, 13, SKY_C, BLACK_C );
		GridData( iGrid, 11, 13, "�κ�Y REJ ��");		GridColor( iGrid, 11, 13, SKY_C, BLACK_C );
		GridData( iGrid, 12, 13, "�κ�Y COK S1 ����");	GridColor( iGrid, 12, 13, GREEN_L, BLACK_C );
		GridData( iGrid, 13, 13, "�κ�Y COK ST ����");	GridColor( iGrid, 13, 13, SKY_C, BLACK_C );
		GridData( iGrid, 14, 13, "�κ�Y COK ST ��");		GridColor( iGrid, 14, 13, SKY_C, BLACK_C );

		
		// COK 
		GridData( iGrid, 1, 16, "COK ROBOT Y ����");			GridColor( iGrid, 1, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 2, 16, "COK ROBOT Y ��");			GridColor( iGrid, 2, 16, GREEN_L, BLACK_C );
		
		GridData( iGrid, 4, 16, "COK ROBOT Z SAFETY");			GridColor( iGrid, 4, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 5, 16, "COK ROBOT Z 1��");			GridColor( iGrid, 5, 16, GREEN_L, BLACK_C );
		GridData( iGrid, 6, 16, "COK ROBOT Z 20��");			GridColor( iGrid, 6, 16, GREEN_L, BLACK_C );

		// REJECT TRAY STACKER
		GridData( iGrid, 8, 16, "REJ 1��");				GridColor( iGrid, 8, 16, PINK_D, BLACK_C );
		GridData( iGrid, 9, 16, "REJ 5��");				GridColor( iGrid, 9, 16, PINK_D, BLACK_C );

		// TR Y
		GridData( iGrid, 11, 16, "TR Y ����Ʈ");		GridColor( iGrid, 11, 16, YELLOW_L, BLACK_C );
		GridData( iGrid, 12, 16, "TR Y BIN 3");			GridColor( iGrid, 12, 16, YELLOW_L, BLACK_C );
		
		// TR CLAMP
		GridData( iGrid, 14, 16, "TR Ŭ���� ����");		GridColor( iGrid, 14,16, YELLOW_L, BLACK_C );
		GridData( iGrid, 15, 16, "TR Ŭ���� ���");		GridColor( iGrid, 15, 16, YELLOW_L, BLACK_C );
	}
}

void CScreen_Motor_With_IO::On_GoMotor( int nAxis, double dGotoPos, int row, int col )
{
	if( nAxis < 0 )
		return;

	if( st_handler.mn_menu_lock )
		return;

	if (COMI.mn_motorbd_init_end != TRUE)  
	{
		if (DoModal_Msg( g_local.Get("L_M_0000") ) == IDOK)
			return ;
	}
	
	//////////////////////////////////////////////////////////////////////////
	GETMOTOR( (ENUM_MOTOR)nAxis ).SetMotGotoPos( dGotoPos );
	
	int nResponse = Func.OnMotor_Go_Check(nAxis, dGotoPos);
	
	if (nResponse != IDOK)
	{
		return;
	}
	
	m_nGo_row = row;
	m_nGo_col = col;
	
	GridData( IDC_CUSTOM_MOTOR, m_nGo_row, m_nGo_col, "...");
	GridColor( IDC_CUSTOM_MOTOR, m_nGo_row, m_nGo_col, RED_L, BLACK_C);
	
	if (st_handler.mn_menu_lock != TRUE)
	{
		st_handler.mn_menu_lock = TRUE;
	}
	// =============================================================================
	
	GETMOTOR( (ENUM_MOTOR)nAxis ).MotorStepClear();
	GETMOTOR( (ENUM_MOTOR)nAxis ).MotorMoveStart( dGotoPos, st_basic.nManualSpeed, MMMODE_WITH_IO_MOVE );
}

LRESULT CScreen_Motor_With_IO::OnMotorCompletion( WPARAM wParam,LPARAM lParam )
{
	if (st_handler.mn_menu_lock != FALSE)
	{
		st_handler.mn_menu_lock = FALSE;
	}
	// ==============================================================================

	if( m_nGo_col > 0 && m_nGo_row > 0 )
	{
		GridData( IDC_CUSTOM_MOTOR, m_nGo_row, m_nGo_col, "Go" );
		GridColor( IDC_CUSTOM_MOTOR, m_nGo_row, m_nGo_col, BLUE_L, BLACK_C);
	}

	m_btn_go.EnableWindow( TRUE );
	m_btn_down.EnableWindow( TRUE );

	m_nGo_col = -1;
	m_nGo_row = -1;

	return 0;
}

void CScreen_Motor_With_IO::OnDestroy() 
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here
	KillTimer( TMR_WITH_IO_READ );

	st_handler.cwnd_motor_with_io = NULL;
	
}

void CScreen_Motor_With_IO::OnInitTarget_TestSite()
{
	TSpread* Grid;
	int col = 2;
	int row = 10;
	int i=0;

	int iGrid = IDC_CUSTOM_TARGET_SITE;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid->SetColWidthInPixels(1, 24);
	Grid->SetColWidthInPixels(2, 24);
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 15 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 15);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );

			CString strNo; strNo.Format("%d", iy + 1 );
			GridData( iGrid, iy + 1, ix + 1, strNo );
		}
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnInitTarget_Robot()
{
	TSpread* Grid;
	int col = 2;
	int row = 4 + 1;
	int i=0;

	int iGrid = IDC_CUSTOM_TARGET_ROBOT;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid->SetColWidthInPixels(1, 20);
	Grid->SetColWidthInPixels(2, 20);
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 20 );
		for( int ix=0; ix<col; ix++ )
		{
			GridFont( iGrid, iy + 1, ix + 1, 15);
			GridControl(iGrid, STATIC, iy + 1, ix + 1, 0);

			GridColor( iGrid, iy + 1, ix + 1, WHITE_C, BLACK );

			if( ix != 0 && iy < 4 )
			{
				CString strNo; strNo.Format("%d", 4 - iy );
				GridData( iGrid, iy + 1, ix + 1, strNo );
			}
		}
	}

	GridMerge( iGrid, 1, 1, 4, 1 );
	GridData(iGrid, 1, 1, "GRIP");

	GridMerge( iGrid, 5,1, 1, 2 );
	GridData( iGrid, 5, 1, "COK Y" );

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnInitTarget_Tray( int iGrid )
{
	TSpread *Grid;

	int col = st_basic.n_tray_x;
	int row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		row = st_basic.n_tray_x;
		col = st_basic.n_tray_y;
	}
	
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
			Grid -> SetRowHeightInPixels(iw + 1, 100 / row );
			for( ih=0; ih<col; ih++ )
			{
				if( iw == 0 )
					Grid -> SetColWidthInPixels(ih + 1, 100 / col );
				
				GridFont( iGrid, iw + 1, ih + 1, 15);
				GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
				GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, true );
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
			}
		}
	}
	else
	{
		for( iw=0; iw<row; iw++ )
		{
			Grid -> SetRowHeightInPixels(iw + 1, 100 / row );
			for( ih=0; ih<col; ih++ )
			{
				if( iw == 0 )
					Grid -> SetColWidthInPixels(ih + 1, 100 / col );
				
				GridFont( iGrid, iw + 1, ih + 1, 15);
				GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
				GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, true );
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
			}
		}
	}

	
	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnUpdateTarget_testSite()
{
	for( int ix = 0; ix<2; ix++ )
	{
		for( int iy=0; iy< 10; iy++ )
		{
			GridColor( IDC_CUSTOM_TARGET_SITE, iy + 1, ix + 1, WHITE_C, BLACK_C );
		}
	}

	if( m_nRobot != -1 )
	{
		if( m_nTarget < 20 )
		{
			GridColor( IDC_CUSTOM_TARGET_SITE, (m_nTarget % 5) * 2 + 1, (m_nTarget / 5) + 1, RED_C, BLACK_C);
			GridColor( IDC_CUSTOM_TARGET_SITE, (m_nTarget % 5) * 2 + 2, (m_nTarget / 5) + 1, RED_C, BLACK_C);
		}
	}
	else
	{
		if( m_nTarget >= 20 && m_nTarget < 100 )
		{
			GridColor( IDC_CUSTOM_TARGET_SITE, (m_nTarget % 10) + 1, (m_nTarget / 10 ) - 1, RED_C, BLACK_C );
		}
	}
}

void CScreen_Motor_With_IO::OnUpdateTarget_Robot()
{
	GridColor( IDC_CUSTOM_TARGET_ROBOT, 1, 1, WHITE_C, BLACK_C );

	GridColor( IDC_CUSTOM_TARGET_ROBOT, 1, 2, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_ROBOT, 2, 2, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_ROBOT, 3, 2, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_ROBOT, 4, 2, WHITE_C, BLACK_C );

	GridColor( IDC_CUSTOM_TARGET_ROBOT, 5, 1, WHITE_C, BLACK_C );

	if( m_nRobot == -1 )
	{
		GridColor( IDC_CUSTOM_TARGET_ROBOT, 1, 1, RED_C, BLACK_C );
	}
	else if( m_nRobot == -2 )
	{
		GridColor( IDC_CUSTOM_TARGET_ROBOT, 5, 1, RED_C, BLACK_C );
	}
	else
	{
		GridColor( IDC_CUSTOM_TARGET_ROBOT, m_nRobot + 1, 2, RED_C, BLACK_C );
	}
}

void CScreen_Motor_With_IO::OnUpdateTarget_Tray( int iGrid )
{
	int col = st_basic.n_tray_x;
	int row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		row = st_basic.n_tray_x;
		col = st_basic.n_tray_y;
	}
	
	for( int iw=0; iw<row; iw++ )
	{
		for( int ih=0; ih<col; ih++ )
		{
			GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
		}
	}

	switch( iGrid )
	{
	case IDC_CUSTOM_TARGET_TRAY1:
		if( GetPosByTarget(m_nTarget) == MPOS_INDEX_BIN1 )
		{
			int x = g_handler.CalctargetIdx( MOTOR_ROBOT_X, m_nTarget );
			int y = g_handler.CalctargetIdx( MOTOR_ROBOT_Y, m_nTarget );
			GridColor( iGrid, y + 1, x + 1, RED_C, BLACK_C);
		}
		break;

	case IDC_CUSTOM_TARGET_TRAY2:
		if( GetPosByTarget(m_nTarget) == MPOS_INDEX_BIN2 )
		{
			int x = g_handler.CalctargetIdx( MOTOR_ROBOT_X, m_nTarget );
			int y = g_handler.CalctargetIdx( MOTOR_ROBOT_Y, m_nTarget );
			GridColor( iGrid, y + 1, x + 1, RED_C, BLACK_C);
		}
		break;

	case IDC_CUSTOM_TARGET_TRAY3://ybs
		if( GetPosByTarget(m_nTarget) == MPOS_INDEX_BIN3 )
		{
			int x = g_handler.CalctargetIdx( MOTOR_ROBOT_X, m_nTarget );
			int y = g_handler.CalctargetIdx( MOTOR_ROBOT_Y, m_nTarget );
			GridColor( iGrid, y + 1, x + 1, RED_C, BLACK_C);
		}
		break;

	case IDC_CUSTOM_TARGET_REJECT:
		if( GetPosByTarget(m_nTarget) == MPOS_INDEX_REJ )
		{
			int x = g_handler.CalctargetIdx( MOTOR_ROBOT_X, m_nTarget );
			int y = g_handler.CalctargetIdx( MOTOR_ROBOT_Y, m_nTarget );
			GridColor( iGrid, y + 1, x + 1, RED_C, BLACK_C);
		}
		break;
	}
	

}

void CScreen_Motor_With_IO::OnBtnRobotGo() 
{
	// TODO: Add your control notification handler code here
	if( m_nTarget < 0 )
		return;

	if (st_handler.mn_menu_lock != TRUE)
	{
		st_handler.mn_menu_lock = TRUE;
	}
	// =============================================================================

	if( m_nRobot == -2 && m_nTarget >= 400 )
	{
		double dPos = GetPosCYAtCokStacker( m_nTarget - 400 );

		GETMOTOR( MOTOR_COK_ROBOT_Y ).MotorStepClear();
		GETMOTOR( MOTOR_COK_ROBOT_Y ).MotorCokYStart( dPos, st_basic.nManualSpeed, MMMODE_WITH_IO_MOVE );
	}
	else
	{
		BOOL bPlace = PLACE;
		if( m_chk_pick.GetCheck() )
			bPlace = PICK;
		double dPosX = g_handler.CalcTargetPosition( MOTOR_ROBOT_X, bPlace, m_nTarget, m_nRobot );
		double dPosY = g_handler.CalcTargetPosition( MOTOR_ROBOT_Y, bPlace, m_nTarget, m_nRobot );
		
		GETMOTOR( MOTOR_ROBOT_Z ).MotorStepClear();
		GETMOTOR( MOTOR_ROBOT_Z ).MotorLinearStart( dPosX, dPosY, st_basic.nManualSpeed, MMMODE_WITH_IO_MOVE );
	}
	
	m_btn_go.EnableWindow( FALSE );

}

void CScreen_Motor_With_IO::OnInitMotorInfo()
{
	TSpread *Grid;

	int col = 2;
	int row = 3;
	int iGrid = IDC_CUSTOM_WITH_IO_MOTOR_INFO;
	
	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 15 );
		for( int ih=0; ih<col; ih++ )
		{
			if( iw == 0 )
				Grid -> SetColWidthInPixels(ih + 1, 109 );
			
			GridFont( iGrid, iw + 1, ih + 1, 15);
			GridColor( iGrid, iw + 1, ih + 1, YELLOW_L, BLACK_C);
			GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
		}
	}

	GridMerge(iGrid, 1, 1, 1, 2 );
	GridData( iGrid, 2, 1, "POWER" );
	GridData( iGrid, 2, 2, "ALARM" );
	GridData( iGrid, 3, 1, "POSITION" );
	
	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnUpdateMotorInfo()
{
	if( m_nAxis < 0 )
		return;

	int iGrid = IDC_CUSTOM_WITH_IO_MOTOR_INFO;

	if( m_nAxis != m_nAxis_old )
	{
		GridData( iGrid, 1, 1, Func.Get_MotorName( m_nAxis ) );
		m_nAxis_old = m_nAxis;
	}

	int nPower = g_comiMgr.Get_MotPower( m_nAxis );
	if( nPower == -1 )
	{
		GridColor( iGrid, 2, 1, BLACK_C, WHITE_C );
	}
	else
	{
		GridColor( iGrid, 2, 1, RED_C, BLACK_C );
	}

	int nAlarm = COMI.Get_MotAlarmStatus( m_nAxis );
	if( nAlarm == 1 )
	{
		GridColor( iGrid, 2, 2, BLACK_C, WHITE_C );
	}
	else
	{
		GridColor( iGrid, 2, 2, RED_C, BLACK_C );
	}

	CString strPos;
	strPos.Format( "%.03f", g_comiMgr.Get_MotCurrentPos(m_nAxis) );

	if( strPos != m_strPos_old )
	{
		GridData( iGrid, 3, 2, strPos );
		m_strPos_old = strPos;
	}
}

void CScreen_Motor_With_IO::OnBtnHome() 
{
	// TODO: Add your control notification handler code here
	if (st_handler.mn_menu_lock != TRUE)
	{
		st_handler.mn_menu_lock = TRUE;
	}

	if( m_nAxis < 0 )
		return;
	
	GETMOTOR( (ENUM_MOTOR)m_nAxis ).MotorStepClear();
	GETMOTOR( (ENUM_MOTOR)m_nAxis ).MotorHomeStart();
}

void CScreen_Motor_With_IO::OnBtnEmgStop() 
{
	if( m_nAxis < 0 )
		return;

	GETMOTOR( (ENUM_MOTOR)m_nAxis ).SetStopReq( true );
}

LONG CScreen_Motor_With_IO::OnUserLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if( m_nAxis < 0 )
		return 0;

	int		nRet;
	
	nRet = Func.DoorOpenCheckSpot();
	
	if (nRet == CTLBD_RET_ERROR)
		return 0;

	// Velocity-Move(Jog) ����϶��� WM_LBUTTONDOWN �̺�Ʈ�� ó���Ѵ�. //
	// Rel/Abs Position Mode������ WM_LBUTTONUP �̺�Ʈ���� �̼� ����� //
	// ó���Ѵ�.                                                       //
	CButtonST *pButton = (CButtonST *)lParam;

	nRet = pButton->GetTag();

	if (pButton->GetTag() == IDC_BTN_MINUS)
	{ // "Jog (-)" ��ư�� ���� ���...
		nRet = CTL_Lib.Motor_SafetyCheck(0, m_nAxis, 0);
			
		if (nRet == CTLBD_RET_ERROR)
		{
			return 0;
		}

		EMOTRET motRet = GETMOTOR( (ENUM_MOTOR)m_nAxis ).JogMoveStart( MINUS );
		if( motRet == MOTRET_ERROR )		cmmErrShowLast(GetSafeHwnd());
		else								st_handler.mn_menu_lock = FALSE;
	}
	else if (pButton->GetTag() == IDC_BTN_PLUS)
	{ 
		nRet = CTL_Lib.Motor_SafetyCheck(0, m_nAxis, 0);
		
		if (nRet == CTLBD_RET_ERROR)
			return 0;
		
		EMOTRET motRet = GETMOTOR( (ENUM_MOTOR)m_nAxis ).JogMoveStart( PLUS );
		
		if( motRet == MOTRET_ERROR )		cmmErrShowLast(GetSafeHwnd());
		else								st_handler.mn_menu_lock = FALSE;
	}

	return 0;
}

LONG CScreen_Motor_With_IO::OnUserLButtonUp(WPARAM wParam, LPARAM lParam)
{	
	if( m_nAxis < 0 )
		return 0;

	int		nRet;

	CButtonST *pButton = (CButtonST *)lParam;

	nRet = pButton->GetTag();

	if (pButton->GetTag() == IDC_BTN_MINUS)
	{ // "Jog (-)" ��ư�� ���� ���...
		cmmSxStop(m_nAxis, FALSE, TRUE);  // ���� �̵� ���� �Լ� 
	}
	else if (pButton->GetTag() == IDC_BTN_PLUS)
	{ 
		cmmSxStop(m_nAxis, FALSE, TRUE);  // ���� �̵� ���� �Լ� 
	}

	return 0;
}

void CScreen_Motor_With_IO::OnInitTarget_cokStacker()
{
	TSpread* Grid;
	int col = 1;
	int row = 6;
	int i=0;

	int iGrid = IDC_CUSTOM_TARGET_COKSTACKER;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);
	
	Grid->SetColWidthInPixels(1, 18);
	
	for( int iy=0; iy<row; iy++ )
	{
		Grid -> SetRowHeightInPixels(iy + 1, 16 );
		GridFont( iGrid, iy + 1, 1, 15);
		GridControl(iGrid, STATIC, iy + 1, 1, 0);
		
		GridColor( iGrid, iy + 1, 1, WHITE_C, BLACK );
		CString strNo; strNo.Format("%d", iy + 1 );
		GridData( iGrid, iy + 1, 1, strNo );
	}

	Grid = NULL;
	delete Grid;
}

void CScreen_Motor_With_IO::OnUpdateTarget_cokStacker()
{
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 1, 1, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 2, 1, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 3, 1, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 4, 1, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 5, 1, WHITE_C, BLACK_C );
	GridColor( IDC_CUSTOM_TARGET_COKSTACKER, 6, 1, WHITE_C, BLACK_C );

	if( GetPosByTarget( m_nTarget ) == MPOS_INDEX_COK_STACKER )
	{
		GridColor( IDC_CUSTOM_TARGET_COKSTACKER, m_nTarget % MPOS_DIVISION + 1, 1, RED_C, BLACK_C );
	}
}

void CScreen_Motor_With_IO::OnBtnDown() 
{

	if( m_nRobot == -2 )
		return;
	// TODO: Add your control notification handler code here
	// z�� motor 
	int nAxis = MOTOR_ROBOT_Z;

	if( nAxis < 0 )
		return;

	if( st_handler.mn_menu_lock )
		return;

	if (COMI.mn_motorbd_init_end != TRUE)  
	{
		if (DoModal_Msg( g_local.Get("L_M_0000") ) == IDOK)
			return ;
	}

	BOOL bPick = PLACE;
	if( m_chk_pick.GetCheck() )
		bPick = PICK;

	double dChk_X = g_handler.CalcTargetPosition( MOTOR_ROBOT_X, bPick, m_nTarget, m_nRobot );
	double dChk_Y = g_handler.CalcTargetPosition( MOTOR_ROBOT_Y, bPick, m_nTarget, m_nRobot );
	if( ( abs(dChk_X - g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) > 0.1f ) ||
		( abs(dChk_Y - g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) > 0.1f ) )
	{
		if ( g_local.GetLocalType() == LOCAL_ENG )
		{
			if (DoModal_Msg( "X, Y axis position does not match." ) == IDOK)//20130930
				return ;
		}
		else
		{
			if (DoModal_Msg( "X, Y�� ��ġ�� ���� �ʽ��ϴ�." ) == IDOK)
				return ;
		}
	}
		
	double dGotoPos = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, bPick, m_nTarget, m_nRobot );
	GETMOTOR( (ENUM_MOTOR)nAxis ).SetMotGotoPos( dGotoPos );
	
	int nResponse = Func.OnMotor_Go_Check(nAxis, dGotoPos);
	
	if (nResponse != IDOK)
	{
		return;
	}

	if (st_handler.mn_menu_lock != TRUE)
	{
		st_handler.mn_menu_lock = TRUE;
	}
	// =============================================================================
	
	GETMOTOR( (ENUM_MOTOR)nAxis ).MotorStepClear();
	GETMOTOR( (ENUM_MOTOR)nAxis ).MotorMoveStart( dGotoPos, st_basic.nManualSpeed, MMMODE_WITH_IO_MOVE );

	m_btn_down.EnableWindow( FALSE );

}

void CScreen_Motor_With_IO::OnBtnTeachS1Start() 
{
	// TODO: Add your control notification handler code here
	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 �� ������ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site1 Start Pos is save.(xyz)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 �� ������ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site1 Start Pos save complete.");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachS1End() 
{
	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 �� ����ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site1 End Pos is save.(xyz)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 �� ����ġ ����Ϸ�";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site1 End Pos save complete.");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachS2Start() 
{
	m_strTempMSG = "�׽�Ʈ ����Ʈ 2 �� ������ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site2 Start Pos is save.(xyz)");
	if (DoModal_Select(m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_START] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 2 �� ������ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site2 Start Pos save complete.");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachS2End() 
{
	m_strTempMSG = "�׽�Ʈ ����Ʈ 2 �� ����ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site2 End Pos is save.(xyz)");
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_END] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 2 �� ����ġ ����Ϸ�";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test_Site2 End Pos save complete.");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachBin1Start() 
{
	m_strTempMSG = "Ʈ���� 1 �� ������ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 Start Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] = dPosZ;

	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_CALC] = dPosY;

	m_strTempMSG = "Ʈ���� 1 �� ������ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 Start Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachBin1End() 
{
	// TODO: Add your control notification handler code here
	m_strTempMSG = "Ʈ���� 1 �� ����ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 End Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END] = dPosY;

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_CALC] = dPosX;

	m_strTempMSG = "Ʈ���� 1 �� ����ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 End Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachBin2Start() 
{
	m_strTempMSG = "Ʈ���� 2 �� ������ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray2 Start Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY2_PICK] = dPosZ;

	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_CALC] = dPosY;

	m_strTempMSG = "Ʈ���� 2 �� ������ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray2 Start Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachBin2End() 
{
	m_strTempMSG = "Ʈ���� 2 �� ����ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray2 End Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] = dPosY;

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_CALC] = dPosX;

	m_strTempMSG = "Ʈ���� 2 �� ����ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray2 End Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachBin3Start() 
{
	m_strTempMSG = "Ʈ���� 3 �� ������ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray3 Start Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;
	
	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);
	
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY3_PICK] = dPosZ;
	
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_CALC] = dPosY;
	
	m_strTempMSG = "Ʈ���� 3 �� ������ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray3 End Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);	
}

void CScreen_Motor_With_IO::OnBtnTeachBin3End() 
{
	m_strTempMSG = "Ʈ���� 3 �� ����ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray3 End Pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;
	
	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_END] = dPosY;
	
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_CALC] = dPosX;
	
	m_strTempMSG = "Ʈ���� 3 �� ����ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray3 End Pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
	
}

void CScreen_Motor_With_IO::OnBtnTeachRejStart() 
{
	m_strTempMSG = "����Ʈ Ʈ������ ������ġ �����մϴ�. (XYZ �÷��̽� ��ġ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Reject Tray Start pos is save.(XYZ)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE] = dPosZ;

	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_CALC] = dPosY;

	m_strTempMSG = "����Ʈ Ʈ������ ������ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Reject Tray Start pos save complete.(XYZ)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachRejEnd() 
{
	m_strTempMSG = "����Ʈ Ʈ������ ����ġ �����մϴ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Reject Tray End pos is save.(XY)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] = dPosY;

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_CALC] = dPosX;

	m_strTempMSG = "����Ʈ Ʈ������ ����ġ ����Ϸ�. (XY)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Reject Tray End pos save complete.(XY)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachCokstStart() 
{
	m_strTempMSG = "COK STACKER�� ������ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER Start pos is save.(XYZ)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_START] = dPosZ;

	m_strTempMSG = "COK STACKER�� ������ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER Start pos save complete.(XYZ)");
	DoModal_Msg( m_strTempMSG);
}

void CScreen_Motor_With_IO::OnBtnTeachCokstEnd() 
{
	m_strTempMSG = "COK STACKER�� ����ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER End pos is save.(XYZ)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_STACKER_END] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PICK_END] = dPosZ;

	m_strTempMSG = "COK STACKER�� ����ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER End pos save complete.(XYZ)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachS1Cok() 
{
	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 COK�� ������ġ �����մϴ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 COK Start pos is save.(XYZ)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_X);
	double dPosY = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Y);
	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START] = dPosX;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START] = dPosY;
	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 COK�� ������ġ ����Ϸ�. (XYZ)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 COK Start pos save complete.(XYZ)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachS1Place() 
{
	// TODO: Add your control notification handler code here
	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 PLACE�� ������ġ �����մϴ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 PLACE Start pos is save.(Z)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PLACE_START] = dPosZ;

	m_strTempMSG = "�׽�Ʈ ����Ʈ 1 PLACE�� ������ġ ����Ϸ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 PLACE Start pos save complete.(Z)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachBin1Place() 
{
	m_strTempMSG = "Ʈ����1�� PLACE ������ġ �����մϴ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 PLACE Start pos is save.(Z)");
	if ( DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PLACE] = dPosZ;

	m_strTempMSG = "Ʈ����1�� PLACE ������ġ ����Ϸ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Tray1 PLACE Start pos save complete.(Z)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachS1CokPlace() 
{
	m_strTempMSG = "�׽�Ʈ����Ʈ 1 COK PLACE ������ġ �����մϴ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 COK PLACE Start pos is save.(Z)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PLACE_START] = dPosZ;

	m_strTempMSG = "�׽�Ʈ����Ʈ 1 COK PLACE ������ġ ����Ϸ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("Test Site1 COK PLACE Start pos save complete.(Z)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachCokstPlace() 
{
	m_strTempMSG = "COK STACKER PLACE ������ġ �����մϴ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER PLACE Start pos is save.(Z)");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosZ = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z);

	st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_STACKER_PLACE_START] = dPosZ;

	m_strTempMSG = "COK STACKER PLACE ������ġ ����Ϸ�. (Z)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T("COK STACKER PLACE Start pos save complete.(Z)");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachS1Calc() 
{
	m_strTempMSG = "SITE 1 �� 2,3,4 ��ġ�� ���, �����մϴ�. (X,Y)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T(" 2,3,4 POS of SITE 1 calculate and save.(X,Y) ");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX_Start = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START];
	double dPosX_End = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END];
	double dPosY_Start = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START];
	double dPosY_End = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END];

	double dGapX = (dPosX_End - dPosX_Start) / 4.0f;
	double dGapY = (dPosY_End - dPosY_Start) / 4.0f;

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] = dPosX_Start + dGapX;
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] = dPosX_Start + dGapX * 2.0f;
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] = dPosX_Start + dGapX * 3.0f;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] = dPosY_Start + dGapY;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] = dPosY_Start + dGapY * 2.0f;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_4] = dPosY_Start + dGapY * 3.0f;

	m_strTempMSG = "SITE 1 �� 2,3,4 ��ġ�� ���, ����Ϸ�. (X,Y)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T(" 2,3,4 POS of SITE 1 calculate and save is completed.(X,Y) ");
	DoModal_Msg( m_strTempMSG );
}

void CScreen_Motor_With_IO::OnBtnTeachS2Calc() 
{
	m_strTempMSG = "SITE 2 �� 2,3,4 ��ġ�� ���, �����մϴ�. (X,Y)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T(" 2,3,4 POS of SITE 2 calculate and save.(X,Y) ");
	if (DoModal_Select( m_strTempMSG ) != IDOK)
		return;

	double dPosX_Start = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START];
	double dPosX_End = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END];
	double dPosY_Start = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START];
	double dPosY_End = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END];

	double dGapX = (dPosX_End - dPosX_Start) / 4.0f;
	double dGapY = (dPosY_End - dPosY_Start) / 4.0f;

	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] = dPosX_Start + dGapX;
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] = dPosX_Start + dGapX * 2.0f;
	st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] = dPosX_Start + dGapX * 3.0f;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_2] = dPosY_Start + dGapY;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_3] = dPosY_Start + dGapY * 2.0f;
	st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] = dPosY_Start + dGapY * 3.0f;
	m_strTempMSG = "SITE 2 �� 2,3,4 ��ġ�� ���, ����Ϸ�. (X,Y)";
	if ( g_local.GetLocalType() == LOCAL_ENG ) m_strTempMSG = _T(" 2,3,4 POS of SITE 2 calculate and save is completed.(X,Y) ");
	DoModal_Msg( m_strTempMSG );
}


void CScreen_Motor_With_IO::OnBtnDoorOpen() 
{
	int nRet, nRet2;
	
	nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
	nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);
	
	if (nRet == IO_ON && nRet2 == IO_ON)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "Manual Mode�� �ƴմϴ�.");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "Manual Mode is Not.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return;
	}
	
	if (COMI.mn_run_status != dSTOP)
	{
		if (st_handler.cwnd_list != NULL)
		{
			sprintf(st_msg.c_abnormal_msg, "��� Stop ���°� �ƴմϴ�..");
			if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_abnormal_msg, "Handler is not Stop.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return ;
	} 
	
	g_ioMgr.set_out_bit(st_io.o_door_lock, IO_OFF);	
}
