// MgrSerialSocketDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MgrSerialSocket.h"
#include "MgrSerialSocketDlg.h"
#include ".\\Ctrl\\Variable.h"
#include ".\\Ctrl\\Alg_Mysql.h"
#include "KeyBoard.h"
#include "io.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_SNUM		8
CString strChar[36] = {_T("0"), _T("1"), _T("2"), _T("3"), _T("4"), 
						 _T("5"), _T("6"), _T("7"), _T("8"), _T("9"), 
						 _T("D"), _T("G"), _T("A"), _T("C"), _T("P"), 
						 _T("B"), _T("I"), _T("L"), _T("N"), _T("H"), 
						 _T("T"), _T("W"), _T("Z"), _T("E"), _T("F"), 
						 _T("J"), _T("K"), _T("M"), _T("O"), _T("Q"), 
						 _T("X"), _T("Y"), _T("R"), _T("S"), _T("U"), 
						 _T("V")};

int nBcrCnt;
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMgrSerialSocketDlg dialog

CMgrSerialSocketDlg::CMgrSerialSocketDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMgrSerialSocketDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMgrSerialSocketDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMgrSerialSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMgrSerialSocketDlg)
	DDX_Control(pDX, IDC_MSG_SERIAL_NUM, m_msgSerialNum);
	DDX_Control(pDX, IDC_MSG_SERIAL_DATA, m_msgSerialData);
	DDX_Control(pDX, IDC_MSG_SERIAL, m_msgSerial);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMgrSerialSocketDlg, CDialog)
	//{{AFX_MSG_MAP(CMgrSerialSocketDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN1, OnBtn1)
	ON_BN_CLICKED(IDC_MSG_SERIAL_DATA, OnMsgSerialData)
	ON_BN_CLICKED(IDC_BTN5, OnBtn5)
	ON_BN_CLICKED(IDC_BTN10, OnBtn10)
	ON_BN_CLICKED(IDC_BTN20, OnBtn20)
	ON_BN_CLICKED(IDC_MSG_SERIAL_NUM, OnMsgSerialNum)
	ON_BN_CLICKED(IDC_BTN_RUN, OnBtnRun)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMgrSerialSocketDlg message handlers

BOOL CMgrSerialSocketDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_pFont	= NULL;
	m_pFont	= new CFont;
	m_pFont->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("MS Sans Serif"));
	InitLabel();
	InitGridManager(0);

	nBcrCnt = 0;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMgrSerialSocketDlg::InitLabel()
{
	CString strTemp;
	m_msgSerial.SetFont(m_pFont);
	m_msgSerial.SetWindowText(_T("Header"));
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

	m_msgSerialNum.SetFont(m_pFont);
	m_msgSerialNum.SetWindowText(_T(""));
	m_msgSerialNum.SetCenterText();
	m_msgSerialNum.SetColor(YELLOW_L);
	m_msgSerialNum.SetGradientColor(YELLOW_L);
	m_msgSerialNum.SetTextColor(BLACK_C);

}

void CMgrSerialSocketDlg::InitGridManager(int nCount)
{
	int i, j;
	int nMaxRow, nMaxCol;
	
	CString strTemp;

	nMaxRow = nCount + 1;	
	nMaxCol = 2;

	m_pGridSerial = (TSpread*)GetDlgItem(IDC_CUSTOM_SOCKET);
	
	m_pGrid.GridReset(m_pGridSerial);
	// 대문자 
	m_pGrid.GridCellSelectDisplay(m_pGridSerial, TRUE);
	m_pGrid.GridRowHeader(m_pGridSerial, FALSE);
	m_pGrid.GridColHeader(m_pGridSerial, FALSE);
	m_pGrid.GridHorizontal(m_pGridSerial, FALSE);
	m_pGrid.GridVertical(m_pGridSerial, TRUE);
	m_pGrid.GridAutoSize(m_pGridSerial, FALSE);
	m_pGrid.GridAutoSize(m_pGridSerial, FALSE);
	m_pGrid.GridCellRows(m_pGridSerial, nMaxRow);
	m_pGrid.GridCellCols(m_pGridSerial, nMaxCol);
	
	for(i=0; i<nMaxRow; i++)
	{
		m_pGrid.GridCellHeight_P(m_pGridSerial, i + 1, 45);
		
		for(j=0; j<nMaxCol; j++)
		{
			if (j == 0)
			{
				m_pGrid.GridCellWidth_P(m_pGridSerial, j+1, 100);
			}
			else
			{
				m_pGrid.GridCellWidth_P(m_pGridSerial, j+1, 340);
			}
			m_pGrid.GridCellControlStatic(m_pGridSerial, i + 1, j+1);
			m_pGrid.GridCellColor(m_pGridSerial, i + 1, j+1, BLACK_L, YELLOW_C);
		}
	}


	m_pGrid.GridCellFont(m_pGridSerial, 1, 1, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSerial, 1, 1, WHITE_C, BLACK_C);
	m_pGrid.GridCellText(m_pGridSerial, 1, 1, _T("No"));
	
	m_pGrid.GridCellFont(m_pGridSerial, 1, 2, "MS Sans Serif", 20);
	m_pGrid.GridCellColor(m_pGridSerial, 1, 2, BLUE_D, WHITE_C);
	m_pGrid.GridCellText(m_pGridSerial, 1, 2, _T("Serial"));	


}



void CMgrSerialSocketDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMgrSerialSocketDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMgrSerialSocketDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CMgrSerialSocketDlg::CheckHeader()
{
	CString strSerialNum = m_msgSerialNum.GetWindowText();

	if( atoi(strSerialNum) > 500)
	{
		AfxMessageBox("The character have to number and It has to less than 500");
		return FALSE;
	}

	if( GetSerialData() != TRUE)
	{
		return FALSE;
	}
	return TRUE;
}

void CMgrSerialSocketDlg::OnBtn1() 
{
	if( CheckHeader() != TRUE)
	{
		return;
	}
	
	nBcrCnt = 1;
	OutBarcode();
/*
	srand(time(0));
	
    int a[MAX_SNUM]={0};
    int ref[MAX_SNUM]={0} ;
    int sel = 0;
	
    for(int i= 0 ; i< 10 ;i++)
        ref[i] = i ;
	
    for(i=0; i< MAX_SNUM ;i++)
    {
        sel = rand() % 10 ;
        a[i]=ref[sel];

        for(int j = sel ; j < MAX_SNUM-i ; j++)
            ref[j] = ref[j+1];
    }

// 	char strSerial[MAX_SNUM];

	CString strSerial;
	CString sendstrSerial;
	CString strHeader = m_msgSerialData.GetWindowText();
	strSerial.Format("%s%d%d%d%d%d%d%d%d", strHeader, a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);

// 	strcpy(strSerial, a);
	CString newStr;
	if( OnKeyTotal(strSerial, newStr) != TRUE )
	{
		AfxMessageBox("Can't make serial try again");
		return;
	}

	strSerial += newStr;
// 	m_msgSerialData.SetWindowText(strSerial);
	KeyCheck(strSerial);
// 	UpdateData(FALSE);

	DisplayGridManager(strSerial);

	sendstrSerial = (LPCTSTR)strSerial;
	DisplaySerial(1, &sendstrSerial);
*/
}

void CMgrSerialSocketDlg::DisplaySerial(int nNum, CString* strSerial)
{
	CString strTemp;
	
	InitGridManager(nNum);

	CString str_file_folder,mstr_temp; 
	
	CString str_folder_data = _T("C:\\MgrSocketSerial\\log\\");
	
	COleDateTime time_cur;
	CTime m_time_current;
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour, mn_cur_min, mn_cur_sec; // 현재 년, 월, 일 정보 저장 변수 
	
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 
	
	m_time_current = CTime::GetCurrentTime();  // 간략한 형식의 현재 시간 정보 얻는다. 
	
	mn_cur_year = time_cur.GetYear();  
	
	mn_cur_month = time_cur.GetMonth();  
	
    mn_cur_day = time_cur.GetDay();  
	
	mn_cur_hour = time_cur.GetHour();
	
	mn_cur_min = time_cur.GetMinute();
	mn_cur_sec = time_cur.GetSecond();
	

	CString str_revipe_name;

	CreateFolder(str_folder_data);
	
	str_revipe_name.Format("%04d%02d%02d%02d%02d%02d", mn_cur_year,mn_cur_month,mn_cur_day,mn_cur_hour,mn_cur_min,mn_cur_sec);
	str_file_folder.Format( "%s%s",str_folder_data, str_revipe_name + ".txt");

	for ( int i=0; i<nNum; i++)
	{			
		strTemp.Format(_T("%d"), i + 1);
		m_pGrid.GridCellText(m_pGridSerial, i + 2, 1, strTemp);
		m_pGrid.GridCellText(m_pGridSerial, i + 2, 2, strSerial[i]);
		:: WritePrivateProfileString("Machine Serial", strTemp, LPCTSTR(strSerial[i]), str_file_folder);
	}
	if( nNum > 0)
		::ShellExecute(NULL, NULL, "NotePAD.exe", str_file_folder, NULL, SW_SHOWNORMAL);// TODO: Add your control notification handler code here

}

void CMgrSerialSocketDlg::DisplayGridManager( CString m_StrSerial)
{
	int nNum;
	int nCount;
	
	CString strTable[3];
	CString strData[3],strData1;
	CString strTemp;

	strData[2]	= (LPCTSTR)m_StrSerial;
	
	nNum = Mysql.MySql_Open();
	if (nNum < 0) return;

	if(Mysql.Database_Select(nNum, _T("AMT")))
	{
		if(Mysql.Table_Select(nNum, _T("AMT"), _T("SOCKET_SERIAL")))
		{
			nCount = Mysql.Record_Text_Search(nNum, _T("SOCKET_SERIAL"), _T("SERIAL"), strData[2]);
			if(nCount > 0)
			{
				Mysql.MySql_Close(nNum);
				
				AfxMessageBox(_T("Please check the serial duplication"));
				return;
			}
			
			if (KeyCheck(strData[2]) == TRUE)
			{
				Mysql.Record_Add(nNum, _T("SOCKET_SERIAL"), strData);
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
			if(Mysql.Table_Create(nNum, _T("AMT"), _T("SOCKET_SERIAL"), Mysql.m_nManagerTableCnt, Mysql.m_strManagerTableName, Mysql.m_nManagerTableType, Mysql.m_nManagerTableNum))
			{
				Mysql.Record_Add(nNum, _T("SOCKET_SERIAL"), strData);
			}
		}
	}
	else
	{
		// amt database가 없으면 database 추가
		if(Mysql.Database_Create(nNum, _T("AMT")))
		{
			// table이 없으면 새로 table 추가
			if(Mysql.Table_Create(nNum, _T("AMT"), _T("SOCKET_SERIAL"), Mysql.m_nManagerTableCnt, Mysql.m_strManagerTableName, Mysql.m_nManagerTableType, Mysql.m_nManagerTableNum))
			{
				Mysql.Record_Add(nNum, _T("SOCKET_SERIAL"), strData);
			}
		}
	}
	
	Mysql.MySql_Close(nNum);
}

int CMgrSerialSocketDlg::OnKeyTotal(CString strKey, CString& strNew)
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
	if (nLength < 9 || nLength > 9) return FALSE;

	for (i=0; i<9; i++)
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

	strNew = (LPCTSTR)strData[3];
	
	return TRUE;
}

void CMgrSerialSocketDlg::OnMsgSerialData() 
{
	CString strSerial = _T("");
	
	KeyBoard(&strSerial);
	
	strSerial.MakeUpper();
	if(strSerial.GetLength() > 1)
	{
		AfxMessageBox("The Header is to have to one word");
		return;
	}

	int nLength  =strSerial.GetLength();
	char chr_data[2];
	strcpy(chr_data, strSerial);
	int n_check_num = 0, n_check_string = 0, n_check_extra = 0;
	
	for (int i = 0; i < nLength; i++)
	{
		if (chr_data[i] >= 48 && chr_data[i] <= 57)			// 숫자 확인
		{
			n_check_num++;
		}
		else if (chr_data[i] >= 65 && chr_data[i] <= 90)	// 문자 확인
		{
			n_check_string++;
		}
		else
		{
			n_check_extra++;
		}
	}
	if(n_check_num>0 || n_check_extra > 0)
	{
		AfxMessageBox("Header have to character");
		return;
	}
	else if(n_check_string> 1)
	{
		AfxMessageBox("Header have to one character");
		return;
	}


	m_msgSerialData.SetWindowText(strSerial);	
}

BOOL CMgrSerialSocketDlg::KeyCheck(CString strKey)
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

BOOL CMgrSerialSocketDlg::DestroyWindow() 
{
	if (m_pFont != NULL)
	{
		delete m_pFont;
		m_pFont = NULL;
	}	
	
	return CDialog::DestroyWindow();
}

void CMgrSerialSocketDlg::OnBtn5() 
{
	if( CheckHeader() != TRUE)
	{
		return;
	}
	
	nBcrCnt = 5;
	OutBarcode();
/*
	srand(time(0));
	

	CString strSerial;
	CString strHeader;
	CString sendtrSerial[5];
	for ( int kk = 0; kk < 5; kk++)
	{
		int a[MAX_SNUM]={0};
		int ref[MAX_SNUM]={0} ;
		int sel = 0;
		
		for(int i= 0 ; i< MAX_SNUM ;i++)
			ref[i] = i ;
		
		for(i=0; i< MAX_SNUM ;i++)
		{
			sel = rand() % 10 ;
			a[i]=ref[sel];
			
			for(int j = sel ; j < MAX_SNUM-i ; j++)
				ref[j] = ref[j+1];
		}
		
		// 	char strSerial[MAX_SNUM];
		
		strHeader = m_msgSerialData.GetWindowText();
		strSerial.Format("%s%d%d%d%d%d%d%d%d", strHeader, a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
		
		// 	strcpy(strSerial, a);
		CString newStr;
		if( OnKeyTotal(strSerial, newStr) != TRUE )
		{
			AfxMessageBox("Can't make serial try again");
			return;
		}
		
		strSerial += newStr;
		// 	m_msgSerialData.SetWindowText(strSerial);
		KeyCheck(strSerial);
		// 	UpdateData(FALSE);
		
		DisplayGridManager(strSerial);
		sendtrSerial[kk] = (LPCTSTR)strSerial;
	}

	if( kk > 1 )
	{
		DisplaySerial(kk, sendtrSerial);
	}

  */
}			


void CMgrSerialSocketDlg::OnBtn10() 
{
	if( CheckHeader() != TRUE)
	{
		return;
	}
	
	nBcrCnt = 10;
	OutBarcode();

/*
	srand(time(0));
	
	
	CString strSerial;
	CString strHeader;
	CString sendtrSerial[10];
	for ( int kk = 0; kk < 10; kk++)
	{
		int a[MAX_SNUM]={0};
		int ref[MAX_SNUM]={0} ;
		int sel = 0;
		
		for(int i= 0 ; i< MAX_SNUM ;i++)
			ref[i] = i ;
		
		for(i=0; i< MAX_SNUM ;i++)
		{
			sel = rand() % 10 ;
			a[i]=ref[sel];
			
			for(int j = sel ; j < MAX_SNUM-i ; j++)
				ref[j] = ref[j+1];
		}
		
		// 	char strSerial[MAX_SNUM];
		
		strHeader = m_msgSerialData.GetWindowText();
		strSerial.Format("%s%d%d%d%d%d%d%d%d", strHeader, a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
		
		// 	strcpy(strSerial, a);
		CString newStr;
		if( OnKeyTotal(strSerial, newStr) != TRUE )
		{
			AfxMessageBox("Can't make serial try again");
			return;
		}
		
		strSerial += newStr;
		// 	m_msgSerialData.SetWindowText(strSerial);
		KeyCheck(strSerial);
		// 	UpdateData(FALSE);
		
		DisplayGridManager(strSerial);
		sendtrSerial[kk] = (LPCTSTR)strSerial;
	}
	
	if( kk > 1 )
	{
		DisplaySerial(kk, sendtrSerial);
	}
*/	
}

void CMgrSerialSocketDlg::OnBtn20() 
{
	if( CheckHeader() != TRUE)
	{
		return;
	}
	
	nBcrCnt = 20;
	OutBarcode();
	
/*	
	CString strSerial;
	CString strHeader;
	CString sendtrSerial[20];
	for ( int kk = 0; kk < 20; kk++)
	{
		int a[MAX_SNUM]={0};
		int ref[MAX_SNUM]={0} ;
		int sel = 0;
		
		for(int i= 0 ; i< MAX_SNUM ;i++)
			ref[i] = i ;
		
		for(i=0; i< MAX_SNUM ;i++)
		{
			sel = rand() % 10 ;
			a[i]=ref[sel];
			
			for(int j = sel ; j < MAX_SNUM-i ; j++)
				ref[j] = ref[j+1];
		}
		
		// 	char strSerial[MAX_SNUM];
		
		strHeader = m_msgSerialData.GetWindowText();
		strSerial.Format("%s%d%d%d%d%d%d%d%d", strHeader, a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
		
		// 	strcpy(strSerial, a);
		CString newStr;
		if( OnKeyTotal(strSerial, newStr) != TRUE )
		{
			AfxMessageBox("Can't make serial try again");
			return;
		}
		
		strSerial += newStr;
		KeyCheck(strSerial);
		
		DisplayGridManager(strSerial);
		sendtrSerial[kk] = (LPCTSTR)strSerial;
	}
	
	if( kk > 1 )
	{
		DisplaySerial(kk, sendtrSerial);
	}
*/	
}

void CMgrSerialSocketDlg::OnMsgSerialNum() 
{
	CString strSerial = _T("");
	
	KeyBoard(&strSerial);
	
	strSerial.TrimLeft();
	strSerial.TrimRight();
	strSerial.TrimLeft(' ');
	strSerial.TrimRight(' ');

	int nLength  = strSerial.GetLength();
	char chr_data[2];
	strcpy(chr_data, strSerial);
	int n_check_num = 0, n_check_string = 0, n_check_extra = 0;
	
	if(nLength <= 0)
	{
		AfxMessageBox("There is no character. Blank character.");
		return;
	}	

	for (int i = 0; i < nLength; i++)
	{
		if (chr_data[i] >= 48 && chr_data[i] <= 57)			// 숫자 확인
		{
			n_check_num++;
		}
		else if (chr_data[i] >= 65 && chr_data[i] <= 90)	// 문자 확인
		{
			n_check_string++;
		}
		else
		{
			n_check_extra++;
		}
	}
	if(n_check_string>0 || n_check_extra > 0 || atoi(strSerial) > 500)
	{
		AfxMessageBox("The character have to number and It has to less than 500");
		return;
	}	
	
	m_msgSerialNum.SetWindowText(strSerial);	
}

BOOL CMgrSerialSocketDlg::GetSerialData()
{
	CString strSerial;

	strSerial = m_msgSerialData.GetWindowText();

	if( strSerial.GetLength() <= 0 || strSerial.GetLength() > 1 )
	{
		AfxMessageBox("Thers is no Header or Header character is too much!");
		return FALSE;
	}

	char chr_data[1];
	int n_check_num = 0, n_check_string = 0, n_check_extra = 0;
	strcpy(chr_data, strSerial);

	for (int i = 0; i < strSerial.GetLength(); i++)
	{
		if (chr_data[i] >= 48 && chr_data[i] <= 57)			// 숫자 확인
		{
			n_check_num++;
		}
		else if (chr_data[i] >= 65 && chr_data[i] <= 90)	// 문자 확인
		{
			n_check_string++;
		}
		else
		{
			n_check_extra++;
		}
	}
	if(n_check_num>0 || n_check_extra > 0 || atoi(strSerial) > 500 || n_check_string > 1)
	{
		AfxMessageBox("Header is not character.");
		return FALSE;
	}
	
	return TRUE;
}

void CMgrSerialSocketDlg::OnBtnRun() 
{
	if( CheckHeader() != TRUE)
	{
		return;
	}
	
	CString strSerialNum = m_msgSerialNum.GetWindowText();	
	nBcrCnt = atoi(strSerialNum);
	OutBarcode();
}

void CMgrSerialSocketDlg::OutBarcode()
{	
	srand(time(0));
	CString strHeader, sendtrSerial[500];
	int sel = 0;

	for ( int kk = 0; kk < nBcrCnt; kk++)
	{		
		char a[MAX_SNUM+1]={'\0',};
		char ref[MAX_SNUM+1]={'\0',} ;
		for(int i=0; i< MAX_SNUM ;i++)
		{
			//a[i]='\0';
			//ref[i]='\0';
			sel = rand() % 36 ;
			a[i] = strChar[sel].GetAt(0);

			
			for(int j = sel ; j < MAX_SNUM-i ; j++)
				strChar[j] = strChar [j+1];
		}
		
		CString newStr,strSerial;
		
		strHeader = m_msgSerialData.GetWindowText();
//		strSerial.Format("%s%d%d%d%d%d%d%d%d", strHeader, a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
		strSerial.Format("%s%s", strHeader, a);
		
		// 	strcpy(strSerial, a);
		
		if( OnKeyTotal(strSerial, newStr) != TRUE )
		{
			AfxMessageBox("Can't make serial try again");
			return;
		}
		
		strSerial += newStr;
		// 	m_msgSerialData.SetWindowText(strSerial);
		KeyCheck(strSerial);
		// 	UpdateData(FALSE);
		
		DisplayGridManager(strSerial);
		sendtrSerial[kk] = (LPCTSTR)strSerial;

		for( i=0; i< MAX_SNUM+1 ;i++)
		{
			a[i]='\0',	ref[i]='\0';
		}

	}
	
	if( kk > 0 )
	{
		DisplaySerial(kk, sendtrSerial);
	}	
}

BOOL CMgrSerialSocketDlg::CreateFolder(LPCTSTR szPath)
{
	int nRet = 0;
	
	CString strPath = szPath;
	
    do{
        nRet = strPath.Find('\\' , nRet + 1);
        if (nRet > 0 )
        {
            CString strTemp = strPath.Left(nRet+1);
			
            if (_access(strTemp.operator LPCTSTR(), 00) == -1)
			{
				CreateDirectory(strTemp , NULL);
			}
        }
    }while (nRet != -1);
	
    return nRet;
}
	

	
