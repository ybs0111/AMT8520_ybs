// Run_Handler_Check.cpp: implementation of the CRun_Handler_Check class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Handler_Check.h"

#include "Variable.h"				// 전역 변수 정의 Class
#include "Public_Function.h"		// Motor 및 Cylinder 동작 Class
#include "FastechPublic_IO.h"
#include "CtlBd_Library.h"

#include "Srcbase\ALocalization.h"

#include "IO_Manager.h"
#include "ComizoaPublic.h"
#include "Cmmsdk.h"

#include "Run_Motors.h"
#include "SrcPart/APartHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRun_Handler_Check Run_Handler_Check;
CRun_Handler_Check Run_Handler_Check_Manual;

CRun_Handler_Check::CRun_Handler_Check()
{
	StartStep = 0;
	StopStep = 0;
	LoaderStep = 0;
	AlaemClearStep = 0;
	BuzzerStep = 0;
	
	MachineResetStep = 0;
	
	DoorStep = 0;
	AreaStep = 0;
	
	n_emo_chk = FALSE;
	n_air_chk = FALSE;

	n_light_curtain_chk = CTL_NO;

	n_front_key = -1;
	n_rear_key = -1;

}

CRun_Handler_Check::~CRun_Handler_Check()
{

}

void CRun_Handler_Check::ButtonCheck_Start()
{
	int nRet, nRet2;
	// **************************************************************************
    // 화면 뷰 전환 불가능한 정보 검사한다.                                      
    // **************************************************************************
	if (st_handler.mn_system_lock != FALSE)  return; // 현재 시스템 Lock 상태인 경우 리턴 
	// **************************************************************************

	if (st_handler.mn_switch_lock != FALSE)	return;	// 모달 창이 떠 있으면 실행하지 않는다.

	switch (StartStep)
	{
	case 0:
		// **********************************************************************
		// RUN 키 I/O 입력이 On 상태이고 상태가 STOP 상태였는지 검사             
		// -> 다시 말해, STOP 상태에서만 RUN 키 입력이 동작한다                  
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_start_switch_chk) == TRUE && COMI.mn_run_status == dSTOP)
		{
			// 만일 Stop Button이 같이 눌렸다면 동작되지 않는다.
			if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == TRUE)
			{
				break;
			}

			StartStep = 100;
		}
		break;

	case 100:
		// **********************************************************************
		// START 키가 순간적으로 On/Off 되는 것을 방지하기 위하여 대기 함        
		// **********************************************************************
		SwitchWaitTime[0] = GetCurrentTime();
		StartStep = 200;
		break;

	case 200: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];

		if(SwitchWaitTime[2] > 50 && g_ioMgr.Get_In_Bit(st_io.i_start_switch_chk) == FALSE)
		{
			SwitchWaitTime[0] = GetCurrentTime();
			StartStep = 300;
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		else if (g_ioMgr.Get_In_Bit(st_io.i_start_switch_chk) == FALSE)
		{
			StartStep = 0;
		}
		break;

	case 300:
		// **********************************************************************
		// RUN 키 I/O 입력이 Off 되었는지 검사                                   
		// -> 알람 화면 출력 상태이면 알람 화면 종료 -> 메인 화면으로 전환       
		// **********************************************************************
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];

		if(SwitchWaitTime[2] > 10)
		{
			if (g_ioMgr.Get_In_Bit(st_io.i_start_switch_chk) == FALSE)
			{
				StartStep = 600;
			}
			else
			{
				StartStep = 0;
			}
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		// **********************************************************************
		break;

	case 600:
		// **********************************************************************
		//  RUN할수 있는 조건인지 검사한다. 이상이 없어야 RUN이 된다.            
		// **********************************************************************
		if (st_handler.mn_motor_init_check != CTL_YES)	// 모터 초기화를 하지 않았다면 동작하지 않는다. 2K4/11/23/ViboX
		{
			Func.OnSet_IO_Port_Sound(IO_ON);
//			st_msg.mstr_abnormal_msg = _T("[MOTOR INIT CHECK] Motor Initial..., become Run.");
			sprintf(st_msg.c_abnormal_msg, "[MOTOR INIT CHECK] Motor Initial..., become Run.");
			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);

			alarm.mstr_code = _T("390200");		// Alarm 화면을 띄워준다. 2K4/11/21/ViboX
			sprintf(alarm.mc_code, "390200");
			alarm.mn_count_mode = 0;				// 알람 카운트 여부 플래그 설정 (알람 카운트 작업 미진행)
			alarm.mn_type_mode = eWARNING;		// 현재 발생한 알람 상태 플래그 설정
			COMI.mn_run_status = dWARNING;	// dJAM,dWARNING  Alarm 화면을 띄워준다. 2K4/11/21/ViboX
			StartStep = 0;
//			g_handler.AddAlarmCnt();
			return;
		}

// 		// JAM이 리셋 되었는지 확인하고 RUN을 시킨다.
		if (alarm.mn_reset_status != CTL_NO)
		{
			sprintf(st_msg.c_abnormal_msg, "[JAM CONFIRM] If do to do Run, do first Jam Reset.");
			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			StartStep = 0;
			break;
		}

		// Door가 열려있으면 RUN이 되지 않는다.
		if (Func.DoorOpenCheckSpot() != CTLBD_RET_GOOD && st_handler.mn_safety == CTL_YES)	
		{
			Func.OnSet_IO_Port_Sound(IO_ON);

			sprintf(st_msg.c_abnormal_msg, "[DOOR CHECK] Door Open being done Run become.");
			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			
			COMI.mn_run_status = dWARNING;	// dJAM,dWARNING  Alarm 화면을 띄워준다. 2K4/11/21/ViboX

			StartStep = 0;
			break;
		}
	
		// Air 감지 센서를 확인한다.
		if (g_ioMgr.Get_In_Bit(st_io.i_main_air_chk) == FALSE)
		{
			Func.OnSet_IO_Port_Sound(IO_ON);
//			st_msg.mstr_abnormal_msg = _T("[AIR CHECK] Though supply Air, become Run.");
			sprintf(st_msg.c_abnormal_msg, "[AIR CHECK] Though supply Air, become Run.");
			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			StartStep = 0;
			return;
		}

		nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
		nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);
		
		if (nRet != IO_ON || nRet2 != IO_ON)
		{			
			st_handler.mstrSelectMessage = "KEY BUTTON CHECK...";
			st_handler.mnSelectMessage = 0;
			
			::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
			
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
			
			StartStep = 620;
			break;
		}

// 		if (g_ioMgr.Get_In_Bit(st_io.i_servo_motor_power_chk) == CTL_NO)
// 		{
// 			st_handler.mstrSelectMessage = "먼저 RESET 버튼을 눌러 주세요.";
// 			
// 			if ( g_local.GetLocalType() == LOCAL_ENG )
// 			{
// 				st_handler.mstrSelectMessage = "RESET BUTTON CHECK...";
// 			}
// 			st_handler.mnSelectMessage = 0;
// 			
// 			
// 			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
// 			
// 			StartStep = 620;
// 			break;
// 		}
		
		if (st_basic.mn_mode_run == CTL_NO)		// OFFLINE이면 상태를 알리자.
		{
			st_handler.mstrSelectMessage = "OFFLINE 상태입니다. RUN 하시겠습니까?";
			
			if ( g_local.GetLocalType() == LOCAL_ENG )
			{
				st_handler.mstrSelectMessage = "NOW OFFLINE. ARE YOU RUN?";
			}
			st_handler.mnSelectMessage = 0;
			
			::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
			
//			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
			
			StartStep = 610;
			break;
		}
		//ybs
		if (g_ioMgr.Get_In_Bit(st_io.i_mc1_chk) == CTL_NO || g_ioMgr.Get_In_Bit(st_io.i_mc2_chk) == CTL_NO)
		{
			st_handler.mstrSelectMessage = "Check MC1 or MC2. Motor,IO Power... Reset Button Check";
			st_handler.mnSelectMessage = 0;
			::PostMessage(st_handler.hWnd, WM_MAINFRAME_WORK, 1001, 0);
			
			StartStep = 610;
			break;
		}
		

		StartStep = 700;
		break;

	case 610:
		if (st_handler.mnSelectMessage == 1)					// YES
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
			StartStep = 700;
		}
		else if (st_handler.mnSelectMessage == 2)			// NO
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
			StartStep = 0;
		}
		break;

	case 620:
		if (st_handler.mnSelectMessage == 1)					// YES
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
			StartStep = 0;
		}
		else if (st_handler.mnSelectMessage == 2)			// NO
		{
			g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_OFF);
			StartStep = 0;
		}
		break;

	case 700:
		if(COMI.mn_run_status != dSTOP) break;

// 		Ret = Func.Robot_Reinstatement_Position(1);		// 복구 동작 확인 해야 함.
// 
// 		if (Ret == RET_GOOD)
// 		{	//정상완료
// 
// 		}
// 		else
// 		{
// 			if(COMI.mn_run_status != dSTOP)
// 			{
// 				StartStep = 0;
// 			}
// 			break;
// 		}

		//2013,0124
// 		st_work.md_reinstatement_pos[0] = CTL_NO;
// 		st_work.md_reinstatement_pos[1] = CTL_NO;
// 		st_work.md_reinstatement_pos[2] = CTL_NO;

		st_work.n_sync_reinstate = CTL_NO;
		st_work.mn_xyz_safety = CTL_NO;

		st_work.mn_reinstate_xyz_robot		= CTL_NO;
		st_work.mn_reinstate_reject_robot	= CTL_NO;
		st_work.mn_reinstate_tester_robot[0] = st_work.mn_reinstate_tester_robot[1] = CTL_NO;
		st_work.mn_reinstate_stacker_robot[0] = st_work.mn_reinstate_stacker_robot[1] = st_work.mn_reinstate_stacker_robot[2] = CTL_NO;

		if(st_handler.cwnd_title != NULL)
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);
		}


		if(st_handler.mn_virtual_supply)
		{
 			st_work.mn_reinstate_stacker_robot[0] = CTL_YES;
 			st_work.mn_reinstate_stacker_robot[1] = CTL_YES;
			st_work.mn_reinstate_stacker_robot[2] = CTL_YES;
		}
		if(st_basic.n_mode_recovery == CTL_NO)
		{
			st_work.mn_reinstate_xyz_robot = CTL_YES;
			st_work.mn_reinstate_reject_robot = CTL_YES;
			st_work.mn_reinstate_tester_robot[0] = CTL_YES;
			st_work.mn_reinstate_tester_robot[1] = CTL_YES;
			st_work.mn_reinstate_stacker_robot[0] = CTL_YES;
			st_work.mn_reinstate_stacker_robot[1] = CTL_YES;
			st_work.mn_reinstate_stacker_robot[2] = CTL_YES;
		}
		else
		{
			COMI.mn_run_status					= dREINSTATE;
		}
		StartStep = 800;
		break;

	case 800:
		if(st_work.mn_reinstate_xyz_robot == CTL_YES && st_work.mn_reinstate_reject_robot == CTL_YES &&
			st_work.mn_reinstate_tester_robot[0] == CTL_YES && st_work.mn_reinstate_tester_robot[1] == CTL_YES && 
			st_work.mn_reinstate_stacker_robot[0] == CTL_YES &&	st_work.mn_reinstate_stacker_robot[1] == CTL_YES && 
			st_work.mn_reinstate_stacker_robot[2] == CTL_YES)
		{
			StartStep = 900;
		}
		else
		{
			if(COMI.mn_run_status != dREINSTATE)	//복귀 동작이 성공도, 실패도 아닌데 복귀동작 상태가 아니라면..
			{
				COMI.mn_run_status = dSTOP;
				if(st_handler.cwnd_title != NULL)
				{
					st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, MACHINE_STATUS, COMI.mn_run_status);
				}
				StartStep = 0;
			}
		}
		break;

	case 900:
		// **********************************************************************
		// RUN 키 I/O 입력이 Off 되었는지 검사                                   
		// -> 알람 화면 출력 상태이면 알람 화면 종료 -> 메인 화면으로 전환       
		// **********************************************************************
		::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 7, 2);		// 메인 프레임으로 메시지 전송하여 알람 화면 종료 요청 
	
		if (st_handler.mn_menu_num != 101)
		{
			::SendMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 1);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
		}

		{

			//2015.0219
			bool nRun = false;
			if( st_basic.mn_mode_xgem == CTL_YES)
			{
				if(st_basic.mn_mode_rcmd == CTL_YES)
				{
					if( st_handler.m_nRemote == CTL_YES )
					{
						nRun = false;
					}
					else
					{
						nRun = true;
					}
				}
			}
			if(nRun == true)
			{
				StartStep = 910;
			}
			else
			{
				StartStep = 1000;
			}
		}

		break;

	case 910:
		Func.OnSavePPID();
		mdTimeXgem = GetCurrentTime();
		StartStep = 920;
		break;

	case 920:
		{
			bool nRun = false;
			if( st_basic.mn_mode_xgem == CTL_YES)
			{
				if(st_basic.mn_mode_rcmd == CTL_YES)
				{
					if( st_handler.m_nRemote == CTL_YES )
					{
						nRun = false;
					}
					else
					{
						nRun = true;
					}
				}
			}
			if(nRun == false)
			{
				StartStep = 1000;
			}
			else
			{
				if( GetCurrentTime() - mdTimeXgem <= 0 ) mdTimeXgem = GetCurrentTime();
				if( GetCurrentTime() - mdTimeXgem > 10000 )
				{
					sprintf(st_msg.c_abnormal_msg, "[XGEM Check] Remote stop or not response.");
					if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					
					alarm.mstr_code = _T("390200");
					sprintf(alarm.mc_code, "390200");
					alarm.mn_count_mode = 0;
					alarm.mn_type_mode = eWARNING;
					COMI.mn_run_status = dWARNING;	// dJAM,dWARNING  Alarm 화면을 띄워준다. 2K4/11/21/ViboX
					StartStep = 0;
				}
			}
		}
		break;

	case 1000:
		Func.OnSet_IO_Port_Run();									// 장비 상태 : 동작 상태인 경우 I/O 출력 내보내는 함수
		alarm.mn_emo_alarm = FALSE;
		st_handler.mn_userstop = FALSE; //2015.0914
		n_emo_chk = FALSE;
		n_air_chk = FALSE;

		n_light_curtain_chk = CTL_NO;

		StartStep = 0;
		break;
	
	default :
		break;
	}
}

void CRun_Handler_Check::ButtonCheck_Stop()
{
	// **************************************************************************
    // 화면 뷰 전환 불가능한 정보 검사한다.                                      
    // **************************************************************************
	if (st_handler.mn_system_lock != FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	// **************************************************************************

	switch (StopStep)
	{
	case 0:
		// **********************************************************************
		// STOP 키 I/O 입력이 On 상태이고 현재 STOP 상태가 아닌지 검사           
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == TRUE && COMI.mn_run_status != dSTOP)
		{
			// 만일 Start Button이 같이 눌렸다면 동작되지 않는다.
			if (g_ioMgr.Get_In_Bit(st_io.i_start_switch_chk) == TRUE)
			{
				break;
			}

			StartStep = 0;
			StopStep = 100;
//			StopStep = 600;
		}
		else if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == TRUE)
		{
			StopStep = 100;
//			StopStep = 600;
		}
		break;

	case 100:
		// **********************************************************************
		// START 키가 순간적으로 On/Off 되는 것을 방지하기 위하여 대기 함        
		// **********************************************************************
		SwitchWaitTime[0] = GetCurrentTime();
		StopStep = 200;
		break;
		
	case 200: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 50 && g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == FALSE)
		{
			SwitchWaitTime[0] = GetCurrentTime();
			StopStep = 300;
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		else if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == FALSE)
		{
			StopStep = 0;
		}
		break;
		
	case 300:
		// **********************************************************************
		// RUN 키 I/O 입력이 Off 되었는지 검사                                   
		// -> 알람 화면 출력 상태이면 알람 화면 종료 -> 메인 화면으로 전환       
		// **********************************************************************
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 10)
		{
			if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == FALSE)
			{
				StopStep = 600;
			}
			else
			{
				StopStep = 0;
			}
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		// **********************************************************************
		break;

	case 600:
		// **********************************************************************
		// STOP 키 I/O 입력이 Off 되었는지 검사                                  
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_stop_switch_chk) == FALSE)
		{
			StopStep = 0; 

			if (COMI.mn_run_status == dINIT)				// 초기화 중에 Stop 버튼이 눌리면... 2K5/02/23/ViboX
			{//900000 1 A "Equipment has not been initialized."
				st_handler.mn_initial_error = TRUE;
				
				alarm.mstr_code = _T("900000");		// Alarm 화면을 띄워준다. 2K5/03/10/ViboX
				sprintf(alarm.mc_code, "900000");
				alarm.mn_count_mode = 0;				// 알람 카운트 여부 플래그 설정 (알람 카운트 작업 미진행)
				alarm.mn_type_mode = eWARNING;		// 현재 발생한 알람 상태 플래그 설정
				COMI.mn_run_status = dWARNING;	// dJAM,dWARNING 

//				g_handler.AddAlarmCnt();
				break;
			}

			if (st_handler.mn_menu_num != 101)
			{
				::PostMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 1);  // 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
			}
			
			st_handler.mn_userstop = CTL_YES;//2015.0914
			
			Func.OnSet_IO_Port_Stop();
		}
		break;

	default : 
		break;
	}
}

void CRun_Handler_Check::ButtonCheck_BuzzerOff()
{
	// **************************************************************************
    // 화면 뷰 전환 불가능한 정보 검사한다.                                      
    // **************************************************************************
	if (st_handler.mn_system_lock != FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	// **************************************************************************
	
	switch(BuzzerStep)
	{
	case 0:
		// **********************************************************************
		//  Reset 키 I/O 입력이 On 상태인지 확인                                 
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_buzzer_off_chk) == TRUE)
		{
			BuzzerStep = 10;
		}
		break;
		
	case 10:
		// **********************************************************************
		//  Reset 키가 순간적으로 On/Off 되는 것을 방지하기 위하여 대기 함       
		// **********************************************************************
		SwitchWaitTime[0] = GetCurrentTime();
		BuzzerStep = 20;
		break;
		
	case 20: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 50 && g_ioMgr.Get_In_Bit(st_io.i_buzzer_off_chk) == FALSE)
		{
			SwitchWaitTime[0] = GetCurrentTime();
			BuzzerStep = 30;
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		else if (g_ioMgr.Get_In_Bit(st_io.i_buzzer_off_chk) == FALSE)
		{
			BuzzerStep = 0;
		}
		break;
		
	case 30: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 10)
		{
			if (g_ioMgr.Get_In_Bit(st_io.i_buzzer_off_chk) == FALSE)
			{
				BuzzerStep = 40;
			}
			else
			{
				BuzzerStep = 0;
			}
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		// **********************************************************************
		break;
		
	case 40:
		// **********************************************************************
		// Sound 키 I/O 입력이 Off 되었는지 검사                                 
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_buzzer_off_chk) == FALSE)
		{
//			st_msg.mstr_normal_msg = _T("[SOUND] OFF.");
			sprintf(st_msg.c_normal_msg, "[SOUND] OFF.");
			if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			
			Func.OnSet_IO_Port_Sound(IO_OFF);
			
			BuzzerStep = 0;
		}
		break;
		
	default : 
		break;
	}
}

void CRun_Handler_Check::ButtonCheck_Alarm_Clear()
{
	// **************************************************************************
    // 화면 뷰 전환 불가능한 정보 검사한다.                                      
    // **************************************************************************
	if (st_handler.mn_system_lock != FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	// **************************************************************************
	
	switch(AlaemClearStep)
	{
	case 0:
		// **********************************************************************
		//  Reset 키 I/O 입력이 On 상태인지 확인                                 
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_alarm_clear_chk) == TRUE)
		{
			AlaemClearStep = 10;
		}
		break;
		
	case 10:
		// **********************************************************************
		//  Reset 키가 순간적으로 On/Off 되는 것을 방지하기 위하여 대기 함       
		// **********************************************************************
		SwitchWaitTime[0] = GetCurrentTime();
		AlaemClearStep = 20;
		break;
		
	case 20: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 50 && g_ioMgr.Get_In_Bit(st_io.i_alarm_clear_chk) == FALSE)
		{
			SwitchWaitTime[0] = GetCurrentTime();
			AlaemClearStep = 30;
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		else if (g_ioMgr.Get_In_Bit(st_io.i_alarm_clear_chk) == FALSE)
		{
			AlaemClearStep = 0;
		}
		break;
		
	case 30: 
		SwitchWaitTime[1] = GetCurrentTime();
		SwitchWaitTime[2] = SwitchWaitTime[1] - SwitchWaitTime[0];
		
		if(SwitchWaitTime[2] > 10)
		{
			if (g_ioMgr.Get_In_Bit(st_io.i_alarm_clear_chk) == FALSE)
			{
				AlaemClearStep = 40;
			}
			else
			{
				AlaemClearStep = 0;
			}
		}//2012,1220
		else if(SwitchWaitTime[2] < 0)
		{
			SwitchWaitTime[0] = GetCurrentTime();
		}
		break;
		
	case 40:
		// **********************************************************************
		// Reset 키 I/O 입력이 Off 되었는지 검사                                 
		// **********************************************************************
		if (g_ioMgr.Get_In_Bit(st_io.i_alarm_clear_chk) == FALSE)
		{
			AlaemClearStep = 110;
		}
		break;
		
	case 110:
		alarm.mn_reset_status = CTL_NO;
//		st_msg.mstr_normal_msg = _T("[ALARM CLEAR] JAM RESET.");
		sprintf(st_msg.c_normal_msg, "[ALARM CLEAR] JAM RESET.");
		if (st_handler.cwnd_list != NULL)  st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		
		if (COMI.mn_run_status != dRUN)	Func.OnSet_IO_Port_Stop();
		g_ioMgr.set_out_bit(st_io.o_alarm_clear_lamp, IO_OFF);	// Reset Switch에 Lamp를 Off 시킨다.
		
		AlaemClearStep = 0;
		break;
		
	default : 
		break;
	}
}

void CRun_Handler_Check::ButtonCheck_ManualAutoKey()
{
	int nRet, nRet2, nRet3;

	nRet = g_ioMgr.get_in_bit(st_io.i_front_select_switch_chk, IO_OFF);
	nRet2 = g_ioMgr.get_in_bit(st_io.i_rear_select_switch_chk, IO_OFF);

	if (n_front_key != nRet)
	{
		n_front_key = nRet;

		if (st_handler.cwnd_title != NULL)	
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, FRONT_KEY_MODE, n_front_key);
		}
	}

	if (n_rear_key != nRet2)
	{
		n_rear_key = nRet2;

		if (st_handler.cwnd_title != NULL)	
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, REAR_KEY_MODE, n_rear_key);
		}
	}

	if (COMI.mn_run_status == dRUN)
	{
		if (nRet == IO_ON && nRet2 == IO_ON)
		{			
			
		}
	}

	nRet3 = st_handler.mn_menu_num;

// #define IDW_SCREEN_MAIN					101
// #define IDW_SCREEN_LOCK					102
// #define IDW_SCREEN_INIT					103
// 	
// #define IDW_SCREEN_BASIC		 			201
// 	
// #define IDW_SCREEN_SET_MAINTENANCE		301
// 	
// #define IDW_SCREEN_WAIT_TIME				401
// 	
// #define IDW_SCREEN_MOTOR_TEACH			501
// #define IDW_SCREEN_MOTOR_SPEED			502
// 	
// #define IDW_SCREEN_LIST_OPERATION		601
// #define IDW_SCREEN_LIST_DATA_VIEW		602
// #define IDW_SCREEN_LIST_STEP				603
// 	
// #define IDW_SCREEN_IO					701
// 	
// #define IDW_SCREEN_ADMINISTRATOR			801
	
	if (nRet3 == IDW_SCREEN_MOTOR_TEACH || nRet3 == IDW_SCREEN_MOTOR_SPEED || nRet3 == IDW_SCREEN_IO ||
		nRet3 == IDW_SCREEN_MOTOR_WITH_IO )
	{
		if (nRet == IO_ON && nRet2 == IO_ON)
		{			
			::SendMessage(st_handler.hWnd, WM_FORM_CHANGE, 1, 1);	// 메인 프레임으로 메시지 전송하여 메인 화면으로 전환 
		}
	}
}

void CRun_Handler_Check::ButtonCheck_Z_Axis_Up()
{
	if (st_handler.mn_system_lock != FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	if (st_handler.cwnd_motorAxis == NULL &&
		st_handler.cwnd_motor_with_io == NULL )  return;

	BOOL bJogUp = g_ioMgr.get_in_bit( st_io.i_jog_up_switch_chk );

	if( m_bJog_Up_old == bJogUp )	return;

	if( bJogUp )
	{
		if( m_bJog_Dn_old == FALSE )
		{
			int nRet = CTL_Lib.Motor_SafetyCheck(0, MOTOR_ROBOT_Z, 0);
			if( nRet == CTL_DOOROPEN)//2016.0330
			{
				return;
			}
			
			if (nRet == CTLBD_RET_ERROR)
				return;
			
			EMOTRET motRet = GETMOTOR( (ENUM_MOTOR)MOTOR_ROBOT_Z ).JogMoveStart( MINUS );

			if( motRet == MOTRET_ERROR )
			{
				HWND hwnd = NULL;
				if( st_handler.cwnd_motorAxis )
					hwnd = st_handler.cwnd_motorAxis->GetSafeHwnd();
				else if( st_handler.cwnd_motor_with_io )
					hwnd = st_handler.cwnd_motor_with_io->GetSafeHwnd();

				if( hwnd )
					cmmErrShowLast(hwnd);
			}
			else					
				st_handler.mn_menu_lock = FALSE;
		}
		else
		{
			cmmSxStop(MOTOR_ROBOT_Z, FALSE, TRUE);
		}
	}
	else
	{
		cmmSxStop(MOTOR_ROBOT_Z, FALSE, TRUE);
	}
	
	m_bJog_Up_old = bJogUp;

}

void CRun_Handler_Check::ButtonCheck_Z_Axis_Down()
{
	if (st_handler.mn_system_lock != FALSE)  return;  // 현재 시스템 Lock 상태인 경우 리턴 
	if (st_handler.cwnd_motorAxis == NULL &&
		st_handler.cwnd_motor_with_io == NULL )  return;

	BOOL bJogDn = g_ioMgr.get_in_bit( st_io.i_jog_dn_switch_chk );

	if( m_bJog_Dn_old == bJogDn )	return;

	if( bJogDn )
	{
		if( m_bJog_Up_old == FALSE )
		{
			int nRet = CTL_Lib.Motor_SafetyCheck(0, MOTOR_ROBOT_Z, 0);
			if( nRet == CTL_DOOROPEN)//2016.0330
			{
				return;
			}
			
			if (nRet == CTLBD_RET_ERROR)
				return;
			
			EMOTRET motRet = GETMOTOR( (ENUM_MOTOR)MOTOR_ROBOT_Z ).JogMoveStart( PLUS );
			

			if( motRet == MOTRET_ERROR )
			{
				HWND hwnd = NULL;
				if( st_handler.cwnd_motorAxis )
					hwnd = st_handler.cwnd_motorAxis->GetSafeHwnd();
				else if( st_handler.cwnd_motor_with_io )
					hwnd = st_handler.cwnd_motor_with_io->GetSafeHwnd();

				if( hwnd )
					cmmErrShowLast(hwnd);
			}
			else					
				st_handler.mn_menu_lock = FALSE;
		}
		else
		{
			cmmSxStop(MOTOR_ROBOT_Z, FALSE, TRUE);
		}
	}
	else
	{
		cmmSxStop(MOTOR_ROBOT_Z, FALSE, TRUE);
	}

	m_bJog_Dn_old = bJogDn;
}

void CRun_Handler_Check::SafetyChk()
{

//	if( ( g_ioMgr.get_in_bit(st_io.i_emo_switch1_chk) == IO_OFF || g_ioMgr.get_in_bit(st_io.i_emo_switch2_chk) == IO_OFF ) && alarm.mn_emo_alarm != TRUE)
//	{
//		if(i_emo_check_flag == CTL_NO)
//		{
//			i_emo_check_flag = CTL_YES;			// 첫번째 EMO가 감지됐다.
//			EMO_CheckTime[0] = GetCurrentTime();
//		}
//		else
//		{
//			EMO_CheckTime[1] = GetCurrentTime();
//
//			if(EMO_CheckTime[1] < EMO_CheckTime[0])		// 위에 안거치고 온거다. 다시 올라가던가.
//			{
//				i_emo_check_flag = CTL_NO;
//			}
//			else
//			{
//				EMO_CheckTime[2] = EMO_CheckTime[1] - EMO_CheckTime[0];
//
//				if(EMO_CheckTime[2] > 300)		// 0.5초동안 감지되면..
//				{
//					if(COMI.mn_run_status == dINIT)
//					{
//						if(st_handler.cwnd_list != NULL)
//						{
//							sprintf( st_msg.c_abnormal_msg, "[Initialize Error] EMO is not check.");
//							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
//						}
//						st_handler.mn_initial_error = TRUE;
//					}
//
//					i_emo_check_flag = CTL_NO;
//
//					//900200 0 90 "EMO Error."
//					alarm.mstr_code = _T("900200");		// 발생한 알람 코드 설정
//					CTL_Lib.Alarm_Error_Occurrence(933, CTL_dWARNING, alarm.mstr_code);
//					alarm.mn_emo_alarm = TRUE;
//				}
//			}
//		}
//	}
//	else
//	{
//		i_emo_check_flag = CTL_NO;
//	}


	if( COMI.mn_run_status != dRUN )
		return;

	if( g_ioMgr.get_in_bit( st_io.i_bottom_safety_door1_chk ) == IO_ON )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CSD100" );
		CTL_Lib.Alarm_Error_Occurrence( 400, dWARNING, "370100" );
	}
	else if( g_ioMgr.get_in_bit( st_io.i_bottom_safety_door2_chk ) == IO_ON )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CSD200" );
		CTL_Lib.Alarm_Error_Occurrence( 401, dWARNING, "370200" );
	}
	else if( g_ioMgr.get_in_bit( st_io.i_bottom_safety_door3_chk ) == IO_ON )
	{
		// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CSD200" );
		CTL_Lib.Alarm_Error_Occurrence( 401, dWARNING, "370100" );
	}
	else if( g_ioMgr.get_in_bit( st_io.i_bottom_safety_door4_chk ) == IO_ON )
	{
		// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CSD200" );
		CTL_Lib.Alarm_Error_Occurrence( 401, dWARNING, "370200" );
	}


}

