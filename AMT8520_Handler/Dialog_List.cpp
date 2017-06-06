// Dialog_List.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_List.h"
#include <atlconv.h>  // TCP/IP 관련 클래스 추가

#include "Public_Function.h"
#include "SrcBase\ACriticalSection.h"
#include "SrcPart\/APartNetworkManager.h"

#include "Run_Network.h"
#include "AlgMemory.h"
// ******************************************************************************
// 네트워크 통신 방식 정의                                                       
// ******************************************************************************
const int SOCK_TCP = 0;
const int SOCK_UDP = 1;
// ******************************************************************************

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMutex mutexlog;

/////////////////////////////////////////////////////////////////////////////
// CDialog_List dialog


CDialog_List::CDialog_List()
{
	//{{AFX_DATA_INIT(CDialog_List)
	//}}AFX_DATA_INIT
	int i;

	CString mstr_temp;			// 저장할 정보 임시 저장 변수 
	char chr_data[50];
	mstr_normal_old_msg = "";
	mstr_abnormal_old_msg = "";
	
	for (i = 0; i < MAX_SERVER_NUM; i++)
	{
		mp_server[i] = NULL;
		mp_client[i] = NULL;
	}

	ml_listmsg_write_time = 10000;

	st_path.mstr_basic =			_T("c:\\AMT8520\\Setting\\BasicTerm.TXT");		// 환경 설정 정보 저장 파일에 대한 폴더 설정 
	
	:: GetPrivateProfileString("ADMIN", "FMachine_Server_Port", "9999", chr_data, 10, st_path.mstr_basic);
	mstr_temp = chr_data;
	mstr_temp.MakeUpper();     // 문자열 대문자로 변경
	mstr_temp.TrimLeft(' ');   // 좌측 문자열 공백 제거              
	mstr_temp.TrimRight(' ');  // 우측 문자열 공백 제거
//	Run_Network.m_Net_FMachine.SetPort( atoi((LPCSTR)mstr_temp) );

	mn_port[0] = atoi(mstr_temp);
	
	for (i = 0; i < 10; i++)
	{
		mn_header[i] = 0;
		mstr_rpy_msg[i] = "";
		mstr_body[i] = "";

		mn_header_C[i] = 0;
		mstr_body_C[i] = "";
	}
	
}


void CDialog_List::DoDataExchange(CDataExchange* pDX)
{
	CInitDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_List)
	DDX_Control(pDX, IDC_MSG_EQP_ID, m_msg_eqp_id);
	DDX_Control(pDX, IDC_MSG_DEVICE_NAME, m_msg_device_name);
	DDX_Control(pDX, IDC_LABEL_EQP_ID, m_label_eqp_id);
	DDX_Control(pDX, IDC_LABEL_DEVICE_NAME, m_label_device_name);
	DDX_Control(pDX, IDC_LIST_DATA, m_list_data);
	DDX_Control(pDX, IDC_LIST_CLOCK, m_list_clock);

	////2015.0604
	DDX_Control(pDX, IDC_LABEL_FTP, m_label_ftp);
	DDX_Control(pDX, IDC_MSG_FTP, m_msg_ftp);
	////

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_List, CInitDialogBar)
	//{{AFX_MSG_MAP(CDialog_List)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_LIST_DATA, OnDblclkListData)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LIST_DATA, OnListDataUpdate)  // 수신 메시지 리스트 박스 컨트롤에 추가 메시지 선언  
	ON_MESSAGE(WM_SOCKET_ACCEPT, OnSocketAccept)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_List message handlers

BOOL CDialog_List::OnInitDialogBar() 
{
	CInitDialogBar::OnInitDialogBar();
	
	// 컨트롤에 설정할 폰트 생성한다
// 	mp_list_font = NULL;
// 	mp_list_font = new CFont;
// 	mp_list_font->CreateFont(14,6,0,0,900,0,0,0,0,0,0,PROOF_QUALITY,0,"MS Sans Serif");
	
	OnList_Digital_Clock_Set();				// 시계 생성.
	OnList_Check_Tab_Stops(TRUE);			// Tab 사용 여부 설정 함수
	OnList_Lable_Set();						// 기타 정보에 대한 Label 설정 함수

	g_Net.OnStartServer();
	g_Net.OnInitClientAddress();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_List::OnDestroy() 
{
	int i;

	// 생성한 폰트 정보 삭제한다
// 	delete mp_list_font;
// 	mp_list_font = NULL;

	for (i = 0; i < MAX_SERVER_NUM; i++)
	{
		if (mp_server[i] != NULL)
		{
			delete mp_server[i];
			mp_server[i] = NULL;
		}
	}

	CInitDialogBar::OnDestroy();
}

void CDialog_List::OnList_Digital_Clock_Set()
{
	m_list_clock.Start(IDB_CLOCKST_PANE, IDB_CLOCKST_BIG, IDB_CLOCKST_SMALL);
}

void CDialog_List::OnList_Lable_Set()
{
	m_label_eqp_id.SetText("EQP ID");
	m_label_eqp_id.SetTextColor(RGB(0, 0, 0));
	m_label_eqp_id.SetBkColor(RGB(181, 182, 140));
	m_label_eqp_id.SetFontBold(TRUE);
	m_label_eqp_id.SetFontName("MS Sans Serif");
	m_label_eqp_id.SetFontSize(11);
	
	m_msg_eqp_id.SetFont(Func.mp_list_font);
	m_msg_eqp_id.SetCenterText();
	m_msg_eqp_id.SetColor(RGB(0,0,0));
	m_msg_eqp_id.SetGradientColor(RGB(0,0,0));
	m_msg_eqp_id.SetTextColor(RGB(255,255,255));
	
	m_label_device_name.SetText("DEVICE NAME");
	m_label_device_name.SetTextColor(RGB(0, 0, 0));
	m_label_device_name.SetBkColor(RGB(181, 182, 140));
	m_label_device_name.SetFontBold(TRUE);
	m_label_device_name.SetFontName("MS Sans Serif");
	m_label_device_name.SetFontSize(11);
	
	m_msg_device_name.SetFont(Func.mp_list_font);
	m_msg_device_name.SetCenterText();
	m_msg_device_name.SetColor(RGB(0,0,0));
	m_msg_device_name.SetGradientColor(RGB(0,0,0));
	m_msg_device_name.SetTextColor(RGB(255,255,255));

	////2015.0604
	m_label_ftp.SetText("FTP Mode");
	m_label_ftp.SetTextColor(RGB(0, 0, 0));
	m_label_ftp.SetBkColor(RGB(181, 182, 140));
	m_label_ftp.SetFontBold(TRUE);
	m_label_ftp.SetFontName("MS Sans Serif");
	m_label_ftp.SetFontSize(11);
	
	m_msg_ftp.SetFont(Func.mp_list_font);
	m_msg_ftp.SetCenterText();
	m_msg_ftp.SetColor(RGB(0,0,0));
	m_msg_ftp.SetGradientColor(RGB(0,0,0));
	m_msg_ftp.SetTextColor(RGB(255,255,255));
	////
}

void CDialog_List::OnList_Check_Tab_Stops(int mb_use_tab_stops)
{
	DWORD dwRemove = (mb_use_tab_stops ? 0 : LBS_USETABSTOPS);
	DWORD dwAdd = (mb_use_tab_stops ? LBS_USETABSTOPS : 0);
	
	m_list_data.ModifyStyle(dwRemove, dwAdd);
	m_list_data.RedrawWindow();
}

LRESULT CDialog_List:: OnListDataUpdate(WPARAM wParam,LPARAM lParam)
{
	//2013,1021
	CSingleLock sing(&mutexlog);
	sing.Lock();
	int n_mode = wParam;					// 시리얼 포트 정보 함수 변수에 설정 
	
	OnList_Message_Clear();					// 리스트 박스 컨트롤에 추가된 모든 데이터 삭제 함수
	
	switch(lParam)
	{
	case RECEIVE_MSG:						// 수신 메시지 
		OnList_Receive_Msg_Display(n_mode);	// 리스트 박스 컨트롤에 수신 메시지 추가 함수
		break;
		
	case SEND_MSG:							// 송신 메세지
		OnList_Send_Msg_Display(n_mode);	// 리스트 박스 컨트롤에 송신 메시지 추가 함수
		break;
		
	case NORMAL_MSG:						// 정상 동작
		OnList_Normal_Msg_Display();		// 리스트 박스 컨트롤에 정상 메시지 추가 함수
		break;
		
	case ABNORMAL_MSG:						// 비정상 동작
		OnList_Abnormal_Msg_Display();		// 리스트 박스 컨트롤에 오류 메시지 추가 함수
		break;		
		
	case MACHINE_INFO:						// 장비에 관한 정보
		OnList_Machine_Data_Set();
		break;
		
	case DEVICE_INFO:						// 디바이스에 관한 정보
		OnList_Machine_Data_Set();
		break;
		
	}
	
	sing.Unlock();
	return 0 ;
}

void CDialog_List::OnList_Message_Clear()
{
	int nIndex = m_list_data.GetCount();
	
	if (nIndex < 100) return;
	
	m_list_data.ResetContent();   // 리스트 박스 컨트롤에 추가된 모든 데이터 삭제 
}


void CDialog_List::OnList_Normal_Msg_Display()
{
	// List에 출력 하다가 죽는 현상을 막기 위해서... 2K11/09/01/ViboX
	int mn_index;	
	std::string mstr_msg =  "";
	CString sTime;
	
	SYSTEMTIME csTime;
	
	GetLocalTime(&csTime);
	
	csTime.wHour;
	csTime.wMinute;
	csTime.wSecond;
	csTime.wMilliseconds;
	
	CString strMsg;
	sTime.Format("[%02d:%02d:%02d.%03d] ", csTime.wHour, csTime.wMinute, csTime.wSecond, csTime.wMilliseconds);

	//  일정 시간 이상일때만 메시지 내용 확인하여 파일로 저장 
	strMsg = clsMem.OnNormalMessageRead();
	if (mstr_normal_old_msg == strMsg /*st_msg.c_normal_msg*/)
	{
		// 같은 메세지가 나오는 문제 처리 
		ml_list_msgterm_time_normal[1] = GetCurrentTime();
		ml_list_msgterm_time_normal[2] = ml_list_msgterm_time_normal[1] - ml_list_msgterm_time_normal[0];
		
		if (ml_list_msgterm_time_normal[2] > ml_listmsg_write_time) 
		{
			//OK 파일로 내용 저장 
		}
		else
		{
			ml_list_msgterm_time_normal[0] = GetCurrentTime();
			return;
		}
	}
	
	if (st_msg.c_normal_msg != _T(""))
	{
		ml_list_msgterm_time_normal[0] = GetCurrentTime();
		
		mstr_normal_old_msg =	st_msg.c_normal_msg;
		mstr_msg = sTime + st_msg.c_normal_msg;
	}
	else
	{
		mstr_msg = "Display Message is Empty";
	}
	
	mn_index = m_list_data.AddString(mstr_msg.c_str(), RGB(0, 0, 255));  // 현재 입력된 정보 리스트 박스 컨트롤에 추가 
	m_list_data.SetCurSel(mn_index);
	
	Func.On_LogFile_Add(99, st_msg.c_normal_msg);
}

void CDialog_List::OnList_Abnormal_Msg_Display()
{
	// List에 출력 하다가 죽는 현상을 막기 위해서... 2K11/09/01/ViboX
	int mn_index;
	std::string mstr_msg =  "";
	CString sTime;

	SYSTEMTIME csTime;

	GetLocalTime(&csTime);

	csTime.wHour;
	csTime.wMinute;
	csTime.wSecond;
	csTime.wMilliseconds;
	CString strMsg;

	sTime.Format("[%02d:%02d:%02d.%03d] ", csTime.wHour, csTime.wMinute, csTime.wSecond, csTime.wMilliseconds);
	
	strMsg = clsMem.OnAbNormalMessageRead();
	// 일정 시간 이상일때만 메시지 내용 확인하여 파일로 저장 
	if (mstr_abnormal_old_msg == strMsg /*st_msg.c_abnormal_msg*/)
	{
		// 같은 메세지가 나오는 문제 처리 
		ml_list_msgterm_time_abnormal[1] = GetCurrentTime();
		ml_list_msgterm_time_abnormal[2] = ml_list_msgterm_time_abnormal[1] - ml_list_msgterm_time_abnormal[0];
		
		if (ml_list_msgterm_time_abnormal[2] > ml_listmsg_write_time) 
		{
			//OK 파일로 내용 저장 
		}
		else
		{
			ml_list_msgterm_time_abnormal[0] = GetCurrentTime();
			return;
		}
	}
	
	if (st_msg.c_abnormal_msg != _T(""))
	{
		ml_list_msgterm_time_abnormal[0] = GetCurrentTime();
		
		mstr_abnormal_old_msg =	st_msg.c_abnormal_msg;
		mstr_msg   =	sTime + st_msg.c_abnormal_msg;
	}
	else
	{
		mstr_msg = "Display Message is Empty";
	}
	
	mn_index = m_list_data.AddString(mstr_msg.c_str(), RGB(255, 0, 0));  // 현재 입력된 정보 리스트 박스 컨트롤에 추가 
	m_list_data.SetCurSel(mn_index);
	
	Func.On_LogFile_Add(99, st_msg.c_abnormal_msg);
}

void CDialog_List::OnList_Send_Msg_Display(int n_mode)
{
	int mn_index;				// 리스트 박스에 추가된 정보의 위치 저장 변수 
	CString s;					// 시스템 시간 정보 임시 저장 변수 
	CString mstr_name;			// 시리얼 포트 및 시간 정보 저장 변수 
	CString mstr_msg = "\t";	// 리스트 박스에 추가할 정보 저장 변수 
	CTime mp_cur_time;			// 현재 시간 정보 저장 변수 

	CString str_send;

	/* ************************************************************************** */
	/* 시리얼 포트 및 메시지 종류에 대한 정보 생성 부분                           */
	/* ************************************************************************** */
	str_send.Format("SND     Com %02d   : ", n_mode);

	mp_cur_time = CTime::GetCurrentTime() ;  // 현재 시간 정보 얻는다. 
	s = mp_cur_time.Format("%p %H %M %S") ;  // 시간 정보 포맷 변경 (오전/오후 , 시간 , 분 , 초 )

	mstr_name = str_send + s;  // 추가할 타이틀 정보 설정 

	mn_index = m_list_data.AddString(mstr_name, RGB(0, 0, 255));  // 리스트 박스 컨트롤에 타이틀 정보 추가 
	m_list_data.SetCurSel(mn_index);

	st_msg.mstr_send = st_serial.mcomm_snd[n_mode-1];
	if (st_msg.mstr_send != _T(""))  mstr_msg += st_msg.mstr_send;
	else   mstr_msg += "Display Message is Empty";

	Func.On_LogFile_Add(1, mstr_msg);
	Func.On_LogFile_Add(99, mstr_msg);

	mn_index = m_list_data.AddString(mstr_msg, RGB(50, 100, 150));  // 현재 입력된 정보 리스트 박스 컨트롤에 추가 
	m_list_data.SetCurSel(mn_index);
}

void CDialog_List::OnList_Receive_Msg_Display(int n_mode)
{
	int mn_index;		// 리스트 박스에 추가된 정보의 위치 저장 변수 
	CString s;			// 시스템 시간 정보 임시 저장 변수 
	CString mstr_name;  // 시리얼 포트 및 시간 정보 저장 변수 
	CString mstr_msg = "\t";  // 리스트 박스에 추가할 정보 저장 변수 
	CTime mp_cur_time;  // 현재 시간 정보 저장 변수 

	CString str_rcv;
	
	/* ************************************************************************** */
	/* 시리얼 포트 및 메시지 종류에 대한 정보 생성 부분                           */
	/* ************************************************************************** */
	str_rcv.Format("RCV     Com %02d   : ", n_mode);
	
	mp_cur_time = CTime::GetCurrentTime() ;  // 현재 시간 정보 얻는다. 
	s = mp_cur_time.Format("%p %H %M %S") ;  // 시간 정보 포맷 변경 (오전/오후 , 시간 , 분 , 초 )
	
	mstr_name = str_rcv + s;  // 추가할 타이틀 정보 설정 
	
	mn_index = m_list_data.AddString(mstr_name, RGB(255, 0, 255));  // 리스트 박스 컨트롤에 타이틀 정보 추가 
	m_list_data.SetCurSel(mn_index);
	
	if (st_msg.mstr_recive != _T(""))  mstr_msg += st_msg.mstr_recive;
	else   mstr_msg += "Display Message is Empty";

	Func.On_LogFile_Add(1, mstr_msg);
	Func.On_LogFile_Add(99, mstr_msg);

	mn_index = m_list_data.AddString(mstr_msg, RGB(50, 100, 150));  // 현재 입력된 정보 리스트 박스 컨트롤에 추가 
	m_list_data.SetCurSel(mn_index);
}

//////////////////////////// send //////////////////////////////////////////////////////

void CDialog_List::OnList_Machine_Data_Set()
{
	m_msg_eqp_id.SetWindowText(_T(st_lamp.mstr_equip_id));		// 장비 호기 정보 출력 
	m_msg_device_name.SetWindowText(_T(st_basic.mstr_device_name));		// DEVICE 정보 출력

	m_msg_ftp.SetWindowText(_T(st_basic.mstr_mode_ftp));//2015.0604
}

void CDialog_List::OnDblclkListData() 
{
	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 

	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   s
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 
	
	mn_cur_year = time_cur.GetYear();  
	mn_cur_month = time_cur.GetMonth();  
    mn_cur_day = time_cur.GetDay();  
	
	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
	mstr_cur_year.Format("%04d", mn_cur_year);
	mstr_cur_month.Format("%02d", mn_cur_month);
	mstr_cur_day.Format("%02d", mn_cur_day);
	
	mstr_file_name = "TOT" + mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
	mstr_create_file = st_path.mstr_total + mstr_file_name;
	mstr_create_file += ".TXT";

	::ShellExecute(NULL, NULL, "NotePAD.exe", mstr_create_file, NULL, SW_SHOWNORMAL);// TODO: Add your control notification handler code here
}

LRESULT CDialog_List::OnSocketAccept( WPARAM wParam,LPARAM lParam )
{
	g_Net.ClientAccept( wParam );
	return 0;
}
