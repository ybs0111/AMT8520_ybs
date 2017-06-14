// Dialog_Data_Module.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Data_Module.h"
#include "SrcPart/PartFunction.h"
#include "SrcPart/APartTray.h"
#include "SrcPart/APartTestSite.h"
#include "SrcPart/APartHandler_COK.h"
#include "SrcPart/APartRobot.h"
#include "SrcPart/APartHandler.h"
#include "Dialog_Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_Module dialog


CDialog_Data_Module::CDialog_Data_Module(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Data_Module::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Data_Module)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_caption_main = "DATA (Module)"; 
}


void CDialog_Data_Module::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Data_Module)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Data_Module, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_Data_Module)
	ON_BN_CLICKED(IDC_BTN_MODULE_SET, OnBtnModuleSet)
	ON_BN_CLICKED(IDC_BTN_TRAY_INFO_SET, OnBtnTrayInfoSet)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CALC_TRAY_MOVE, OnBtnCalcTrayMove)
	ON_BN_CLICKED(IDC_BTN_MDL_CLEAR, OnBtnMdlClear)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_Module message handlers

void CDialog_Data_Module::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
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

void CDialog_Data_Module::GridControl(UINT nID, int type, int row, int col, int pos)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	SS_CELLTYPE CellType;
	SS_DATEFORMAT dateFormat = { TRUE, '/', IDF_DDMMYY, FALSE };
	SS_TIMEFORMAT tmFormat;
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
			Grid->SetTypeComboBox(&CellType, 0, m_strCombo);
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
			Grid->SetTypeNumberEx(&CellType,0, 0, 0, m_nNumMax,0,".",",",0,0,1,0,1.001);
			break;
	}
	
	Grid->SetCellType(col,row,&CellType);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);
	if( data == szBuf )
	{
		Grid = NULL;
		delete Grid;
		return;
	}

	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

CString CDialog_Data_Module::GetGridData( UINT nID, int row, int col )
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);

	Grid = NULL;
	delete Grid;

	return szBuf;
}


void CDialog_Data_Module::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

BOOL CDialog_Data_Module::OnInitDialog() 
{
	StandardDialog::OnInitDialog();

	m_selected = EDMS_NONE;
	m_nSelected_ModuleIdx = -1;
	m_strFirstS = EMS_NONE;
	
	OnInit_TestSite();
	OnInit_TrayList();
	OnInit_TrayList_Extra();
	//OnInit_SelectedTray();
	OnInit_RobotPicker();
	OnInit_TrayInfo();
	OnInit_ModuleInfo();

	OnDisplay_TestSite();
	OnDisplay_TrayList();
	OnDisplay_TrayList_Extra();
	OnDisplay_TrayInfo();
	OnDisplay_SelectedTray();
	OnDisplay_Robot();
	OnDisplay_ModuleInfo();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Data_Module::OnInit_TestSite()
{
	TSpread* Grid;
	int row = 11;
	int col = 4;
	//m_nNumMax = 6;

	int iGrid = IDC_CUSTOM_DATA_MDL_TSITE;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 40 );
	Grid -> SetColWidthInPixels(2, 167 );

	Grid -> SetColWidthInPixels(3, 40 );
	Grid -> SetColWidthInPixels(4, 167 );
	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 20);
			
		for( int iw=0; iw<col; iw++ )
		{
			GridFont(iGrid, ih + 1, iw + 1, 15);
			GridColor( iGrid, ih + 1, iw + 1, WHITE_C, BLACK_C );
			GridControl(iGrid, STATIC, ih + 1, iw + 1, 0);
		}

		if( ih > 0 )
		{
			CString strNo;
			strNo.Format( "%d", ih );
			GridData( iGrid, ih + 1, 1, strNo );
			GridData( iGrid, ih + 1, 3, strNo );
		}
		
	}

	GridMerge( iGrid, 1, 1, 1, 2 );	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C ); GridData( iGrid, 1, 1, "TEST SITE 1" );
	GridMerge( iGrid, 1, 3, 1, 2 ); GridColor( iGrid, 1, 3, BLACK_C, WHITE_C );	GridData( iGrid, 1, 3, "TEST SITE 2" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnInit_TrayList()
{
	TSpread* Grid;
	int row = 13;
	int col = 1;
	//m_nNumMax = 6;

	int iGrid = IDC_CUSTOM_DATA_MDL_TRAYLIST;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 120 );

	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 20);
		GridFont(iGrid, ih + 1, 1, 15);
		GridColor( iGrid, ih + 1, 1, WHITE_C, BLACK_C );
		GridControl(iGrid, STATIC, ih +1, 1, 0);
	}

	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C ); GridData( iGrid, 1, 1, "TRAY LIST" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnInit_SelectedTray()
{
	TSpread *Grid;

	int col = st_basic.n_tray_x;
	int row = st_basic.n_tray_y;

	bool bTurn = false;

	int iIdx = m_selected - EDMS_TRAY1;
	if( iIdx > -1 )
	{
		bTurn = (g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN1 ||
			g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN2 ||
			g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN3 ||
			g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT ||
			g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN_TO_REJECT ||
			g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT_TO_BIN );
	}
	
	if( bTurn && (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		row = st_basic.n_tray_x;
		col = st_basic.n_tray_y;
	}
	int iGrid = IDC_CUSTOM_DATA_MDL_SELECTED_TRAY;
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
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn);
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
				
				if( iIdx > -1 && g_Tray.GetTray(iIdx).IsExistModule(iMdlIdx - 1) )
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
			Grid -> SetRowHeightInPixels(iw + 1, 100 / row );
			for( ih=0; ih<col; ih++ )
			{
				if( iw == 0 )
					Grid -> SetColWidthInPixels(ih + 1, 100 / col );
				
				GridFont( iGrid, iw + 1, ih + 1, 15);
				GridColor( iGrid, iw + 1, ih + 1, WHITE_C, BLACK_C);
				GridControl(iGrid, STATIC, iw + 1, ih + 1, 0);
				
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn);
				CString strIdx;
				strIdx.Format( "%02d", iMdlIdx );
				GridData( iGrid, iw + 1, ih + 1, strIdx );
				
				if( iIdx > -1 && g_Tray.GetTray(iIdx).IsExistModule(iMdlIdx - 1) )
				{
					GridColor( iGrid, iw + 1, ih + 1, CalcModuleColor( g_Tray.GetTray(iIdx).GetModule(iMdlIdx - 1) ), BLACK_C);
				}
			}
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnInit_RobotPicker()
{
	TSpread* Grid;
	int row = 5;
	int col = 1;

	int iGrid = IDC_CUSTOM_DATA_MDL_ROBOT;

	Grid = (TSpread*)GetDlgItem( IDC_CUSTOM_DATA_MDL_ROBOT );
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
		Grid -> SetColWidthInPixels(1, 200 );
			
		GridFont( IDC_CUSTOM_DATA_MDL_ROBOT, iw + 1, 1, 15);
		GridControl(IDC_CUSTOM_DATA_MDL_ROBOT, STATIC, iw + 1, 1, 0);
	}

	GridColor( IDC_CUSTOM_DATA_MDL_ROBOT, 1, 1, BLACK, WHITE_C );
	GridData( IDC_CUSTOM_DATA_MDL_ROBOT, 1, 1, "ROBOT" );
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnInit_TrayInfo()
{
	TSpread* Grid;
	int row = 6 + 1;
	int col = 2;
	
	int iGrid = IDC_CUSTOM_DATA_MDL_TRAY_INFO;
	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 100 );
	Grid -> SetColWidthInPixels(2, 160 );
	for( int iw=0; iw<row; iw++ )
	{
		Grid -> SetRowHeightInPixels(iw + 1, 20);
			
		GridFont(iGrid, iw + 1, 1, 15);
		GridFont(iGrid, iw + 1, 2, 15);
		GridControl(iGrid, STATIC, iw + 1, 1, 0);

		GridColor( iGrid, iw + 1, 1, ORANGE_C, BLACK_C );
	}

	GridMerge( iGrid, 1, 1, 1, 2 );
	GridColor(iGrid, 1, 1, BLACK, WHITE_C );
	GridData(iGrid, 1, 1, "TRAY INFO" );

	GridData( iGrid, 2, 1, "POS" );
	GridData( iGrid, 3, 1, "POS_TARGET" );
	GridData( iGrid, 4, 1, "BUFFER NO." );
	GridData( iGrid, 5, 1, "LAST MODULE" );
	GridData( iGrid, 6, 1, "LOT NO." );
	GridData( iGrid, 7, 1, "EMPTY TRAY" );

	m_nNumMax = 9999;
	m_strCombo = "NONE";
	m_strCombo += "\tFRONT\tCONV1\tCONV2\tCONV3\tNEXT";
	m_strCombo += "\tF_TO_CONV1\tC1_TO_C2\tC2_TO_C3\tC3_TO_NEXT";
	m_strCombo += "\tC_TO_WORK1\tC_TO_WORK2\tC_TO_WORK3";
	m_strCombo += "\tWORK_TO_C1\tWORK_TO_C2\tWORK_TO_C3";
	m_strCombo += "\tWORK1\tWORK2\tWORK3";
	m_strCombo += "\tWORK_TO_REJ\tREJ_TO_WORK";
	m_strCombo += "\tREJECT";
	GridControl(iGrid, COMBO, 2, 2, 0);
	GridControl(iGrid, COMBO, 3, 2, 0);
	GridControl(iGrid, NUMBER, 4, 2, 0);
	GridControl(iGrid, STATIC, 5, 2, 0);
	GridControl(iGrid, EDIT, 6, 2, 0);
	GridControl(iGrid, STATIC, 7, 2, 0);
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnInit_ModuleInfo()
{
	TSpread* Grid;
	//	int row = 9 + 1;
	//2013,0715
	//int row = 11 + 1;
	//2013,0924
	//int row = 12 + 1;
	//2015.1123
	int row = 12 + 1 + 1;

	int col = 2;

	int iGrid = IDC_CUSTOM_DATA_MDL_MODULE_INFO;
	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 100 );
	Grid -> SetColWidthInPixels(2, 160 );
	for( int iw=0; iw<row; iw++ )
	{
		//Grid -> SetRowHeightInPixels(iw + 1, 18);
		//2015.1123
		Grid -> SetRowHeightInPixels(iw + 1, 17);
			
		GridFont(iGrid, iw + 1, 1, 14);
		GridFont(iGrid, iw + 1, 2, 14);
		GridControl(iGrid, STATIC, iw + 1, 1, 0);

		GridColor( iGrid, iw + 1, 1, ORANGE_C, BLACK_C );
	}

	GridMerge( iGrid, 1, 1, 1, 2 );
	GridColor(iGrid, 1, 1, BLACK, WHITE_C );
	GridData(iGrid, 1, 1, "MODULE INFO" );

	GridData( iGrid, 2, 1, "LOT NO." );
	GridData( iGrid, 3, 1, "SN" );
	GridData( iGrid, 4, 1, "SERIAL NO." );
	GridData( iGrid, 5, 1, "PPID" );
	GridData( iGrid, 6, 1, "WWN" );
	GridData( iGrid, 7, 1, "BIN" );
	GridData( iGrid, 8, 1, "SCRAP CODE" );
	GridData( iGrid, 9, 1, "STATE" );
//	GridData( iGrid, 10, 1, "TEST COUNT" );
	//2013,0715
	GridData( iGrid, 10, 1, "C_SERIAL NO" );
	GridData( iGrid, 11, 1, "PSID" );
	GridData( iGrid, 12, 1, "TEST COUNT" );
	//2013,0924
	GridData( iGrid, 13, 1, "TEST POS" );
	//2015.1123
	GridData( iGrid, 14, 1, "C_SERIAL2 NO" );

	m_nNumMax = 3;
	m_strCombo = "NONE";
	m_strCombo += "\tLOAD\tREADY\tREADY_RECV\tSTART\tTEST\tABORT\tABORT_RECV\tGOOD\tFAIL\tRELOAD";
	GridControl(iGrid, EDIT, 2, 2, 0);
	GridControl(iGrid, EDIT, 3, 2, 0);
	GridControl(iGrid, EDIT, 4, 2, 0);
	GridControl(iGrid, EDIT, 5, 2, 0);
	GridControl(iGrid, EDIT, 6, 2, 0);
	GridControl(iGrid, EDIT, 7, 2, 0);
	GridControl(iGrid, EDIT, 8, 2, 0);
	GridControl(iGrid, COMBO, 9, 2, 0);
	// 	GridControl(iGrid, NUMBER, 10, 2, 0);
	//2013,0715
	GridControl(iGrid, EDIT, 10, 2, 0);
	GridControl(iGrid, EDIT, 11, 2, 0);
	GridControl(iGrid, NUMBER, 12, 2, 0);
	//2013,0924

	m_nNumMax = 10;
	GridControl(iGrid, NUMBER, 13, 2, 0);
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnDisplay_TestSite()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		AModule mdl = g_site.GetModule(i);
		GridColor( IDC_CUSTOM_DATA_MDL_TSITE, i % 10 + 2, i / 10 * 2 + 2, CalcModuleColor( mdl ), BLACK_C );
		GridData( IDC_CUSTOM_DATA_MDL_TSITE, i %10 + 2, i / 10 * 2 + 2, mdl.GetSerial() );
	}

	if( m_selected == EDMS_TESTSITE && m_nSelected_ModuleIdx > -1 )
	{
		GridColor( IDC_CUSTOM_DATA_MDL_TSITE, m_nSelected_ModuleIdx % 10 + 2, m_nSelected_ModuleIdx / 10 * 2 + 2, ORANGE_L, BLACK_C );
		GridData( IDC_CUSTOM_DATA_MDL_TSITE, m_nSelected_ModuleIdx %10 + 2, m_nSelected_ModuleIdx / 10 * 2 + 2, "SELECTED" );
	}
}

void CDialog_Data_Module::OnDisplay_TrayList()
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		g_Tray.GetTray( i );
		if( g_Tray.GetTray(i).GetPos() == TPOS_NONE )
		{
			GridColor( IDC_CUSTOM_DATA_MDL_TRAYLIST, i + 2, 1, WHITE_C, BLACK_C );
			GridData( IDC_CUSTOM_DATA_MDL_TRAYLIST, i + 2, 1, "" );
		}
		else
		{
			GridColor( IDC_CUSTOM_DATA_MDL_TRAYLIST, i + 2, 1, BLUE_L, BLACK_C );

			CString strTrayName;
			strTrayName.Format( "%s [%d]", g_Tray.GetStrPos(g_Tray.GetTray(i).GetPos()), g_Tray.GetTray(i).GetBufferNo() );
			GridData( IDC_CUSTOM_DATA_MDL_TRAYLIST, i + 2, 1, strTrayName );
		}
	}
	
	if( m_selected >= EDMS_TRAY1 && m_selected <= EDMS_TRAY12 )
	{
		GridColor( IDC_CUSTOM_DATA_MDL_TRAYLIST, m_selected - EDMS_TRAY1 + 2, 1, ORANGE_L, BLACK_C );
		GridData( IDC_CUSTOM_DATA_MDL_TRAYLIST, m_selected - EDMS_TRAY1 + 2, 1, "SELECTED" );
	}
}

void CDialog_Data_Module::OnDisplay_TrayInfo()
{
	ATray tray;
	if( m_selected >= EDMS_TRAY1 &&
		m_selected <= EDMS_TRAY12 )
	{
		tray = g_Tray.GetTray( m_selected - EDMS_TRAY1 );
	}

	CString strPos, strPosTarget, strBufferNo;
	strPos.Format( "%d", tray.GetPos() );
	strPosTarget.Format( "%d", tray.GetTargetPos() );
	strBufferNo.Format( "%d", tray.GetBufferNo() );

	CString strLastMdl = "NO";
	if( tray.GetLastMdl() )
		strLastMdl = "YES";

	CString strEmptyTray = "NO";
	if( tray.GetEmptyTray() )
		strEmptyTray = "YES";



	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 2, 2, strPos );
	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 3, 2, strPosTarget );
	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 4, 2, strBufferNo );
	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 5, 2, strLastMdl );
	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 6, 2, tray.GetStrLotID() );
	GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 7, 2, strEmptyTray );
}

void CDialog_Data_Module::OnDisplay_SelectedTray()
{
	OnInit_SelectedTray();
	
	if( m_selected < EDMS_TRAY1 || m_selected > EDMS_TRAY12 )
		return;

	if( m_nSelected_ModuleIdx < 0 )
		return;

	int col = st_basic.n_tray_x;
	int row = st_basic.n_tray_y;
	bool bTurn = (g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_BIN1 ||
		g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_BIN2 ||
		g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_BIN3 ||
		g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_REJECT ||
		g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_BIN_TO_REJECT ||
		g_Tray.GetTray(m_selected - EDMS_TRAY1).GetPos() == TPOS_REJECT_TO_BIN );

	if( bTurn && 
		(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		row = st_basic.n_tray_x;
		col = st_basic.n_tray_y;
	}

	int iw=0,ih=0;
	//2013,0306
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( iw=row-1; iw>=0; iw-- )
		{
			for( ih=0; ih<col; ih++ )
			{
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn );
				COLORREF color = WHITE_C;
				if( g_Tray.GetTray(m_selected - EDMS_TRAY1).IsExistModule(iMdlIdx - 1) )
				{
					color = CalcModuleColor( g_Tray.GetTray(m_selected - EDMS_TRAY1).GetModule(iMdlIdx - 1) );
				}
				if( (iMdlIdx - 1) == m_nSelected_ModuleIdx )
				{
					color = ORANGE_L;
				}
				GridColor( IDC_CUSTOM_DATA_MDL_SELECTED_TRAY, iw + 1, ih + 1, color, BLACK_C );
			}
		}
	}
	else
	{
		for( iw=0; iw<row; iw++ )
		{
			for( ih=0; ih<col; ih++ )
			{
				int iMdlIdx = GetMdlIdx( ih, iw, col, row, bTurn );
				COLORREF color = WHITE_C;
				if( g_Tray.GetTray(m_selected - EDMS_TRAY1).IsExistModule(iMdlIdx - 1) )
				{
					color = CalcModuleColor( g_Tray.GetTray(m_selected - EDMS_TRAY1).GetModule(iMdlIdx - 1) );
				}
				if( (iMdlIdx - 1) == m_nSelected_ModuleIdx )
				{
					color = ORANGE_L;
				}
				GridColor( IDC_CUSTOM_DATA_MDL_SELECTED_TRAY, iw + 1, ih + 1, color, BLACK_C );
			}
		}
	}
}

void CDialog_Data_Module::OnDisplay_Robot()
{
	for( int i=0; i<PICKER_CNT; i++ )
	{
		if( g_robot.GetPicker(i).GetModule().IsExist() )
		{
			GridColor( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + i, 1, CalcModuleColor( g_robot.GetPicker(i).GetModule() ), BLACK_C );
			GridData( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + i, 1, g_robot.GetPicker(i).GetModule().GetSerial() );
		}
		else
		{
			GridColor( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + i, 1, WHITE_C, BLACK_C );
			GridData( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + i, 1, "" );
		}
	}

	if( m_selected == EDMS_ROBOT && m_nSelected_ModuleIdx > -1 )
	{
		GridColor( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + m_nSelected_ModuleIdx, 1, ORANGE_L, BLACK_C );
		GridData( IDC_CUSTOM_DATA_MDL_ROBOT, 2 + m_nSelected_ModuleIdx, 1, "SELECTED" );
	}

}

void CDialog_Data_Module::OnDisplay_ModuleInfo()
{
	AModule mdl;
	int nTestCnt = -1;
	CString strData;
	if( m_nSelected_ModuleIdx > -1 )
	{
		switch( m_selected )
		{
		case EDMS_TESTSITE:
			mdl = g_site.GetModule( m_nSelected_ModuleIdx );
			//2013,0924
			strData = "";
			nTestCnt = g_site.GetModule(m_nSelected_ModuleIdx).GetTestCnt();
			if( nTestCnt > 0 )
			{
				strData.Format( "%d", mdl.GetTestedPos( nTestCnt - 1 ) + 1 );
			}
			GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 13, 2, strData);
			break;
			
		case EDMS_TRAY1:
		case EDMS_TRAY2:
		case EDMS_TRAY3:
		case EDMS_TRAY4:
		case EDMS_TRAY5:
		case EDMS_TRAY6:
		case EDMS_TRAY7:
		case EDMS_TRAY8:
		case EDMS_TRAY9:
		case EDMS_TRAY10:
		case EDMS_TRAY11:
		case EDMS_TRAY12:
			mdl = g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( m_nSelected_ModuleIdx );
			//2013,0924
			strData = "";
			nTestCnt = g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( m_nSelected_ModuleIdx ).GetTestCnt();
			if( nTestCnt > 0 )
			{
				strData.Format( "%d", mdl.GetTestedPos( nTestCnt - 1 ) + 1 );
			}
			GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 13, 2, strData);
			break;
			
		case EDMS_ROBOT:
			mdl = g_robot.GetPicker( m_nSelected_ModuleIdx ).GetModule();
			//2013,0924
			strData = "";
			nTestCnt = g_robot.GetPicker(m_nSelected_ModuleIdx).GetModule().GetTestCnt();
			if( nTestCnt > 0 )
			{
				strData.Format( "%d", mdl.GetTestedPos( nTestCnt - 1 ) + 1 );
			}
			GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 13, 2, strData);
			break;
		}
	}

	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 2, 2, mdl.GetLotNo() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 3, 2, mdl.GetSN() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 4, 2, mdl.GetSerial() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 5, 2, mdl.GetPPID() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 6, 2, mdl.GetWWN() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 7, 2, mdl.GetBin() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 8, 2, mdl.GetScrapCode() );
	//2013,0715
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 10, 2, mdl.GetCSN() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 11, 2, mdl.GetPSID() );

	CString strState;
	strState.Format( "%d", mdl.GetModuleState() );
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 9, 2, strState );

	CString strCnt;
	strCnt.Format("%d", mdl.GetTestCnt() );
//	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 10, 2, strCnt );
	//2013,0715
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 12, 2, strCnt );

	//2015.1123
	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 14, 2, mdl.GetCSN2() );
}

void CDialog_Data_Module::OnCellClick( WPARAM wParam, LPARAM lParam )
{
	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	ENUM_DATA_MODULE_SELECTE edms_old = m_selected;
	int nSelected_Old = m_nSelected_ModuleIdx;


	switch( wParam )
	{
	case IDC_CUSTOM_DATA_MDL_TSITE:
		{
			if( lpcc->Col % 2 != 0 )	break;
			if( lpcc->Row <= 1 )		break;

			m_selected = EDMS_TESTSITE;
			m_nSelected_ModuleIdx = (lpcc->Col - 1) / 2 * 10 + lpcc->Row - 2;

			//2013,0715
			m_strFirstS = g_site.GetModule(m_nSelected_ModuleIdx).GetModuleState();
		}
		break;

	case IDC_CUSTOM_DATA_MDL_TRAYLIST:
		{
			if( lpcc->Row <= 1 )		break;

			m_selected = (ENUM_DATA_MODULE_SELECTE)(EDMS_TRAY1 + lpcc->Row - 2);
			m_nSelected_ModuleIdx = -1;
		}
		break;
		
	case IDC_CUSTOM_DATA_MDL_ROBOT:
		{
			if( lpcc->Row <= 1 )		break;

			m_selected = EDMS_ROBOT;
			m_nSelected_ModuleIdx = lpcc->Row -2;
		}
		break;

	case IDC_CUSTOM_DATA_MDL_TRAY_INFO:
		{
			//2013,0815 ybs
			int iIdx = m_selected - EDMS_TRAY1;
			if( iIdx > -1 )
			{
				if(g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT)
				{
					AfxMessageBox("Reject 트레이를 수정 할 수 없습니다.");
					return;
				}
			}

			if( lpcc->Col == 1 )		break;

			if( lpcc->Row == 5 ||
				lpcc->Row == 7 )
			{
				CString strVal = GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, lpcc->Row, 2 );
				if( strVal == "NO" )
					strVal = "YES";
				else 
					strVal = "NO";

				GridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, lpcc->Row, 2, strVal );

			}
			break;
		}
		break;

	case IDC_CUSTOM_DATA_MDL_SELECTED_TRAY:
		{
			if( m_selected < EDMS_TRAY1 || m_selected > EDMS_TRAY12 )
				return;

			int col = st_basic.n_tray_x;
			int row = st_basic.n_tray_y;
			
			bool bTurn = false;
			
			int iIdx = m_selected - EDMS_TRAY1;
			if( iIdx > -1 )
			{
				bTurn = (g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN1 ||
					g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN2 ||
					g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN3 ||
					g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT ||
					g_Tray.GetTray(iIdx).GetPos() == TPOS_BIN_TO_REJECT ||
					g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT_TO_BIN );
			}
			
			if( bTurn && (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
			{
				row = st_basic.n_tray_x;
				col = st_basic.n_tray_y;
			}
			m_nSelected_ModuleIdx = GetMdlIdx( lpcc->Col - 1, lpcc->Row - 1, col, row, bTurn ) - 1;
		}
	}


	if( edms_old == m_selected && nSelected_Old == m_nSelected_ModuleIdx )
		return;

	OnDisplay_TestSite();
	OnDisplay_TrayList();
	OnDisplay_TrayInfo();
	OnDisplay_SelectedTray();
	OnDisplay_Robot();
	OnDisplay_ModuleInfo();
}

void CDialog_Data_Module::OnBtnModuleSet() 
{
	if( m_nSelected_ModuleIdx < 0 )
		return;

	AModule mdl;
	mdl.SetLotNo( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 2, 2 ) );
	mdl.SetSN( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 3, 2 ) );
	mdl.SetSerial( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 4, 2 ) );
	mdl.SetPPID( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 5, 2 ) );
	mdl.SetWWN( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 6, 2 ) );
	mdl.SetBin( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 7, 2 ) );
	mdl.SetScrapCode( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 8, 2 ) );

	mdl.SetModuleState( (EMODULE_STATE)atoi(( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 9, 2 ) )) );
	//2013,0623
// 	CString strState;
// 	int nBin = atoi(( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 9, 2 ) ));
// 	strState.Format( "%d", m_strFirstS );
// 	// 	GridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 9, 2, strState );
// 	if(mdl.IsTestEnd() == false)
// 	{
// 		if(atoi(strState) < 8 && (nBin >= 8 && nBin < 10) )
// 		{
// 			CDialog_Message dlgMsg;
// 			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
// 			{
// 				sprintf(st_msg.c_normal_msg, "%d->%d 테스트상황을 변경시킬 수 없습니다.",atoi(strState),nBin);
// 				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
// 			}
// 			st_msg.str_fallacy_msg.Format("%s",st_msg.c_normal_msg);
// 			dlgMsg.DoModal();
// 			mdl.SetModuleState( m_strFirstS );
// 		}
// 	}
// 	else
// 	{
// 		mdl.SetModuleState( (EMODULE_STATE)atoi(( GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 9, 2 ) )) );
// 	}
	
	
	mdl.SetCSN(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 10, 2 ) );
	mdl.SetPSID(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 11, 2 ) );
	
	mdl.SetTestCnt( atoi(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 12, 2 )) );
	//2013,0924
	mdl.SetTestedPos( atoi(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 13, 2 )) );

	//2015.1123
// 	if( st_basic.n_cserial2_mode == CTL_YES )
// 	{
// 		mdl.SetCSN2(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 14, 2 ) );
// 	}
//     else
// 	{
// 		mdl.SetCSN2("");
// 	}
	mdl.SetCSN2(GetGridData( IDC_CUSTOM_DATA_MDL_MODULE_INFO, 14, 2 ) );

	
	switch( m_selected )
	{
	case EDMS_TESTSITE:
		g_site.SetModule( m_nSelected_ModuleIdx, mdl );
		break;

	case EDMS_TRAY1:
	case EDMS_TRAY2:
	case EDMS_TRAY3:
	case EDMS_TRAY4:
	case EDMS_TRAY5:
	case EDMS_TRAY6:
	case EDMS_TRAY7:
	case EDMS_TRAY8:
	case EDMS_TRAY9:
	case EDMS_TRAY10:
	case EDMS_TRAY11:
	case EDMS_TRAY12:
		g_Tray.GetTray( m_selected - EDMS_TRAY1 ).SetModule( m_nSelected_ModuleIdx, mdl );
		break;

	case EDMS_ROBOT:
		g_robot.GetPicker( m_nSelected_ModuleIdx ).SetModule( mdl );
		break;
	}

	OnDisplay_TestSite();
	OnDisplay_TrayList();
	OnDisplay_TrayInfo();
	OnDisplay_SelectedTray();
	OnDisplay_Robot();
	OnDisplay_ModuleInfo();
}

void CDialog_Data_Module::OnBtnTrayInfoSet() 
{
	if( m_selected < EDMS_TRAY1 || m_selected > EDMS_TRAY12 )
		return;

	int iIdx = m_selected - EDMS_TRAY1;
	if( iIdx > -1 )
	{
		if(g_Tray.GetTray(iIdx).GetPos() == TPOS_REJECT)
		{
			AfxMessageBox("Reject 트레이를 수정 할 수 없습니다.");
			return;
		}
	}

	ATray& tray = g_Tray.GetTray( m_selected - EDMS_TRAY1 );
	tray.SetPos( (ETRAY_POS)atoi(GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 2, 2 ) ) );
	tray.SetTargetPos( (ETRAY_POS)atoi(GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 3, 2 ) ) );
	tray.SetBufferNo( atoi( GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 4, 2 ) ) );
	
	CString strLastMdl = GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 5, 2 );
	CString strEmptyTray = GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 7, 2 );
	tray.SetLastMdl( (strLastMdl == "YES" ) );
	tray.SetLotID( GetGridData( IDC_CUSTOM_DATA_MDL_TRAY_INFO, 6, 2 ) );
	tray.SetEmptyTray( (strEmptyTray == "YES" ) );
}

void CDialog_Data_Module::OnDestroy() 
{
	StandardDialog::OnDestroy();
	
	if( st_handler.cwnd_main == NULL )
		return;

	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDR_PICKER);
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_0 );
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_1 );
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_2 );//ybs
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_SEL_TRAY);
	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_REJ_FLOOR );

	for( int i=0; i<MAX_TRAY; i++ )
	{
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_INIT, i );
	}

}

void CDialog_Data_Module::OnBtnCalcTrayMove() 
{
	g_handler.CalcTrayMoveNext_fromRejFull();
	g_handler.CalcTrayMoveNext_fromBin3();
	g_handler.CalcTrayMoveNext_fromReject();
	
	g_handler.CalcTrayMoveNext_fromBin();
}

void CDialog_Data_Module::OnBtnMdlClear() 
{
	switch( m_selected )
	{
	case EDMS_TRAY1:
	case EDMS_TRAY2:
	case EDMS_TRAY3:
	case EDMS_TRAY4:
	case EDMS_TRAY5:
	case EDMS_TRAY6:
	case EDMS_TRAY7:
	case EDMS_TRAY8:
	case EDMS_TRAY9:
	case EDMS_TRAY10:
	case EDMS_TRAY11:
	case EDMS_TRAY12:
		{
			int nCnt = st_basic.n_tray_x * st_basic.n_tray_y;
			for( int i=0; i<nCnt; i++ )
			{
				if( g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( i ).IsExist() )
				{
					g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( i ).SetBin("");
					g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( i ).SetModuleState( EMS_LOAD );
					g_Tray.GetTray( m_selected - EDMS_TRAY1 ).GetModule( i ).SetTestCnt( 0 );
				}
			}
		}
		break;

	default:
		return;
	}

	OnDisplay_TrayList();
	OnDisplay_TrayInfo();
	OnDisplay_SelectedTray();
}

void CDialog_Data_Module::OnInit_TrayList_Extra()
{
	if( g_handler.GetMachinePos() != EMPOS_FRONT )
	{
		GetDlgItem( IDC_CUSTOM_DATA_MDL_TRAY_EXTRA )->MoveWindow( 0, 0, 0, 0 );
		return;
	}

	TSpread* Grid;
	int row = 13;
	int col = 1;
	//m_nNumMax = 6;

	int iGrid = IDC_CUSTOM_DATA_MDL_TRAY_EXTRA;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 120 );

	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 17);
		GridFont(iGrid, ih + 1, 1, 10);
		GridColor( iGrid, ih + 1, 1, WHITE_C, BLACK_C );
		GridControl(iGrid, STATIC, ih +1, 1, 0);
	}

	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C ); GridData( iGrid, 1, 1, "TRAY LIST(EXTRA)" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Module::OnDisplay_TrayList_Extra()
{
	if( g_handler.GetMachinePos() != EMPOS_FRONT )
		return;

	for( int i=0; i<MAX_TRAY; i++ )
	{
		g_Tray.GetTrayExtra( i );
		if( g_Tray.GetTrayExtra(i).GetPos() == TPOS_NONE )
		{
			GridColor( IDC_CUSTOM_DATA_MDL_TRAY_EXTRA, i + 2, 1, WHITE_C, BLACK_C );
			GridData( IDC_CUSTOM_DATA_MDL_TRAY_EXTRA, i + 2, 1, "" );
		}
		else
		{
			GridColor( IDC_CUSTOM_DATA_MDL_TRAY_EXTRA, i + 2, 1, BLUE_L, BLACK_C );

			CString strTrayName;
			strTrayName.Format( "%s/%s", g_Tray.GetStrPos(g_Tray.GetTrayExtra(i).GetPos() ), g_Tray.GetStrPos(g_Tray.GetTrayExtra(i).GetTargetPos() ) );
			GridData( IDC_CUSTOM_DATA_MDL_TRAY_EXTRA, i + 2, 1, strTrayName );
		}
	}
}
