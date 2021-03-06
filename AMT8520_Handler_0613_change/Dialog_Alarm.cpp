// Dialog_Alarm.cpp : implementation file
//

#include "stdafx.h"
#include "Handler.h"
#include "Dialog_Alarm.h"

#include "MainFrm.h"	// MainFrm에 선언한 알람 클래스 포인터 정보 얻기 위하여 추가 
#include "io.h" 

#include "Srcbase\ALocalization.h"
#include "ComizoaPublic.h"

#include "AClient_Xgem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialog_Alarm dialog


CDialog_Alarm::CDialog_Alarm(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog_Alarm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog_Alarm)
	//}}AFX_DATA_INIT
}


void CDialog_Alarm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialog_Alarm)
	DDX_Control(pDX, IDC_MSG_ALARM_TIME, m_msg_alarm_time);
	DDX_Control(pDX, IDC_DATA_ALARM_TIME, m_data_alarm_time);
	DDX_Control(pDX, IDC_MSG_LOT_END, m_msg_lot_end);
	DDX_Control(pDX, IDC_MSG_ALARM_TYPE, m_msg_alarm_type);
	DDX_Control(pDX, IDC_MSG_ALARM_TITLE, m_msg_alarm_title);
	DDX_Control(pDX, IDC_MSG_ALARM_REPARE, m_msg_alarm_repare);
	DDX_Control(pDX, IDC_MSG_ALARM_CODE, m_msg_alarm_code);
	DDX_Control(pDX, IDC_IMAGE_ALARM, m_image_alarm);
	DDX_Control(pDX, IDC_GROUP_PREV_ALARM, m_group_prev_alarm);
	DDX_Control(pDX, IDC_GROUP_ALARM_INFO, m_group_alarm_info);
	DDX_Control(pDX, IDC_GIF_ALARM, m_gif_alarm);
	DDX_Control(pDX, IDC_DATA_ALARM_TYPE, m_data_alarm_type);
	DDX_Control(pDX, IDC_DATA_ALARM_CODE, m_data_alarm_code);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_10, m_btn_prev_alarm_10);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_9, m_btn_prev_alarm_9);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_8, m_btn_prev_alarm_8);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_7, m_btn_prev_alarm_7);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_6, m_btn_prev_alarm_6);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_5, m_btn_prev_alarm_5);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_4, m_btn_prev_alarm_4);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_3, m_btn_prev_alarm_3);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_2, m_btn_prev_alarm_2);
	DDX_Control(pDX, IDC_BTN_PREV_ALARM_1, m_btn_prev_alarm_1);
	DDX_Control(pDX, IDC_BTN_BUZZER_OFF, m_btn_buzzer_off);
	DDX_Control(pDX, IDOK, m_btn_alarm_ok);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialog_Alarm, CDialog)
	//{{AFX_MSG_MAP(CDialog_Alarm)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_BUZZER_OFF, OnBtnBuzzerOff)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_1, OnBtnPrevAlarm1)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_2, OnBtnPrevAlarm2)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_3, OnBtnPrevAlarm3)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_4, OnBtnPrevAlarm4)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_5, OnBtnPrevAlarm5)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_6, OnBtnPrevAlarm6)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_7, OnBtnPrevAlarm7)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_8, OnBtnPrevAlarm8)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_9, OnBtnPrevAlarm9)
	ON_BN_CLICKED(IDC_BTN_PREV_ALARM_10, OnBtnPrevAlarm10)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialog_Alarm message ICTs

BOOL CDialog_Alarm::Create(CWnd* pParentWnd) 
{
	return CDialog::Create(IDD, pParentWnd);
}

void CDialog_Alarm::PostNcDestroy() 
{
	((CMainFrame*)AfxGetMainWnd())->mp_alarm_dlg = NULL ; // 알람 화면 핸들 정보 초기화 
	delete  this ;
	
	CDialog::PostNcDestroy();  // Destroy 작업이 이루어지지 않으면 이 부분 주석 처리 
}

BOOL CDialog_Alarm::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTime	time;
	CString ocurr_date = _T("");
	
	// **************************************************************************
	// 컨트롤에 설정할 폰트 생성한다                                             
	// **************************************************************************
// 	mp_alarm_font = NULL;
// 	mp_alarm_font = new CFont;
// 	mp_alarm_font->CreateFont(15,0,0,0,900,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"MS Sans Serif");
	// **************************************************************************

	n_last_alarm = 0;
	time = CTime::GetCurrentTime();

	//발생한 알람을 List에 넣는다.
	if (alarm.mstr_code != "")
	{
//		time = alarm.stl_cur_alarm_time;
		ocurr_date.Format("%4d-%02d-%02d (%02d:%02d:%02d)", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

		alarm.mstr_list_jam.AddHead(alarm.mstr_code);			// 2K91228ViboX
		alarm.mstr_list_jam_time.AddTail(ocurr_date);
		
		n_last_alarm = alarm.mstr_list_jam.GetCount();
		
		if (n_last_alarm > 10)
		{
			alarm.mstr_list_jam.RemoveTail();
			alarm.mstr_list_jam_time.RemoveTail();
		}
	}
	else if (alarm.mstr_code == "")
	{
		n_last_alarm = alarm.mstr_list_jam.GetCount();
		
		if(n_last_alarm == 0)
		{
			alarm.mstr_code = _T("990000");
			sprintf(alarm.mc_code, "990000");

			ocurr_date.Format("%4d-%02d-%02d (%02d:%02d:%02d)", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
			alarm.mstr_list_jam.AddTail(alarm.mstr_code);
			alarm.mstr_list_jam_time.AddTail(ocurr_date);
		}
		else
		{
			alarm.mstr_code = alarm.mstr_list_jam.GetAt(alarm.mstr_list_jam.FindIndex(0));
			
			if(alarm.mstr_code == "")
			{
				alarm.mstr_code = _T("990000");
				sprintf(alarm.mc_code, "990000");

				ocurr_date.Format("%4d-%02d-%02d (%02d:%02d:%02d)", time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
				alarm.mstr_list_jam.AddTail(alarm.mstr_code);
				alarm.mstr_list_jam_time.AddTail(ocurr_date);
			}
		}
	}

	OnAlarm_OcurrDate_Display(1);

	// **************************************************************************
	// 현재 발생한 알람에 대한 메시지 정보 설정한다                              
	// **************************************************************************
	mcls_alarm_load.On_Alarm_Info_Set_to_Variable(alarm.mstr_code);  // 발생한 알람 메시지 전역 변수에 설정하는 함수
	// **************************************************************************
	
	OnAlarm_Default_Data_Load();	// 알람 화면 관련 기본 정보 설정 함수
	OnAlarm_PrevList_Set();			// 이전 알람 관련

	OnAlarm_Picture_Set();			// 알람 화면에 대한 GIF 그림 파일 로딩 함수
	OnAlarm_GroupBox_Set();			// 알람 화면에 대한 칼라 그룹 박스 생성 함수
	OnAlarm_Lable_Set();			// 알람 화면에 대한 칼라 텍스트 박스 생성 함수
	OnAlarm_LCD_Digital_Set();		// 알람 화면에 대한 LCD 디비털 텍스트 박스 생성 함수
	OnAlarm_EditBox_Set();			// 알람 화면에 대한 대한 칼라 에디트 박스 생성 함수

	OnAlarm_Controls_Show(FALSE);	// LOT END 메시지 출력 컨트롤 Show 제어 함수

	OnAlarm_Data_Loading();			// 현재 발생한 알람에 대한 그림 및 조치 사항 정보 로딩 함수
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialog_Alarm::OnDestroy() 
{
	int i = 0;
	// **************************************************************************
	// LCD 디지털 문자 스크롤 동작 정지한다                                      
	// **************************************************************************
	m_msg_lot_end.StopScroll();
	// **************************************************************************

	// **************************************************************************
	// 생성한 폰트 정보 삭제한다                                                 
	// **************************************************************************
// 	delete mp_alarm_font;
// 	mp_alarm_font = NULL;
	// **************************************************************************
	
	m_image_alarm.ClearDrawing();

	// RUN중에 알람화면을 확인했을 경우에는 STOP시키지 않는다.
	if (COMI.mn_run_status != dRUN)
	{
		//Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수

		if(st_handler.cwnd_title != NULL)  
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);
		}
	}

	alarm.mstr_code = "";

	st_handler.mn_alarm_screen = FALSE;  // 현재 알람 화면이 출력되지 않았다고 해당 플래그 설정 

	CDialog::OnDestroy();	
}

// ******************************************************************************
// ESC 키 누르면 화면 사라짐 방지하기 위하여 추가한 함수                         
// ******************************************************************************
void CDialog_Alarm::OnCancel() 
{
	return;
}
// ******************************************************************************

void CDialog_Alarm::OnAlarm_Picture_Set()
{
	if (m_gif_alarm.Load(MAKEINTRESOURCE(IDR_GIF_ALARM),_T("GIF")))
		m_gif_alarm.Draw();
}

void CDialog_Alarm::OnAlarm_GroupBox_Set()
{
	CSxLogFont alarm_font(15,FW_SEMIBOLD,false,"Arial");

	m_group_alarm_info.SetFont(alarm_font);
	m_group_alarm_info.SetCatptionTextColor(RGB(145,25,0));
	m_group_alarm_info.SetFontBold(TRUE);

	m_group_prev_alarm.SetFont(alarm_font);
	m_group_prev_alarm.SetCatptionTextColor(RGB(145,25,0));
	m_group_prev_alarm.SetFontBold(TRUE);
}

void CDialog_Alarm::OnAlarm_Lable_Set()
{
	m_msg_alarm_code.SetTextFont(Func.mp_alarm_font);
	m_msg_alarm_code.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 6, 4);
	m_msg_alarm_code.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_data_alarm_code.SetTextColor(RGB(25, 25, 85));
	m_data_alarm_code.SetBkColor(RGB(180, 230, 250));
	m_data_alarm_code.SetFontBold(TRUE);
	m_data_alarm_code.SetFontName("MS Sans Serif");
	m_data_alarm_code.SetFontSize(12);
	
	m_msg_alarm_type.SetTextFont(Func.mp_alarm_font);
	m_msg_alarm_type.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 6, 4);
	m_msg_alarm_type.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_data_alarm_type.SetTextColor(RGB(25, 25, 85));
	m_data_alarm_type.SetBkColor(RGB(180, 230, 250));
	m_data_alarm_type.SetFontBold(TRUE);
	m_data_alarm_type.SetFontName("MS Sans Serif");
	m_data_alarm_type.SetFontSize(12);

	m_msg_alarm_repare.SetTextFont(Func.mp_alarm_font);
	m_msg_alarm_repare.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 6, 4);
	m_msg_alarm_repare.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_msg_alarm_title.SetTextFont(Func.mp_alarm_font);
	m_msg_alarm_title.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 6, 4);
	m_msg_alarm_title.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_msg_alarm_time.SetTextFont(Func.mp_alarm_font);
	m_msg_alarm_time.SetColor(RGB(0, 0, 0), RGB(255, 255, 255), 6, 4);
	m_msg_alarm_time.SetTextAlign(CFloatST::STA_CENTER, 10);

	m_data_alarm_time.SetTextColor(RGB(25, 25, 85));
	m_data_alarm_time.SetBkColor(RGB(180, 230, 250));
	m_data_alarm_time.SetFontBold(TRUE);
	m_data_alarm_time.SetFontName("MS Sans Serif");
	m_data_alarm_time.SetFontSize(12);
}

void CDialog_Alarm::OnAlarm_LCD_Digital_Set()
{
	m_msg_lot_end.SetNumberOfLines(1);
	m_msg_lot_end.SetXCharsPerLine(16);
	m_msg_lot_end.SetSize(CMatrixStatic::LARGE);
	m_msg_lot_end.SetDisplayColors(RGB(0, 0, 0), RGB(255, 60, 0), RGB(103, 30, 0));
	m_msg_lot_end.AdjustClientXToSize(16);
	m_msg_lot_end.AdjustClientYToSize(1);
	m_msg_lot_end.SetAutoPadding(true, '!');
	m_msg_lot_end.DoScroll(300, CMatrixStatic::LEFT);
}

void CDialog_Alarm::OnAlarm_EditBox_Set()
{
	m_edit_alarm_title.SubclassDlgItem(IDC_EDIT_ALARM_TITLE, this);
	m_edit_alarm_title.bkColor(RGB(60, 140, 10));
	m_edit_alarm_title.textColor(RGB(255, 255, 255));
	m_edit_alarm_title.setFont(-12, FW_NORMAL, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");

	m_edit_alarm_repare.SubclassDlgItem(IDC_EDIT_ALARM_REPARE, this);
	m_edit_alarm_repare.bkColor(RGB(60, 140, 10));
	m_edit_alarm_repare.textColor(RGB(255, 255, 255));
	m_edit_alarm_repare.setFont(-12, FW_NORMAL, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
}

void CDialog_Alarm::OnAlarm_Default_Data_Load()
{
	// **************************************************************************
    // 알람 관련 정보 저장되어 있는 파일명 설정한다                              
    // **************************************************************************
	mstr_data_file = "JamData.TXT";					// 알람 출력 유형 정보 로딩 파일명 설정 

	switch( g_local.GetLocalType() )
	{
	case LOCAL_KOR:	mstr_action_file = "JamAction_Kor.TXT";	break;
	case LOCAL_ENG:	mstr_action_file = "JamAction_Eng.TXT";	break;
	case LOCAL_CHN:	mstr_action_file = "JamAction_Chn.TXT";	break;
	}
	// **************************************************************************
}

int CDialog_Alarm::OnAlarm_Data_Loading()
{
	CString str_chk_data;
	CString str_load_file;
	CString str_alarm_cur_msg;
	int n_pos;  

	if (alarm.mstr_code == "990000")
	{
		m_data_alarm_code.SetText(_T(alarm.mstr_pcode));  // 알람 코드 화면 출력
	}
	else
	{
		m_data_alarm_code.SetText(_T(alarm.mstr_code));  // 알람 코드 화면 출력
	}

	str_alarm_cur_msg = "\r\n";
	str_alarm_cur_msg += " " + st_alarm.mstr_cur_msg;
	
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM_TITLE))->SetWindowText(_T(str_alarm_cur_msg));      // 알람 타이틀 메시지 화면 출력

	// **************************************************************************
	// 발생한 알람에 대한 모든 정보 로딩한다                                     
	// **************************************************************************
	str_chk_data = mcls_alarm_load.On_Alarm_Display_Info_Load(st_path.mstr_path_alarm, mstr_data_file, mstr_action_file, alarm.mstr_code);  // 발생 알람 정보 설정 함수
	n_pos = str_chk_data.Find("Trouble");
	if (n_pos>=0)  
	{
		((CEdit*)GetDlgItem(IDC_EDIT_ALARM_REPARE))->SetWindowText(str_chk_data);  // 에러 정보 화면 출력
		return FALSE;
	}
	// **************************************************************************
	int pos;	
	pos = st_alarm.mstr_bmp_file.Find(".");	
	if(pos < 0)		
	{		
		st_alarm.mstr_bmp_file = "no-image.BMP";		
    }
	// **************************************************************************
	// 발생한 알람 표시할 그림 파일 정보 로딩한다                                
	// **************************************************************************
	str_load_file = st_path.mstr_path_alarm + st_alarm.mstr_bmp_file;  // 출력할 그림 파일에 대한 [폴더]+[파일명] 설정

	m_image_alarm.ClearDrawing();  // 기존 출력 그림 파일 정보 삭제
	if (!m_image_alarm.LoadImage(str_load_file))  // 발생한 알람에 대한 그림 로딩
	{
		str_chk_data = "Picture File Name : "+ str_load_file;
		str_chk_data += "\r\n";
		str_chk_data += "Loading Fail";
		
		((CEdit*)GetDlgItem(IDC_EDIT_ALARM_REPARE))->SetWindowText(str_chk_data);  // 에러 정보 화면 출력
		return FALSE;
	}
	// **************************************************************************

	// **************************************************************************
	// 발생한 알람에 대한 부위를 화면에 표시한다                                 
	// **************************************************************************
	m_image_alarm.Invalidate();  // 화면 출력 생신

	OnAlarm_Image_Style_Set(st_path.mstr_path_alarm);  // 발생한 알람 부위 표시 방법 설정 함수
	OnAlarm_Data_Display(alarm.mn_type_mode);          // 발생한 알람에 대한 각종 정보 화면 출력 함수
	// **************************************************************************

	return TRUE;
}

void CDialog_Alarm::OnAlarm_Image_Style_Set(CString strPath)
{
	if (st_alarm.mstr_style == "NONE")
	{
		return;
	}
	else if (st_alarm.mstr_style == "CIRCLE") 
	{
		m_image_alarm.SetCircleStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, st_alarm.mn_Offset);
		m_image_alarm.SetLineStyle(st_alarm.mn_LineWidth, RGB(st_alarm.mn_R, st_alarm.mn_G, st_alarm.mn_B));
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_LEFT")
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_LEFT, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_RIGHT")
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_RIGHT, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_UP") 
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_UP, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_DOWN")
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_DOWN, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_LEFTUP")
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_LEFTUP, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_LEFTDOWN")
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_LEFTDOWN, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_RIGHTUP")   
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_RIGHTUP, strPath, st_alarm.mn_Offset);
		return;
	}
	else if (st_alarm.mstr_style == "ARROW_RIGHTDOWN") 
	{
		m_image_alarm.SetArrowStyle(st_alarm.mn_Cx, st_alarm.mn_Cy, CBmpImageST::JS_ARROW_RIGHTDOWN, strPath, st_alarm.mn_Offset);
		return;
	}
}

void CDialog_Alarm::OnAlarm_Data_Display(int n_alarm_state)
{
	CString str_state;  // 알람 상태 정보 저장 변수 

	switch(n_alarm_state)
	{
	case 0: 
		str_state = "Warning";
		break;

	case 1:
		str_state = "Running";
		break;

	case 2:
		str_state = "Stop";
		break;

	default:
		str_state = "Error";
		break;
	}

	m_data_alarm_type.SetText(_T(str_state));       // 알람 유형 화면 출력
	
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM_REPARE))->SetWindowText(_T(st_alarm.mstr_repair_msg));  // 알람 조치 사항 화면 출력

	if (COMI.mn_run_status == dLOTEND)			// LOT END
	{
		OnAlarm_Controls_Show(TRUE);				// LOT END 메시지 표시 컨트롤 화면 출력 제어 함수
	}
}

void CDialog_Alarm::OnAlarm_Controls_Show(int n_state)
{
	switch(n_state)
	{
	case 0 :
		m_msg_lot_end.ShowWindow(SW_HIDE);
		break;
	
	case 1: 
		m_msg_lot_end.ShowWindow(SW_SHOW);
		m_msg_lot_end.SetText(_T("Lot End"));
		break;
	default:
		break;
	}
}

void CDialog_Alarm::OnAlarm_PrevList_Set()
{
	int i, cnt = alarm.mstr_list_jam.GetCount();
	
	OnAlarm_PrevButton_List_Set(cnt);
	
	if (cnt > 10)	cnt = 10;
	
	for (i = 0; i < cnt; i++)
	{
		m_pstrJamCode[i] = alarm.mstr_list_jam.GetAt(alarm.mstr_list_jam.FindIndex(i));
	}
	
	OnAlarm_PrevList_Caption_Set(cnt);
}

void CDialog_Alarm::OnAlarm_PrevButton_List_Set(int n_cnt)
{
	if (n_cnt > 10)	n_cnt = 10;

	m_btn_prev_alarm_1.ShowWindow(FALSE);
	m_btn_prev_alarm_2.ShowWindow(FALSE);
	m_btn_prev_alarm_3.ShowWindow(FALSE);
	m_btn_prev_alarm_4.ShowWindow(FALSE);
	m_btn_prev_alarm_5.ShowWindow(FALSE);
	m_btn_prev_alarm_6.ShowWindow(FALSE);
	m_btn_prev_alarm_7.ShowWindow(FALSE);
	m_btn_prev_alarm_8.ShowWindow(FALSE);
	m_btn_prev_alarm_9.ShowWindow(FALSE);
	m_btn_prev_alarm_10.ShowWindow(FALSE);
	
	switch(n_cnt)
	{
	case 0:
		break;
	case 1:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		break;
	case 2:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		break;
	case 3:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		break;
	case 4:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		break;
	case 5:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		break;
	case 6:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		m_btn_prev_alarm_6.ShowWindow(TRUE);
		break;
	case 7:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		m_btn_prev_alarm_6.ShowWindow(TRUE);
		m_btn_prev_alarm_7.ShowWindow(TRUE);
		break;
	case 8:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		m_btn_prev_alarm_6.ShowWindow(TRUE);
		m_btn_prev_alarm_7.ShowWindow(TRUE);
		m_btn_prev_alarm_8.ShowWindow(TRUE);
		break;
	case 9:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		m_btn_prev_alarm_6.ShowWindow(TRUE);
		m_btn_prev_alarm_7.ShowWindow(TRUE);
		m_btn_prev_alarm_8.ShowWindow(TRUE);
		m_btn_prev_alarm_9.ShowWindow(TRUE);
		break;
	case 10:
		m_btn_prev_alarm_1.ShowWindow(TRUE);
		m_btn_prev_alarm_2.ShowWindow(TRUE);
		m_btn_prev_alarm_3.ShowWindow(TRUE);
		m_btn_prev_alarm_4.ShowWindow(TRUE);
		m_btn_prev_alarm_5.ShowWindow(TRUE);
		m_btn_prev_alarm_6.ShowWindow(TRUE);
		m_btn_prev_alarm_7.ShowWindow(TRUE);
		m_btn_prev_alarm_8.ShowWindow(TRUE);
		m_btn_prev_alarm_9.ShowWindow(TRUE);
		m_btn_prev_alarm_10.ShowWindow(TRUE);
		break;
	}
}

void CDialog_Alarm::OnAlarm_PrevList_Caption_Set(int n_cnt)
{
	SetDlgItemText(IDC_BTN_PREV_ALARM_1, _T(m_pstrJamCode[0]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_2, _T(m_pstrJamCode[1]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_3, _T(m_pstrJamCode[2]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_4, _T(m_pstrJamCode[3]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_5, _T(m_pstrJamCode[4]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_6, _T(m_pstrJamCode[5]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_7, _T(m_pstrJamCode[6]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_8, _T(m_pstrJamCode[7]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_9, _T(m_pstrJamCode[8]));
	SetDlgItemText(IDC_BTN_PREV_ALARM_10, _T(m_pstrJamCode[9]));
}

CString CDialog_Alarm::OnAlarm_PrevAlarm_Check(int n_cnt)
{
	int i, cnt = alarm.mstr_list_jam.GetCount();

	return m_pstrJamCode[n_cnt - 1];

	if (n_cnt > 10)	n_cnt = 10;
	
	for (i = 0; i < cnt; i++)
	{
		m_pstrJamCode[i] = alarm.mstr_list_jam.GetAt(alarm.mstr_list_jam.FindIndex(i));
	}
	
	return m_pstrJamCode[cnt - n_cnt];
}

void CDialog_Alarm::OnAlarm_PrevAlarm_Display(int n_alarm_pos)
{
	alarm.mstr_code = OnAlarm_PrevAlarm_Check(n_alarm_pos);
	OnAlarm_Controls_Show(FALSE);										// LOT END 메시지 출력 컨트롤 Show 제어 함수
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM_REPARE))->SetWindowText("");		// 에러 정보 화면 출력 초기화
	m_image_alarm.ClearDrawing();												// 기존 출력 그림 파일 정보 삭제
	m_data_alarm_type.SetText("");										// 알람 유형 화면 출력 초기화
	m_data_alarm_code.SetText("");										// 알람 코드 화면 출력 초기화
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM_TITLE))->SetWindowText("");      // 알람 타이틀 메시지 화면 출력 초기화
	((CEdit*)GetDlgItem(IDC_EDIT_ALARM_REPARE))->SetWindowText("");		// 알람 조치 사항 화면 출력 초기화

	// **************************************************************************
	// 현재 발생한 알람에 대한 메시지 정보 설정한다                              
	// **************************************************************************
	mcls_alarm_load.On_Alarm_Info_Set_to_Variable(alarm.mstr_code);		// 발생한 알람 메시지 전역 변수에 설정하는 함수
	// **************************************************************************

	OnAlarm_Data_Loading();												// 현재 발생한 알람에 대한 그림 및 조치 사항 정보 로딩 함수
	OnAlarm_OcurrDate_Display(n_alarm_pos);
}

void CDialog_Alarm::OnBtnBuzzerOff() 
{
	// **************************************************************************
	// 부저 상태를 Off로 변환시킨다                                              
	// **************************************************************************
//	Func.OnSet_IO_Port_Sound(IO_OFF);
	// **************************************************************************
}

void CDialog_Alarm::OnBtnPrevAlarm1() 
{
	OnAlarm_PrevAlarm_Display(1);
}

void CDialog_Alarm::OnBtnPrevAlarm2() 
{
	OnAlarm_PrevAlarm_Display(2);
}

void CDialog_Alarm::OnBtnPrevAlarm3() 
{
	OnAlarm_PrevAlarm_Display(3);
}

void CDialog_Alarm::OnBtnPrevAlarm4() 
{
	OnAlarm_PrevAlarm_Display(4);
}

void CDialog_Alarm::OnBtnPrevAlarm5() 
{
	OnAlarm_PrevAlarm_Display(5);
}

void CDialog_Alarm::OnBtnPrevAlarm6() 
{
	OnAlarm_PrevAlarm_Display(6);
}

void CDialog_Alarm::OnBtnPrevAlarm7() 
{
	OnAlarm_PrevAlarm_Display(7);
}

void CDialog_Alarm::OnBtnPrevAlarm8() 
{
	OnAlarm_PrevAlarm_Display(8);
}

void CDialog_Alarm::OnBtnPrevAlarm9() 
{
	OnAlarm_PrevAlarm_Display(9);
}

void CDialog_Alarm::OnBtnPrevAlarm10() 
{
	OnAlarm_PrevAlarm_Display(10);
}

void CDialog_Alarm::OnOK() 
{	
	if (COMI.mn_run_status == dWARNING || COMI.mn_run_status == dJAM || COMI.mn_run_status == dLOTEND)
	{
		Func.OnSet_IO_Port_Stop();
	}
	DestroyWindow();

	if( st_basic.mn_mode_xgem == CTL_YES )
	{
		g_client_xgem.SetGemAlarmState(alarm.mstr_code, 0);
	}
}

void CDialog_Alarm::OnAlarm_OcurrDate_Display(int nPos)
{
	int index = alarm.mstr_list_jam_time.GetCount();
	CString ocurr_date;
	
	ocurr_date = alarm.mstr_list_jam_time.GetAt(alarm.mstr_list_jam_time.FindIndex(index - nPos));
	
	m_data_alarm_time.SetText(_T(ocurr_date));
}
