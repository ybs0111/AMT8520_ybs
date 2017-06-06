// Dialog_Data_Cok.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Data_Cok.h"
#include "SrcPart/PartFunction.h"
#include "SrcPart/APartHandler_COK.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_Cok dialog


CDialog_Data_Cok::CDialog_Data_Cok(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Data_Cok::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Data_Cok)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_caption_main = "DATA (COK)"; 
}


void CDialog_Data_Cok::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Data_Cok)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Data_Cok, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_Data_Cok)
	ON_BN_CLICKED(IDC_BTN_SET, OnBtnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_Cok message handlers

void CDialog_Data_Cok::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
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

void CDialog_Data_Cok::GridControl(UINT nID, int type, int row, int col, int pos)
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
			Grid->SetTypeNumberEx(&CellType,0, 0, 0, m_nNumMax,0,".",",",0,0,1,0,1.001);
			break;
	}
	
	Grid->SetCellType(col,row,&CellType);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Cok::GridData(UINT nID, int row, int col, CString data)
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

CString CDialog_Data_Cok::GetGridData( UINT nID, int row, int col )
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	char szBuf[1024];
	Grid->GetValue(col, row, szBuf);

	Grid = NULL;
	delete Grid;

	return szBuf;
}


void CDialog_Data_Cok::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Cok::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

BOOL CDialog_Data_Cok::OnInitDialog() 
{
	StandardDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	OnInitTestsite();
	OnInitRobot();
	OnInitCokStacker();

	OnDisplayTestsite();
	OnDisplayRobot();
	OnDisplayStacker();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Data_Cok::OnInitTestsite()
{
	TSpread* Grid;
	int row = 12;
	int col = 4;
	m_nNumMax = 6;

	int iGrid = IDC_CUSTOM_DATA_COK_TSITE;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 40 );
	Grid -> SetColWidthInPixels(2, 120 );

	Grid -> SetColWidthInPixels(3, 40 );
	Grid -> SetColWidthInPixels(4, 120 );
	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 20);
			
		for( int iw=0; iw<col; iw++ )
		{
			GridFont(iGrid, ih + 1, iw + 1, 15);
			GridColor( iGrid, ih + 1, iw + 1, WHITE_C, BLACK_C );

			if( (iw == 1 || iw == 3) && ih > 1 )
			{
				GridControl(iGrid, NUMBER, ih + 1, iw + 1, 0);
			}
			else
			{
				GridControl(iGrid, STATIC, ih + 1, iw + 1, 0);
			}
		}

		if( ih > 1 )
		{
			CString strNo;
			strNo.Format( "%d", ih - 1 );
			GridData( iGrid, ih + 1, 1, strNo );
			GridData( iGrid, ih + 1, 3, strNo );
		}
		
	}

	GridMerge( iGrid, 1, 1, 1, 2 );	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C ); GridData( iGrid, 1, 1, "TEST SITE 1" );
	GridMerge( iGrid, 1, 3, 1, 2 ); GridColor( iGrid, 1, 3, BLACK_C, WHITE_C );	GridData( iGrid, 1, 3, "TEST SITE 2" );

	GridData( iGrid, 2, 2, "COK TYPE" );
	GridData( iGrid, 2, 4, "COK TYPE" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Cok::OnInitRobot()
{
	TSpread* Grid;
	int row = 2;
	int col = 1;
	
	m_nNumMax = 6;

	int iGrid = IDC_CUSTOM_DATA_COK_ROBOT;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 140 );

	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 20);

		GridFont(iGrid, ih + 1, 1, 15);
		GridColor( iGrid, ih + 1, 1, WHITE_C, BLACK_C );
	}

	GridControl(iGrid, STATIC, 1, 1, 0);
	GridControl(iGrid, NUMBER, 2, 1, 0);

	GridData( iGrid, 1, 1, "ROBOT COK" );
	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Cok::OnInitCokStacker()
{
	TSpread* Grid;
	int row = 7;
	int col = 2;

	m_nNumMax = 20;

	int iGrid = IDC_CUSTOM_DATA_COK_STACKER;

	Grid = (TSpread*)GetDlgItem( iGrid );
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	Grid -> SetColWidthInPixels(1, 20 );
	Grid -> SetColWidthInPixels(2, 118 );
	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 20);

		for( int iw=0; iw<col; iw++ )
		{
			GridFont(iGrid, ih + 1, iw + 1, 15);
			
			if( ih == 0 || iw == 0)
			{
				GridControl(iGrid, STATIC, ih + 1, iw + 1, 0);
				GridColor( iGrid, ih + 1, iw + 1, WHITE_C, BLACK_C );
			}
			else
			{
				GridControl(iGrid, NUMBER, ih + 1, iw + 1, 0);
				GridColor( iGrid, ih + 1, iw + 1, GetCokCol(ih - 1), BLACK_C );
			}
		}
		
	}

	GridMerge( iGrid, 1, 1, 1, 2 );
	GridData( iGrid, 1, 1, "COK STACKER ( CNT )" );
	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C );

	GridData( iGrid, 2, 1, "1" );
	GridData( iGrid, 3, 1, "2" );
	GridData( iGrid, 4, 1, "3" );
	GridData( iGrid, 5, 1, "4" );
	GridData( iGrid, 6, 1, "5" );
	GridData( iGrid, 7, 1, "6" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_Cok::OnDisplayTestsite()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		int nCokType = g_handler_cok.GetSiteCok(i);
		CString strCokType;
		strCokType.Format( "%d", nCokType + 1 );
		GridData( IDC_CUSTOM_DATA_COK_TSITE, i % 10 + 3, i / 10 * 2 + 2, strCokType );
		GridColor( IDC_CUSTOM_DATA_COK_TSITE, i % 10 + 3, i / 10 * 2 + 2, GetCokCol( nCokType ), BLACK_C );
	}
}

void CDialog_Data_Cok::OnDisplayRobot()
{
	int nCokID = Run_Robot.GetCokID();
	CString strCnt;
	strCnt.Format( "%d", nCokID + 1 );
	GridData( IDC_CUSTOM_DATA_COK_ROBOT, 2, 1, strCnt );
	GridColor( IDC_CUSTOM_DATA_COK_ROBOT, 2, 1, GetCokCol(nCokID), BLACK_C );
}

void CDialog_Data_Cok::OnDisplayStacker()
{
	for( int i=0; i<COK_TYPE_CNT; i++ )
	{
		int nCnt = g_handler_cok.GetCokCnt(i);
		CString strCnt;
		strCnt.Format("%d", nCnt );
		GridData( IDC_CUSTOM_DATA_COK_STACKER, i + 2, 2, strCnt );
	}
}

void CDialog_Data_Cok::OnBtnSet() 
{
	// TSITE
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		CString strCokType = GetGridData( IDC_CUSTOM_DATA_COK_TSITE, i % 10 + 3, i / 10 * 2 + 2 );
		int nCokType = atoi( strCokType );
		g_handler_cok.SetSiteCok( i, nCokType - 1 );
	}

	CString strRobotCok = GetGridData( IDC_CUSTOM_DATA_COK_ROBOT, 2, 1 );
	int nRobotCok = atoi( strRobotCok );
	Run_Robot.SetCokID( nRobotCok - 1);

	for( i=0; i<COK_TYPE_CNT; i++ )
	{
		CString strCnt = GetGridData( IDC_CUSTOM_DATA_COK_STACKER, i + 2, 2 );
		int nCnt = atoi( strCnt );

		g_handler_cok.SetCokCnt( i, nCnt );
	}

	OnDisplayTestsite();
	OnDisplayRobot();
	OnDisplayStacker();

	if( st_handler.cwnd_main )
	{
		for( i=0; i<TSITE_COK_CNT; i++ )
			st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_SITE, i );


		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 0 );
		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 1 );
		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 2 );
		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 3 );
		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 4 );
		st_handler.cwnd_main->SendMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, 5 );
	}
}
