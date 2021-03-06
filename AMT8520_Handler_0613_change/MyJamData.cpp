// MyJamData.cpp: implementation of the CMyJamData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Handler.h"
#include "MyJamData.h"

#include "io.h" 
#include "Public_Function.h"  // 모터 보드 및 I/O 보드 관련 클래스
#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "SrcPart/APartHandler.h"
#include "AMTLotManager.h"
#include "AClient_Xgem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CMyJamData			MyJamData;
CMyJamData::CMyJamData()
{
	mn_noError = FALSE;
}

CMyJamData::~CMyJamData()
{

}

/* ****************************************************************************** */
/* 발생 알람 정보 설정 함수                                                       */
/* -> strPath     : 알람 정보 저장 파일 존재 폴더                                 */
/* -> strDataFile : 알람 정보 저장된 File Name(File Name+확장자)                        */
/* -> strCode     : 발생한 알람 Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Display_Info_Load(CString strPath, CString strDataFile, CString strActionFile, CString strCode)
{
	CString str_file_path;  // 폴더 저장 변수 */
	CString str_alarm_chk;  // 함수 리턴 정보 저장 변수 */
	int n_pos;

	str_file_path = strPath;
	if (str_file_path.Right(1) != "\\")  // 파일 경로 구분자 존재 검사 */
	{
		str_file_path += "\\";
	}

	str_alarm_chk = On_Alarm_Bmp_Data_Load(str_file_path, strDataFile, strCode);  // 발생 알람 부위 표시용 그림 File Name 설정 함수 */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // 에러 발생
	{
		return str_alarm_chk;
	}

	str_alarm_chk = On_Alarm_Style_Data_Load(str_file_path, strDataFile, strCode);  // 알람 발생 부위 표시 형태 및 출력 위치 설정 함수 */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // 에러 발생
	{
		return str_alarm_chk;
	}

	str_alarm_chk = On_Alarm_Repair_Data_Load(str_file_path, strActionFile, strCode);  // 발생한 알람에 대한 조치 사항 설정 함수 */
	n_pos = str_alarm_chk.Find("Trouble");
	if (n_pos >= 0)  // 에러 발생
	{
		return str_alarm_chk;
	}

	return str_alarm_chk;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 발생 알람 부위 표시용 그림 File Name 설정 함수                                 */
/* -> strPath     : 알람 표시 그림 파일 존재 폴더                                 */
/* -> strDataFile : 알람 표시 그림 File Name(File Name+확장자)                    */
/* -> strCode     : 발생한 알람 Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Bmp_Data_Load(CString strPath, CString strDataFile, CString strCode)
{
	CString str_return_data;  // 함수 리턴 플래그
	CString str_file;  // 알람 표시용 그림에 대한 [폴더]+[File Name] 저장 변수
	CString str_temp;  // 알람 표시용 그림 File Name 임시 저장 변수

	char chr_buf[1000];

	str_return_data = _T("OK");  // 함수 리턴 플래그 초기화

	str_file = strPath + strDataFile;  // [폴더]+[File Name] 설정

	if (!GetPrivateProfileString(strCode, "DEF", "", chr_buf, 1000, str_file))
	{
		str_return_data = "File Name : "+ str_file;
		str_return_data += "\r\n";
		str_return_data += "[DEF] The information we read. Trouble -> Code : ";
		str_return_data += strCode;

		return str_return_data;
	}

	str_temp.Format("%s", chr_buf);
	st_alarm.mstr_bmp_file = str_temp;  // 알람 표시용 그램 File Name 설정

	return str_return_data;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 알람 발생 부위 표시 형태 및 출력 위치 설정 함수                                */
/* -> strPath     : 부위 표시 형태 저장 파일 폴더                                 */
/* -> strDataFile : 부위 표시 형태 저장 File Name(File Name+확장자)               */
/* -> strCode     : 발생한 알람 Code                                              */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Style_Data_Load(CString strPath, CString strDataFile, CString strCode)
{
	CString str_chk_data;		// 함수 리턴 플래그
	CString str_chk_file;		// 알람 부위 표시 정보에 대한 [폴더]+[File Name] 저장 변수
	CString str_chk, str_chk2;  // 알람 부위 표시 형태 및 위치 저장 변수
	int start=0, end, i;		// 읽을 데이터 위치 저장 변수
	int max=4;					// 읽을 데이터 최대 갯수 (화살표 모양으로 초기화)

	char chr_buf[1000];
	char num[20];

	str_chk_data = _T("OK");  // 함수 리턴 플래그 초기화

	str_chk_file = strPath + strDataFile;  // [폴더]+[File Name] 설정

	if (!GetPrivateProfileString(strCode, "STYLE", "", chr_buf, 1000, str_chk_file))
	{
		str_chk_data = "File Name : "+ str_chk_file;
		str_chk_data += "\r\n";
		str_chk_data += "[STYLE] The information we read. Trouble -> Code : ";
		str_chk_data += strCode;

		return str_chk_data;
	}

	str_chk.Format("%s", chr_buf);

	for(i=0; i<max; i++)
	{
		end = str_chk.Find(';', start);
		if (end == -1)  // 구분자 미존재
		{
			break;
		}
		else            // 구분자 존재
		{
			str_chk2 = str_chk.Mid(start, end-start);  // 구분자 사이의 한 개 아이템 설정
			str_chk2.TrimLeft(' ');
			str_chk2.TrimRight(' ');

			if (i == 0)  // 알람 부위 표시 형태
			{
				st_alarm.mstr_style = str_chk2;       // 알람 부이 표시 형태 설정
				if (st_alarm.mstr_style == "CIRCLE")  // 부위 표시 정보 8개 존재
				{
					max = 8;    
				}
				else if (st_alarm.mstr_style == "NONE")
				{
					max = 1;
				}
			}
			else
			{
				lstrcpy(num, str_chk2);
				if (i == 1)
				{
					st_alarm.mn_Cx = atoi(num);         // X축 픽셀 위치
				}
				else if (i == 2)
				{
					st_alarm.mn_Cy = atoi(num);         // Y축 픽셀 위치
				}
				else if (i == 3)
				{
					st_alarm.mn_Offset = atoi(num);     // 원 크기
				}
				else if (i == 4)
				{
					st_alarm.mn_LineWidth = atoi(num);  // 라인 두께
				}
				else if (i == 5)
				{
					st_alarm.mn_R = atoi(num);          // RGB 중 R 값
				}
				else if (i == 6)
				{
					st_alarm.mn_G = atoi(num);          // RGB 중 G 값
				}
				else if (i == 7) 
				{
					st_alarm.mn_B = atoi(num);          // RGB 중 B 값
				}
				/* ************************************************************** */
			}

			start = end + 1;  // 다음 데이터 위치 설정
		}
	}

	if (i != max)  // Data Format 에러
	{
		str_chk_data = "File Name : "+ str_chk_file;
		str_chk_data += "\r\n";
		str_chk_data += "[STYLE] Data Format. Trouble -> Code : ";
		str_chk_data += strCode;

		return str_chk_data;
	}

	return str_chk_data;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 발생한 알람에 대한 조치 사항 설정 함수                                         */
/* -> strPath       : 알람 조치 사항 저장된 파일 폴더                             */
/* -> strActionFile : 알람 조치 사항 저장된 File Name(File Name+확장자)           */
/* -> strCode       : 발생한 알람 Code                                            */
/* ****************************************************************************** */
CString CMyJamData::On_Alarm_Repair_Data_Load(CString strPath, CString strActionFile, CString strCode)
{
	CString str_return_val;		// 함수 리턴 플래그
	CString str_repair_file;	// 조치 사항에 대한 [폴더]+[File Name] 저장 변수
	CString str_act_item;		// 조치 사항 읽을 위치 설정 변수
	CString str_act_temp;		// 조치 사항 임시 저장 변수
	int n_chk_cnt;				// 조치 사항 갯수 저장 변수
	int i; 
	
	char chr_buf[1000];

	str_return_val = _T("OK");  // 함수 리턴 플래그 초기화

	str_repair_file = strPath + strActionFile;  // [폴더]+[File Name] 설정

	if (!GetPrivateProfileString(strCode, "COUNT", "0", chr_buf, 10, str_repair_file))
	{
		str_return_val = "File Name : "+ str_repair_file;
		str_return_val += "\r\n";
		str_return_val += "[COUNT] The information we read. Trouble -> Code : ";
		str_return_val += strCode;

		return str_return_val;
	}

	n_chk_cnt = atoi(chr_buf);  // 읽을 조치 사항 갯수 설정
	if (n_chk_cnt<1)
	{
		str_return_val = "File Name : "+ str_repair_file;
		str_return_val += "\r\n";
		str_return_val += "[COUNT] Data Format Trouble -> Code : ";
		str_return_val += strCode;

		return str_return_val;
	}
	else
	{
		st_alarm.mn_action_cnt = n_chk_cnt;
	}

	(st_alarm.mstr_repair_msg).Empty();  //  조치 사항 저장 변수 초기화

	for (i=0; i<(st_alarm.mn_action_cnt); i++)   
	{
		str_act_item.Format("%d", i+1);  // 조치 사항 정보 읽을 위치 설정
		if (!GetPrivateProfileString(strCode, (LPCTSTR)str_act_item, "", chr_buf, 1000, str_repair_file))
		{
			str_return_val = "File Name : "+ str_repair_file;
			str_return_val += "\r\n";
			str_return_val += "An action subject. The information we read. Trouble";

			str_return_val += "\r\n";
			str_return_val += "Code : ";
			str_return_val += strCode;

			str_return_val += "\r\n";
			str_return_val += "Position : ";
			str_return_val += str_act_item;

			return str_return_val;
		}
		/* ********************************************************************** */

		st_alarm.mstr_repair_msg += "\r\n";			//조치 사항에 맨 첫줄을 띄자.

		str_act_temp.Format(" %s", chr_buf);  // 조치 사항 설정
		st_alarm.mstr_repair_msg += str_act_temp;

		if (i != ((st_alarm.mn_action_cnt)-1))  
			st_alarm.mstr_repair_msg += "\r\n";  // 조치 사항 단락 구분자 설정
	}
	/* ************************************************************************** */
	
	return str_return_val;
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 발생한 알람 화면 출력 함수                                                     */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Display()
{
	CString str_cnt, str_temp;
	char temp_code[10]={0,}, temp_cnt[10]={0,}, chr_buf[20]={0,};
	
	CString str_display_time;	// 파일 출력용 시간 저장 변수
	CString str_alarm_code;		// 발생한 알람 코드 저장 변수
	CString str_alarm_msg;		// 발생한 알람 메시지 저장 변수
	int n_cur_hour=0;				// 현재 시간 저장 변수
	int n_chk=0;					// 숫자형 알람 코드 저장 변수
	long n_chk_time=0;			// 알람 발생 시간 간격 저장 변수
	int n_data_save = 0;		// Jam을 저장할 것이냐?

	COleDateTime cur_time;
	CTime alarm_occured_time;

 	/* ************************************************************************** */
 	/* 장비 동작 중 알람 발생했는지 검사한다                                      */
 	/* -> 알람 발생한 경우 로그 파일 및 임시 파일에 저장하고 화면에 출력한다      */
 	/* ************************************************************************** */
 	if (COMI.mn_run_status == CTL_dJAM || COMI.mn_run_status == CTL_dWARNING || COMI.mn_run_status == CTL_dLOTEND)  // 알람 발생
 	{	
		//alarm.mstr_code = "990001" ;


 		if (st_handler.mn_alarm_screen == TRUE)	return; 
 		/* ********************************************************************** */
 		/* 알람 화면 출력 불가능한 경우 검사한다.                                 */
 		/* -> st_handler.mn_system_lock : SYSTEM LOCK 상태 플래그                  */
 		/* ********************************************************************** */
 		if (st_handler.mn_system_lock != FALSE)  // SYSTEM LOCK
 		{
 			return;  // 강제 리턴
 		}
 		/* ********************************************************************** */
 

 		/* ************************************************************************** */
 		/* 알람 발생한 시간 설정한다                                                  */
 		/* ************************************************************************** */
 		cur_time = COleDateTime::GetCurrentTime();
 		n_cur_hour = cur_time.GetHour();
 
 		alarm_occured_time = CTime::GetCurrentTime();
 		str_display_time = alarm_occured_time.Format("%c");
 		/* ************************************************************************** */

 
 		/* ********************************************************************** */
 		/* 알람 화면 출력 상태 및 알람 카운트 여부 검사한다                       */
 		/* -> 이미 알람 화면 출력 중인 경우 재 출력할 필요 없으며                 */
 		/*    이 경우에 발생하는 알람은 무시한다                                  */
 		/*  : 알람 화면 출력 중에 알람 Code 변경되면 출력 메시지 달라지기 때문    */
 		/* -> st_handler.mn_alarm_screen : 알람 화면 출력 상태 플래그              */
 		/* -> st_alarm.mn_cur_state : 알람 저장 유무 플래그                        */
 		/* ********************************************************************** */
 		if (st_handler.mn_alarm_screen != TRUE)  // 알람 화면 미출력
 		{
 			if (COMI.mn_run_status == CTL_dLOTEND)		
 			{
 				alarm.mn_reset_status = CTL_NO;
 			}
 			else	alarm.mn_reset_status = CTL_YES;
 
 			//발생한 알람을 List에 넣는다.
 			if (alarm.mstr_code != "")
 			{
 				/* ************************************************************************** */
 				/* 현재 발생한 알람에 대한 메시지 정보 설정한다                               */
 				/* ************************************************************************** */
 				On_Alarm_Info_Set_to_Variable(alarm.mstr_code);  // 발생한 알람 메시지 전역 변수에 설정하는 함수
 				/* ************************************************************************** */
 				sprintf(temp_code, "%s", alarm.mstr_code);
// 				str_cnt.Format("%d", st_alarm.mn_cur_state);
 				sprintf(temp_cnt, "%s", str_cnt);
 
 				/* ************************************************************************** */
 				/* 바로 전에 발생한 알람 코드와 비교하여 동일한 경우 알람 카운트 미진행       */
 				/* -> 알람 카운트 시간 간격은 30초로 설정한다                                 */
 				/* ************************************************************************** */
 				alarm.stl_cur_alarm_time = GetCurrentTime();
 
 				n_chk = atoi(alarm.mstr_code);
				n_data_save = CTL_NO;				// 기본은 NO

				//st_basic.mn_alarm_delay_time = 10;
 				
 				if (alarm.stl_prev_code == n_chk)	// 알람 코드 동일
 				{
// 					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_prev_alarm_time; 

					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_alarm_start_time; 
 					
 					if (n_chk_time < st_basic.mn_alarm_delay_time * 1000 /*|| alarm.stl_alarm_start_time == 0*/)						// Basic에서 설정할수 있게 변경함. 2K9/11/19/ViboX
 					{
 						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
 					}
 					else
 					{
 						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
 						n_data_save = CTL_YES;			// Jam을 저장 및 Count를 한다.
 					}
 				}
 				else
 				{
 					/* ********************************************************************** */
 					/* 발생한 알람 정보로 이전 정보를 대체한다                                */
 					/* -> 다음에 발생하는 알람과 비교하는 정보로 사용된다                     */
 					/* ********************************************************************** */
//  					alarm.stl_prev_code = n_chk;
//  					alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
//  					/* ********************************************************************** */
//  
//  					n_data_save = CTL_YES;				// Jam을 저장 및 Count를 한다.
					n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_alarm_start_time; 
					if (n_chk_time < 120 * 1000/*2분*/)
					{
						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
						mn_noError = TRUE;
						On_Occured_Alarm_Saving_to_File();  // 발생한 알람 로그 파일에 저장하는 함수
					}
					else
					{
						alarm.stl_prev_code = n_chk;
						alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
						n_data_save = CTL_YES;			// Jam을 저장 및 Count를 한다.
					}
				}
 				/* ************************************************************************** */
 			}

			if (st_handler.mn_status == CTL_NO || st_basic.mn_mode_run == CTL_NO || st_handler.mn_virtual_supply == 1 || (g_lotMgr.GetLotIDAt(0).GetLength() < 5))
			{
				n_data_save = CTL_NO;
			}

 			if (n_data_save == CTL_YES)
 			{
				if (st_alarm.mstr_cur_state == "1")			// Count하는 Alarm이면... 2K9/08/28/ViboX
				{
					alarm.mn_alarm_occured++;
					alarm.mn_alarm_occured_day++;

					str_temp = LPCTSTR(_itoa(alarm.mn_alarm_occured_day, chr_buf, 10)) ;
					:: WritePrivateProfileString("BasicData", "Day_Alarm_Count", LPCTSTR(str_temp), st_path.mstr_basic);
				}

				if (st_handler.mn_status == CTL_YES)					// 장비가 가동중이냐
				{
					g_handler.AddAlarmCnt();
					On_Occured_Alarm_Saving_to_File();  // 발생한 알람 로그 파일에 저장하는 함수
				}
 			}
 
 			st_handler.mn_alarm_screen = TRUE;	// 알람 화면 출력 플래그 설정
 
 			Func.OnSet_IO_Port_Alarm();			// 알람 상태 I/O 출력 함수

 			if( st_basic.mn_mode_xgem == CTL_YES )
			{
				g_client_xgem.SetGemAlarmState(alarm.mstr_code, 1);
			}
			
			::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 7, 1);  // 메인 프레임에 알람 화면 출력 요청 
 
 			if (st_handler.cwnd_title != NULL)  // 타이틀 바 출력 검사
 				st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);  // 타이틀 [알람] 출력 요청

 		}
// 		/* ********************************************************************** */
 	}
	/* ************************************************************************** */
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 발생한 알람 로그 파일에 저장하는 함수                                          */
/* ****************************************************************************** */
void CMyJamData::On_Occured_Alarm_Saving_to_File()
{
	CString str_display_time;	// 파일 출력용 시간 저장 변수
	CString str_alarm_code;		// 발생한 알람 Code 저장 변수
	CString str_alarm_msg;		// 발생한 알람 메시지 저장 변수
	int n_cur_hour;				// 현재 시간 저장 변수
	int n_chk;					// 숫자형 알람 Code 저장 변수
	long n_chk_time;			// 알람 발생 시간 간격 저장 변수

	COleDateTime cur_time;
	CTime alarm_occured_time;

	/* ************************************************************************** */
	/* 알람 발생한 시간 설정한다                                                  */
	/* -> SHIFT 상태 검사 및 알람 발생 시간 저장하기 위하여 필요                  */
	/* ************************************************************************** */
	cur_time = COleDateTime::GetCurrentTime();
	n_cur_hour = cur_time.GetHour();

	alarm_occured_time = CTime::GetCurrentTime();
	str_display_time = alarm_occured_time.Format("%Y/%m/%d  %H:%M:%S");
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* 알람 Code 및 알람 메시지 함수 변수에 설정한다                              */
	/* -> 현재는 이 함수 호출 전에 새로운 알람이 발생하는 경우 알람 Code와        */
	/*    조치 사항 및 기타 정보가 일치하지 않을 수 있다                          */
	/* -> alarm.mstr_code : 현재 발생한 알람 Code 정보 저장 변수                   */
	/* -> st_alarm.mstr_cur_msg : 발생한 알람 메시지 저장 변수                     */
	/* ************************************************************************** */
	str_alarm_code = alarm.mstr_code;
	str_alarm_msg = st_alarm.mstr_cur_msg;

	st_alarmdb.mole_date = COleDateTime::GetCurrentTime();

// 	st_alarmdb.mstrlot_no = st_pcb[1].strLotNumber;
// 	st_alarmdb.mstrpart_no = st_pcb[1].strPartNumber;

	st_alarmdb.mstrAlarmCode = str_alarm_code;
	st_alarmdb.mstrAlarmMsg = str_alarm_msg;
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* 일별, 월별 알람 발생 횟수 임시 파일에 추가한다                             */
	/* ************************************************************************** */
	//2012,0306
	if(mn_noError == TRUE)
	{
		mn_noError = FALSE;
		On_UnderDaily_ListFile_Add(str_alarm_code, str_display_time, str_alarm_msg); // 일별, 월별 알람 정보 리스트 파일에 추가 함수 */
		return;
	}

	if (st_alarm.mstr_cur_state == "1")			// Count하는 Alarm이면... 2K9/08/28/ViboX
	{
		On_Daily_ListFile_Add(str_alarm_code, str_display_time, str_alarm_msg); // 일별, 월별 알람 정보 리스트 파일에 추가 함수 */
		//Func.DB_Write_Alarm(st_alarmdb);
	}
	/* ************************************************************************** */
	
	/* ************************************************************************** */
	/* 바로 전에 발생한 알람 Code와 비교하여 동일한 경우 알람 카운트 미진행       */
	/* -> 알람 카운트 시간 간격은 30초로 설정한다                                 */
	/* ************************************************************************** */
	alarm.stl_cur_alarm_time = GetCurrentTime();

	n_chk = atoi(str_alarm_code);
	
	if (alarm.stl_prev_code == n_chk)  // 알람 Code 동일
	{
		n_chk_time = alarm.stl_cur_alarm_time - alarm.stl_prev_alarm_time; 
		
		if (n_chk_time < 30000) 
		{
			alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
			return;
		}
	}
	else
	{
		/* ********************************************************************** */
		/* 발생한 알람 정보로 이전 정보를 대체한다                                */
		/* -> 다음에 발생하는 알람과 비교하는 정보로 사용된다                     */
		/* ********************************************************************** */
		alarm.stl_prev_code = n_chk;
		alarm.stl_prev_alarm_time = alarm.stl_cur_alarm_time;
		/* ********************************************************************** */
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* SHIFT별 알람 발생 횟수 임시 저장 파일에 알람 추가한다                      */
	/* -> 알람 발생 횟수 임시 저장 파일 미존재 시 생성할 알람 리스트 파일 이름은  */
	/*    일별 파일 이름에서 헤더를 제외한 정보와 동일하다                        */
	/* ************************************************************************** */
	if (n_cur_hour >= 6 && n_cur_hour < 14)  // SHIFT 중 DAY */
	{
//		mcls_alarm_list.On_Shift_File_Add(1, str_alarm_code, str_display_time);  // SHIFT별 알람 발생 횟수 정보 임시 파일에 추가 함수 */
	}
	else if (n_cur_hour >= 14 && n_cur_hour < 22)  // SHIFT 중 SWING */
	{
//		mcls_alarm_list.On_Shift_File_Add(2, str_alarm_code, str_display_time);  // SHIFT별 알람 발생 횟수 정보 임시 파일에 추가 함수 */
	}
	else  // SHIFT 중 GY */
	{
//		mcls_alarm_list.On_Shift_File_Add(3, str_alarm_code, str_display_time);  // SHIFT별 알람 발생 횟수 정보 임시 파일에 추가 함수 */
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* LOT별 알람 발생 횟수 임시 파일에 추가한다                                  */
	/* ************************************************************************** */
//	mcls_alarm_list.On_Lot_ListFile_Add(str_alarm_code, str_display_time);  // LOT별 알람 발생 횟수 정보 임시 파일에 추가 함수 */
	/* ************************************************************************** */
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 파일에 저장된 모든 알람 정보 전역 변수에 설정하는 함수                         */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Info_Load()
{
	CString str_load_file;
	CString str_read_data;  // 파일에 저장된 한 라인 정보 저장 변수
	CString str_temp_data;
	CString str_comment_data;
	int n_count, n_len;
	int n_pos = 0 ;

	char chr_s[300];
	FILE  *fp ;  // 파일 핸들 저장 변수

	str_load_file = _T("c:\\AMT8520\\Bmp\\Error.txt"); 

	/* ************************************************************************** */
    /* 알람 정보 저장할 전역 구조체 멤버 변수 초기화                              */
    /* ************************************************************************** */
	for (n_count = 0; n_count < 2000; n_count++)  
	{
		(st_alarm.mstr_e_content[n_count]).Empty();  // 파일에서 로딩한 한 라인 정보 저장 변수 초기화
		(st_alarm.mstr_e_code[n_count]).Empty();     // 알람 Code 저장 변수 초기화
		(st_alarm.mstr_e_state[n_count]).Empty();    // 알람 상태 저장 변수 초기화
		(st_alarm.mstr_e_part[n_count]).Empty();     // 알람 발생 부위 저장 변수 초기화
		(st_alarm.mstr_e_msg[n_count]).Empty();      // 알람 메시지 저장 변수 초기화
	}
	/* ************************************************************************** */
	/* ************************************************************************** */
    /* 알람 정보 저장된 파일 연다                                                 */
    /* ************************************************************************** */
	if ((fp=fopen(str_load_file,"rt"))==NULL)  
	{
		Func.MsgLog("The failure because we open the file.");
		return ;
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
    /* 마지막 파일 정보까지 모두 읽어 전역 변수에 설정한다                        */
    /* ************************************************************************** */
	while(!feof(fp))  // 파일 마지막 검사
	{
		sprintf(chr_s, "");
		
        if (fgets(chr_s,300,fp)==NULL && ferror(fp))
		{   
			Func.MsgLog("The failure because we read the file.");
			return ;
		}
		
		str_read_data.Format("%s",chr_s);  // 파일에서 읽은 한 라인 정보 설정

		n_len = lstrlen(str_read_data);		//빈줄 체크

		if (!str_read_data.IsEmpty() && n_len > 2)
		{
			str_comment_data = str_read_data.Left(2);		//주석줄인지 확인한다.

			if (str_comment_data != "//")					//주석줄이 아니라면...
			{
				st_alarm.mstr_e_content[n_pos] = str_read_data;         // 파일에서 로딩한 한 라인 정보 설정
				st_alarm.mstr_e_code[n_pos] = str_read_data.Left(6) ;    // 알람 Code 설정
				st_alarm.mstr_e_state[n_pos] = str_read_data.Mid(7,1) ;  // 알람 상태 설정
				st_alarm.mstr_e_part[n_pos] = str_read_data.Mid(9,1) ;   // 알람 발생 부위 설정

				str_temp_data = str_read_data.Mid(12,270);
				
				if (st_alarm.mstr_e_code[n_pos] != "990000")
				{
					st_alarm.mstr_e_msg[n_pos] = str_temp_data.Left(str_temp_data.GetLength() - 3) ;  // 공백 제거 알람 메시지 설정
				}
				else
				{
					st_alarm.mstr_e_msg[n_pos] = str_temp_data.Left(str_temp_data.GetLength() - 1) ;  // 공백 제거 알람 메시지 설정
				}
				n_pos++;
			}
		}
	}
	/* ************************************************************************** */

	fclose(fp) ;  // 파일 종료
}
/* ****************************************************************************** */

/* ****************************************************************************** */
/* 발생한 알람 메시지 전역 변수에 설정하는 함수                                   */
/* ****************************************************************************** */
void CMyJamData::On_Alarm_Info_Set_to_Variable(CString strErrCode)
{
	int n_cnt ;
	int n_cur_num = -1;    // 알람에 대한 배열 위치 저장 변수
	CString str_err_code;  // 알람 Code 저장 변수
	CString strPath;

	CString mstr_create_file;  // 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수
	CString mstr_content;  // 한 라인에 출력할 문자열 정보 저장 변수 
	int mn_existence;      // 파일 존재 유무 설정 플래그 
	char fileName[256];    // 검색할 파일 정보 설정 함수 
	FILE  *fp ;            // 파일에 대한 포인터 설정 변수 
	
	CString str_title_time, str_file_time, str_new_file, str_display_time;		// 파일에 출력할 타이틀 시간 저장 변수
	int n_cur_year, n_cur_month, n_cur_day;		// 년, 월, 일 저장 변수
	
	COleDateTime mtime_cur;						// 현재 시간 저장 변수
	CTime mp_time_cur, alarm_occured_time;							// 타이틀 시간 저장 변수

	/* ************************************************************************** */
    /* 입력된 알람 Code에서 좌/우 공백을 제거한다                                 */
    /* ************************************************************************** */
	str_err_code = strErrCode;
	str_err_code.TrimLeft(' ');	
	str_err_code.TrimRight(' ');
	/* ************************************************************************** */

	/* ************************************************************************** */
    /* 발생한 알람 Code에 대한 배열 위치 검색한다                                 */
    /* ************************************************************************** */
	for(n_cnt=0; n_cnt<2000; n_cnt++)
	{  
		if ( (st_alarm.mstr_e_code[n_cnt]).Compare(str_err_code) == 0 )
		{
			n_cur_num = n_cnt ; 
			break;
		}
	}
	/* ************************************************************************** */
	
	if (n_cur_num == -1)  // 미존재 알람
	{
		/* ************************************************************************** */
		/* 파일 이름으로 사용할 날짜 설정한다                                         */
		/* ************************************************************************** */
		mtime_cur = COleDateTime::GetCurrentTime();
		
		n_cur_year = mtime_cur.GetYear();  
		n_cur_month = mtime_cur.GetMonth();  
		n_cur_day = mtime_cur.GetDay();  

		alarm_occured_time = CTime::GetCurrentTime();
		str_display_time = alarm_occured_time.Format("%Y/%m/%d  %H:%M:%S");
		
		str_new_file.Format("ACD%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
		
		mstr_create_file = "C:\\AMT8520\\Log\\AlarmCodeDebug\\" + str_new_file;
		mstr_create_file += ".TXT";
		
		sprintf(fileName, "%s", mstr_create_file);
		mn_existence = access(fileName,0);
		
		if (mn_existence == -1)  /* 파일 미존재 */
		{
			mstr_create_file = "C:\\AMT8520\\Log\\AlarmCodeDebug\\" + str_new_file;
			mstr_create_file += ".TXT";
			/* ********************************************************************** */
		}
		
		/* ************************************************************************** */
		/* 알람 발생 횟수 정보 저장 파일에 추가 가능한 형태 파일로 생성               */
		/* ************************************************************************** */
		if ((fp=fopen(mstr_create_file,"a+"))==NULL)  
		{
//			Func.MsgLog("파일 열기 실패!..");
			return;
		}
		/* ************************************************************************** */
		
		mstr_content = "찾지 못한 알람코드 : " + str_err_code;
		fprintf(fp,"%s\n",mstr_content) ;
		mstr_content = "time : " + str_display_time;
		fprintf(fp,"%s\n",mstr_content) ;
		mstr_content = "------------------------------------------------------------------";
		fprintf(fp,"%s\n",mstr_content) ;
		
		if (ferror(fp))  
		{
			Func.MsgLog("파일 저장 실패!..") ;
			clearerr(fp);
			fclose(fp);
			return ;
		}
		/* ************************************************************************** */
		
		fclose(fp);  /* 파일 종료 */

		alarm.mstr_pcode = alarm.mstr_code;
		alarm.mstr_code = "990000";
		sprintf(alarm.mc_code, "990000");
		str_err_code = "990000";

		for (n_cnt=0; n_cnt<2000; n_cnt++)
		{  
			if ( (st_alarm.mstr_e_code[n_cnt]).Compare(str_err_code) == 0 )
			{
				n_cur_num = n_cnt ; 
				break;
			}
		}
		/* ************************************************************************** */
	}
	
	/* ************************************************************************** */
    /* 발생한 알람에 대한 상태와 메시지 전역 변수에 설정한다                      */
    /* ************************************************************************** */
//	st_alarm.mn_cur_state = atoi(st_alarm.mstr_e_state[n_cur_num]);
	st_alarm.mstr_cur_state = st_alarm.mstr_e_state[n_cur_num];
	st_alarm.mstr_cur_msg = st_alarm.mstr_e_msg[n_cur_num];
	/* ************************************************************************** */
}
/* ****************************************************************************** */
void CMyJamData::On_UnderDaily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg)
{
	CString mstr_file_name;    // 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;  // 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;  // 한 라인에 출력할 문자열 정보 저장 변수 
	int mn_existence;      // 파일 존재 유무 설정 플래그 
	char fileName[256];    // 검색할 파일 정보 설정 함수 
	FILE  *fp ;            // 파일에 대한 포인터 설정 변수 
	
	CString str_title_time, str_file_time, str_new_file;		// 파일에 출력할 타이틀 시간 저장 변수
	int n_cur_year, n_cur_month, n_cur_day;		// 년, 월, 일 저장 변수
	
	COleDateTime mtime_cur;						// 현재 시간 저장 변수
	CTime mp_time_cur;							// 타이틀 시간 저장 변수
	
	/* ************************************************************************** */
	/* 파일 이름으로 사용할 날짜 설정한다                                         */
	/* ************************************************************************** */
	mtime_cur = COleDateTime::GetCurrentTime();
	
	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  
	
	str_new_file.Format("DY%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);
	
	if (str_alarm_code.IsEmpty()==TRUE)  /* 알람 코드 미존재 */
	{
		//Func.MsgLog("Alarm 코드 에러!.."); 
		return;
	}
	
	mstr_create_file = "C:\\AMT8520\\Alarm\\UnderDaily\\" + str_new_file;
	mstr_create_file += ".TXT";
	
	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* 파일 미존재 */
	{
		mstr_create_file = "C:\\AMT8520\\Alarm\\UnderDaily\\" + str_new_file;
		mstr_create_file += ".TXT";
		/* ********************************************************************** */
	}
	
	/* ************************************************************************** */
	/* 알람 발생 횟수 정보 저장 파일에 추가 가능한 형태 파일로 생성               */
	/* ************************************************************************** */
	if ((fp=fopen(mstr_create_file,"a+"))==NULL)  
	{
		Func.MsgLog("파일 열기 실패!..");
		return;
	}
	/* ************************************************************************** */
	
	/* ************************************************************************** */
	/* 로그 파일에 현재 발생한 알람 정보 저장한다                                 */
	/* ************************************************************************** */
	//mstr_content = "--------------------------------------------------------";
	//fprintf(fp,"%s\n",mstr_content) ;
	
	st_alarm.mn_e_list++;
	
	mstr_temp_data.Format("| %4d |", st_alarm.mn_e_list);
	mstr_content = mstr_temp_data; 
	mstr_temp_data.Format(" %6s |", str_alarm_code);
	mstr_content += mstr_temp_data; 
	mstr_temp_data.Format(" %-44s |", str_alarm_msg);
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format("          |     |");
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format(" %17s |", str_display_time);
	mstr_content += mstr_temp_data;
	fprintf(fp,"%s\n",mstr_content) ;
	
	mstr_content = "+------------------------------------------------------------------------------------------------------+";
	fprintf(fp,"%s\n",mstr_content) ;
	
	if (ferror(fp))  
	{
		Func.MsgLog("파일 저장 실패!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	/* ************************************************************************** */
	
	fclose(fp);  /* 파일 종료 */
}

void CMyJamData::On_Daily_ListFile_Add(CString str_alarm_code, CString str_display_time, CString str_alarm_msg)
{
	CString mstr_file_name;    // 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;  // 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;  // 한 라인에 출력할 문자열 정보 저장 변수 
	int mn_existence;      // 파일 존재 유무 설정 플래그 
	char fileName[256];    // 검색할 파일 정보 설정 함수 
	FILE  *fp ;            // 파일에 대한 포인터 설정 변수 

	CString str_title_time, str_file_time, str_new_file;		// 파일에 출력할 타이틀 시간 저장 변수
	int n_cur_year, n_cur_month, n_cur_day;		// 년, 월, 일 저장 변수
	
	COleDateTime mtime_cur;						// 현재 시간 저장 변수
	CTime mp_time_cur;							// 타이틀 시간 저장 변수
	
	/* ************************************************************************** */
	/* 파일 이름으로 사용할 날짜 설정한다                                         */
	/* ************************************************************************** */
	mtime_cur = COleDateTime::GetCurrentTime();
	
	n_cur_year = mtime_cur.GetYear();  
	n_cur_month = mtime_cur.GetMonth();  
    n_cur_day = mtime_cur.GetDay();  
	
	str_new_file.Format("DY%04d%02d%02d", n_cur_year, n_cur_month, n_cur_day);

	if (str_alarm_code.IsEmpty()==TRUE)  /* 알람 코드 미존재 */
	{
		//Func.MsgLog("Alarm 코드 에러!.."); 
		return;
	}

	mstr_create_file = "C:\\AMT8520\\Alarm\\Daily\\" + str_new_file;
	mstr_create_file += ".TXT";

	sprintf(fileName, "%s", mstr_create_file);
	mn_existence = access(fileName,0);
	
	if (mn_existence == -1)  /* 파일 미존재 */
	{
		mstr_create_file = "C:\\AMT8520\\Alarm\\Daily\\" + str_new_file;
		mstr_create_file += ".TXT";
		/* ********************************************************************** */
	}

	/* ************************************************************************** */
	/* 알람 발생 횟수 정보 저장 파일에 추가 가능한 형태 파일로 생성               */
	/* ************************************************************************** */
	if ((fp=fopen(mstr_create_file,"a+"))==NULL)  
	{
		Func.MsgLog("파일 열기 실패!..");
		return;
	}
	/* ************************************************************************** */

	/* ************************************************************************** */
	/* 로그 파일에 현재 발생한 알람 정보 저장한다                                 */
	/* ************************************************************************** */
	//mstr_content = "--------------------------------------------------------";
	//fprintf(fp,"%s\n",mstr_content) ;

	st_alarm.mn_e_list++;

	mstr_temp_data.Format("| %4d |", st_alarm.mn_e_list);
	mstr_content = mstr_temp_data; 
	mstr_temp_data.Format(" %6s |", str_alarm_code);
	mstr_content += mstr_temp_data; 
	mstr_temp_data.Format(" %-44s |", str_alarm_msg);
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format("          |     |");
	mstr_content += mstr_temp_data;
	mstr_temp_data.Format(" %17s |", str_display_time);
	mstr_content += mstr_temp_data;
	fprintf(fp,"%s\n",mstr_content) ;

	mstr_content = "+------------------------------------------------------------------------------------------------------+";
	fprintf(fp,"%s\n",mstr_content) ;

	if (ferror(fp))  
	{
		Func.MsgLog("파일 저장 실패!..") ;
		clearerr(fp);
		fclose(fp);
		return ;
	}
	/* ************************************************************************** */

	fclose(fp);  /* 파일 종료 */
}

CString GetJamCode_GripperDn_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0GD0%d", iOn );
	strJamCode.Format( "51000%d", iOn );
	return strJamCode;
}

CString GetJamCode_VacuumDn_Time( int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0P%d0%d", iIdx+1, iOn );
	strJamCode.Format( "540%d0%d", iIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_GripperOn_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0GP%d", iOn );
	strJamCode.Format( "5110%02d", iOn );
	return strJamCode;
}

CString GetJamCode_GripperDn_Chk( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0GD0%d", iOn );
	strJamCode.Format( "25000%d", iOn );
	return strJamCode;
}

CString GetJamCode_VacuumDn_Chk( int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0P%d0%d", iIdx+1, iOn );
	strJamCode.Format( "280%d0%d", iIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Plate_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CSP%d0%d", nSite+1, iOn );
	strJamCode.Format( "360%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Insert_Chk( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0I%d0%d", nSite * 5 + iIdx, iOn );
	strJamCode.Format( "270%d0%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Lock_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CSL%d0%d", nSite+1, iOn );
	strJamCode.Format( "350%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_HF_Clamp_Chk( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0F%d0%d", nSite+1, iOn );
	strJamCode.Format( "230%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayGuide_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0G%d0%d", nTray+1, iOn );
	strJamCode.Format( "240%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayPusher_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CPS%d0%d", nTray+1, iOn );
	strJamCode.Format( "340%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayExist_Chk( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T%d0%d", nTray + 4, iOn );
	strJamCode.Format( "300%d0%d", nTray + 4, iOn );
	return strJamCode;
}

CString GetJamCode_AlignChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0A%d0%d", nIdx+1, iOn );
	strJamCode.Format( "200%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_StopperChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0S%d0%d", nIdx+1, iOn );
	strJamCode.Format( "292%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Align_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0A%d0%d", nIdx+1, iOn );
	strJamCode.Format( "480%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Stopper_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0S%d0%d", nIdx+1, iOn );
	strJamCode.Format( "570%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_ClampChk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0C%d0%d", nIdx+1, iOn );
	strJamCode.Format( "210%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Clamp_Time( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0C%d0%d", nIdx+1, iOn );
	strJamCode.Format( "490%d0%d", nIdx+1, iOn );
	return strJamCode;
}

CString GetJamCode_Elev_Z( int nIdx, int nPos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMZ%d0%d", nIdx+1, nPos );
	strJamCode.Format( "465%d0%d", nIdx+1, nPos );
	return strJamCode;
}

CString GetJamCode_Elev_R( int nIdx, int nPos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMR%d0%d", nIdx+1, nPos );
	strJamCode.Format( "461%d0%d", nIdx+1, nPos );
	return strJamCode;
}

CString GetJamCode_ConvTray_Chk( int nIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T%d0%d", nIdx + 1, iOn );
	strJamCode.Format( "300%d0%d", nIdx + 1, iOn );
	return strJamCode;
}

CString GetJamCode_Transfer_Y( int npos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMTY0%d", npos );
	strJamCode.Format( "46310%d", npos );
	return strJamCode;
}

CString GetJamCode_Transfer_Dn( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0TD0%d", iOn );
	strJamCode.Format( "31000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Variable( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0RL0%d", iOn );
	strJamCode.Format( "29100%d", iOn );
	return strJamCode;
}

CString GetJamCode_TransferTray( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0T70%d", iOn );
	strJamCode.Format( "30070%d", iOn );
	return strJamCode;
}

CString GetJamCode_Transfer_C( int npos )
{
	CString strJamCode;
// 	strJamCode.Format( "PMTC0%d", npos );
	strJamCode.Format( "46300%d", npos );
	return strJamCode;
}

CString GetJamCode_CokLock( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0CL0%d", iOn );
	strJamCode.Format( "22000%d", iOn );
	return strJamCode;
}

CString GetJamCode_TS_Insert_Time( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0I%d0%d", nSite * 5 + iIdx, iOn );
	strJamCode.Format( "520%d0%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}

//2013,0204
CString GetJamCode_TS_Pogo_Time( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
	if(nSite == 0)	strJamCode.Format( "520%d1%d", nSite * 5 + iIdx, iOn );
	else            strJamCode.Format( "521%d1%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}


CString GetJamCode_TS_Plate_Time( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TSP%d0%d", nSite+1, iOn );
	strJamCode.Format( "640%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TS_Lock_Time( int nSite, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TSL%d0%d", nSite+1, iOn );
	strJamCode.Format( "630%d0%d", nSite+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayGuide_Time( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0G%d0%d", nTray+1, iOn );
	strJamCode.Format( "500%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_TrayPusher_Time( int nTray, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "TPS%d0%d", nTray+1, iOn );
	strJamCode.Format( "530%d0%d", nTray+1, iOn );
	return strJamCode;
}

CString GetJamCode_ModuleExist_Chk( int nPicker, int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "CIT%d0%d", nPicker+1, iOn );
	strJamCode.Format( "330%d0%d", nPicker+1, iOn );
	return strJamCode;
}

CString GetJamCode_GripperOn_Chk( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "C0GP0%d", iOn );
	strJamCode.Format( "26000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Rej_Guide_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0RG0%d", iOn );
	strJamCode.Format( "55000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Rej_Seperator_Time( int iOn )
{//550010 1 A "리젝트 스태커 세퍼레이터 후진 제한시간 에러."
	CString strJamCode;
// 	strJamCode.Format( "T0RG0%d", iOn );
	strJamCode.Format( "55001%d", iOn );
	return strJamCode;
}


CString GetJamCode_Rail_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0RL0%d", iOn );
	strJamCode.Format( "56000%d", iOn );
	return strJamCode;
}

CString GetJamcode_Rej_Transfer_Down_Time( int iOn )
{
	CString strJamCode;
// 	strJamCode.Format( "T0TD0%d", iOn );
	strJamCode.Format( "59000%d", iOn );
	return strJamCode;
}

CString GetJamCode_Pogo_Insert_Before_Chk( int nSite, int iIdx, int iOn )
{
	CString strJamCode;
	strJamCode.Format( "270%d1%d", nSite * 5 + iIdx, iOn );
	return strJamCode;
}
