// Dialog_Data_TestSite.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Data_TestSite.h"
#include "Variable.h"
#include "SrcPart/APartTestSite.h"
#include "AClient_BPC.h"
#include "ComizoaPublic.h"
#include "Dialog_Socket_Comment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_TestSite dialog


CDialog_Data_TestSite::CDialog_Data_TestSite(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Data_TestSite::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Data_TestSite)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_caption_main = "CONTROL (TEST SITE)";
}


void CDialog_Data_TestSite::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Data_TestSite)
	DDX_Control(pDX, IDC_CHK_UPDATE_BY_BPC, m_chk_update_by_bpc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Data_TestSite, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_Data_TestSite)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_QUERY, OnBtnQuery)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)
	ON_MESSAGE( WM_DRAW_DATA_TESTSITE, OnDraw_Data_TestSite)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_TestSite message handlers

BOOL CDialog_Data_TestSite::OnInitDialog() 
{
	StandardDialog::OnInitDialog();

	st_handler.cwnd_data_testsite = this;
	
	// TODO: Add extra initialization here
	m_chk_update_by_bpc.SetCheck( false );

	OnInit_TestSite();
	OnDisplay_TestSite();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Data_TestSite::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
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

void CDialog_Data_TestSite::GridControl(UINT nID, int type, int row, int col, int pos)
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

void CDialog_Data_TestSite::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);

	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_TestSite::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_TestSite::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_TestSite::OnInit_TestSite()
{
	TSpread* Grid;
	int row = 12;
	int col = 6;

	int iGrid = IDC_CUSTOM_TESTSITE;

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
	Grid -> SetColWidthInPixels(3, 120 );

	Grid -> SetColWidthInPixels(4, 40 );
	Grid -> SetColWidthInPixels(5, 120 );
	Grid -> SetColWidthInPixels(6, 120 );
	for( int ih=0; ih<row; ih++ )
	{
		Grid -> SetRowHeightInPixels(ih + 1, 40);
			
		for( int iw=0; iw<col; iw++ )
		{
			GridFont(iGrid, ih + 1, iw + 1, 15);
			GridControl(iGrid, STATIC, ih + 1, iw + 1, 0);
			GridColor( iGrid, ih + 1, 1, WHITE_C, BLACK_C );
		}

		if( ih > 1 )
		{
			CString strNo;
			strNo.Format( "%d", ih - 1 );
			GridData( iGrid, ih + 1, 1, strNo );
			GridData( iGrid, ih + 1, 4, strNo );
		}
		
	}

	GridMerge( iGrid, 1, 1, 1, 3 );	GridColor( iGrid, 1, 1, BLACK_C, WHITE_C ); GridData( iGrid, 1, 1, "TEST SITE 1" );
	GridMerge( iGrid, 1, 4, 1, 3 ); GridColor( iGrid, 1, 4, BLACK_C, WHITE_C );	GridData( iGrid, 1, 4, "TEST SITE 2" );

	GridData( iGrid, 2, 2, "USE" );
	GridData( iGrid, 2, 3, "BOARD" );
	GridData( iGrid, 2, 5, "USE" );
	GridData( iGrid, 2, 6, "BOARD" );

	Grid = NULL;
	delete Grid;
}

void CDialog_Data_TestSite::OnDisplay_TestSite()
{
	int iGrid = IDC_CUSTOM_TESTSITE;
	for( int i=0; i<TSITE_SOCKET_CNT; i++ )
	{
		if( g_site.GetEnableSocket(i) > 0 /*g_site.GetEnableSocket(i)*/ )//2019.0709
		{
			GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), BLUE_L, BLACK_C );
			GridData( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), "ENABLE" );
		}
		else if( g_site.GetEnableSocket(i) < 0 )//2019.0709 MAX_XOUNT
		{
			if( g_site.GetEnableSocket(i) == -1 )
				GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), BLACK_C, BLUE_L );
			else if( g_site.GetEnableSocket(i) == -1 )
				GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), BLACK_L, BLUE_L );
			else if( g_site.GetEnableSocket(i) == -1 )
				GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), DSERBLE_BC, BLUE_L );
			else
				GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), ORANGE_L, BLUE_L );
			GridData( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), "DISABLE" );
		}
		else
		{
			GridColor( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), BLACK_C, BLUE_L );
			GridData( iGrid, 3 + (i % 10), 2 + (3 * (i / 10)), "DISABLE" );
		}

		if( g_site.GetBoardOn(i) )
		{
			GridColor( iGrid, 3 + (i % 10), 3 + (3 * (i / 10)), YELLOW_L, BLACK_C );
			GridData( iGrid, 3 + (i % 10), 3 + (3 * (i / 10)), "ON" );
		}
		else
		{
			GridColor( iGrid, 3 + (i % 10), 3 + (3 * (i / 10)), RED_D, BLUE_L );
			GridData( iGrid, 3 + (i % 10), 3 + (3 * (i / 10)), "OFF" );
		}
	}
}

void CDialog_Data_TestSite::OnCellClick( WPARAM wParam, LPARAM lParam )
{
	if( COMI.mn_run_status != dSTOP )
		return;

	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	bool bUse = false;

	if( wParam != IDC_CUSTOM_TESTSITE )
		return;

	if( lpcc->Col == 2 )
	{
		if( lpcc->Row > 2 )
		{
			if( g_site.GetEnableSocket( lpcc->Row - 3 ) <= 0 ) bUse = false;
			else                                               bUse = true;
			//OnUse( lpcc->Row - 3, !g_site.GetEnableSocket( lpcc->Row - 3 ) );//2016.0709
			OnUse( lpcc->Row - 3, !bUse );
		}
	}
	else if( lpcc->Col == 3 )
	{
		if( lpcc->Row > 2 )
		{
			OnBoardOn( lpcc->Row - 3, !g_site.GetBoardOn( lpcc->Row - 3 ) );
		}
	}
	else if( lpcc->Col == 5 )
	{
		if( lpcc->Row > 2 )
		{
			if( g_site.GetEnableSocket( 10 + lpcc->Row - 3) <= 0 ) bUse = false;
			else                                               bUse = true;
			//OnUse( 10 + lpcc->Row - 3, !g_site.GetEnableSocket( 10 + lpcc->Row - 3 ) );//2016.0709
			OnUse( 10 + lpcc->Row - 3, !bUse );
		}
	}
	else if( lpcc->Col == 6 )
	{
		if( lpcc->Row > 2 )
		{
			OnBoardOn( 10 + lpcc->Row - 3, !g_site.GetBoardOn( 10 + lpcc->Row - 3 ) );
		}
	}
}

LRESULT CDialog_Data_TestSite::OnDraw_Data_TestSite( WPARAM wParam,LPARAM lParam )
{
	OnDisplay_TestSite();

	return 0;
}

void CDialog_Data_TestSite::OnUse( int iIdx, bool bVal )//2016.0709
{
	CString str_log;
	if( m_chk_update_by_bpc.GetCheck() )
	{
		ENUM_USE use = EUSE_DISABLE;
		if( bVal )	use = EUSE_ENABLE;
		g_client_bpc.SetSlotStatusChange( iIdx, use );			 


	}
	else
	{
		g_site.SetEnableSocket( iIdx, bVal );
		//2015.1005
		g_client_bpc.SetSocketOnOff( iIdx );
		//kwlee 2015.1008 log 추가
		if (bVal == true)
		{
			str_log.Format("Manual Socket Use : %d : On",iIdx);
			Func.On_LogFile_Add(99, str_log);
		}
		else
		{
			str_log.Format("Manual Socket Use : %d : OFF",iIdx);
			Func.On_LogFile_Add(99, str_log);
		}
		
		
		OnDisplay_TestSite();
	}
	CDialog_Socket_Comment dlg(iIdx);
	dlg.DoModal();
}

void CDialog_Data_TestSite::OnBoardOn( int iIdx, bool bVal )
{
	if( m_chk_update_by_bpc.GetCheck() )
	{

	}
	else
	{
		g_site.SetBoardOn( iIdx, bVal );
		OnDisplay_TestSite();
	}
}


void CDialog_Data_TestSite::OnDestroy() 
{
	StandardDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	for ( int i = 0; i < 20; i++)
	{
		st_handler.m_bBackupEnable2[st_basic.mn_hotfix_pos -1][i] = g_site.GetEnableSocket(i);
	}

	st_handler.cwnd_data_testsite = NULL;
}

void CDialog_Data_TestSite::OnBtnQuery() 
{
	if( COMI.mn_run_status != dSTOP )
		return;

	g_client_bpc.SendSlotQuery();
}
