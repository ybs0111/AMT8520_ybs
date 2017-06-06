// DialogAdd.cpp : implementation file
//

#include "stdafx.h"
#include "ManagerSocket.h"
#include "DialogAdd.h"
// #include "afxdialogex.h"
#include "KeyBoard.h"
#include ".\\Ctrl\\Variable.h"
#include ".\\Ctrl\\Alg_Mysql.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogAdd dialog


CDialogAdd::CDialogAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAdd)
	//}}AFX_DATA_INIT
}


void CDialogAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAdd)
	DDX_Control(pDX, IDC_MSG_COUNT_DATA, m_msgCountData);
	DDX_Control(pDX, IDC_MSG_COUNT, m_msgCount);
	DDX_Control(pDX, IDC_MSG_HIFIX_DATA, m_msgHifixData);
	DDX_Control(pDX, IDC_MSG_HIFIX, m_msgHifix);
	DDX_Control(pDX, IDC_MSG_SERIAL_DATA, m_msgSerialData);
	DDX_Control(pDX, IDC_MSG_SERIAL, m_msgSerial);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAdd, CDialog)
	//{{AFX_MSG_MAP(CDialogAdd)
	ON_BN_CLICKED(IDC_MSG_SERIAL_DATA, OnMsgSerialData)
	ON_BN_CLICKED(IDC_MSG_HIFIX_DATA, OnMsgHifixData)
	ON_BN_CLICKED(IDC_MSG_COUNT_DATA, OnMsgCountData)
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAdd message handlers


BOOL CDialogAdd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_pFont	= NULL;
	m_pFont	= new CFont;
	m_pFont->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0, _T("MS Sans Serif"));

	InitLabel();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDialogAdd::DestroyWindow() 
{
	if (m_pFont != NULL)
	{
		delete m_pFont;
		m_pFont = NULL;
	}	
	return CDialog::DestroyWindow();
}

void CDialogAdd::OnMsgSerialData() 
{
	CString strTemp;
	
	strTemp = _T("");
	KeyBoard(&strTemp);
	
	m_msgSerialData.SetWindowText(strTemp);	
}

void CDialogAdd::OnMsgHifixData() 
{
	int nKey;
	
	CString strTemp;
	
	nKey = 1;
	KeyPadI(1, st_manager_info.nHifixNum, &nKey);
	
	strTemp.Format(_T("%d"), nKey);
	
	m_msgHifixData.SetWindowText(strTemp);	
}

void CDialogAdd::OnMsgCountData() 
{
	int nKey;
	
	CString strTemp;
	
	nKey = 0;
	KeyPadI(1, 10000000, &nKey);
	
	strTemp.Format(_T("%d"), nKey);
	
	m_msgCountData.SetWindowText(strTemp);	
}

void CDialogAdd::OnBtnAdd() 
{
	int nNum;
	int nHifix;
	int nCount;
	
	CString strData[8];
	
	nNum = Mysql.MySql_Open();
	
	if (nNum < 0) return;
	
	strData[2]	= m_msgSerialData.GetWindowText();
	
	//nHifix = _wtoi(m_msgHifixData.GetWindowText());
	nHifix = atoi(m_msgHifixData.GetWindowText());
	if (nHifix < 1 || nHifix > st_manager_info.nHifixNum) return;
	strData[3].Format(_T("%d"), nHifix);
	
//	nCount = _wtoi(m_msgCountData.GetWindowText());
	nCount = atoi(m_msgCountData.GetWindowText());
	if (nCount < 0) nCount = 0;
	strData[4].Format(_T("%d"), nCount);
	strData[5]	= _T("-1");
	//strData[6].Format(_T("%s"), st_manager_info.strHifix[nHifix - 1]);
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
			
			Mysql.Record_Add(nNum, _T("SOCKET"), strData);
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
	
	Mysql.MySql_Close(nNum);	
}

void CDialogAdd::OnBtnExit() 
{
	CDialog::OnOK();	
}

void CDialogAdd::InitLabel() 
{
	CString strTemp;
	
	m_msgHifix.SetFont(m_pFont);
	m_msgHifix.SetWindowText(_T("Hifix"));
	m_msgHifix.SetCenterText();
	m_msgHifix.SetColor(WHITE_C);
	m_msgHifix.SetGradientColor(ORANGE_C);
	m_msgHifix.SetTextColor(BLACK_C);
	
	m_msgHifixData.SetFont(m_pFont);
	m_msgHifixData.SetWindowText(_T("1"));
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
	
	m_msgCount.SetFont(m_pFont);
	m_msgCount.SetWindowText(_T("Count"));
	m_msgCount.SetCenterText();
	m_msgCount.SetColor(WHITE_C);
	m_msgCount.SetGradientColor(ORANGE_C);
	m_msgCount.SetTextColor(BLACK_C);
	
	m_msgCountData.SetFont(m_pFont);
	m_msgCountData.SetWindowText(_T("0"));
	m_msgCountData.SetCenterText();
	m_msgCountData.SetColor(YELLOW_L);
	m_msgCountData.SetGradientColor(YELLOW_L);
	m_msgCountData.SetTextColor(BLACK_C);	
}
