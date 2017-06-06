// DialogManager.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerSocket.h"
#include "DialogManager.h"
#include ".\\Ctrl\\Variable.h"
#include ".\\Ctrl\\Alg_Mysql.h"
#include "KeyBoard.h"
#include "DialogAdd.h"
#include "io.h"			//2011.0514  파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogManager dialog


CDialogManager::CDialogManager(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogManager::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogManager)
	//}}AFX_DATA_INIT
}


void CDialogManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogManager)
	DDX_Control(pDX, IDC_COMBO_HIFIX, m_cboHifix);
	DDX_Control(pDX, IDC_MSG_HIFIX_NUM_DATA, m_msgHifixNumData);
	DDX_Control(pDX, IDC_MSG_HIFIX_NUM, m_msgHifixNum);
	DDX_Control(pDX, IDC_GROUP_SOCKET, m_groupSocket);
	DDX_Control(pDX, IDC_BTN_SOCKET_REMOVE, m_btnRemove);
	DDX_Control(pDX, IDC_BTN_SOCKET_INSERT, m_btnInsert);
	DDX_Control(pDX, IDC_BTN_DELETE_SOCKET, m_btnDel);
	DDX_Control(pDX, IDC_BTN_ADD_SOCKET, m_btnAdd);
	DDX_Control(pDX, IDC_MSG_SLOT_DATA, m_msgSlotData);
	DDX_Control(pDX, IDC_MSG_SLOT, m_msgSlot);
	DDX_Control(pDX, IDC_MSG_SERIAL_DATA, m_msgSerialData);
	DDX_Control(pDX, IDC_MSG_SERIAL, m_msgSerial);
	DDX_Control(pDX, IDC_MSG_COUNT_DATA_1, m_msgCountData);
	DDX_Control(pDX, IDC_MSG_COUNT_1, m_msgCount);
	DDX_Control(pDX, IDC_MSG_HIFIX_DATA, m_msgHifixData);
	DDX_Control(pDX, IDC_MSG_HIFIX, m_msgHifix);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogManager, CDialog)
	//{{AFX_MSG_MAP(CDialogManager)
	ON_BN_CLICKED(IDC_MSG_HIFIX_DATA, OnMsgHifixData)
	ON_BN_CLICKED(IDC_MSG_SERIAL_DATA, OnMsgSerialData)
	ON_BN_CLICKED(IDC_MSG_SLOT_DATA, OnMsgSlotData)
	ON_BN_CLICKED(IDC_MSG_COUNT_DATA_1, OnMsgCountData1)
	ON_BN_CLICKED(IDC_BTN_SOCKET_INSERT, OnBtnSocketInsert)
	ON_BN_CLICKED(IDC_BTN_SOCKET_REMOVE, OnBtnSocketRemove)
	ON_BN_CLICKED(IDC_BTN_ADD_SOCKET, OnBtnAddSocket)
	ON_BN_CLICKED(IDC_BTN_DELETE_SOCKET, OnBtnDeleteSocket)
	ON_BN_CLICKED(IDC_MSG_HIFIX_NUM_DATA, OnMsgHifixNumData)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_MESSAGE(SSM_CLICK  , OnCellClick)
	ON_BN_CLICKED(IDC_BTN_CHANGE_SOCKET, OnBtnChangeSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogManager message handlers
BOOL CDialogManager::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_pFont != NULL)
	{
		delete m_pFont;
		m_pFont = NULL;
	}	
	return CDialog::DestroyWindow();
}


BOOL CDialogManager::Create()
{
	return CDialog::Create( CDialogManager::IDD );
}

BOOL CDialogManager::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pFont	= NULL;
	m_pFont	= new CFont;
	m_pFont->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("MS Sans Serif"));

	CRect rectArea;

	m_nMode = 0;
	m_nRow = 0;

	m_nMode = 1;
	m_nHifixData = m_nHifixNum = 0;
	
	InitGridManager();
	InitGridHifix();
	InitLabel();
	InitButton();
	InitGroupBox();
	
	DisplayHifix(m_nHifixNum);
	DisplaySocket();
	
	GetWindowRect(&rectArea);
	MoveWindow(m_cpXY.x, m_cpXY.y, rectArea.Width(), rectArea.Height());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LOGFONT CDialogManager::LogFont(int nFontSize)
{
	LOGFONT lf;
	::ZeroMemory(&lf,sizeof(LOGFONT));
	lf.lfHeight = nFontSize;
	lf.lfWeight = 10;
	lf.lfWeight = 10;
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet=VIETNAMESE_CHARSET;

	return lf;
}


void CDialogManager::InitGridManager()
{
	int i, j;
	int nMaxRow, nMaxCol;

	CString strTemp;

	if (st_manager_info.nMaxRow <= 0) return;

	nMaxRow = st_manager_info.nMaxRow * 3;	
	nMaxCol = st_manager_info.nMaxCol + 1;

	m_pGridManager = (TSpread*)GetDlgItem(IDC_CUSTOM_MANAGER);

	m_pGrid.GridReset(m_pGridManager);
	// 대문자 
	m_pGrid.GridCellSelectDisplay(m_pGridManager, TRUE);
	m_pGrid.GridRowHeader(m_pGridManager, FALSE);
	m_pGrid.GridColHeader(m_pGridManager, FALSE);
	m_pGrid.GridHorizontal(m_pGridManager, FALSE);
	m_pGrid.GridVertical(m_pGridManager, FALSE);
	m_pGrid.GridAutoSize(m_pGridManager, FALSE);
	m_pGrid.GridAutoSize(m_pGridManager, FALSE);
	m_pGrid.GridCellRows(m_pGridManager, nMaxRow);
	m_pGrid.GridCellCols(m_pGridManager, nMaxCol);

	for(i=0; i<nMaxRow; i++)
	{
		m_pGrid.GridCellHeight_P(m_pGridManager, i + 1, 58);
		
		for(j=0; j<nMaxCol; j++)
		{
			m_pGrid.GridCellWidth_P(m_pGridManager, j+1, 111);

			m_pGrid.GridCellControlStatic(m_pGridManager, i + 1, j+1);
		}
	}

	for (i=0; i<(st_manager_info.nMaxSlot/st_manager_info.nMaxCol); i++)
	{
		m_pGrid.GridCellFont(m_pGridManager, (i * 3) + 1, 1, "MS Sans Serif", 20);
		m_pGrid.GridCellColor(m_pGridManager, (i * 3) + 1, 1, WHITE_C, BLACK_C);
		m_pGrid.GridCellText(m_pGridManager, (i * 3) + 1, 1, _T("Item"));

		m_pGrid.GridCellFont(m_pGridManager, (i * 3) + 2, 1, "MS Sans Serif", 20);
		m_pGrid.GridCellColor(m_pGridManager, (i * 3) + 2, 1, BLUE_D, WHITE_C);
		m_pGrid.GridCellText(m_pGridManager, (i * 3) + 2, 1, _T("Serial"));

		m_pGrid.GridCellFont(m_pGridManager, (i * 3) + 3, 1, "MS Sans Serif", 20);
		m_pGrid.GridCellColor(m_pGridManager, (i * 3) + 3, 1, BLUE_D, WHITE_C);
		m_pGrid.GridCellText(m_pGridManager, (i * 3) + 3, 1, _T("Count"));
		
		for (j=0; j<st_manager_info.nMaxCol; j++)
		{
			strTemp.Format(_T("%d"), i * st_manager_info.nMaxCol + (j + 1));
			m_pGrid.GridCellFont(m_pGridManager, i * 3 + 1, j + 2, "MS Sans Serif", 20);
			m_pGrid.GridCellColor(m_pGridManager, i * 3 + 1, j + 2, YELLOW_L, BLACK_C);
			m_pGrid.GridCellText(m_pGridManager, i * 3 + 1, j + 2, strTemp);
		}
	}

	DisplayManager();
/*
	m_pGridManager.SetFrameFocusCell(FALSE);
	m_pGridManager.SetTrackFocusCell(FALSE);
	m_pGridManager.EnableSelection(FALSE);
				   
	m_pGridManager.SetGridLineColor(BLACK_C);
	m_pGridManager.SetGridLines(1);
				   
	m_pGridManager.SetRowCount(nMaxRow);
	m_pGridManager.SetColumnCount(nMaxCol);
				   
	m_pGridManager.SetFixedRowCount(0);
	m_pGridManager.SetFixedColumnCount(0);
	m_pGridManager.SetFixedBkColor(RGB(0,0,200));
	m_pGridManager.SetFixedBkColor(RGB(200,200,255));
	m_pGridManager.SetTextBkColor(RGB(150,150,200));

	GetDlgItem(IDC_GROUP_MANAGER)->GetWindowRect(rtArea[0]); // group box rect 구하기 
	ScreenToClient(rtArea[0]); // window상에서 group box 좌표 구하기

	nWidth	= rtArea[0].Width() - 10;
	nHeight	= rtArea[0].Height() - 10;

	nX	= nWidth / nMaxCol;

	if (nMaxRow > 0)	
	{
		nY = nHeight / nMaxRow;
	}
	else	
	{
		nY = 1;
	}

	// group box에서 grid 센터에 표시하기 위한 x좌표 구하기 
	nCenX = rtArea[0].left + ((rtArea[0].Width() - (nX * nMaxCol)) / 2) - 1;
	// group box에서 grid 센터에 표시하기 위한 y좌표 구하기 
	nCenY = rtArea[0].top + ((rtArea[0].Height() - (nY * nMaxRow)) / 2) - 1;
	// grid size 계산하여 중심에 다시 그리기 
	GetDlgItem(IDC_CUSTOM_MANAGER)->MoveWindow(nCenX, nCenY, (nX * nMaxCol) + 4, (nY * nMaxRow) + 4, TRUE);

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridManager.SetRowHeight(i, nY);

		for (j=0; j<nMaxCol; j++)
		{
			m_pGridManager.SetColumnWidth(j, nX);
			m_pGridManager.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridManager.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridManager.SetItemFont(i, j, &LogFont(14));
			m_pGridManager.SetItemState(i, j, GVIS_READONLY);
		}
	}

	for (i=0; i<(st_manager_info.nMaxSlot/st_manager_info.nMaxCol); i++)
	{
		m_pGridManager.SetItemBkColour((i * 3), 0, WHITE_C, BLACK_C);
		m_pGridManager.SetItemFont((i * 3), 0, &LogFont(20));
		m_pGridManager.SetItemText((i * 3), 0, _T("Item"));

		m_pGridManager.SetItemBkColour((i * 3) + 1, 0, BLUE_D, WHITE_C);
		m_pGridManager.SetItemFont((i * 3) + 1, 0, &LogFont(20));
		m_pGridManager.SetItemText((i * 3) + 1, 0, _T("Serial"));

		m_pGridManager.SetItemBkColour((i * 3) + 2, 0, BLUE_D, WHITE_C);
		m_pGridManager.SetItemFont((i * 3) + 2, 0, &LogFont(20));
		m_pGridManager.SetItemText((i * 3) + 2, 0, _T("Count"));

		for (j=0; j<st_manager_info.nMaxCol; j++)
		{
			strTemp.Format(_T("%d"), i * st_manager_info.nMaxCol + (j + 1));
			m_pGridManager.SetItemBkColour(i * 3, j + 1, YELLOW_L, BLACK_C);
			m_pGridManager.SetItemFont(i * 3, j + 1, &LogFont(20));
			m_pGridManager.SetItemText(i * 3, j + 1, strTemp);
		}
	}

	m_pGridManager.Refresh();

	DisplayManager();*/
}


void CDialogManager::InitGridSocket(int nCount)
{
	int i, j;
	int nMaxRow, nMaxCol;
	
	CString strTemp;

	nMaxRow = nCount + 1;	
	nMaxCol = 4;

	m_pGridSocket = (TSpread*)GetDlgItem(IDC_CUSTOM_SOCKET);
	
	m_pGrid.GridReset(m_pGridSocket);
	// 대문자 
	m_pGrid.GridCellSelectDisplay(m_pGridSocket, TRUE);
	m_pGrid.GridRowHeader(m_pGridSocket, FALSE);
	m_pGrid.GridColHeader(m_pGridSocket, FALSE);
	m_pGrid.GridHorizontal(m_pGridSocket, FALSE);
	m_pGrid.GridVertical(m_pGridSocket, FALSE);
	m_pGrid.GridAutoSize(m_pGridSocket, FALSE);
	m_pGrid.GridAutoSize(m_pGridSocket, FALSE);
	m_pGrid.GridCellRows(m_pGridSocket, nMaxRow);
	m_pGrid.GridCellCols(m_pGridSocket, nMaxCol);
	
	for(i=0; i<nMaxRow; i++)
	{
		m_pGrid.GridCellHeight_P(m_pGridSocket, i + 1, 45);
		
		for(j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGrid.GridCellWidth_P(m_pGridSocket, j+1, 70);
			}
			else
			{
				m_pGrid.GridCellWidth_P(m_pGridSocket, j+1, 125);
			}
			m_pGrid.GridCellControlStatic(m_pGridSocket, i + 1, j+1);
			m_pGrid.GridCellColor(m_pGridSocket, i + 1, j+1, BLACK_L, YELLOW_C);
		}
	}


	m_pGrid.GridCellFont(m_pGridSocket, 1, 1, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSocket, 1, 1, WHITE_C, BLACK_C);
	m_pGrid.GridCellText(m_pGridSocket, 1, 1, _T("No"));
	
	m_pGrid.GridCellFont(m_pGridSocket, 1, 2, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSocket, 1, 2, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridSocket, 1, 2, _T("Serial"));
	
	m_pGrid.GridCellFont(m_pGridSocket, 1, 3, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSocket, 1, 3, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridSocket, 1, 3, _T("Count"));

	m_pGrid.GridCellFont(m_pGridSocket, 1, 4, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSocket, 1, 4, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridSocket, 1, 4, _T("Hifix"));

/*	m_pGridSocket.SetFrameFocusCell(FALSE);
	m_pGridSocket.SetTrackFocusCell(FALSE);
	m_pGridSocket.EnableSelection(FALSE);
				   
	m_pGridSocket.SetGridLineColor(BLACK_C);
	m_pGridSocket.SetGridLines(1);
				   
	m_pGridSocket.SetRowCount(nMaxRow);
	m_pGridSocket.SetColumnCount(nMaxCol);
				   
	m_pGridSocket.SetFixedRowCount(1);
	m_pGridSocket.SetFixedColumnCount(0);
	m_pGridSocket.SetFixedBkColor(RGB(0,0,200));
	m_pGridSocket.SetFixedBkColor(RGB(200,200,255));
	m_pGridSocket.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridSocket.SetRowHeight(i, 30);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridSocket.SetColumnWidth(j, 78);
			}
			else
			{
				m_pGridSocket.SetColumnWidth(j, 150);
			}
			m_pGridSocket.SetItemBkColour(i, j, BLACK_L, WHITE_C);
			m_pGridSocket.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridSocket.SetItemFont(i, j, &LogFont(14));
			m_pGridSocket.SetItemState(i, j, GVIS_READONLY);
		}
	}

	m_pGridSocket.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridSocket.SetItemFont(0, 0, &LogFont(20));
	m_pGridSocket.SetItemText(0, 0, _T("No"));

	m_pGridSocket.SetItemBkColour(0, 1, BLUE_D, WHITE_C);
	m_pGridSocket.SetItemFont(0, 1, &LogFont(20));
	m_pGridSocket.SetItemText(0, 1, _T("Serial"));

	m_pGridSocket.SetItemBkColour(0, 2, BLUE_D, WHITE_C);
	m_pGridSocket.SetItemFont(0, 2, &LogFont(20));
	m_pGridSocket.SetItemText(0, 2, _T("Count"));

	m_pGridSocket.SetItemBkColour(0, 3, BLUE_D, WHITE_C);
	m_pGridSocket.SetItemFont(0, 3, &LogFont(20));
	m_pGridSocket.SetItemText(0, 3, _T("Hifix"));

	m_pGridSocket.Refresh();*/
}


void CDialogManager::InitGridHifix()
{
	int i, j;
	int nMaxRow, nMaxCol;
	
	CString strTemp;

	m_pGridHifix = (TSpread*)GetDlgItem(IDC_CUSTOM_HIFIX);

	nMaxRow = st_manager_info.nHifixNum + 1;	
	nMaxCol = 2;

/*
	m_pGridHifix.SetFrameFocusCell(FALSE);
	m_pGridHifix.SetTrackFocusCell(FALSE);
	m_pGridHifix.EnableSelection(FALSE);
				   
	m_pGridHifix.SetGridLineColor(BLACK_C);
	m_pGridHifix.SetGridLines(1);
				   
	m_pGridHifix.SetRowCount(nMaxRow);
	m_pGridHifix.SetColumnCount(nMaxCol);
				   
	m_pGridHifix.SetFixedRowCount(1);
	m_pGridHifix.SetFixedColumnCount(0);
	m_pGridHifix.SetFixedBkColor(RGB(0,0,200));
	m_pGridHifix.SetFixedBkColor(RGB(200,200,255));
	m_pGridHifix.SetTextBkColor(RGB(150,150,200));

	for (i=0; i<nMaxRow; i++)
	{
		m_pGridHifix.SetRowHeight(i, 30);

		for (j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGridHifix.SetColumnWidth(j, 180);
				if (i > 0)
				{
					m_pGridHifix.SetItemBkColour(i, j, BLACK_L, YELLOW_C);
					strTemp.Format(_T("%d"), i);
					m_pGridHifix.SetItemText(i, j, strTemp);
					}
			}
			else
			{
				m_pGridHifix.SetColumnWidth(j, 280);
				if (i > 0)
				{
					m_pGridHifix.SetItemBkColour(i, j, YELLOW_L, BLACK_C);
					m_pGridHifix.SetItemText(i, j, st_manager_info.strHifix[i - 1]);
				}
			}
			m_pGridHifix.SetItemFormat(i, j, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			m_pGridHifix.SetItemFont(i, j, &LogFont(14));
			m_pGridHifix.SetItemState(i, j, GVIS_READONLY);
		}
	}

	m_pGridHifix.SetItemBkColour(0, 0, BLUE_D, WHITE_C);
	m_pGridHifix.SetItemFont(0, 0, &LogFont(20));
	m_pGridHifix.SetItemText(0, 0, _T("No"));

	m_pGridHifix.SetItemBkColour(0, 1, BLUE_D, WHITE_C);
	m_pGridHifix.SetItemFont(0, 1, &LogFont(20));
	m_pGridHifix.SetItemText(0, 1, _T("Hifix"));

	m_pGridHifix.Invalidate(FALSE);*/
	m_pGrid.GridReset(m_pGridHifix);
	// 대문자 
	m_pGrid.GridCellSelectDisplay(m_pGridHifix, TRUE);
	m_pGrid.GridRowHeader(m_pGridHifix, FALSE);
	m_pGrid.GridColHeader(m_pGridHifix, FALSE);
	m_pGrid.GridHorizontal(m_pGridHifix, FALSE);
	m_pGrid.GridVertical(m_pGridHifix, FALSE);
	m_pGrid.GridAutoSize(m_pGridHifix, FALSE);
	m_pGrid.GridAutoSize(m_pGridHifix, FALSE);
	m_pGrid.GridCellRows(m_pGridHifix, nMaxRow);
	m_pGrid.GridCellCols(m_pGridHifix, nMaxCol);
	
	for(i=0; i<nMaxRow; i++)
	{
		m_pGrid.GridCellHeight_P(m_pGridHifix, i + 1, 27);
		
		for(j=0; j<nMaxCol; j++)
		{
			switch(j)
			{
				case 0:
					m_pGrid.GridCellWidth_P(m_pGridHifix, j+1, 150);

					if (i > 0)
					{
						strTemp.Format(_T("%d"), i);
						m_pGrid.GridCellText(m_pGridHifix, i + 1, 1, strTemp);
						m_pGrid.GridCellColor(m_pGridHifix, i + 1, 1, BLACK_L, YELLOW_C);
					}
					break;
					
				case 1:
					m_pGrid.GridCellWidth_P(m_pGridHifix, j+1, 245);
					
					if (i > 0)
					{
						m_pGrid.GridCellText(m_pGridHifix, i + 1, 2, st_manager_info.strHifix[i - 1]);
						m_pGrid.GridCellColor(m_pGridHifix, i + 1, 2, YELLOW_L, BLACK_C);
					}
					break;
			}
			
			m_pGrid.GridCellControlStatic(m_pGridHifix, i + 1, j+1);
		}
	}
	

	m_pGrid.GridCellFont(m_pGridHifix, 1, 1, "MS Sans Serif", 16);
	m_pGrid.GridCellColor(m_pGridHifix, 1, 1, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridHifix, 1, 1, _T("No"));
	
	m_pGrid.GridCellFont(m_pGridHifix, 1, 2, "MS Sans Serif", 18);
	m_pGrid.GridCellColor(m_pGridHifix, 1, 2, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridHifix, 1, 2, _T("Hifix"));
}


void CDialogManager::InitButton()
{
/*	m_btnAdd.SetBitmaps(IDC_BTN_ADD_SOCKET, IDB_BITMAP_ADD, RGB(102, 102, 102), IDB_BITMAP_ADD, RGB(212, 208, 200));
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnAdd.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnAdd.SetFont(m_pFont);
	m_btnAdd.SetTooltipText(_T("SOCKET ADD"));

	m_btnDel.SetBitmaps(IDC_BTN_DELETE_SOCKET, IDB_BITMAP_DEL, RGB(102, 102, 102), IDB_BITMAP_ADD, RGB(212, 208, 200));
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnDel.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnDel.SetFont(m_pFont);
	m_btnDel.SetTooltipText(_T("SOCKET DEL"));

	m_btnInsert.SetBitmaps(IDC_BTN_SOCKET_INSERT, IDB_BITMAP_ADD, RGB(102, 102, 102), IDB_BITMAP_ADD, RGB(212, 208, 200));
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnInsert.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnInsert.SetFont(m_pFont);
	m_btnInsert.SetTooltipText(_T("INSERT SOCKET"));

	m_btnRemove.SetBitmaps(IDC_BTN_SOCKET_REMOVE, IDB_BITMAP_DEL, RGB(102, 102, 102), IDB_BITMAP_ADD, RGB(212, 208, 200));
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_BK_IN, WINDOW_DN);
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_BK_OUT, WINDOW_UP);
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, WINDOW_UP);
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_FG_IN, BLACK_C);
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_FG_OUT, BLUE_C);
	m_btnRemove.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, BLUE_C);
	m_btnRemove.SetFont(m_pFont);
	m_btnRemove.SetTooltipText(_T("REMOVE SOCKET"));*/
}


void CDialogManager::InitGroupBox()
{
	m_groupSocket.SetFont(LogFont(14));
	m_groupSocket.SetCatptionTextColor(BLUE_C);
	m_groupSocket.SetBorderColor(ORANGE_C);
	m_groupSocket.SetFontBold(TRUE);
}


void CDialogManager::InitLabel()
{
	CString strTemp;

	m_msgHifix.SetFont(m_pFont);
	m_msgHifix.SetWindowText(_T("Hifix Num"));
	m_msgHifix.SetCenterText();
	m_msgHifix.SetColor(WHITE_C);
	m_msgHifix.SetGradientColor(ORANGE_C);
	m_msgHifix.SetTextColor(BLACK_C);

	m_msgHifixData.SetFont(m_pFont);
	strTemp.Format(_T("%d"), m_nHifixNum);
	m_msgHifixData.SetWindowText(strTemp);
	m_msgHifixData.SetCenterText();
	m_msgHifixData.SetColor(YELLOW_L);
	m_msgHifixData.SetGradientColor(YELLOW_L);
	m_msgHifixData.SetTextColor(BLACK_C);

	m_msgSerial.SetFont(m_pFont);
	m_msgSerial.SetWindowText(_T("Serial"));
	m_msgSerial.SetCenterText();
	m_msgSerial.SetColor(WHITE_C);
	m_msgSerial.SetGradientColor(ORANGE_C);
	m_msgSerial.SetTextColor(BLACK_C);

	m_msgSerialData.SetFont(m_pFont);
	m_msgSerialData.SetWindowText(_T(""));
	m_msgSerialData.SetCenterText();
	m_msgSerialData.SetColor(YELLOW_L);
	m_msgSerialData.SetGradientColor(YELLOW_L);
	m_msgSerialData.SetTextColor(BLACK_C);

	m_msgHifixNum.SetFont(m_pFont);
	m_msgHifixNum.SetWindowText(_T("Hifix"));
	m_msgHifixNum.SetCenterText();
	m_msgHifixNum.SetColor(WHITE_C);
	m_msgHifixNum.SetGradientColor(ORANGE_C);
	m_msgHifixNum.SetTextColor(BLACK_C);

	m_msgHifixNumData.SetFont(m_pFont);
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgHifixNumData.SetCenterText();
	m_msgHifixNumData.SetColor(YELLOW_L);
	m_msgHifixNumData.SetGradientColor(YELLOW_L);
	m_msgHifixNumData.SetTextColor(BLACK_C);

	m_msgCount.SetFont(m_pFont);
	m_msgCount.SetWindowText(_T("Count"));
	m_msgCount.SetCenterText();
	m_msgCount.SetColor(WHITE_C);
	m_msgCount.SetGradientColor(ORANGE_C);
	m_msgCount.SetTextColor(BLACK_C);

	m_msgCountData.SetFont(m_pFont);
	m_msgCountData.SetWindowText(_T(""));
	m_msgCountData.SetCenterText();
	m_msgCountData.SetColor(YELLOW_L);
	m_msgCountData.SetGradientColor(YELLOW_L);
	m_msgCountData.SetTextColor(BLACK_C);

	m_msgSlot.SetFont(m_pFont);
	m_msgSlot.SetWindowText(_T("Slot"));
	m_msgSlot.SetCenterText();
	m_msgSlot.SetColor(WHITE_C);
	m_msgSlot.SetGradientColor(ORANGE_C);
	m_msgSlot.SetTextColor(BLACK_C);

	m_msgSlotData.SetFont(m_pFont);
	m_msgSlotData.SetWindowText(_T(""));
	m_msgSlotData.SetCenterText();
	m_msgSlotData.SetColor(YELLOW_L);
	m_msgSlotData.SetGradientColor(YELLOW_L);
	m_msgSlotData.SetTextColor(BLACK_C);

// 	m_msgType.SetFont(m_pFont);
// 	m_msgType.SetWindowText(_T("Type"));
// 	m_msgType.SetCenterText();
// 	m_msgType.SetColor(WHITE_C);
// 	m_msgType.SetGradientColor(ORANGE_C);
// 	m_msgType.SetTextColor(BLACK_C);
// 
// 	m_msgTypeData.SetFont(m_pFont);
// 	m_msgTypeData.SetWindowText(_T(""));
// 	m_msgTypeData.SetCenterText();
// 	m_msgTypeData.SetColor(YELLOW_L);
// 	m_msgTypeData.SetGradientColor(YELLOW_L);
// 	m_msgTypeData.SetTextColor(BLACK_C);

	//2016.0530
	m_cboHifix.SetWindowText(strTemp);

}

void CDialogManager::DisplayHifix(int nHifix)
{
	int i;
	
	for (i=0; i<st_manager_info.nHifixNum; i++)
	{
		//m_pGridHifix.SetItemBkColour(i + 1, 1, YELLOW_L, BLACK_C);
		m_pGrid.GridCellColor(m_pGridHifix, i + 2, 2, YELLOW_L, BLACK_C);
	}
	
	if (nHifix < 0) 
	{
		//m_pGridHifix.Invalidate(FALSE);
		return;
	}
	
	//m_pGridHifix.SetItemBkColour(nHifix, 1, GREEN_C, BLACK_C);
	
	//m_pGridHifix.Refresh();
	if( m_nMode == 0)
	{
		m_pGrid.GridCellColor(m_pGridHifix, nHifix + 2, 2, PINK, BLACK_C);
	}
	else//setting
	{
		m_pGrid.GridCellColor(m_pGridHifix, nHifix + 2, 2, GREEN_C, BLACK_C);
		m_nHifixNum = nHifix;
	}
}


void CDialogManager::DisplayManager()
{
/*	CString strTemp;
	
	int i, j;
	int nSlot;
	
	for (i=0; i<st_manager_info.nMaxRow; i++)
	{
		for (j=0; j<st_manager_info.nMaxCol; j++)
		{
			nSlot = (i * st_manager_info.nMaxCol) + j;
			m_pGridManager.SetItemText((i * 3) + 1, j + 1, st_socket_info[m_nHifixNum - 1][nSlot].strSerial);
			strTemp.Format(_T("%d"), st_socket_info[m_nHifixNum - 1][nSlot].nCount);
			m_pGridManager.SetItemText((i * 3) + 2, j + 1, strTemp);
		}
	}
	
	m_pGridManager.Refresh();*/
	CString strTemp;
	
	int i, j;
	int nSlot;
	
	m_pGridManager = (TSpread*)GetDlgItem(IDC_CUSTOM_MANAGER);

	for (i=0; i<st_manager_info.nMaxRow; i++)
	{
		for (j=0; j<st_manager_info.nMaxCol; j++)
		{
			nSlot = (i * st_manager_info.nMaxCol) + j;
			//m_pGridManager.SetItemText((i * 3) + 1, j + 1, st_socket_info[m_nHifixNum - 1][nSlot].strSerial);
			//m_pGrid.GridCellText(m_pGridManager, (i * 3) + 2, j + 2, st_socket_info[m_nHifixNum - 1][nSlot].strSerial);
			m_pGrid.GridCellText(m_pGridManager, (i * 3) + 2, j + 2, st_socket_info[m_nHifixNum][nSlot].strSerial);
			//strTemp.Format(_T("%d"), st_socket_info[m_nHifixNum - 1][nSlot].nCount);
			strTemp.Format(_T("%d"), st_socket_info[m_nHifixNum][nSlot].nCount);
			//m_pGridManager.SetItemText((i * 3) + 2, j + 1, strTemp);
			m_pGrid.GridCellText(m_pGridManager, (i * 3) + 3, j + 2, strTemp);
		}
	}
}


void CDialogManager::DisplaySocket()
{
	int nNum;
	int nDataCount;
	int nPos[3];
	int i, j;
	int nVal[5];
	
	CString strTable[8];
	CString strData;
	CString strTemp;
	
	nNum = Mysql.MySql_Open();
	if (nNum < 0) return;
	
	if(Mysql.Database_Select(nNum, _T("AMT")))
	{

		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			nVal[0] = -1;
			nDataCount = Mysql.Record_Int_Search(nNum, _T("SOCKET"), _T("SLOT"), _T("="), nVal);
			
			InitGridSocket(nDataCount);
			if (nDataCount > 0)
			{
				//InitGridSocket(nDataCount);
				for (i=0; i<nDataCount; i++)
				{
					strData = Mysql.Record_Read(nNum);
					
					nPos[0] =0;
					for (j=0; j<Mysql.m_nManagerTableCnt; j++)
					{
						nPos[1]		= strData.Find(_T(","), nPos[0] + 1);
						strTable[j]	= strData.Mid(nPos[0], (nPos[1] - nPos[0]));
						nPos[0]		= nPos[1] + 1;
					}
					
					strTemp.Format(_T("%d"), i + 1);
					m_pGrid.GridCellText(m_pGridSocket, i + 2, 1, strTemp);
					m_pGrid.GridCellText(m_pGridSocket, i + 2, 2, strTable[2]);
					m_pGrid.GridCellText(m_pGridSocket, i + 2, 3, strTable[4]);

					strTemp.Format(_T("%d"), atoi(strTable[3]) + 1);
					m_pGrid.GridCellText(m_pGridSocket, i + 2, 4, strTemp);
				}
			}
		}
		else InitGridSocket(0);
	}
	
	Mysql.MySql_Close(nNum);
}

void CDialogManager::OnMsgHifixData() 
{
	// TODO: Add your control notification handler code here
// 	int nKey;
// 	
// 	CString strTemp;
// 	
// 	//nKey = _wtoi(m_msgHifixNumData.GetWindowText());
// 	nKey = atoi(m_msgHifixNumData.GetWindowText());
// 	KeyPadI(0, st_manager_info.nHifixNum, &nKey);
// 	
// 	m_nHifixNum = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 
// 	DisplayManager();
// 	m_msgHifixData.SetWindowText(strTemp);
	
// 	m_msgTypeData.SetWindowText(st_manager_info.strHifix[nKey - 1]);	
}

void CDialogManager::OnMsgSerialData() 
{
	// TODO: Add your control notification handler code here
	CString strSerial = _T("");
	
	KeyBoard(&strSerial);
	
	strSerial.MakeUpper();
	m_msgSerialData.SetWindowText(strSerial);	
}

void CDialogManager::OnMsgSlotData() 
{
	// TODO: Add your control notification handler code here
	int nKey;
	
	CString strTemp;
	
	nKey = 0;
	KeyPadI(0, st_manager_info.nMaxSlot, &nKey);
	
	strTemp.Format(_T("%d"), nKey);
	m_msgSlotData.SetWindowText(strTemp);	
}

void CDialogManager::OnMsgCountData1() 
{
	// TODO: Add your control notification handler code here
	int nKey;
	
	CString strTemp;
	
	//nKey = _wtoi(m_msgCountData.GetWindowText());
	nKey = atoi(m_msgCountData.GetWindowText());
	KeyPadI(0, 1000000, &nKey);
	
	strTemp.Format(_T("%d"), nKey);
	m_msgCountData.SetWindowText(strTemp);	
}

void CDialogManager::OnBtnSocketInsert() 
{
	// TODO: Add your control notification handler code here
	int nHifix;
	int nSlot;
	int nNum;
	int nDataCount;
	int nPos[3];
	int i;
	int nCount;
	
	CString strTable[8];
	CString strData;
	CString strTemp;
	
	nHifix = m_nHifixData;//m_nHifixData - 1;
	if (nHifix < 0 || nHifix > 9) return;
	
	//nSlot = _wtoi(m_msgSlotData.GetWindowText()) - 1;
	nSlot = atoi(m_msgSlotData.GetWindowText());//atoi(m_msgSlotData.GetWindowText()) - 1;

	nSlot = nSlot-1;
	if (nSlot < 0 || nSlot > 99) return;
	
	if (st_socket_info[nHifix][nSlot].strSerial != _T(""))
	{
		AfxMessageBox(_T("Please check the socket information"));
		
		return;
	}
	
	nNum = Mysql.MySql_Open();
	if (nNum < 0) return;
	
	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			nDataCount = Mysql.Record_Text_Search(nNum, _T("SOCKET"), _T("SERIAL"), m_msgSerialData.GetWindowText());
			
			if ( nDataCount > 0)
			{
				strData = Mysql.Record_Read(nNum);
				
				nPos[0] =0;
				for (i=0; i<Mysql.m_nManagerTableCnt; i++)
				{
					nPos[1]		= strData.Find(_T(","), nPos[0] + 1);
					strTable[i]	= strData.Mid(nPos[0], (nPos[1] - nPos[0]));
					nPos[0]		= nPos[1] + 1;
				}
				
				if( (atoi(strTable[3]) + 1) != m_nHifixData + 1)
				{
					Mysql.MySql_Close(nNum);
					AfxMessageBox(_T("Please check the Hifix number information"));					
					return;	
				}
				else if (strTable[5] == _T("-1"))
				{
					strTemp = m_msgSlotData.GetWindowText();
					int nSlot = atoi(strTemp);
					if( nSlot > 0 )
					{
						strTemp.Format(_T("%d"), nSlot - 1);						
						Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SERIAL"), strTable[2], _T("HIFIX"), strTable[3], _T("SLOT"), strTemp /*m_msgSlotData.GetWindowText()*/);
						CString str_Serial;
						nCount = atoi(m_msgCountData.GetWindowText());
						str_Serial.Format(" REPLACE, HIFIX  %d, SLOT %d, %s, CNT : %d ", strTable[3], (nSlot+1), strTable[2], nCount );
						On_LogFile_Add(str_Serial);
					}
				}
				else
				{
					Mysql.MySql_Close(nNum);
					AfxMessageBox(_T("Please check the serial number information"));					
					return;
				}
			}
			else
			{
				if (KeyCheck(m_msgSerialData.GetWindowText()) == TRUE)
				{
					strTable[2]	= m_msgSerialData.GetWindowText();
					
					strTable[3].Format(_T("%d"), m_nHifixData);
					
					//nCount = _wtoi(m_msgCountData.GetWindowText());
					nCount = atoi(m_msgCountData.GetWindowText());
					if (nCount < 0) nCount = 0;
					strTable[4].Format(_T("%d"), nCount);
					strTemp = m_msgSlotData.GetWindowText();
					int nSlot = atoi(strTemp);
					nSlot = nSlot - 1;
					if( nSlot >= 0 )
					{
						strTable[5].Format(_T("%d"),nSlot);
						strTable[6].Format(_T("%s"), st_manager_info.strHifix[nHifix]);
						strTable[7]	= _T("NULL");
						Mysql.Record_Add(nNum, _T("SOCKET"), strTable);

						CString str_Serial;
						str_Serial.Format(" Insert, HIFIX  %d, SLOT %d, %s, CNT : %d ", strTable[3], (nSlot+1), strTable[2], nCount );
						On_LogFile_Add(str_Serial);
					}
					
				}
				else
				{
					Mysql.MySql_Close(nNum);
					
					AfxMessageBox(_T("Please check the serial key"));
					return;
				}
			}
		}
	}
	else
	{
		AfxMessageBox(_T("First, You have to add socket!!!"));
		return;
	}

	
	st_socket_info[nHifix][nSlot].strSerial = m_msgSerialData.GetWindowText();
	//st_socket_info[nHifix][nSlot].nCount	= _wtoi(m_msgCountData.GetWindowText());
	st_socket_info[nHifix][nSlot].nCount	= atoi(m_msgCountData.GetWindowText());
	st_socket_info[nHifix][nSlot].strType	= st_manager_info.strHifix[nHifix];
	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgCountData.SetWindowText(_T(""));
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgSlotData.SetWindowText(_T("1"));
//	m_msgTypeData.SetWindowText(_T(""));
	
// 	m_nHifixData = 0;
	DisplayHifix(m_nHifixData);
	
	Mysql.MySql_Close(nNum);
	
	DisplaySocket();
	DisplayManager();

	InitGridManager();

}

void CDialogManager::OnBtnSocketRemove() 
{
	// TODO: Add your control notification handler code here
	int nHifix;
	int nSlot;
	int nNum;
	int nDataCount;
	int nPos[3];
	int i,nCount;
	
	CString strTable[8];
	CString strData;
	CString strTemp;
	
	nHifix = m_nHifixData;//m_nHifixData - 1;
	if (nHifix < 0 || nHifix > 9) return;
	
	//nSlot = _wtoi(m_msgSlotData.GetWindowText()) - 1;
	nSlot = atoi(m_msgSlotData.GetWindowText());//atoi(m_msgSlotData.GetWindowText()) - 1;
	nSlot = nSlot -1;
	if (nSlot < 0 || nSlot > 99) return;
	
	if (st_socket_info[nHifix][nSlot].strSerial == _T(""))
	{
		AfxMessageBox(_T("Please check the socket information"));
		
		return;
	}
	
	nNum = Mysql.MySql_Open();
	if (nNum < 0) return;
	
	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			strData = m_msgSerialData.GetWindowText();
			nDataCount = Mysql.Record_Text_Search(nNum, _T("SOCKET"), _T("SERIAL"), strData);
			
			if (nDataCount > 0)
			{
				strData = Mysql.Record_Read(nNum);
				
				nPos[0] =0;
				for (i=0; i<Mysql.m_nManagerTableCnt; i++)
				{
					nPos[1]		= strData.Find(_T(","), nPos[0] + 1);
					strTable[i]	= strData.Mid(nPos[0], (nPos[1] - nPos[0]));
					nPos[0]		= nPos[1] + 1;
				}
				
				Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SERIAL"), strTable[2], _T("HIFIX"), strTable[3], _T("SLOT"), _T("-1"));

				CString str_Serial;
				nCount = atoi(m_msgCountData.GetWindowText());
				str_Serial.Format(" REMOVE, HIFIX  %d, SLOT %d, %s, CNT : %d ", strTable[3], (nSlot+1), strTable[2], nCount );
				On_LogFile_Add(str_Serial);

			}
			else
			{
				Mysql.MySql_Close(nNum);
				
				return;
			}
		}
	}
	
	st_socket_info[nHifix][nSlot].strSerial = _T("");
	st_socket_info[nHifix][nSlot].nCount	= 0;
	st_socket_info[nHifix][nSlot].strType	= _T("");
	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgCountData.SetWindowText(_T(""));
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgSlotData.SetWindowText(_T("1"));
//	m_msgTypeData.SetWindowText(_T(""));
	
// 	m_nHifixData = 0;
	DisplayHifix(m_nHifixData);
	
	Mysql.MySql_Close(nNum);
	
	DisplaySocket();
	DisplayManager();

	InitGridManager();
	
}

void CDialogManager::OnBtnAddSocket() 
{
	int nNum;
	int nHifix;
	int nCount;
	
	CString strData[8];
	
	nNum = Mysql.MySql_Open();
	
	if (nNum < 0) return;
	
	strData[2]	= m_msgSerialData.GetWindowText();
	if( strData[2] == _T("") || strData[2].GetLength() < 12 ) return;
	
	nHifix = m_nHifixData;//m_nHifixData - 1;
	if (nHifix < 0 || nHifix > st_manager_info.nHifixNum) return;
	strData[3].Format(_T("%d"), m_nHifixData);
	
	//nCount = _wtoi(m_msgCountData.GetWindowText());
	nCount = atoi(m_msgCountData.GetWindowText());
	if (nCount < 0 ) nCount = 0;
	strData[4].Format(_T("%d"), nCount);
	strData[5]	= _T("-1");
	strData[6].Format(_T("%s"), st_manager_info.strHifix[nHifix]);
	strData[7]	= _T("NULL");

	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgHifixData.SetWindowText(_T("1"));
	m_msgCountData.SetWindowText(_T("0"));

	
	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			nCount = Mysql.Record_Text_Search(nNum, _T("SOCKET"), _T("SERIAL"), strData[2]);
			if(nCount > 0)
			{
				Mysql.MySql_Close(nNum);
				
				AfxMessageBox(_T("Please check the serial duplication"));
				return;
			}
			
			if (KeyCheck(strData[2]) == TRUE)
			{
				Mysql.Record_Add(nNum, _T("SOCKET"), strData);
			}
			else
			{
				Mysql.MySql_Close(nNum);
				
				AfxMessageBox(_T("Please check the serial key"));
				return;
			}
		}
		else
		{
			// table이 없으면 새로 table 추가
			if(Mysql.Table_Create(nNum, _T("AMT"), _T("SOCKET"), Mysql.m_nManagerTableCnt, Mysql.m_strManagerTableName, Mysql.m_nManagerTableType, Mysql.m_nManagerTableNum))
			{
				Mysql.Record_Add(nNum, _T("SOCKET"), strData);
			}
		}
	}
	else
	{
		// amt database가 없으면 database 추가
		if(Mysql.Database_Create(nNum, _T("AMT")))
		{
			// table이 없으면 새로 table 추가
			if(Mysql.Table_Create(nNum, _T("AMT"), _T("SOCKET"), Mysql.m_nManagerTableCnt, Mysql.m_strManagerTableName, Mysql.m_nManagerTableType, Mysql.m_nManagerTableNum))
			{
				Mysql.Record_Add(nNum, _T("SOCKET"), strData);
			}
		}
	}
	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgCountData.SetWindowText(_T(""));
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgSlotData.SetWindowText(_T("1"));
//	m_msgTypeData.SetWindowText(_T(""));
	
	m_nHifixData = 0;
	DisplayHifix(m_nHifixData);
	
	Mysql.MySql_Close(nNum);
	
	DisplaySocket();

	InitGridManager();

	
}

void CDialogManager::OnBtnDeleteSocket() 
{
	// TODO: Add your control notification handler code here
	int nNum;
	int nHifix;
	int nCount;
	int i;

	CString strData[8];

	nNum = Mysql.MySql_Open();

	if (nNum < 0) return;

	strData[2]	= m_msgSerialData.GetWindowText();
	
	//nHifix = m_nHifixData - 1;
	nHifix = m_nHifixData;
	if (nHifix < 0 || nHifix > st_manager_info.nHifixNum) return;
	strData[3].Format(_T("%d"), m_nHifixData);

	//nCount = _wtoi(m_msgCountData.GetWindowText());
	nCount = atoi(m_msgCountData.GetWindowText());
	if (nCount < 0) nCount = 0;
	strData[4].Format(_T("%d"), nCount);
	strData[5]	= _T("-1");
	strData[6].Format(_T("%s"), st_manager_info.strHifix[nHifix]);
	strData[7]	= _T("NULL");

	m_msgSerialData.SetWindowText(_T(""));
	m_msgHifixData.SetWindowText(_T("1"));
	m_msgCountData.SetWindowText(_T("0"));

	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			Mysql.Record_Text_Delete(nNum, _T("SOCKET"), _T("SERIAL"), strData[2]);
		}
	}

	for (i=0; i<st_manager_info.nMaxSlot; i++)
	{
		if (st_socket_info[nHifix][i].strSerial == strData[2])
		{
			st_socket_info[nHifix][i].strSerial = _T("");
			st_socket_info[nHifix][i].nCount	= 0;
			st_socket_info[nHifix][i].strType	= _T("");

			break;
		}
	}

	m_msgSerialData.SetWindowText(_T(""));
	m_msgCountData.SetWindowText(_T(""));
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgSlotData.SetWindowText(_T("1"));
//	m_msgTypeData.SetWindowText(_T(""));

	m_nHifixData = 0;
	DisplayHifix(m_nHifixData);

	Mysql.MySql_Close(nNum);

	DisplaySocket();
	DisplayManager();

	InitGridManager();


}


BOOL CDialogManager::KeyCheck(CString strKey)
{
	CString strMaster[26] = {_T("D"), _T("G"), _T("A"), _T("C"), _T("P"), 
							 _T("B"), _T("I"), _T("L"), _T("N"), _T("H"), 
							 _T("T"), _T("W"), _T("Z"), _T("E"), _T("F"), 
							 _T("J"), _T("K"), _T("M"), _T("O"), _T("Q"), 
							 _T("X"), _T("Y"), _T("R"), _T("S"), _T("U"), 
							 _T("V")};

	CString strData[5];
	CString strTemp;

	int		nKey[12];
	int		nVal;
	int		i, j;
	int		nLength;
	int		nA, nB, nC, nD, nE;

	nLength = strKey.GetLength();
	if (nLength < 12 || nLength > 12) return FALSE;

	for (i=0; i<12; i++)
	{
		nVal = -1;
		strTemp = strKey.GetAt(i);

		for (j=0; j<26; j++)
		{
			if (strTemp == strMaster[j])
			{
				nVal = j + 1;
				break;
			}
		}

		if (nVal < 0)
		{
			//nVal	= _wtoi(strTemp);
			nVal	= atoi(strTemp);
			if (nVal >= 0)
			{
				//nKey[i] = _wtoi(strTemp);
				nKey[i] = atoi(strTemp);
			}
			else
			{
				return FALSE;
			}
		}
		else 
		{
			nKey[i] = nVal;
		}
	}

	nA = nKey[1] * nKey[2];
	nB = nKey[3] * nKey[4];
	nC = nKey[5] * nKey[6];
	nD = nKey[7] * nKey[8];
	nE = nA + nB + nC + nD;

	strData[0].Format(_T("%05d"), nE);

	strData[1]	= strData[0].Mid(3, 1);
	strData[2]	= strData[0].Mid(4, 1);

	nVal = ((nKey[2] * 2) % 26);
	if (nVal == 0) nVal = 25;
	else nVal -= 1;

	if (nVal < 0 || nVal > 25) return FALSE;

	strData[3].Format(_T("%s%s%s"), strData[1], strMaster[nVal], strData[2]);
	strData[4] = strKey.Mid(9, 3);

	if (strData[3] != strData[4]) return FALSE;

	return TRUE;
}


void CDialogManager::OnMsgHifixNumData() 
{
	// TODO: Add your control notification handler code here
	
}

void CDialogManager::OnCellClick(WPARAM wParam, LPARAM lParam)
{
	int nRow, nCol;

	CString strData;

	LPSS_CELLCOORD lpcc = (LPSS_CELLCOORD)lParam;

	nRow				= lpcc->Row;
	nCol				= lpcc->Col;

	if(wParam == IDC_CUSTOM_MANAGER)
	{
		//AMT8562 -->
		if (nCol < 1 || nRow > 4) return;
		
		m_pGridManager = (TSpread*)GetDlgItem(IDC_CUSTOM_MANAGER);

		switch (nRow)
		{
			case 1:
				InitGridManager();
				m_msgSerialData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 1, nCol));
				m_msgCountData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 2, nCol));
				//strTemp.Format(_T("%d"), m_nHifixNum);
				//m_msgHifixNumData.SetWindowText(strTemp);
// 				m_nHifixData = m_nHifixNum;
// 				DisplayHifix(m_nHifixNum);
				strData.Format("%d", nCol - 2 + 1);
				m_msgSlotData.SetWindowText(strData);
//				m_msgTypeData.SetWindowText(st_manager_info.strHifix[m_nHifixNum - 1]);
				GridColor( IDC_CUSTOM_MANAGER, nRow, nCol, GREEN_L, BLACK_C );
				break;
				
			case 4:
				InitGridManager();
				m_msgSerialData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 1, nCol));
				m_msgCountData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 2, nCol));
				//strTemp.Format(_T("%d"), m_nHifixNum);
				//m_msgHifixNumData.SetWindowText(strTemp);
// 				m_nHifixData = m_nHifixNum;
// 				DisplayHifix(m_nHifixNum);
				strData.Format("%d", (nCol - 2) + 10 + 1);
				m_msgSlotData.SetWindowText(strData);
//				m_msgTypeData.SetWindowText(st_manager_info.strHifix[m_nHifixNum - 1]);
				GridColor( IDC_CUSTOM_MANAGER, nRow, nCol, GREEN_L, BLACK_C );
				break;
			
			////////////////////////////////////////////////////////////////////////////////////////////////
			//AMT8562 -->
			case 7:
				InitGridManager();
				m_msgSerialData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 1, nCol));
				m_msgCountData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 2, nCol));
				//strTemp.Format(_T("%d"), m_nHifixNum);
				//m_msgHifixNumData.SetWindowText(strTemp);
// 				m_nHifixData = m_nHifixNum;
// 				DisplayHifix(m_nHifixNum);
				strData.Format("%d", nCol - 2 + 1);
				m_msgSlotData.SetWindowText(strData);
//				m_msgTypeData.SetWindowText(st_manager_info.strHifix[m_nHifixNum - 1]);
				GridColor( IDC_CUSTOM_MANAGER, nRow, nCol, GREEN_L, BLACK_C );
				break;
				
			case 10:
				InitGridManager();
				m_msgSerialData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 1, nCol));
				m_msgCountData.SetWindowText(m_pGrid.GridCellText(m_pGridManager, nRow + 2, nCol));
				//strTemp.Format(_T("%d"), m_nHifixNum);
				//m_msgHifixNumData.SetWindowText(strTemp);
// 				m_nHifixData = m_nHifixNum;
// 				DisplayHifix(m_nHifixNum);
				strData.Format("%d", nCol - 2 + 1);
				m_msgSlotData.SetWindowText(strData);
//				m_msgTypeData.SetWindowText(st_manager_info.strHifix[m_nHifixNum - 1]);
				GridColor( IDC_CUSTOM_MANAGER, nRow, nCol, GREEN_L, BLACK_C );
				break;
			////////////////////////////////////////////////////////////////////////////////////////////////
			//AMT8562 <--
		}
	}
	else if(wParam == IDC_CUSTOM_HIFIX)
	{
		if (nRow <= 1) return;

		m_nMode = 0;
		m_nHifixData = nRow - 2;
		DisplayHifix(m_nHifixData);

	}
	else if(wParam == IDC_CUSTOM_SOCKET)
	{
		if (nRow <= 1) return;

		m_pGridSocket = (TSpread*)GetDlgItem(IDC_CUSTOM_SOCKET);

		strData = m_pGrid.GridCellText(m_pGridSocket, nRow, 2);
		m_msgSerialData.SetWindowText(strData);

		strData = m_pGrid.GridCellText(m_pGridSocket, nRow, 3);
		m_msgCountData.SetWindowText(strData);
	/*
		strData = m_pGridSocket.GetItemText(nRow, 3);
		m_msgHifixNumData.SetWindowText(strData);

//		m_msgTypeData.SetWindowText(st_manager_info.strHifix[_wtoi(strData) - 1]);*/
		strData = m_pGrid.GridCellText(m_pGridSocket, nRow, 4);
		int nHix = atoi(strData);
		if( nHix < 1) return;
// 		m_nHifixData = nHix -1;
// 		DisplayHifix(m_nHifixData);

		m_nRow = nRow;
		GridColor( IDC_CUSTOM_SOCKET, m_nRow, 2, GREEN_L, BLACK_C );
	}
}

// void CDialogManager::OnSelchangeComboHifix() 
// {
// 	int nKey;
// 	
// 	CString strTemp;
// 	
// 	//nKey = _wtoi(m_msgHifixNumData.GetWindowText());
// 	m_cboHifix.GetWindowText(strTemp);
// 	nKey = atoi(strTemp);
// 	if( nKey <= 0) return;
// 	// 	KeyPadI(1, st_manager_info.nHifixNum, &nKey);
// 	// 	
// 	// 	strTemp.Format(_T("%d"), nKey);
// 	//strTemp = m_cboHifix.GetWindowText();
// 	
// // 	m_msgTypeData.SetWindowText(st_manager_info.strHifix[nKey - 1]);	
// 	
// 	//m_cboHifix.SetWindowText(strTemp);	
// }

// void CDialogManager::OnEditupdateComboHifix() 
// {
// 	int nKey;
// 	
// 	CString strTemp;
// 	
// 	//nKey = _wtoi(m_msgHifixNumData.GetWindowText());
// 	m_cboHifix.GetWindowText(strTemp);
// 	nKey = atoi(strTemp);
// 	if( nKey <= 0) return;	
// }
// 
// void CDialogManager::OnEditchangeComboHifix() 
// {
// 	int nKey;
// 	
// 	CString strTemp;
// 	
// 	//nKey = _wtoi(m_msgHifixNumData.GetWindowText());
// 	nKey = atoi(m_msgHifixNumData.GetWindowText());
// 	KeyPadI(1, st_manager_info.nHifixNum, &nKey);
// 	
// 	m_nHifixNum = nKey;
// 	strTemp.Format(_T("%d"), nKey);
// 	
// 	DisplayManager();
// 	m_msgHifixData.SetWindowText(strTemp);	
// 
// 	//nKey = _wtoi(m_msgHifixNumData.GetWindowText());
// 	m_cboHifix.GetWindowText(strTemp);
// 	nKey = atoi(strTemp);
// 	if( nKey <= 0) return;
// 
// }

void CDialogManager::OnBtnApply() 
{
	CString str;
// 	m_cboHifix.GetWindowText(str);
// 
// 	int nHifix = atoi(str);
// 
// 	if( nHifix <=0 || nHifix > 5) 
// 	{
// 		AfxMessageBox(_T("Please check the HiFix data information"));
// 		return;
// 	}
// 
// 
// 	m_nHifixNum = nHifix-1;


	m_nMode = 1;
	if( m_nHifixData <0 || m_nHifixData >= 5) 
	{
		AfxMessageBox(_T("Please check the HiFix data information"));
		return;
	}

	str.Format(_T("%d"), m_nHifixData+1);

	DisplayHifix(m_nHifixData);
	DisplayManager();
	m_msgHifixData.SetWindowText(str);

	
}

void CDialogManager::GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk)
{
	TSpread *Grid = (TSpread*)GetDlgItem(nID);
	Grid->SetColor(col, row, bk, tk);
	Grid->SetBackColorStyle(SS_BACKCOLORSTYLE_UNDERGRID);
	
	Grid = NULL;
	delete Grid;
}

void CDialogManager::OnBtnChangeSocket() 
{
	// TODO: Add your control notification handler code here
	int nNum;
	int nHifix;
	int nCount;
	
	CString strData[8];
	
	nNum = Mysql.MySql_Open();
	
	if (nNum < 0) return;
	Mysql.Table_Delete(nNum, _T("SOCKET"));
	return;
	
	
	strData[2]	= m_msgSerialData.GetWindowText();
	
	nHifix = m_nHifixData + 1;//m_nHifixData - 1;
	if (nHifix < 0 || nHifix > st_manager_info.nHifixNum) return;
	strData[3].Format(_T("%d"), m_nHifixData);
	
	//nCount = _wtoi(m_msgCountData.GetWindowText());
	nCount = atoi(m_msgCountData.GetWindowText());
	if (nCount < 0) nCount = 0;
	strData[4].Format(_T("%d"), nCount);
	strData[5]	= _T("-1");
	strData[6].Format(_T("%s"), st_manager_info.strHifix[nHifix]);
	strData[7]	= _T("NULL");
	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgHifixData.SetWindowText(_T("1"));
	m_msgCountData.SetWindowText(_T("0"));
	
	
	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET")))
		{
			nCount = Mysql.Record_Text_Search(nNum, _T("SOCKET"), _T("SERIAL"), strData[2]);
			
			if (nCount > 0)
			{
				Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SERIAL"), strData[2], _T(""), _T(""), _T("HIFIX"), strData[3]);
				Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SERIAL"), strData[2], _T(""), _T(""), _T("COUNT"), strData[4]);
				Mysql.Record_Updata(nNum, _T("SOCKET"), _T("SERIAL"), strData[2], _T(""), _T(""), _T("TYPE"), strData[6]);
			}
			else
			{
				Mysql.MySql_Close(nNum);
				
				AfxMessageBox(_T("Please check the serial key"));
				return;
			}
		}
	}
	
	m_msgSerialData.SetWindowText(_T(""));
	m_msgCountData.SetWindowText(_T(""));
	m_msgHifixNumData.SetWindowText(_T(""));
	m_msgSlotData.SetWindowText(_T("1"));

// 	m_nHifixData = 0;
	DisplayHifix(m_nHifixData);
	
	Mysql.MySql_Close(nNum);
	
	DisplaySocket();
	
	InitGridManager();
}


BOOL CDialogManager::CreateFolder(LPCTSTR szPath)
{
	int nRet = 0;
	CString strTemp;
	CString strPath = szPath;
	
    do{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            strTemp = strPath.Left(nRet+1);
			
            if (_access(strTemp.operator LPCTSTR(), 00) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return nRet;
}

int CDialogManager::FileSizeCheck(char * cp_filename, int n_size, int n_check)
{
	CFileFind finder;
	long nLen=0, nflag=0;
	BOOL bContinue;
	
	if(bContinue = finder.FindFile(cp_filename))
	{	
		if (n_check == 1)			// Size를 체크하는것이면...
		{
			finder.FindFile(cp_filename);
			finder.FindNextFile();
			finder.GetFileName();
			nLen = finder.GetLength();
			if(nLen > n_size)
			{
				nflag = 1;
			}
			finder.Close();	
		}
		else						// Size를 확인하는것이면...
		{
			finder.FindFile(cp_filename);
			finder.FindNextFile();
			finder.GetFileName();
			nLen = finder.GetLength();
			
			nflag = nLen;
			
			finder.Close();	
		}
	}
	
	return nflag;
}

void CDialogManager::On_LogFile_Add( CString str_msg )
{
	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour, mn_cur_min, mn_cur_sec; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;		// 한 라인에 출력할 문자열 정보 저장 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 
	CTime m_time_current;		// 간략한 헝식의 현재 시간 정보 저장 변수
	int mn_existence;			// 파일 존재 유무 설정 플래그 
	char fileName[256];			// 검색할 파일 정보 설정 함수 
	FILE  *fp ;					// 파일에 대한 포인터 설정 변수 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	CString mstr_folder, mstr_file;
	int Ret;

	CString sTime;
	
	SYSTEMTIME csTime;
	
	GetLocalTime(&csTime);
	
	csTime.wYear;
	csTime.wMonth;
	csTime.wDay;
	csTime.wHour;
	csTime.wMinute;
	csTime.wSecond;
	csTime.wMilliseconds;
	
	sTime.Format("[%04d/%02d/%02d %02d:%02d:%02d.%03d] ", csTime.wYear, csTime.wMonth, csTime.wDay, csTime.wHour, csTime.wMinute, csTime.wSecond, csTime.wMilliseconds);

	if (str_msg.IsEmpty() == TRUE)  
	{
		return;
	}

	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 

	m_time_current = CTime::GetCurrentTime();  // 간략한 형식의 현재 시간 정보 얻는다. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();

	mn_cur_min = time_cur.GetMinute();
	mn_cur_sec = time_cur.GetSecond();
	// **************************************************************************

	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
	mstr_cur_year.Format("%04d", mn_cur_year);
	mstr_cur_month.Format("%02d", mn_cur_month);
	mstr_cur_day.Format("%02d", mn_cur_day);

	// **************************************************************************
	// 현재 시간 정보 얻는다                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();	// 현재 시간 정보를 설정한다. 
	mstr_cur_hour.Format("%d",time_cur.GetHour());	// 현재 시간 정보를 설정한다.
	mstr_cur_min.Format("%d",time_cur.GetMinute());	// 현재 분 정보를 설정한다. 
	mstr_cur_sec.Format("%d",time_cur.GetSecond());	// 현재 초 정보를 설정한다. 
	str_display_time = m_time_current.Format("%c");  // 리스트 파일에 출력할 타이틀 시간 정보 설정 
	// **************************************************************************

	mstr_file.Format("%d.txt",time_cur.GetHour());

	CString strLogPath  = _T("C:\\AMT8520\\Log\\");
	CString mstr_sokser = _T(strLogPath + "SocketSerial\\");				// Total Log 파일에 대한 폴더 설정 
	CreateFolder(mstr_sokser);

	mstr_file_name = "SOKSER" + mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
	mstr_create_file = mstr_sokser + mstr_file_name;

	mstr_create_file += ".TXT";

	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName, 0);
	
	if (mn_existence == -1)
	{
		mstr_file_name = "SOKSER" + mstr_cur_year;
		mstr_file_name += mstr_cur_month; 
		mstr_file_name += mstr_cur_day; 
		mstr_create_file = mstr_sokser + mstr_file_name;
		mstr_create_file += ".TXT";
	}

	char chCreateFile[50];
	sprintf(chCreateFile,"%s",mstr_create_file);

	Ret = FileSizeCheck(chCreateFile, 1000000, 1); //size and rename
	
	if (Ret == 1) //file size over
	{
		CString strTmp = mstr_create_file.Left( mstr_create_file.GetLength() - 4 );
		BackupName.Format("%s.%02d%02d%02d.bak",strTmp, mn_cur_hour, mn_cur_min, mn_cur_sec );
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + ".bak" ;
		rename(mstr_create_file, BackupName);
	}

	// **************************************************************************
	// 알람 발생 횟수 정보 저장 파일에 추가 가능한 형태 파일로 생성              
	// **************************************************************************
	if ((fp = fopen(mstr_create_file,"a+")) == NULL)  
	{
		CString str;
		str.Format("File Open Failue .. [%s]", mstr_create_file); 
		AfxMessageBox(str);
		return;
	}
	// **************************************************************************

	// **************************************************************************
	// 로그 파일에 현재 발생한 알람 정보 저장한다                                
	// **************************************************************************
	mstr_content.Format("%s : %s",sTime,str_msg);

	fprintf(fp,"%s\n",mstr_content);

	if (ferror(fp))  
	{
		CString str;
		str = _T("File save failure.");
		AfxMessageBox(str);		
		fclose(fp);
		return ;
	}
	// **************************************************************************
	fclose(fp);  // 파일을 종료한다.
}