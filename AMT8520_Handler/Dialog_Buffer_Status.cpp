// Dialog_Buffer_Status.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "Dialog_Buffer_Status.h"

#include "Dialog_KeyBoard.h"

#include "Dialog_Select.h"
#include "io.h" 

#include "GridControlAlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Buffer_Status dialog


CDialog_Buffer_Status::CDialog_Buffer_Status(CWnd* pParent /*=NULL*/)
	: StandardDialog(CDialog_Buffer_Status::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Buffer_Status)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_caption_main = "buffer / tray status";
}


void CDialog_Buffer_Status::DoDataExchange(CDataExchange* pDX)
{
	StandardDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Buffer_Status)
	DDX_Control(pDX, IDC_LABEL_TITLE, m_label_title);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Buffer_Status, StandardDialog)
	//{{AFX_MSG_MAP(CDialog_Buffer_Status)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	//}}AFX_MSG_MAP
	ON_MESSAGE(SSM_CLICK, OnCellClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Buffer_Status message handlers

BOOL CDialog_Buffer_Status::OnInitDialog() 
{
	StandardDialog::OnInitDialog();

	// ************************************************************************** 
	// 컨트롤에 설정할 폰트 생성한다                                              
	// ************************************************************************** 
// 	mp_buffer_font = NULL;
// 	mp_buffer_font = new CFont;
// 	mp_buffer_font->CreateFont(15,0,0,0,0,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Arial");
	
	On_Init_Buffer();
	OnBuffer_Label_Set();
	On_Display_Buffer();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Buffer_Status::OnDestroy() 
{
// 	delete mp_buffer_font;
// 	mp_buffer_font = NULL;

	StandardDialog::OnDestroy();	
}

void CDialog_Buffer_Status::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	StandardDialog::OnTimer(nIDEvent);
}

void CDialog_Buffer_Status::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	Grid->SetColor(col, row, bk, tk);
	Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Buffer_Status::GridControl(UINT nID, int type, int row, int col, int pos)
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

void CDialog_Buffer_Status::GridData(UINT nID, int row, int col, CString data)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->SetValue(col, row, data);

	Grid = NULL;
	delete Grid;
}

void CDialog_Buffer_Status::GridFont(UINT nID, int row, int col, int size)
{
	TSpread *Grid = (TSpread *)GetDlgItem(nID);
	HFONT font;
	CString strTemp;
	
	font = CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "돋음체");
	Grid->SetFont(col, row, font, TRUE);

	Grid = NULL;
	delete Grid;
}

void CDialog_Buffer_Status::GridMerge(UINT nID, int srow, int scol, int nrow, int ncol)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	
	Grid->AddCellSpan(scol, srow, ncol, nrow);

	Grid = NULL;
	delete Grid;
}

void CDialog_Buffer_Status::On_Init_Buffer()
{
	TSpread *Grid;
	CString str_pos;
	int bufferX;
	int row = 25;
	int col = 4;
	int i, j;
	int nWidthInPixel;
	int nWidthInPixel2;
	int nWidthInPixel3;

	switch (st_msg.n_select_buffer)
	{
	case 1:
	case 2:
		bufferX = 1;
		row = 20;
		break;

	case 3:
		bufferX = 1;
		row = 40;
		break;

	case 4:
		bufferX = 2;
		row = 90;
		break;

	case 5:
		bufferX = 1;
		row = 10;
		break;

	case 6:
	case 7:
		bufferX = 1;
		row = 30;
		break;

	case 8:
	case 9:
		bufferX = st_basic.n_tray_x;
		row = 25;
		break;
	}

	col = 4 * bufferX;
	
	Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_BUFFER);
	Grid -> SetBool(SSB_EDITMODEPERMANENT, TRUE);
	Grid -> SetBool(SSB_SHOWCOLHEADERS, FALSE);
	Grid -> SetBool(SSB_SHOWROWHEADERS, FALSE);
	Grid -> SetBool(SSB_HORZSCROLLBAR, FALSE);
	Grid -> SetBool(SSB_VERTSCROLLBAR, FALSE);
	
	Grid -> SetMaxRows(row);
	Grid -> SetMaxCols(col);

	switch (bufferX)
	{
	case 1:
		nWidthInPixel = 315;
		nWidthInPixel2 = 60;
		nWidthInPixel3 = 100;
		break;
		
	case 2:
		nWidthInPixel = 145;
		nWidthInPixel2 = 30;
		nWidthInPixel3 = 50;
		break;
		
	case 3:
		nWidthInPixel = 90;
		nWidthInPixel2 = 20;
		nWidthInPixel3 = 32;
		break;
	}

	if (st_msg.n_select_buffer == 4)
	{
		nWidthInPixel = 138;
		Grid -> SetBool(SSB_VERTSCROLLBAR, TRUE);
	}
	
	for (i = 1; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			Grid -> SetRowHeightInPixels(i, 16);
			
			if (j == 1 || j == 5 || j == 9)
			{
				Grid -> SetColWidthInPixels(j, 25);
			}
			else if (j == 2 || j == 6 || j == 10)
			{
				Grid -> SetColWidthInPixels(j, nWidthInPixel);
			}
			else if (j == 3 || j == 7 || j == 11)
			{
				Grid -> SetColWidthInPixels(j, nWidthInPixel2);
			}
			else if (j == 4 || j == 8 || j == 12)
			{
				Grid -> SetColWidthInPixels(j, nWidthInPixel3);
			}
			
			if (j == 1 || j == 5 || j == 9)
			{
				GridColor(IDC_CUSTOM_BUFFER, i, j, TEST_BC, BLACK_C);
			}
			else
			{
				GridColor(IDC_CUSTOM_BUFFER, i, j, WHITE_C, BLACK_C);
			}
			
			GridControl(IDC_CUSTOM_BUFFER, STATIC, i, j, 0);
			GridFont(IDC_CUSTOM_BUFFER, i, j, 12);
		}
	}
	
	for (j = 1; j < bufferX + 1; j++)
	{
		for (i = 1; i < row + 1; i++)
		{
			str_pos.Format("#%02d", (j - 1) * row + i);
			
			if ((j - 1) * row + i < row + 1)
			{
				GridData(IDC_CUSTOM_BUFFER, i, 1, str_pos);
			}
			else if ((j - 1) * row + i < row * 2 + 1)
			{
				GridData(IDC_CUSTOM_BUFFER, i, 5, str_pos);
			}
			else
			{
				GridData(IDC_CUSTOM_BUFFER, i, 9, str_pos);
			}
		}
	}
	
	Grid = NULL;
	delete Grid;
}

void CDialog_Buffer_Status::OnBtnApply() 
{
	On_Display_Buffer();

	if (st_handler.cwnd_main != NULL)
	{
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_TRAY_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_BUFFER_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOB_TRAY_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_PICKER_INFO_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_REJECT_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_CUSTOMER_DSP, 0);
		st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_RETEST_DSP, 0);
	}
}

void CDialog_Buffer_Status::OnBuffer_Label_Set()
{
	CString strText;

	m_label_title.SetFont(Func.mp_buffer_font);
	m_label_title.SetCenterText();
	m_label_title.SetColor(BLUE_D);
	m_label_title.SetGradientColor(SKY_L);
	m_label_title.SetTextColor(RGB(255, 255, 255));

	switch (st_msg.n_select_buffer)
	{
	case 1:
		strText = "Rear buffer status";
		break;
	case 2:
		strText = "Front buffer status";
		break;		
	case 3:
		strText = "Retest buffer status";
		break;
	case 4:
		strText = "Lob buffer status";
		break;
	case 5:
		strText = "Picker status";
		break;
	case 6:
		strText = "Reject buffer status";
		break;
	case 7:
		strText = "Costomer buffer status";
		break;
	case 8:
		strText = "Tray 1 status";
		break;
	case 9:
		strText = "Tray 2 status";
		break;
	}
	m_label_title.SetWindowText(_T(strText));
}

void CDialog_Buffer_Status::On_Display_Buffer()
{
	CString str_pos;
	CString strMDLInfor;
	int nPos;
	int row = 25;
	int col = 4;
	int i, j;

	switch (st_msg.n_select_buffer)
	{
	case 1:
	case 2:
		col = 1;
		row = 20;
		break;

	case 3:
		col = 1;
		row = 40;
		break;

	case 4:
		col = 2;
		row = 90;
		break;

	case 5:
		col = 1;
		row = 10;
		break;

	case 6:
	case 7:
		col = 1;
		row = 30;
		break;

	case 8:
	case 9:
		col = st_basic.n_tray_x;
		row = 25;
		break;
	}

	switch (st_msg.n_select_buffer)
	{
	case 1:
		nPos = BT_REAR_BUFFER;
		break;
	case 2:
		nPos = BT_FRONT_BUFFER;
		break;
	case 3:
		nPos = BT_RETEST_BUFFER;
		break;
	case 4:
		nPos = BT_LOB_BUFFER;
		break;
	case 5:
		nPos = BT_PICKER;

		for (i = 1; i <= row; i++)
		{
			st_buffer_tray[nPos].nDeviceExistence[0][0][i - 1] = st_modulemap.nDeviceRobotPicker[0][i - 1];
			st_buffer_tray[nPos].nDeviceExistence[1][0][i - 1] = st_modulemap.nDeviceRobotPicker[1][i - 1];
		}
		break;
	case 6:
		nPos = BT_REJECT_BUFFER;
		break;
	case 7:
		nPos = BT_CUSTOMER_BUFFER;
		break;
	case 8:
		nPos = BT_TRAY_1;
		break;
	case 9:
		nPos = BT_TRAY_2;
		break;
	}
	
//	int nDeviceExistence[4][3][100];


	for (i = 1; i <= row; i++)
	{
		for (j = 1; j <= col; j++)
		{
			if (st_buffer_tray[nPos].nDeviceExistence[0][j - 1][i - 1] == CTL_YES)
			{
				strMDLInfor = st_buffer_tray[nPos].cLotNumber[j - 1][i - 1];
				strMDLInfor += st_buffer_tray[nPos].cSerialNumber[j - 1][i - 1];

				GridData(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), strMDLInfor);

				if (st_buffer_tray[nPos].nDeviceExistence[1][j - 1][i - 1] == BIN_PASS)
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), BLUE_D, WHITE_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), BLUE_D, WHITE_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), BLUE_D, WHITE_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), "Pass");
				}
				else if (st_buffer_tray[nPos].nDeviceExistence[1][j - 1][i - 1] == BIN_REJECT)
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), RED_C, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), RED_C, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), RED_C, BLACK_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), "reject");
				}
				else if (st_buffer_tray[nPos].nDeviceExistence[1][j - 1][i - 1] == BIN_HTO)
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), YELLOW_C, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), YELLOW_C, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), YELLOW_C, BLACK_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), "hto");
				}
				else if (st_buffer_tray[nPos].nDeviceExistence[1][j - 1][i - 1] == BIN_NTO)
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), YELLOW_D, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), YELLOW_D, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), YELLOW_D, BLACK_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), "nto");
				}
				else if (st_buffer_tray[nPos].nDeviceExistence[1][j - 1][i - 1] == BIN_BTO)
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), "bto");
				}
				else
				{
					GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), YELLOW_L, BLACK_C);
					GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), " ");
				}
			}
			else
			{
				GridColor(IDC_CUSTOM_BUFFER, i, 2 + ((j - 1) * 4), WHITE_C, BLACK_C);
				GridColor(IDC_CUSTOM_BUFFER, i, 3 + ((j - 1) * 4), WHITE_C, BLACK_C);
				GridColor(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), WHITE_C, BLACK_C);
				GridData(IDC_CUSTOM_BUFFER, i, 4 + ((j - 1) * 4), " ");
			}
		}
	}
}

void CDialog_Buffer_Status::OnCellClick(WPARAM wParam, LPARAM lParam)
{
	int nId = -1;
	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;
	
	if (lpcc->Col == 1)				return;

	switch (wParam)
	{
	case IDC_CUSTOM_BUFFER:
		nId = 0;
		break;	
	}
	
	if (nId > -1)
	{
		OnChangeValue(nId, lpcc->Row, lpcc->Col);
	}
}

void CDialog_Buffer_Status::OnChangeValue(int nid, int nrow, int ncol)
{
	TSpread *Grid; 
	CString strTmp;
	char chvalue[100];
	CString strText;
	CString strText2;
	int n_response;
	int nPos = -1;

	CDialog_Keyboard keyboard_dlg;
	
	CString str_temp;	// 저장할 정보 임시 저장 변수 

	if (nid == 0)
	{
		Grid = (TSpread*)GetDlgItem(IDC_CUSTOM_BUFFER);
		
		Grid->GetValue(ncol, nrow, chvalue);
		strTmp.Format("%s", chvalue);
		
		st_msg.mstr_typing_msg = strTmp;  // 키보드 상자 출력 데이터 설정
		
		n_response = keyboard_dlg.DoModal();
		
		if (n_response == IDOK)
		{
			strTmp = st_msg.mstr_typing_msg;  // 키보드 상자 입력 정보 클래스 변수에 설정
			
			strTmp.MakeUpper();
			strTmp.TrimLeft(' ');               
			strTmp.TrimRight(' ');
			
			Grid->SetValue(ncol, nrow, strTmp);
			
			switch (st_msg.n_select_buffer)
			{
			case 1:
				nPos = BT_REAR_BUFFER;
				break;
			case 2:
				nPos = BT_FRONT_BUFFER;
				break;
			case 3:
				nPos = BT_RETEST_BUFFER;
				break;
			case 4:
				nPos = BT_LOB_BUFFER;
				break;
			case 5:
				nPos = BT_PICKER;
				// 				
				// 				for (i = 1; i <= row; i++)
				// 				{
				// 					st_buffer_tray[nPos].nDeviceExistence[0][0][i - 1] = st_modulemap.nDeviceRobotPicker[0][i - 1];
				// 					st_buffer_tray[nPos].nDeviceExistence[1][0][i - 1] = st_modulemap.nDeviceRobotPicker[1][i - 1];
				// 				}
				break;
			case 6:
				nPos = BT_REJECT_BUFFER;
				break;
			case 7:
				nPos = BT_CUSTOMER_BUFFER;
				break;
			case 8:
				nPos = BT_TRAY_1;
				break;
			case 9:
				nPos = BT_TRAY_2;
				break;
			}
			
			if (ncol == 2 || ncol == 6 || ncol == 10)				// 시리얼 번호 및 유무임.
			{
				if (strTmp == "")
				{
					if (st_msg.n_select_buffer != 5)
					{
						if (ncol == 2)						// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][0][nrow - 1] = CTL_NO;
						}
						else if (ncol == 6)					// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][1][nrow - 1] = CTL_NO;
						}
						else if (ncol == 10)				// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][2][nrow - 1] = CTL_NO;
						}
					}
					else
					{
						st_modulemap.nDeviceRobotPicker[0][nrow - 1] = CTL_NO;
					}
				}
				else
				{
					if (st_msg.n_select_buffer != 5)
					{
						if (ncol == 2)						// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][0][nrow - 1] = CTL_YES;
							sprintf(st_buffer_tray[nPos].cLotNumber[0][nrow - 1], strTmp);
						}
						else if (ncol == 6)					// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][1][nrow - 1] = CTL_YES;
							sprintf(st_buffer_tray[nPos].cLotNumber[1][nrow - 1], strTmp);
						}
						else if (ncol == 10)				// 시리얼 번호 및 유무임.
						{
							st_buffer_tray[nPos].nDeviceExistence[0][2][nrow - 1] = CTL_YES;
							sprintf(st_buffer_tray[nPos].cLotNumber[2][nrow - 1], strTmp);
						}
					}
					else
					{
						st_modulemap.nDeviceRobotPicker[0][nrow - 1] = CTL_YES;
					}
				}
			}
			else if (ncol == 4 || ncol == 8 || ncol == 12)				// 시리얼 번호 및 유무임.
			{
				if (st_msg.n_select_buffer != 5)
				{
					if (ncol == 4)						// 시리얼 번호 및 유무임.
					{
						st_buffer_tray[nPos].nDeviceExistence[1][0][nrow - 1] = On_Buffer_Bin_Change(strTmp);
					}
					else if (ncol == 8)					// 시리얼 번호 및 유무임.
					{
						st_buffer_tray[nPos].nDeviceExistence[1][1][nrow - 1] = On_Buffer_Bin_Change(strTmp);
					}
					else if (ncol == 12)				// 시리얼 번호 및 유무임.
					{
						st_buffer_tray[nPos].nDeviceExistence[1][2][nrow - 1] = On_Buffer_Bin_Change(strTmp);
					}
				}
				else
				{
					st_modulemap.nDeviceRobotPicker[1][nrow - 1] = On_Buffer_Bin_Change(strTmp);
				}
			}
		}
		else if (n_response == IDCANCEL)
		{
			
		}
	}
}

int CDialog_Buffer_Status::On_Buffer_Bin_Change(CString strBin)
{
	strBin.MakeUpper();
	strBin.TrimLeft(' ');
	strBin.TrimRight(' ');

	if (strBin == "")
	{
		return CTL_NO;
	}
	else if (strBin == "PASS")
	{
		return BIN_PASS;
	}
	else if (strBin == "REJECT")
	{
		return BIN_REJECT;
	}
	else if (strBin == "HTO")
	{
		return BIN_HTO;	
	}
	else if (strBin == "NTO")
	{
		return BIN_NTO;
	}
	else if (strBin == "BTO")
	{
		return BIN_BTO;
	}
	else
	{
		return CTL_NO;
	}
	
}
