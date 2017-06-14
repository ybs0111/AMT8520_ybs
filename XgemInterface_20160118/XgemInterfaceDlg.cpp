// XgemInterfaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XgemInterface.h"
#include "XgemInterfaceDlg.h"
#include "Global.h"
#include "Variable.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "XgemCommand.h"
#include "io.h"			// 파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
tagHANDLER_INFO	st_handler_info;
tagCLIENT_INFO	st_client_info[10];
tagSERVER_INFO	st_server_info[10];
tagMSG_INFO		st_msg_info;
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
// CXgemInterfaceDlg dialog

CXgemInterfaceDlg::CXgemInterfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXgemInterfaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXgemInterfaceDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	int i;

	m_pFont				= NULL;
	m_nXgemOpenFlag		= CONNECT_NO;
	m_nXgemStartFlag	= CONNECT_NO;

	m_strRecvData		= "";

	for(i=0; i<10; i++)
	{
		m_pServer[i] = NULL;
		m_pClient[i] = NULL;
	}
}

void CXgemInterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXgemInterfaceDlg)
	DDX_Control(pDX, IDC_LIST_DATA, m_list_data);
	DDX_Control(pDX, IDC_MSG_MACHINE_STATUS, m_msgMachineStatus);
	DDX_Control(pDX, IDC_MSG_HOST_CONNECTOR, m_msgHostConnt);
	DDX_Control(pDX, IDC_MSG_EQ_CONNECTOR, m_msgEqConnt);
	DDX_Control(pDX, IDC_EXGEMCTRL_GEM, m_hXgem);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXgemInterfaceDlg, CDialog)
	//{{AFX_MSG_MAP(CXgemInterfaceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_VIEW, OnView)
	ON_BN_CLICKED(IDC_BTN_HIDE, OnBtnHide)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotification)
	ON_MESSAGE(WM_CLIENT_MSG_1, OnCommand_Client_1)							// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_CLIENT_MSG_2, OnCommand_Client_2)							// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_SERVER_MSG_1, OnCommand_Server_1)							// Network관련된 작업을 담당한다.
	ON_MESSAGE(WM_LIST_DATA, OnListDataUpdate)  // 수신 메시지 리스트 박스 컨트롤에 추가 메시지 선언  
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXgemInterfaceDlg message handlers

BOOL CXgemInterfaceDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	CreateFolder("C:\\XGEM\\Log\\CIM\\Monthly\\");
	CreateFolder("C:\\XGEM\\Log\\CIM\\Daily\\");

	st_handler_info.cwnd_Main = this;

	if (!m_TrayIcon.Create(this, WM_ICON_NOTIFY, "XGem Interface Program", 
		NULL, IDR_MENU))
		return -1;
	
	// 아이콘 설정
	// 아이콘 아이디에 자신의 아이콘을 넣으세요...
	m_TrayIcon.SetIcon(IDI_ICON4);

	m_pFont = NULL;
	m_pFont = new CFont;
	m_pFont->CreateFont(12,8,0,0,900,0,0,0,0,0,0,DEFAULT_QUALITY,0,"Courier");

	OnLabelSet();
	OnXgemVariable();

	st_server_info[0].n_port = 10056;
	::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_CONNECT, 0);

	int k = 0;
	for ( int i = 0;i < 1000000; i++)
	{
		k++;
	}
	CString strHead, strFunc, strGemInit;
	CString strPath = "C:\\XGEM\\EqSample_1.cfg";
	
	strFunc = "FUNC=COMMUNICATION,CONFIG=" + strPath + ",";
	strHead.Format("HD=%06d,", strFunc.GetLength());
	strGemInit = strHead+strFunc;

	OnEqToXgemCommand( (LPCTSTR) strGemInit);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXgemInterfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CXgemInterfaceDlg::OnPaint() 
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
HCURSOR CXgemInterfaceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXgemInterfaceDlg::OnLabelSet()
{
	m_msgEqConnt.SetFont(m_pFont);
	m_msgEqConnt.SetWindowText("XGEM ↔ EQ");
	m_msgEqConnt.SetCenterText();
	m_msgEqConnt.SetColor(BLUE_D);
	m_msgEqConnt.SetGradientColor(BLUE_D);
	m_msgEqConnt.SetTextColor(WHITE_C);
	OnEqConnectStatus(CONNECT_NO);

	m_msgHostConnt.SetFont(m_pFont);
	m_msgHostConnt.SetWindowText("HOST ↔ XGEM");
	m_msgHostConnt.SetCenterText();
	m_msgHostConnt.SetColor(BLUE_D);
	m_msgHostConnt.SetGradientColor(BLUE_D);
	m_msgHostConnt.SetTextColor(WHITE_C);
	OnHostConnectStatus(CONNECT_NO);

	m_msgMachineStatus.SetFont(m_pFont);
	m_msgMachineStatus.SetWindowText("HOST ↔ XGEM");
	m_msgMachineStatus.SetCenterText();
	m_msgMachineStatus.SetColor(BLUE_D);
	m_msgMachineStatus.SetGradientColor(BLUE_D);
	m_msgMachineStatus.SetTextColor(WHITE_C);
	OnMachineStatus(MC_OFF_LINE);
}



BEGIN_EVENTSINK_MAP(CXgemInterfaceDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CXgemInterfaceDlg)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 2 /* eGEMCommStateChanged */, OneGEMCommStateChangedExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 3 /* eGEMControlStateChanged */, OneGEMControlStateChangedExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 27 /* eXGEMStateEvent */, OneXGEMStateEventExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 10 /* eGEMReqRemoteCommand */, OneGEMReqRemoteCommandExgemctrlGem, VTS_I4 VTS_BSTR VTS_I4 VTS_PBSTR VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 24 /* eGEMTerminalMessage */, OneGEMTerminalMessageExgemctrlGem, VTS_I4 VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 25 /* eGEMTerminalMultiMessage */, OneGEMTerminalMultiMessageExgemctrlGem, VTS_I4 VTS_I4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 9 /* eGEMErrorEvent */, OneGEMErrorEventExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 7 /* eGEMRspGetDateTime */, OneGEMRspGetDateTimeExgemctrlGem, VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 8 /* eGEMReqDateTime */, OneGEMReqDateTimeExgemctrlGem, VTS_I4 VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 6 /* eGEMReqGetDateTime */, OneGEMReqGetDateTimeExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 33 /* eGEMReportedEvent */, OneGEMReportedEventExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 35 /* eGEMReqOffline */, OneGEMReqOfflineExgemctrlGem, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 36 /* eGEMReqOnline */, OneGEMReqOnlineExgemctrlGem, VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 5 /* eGEMECVChanged */, OneGEMECVChangedExgemctrlGem, VTS_I4 VTS_PI4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 4 /* eGEMReqChangeECV */, OneGEMReqChangeECVExgemctrlGem, VTS_I4 VTS_I4 VTS_PI4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 1 /* eSECSMessageReceived */, OneSECSMessageReceivedExgemctrlGem, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 11 /* eGEMReqPPLoadInquire */, OneGEMReqPPLoadInquireExgemctrlGem, VTS_I4 VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 12 /* eGEMRspPPLoadInquire */, OneGEMRspPPLoadInquireExgemctrlGem, VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 13 /* eGEMReqPPSend */, OneGEMReqPPSendExgemctrlGem, VTS_I4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 14 /* eGEMRspPPSend */, OneGEMRspPPSendExgemctrlGem, VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 15 /* eGEMReqPP */, OneGEMReqPPExgemctrlGem, VTS_I4 VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 16 /* eGEMRspPP */, OneGEMRspPPExgemctrlGem, VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 17 /* eGEMReqPPDelete */, OneGEMReqPPDeleteExgemctrlGem, VTS_I4 VTS_I4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 18 /* eGEMReqPPList */, OneGEMReqPPListExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 19 /* eGEMReqPPFmtSend */, OneGEMReqPPFmtSendExgemctrlGem, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_PBSTR VTS_PI4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 20 /* eGEMRspPPFmtSend */, OneGEMRspPPFmtSendExgemctrlGem, VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 21 /* eGEMReqPPFmt */, OneGEMReqPPFmtExgemctrlGem, VTS_I4 VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 22 /* eGEMRspPPFmt */, OneGEMRspPPFmtExgemctrlGem, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_PBSTR VTS_PI4 VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 23 /* eGEMRspPPFmtVerification */, OneGEMRspPPFmtVerificationExgemctrlGem, VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 28 /* eGEMRspAllECInfo */, OneGEMRspAllECInfoExgemctrlGem, VTS_I4 VTS_PI4 VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 29 /* eGEMReqPPSendEx */, OneGEMReqPPSendExExgemctrlGem, VTS_I4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 30 /* eGEMRspPPEx */, OneGEMRspPPExExgemctrlGem, VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 31 /* eGEMReqPPEx */, OneGEMReqPPExExgemctrlGem, VTS_I4 VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 32 /* eGEMRspPPSendEx */, OneGEMRspPPSendExExgemctrlGem, VTS_BSTR VTS_BSTR VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 34 /* eGEMRspLoopback */, OneGEMRspLoopbackExgemctrlGem, VTS_I4 VTS_PI4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 37 /* eGEMNotifyPerformanceWarning */, OneGEMNotifyPerformanceWarningExgemctrlGem, VTS_I4)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 38 /* eGEMSecondaryMsgReceived */, OneGEMSecondaryMsgReceivedExgemctrlGem, VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 39 /* eGEMReqPPFmtSend2 */, OneGEMReqPPFmtSend2ExgemctrlGem, VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_PBSTR VTS_PI4 VTS_PBSTR VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXGEMCTRL_GEM, 40 /* eGEMRspPPFmt2 */, OneGEMRspPPFmt2ExgemctrlGem, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_PBSTR VTS_PI4 VTS_PBSTR VTS_PBSTR)
	ON_EVENT(CXgemInterfaceDlg, IDC_EXCOMCTRL31, 2 /* SecsEvent */, OnSecsEventExcomctrl31, VTS_I2 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CXgemInterfaceDlg::OneGEMCommStateChangedExgemctrlGem(long nState) 
{
	// TODO: Add your control notification handler code here
	if (nState == Comm_Disabled)
	{
		OnHostConnectStatus(CONNECT_NO);

		sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] Comm_Disabled");
		::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
	}
	else if(nState == Comm_WaitCRFromHost)
	{
		OnHostConnectStatus(CONNECT_NO);

		sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] Comm_WaitCRFromHost");
		::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
	}
	else if(nState == Comm_WaitDelay)
	{
	}
	else if(nState == Comm_WaitCRA)
	{
		OnHostConnectStatus(CONNECT_NO);
		OnMachineStatus(MC_OFF_LINE);

		sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] Comm_WaitCRA");
		::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
	}
	else if(nState == Comm_Communicating)
	{
		OnHostConnectStatus(CONNECT_YES);
		ShowWindow(SW_HIDE);

		sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] Comm_Communicating");
		::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);

		clsXgem.OnEqMachineRemote();
	}
}

void CXgemInterfaceDlg::OneGEMControlStateChangedExgemctrlGem(long nState) 
{
	// TODO: Add your control notification handler code here
	CString strHead;
	CString strFunc;
	int     a;

	if (nState == Control_None)
	{
		OnMachineStatus(MC_OFF_LINE);
	}
	else if (nState == Control_EqOffline)
	{
		a = 0;
		strFunc = "FUNC=INTERFACE,CONNECT=0,";
		strHead.Format("HD=%06d,", strFunc.GetLength());
		sprintf(st_server_info[0].ch_send, strHead+strFunc);
		
		::Sleep(500);
		::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);

		OnMachineStatus(MC_OFF_LINE);
	}
	else if (nState == Control_AttemptOnline)
	{
		a = 1;
		strFunc = "FUNC=INTERFACE,CONNECT=1,";
		strHead.Format("HD=%06d,", strFunc.GetLength());
		sprintf(st_server_info[0].ch_send, strHead+strFunc);
		
		::Sleep(500);
		::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);

		OnMachineStatus(MC_ON_LINE_LOCAL);
	}
	else if (nState == Control_HostOffline)
	{
		a = 0;
		strFunc = "FUNC=INTERFACE,CONNECT=0,";
		strHead.Format("HD=%06d,", strFunc.GetLength());
		sprintf(st_server_info[0].ch_send, strHead+strFunc);
		
		::Sleep(500);
		::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);

		OnMachineStatus(MC_OFF_LINE);
	}
	else if (nState == Control_Local)
	{
		a = 1;
		strFunc = "FUNC=INTERFACE,CONNECT=1,";
		strHead.Format("HD=%06d,", strFunc.GetLength());
		sprintf(st_server_info[0].ch_send, strHead+strFunc);
		
		::Sleep(500);
		::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);

		OnMachineStatus(MC_ON_LINE_LOCAL);
	}
	else if (nState == Control_Remote)
	{
		a = 2;
		strFunc = "FUNC=INTERFACE,CONNECT=2,";
		strHead.Format("HD=%06d,", strFunc.GetLength());
		sprintf(st_server_info[0].ch_send, strHead+strFunc);
		
		::Sleep(500);
		::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);

		OnMachineStatus(MC_ON_LINE_REMOTE);

		clsXgem.OnEqMachineStatus(MC_STOP);
	}
}


void CXgemInterfaceDlg::OnHostConnectStatus(int nStatus)
{
	switch(nStatus)
	{
		case CONNECT_NO:
			m_msgHostConnt.SetColor(GREEN_D);
			m_msgHostConnt.SetGradientColor(GREEN_D);
			m_msgHostConnt.SetTextColor(BLACK_C);
			break;

		case CONNECT_YES:
			m_msgHostConnt.SetColor(GREEN_C);
			m_msgHostConnt.SetGradientColor(GREEN_C);
			m_msgHostConnt.SetTextColor(BLACK_C);
			break;
	}
}

void CXgemInterfaceDlg::OnEqConnectStatus(int nStatus)
{
	switch(nStatus)
	{
		case CONNECT_NO:
			m_msgEqConnt.SetColor(GREEN_D);
			m_msgEqConnt.SetGradientColor(GREEN_D);
			m_msgEqConnt.SetTextColor(BLACK_C);
			break;
			
		case CONNECT_YES:
			m_msgEqConnt.SetColor(GREEN_C);
			m_msgEqConnt.SetGradientColor(GREEN_C);
			m_msgEqConnt.SetTextColor(BLACK_C);
			break;
	}
}

void CXgemInterfaceDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CXgemInterfaceDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	int i;
	if(m_pFont != NULL)
	{
		delete m_pFont;
		m_pFont = NULL;
	}
	
	if (m_nXgemStartFlag == CONNECT_YES)
	{
		m_hXgem.Stop();
	}

	if (m_nXgemOpenFlag == CONNECT_YES)
	{
		m_hXgem.Close();
	}

	for(i=0; i<10; i++)
	{
		if(m_pServer[i] != NULL)
		{
			delete m_pServer[i];
			m_pServer[i] = NULL;
		}

		if(m_pClient[i] != NULL)
		{
			delete m_pClient[i];
			m_pClient[i] = NULL;
		}
	}
	CDialog::OnClose();
}

void CXgemInterfaceDlg::OneXGEMStateEventExgemctrlGem(long nState) 
{
	// TODO: Add your control notification handler code here
	clsXgem.OnXgemEvent(nState);
}

BOOL CXgemInterfaceDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}

LRESULT CXgemInterfaceDlg::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	return m_TrayIcon.OnTrayNotification(wParam, lParam);
}

void CXgemInterfaceDlg::OnExit() 
{
	// TODO: Add your command handler code here
	CDialog::OnOK();
}

void CXgemInterfaceDlg::OnView() 
{
	// TODO: Add your command handler code here
	ShowWindow(SW_SHOW);
}

void CXgemInterfaceDlg::OnMachineStatus(int nStatus)
{
	switch(nStatus)
	{
		case MC_OFF_LINE:
			m_msgMachineStatus.SetWindowText("Off - Line");
			m_msgMachineStatus.SetColor(GREEN_D);
			m_msgMachineStatus.SetGradientColor(GREEN_D);
			m_msgMachineStatus.SetTextColor(BLACK_L);
			break;

		case MC_ON_LINE_LOCAL:
			m_msgMachineStatus.SetWindowText("On - Line(Local)");
			m_msgMachineStatus.SetColor(GREEN_L);
			m_msgMachineStatus.SetGradientColor(GREEN_L);
			m_msgMachineStatus.SetTextColor(BLACK_C);
			break;

		case MC_ON_LINE_REMOTE:
			m_msgMachineStatus.SetWindowText("On - Line(Remote)");
			m_msgMachineStatus.SetColor(GREEN_C);
			m_msgMachineStatus.SetGradientColor(GREEN_C);
			m_msgMachineStatus.SetTextColor(BLACK_C);
			break;
	}
}

LRESULT CXgemInterfaceDlg::OnCommand_Client_1(WPARAM wParam, LPARAM lParam)
{	
	CString str_send;
	CString str_rev;
	CString str_tmp;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if(st_client_info[0].n_connect == CTL_NO)
			{
				m_pClient[0] = new CClientSocket;
				if(m_pClient[0]->Create(0, st_client_info[0].str_ip, st_client_info[0].n_port, 0))
				{
					st_client_info[0].n_connect = CTL_YES;
				}
				else
				{
					delete m_pClient[0];
					m_pClient[0]			= NULL;
					
					st_client_info[0].n_connect = CTL_NO;
				}
			}
			break;

		case CLIENT_CLOSE:
			if(st_client_info[0].n_connect == CTL_YES)
			{
				delete m_pClient[0];
				m_pClient[0]			= NULL;

				st_client_info[0].n_connect	= CTL_NO;
			}
			break;

		case CLIENT_SEND:
			if(st_client_info[0].n_connect == CTL_YES)
			{
				str_send.Format("%s", st_client_info[0].ch_send);
				m_pClient[0]->Send(str_send, str_send.GetLength());
			}
			break;

		case CLIENT_REV:
			str_tmp.Format("%s", st_client_info[0].ch_rev);
			str_rev = str_tmp.Mid(0, st_client_info[0].n_rev_length);  // 입력된 모든 데이터를 버퍼에 저장한다
			break;
	}
	return 0;
}

LRESULT CXgemInterfaceDlg::OnCommand_Client_2(WPARAM wParam, LPARAM lParam)
{	
	CString str_send;
	CString str_rev;
	CString str_tmp;

	switch(wParam)
	{
		case CLIENT_CONNECT:
			if(st_client_info[1].n_connect == CTL_NO)
			{
				m_pClient[1] = new CClientSocket;
				if(m_pClient[1]->Create(1, st_client_info[1].str_ip, st_client_info[1].n_port, 0))
				{
					st_client_info[1].n_connect = CTL_YES;
				}
				else
				{
					delete m_pClient[1];
					m_pClient[1]			= NULL;
					
					st_client_info[1].n_connect = CTL_NO;
				}
			}
			break;

		case CLIENT_CLOSE:
			if(st_client_info[1].n_connect == CTL_YES)
			{
				delete m_pClient[1];
				m_pClient[1]			= NULL;

				st_client_info[1].n_connect	= CTL_NO;
			}
			break;

		case CLIENT_SEND:
			if(st_client_info[1].n_connect == CTL_YES)
			{
				str_send.Format("%s", st_client_info[1].ch_send);
				m_pClient[1]->Send(str_send, str_send.GetLength());
			}
			break;

		case CLIENT_REV:
			str_rev.Format("%s", st_client_info[1].ch_rev);
			
			break;
	}
	
	return 0;
}

LRESULT CXgemInterfaceDlg::OnCommand_Server_1(WPARAM wParam, LPARAM lParam)
{	
	CString str_send;
	CString str_rev;
	CString str_tmp;
	CString str_find;
	int		pos = -1;
	int		count;

	switch(wParam)
	{
		case SERVER_CONNECT:
/*			if(st_server_info[lParam].n_connect == CTL_NO)
			{
				m_pServer[lParam] = new CServerSocket;
				if(m_pServer[lParam]->Create(lParam, st_server_info[lParam].n_port))
				{
					OnEqConnectStatus(CONNECT_YES);
					st_server_info[lParam].n_connect = CTL_YES;
				}
			}*/
			if(st_server_info[lParam].n_connect == CTL_NO)
			{
				m_pServer[lParam] = new CServerSocket;
				if(m_pServer[lParam]->Create(lParam, st_server_info[lParam].n_port))
				{
				}
			}
			break;
			
		case SERVER_CLOSE:
			if(st_server_info[lParam].n_connect == CTL_YES)
			{
				OnEqConnectStatus(CONNECT_NO);
				st_server_info[lParam].n_connect = CTL_NO;
			}
			break;
			
		case SERVER_SEND:
			if(st_server_info[lParam].n_connect == CTL_YES)
			{
				count = m_pServer[lParam]->GetClientCount();

				str_send.Format("%s", st_server_info[lParam].ch_send);
				m_pServer[lParam]->SendClientAt(m_pServer[lParam]->GetClient(count-1), str_send, str_send.GetLength());

				sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ]" + str_send);
				::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
			}
			break;
			
		case SERVER_REV:
			str_tmp.Format("%s", st_server_info[lParam].ch_rev);
			str_rev = str_tmp.Mid(0, st_server_info[lParam].n_rev_length);  // 입력된 모든 데이터를 버퍼에 저장한다
			
			sprintf(st_msg_info.c_normal_msg, "[EQ->XGEM]" + str_rev);
			::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);

			OnEqToXgemCommand(str_rev);
			break;

		case SERVER_CLIENT_COMM:
			if(st_server_info[lParam].n_connect == CTL_NO)
			{
				OnEqConnectStatus(CONNECT_YES);
				st_server_info[lParam].n_connect = CTL_YES;
			}
			break;
	}
	
	return 0;
}


void CXgemInterfaceDlg::OnEqToXgemCommand(CString strRecv)
{
	FILE *fp;

	int nLength;
	int nHead;
	int nPos[2];
	int nStatus;

	char chBody[100];

	CString strFunc;
	CString strFind;
	CString strConnect;
	CString strPath;

	nLength	= strRecv.GetLength() - 10;

	nPos[0]	= -1;
	nPos[0]	= strRecv.Find("HD=", 0);

	if(nPos[0] > -1)
	{
		nHead = atoi(strRecv.Mid(nPos[0]+3, 6));
		if(nHead == nLength)
		{
			strFunc = OnEqToXgemFunction("FUNC", strRecv);

			if (strFunc == "COMMUNICATION")
			{
				clsXgem.OnEqCommunication(&m_hXgem, strRecv);
			}
			else if (strFunc == "OPERATE")
			{
				nPos[0] = strRecv.Find("CONNECT=", 0);
				nPos[1] = strRecv.Find(",", nPos[0]);
				
				nLength = (nPos[1] - nPos[0]) - 8;
				
				strConnect	= "";
				strConnect = strRecv.Mid(nPos[0] + 8, nLength);

				if (strConnect == "0")
				{
					clsXgem.OnEqMachineOffLine();
				}
				else if(strConnect == "1")
				{
					clsXgem.OnEqMachineLocal();
				}
				else if(strConnect == "2")
				{
					clsXgem.OnEqMachineRemote();
				}
			}
			else if (strFunc == "ALARM")
			{
				clsXgem.OnEqAlarm(strRecv);
			}
			else if (strFunc == "CEID")
			{
				clsXgem.OnEqCEID(strRecv);
			}
			else if (strFunc == "HOST_TIME")
			{
				clsXgem.OnHostGetCurrentTime();
			}
			else if (strFunc == "RUN_STATUS")
			{
				nPos[0] = strRecv.Find("STATUS=", 0);
				nPos[1] = strRecv.Find(",", nPos[0]);
				
				nLength = (nPos[1] - nPos[0]) - 7;
				
				nStatus = atoi(strRecv.Mid(nPos[0] + 7, nLength));
				clsXgem.OnEqMachineStatus(nStatus);
			}
			else if (strFunc == "PPID_CHANGE")
			{
				nPos[0]				= strRecv.Find("PP_NAME=", 0);
				nPos[1]				= strRecv.Find(",", nPos[0]);
				nLength				= (nPos[1] - nPos[0]) - 8;
				clsXgem.m_strPPID	= strRecv.Mid(nPos[0] + 8, nLength);

				nPos[0]				= strRecv.Find("PP_PATH=", 0);
				nPos[1]				= strRecv.Find(",", nPos[0]);
				nLength				= (nPos[1] - nPos[0]) - 8;
				strPath				= strRecv.Mid(nPos[0] + 8, nLength);

				fp = fopen ( strPath, "r" );
				
				if ( fp == (FILE *)NULL ) 
				{
					return ;
				}

				clsXgem.m_strPPBody = "";
				while ( !feof(fp) )
				{
					if ( fgets (chBody, 100, fp ) == NULL )
					{
						break;
					}
					clsXgem.m_strPPBody += chBody;
				}
				fclose(fp);

				if(clsXgem.OnEqPPIDLoad(clsXgem.m_strPPID) == CTLBD_RET_GOOD)
				{
					sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] S7F1 SEND SUCCESS");
				}
				else
				{
					sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] S7F1 SEND FAIL");
				}
				::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
			}

			sprintf(st_server_info[0].ch_send, strRecv);
			::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);
		}
	}
}

CString CXgemInterfaceDlg::OnEqToXgemFunction(CString strFind, CString strRecv)
{
	int		nPos[2];
	int		nLength;
	CString strData;

	nPos[0] = strRecv.Find(strFind, 0);
	nPos[1] = strRecv.Find(",", nPos[0]+1);

	nLength	= (nPos[1] - nPos[0]) - (strFind.GetLength()+1);
	strData = strRecv.Mid(nPos[0]+strFind.GetLength()+1, nLength);

	return strData;
}

void CXgemInterfaceDlg::OneGEMReqRemoteCommandExgemctrlGem(long nMsgId, LPCTSTR sRcmd, long nCount, BSTR FAR* psNames, BSTR FAR* psVals) 
{
	// TODO: Add your control notification handler code here
	// RCMD MESSAGE 확인
	int i;
	long nResult = 0;
	CString strRecv, strTmp;
	CString strName, strValue;

	strRecv = "";
	for(i=0; i<nCount; i++)
	{
		strName		= psNames[i];
		strValue	= psVals[i];
		strTmp.Format("REP_%02d=[%s,%s],", i+1, strName, strValue);
		strRecv += strTmp;
	}


	clsXgem.OnHostRCMD(nMsgId, nCount, sRcmd, strRecv);
}

void CXgemInterfaceDlg::OneGEMTerminalMessageExgemctrlGem(long nTid, LPCTSTR sMsg) 
{
	// TODO: Add your control notification handler code here
	// OP CALL MESSAGE 확인 
	CString strRecv;

	strRecv = "FUNC=OPCALL,MSG=";
	strRecv += sMsg;
	strRecv += ",";
	sprintf(st_server_info[0].ch_send, strRecv);
	::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);
}

void CXgemInterfaceDlg::OneGEMTerminalMultiMessageExgemctrlGem(long nTid, long nCount, BSTR FAR* psMsg) 
{
	// TODO: Add your control notification handler code here
	int i;
	CString strRecv, strTmp;
	
	strRecv = "";
	for(i=0; i<nCount; i++)
	{
		strTmp.Format("MSG_%02d=", i+1);
		strRecv += strTmp;
		strRecv += psMsg[i]; 
		strRecv += ","; 
	}

	sprintf(st_server_info[0].ch_send, strRecv);
	::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);
}

void CXgemInterfaceDlg::OneGEMErrorEventExgemctrlGem(long nErrorCode) 
{
	// TODO: Add your control notification handler code here

}

void CXgemInterfaceDlg::OneGEMRspGetDateTimeExgemctrlGem(LPCTSTR sSystemTime) 
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME Time; 
	CString strTime;

	strTime = sSystemTime;

	::GetLocalTime(&Time);	

	Time.wYear		= atoi(strTime.Mid(0, 4));
	Time.wMonth		= atoi(strTime.Mid(4, 2));
	Time.wDay		= atoi(strTime.Mid(6, 2));
	Time.wHour		= atoi(strTime.Mid(8, 2));
	Time.wMinute	= atoi(strTime.Mid(10, 2));
	Time.wSecond	= atoi(strTime.Mid(12, 2));

	::SetLocalTime(&Time);
}

void CXgemInterfaceDlg::OneGEMReqDateTimeExgemctrlGem(long nMsgId, LPCTSTR sSystemTime) 
{
	// TODO: Add your control notification handler code here
	clsXgem.OnHostSetCurrentTime(nMsgId, 0);
}

void CXgemInterfaceDlg::OneGEMReqGetDateTimeExgemctrlGem(long nMsgId) 
{
	// TODO: Add your control notification handler code here
	clsXgem.OnHostGetCurrentTime();
}

void CXgemInterfaceDlg::OnBtnHide() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
}

void CXgemInterfaceDlg::OnListNormalMsgDisplay()
{
	int nIndex;	
	CString mstr_msg =  "";
	CTime time = CTime::GetCurrentTime();
	
	//2011.0504 추가 일정 시간 이상일때만 메시지 내용 확인하여 파일로 저장 
	if (m_strOldMsg == st_msg_info.c_normal_msg)//st_other.str_abnormal_msg) //2011.0829 test  if(st_other.str_old_abnormal_msg == st_other.str_abnormal_msg)
	{
		//2011.0504 추가 같은 메세지가 나오는 문제 처리 
		m_lNormalTime[1] = GetCurrentTime();
		m_lNormalTime[2] = m_lNormalTime[1] - m_lNormalTime[0];
		
		if (m_lNormalTime[2] > 5000) 
		{
			//OK 파일로 내용 저장 
		}
		else
		{
			m_lNormalTime[0] = GetCurrentTime();
			return;
		}
	}
	
	if (st_msg_info.c_normal_msg != _T(""))
	{
		m_lNormalTime[0] = GetCurrentTime();
		
		m_strOldMsg =	st_msg_info.c_normal_msg;
		mstr_msg.Format("[%04d:%02d:%02d:%02d:%02d:%02d] %s", time.GetYear(),
														time.GetMonth(),
														time.GetDay(),
														time.GetHour(),
														time.GetMinute(), 
														time.GetSecond(),
														st_msg_info.c_normal_msg);
	}
	else
	{
		mstr_msg = "Display Message is Empty";
	}
	
	OnLogFileSave(st_msg_info.c_normal_msg);
	nIndex = m_list_data.AddString(mstr_msg, RGB(0, 0, 255));  // 현재 입력된 정보 리스트 박스 컨트롤에 추가 
	m_list_data.SetCurSel(nIndex);
}

LRESULT CXgemInterfaceDlg:: OnListDataUpdate(WPARAM wParam,LPARAM lParam)
{
	OnListMessageClear();					// 리스트 박스 컨트롤에 추가된 모든 데이터 삭제 함수
	switch(wParam)
	{
		case NORMAL_MSG:						// 정상 동작
			OnListNormalMsgDisplay();
			break;
	}

	return 0;
}

void CXgemInterfaceDlg::OnListMessageClear()
{
	int nIndex = m_list_data.GetCount();
	
	if (nIndex < 100) return;
	
	m_list_data.ResetContent();   // 리스트 박스 컨트롤에 추가된 모든 데이터 삭제 
}

void CXgemInterfaceDlg::OneGEMReportedEventExgemctrlGem(long nEventID) 
{
	// TODO: Add your control notification handler code here
	int a;

	a = 0 ;
}

void CXgemInterfaceDlg::OneGEMReqOfflineExgemctrlGem(long nMsgId, long nFromState, long nToState) 
{
	// TODO: Add your control notification handler code here
}

void CXgemInterfaceDlg::OneGEMReqOnlineExgemctrlGem(long nMsgId, long nFromState, long nToState) 
{
	// TODO: Add your control notification handler code here
	m_hXgem.GEMRspOnline(nMsgId, 1);
}

void CXgemInterfaceDlg::OneGEMECVChangedExgemctrlGem(long nCount, long FAR* pnEcIds, BSTR FAR* psVals) 
{
	// TODO: Add your control notification handler code here
	int a;
	
	a = 0 ;
	AfxMessageBox("3");
}

void CXgemInterfaceDlg::OneGEMReqChangeECVExgemctrlGem(long nMsgId, long nCount, long FAR* pnEcIds, BSTR FAR* psVals) 
{
	// TODO: Add your control notification handler code here
	int a;
	
	a = 0 ;
	AfxMessageBox("4");
}

void CXgemInterfaceDlg::OneSECSMessageReceivedExgemctrlGem(long nObjectID, long nStream, long nFunction, long nSysbyte) 
{
	// TODO: Add your control notification handler code here
	clsXgem.OnXgemMessage(nObjectID, nStream, nFunction, nSysbyte);
}

void CXgemInterfaceDlg::OneGEMReqPPLoadInquireExgemctrlGem(long nMsgId, LPCTSTR sPpid, long nLength) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPLoadInquireExgemctrlGem(LPCTSTR sPpid, long nResult) 
{
	// TODO: Add your control notification handler code here
	if (m_strPPID == sPpid)
	{
		if (nResult == 0)
		{
			clsXgem.OnEqPPIDSend(sPpid, m_strPPBody);
		}
	}
}

void CXgemInterfaceDlg::OneGEMReqPPSendExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sBody) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPSendExgemctrlGem(LPCTSTR sPpid, long nResult) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nResult;
}

void CXgemInterfaceDlg::OneGEMReqPPExgemctrlGem(long nMsgId, LPCTSTR sPpid) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPExgemctrlGem(LPCTSTR sPpid, LPCTSTR sBody) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMReqPPDeleteExgemctrlGem(long nMsgId, long nCount, BSTR FAR* psPpid) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMReqPPListExgemctrlGem(long nMsgId) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMReqPPFmtSendExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPFmtSendExgemctrlGem(LPCTSTR sPpid, long nResult) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMReqPPFmtExgemctrlGem(long nMsgId, LPCTSTR sPpid) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPFmtExgemctrlGem(LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMRspPPFmtVerificationExgemctrlGem(LPCTSTR sPpid, long nResult) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMRspAllECInfoExgemctrlGem(long lCount, long FAR* plVid, BSTR FAR* psName, BSTR FAR* psValue, BSTR FAR* psDefault, BSTR FAR* psMin, BSTR FAR* psMax, BSTR FAR* psUnit) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMReqPPSendExExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sRecipePath) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPExExgemctrlGem(LPCTSTR sPpid, LPCTSTR sRecipePath) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMReqPPExExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sRecipePath) 
{
	// TODO: Add your control notification handler code here
	long id;

	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPSendExExgemctrlGem(LPCTSTR sPpid, LPCTSTR sRecipePath, long nResult) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMRspLoopbackExgemctrlGem(long nCount, long FAR* pnAbs) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMNotifyPerformanceWarningExgemctrlGem(long nLevel) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OneGEMSecondaryMsgReceivedExgemctrlGem(long nS, long nF, long nSysbyte, LPCTSTR sParam1, LPCTSTR sParam2, LPCTSTR sParam3) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nSysbyte;
}

void CXgemInterfaceDlg::OneGEMReqPPFmtSend2ExgemctrlGem(long nMsgId, LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCoode, long FAR* pnParamCount, BSTR FAR* psParamNames, BSTR FAR* psParamValues) 
{
	// TODO: Add your control notification handler code here
	long id;
	
	id = nMsgId;
}

void CXgemInterfaceDlg::OneGEMRspPPFmt2ExgemctrlGem(LPCTSTR sPpid, LPCTSTR sMdln, LPCTSTR sSoftRev, long nCount, BSTR FAR* psCCode, long FAR* pnParamCount, BSTR FAR* psParamNames, BSTR FAR* psParamValues) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OnSecsEventExcomctrl31(short nEventId, long lParam) 
{
	// TODO: Add your control notification handler code here
	
}

void CXgemInterfaceDlg::OnXgemVariable()
{
/*	int nCeid, nReport;

	clsXgem.m_strEqID					= "ES";

	nCeid	= 14;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 206;
	clsXgem.m_nVid[nCeid][nReport][1]	= 207;

	nCeid	= 17;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 204;
	
	nCeid	= 1001;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 1;
	clsXgem.m_nVid[nCeid][nReport][1]	= 2101;
	
	nCeid	= 1002;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2109;
	
	nCeid	= 1003;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2110;
	
	nCeid	= 1007;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2104;
	clsXgem.m_nVid[nCeid][nReport][1]	= 2105;
	
	nCeid	= 1015;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2101;
	clsXgem.m_nVid[nCeid][nReport][1]	= 2104;
	clsXgem.m_nVid[nCeid][nReport][2]	= 2105;
	
	nCeid	= 1300;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2101;
	clsXgem.m_nVid[nCeid][nReport][1]	= 2103;
	clsXgem.m_nVid[nCeid][nReport][2]	= 2701;
	clsXgem.m_nVid[nCeid][nReport][3]	= 2702;
	clsXgem.m_nVid[nCeid][nReport][4]	= 2709;
	clsXgem.m_nVid[nCeid][nReport][5]	= 2710;
	clsXgem.m_nVid[nCeid][nReport][6]	= 2711;
	
	nCeid	= 1303;
	nReport	= 0;
	clsXgem.m_nVid[nCeid][nReport][0]	= 2101;
	clsXgem.m_nVid[nCeid][nReport][1]	= 2103;
	clsXgem.m_nVid[nCeid][nReport][2]	= 2701;
	clsXgem.m_nVid[nCeid][nReport][3]	= 2702;
	clsXgem.m_nVid[nCeid][nReport][4]	= 2709;
	clsXgem.m_nVid[nCeid][nReport][5]	= 2710;
	clsXgem.m_nVid[nCeid][nReport][6]	= 2711;*/

	int i, j, k;
	int nCeid;
	int nCeidCnt;
	int nReportCnt;
//	int nVid;
	int nVidCnt;
	char chdata[100];

	CString strPath = "C:\\XGEM\\XGEM_DATA_1.TXT";
	CString strTmp;
	CString strCeid;
	CString strReport;
	CString strVid;

	:: GetPrivateProfileString("MACHINE", "EQP_ID", "", chdata, 100, strPath);
	clsXgem.m_strEqID = chdata;

	:: GetPrivateProfileString("CEID", "CEID_CNT", "", chdata, 100, strPath);
	nCeidCnt = atoi(chdata);

	for(i=0; i<nCeidCnt; i++)
	{
		strCeid.Format("CEID_%02d", i+1);
		:: GetPrivateProfileString(strCeid, "CEID_NUM", "", chdata, 100, strPath);
		nCeid = atoi(chdata);

		:: GetPrivateProfileString(strCeid, "REPORT", "", chdata, 100, strPath);
		nReportCnt = atoi(chdata);
		
		for(j=0; j<nReportCnt; j++)
		{
			strTmp.Format("REPORT_%02d_VID_CNT", j+1);
			:: GetPrivateProfileString(strCeid, strTmp, "", chdata, 100, strPath);
			nVidCnt = atoi(chdata);
			
			for(k=0; k<nVidCnt; k++)
			{
				strTmp.Format("REPORT_%02d_VID_%03d", j+1, k+1);
				:: GetPrivateProfileString(strCeid, strTmp, "", chdata, 100, strPath);
				clsXgem.m_nVid[nCeid][j][k] = atoi(chdata);
			}
		}
	}
}

void CXgemInterfaceDlg::OnLogFileSave(CString strLog)
{
	CString str_display_time;	// 파일 출력용 시간 저장 변수
	CString str_msg;		// 발생한 알람 메시지 저장 변수
	int n_cur_hour;				// 현재 시간 저장 변수
	
	COleDateTime cur_time;
	CTime alarm_occured_time;
	
	CString str_file_month, str_file_day, str_title_time;
	int n_cur_year, n_cur_month, n_cur_day;		// 년, 월, 일 저장 변수
	
	CString str_month_path, str_day_path;
	
	FILE *fp;
	CString Load_file = _T("");
	
	cur_time = COleDateTime::GetCurrentTime();
	n_cur_hour = cur_time.GetHour();
	
	alarm_occured_time = CTime::GetCurrentTime();
	str_display_time = alarm_occured_time.Format("%c");
	// **************************************************************************

	// **************************************************************************
	// 일별, 월별 알람 발생 횟수 임시 파일에 추가한다                            
	// **************************************************************************
	//	mcls_alarm_list.On_Daily_ListFile_Add(str_alarm_code, str_display_time, str_alarm_msg); // 일별, 월별 알람 정보 리스트 파일에 추가 함수
	// **************************************************************************
	
	// **************************************************************************
	// 바로 전에 발생한 알람 Code와 비교하여 동일한 경우 알람 카운트 미진행      
	// -> 알람 카운트 시간 간격은 30초로 설정한다                                
	// **************************************************************************
	
	// **************************************************************************
	// 파일 이름으로 사용할 날짜 설정한다                                        
	// **************************************************************************
	cur_time = COleDateTime::GetCurrentTime();

	n_cur_year = cur_time.GetYear();  
	n_cur_month = cur_time.GetMonth();  
    n_cur_day = cur_time.GetDay();  
	
	str_file_month.Format("%04d%02d", n_cur_year, n_cur_month);
	str_file_day.Format("%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
	str_title_time = alarm_occured_time.Format("%Y/%m/%d %H:%M:%S");
	// **************************************************************************
	
	str_month_path = _T("C:\\XGEM\\Log\\CIM\\Monthly\\MT" + str_file_month + ".TXT");
	str_day_path = _T("C:\\XGEM\\Log\\CIM\\Daily\\DY" + str_file_day + ".TXT");
	
	// 월 데이타 기록
	if ((fp = fopen(str_month_path, "a+")) == NULL)  
	{
		AfxMessageBox("The failure because we open the file.");
		return;
	}
	
	fprintf(fp, "[%s] %s \r\n", str_title_time, strLog);	
	fclose(fp);
	// 일 데이타 기록
	if ((fp = fopen(str_day_path, "a+")) == NULL)  
	{
		AfxMessageBox("The failure because we open the file.");
		return;
	}
	
	fprintf(fp, "[%s] %s \r\n", str_title_time, strLog);	
	fclose(fp);
}

BOOL CXgemInterfaceDlg::CreateFolder(LPCTSTR szPath)
{
	int nRet = 0;
	
	CString strPath;// = szPath;
	
	strPath.Format("%s", szPath);
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

void CXgemInterfaceDlg::OnButton1() 
{	
	
	CString strPath = "C:\\XGEM\\EqSample_1.cfg";
	
	CString strFunc = "FUNC=COMMUNICATION,CONFIG=" + strPath + ",";

	clsXgem.OnEqCommunication(&m_hXgem, strFunc);	
}
