// CtlBd_Library.cpp: implementation of the CCtlBd_Library class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtlBd_Library.h"


#include "io.h"			//2011.0514  파일 존재 유무 검사 함수 호출하기 위해서는 반드시 필요
#include "FAS_HSSI.h"
#include "ComizoaPublic.h"
#include "FastechPublic_IO.h"
#include "CtlBd_Function.h"
#include "IO_Manager.h"
#include "SrcPart/APartHandler.h"

#include "Srcbase\ALocalization.h"//20130930

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtlBd_Library::CCtlBd_Library()
{

}

CCtlBd_Library::~CCtlBd_Library()
{

}

CCtlBd_Library CTL_Lib;

//////////////////////////////////////////////////////////////////////////////
//version:0.1 (2011.1026) 
//알람발생 처리방법, 모터 및 I/O 관련 라이브러리를 초기화 및 응용하여 실제사용하는 함수정리 
//
//참고 기본함수 
//1. #include "ComizoaPublic.h"
//2. #include "FastechPublic_IO.h"
///////////////////////////////////////////////////////////////////////////////

int CCtlBd_Library::Alarm_Error_Occurrence(int n_jampos, int n_run_status, char c_alarmcode[10])
{
	//그간은 알람이 발생하면 각 쓰레드에서 jamcode 및 run status 정보를 바뀌어 장비를 정지하고 
	//문제를 해결했으나 앞으로는 이 함수를 모든곳에서 사용하여 알람정보 및 가동 상태를 표준함수 하나를 
	//사용하여 장비 가동 및 알람 문제를 처리한다 
	//
	//각 쓰레드에 각각있던 알람 처리방법은 이 함수를 모두 사용한다 
   //
	//2013,0123
	if(COMI.mn_run_status == dINIT) alarm.stl_alarm_start_time = GetCurrentTime();
	CTL_Lib.mn_jampos = n_jampos; //jam이 발생한 위치를 저장한다 
	CTL_Lib.mn_run_status = n_run_status; //장비의 가동상태를 변경한다 
	COMI.mn_run_status = n_run_status;
	alarm.mstr_code = c_alarmcode;
	alarm.stl_cur_alarm_time = GetCurrentTime();

	strcpy(mc_alarmcode, c_alarmcode);

//	g_handler.AddAlarmCnt();

	g_ioMgr.set_out_bit( st_io.o_bin_conveyor1, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor2, IO_OFF );
//	g_ioMgr.set_out_bit( st_io.o_bin_conveyor3, IO_OFF ); //2013,0916

	//2012,1220
	// 	CtlBdFunc.Alarm_Error_Occurrence(CTL_Lib.mn_jampos, COMI.mn_run_status, COMI.mn_run_status, alarm.mc_code);
	MyJamData.On_Alarm_Info_Set_to_Variable(alarm.mstr_code);
	
	if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
	{
		sprintf(st_msg.c_abnormal_msg, "[%d] [%s] [%s]", CTL_Lib.mn_jampos, alarm.mstr_code, st_alarm.mstr_cur_msg);
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
	}

	return BD_GOOD;
}

int CCtlBd_Library::Alarm_Error_Occurrence( int n_jampos, int n_run_status, CString strJamCode )
{
	//2013,0123
	if(COMI.mn_run_status == dINIT) alarm.stl_alarm_start_time = GetCurrentTime();
	CTL_Lib.mn_jampos = n_jampos; //jam이 발생한 위치를 저장한다 
	CTL_Lib.mn_run_status = n_run_status; //장비의 가동상태를 변경한다 
	COMI.mn_run_status = n_run_status;
	alarm.mstr_code = strJamCode;
	alarm.stl_cur_alarm_time = GetCurrentTime();

	strcpy(mc_alarmcode, (LPCSTR)strJamCode);

//	g_handler.AddAlarmCnt();

	g_ioMgr.set_out_bit( st_io.o_bin_conveyor1, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor2, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_bin_conveyor3, IO_OFF );

	//2012,1220
// 	CtlBdFunc.Alarm_Error_Occurrence(CTL_Lib.mn_jampos, COMI.mn_run_status, COMI.mn_run_status, alarm.mc_code);
	MyJamData.On_Alarm_Info_Set_to_Variable(alarm.mstr_code);
	
	if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
	{
		sprintf(st_msg.c_abnormal_msg, "[%d] [%s] [%s]", CTL_Lib.mn_jampos, alarm.mstr_code, st_alarm.mstr_cur_msg);
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
	}

	

	return BD_GOOD;
}


int CCtlBd_Library::Initialize_motor_board(int n_bd_type, CString s_filename)	//motor 보드 초기화시 사용
{
	int nRet = 0, i=0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Motor Board Initialize 
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(n_bd_type == 0) //0:파스텍 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{		//모터 보드별 축수를 정의한다 

		COMI.mn_totalmotboard_number = 3;  //모터보드가 3개 있다 
		COMI.mn_totalmotoraxis_number= 18; //모터의 총 수량은 18개이다  
		COMI.mn_bd_axis_number[0] = 8;   //0번 모터보드는 8축용 보드이다  
		COMI.mn_bd_axis_number[1] = 6;   //1번 모터보드는 6축용 보드이다 
		COMI.mn_bd_axis_number[2] = 4;   //2번 모터보드는 4축용 보드이다 
 	}
	else if(n_bd_type == 1) //1:커미조아 보드를 사용하는 장비이면, 1:커미조아 모터를 사용하는 장비 
	{
		//1. 보드 정보 읽어들인다 
		//2. 각종 셋팅 정보를 적용한다 
		nRet = COMI.Initialize_MotBoard(s_filename);			// MOTOR 보드 초기화 여부 검사한다
		if (nRet == BD_ERROR)
		{
			sprintf(mc_normal_msg,"[MOTOR BOARD] 초기화 에러.");
			LogFile_Write("c:\\AMT_LOG\\Motor\\", mc_normal_msg);
			return BD_ERROR;
		}		

		COMI.mn_totalmotboard_number = 3;  //모터보드가 3개 있다 
		COMI.mn_totalmotoraxis_number= 18; //모터의 총 수량은 18개이다  
		COMI.mn_bd_axis_number[0] = 8;   //0번 모터보드는 8축용 보드이다  
		COMI.mn_bd_axis_number[1] = 6;   //1번 모터보드는 6축용 보드이다 
		COMI.mn_bd_axis_number[2] = 4;   //2번 모터보드는 4축용 보드이다 

// 		if(COMI.mn_motorbd_init_end == BD_YES)
// 		{			
// 			for(i=0; i<COMI.mn_totalmotoraxis_number; i++)
// 			{//n_simul_mode => 0:시뮬레이션 모드 비활성, 1:시뮬레이션 모드 활성
// 
// 				if(COMI.mn_simulation_mode == 1) //모터를 가상으로 제어하여 실제 모터 출력이 나가고 기구물이 이동한것처럼 동작한다 
// 				{//활성
// 					COMI.Set_Simulation_Mode(i, 1); 
// 				}
// 				else
// 				{//비활성 
// 					COMI.Set_Simulation_Mode(i, 0); 
// 				}
// 			}		
//		}
	}

	return  BD_GOOD;
}

//아래 내용은 I/O 보드 초기화시 필요한 내용을 예로 들은것이니 
//장비별 필요한 내용을 셋팅하여 사용하면 된다.
int CCtlBd_Library::Initialize_io_Board(int n_bd_type)
{
	//if(n_bd_type == 0) //파스텍 HSSI I/O board 
	//{
	//}
	int i=0,j=0;
	INT HSSI_speed = 0;
	int Ret = 0;
	INT pMasterNo;
	

	/////////////////초기에 초기화시 //////////////////////////////////////////////////////////////////////
	Ret = FAS_IO.Set_IO_BoardOpen(&pMasterNo, START_NOTHING); //START_LOADDATA); //START_NOTHING); //START_CLEAR);
	
	if (Ret == FALSE)
	{
		return BD_ERROR;
	}

	return BD_GOOD;
}

int CCtlBd_Library::Motor_LinearSafety(int n_MapIndex, double *dp_PosList)
{
	double dCurrX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
	double dCurrY = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
	double dCurrT = g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y );

	if( n_MapIndex == M_ROBOT_XY_INDEX )
	{
		//2016.0410
		if(st_handler.mn_mirror_type == CTL_YES )
		{
			if( dCurrX < st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( dp_PosList[0] < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					if( dCurrY > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
					{
						if( dp_PosList[1] >= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
							
						}
						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}

					}
					else if( dCurrY <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
					{
						if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if(  dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
								dCurrT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety5] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{//2016.0410
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety6] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
					{
						if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 100*COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety10] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}//2016.0414
// 						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100  )
// 						{
// 							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
// 							{
// 								alarm.mstr_code = _T("450002");
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
// 								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear XY axis robot move error!" );
// 								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
// 								return BD_ERROR;
// 							}
// 						}
						else if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) &&
							dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety8] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else// if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT])
					{					
						if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety9] Linear XY axis robot move error!" );
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
							return BD_ERROR;
						}					
					}
				}
			}
			else// if( dCurrX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( dp_PosList[0] < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					if( dCurrY > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
					{
						if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety1_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety2_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
					{
						if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if(  dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
								dCurrT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety5_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{//2016.0410
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety6_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
					{
						if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 100*COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety10_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}//2016.0414
// 						else if(  dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100 )
// 						{
// 							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
// 							{
// 								alarm.mstr_code = _T("450002");
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
// 								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear XY axis robot move error!" );
// 								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
// 								return BD_ERROR;
// 							}
// 						}
						else if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) &&
							dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety8_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else// if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT])
					{					
						if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety9_1] Linear XY axis robot move error!" );
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
							return BD_ERROR;
						}					
					}
				}

			}

		}
		else// if(st_handler.mn_mirror_type == CTL_NO)
		{
			if( dCurrX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( dp_PosList[0] > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					if( dCurrY > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
					{
						if( dp_PosList[1] >= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}								
							
						}
						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety1_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety2_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety3_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
					{
						if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if(  dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
								dCurrT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety5_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{//2016.0410
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety6_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
					{
						if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 100*COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety10_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}//20160414
// 						else if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100 ) )
// 						{
// 							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] && 
// 								dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
// 							{
// 								alarm.mstr_code = _T("450002");
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
// 								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7_2] Linear XY axis robot move error!" );
// 								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
// 								return BD_ERROR;
// 							}
// 						}
						else if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) &&
							dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety8_2] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else// if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT])
					{					
						if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_2] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety9_2] Linear XY axis robot move error!" );
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
							return BD_ERROR;
						}					
					}
				}
			}
			else// if( dCurrX < st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( dp_PosList[0] > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					if( dCurrY > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
					{
						if( dp_PosList[1] >= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
							
						}
						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety1_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety2_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety3_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
					{
						if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety0_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300.0f ) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety4_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
								dCurrT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety5_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{//2016.0410
							if( dCurrT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety6_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] && dCurrY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
					{
						if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
						{
							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 100*COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety10_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}//2016.0414
// 						else if(  dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100 ) )
// 						{
// 							if( dCurrT > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] && 
// 								dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
// 							{
// 								alarm.mstr_code = _T("450002");
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
// 								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear XY axis robot move error!" );
// 								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
// 								return BD_ERROR;
// 							}
// 						}
						else if( dp_PosList[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) &&
							dp_PosList[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
						else// if( dp_PosList[1] <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety8_3] Linear XY axis robot move error!" );
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
								return BD_ERROR;
							}
						}
					}
					else// if( dCurrY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT])
					{					
						if( dCurrT < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_3] Linear 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[LSafety9_3] Linear XY axis robot move error!" );
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, MOTOR_ROBOT_Y, "450002", CtlBdFunc.ms_ErrMsg);
							return BD_ERROR;
						}					
					}
				}
			}
		}
	}
	return BD_GOOD;
}

int CCtlBd_Library::Motor_SafetyCheck(int n_mode, int n_axis, double d_targetpos) 	//모터 안전 정보 셋팅시 사용 
{
	//안전 관련하여는 참고하여 장비별 적용한다 

	// 예) 
	// n_Mode = 0:Home Check, 1:Start, 2:Check, 3:Jog, 4:Length Change
	char cJamcode[10] = {NULL};
	int nRet = 0, nRet1 = 0, nRet2 = 0, nRet3 = 0;
	double d_CurPos[4] = {0,};
	double d_Pos[4] = {0,};
	double d_GapCheck = 0;
	int i = 0;

	if( n_axis != MOTOR_ROBOT_Z)
	{
		if(Func.DoorOpenCheckSpot() != CTLBD_RET_GOOD)
		{
			return CTL_DOOROPEN;//2016.0330
		}
	}

// 
//2011.0201 주석처리 	int n_SetTime = 50; //091127 20->50으로 변경 100; //500ms 동안 계속 감기되면 그때 비로소 I/O 상태 return 

	// **************************************************************************
	// 모터 알람 상태 검사한다                                                   
	// -> 알람 발생한 경우 알람 해제한다                                         
	// **************************************************************************
	//software limit 값 셋팅 체크 에러 
	if(d_targetpos < COMI.md_limit_position[n_axis][0] && st_handler.mn_virtual_mode == 0 ) //
	{
		sprintf(cJamcode, "%02d0004", n_axis);
		Alarm_Error_Occurrence(0, CTL_dWARNING, cJamcode);
		
		if (st_handler.cwnd_list != NULL)
		{  // 리스트 바 화면 존재
			sprintf(st_msg.c_abnormal_msg, "Motor[%d] Software Limit Error_1[%4.3f] [%4.3f]", n_axis, COMI.md_limit_position[n_axis][0], d_targetpos);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return BD_ERROR;
	}	
	if(d_targetpos > COMI.md_limit_position[n_axis][1] && st_handler.mn_virtual_mode == 0 ) //
	{
		sprintf(cJamcode, "%02d0004", n_axis);
		Alarm_Error_Occurrence(1, CTL_dWARNING, cJamcode);
		
		if (st_handler.cwnd_list != NULL)
		{  // 리스트 바 화면 존재
			sprintf(st_msg.c_abnormal_msg, "Motor[%d] Software Limit Error_2[%4.3f] [%4.3f]", n_axis, COMI.md_limit_position[n_axis][1], d_targetpos);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return BD_ERROR;
	}

	// **************************************************************************
	// 모터 파워 상태 검사한다                                                   
	// -> 모터 POWER OFF 시 POWER ON 상태로 만든다                               
	// **************************************************************************
	if (g_comiMgr.Get_MotPower(n_axis) == CTL_ERROR )          // 모터 POWER 상태 검사 함수 
	{
		if (COMI.Set_MotPower(n_axis, CTL_ON) == CTLBD_RET_GOOD)       // 모터 POWER ON 설정 함수 
		{
			if (st_handler.cwnd_list != NULL)
			{  // 리스트 바 화면 존재
				sprintf(st_msg.c_normal_msg, "[MotorPowerOn_1] Axis=%d, rcnt=%d", n_axis, st_motor[n_axis].n_retry_cnt);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			if (COMI.Get_MotPower(n_axis) == CTL_ERROR)  // 모터 POWER 상태 검사 함수 
			{
				if (st_motor[n_axis].n_retry_cnt > (MOT_RTY_CNT))
				{
					sprintf(cJamcode, "%02d0005", n_axis);
					Alarm_Error_Occurrence(2, CTL_dWARNING, cJamcode);	
					return BD_ERROR;
				}
				else
				{
					COMI.mn_retry_cnt[n_axis]++ ;
					return BD_RETRY;
				}
			}
		}
		else
		{
			if (st_motor[n_axis].n_retry_cnt > MOT_RTY_CNT)
			{
				printf(cJamcode, "%02d0005", n_axis);
				Alarm_Error_Occurrence(3, CTL_dWARNING, cJamcode);		
				return BD_ERROR;
			}
			else
			{
				COMI.mn_retry_cnt[n_axis]++ ;
				return BD_RETRY;
			}
		}
	}

	//모터 알람 발생시 처리 조건 
	if (COMI.Get_MotAlarmStatus(n_axis) == CTL_ERROR)      // 모터 ALARM 상태 검사 함수 
	{
		if (COMI.Set_MotAlarmClear(n_axis) == CTL_GOOD)       // 모터 ALARM CLEAR 함수 
		{
			//091119 james			Sleep(1000);  // 일정 시간 후에 상태 확인하기 위해 SLEEP 사용한다 
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "[Alarm Reset] Axis=%d, rcnt=%d", n_axis, COMI.mn_retry_cnt[n_axis]);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			if (COMI.Get_MotAlarmStatus(n_axis) == CTL_ERROR )  // 모터 ALARM 상태 검사 함수 
			{
				if (COMI.mn_retry_cnt[n_axis] > MOT_RTY_CNT)
				{			
					sprintf(cJamcode, "%02d0002",  n_axis); alarm.mstr_code = _T(cJamcode); //091216 	alarm.mn_count_mode = MOT_DEBUG;	
					sprintf(alarm.mc_code, "%02d0002", n_axis);
					CtlBdFunc.ms_ErrMsg.Format("[Safety] Mot[%d] rcnt[%d] Get Alarm Check Error", n_axis, COMI.mn_retry_cnt[n_axis]);				
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, cJamcode, CtlBdFunc.ms_ErrMsg);
					//2011.0210 COMI.mn_retry_cnt[n_axis] = 0;  // 알람 해제 시도 횟수 
					return CTL_ERROR;
				}
				else
				{
					COMI.mn_retry_cnt[n_axis]++ ;
					return CTL_RETRY;
				}
			}
		}
		else
		{
			if (COMI.mn_retry_cnt[n_axis] > MOT_RTY_CNT)
			{
				sprintf(cJamcode, "%02d0002",  n_axis); alarm.mstr_code = _T(cJamcode); //091216 	alarm.mn_count_mode = MOT_DEBUG;	
				sprintf(alarm.mc_code, "%02d0002", n_axis);
				CtlBdFunc.ms_ErrMsg.Format("[Safety] Mot[%d] rcnt[%d] Set Alarm Return Error", n_axis, COMI.mn_retry_cnt[n_axis]);				
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, cJamcode, CtlBdFunc.ms_ErrMsg);
				
				//2011.0210 COMI.mn_retry_cnt[n_axis] = 0;  // 알람 해제 시도 횟수 
				return CTL_ERROR;
			}
			else
			{
				COMI.mn_retry_cnt[n_axis]++ ;
				return CTL_RETRY;
			}
		}
	}
	// **************************************************************************

	// **************************************************************************
	// 모터의 안전 위치를 체크한다. 장비에 맞게 넣는것 보다. 딴쪽에 지정해 놓고 불러다 써야 할듯.                                           
	//***************************************************************************

	if(n_axis == MOTOR_ROBOT_X)
	{
		if( (COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 20.f) && 
			((d_targetpos > COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) + 110.f) || (d_targetpos < COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) - 110.f)) )
		{
			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] ||
				COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > 5.0)
			{
				COMI.Set_MotStop(0, MOTOR_ROBOT_X);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x1] 트레이에서 X축이 트레이 거리보다 많이 갈려고함 - Z축을 Safety가 아님" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety and Z Axis must be 5mm under. !!!" );//20130930
				// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);

				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x1)  X:[%.3f] Y:[%.3f] Z:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}

				return CTL_ERROR;
			}
		}
		if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) <= st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 20.f ) 
		{	
			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.f)
			{
				COMI.Set_MotStop(0, MOTOR_ROBOT_X);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x2] 로봇 X축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x2)  X:[%.3f] Y:[%.3f] Z:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
			
		}
		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
// 			if(	d_targetpos < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f)
			if(	d_targetpos < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.0f)//2014,0806
			{		
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				{
					if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ))
					{
						alarm.mstr_code = _T("450002");
						CtlBdFunc.ms_ErrMsg.Format("[Safety1] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930
						// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
				{
					if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) &&
						st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y))
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety2] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety2] X axis robot move error - transfer location not rejected!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			//2014,0717
			if(COMI.mn_run_status == dRUN)
			{
				if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
				{
					if( d_targetpos < 50/* || (g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < 50)*/ )
					{
						alarm.mstr_code = _T("100003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety22] 장비리셋 해주세요(초기화) !!!" );
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety22] Please reset device(Initialization) !!!");//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			if( d_targetpos >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] + 50 )
			{
				alarm.mstr_code = _T("100003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety22] Pos : %.3f X축 최대 값이상로 동작할려고 합니다. !!!", d_targetpos );
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety22] Please reset device(Initialization) !!!");//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
				return CTL_ERROR;
			}

		}
		else
		{
			//2016.0329
//			if(	d_targetpos > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 10.0f)
			if(	d_targetpos > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
// 				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
				{
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
					{
						alarm.mstr_code = _T("450002");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x3] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_x3)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
				else if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
				{
					if( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) &&
						st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) )
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safet_x4] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety2] X axis robot move error - transfer location not rejected!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_x4)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}

				}
			}
			if( d_targetpos >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_END] + 50 )// || (g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) >= 1150) )
			{
				alarm.mstr_code = _T("100003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x5] Pos : %.3f X축 최대 값이상로 동작할려고 합니다. !!!", d_targetpos );
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety22] Please reset device(Initialization) !!!");//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x5)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
		}
		if(COMI.mn_run_status != dINIT)
		{
			if( d_targetpos < -10 )//2014,0726
			{
				alarm.mstr_code = _T("100003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x6] Pos : %.3f X축 최소 값이하로 동작할려고 합니다. !!!", d_targetpos );
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety23] X Axis going to minuum pos(MINUS) !!!");//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x6)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
		}
	}
	if(n_axis == MOTOR_ROBOT_Y)
	{
		
		//2014,0310
		if( (COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 10.f) || 
			(d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 20.f) )
		{
			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - 120.f &&
				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] + 120.f)
			{
			}
			else if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] - 120.f &&
				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] + 120.f)
			{
			}
			else if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - 120.f &&
				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + 120.f)
			{
			}
			else if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] - 120.f &&
				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + 120.f)
			{
			}
			else
			{
				if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] ||
					COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > 5.0)
				{
					COMI.Set_MotStop(0, MOTOR_ROBOT_X);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
					alarm.mstr_code = _T("450003");
 					CtlBdFunc.ms_ErrMsg.Format("[Safety_y1] 로봇 X축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
 					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety ans Z Axis must be 5mm under !!!" );//20130930
 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_y1)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
		}
		if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) <= st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 20.f ) 
		{	
			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.f)
			{
				COMI.Set_MotStop(0, MOTOR_ROBOT_X);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
				COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y2] 로봇 X축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_y2)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
			
		}

		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			//2016.0119
// 			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) < st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 30.f &&
// 				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] + 100.f )
// 			{
// 				if( ( d_targetpos <= st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 30.f ) && 
// 					( d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 5.0f) &&
// 					g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) 
// 				{
// 					alarm.mstr_code = _T("450003");
// 					CtlBdFunc.ms_ErrMsg.Format("[Safety39] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
// 					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety39] X axis robot move error - transfer position is not Bin pos !!!" );
// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
// 					COMI.Set_MotStop(0, MOTOR_ROBOT_X);
// 					COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
// 					return CTL_ERROR;
// 				}
// 				else if( ( d_targetpos <= st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 30.f ) && 
// 					( d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 5.0f) &&
// 					g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > (double)0.0 ) 
// 				{
// 					alarm.mstr_code = _T("450003");
// 					CtlBdFunc.ms_ErrMsg.Format("[Safety40] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
// 					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety40] X axis robot move error - transfer position is not bined !!!" );
// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
// 					COMI.Set_MotStop(0, MOTOR_ROBOT_X);
// 					COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
// 					return CTL_ERROR;
// 				}				
// 			}




			if( st_handler.mn_cok_change == CTL_YES)
			{
				if( g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
				{
					alarm.mstr_code = _T("450003");
					CtlBdFunc.ms_ErrMsg.Format("[Safety33] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safetyss] X axis robot move error - transfer position is not bined !!!" );
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					return CTL_ERROR;
				}
			}
			else
			{
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				{
					if(g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3))
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety5] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety5] X axis robot move error - transfer position is not rejected !!!" );//20130930
						
						// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				{
					if(g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_Y])
// 					if(d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
// 						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 10 )
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety6] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety6] X axis robot move error - transfer position is not rejected !!!" );//20130930
						// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}

		}
		else
		{
			//2016.0119
// 			if( COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y) < st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 50.f &&
// 				COMI.Get_MotCurrentPos(MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + 100.f )
// 			{
// 				if( ( d_targetpos <= st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 30.f ) && 
// 					( d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 5.0f) &&
// 					g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) 
// 				{
// 						alarm.mstr_code = _T("450003");
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety39] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
// 						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety39] X axis robot move error - transfer position is not Bin pos !!!" );
// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
// 						COMI.Set_MotStop(0, MOTOR_ROBOT_X);
// 						COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
// 						return CTL_ERROR;
// 				}
// 				else if( ( d_targetpos <= st_handler.md_common_teaching[CT_ROBOT_COK_END_Y] - 30.f ) && 
// 					( d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 5.0f) &&
// 					g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > (double)0.0 ) 
// 				{
// 						alarm.mstr_code = _T("450003");
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety40] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
// 						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety40] X axis robot move error - transfer position is not bined !!!" );
// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
// 						COMI.Set_MotStop(0, MOTOR_ROBOT_X);
// 						COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
// 						return CTL_ERROR;
// 				}				
// 			}


			//2014,0124
			if( st_handler.mn_cok_change == CTL_YES)
			{
				if( g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
				{
					alarm.mstr_code = _T("450003");
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y3] 로봇 X축 이동 에러 - 트랜스퍼 BIN 위치 아님 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety33] X axis robot move error - transfer position is not bined !!!" );
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_y3)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
			else
			{

				/*if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) > ( st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] ) )
				{
					if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 300) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) >=  st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x17)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						}
						else if(  d_targetpos <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] &&	d_targetpos >= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT])
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x19] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x19)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;

						}
						else if(  d_targetpos <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
								{
									sprintf(st_msg.c_abnormal_msg, "Safety_x18)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
										COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
								}
								return CTL_ERROR;
							}
							
						}
					}
					else if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] ) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) >=  st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x17)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						}
						else if(  d_targetpos > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
						{
							if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
								{
									sprintf(st_msg.c_abnormal_msg, "Safety_x17)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
										COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
								}
								return CTL_ERROR;
							
						}
						else if(  d_targetpos <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
								{
									sprintf(st_msg.c_abnormal_msg, "Safety_x18)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
										COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
								}
								return CTL_ERROR;
							}
							
						}
					}
					else if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							alarm.mstr_code = _T("450002");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x17)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						}
						else if(  d_targetpos > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] )
						{
							if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
								{
									sprintf(st_msg.c_abnormal_msg, "Safety_x17)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
										COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
								}
								return CTL_ERROR;
								
							}
						}
						else if(  d_targetpos < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] )
						{
							if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
							{
								alarm.mstr_code = _T("450002");
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
								{
									sprintf(st_msg.c_abnormal_msg, "Safety_x18)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
										COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
								}
								return CTL_ERROR;
							}
							
						}
					}
				}*/
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 100.0f &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				{//2013,1220
					if( g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3) + 5.0f)
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y4] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y4] X axis robot move error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_y4)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
				if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 100.0f &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_Y] + 5.0f)
				{//2013,1220
					if(g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_Y])
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y5] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y5] X axis robot move error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_y5)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
			}
		}

	}

 	switch( n_axis )
	{
	case MOTOR_BIN_1_Z:
		break;

	case MOTOR_BIN_1_R:
		if( abs( st_motor[MOTOR_BIN_1_Z].d_pos[P_BIN_Z_ROTATE] - g_comiMgr.Get_MotCurrentPos( MOTOR_BIN_1_Z ) ) > COMI.md_allow_value[MOTOR_BIN_1_Z] )
		{
			alarm.mstr_code = _T("451100");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 공급1 회전축 이동 에러 - Z축 회전 위치 아님 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Supply1 axis of rotation to move the error - Z axis rotational position is not !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "451100", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		break;

	case MOTOR_BIN_2_Z:
		break;

	case MOTOR_BIN_2_R:
		if( abs( st_motor[MOTOR_BIN_2_Z].d_pos[P_BIN_Z_ROTATE] - g_comiMgr.Get_MotCurrentPos( MOTOR_BIN_2_Z ) ) > COMI.md_allow_value[MOTOR_BIN_2_Z] )
		{
			alarm.mstr_code = _T("451200");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 공급2 회전축 이동 에러 - Z축 회전 위치 아님 !!!" );
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Supply2 axis of rotation to move the error - Z axis rotational position is not !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "451200", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		break;

	case MOTOR_REJECT_Z:
		break;

	case MOTOR_REJECT_R:
		if( abs( st_motor[MOTOR_REJECT_Z].d_pos[P_BIN_Z_ROTATE] - g_comiMgr.Get_MotCurrentPos( MOTOR_REJECT_Z ) ) > COMI.md_allow_value[MOTOR_REJECT_Z] )
		{
			alarm.mstr_code = _T("451300");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 공급3 회전축 이동 에러 - Z축 회전 위치 아님 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Supply3 axis of rotation to move the error - Z axis rotational position is not !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "451300", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		break;

	case MOTOR_TRAY_TR_Y:
		if( g_ioMgr.get_out_bit( st_io.o_reject_transfer_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_reject_transfer_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_reject_transfer_up_chk, IO_ON ) == IO_OFF )
		{
			alarm.mstr_code = _T("453100");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 트랜스퍼 Y축 이동 에러 - 트랜스퍼 업상태 아님 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Y axis error - transfer state is not up !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453100", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		//2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			//ybs
			if(COMI.mn_run_status == dINIT)
			{
				if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_X] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
					st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) )
				{
					if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
					{
						alarm.mstr_code = _T("453101");
						CtlBdFunc.ms_ErrMsg.Format("[Safety] 트랜스퍼 Y축 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Y axis error - X axis robot collision risk !!!" );//20130930
						// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMTY01", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453101", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			else
			{
				if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_X] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
					st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) )
				{
					alarm.mstr_code = _T("453101");
					CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] 트랜스퍼 Y축 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] Transfer Y axis error - X axis robot collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453101", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_tr)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] Target_tr:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
		}
		else
		{
			if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_X] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
				st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) )
			{
				alarm.mstr_code = _T("453101");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] 트랜스퍼 Y축 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Y axis error - X axis robot collision risk !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453101", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_tr)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] Target_tr:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
		}

		break;

	case MOTOR_TRAY_TR_CLAMP:
		//2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + 10.f)
			{				
				//ybs
				if(COMI.mn_run_status == dINIT)
				{
					if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.f))
					{
						if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
						{
							alarm.mstr_code = _T("454100");
							CtlBdFunc.ms_ErrMsg.Format("[Safety] 트랜스퍼 클램프 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Clamp axis error - X axis robot collision risk !!!" );//20130930
							// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMTC01", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "454100", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
				else
				{
// 					if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.f))
					if((g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.f)) &&
						(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.f)))
					{
						alarm.mstr_code = _T("454100");
						CtlBdFunc.ms_ErrMsg.Format("[Safety] 트랜스퍼 클램프 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Clamp axis error - X axis robot collision risk !!!" );//20130930
						// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMTC01", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "454100", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			
		}
		else
		{
			if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + 10.0f)
			{
// 				if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.0f))
				if((g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.f)) &&
					(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.f)))
				{
					alarm.mstr_code = _T("454100");
					CtlBdFunc.ms_ErrMsg.Format("[Safety_tr_clamp] 트랜스퍼 클램프 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_tr_clamp] Transfer Clamp axis error - X axis robot collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "454100", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_tr_clamp)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] Target_clamp:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
		}		
// 		if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
// 			st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) )
// 		{
// 			alarm.mstr_code = _T("454100");
// 			CtlBdFunc.ms_ErrMsg.Format("[Safety] 트랜스퍼 클램프 이동 에러 - 로봇 X축 충돌 위험 !!!" );				
// // 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMTC01", CtlBdFunc.ms_ErrMsg);
// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "454100", CtlBdFunc.ms_ErrMsg);
// 			return CTL_ERROR;
// 		}
		break;

	case MOTOR_TRAY_STACKER:

		break;

	case MOTOR_ROBOT_Z:
		if( COMI.mn_run_status == dRUN && 
			(g_ioMgr.get_out_bit( st_io.o_gripper_updn, IO_OFF ) == IO_ON  ||
			g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_gripper_up_chk, IO_ON ) == IO_OFF) )
		{
			alarm.mstr_code = _T("450200");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 Z축 이동 에러 - 그리퍼 다운상태 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Z axis robot error - gripper down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450200", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3)) )
		{
			if(d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.0f)
			{//470000 1 A "로봇 Z축 세이프티 체크 에러."
				alarm.mstr_code = _T("470000");
				CtlBdFunc.ms_ErrMsg.Format("[Safety15] 로봇 Z축 안전 위치보다 더 내려갈려고 한다(최대 : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.0f);				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety15] Z axis robot is going to go down more than a safe position(MAX : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.0f);//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "470000", CtlBdFunc.ms_ErrMsg);
				return CTL_ERROR;
			}
		}
		if((g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG] - 5.0f)) && 
			(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG] + 5.0f)) &&
			(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG] - 5.0f)) && 
			(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG] + 5.0f)) )
		{
			if( (d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK] + 5.0f) || (d_targetpos > 50.0f))
			{//470000 1 A "로봇 Z축 세이프티 체크 에러."
				alarm.mstr_code = _T("470000");
				CtlBdFunc.ms_ErrMsg.Format("[Safety16] Align Buffer에 로봇 Z축 안전 위치보다 더 내려갈려고 한다(최대 : %.3f or 30.0!!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK] + 5.0f);				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety16] Align Buffer safe position in the Z axis robot is going to go down more than(MAX : %.3f or 30.0!!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK] + 5.0f);//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "470000", CtlBdFunc.ms_ErrMsg);
				return CTL_ERROR;
			}
		}

		if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - 10.0f &&
			g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] + 10.0f)
		{
			if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] - 10.0f &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] + 10.0f)
			{
				if(d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f)
				{//470000 1 A "로봇 Z축 세이프티 체크 에러."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety17] 로봇 Z축 안전 위치보다 더 내려갈려고 한다(최대 : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety17] Z axis robot is going to go down more than a safe position(MAX : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "470000", CtlBdFunc.ms_ErrMsg);
					return CTL_ERROR;
				}

			}
		}

		if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - 10.0f &&
			g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] + 10.0f)
		{
			if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] - 10.0f &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_START] + 10.0f)
			{
				if(d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE2_PICK_END] + 10.0f)
				{//470000 1 A "로봇 Z축 세이프티 체크 에러."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety18] 로봇 Z축 안전 위치보다 더 내려갈려고 한다(최대 : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety18] Z axis robot is going to go down more than a safe position(MAX : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "470000", CtlBdFunc.ms_ErrMsg);
					return CTL_ERROR;
				}
				
			}
		}

		if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.0f &&
			g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + 10.0f)
		{
			if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.0f &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] + 10.0f)
			{
				if(d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE] + 10.0f)
				{//470000 1 A "로봇 Z축 세이프티 체크 에러."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety19] 로봇 Z축 안전 위치보다 더 내려갈려고 한다(최대 : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE] + 10.0f);				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety19] Z axis robot is going to go down more than a safe position(MAX : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE] + 10.0f);//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "470000", CtlBdFunc.ms_ErrMsg);
					return CTL_ERROR;
				}
				
			}
		}

// 		if( COMI.mn_run_status == dRUN &&
// 			(g_ioMgr.get_out_bit( st_io.o_picker1_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_out_bit( st_io.o_picker2_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_out_bit( st_io.o_picker3_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_out_bit( st_io.o_picker4_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_in_bit( st_io.i_picker1_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker2_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker3_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker4_dn_chk, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_in_bit( st_io.i_picker1_up_chk, IO_ON ) == IO_OFF || 
// 			g_ioMgr.get_in_bit( st_io.i_picker2_up_chk, IO_ON ) == IO_OFF || 
// 			g_ioMgr.get_in_bit( st_io.i_picker3_up_chk, IO_ON ) == IO_OFF || 
// 			g_ioMgr.get_in_bit( st_io.i_picker4_up_chk, IO_ON ) == IO_OFF) )
// 		{
// 			alarm.mstr_code = _T("450201");
// 			CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 Z축 이동 에러 - 피커 다운상태 !!!" );				
// // 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0Z01", CtlBdFunc.ms_ErrMsg);
// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450201", CtlBdFunc.ms_ErrMsg);
// 			return CTL_ERROR;
// 		}
		break;

	//2014,0717
	case MOTOR_PITCH_Z:
		if( COMI.mn_run_status != dRUN )
		{
			if( st_motor[MOTOR_ROBOT_Z].d_pos[P_CZ_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			{
// 				alarm.mstr_code = _T("450003");
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety90] 로봇 X축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
// 				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety !!!" );//20130930
// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
// 				return CTL_ERROR;
			}
		}
		break;

	case MOTOR_ROBOT_X:
		if( COMI.mn_run_status != dRUN )
		{
			if( st_motor[MOTOR_ROBOT_Z].d_pos[P_CZ_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			{
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x7] 로봇 X축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x7] X axis robot move error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				return CTL_ERROR;
			}
		}

		if( g_ioMgr.get_out_bit( st_io.o_gripper_updn, IO_OFF ) == IO_ON  ||
			g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_gripper_up_chk, IO_ON ) == IO_OFF )
		{
			alarm.mstr_code = _T("450000");
			CtlBdFunc.ms_ErrMsg.Format("[Safety_x8] 로봇 X축 이동 에러 - 그리퍼 다운상태 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x8] X axis robot move error - gripper down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450000", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		if( g_ioMgr.get_out_bit( st_io.o_picker1_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker2_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker3_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker4_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_in_bit( st_io.i_picker1_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker2_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker3_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker4_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_picker1_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker2_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker3_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker4_up_chk, IO_ON ) == IO_OFF )
		{
			CtlBdFunc.ms_ErrMsg.Format("[Safety_x9] 로봇 X축 이동 에러 - 피커 다운상태 !!!" );				

			alarm.mstr_code = _T("450001");
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x9] X axis robot move error - Picker down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450001", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
// 			if( st_handler.mn_cok_change == CTL_YES && 
// 				(d_targetpos < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 50.f) &&
// 				(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f )) )
			if( st_handler.mn_cok_change == CTL_YES && 
				(d_targetpos < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X] ) &&
				(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f )) )
			{
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety30] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );	
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety30] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				return CTL_ERROR;
				
			}

// 			if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f && 
// 				d_targetpos <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f)
			if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f && 
				d_targetpos < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X])
			{			
				if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + COMI.md_allow_value[MOTOR_ROBOT_Y])
				{
					if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ))
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety7] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety7] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
						// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
				{
					if(CHINA_VER == 1)
					{
						if( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) &&
							st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) )
						{
							alarm.mstr_code = _T("450003");
							CtlBdFunc.ms_ErrMsg.Format("[Safety8] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety8] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
					else
					{
						if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ))
						{
							alarm.mstr_code = _T("450003");
							CtlBdFunc.ms_ErrMsg.Format("[Safety8] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety8] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) <  st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_ROBOT_Y])
				{
					//CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 X축 이동 에러 - 트랜스퍼 위치 아님 !!!" );				
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					//return CTL_ERROR;
				}
			}
			if(d_targetpos < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f)
			{
				if( (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] - 5.0f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
					(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) &&
					st_handler.mn_virtual_mode == 0 )
				{
					//ybs
					//2014,0717//문제가 됨 cok 교체
 					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 10.f) )
					{
						if( st_handler.mn_cok_change != CTL_YES)//2014,1106
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety9] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety9] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}//2014,0714
				if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
					((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_ROBOT_Y]) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
					st_handler.mn_virtual_mode == 0 )
				{
					if( CHINA_VER == 1)
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) &&
							 g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - (COMI.md_allow_value[MOTOR_TRAY_TR_Y]* 5) )
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety10] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety10] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) )
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety10] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety10] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
			}
			if( (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 30.0f + COMI.md_allow_value[MOTOR_ROBOT_X] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
				(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - 20.f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
				(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 50.f < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) ))
			{
				if(d_targetpos > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.f)
				{
					alarm.mstr_code = _T("450004");
					CtlBdFunc.ms_ErrMsg.Format("[Safety77] 로봇 X축 이동 에러 - 트랜스퍼 클램프 세이프티 위치 아님 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - not a transfer clamp safety position !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450004", CtlBdFunc.ms_ErrMsg);
					return CTL_ERROR;
				}
			}
			
			if( (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 30.0f + COMI.md_allow_value[MOTOR_ROBOT_X] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
				(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) )
			{
				if(d_targetpos > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.f &&
					st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 10.f <= g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ))
				{
					if(	st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 10.f  < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) &&
						st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.f  > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) )

					{
						alarm.mstr_code = _T("450004");
						CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 X축 이동 에러 - 트랜스퍼 클램프 세이프티 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - not a transfer clamp safety position !!!" );//20130930
						// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X04", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450004", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
				else if(d_targetpos > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.f &&
					st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 10.f > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ))
				{
					if(	st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.f  > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y))
					{
						alarm.mstr_code = _T("450004");
						CtlBdFunc.ms_ErrMsg.Format("[Safety66] 로봇 X축 이동 에러 - 트랜스퍼 클램프 세이프티 위치 아님 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - not a transfer clamp safety position !!!" );//20130930
						// 			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X04", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450004", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}

				}
			}
		}
		else
		{
			if( st_handler.mn_cok_change == CTL_YES && 
				(d_targetpos > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) &&
				(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f )) )
			{
				alarm.mstr_code = _T("450003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x10] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );	
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x10] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x10)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
				
			}
			if(	g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] && 
				d_targetpos > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X])
			{			
				if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + COMI.md_allow_value[MOTOR_ROBOT_Y])
				{
					if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ))
					{
						alarm.mstr_code = _T("450003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] 로봇 X축 이동 에러 - 트랜스퍼 리젝트 위치 아님 !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_x11)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
				{
					if( CHINA_VER == 1)
					{
						if( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) &&
							st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) )
						{
							alarm.mstr_code = _T("450003");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x11)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						}
					}
					else
					{
						if(st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ))
						{
							alarm.mstr_code = _T("450003");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x12] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x12] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x12)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						}
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) <  st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_ROBOT_Y])
				{
					//CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 X축 이동 에러 - 트랜스퍼 위치 아님 !!!" );				
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					//return CTL_ERROR;
				}

			}

			if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] + 5.0f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
				st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) )
			{
	//ㅛㅠㄴ			CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 X축 이동 에러 - 트랜스퍼 공급 위치 아님 !!!" );				
	//			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
	//			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
	//			return CTL_ERROR;
			}

//			if(d_targetpos > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_START] + COMI.md_allow_value[MOTOR_ROBOT_X])
			//2016.0329
			if(d_targetpos > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X])
			{
// 				if( (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
// 					(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) &&
// 					st_handler.mn_virtual_mode == 0 )
				//2016.0329
				if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f) &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )  &&
					st_handler.mn_virtual_mode == 0 )
				{
					//ybs
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) )
					{
						//if( st_handler.mn_cok_change != CTL_YES)//2014,1106
						//{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety_13] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_13] XY axis robot move error - transfer collision risk !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
							{
								sprintf(st_msg.c_abnormal_msg, "Safety_x13)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
									COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
							}
							return CTL_ERROR;
						//}

					}
				}
// 				if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X]) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 					((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_ROBOT_Y]) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
// 					st_handler.mn_virtual_mode == 0 )
				//2016.0329
				if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
					( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) ) &&
					( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y]) ) )
				{
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{
						alarm.mstr_code = _T("455000");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x14] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x14] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_x14)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
				//2016.0329
				if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
					( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > (st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 10*COMI.md_allow_value[MOTOR_ROBOT_Y]) ) &&
					( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200.0f ) ) )
				{
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{
						alarm.mstr_code = _T("455000");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x15] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x15] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_x15)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
			}

// 			if( (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
// 				((st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_CLAMP]) > g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_CLAMP ) ) )
			//2016.0329
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > ( st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] ) &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_STACKER_END] - COMI.md_allow_value[MOTOR_ROBOT_Y] ) &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_CLAMP ) < ( st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_CLAMP] ) )
			{
				alarm.mstr_code = _T("450004");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x16] 로봇 X축 이동 에러 - 트랜스퍼 클램프 세이프티 위치 아님 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x16] X axis robot move error - not a transfer clamp safety position !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450004", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_x16)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetX:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
		}



		break;

	case MOTOR_ROBOT_Y:
		//2016.0329
//		if( d_targetpos >= st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] + 50*3 + 10 )
		if( d_targetpos >= st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] + 50*3 + 10)
		{
			alarm.mstr_code = _T("450103");
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y6]pos : %.3f 로봇 Y축 이동 에러 - Y축 최대위치를 벗어날려고 함 !!!",d_targetpos );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y6] Y axis robot error - Limit Pos exceed!!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_abnormal_msg, "Safety_y6)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
					COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}
			return CTL_ERROR;

		}
		if( COMI.mn_run_status != dINIT )
		{
			if( d_targetpos < -10 )//2014,0726
			{
				alarm.mstr_code = _T("450103");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y7]pos : %.3f 로봇 Y축 이동 에러 - Y축 최소위치를 벗어날려고 함 !!!",d_targetpos );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y7] Y axis robot error - Limit Pos exceed!!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_y7)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
				
			}
		}
		if( COMI.mn_run_status != dRUN )
		{
			if( st_motor[MOTOR_ROBOT_Z].d_pos[P_CZ_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
			{
				alarm.mstr_code = _T("450103");
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y8] 로봇 Y축 이동 에러 - Z축 Safety 위치 아님 !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y8] Y axis robot error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "Safety_y8)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
						COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				return CTL_ERROR;
			}
		}

		if( g_ioMgr.get_out_bit( st_io.o_gripper_updn, IO_OFF ) == IO_ON  ||
			g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_gripper_up_chk, IO_ON ) == IO_OFF )
		{
			alarm.mstr_code = _T("450100");
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] 로봇 Y축 이동 에러 - 그리퍼 다운상태 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] Y axis robot error - Gripper down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450100", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_abnormal_msg, "Safety_y9)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
					COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}
			return CTL_ERROR;
		}

		if( g_ioMgr.get_out_bit( st_io.o_picker1_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker2_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker3_updn, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_picker4_updn, IO_OFF ) == IO_ON ||
// 			g_ioMgr.get_in_bit( st_io.i_picker1_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker2_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker3_dn_chk, IO_OFF ) == IO_ON || 
// 			g_ioMgr.get_in_bit( st_io.i_picker4_dn_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_picker1_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker2_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker3_up_chk, IO_ON ) == IO_OFF || 
			g_ioMgr.get_in_bit( st_io.i_picker4_up_chk, IO_ON ) == IO_OFF )
		{
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] 로봇 Y축 이동 에러 - 피커 다운상태 !!!" );	
			alarm.mstr_code = _T("450101");
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] Y axis robot error - Picker down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450101", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_abnormal_msg, "Safety_y9)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
					COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}
			return CTL_ERROR;
		}

// 		if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 10.0f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
// 			st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 5.0f > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) &&
// 			st_handler.mn_virtual_mode == 0 )
// 		{
// 			//ybs
// 			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
// 			{
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
// 				// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
// 				return CTL_ERROR;
// 			}
// 		}
// 
// 		if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 10.0f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
// 			st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 5.0f > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) &&
// 			st_handler.mn_virtual_mode == 0 )
// 		{
// 			//ybs
// 			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
// // 			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y]*5) )
// 			{
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
// // 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
// 				return CTL_ERROR;
// 			}
// 		}

		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 50.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
				( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) )
			{
				//ybs
				if(COMI.mn_run_status == dINIT)
				{
					if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
					{
						if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety11] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety11] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
				else
				{
					//2014,0717
//// 					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
// 					if( d_targetpos < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f &&
// 						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 10.f &&
// 						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f &&
// 						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 10.0f)
// 					{
// 						alarm.mstr_code = _T("455000");
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety11] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
// 						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety11] XY axis robot move error - transfer collision risk !!!" );//20130930
// 						// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
// 						return CTL_ERROR;
// 					}
					//2016.0329
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
					{
						if( d_targetpos < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] &&
							g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f &&
							g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 10.0f)
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety11] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety11] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
			}
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				(d_targetpos < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f)) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )) &&
				( d_targetpos < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
				( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] )) )
			{
				//ybs
				if(COMI.mn_run_status == dINIT)
				{
					if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
					{
						if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
						{
							CtlBdFunc.ms_ErrMsg.Format("[Safety12] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety12] XY axis robot move error - transfer collision risk !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
				else
				{//2014,0717
// 					if(	g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - 50 &&
						(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.0f) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 10.0f) ) )
					{
						if( d_targetpos < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f)
						{
							CtlBdFunc.ms_ErrMsg.Format("[Safety12] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety12] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;

						}
					}
					else if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_START] &&
						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - 50 &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 10.0f ) )
					{
						CtlBdFunc.ms_ErrMsg.Format("[Safety12] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety12] XY axis robot move error - transfer collision risk !!!" );//20130930
						// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
				((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_ROBOT_Y]) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
				st_handler.mn_virtual_mode == 0 )
			{
				//ybs
				if(COMI.mn_run_status == dINIT)
				{
					if( CHINA_VER == 1 )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 )) &&
							g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
							{
								CtlBdFunc.ms_ErrMsg.Format("[Safety13] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety13] XY axis robot move error - transfer collision risk !!!" );//20130930
								// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
								return CTL_ERROR;
							}
						}
					}
					else
					{
						//2014,0725
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5) )
						{
							if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
							{
								CtlBdFunc.ms_ErrMsg.Format("[Safety13] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety13] XY axis robot move error - transfer collision risk !!!" );//20130930
								// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
								return CTL_ERROR;
							}
						}
					}
				}
				else
				{
// 					if(	g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5) )
					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 )) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 ) ) )
					{
						CtlBdFunc.ms_ErrMsg.Format("[Safety13] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety13] XY axis robot move error - transfer collision risk !!!" );//20130930
						// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}

// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				(d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END]) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
				(d_targetpos > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
			{
				//ybs
				if(COMI.mn_run_status == dINIT)
				{
					if( CHINA_VER == 1 )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 )) &&
							g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
							{
								CtlBdFunc.ms_ErrMsg.Format("[Safety14] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety14] XY axis robot move error - transfer collision risk !!!" );//20130930
								// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
								return CTL_ERROR;
							}
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5) )
						{
							if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_OFF)
							{
								CtlBdFunc.ms_ErrMsg.Format("[Safety14] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety14] XY axis robot move error - transfer collision risk !!!" );//20130930
								// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
								return CTL_ERROR;
							}
						}
					}
				}
				else
				{
					if( CHINA_VER == 1 )
					{
						if(	g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5) &&
							g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							CtlBdFunc.ms_ErrMsg.Format("[Safety14] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety14] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
					else
					{
						if(	g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5) )
						{
							CtlBdFunc.ms_ErrMsg.Format("[Safety14] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety14] XY axis robot move error - transfer collision risk !!!" );//20130930
							// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MMXY00", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
			}
		}
		else
		{
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 50.f) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f) > g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
				( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) )
			{
				if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
				{
// 					if( d_targetpos < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f &&
// 						d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 10.f &&
// 						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f &&
// 						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 10.0f)
					//2016.0329
					if( d_targetpos < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 5.0f &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - 10.0f)
					{
						alarm.mstr_code = _T("455000");
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y10] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y10] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
						{
							sprintf(st_msg.c_abnormal_msg, "Safety_y10)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
								COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
						}
						return CTL_ERROR;
					}
				}
				
			}
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 50.f) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				(d_targetpos < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 50.f)) &&
// 				st_handler.mn_virtual_mode == 0 && 
// 				(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - 10.f)) )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )) &&
				( d_targetpos < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
				( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y] )) )
			{
				if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - 5.0f/*(COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)*/) )
				{
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y11] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y11] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_y11)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
			//2014,0124
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 100.f) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END] - COMI.md_allow_value[MOTOR_ROBOT_Y]) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
				( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) > (st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] - COMI.md_allow_value[MOTOR_ROBOT_Y]) ) )
			{
				if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 )) &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ))
				{
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y12] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y12] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_y12)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
// 			if( ((st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 10.f) < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) &&
// 				(d_targetpos > st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END]) &&
// 				st_handler.mn_virtual_mode == 0 )
			//2016.0329
			if( ( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > (st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
				(d_targetpos > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y]) )
			{
				if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5 )) &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
				{
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y13] 로봇 XY축 이동 에러 - 트랜스퍼 충돌 위험 !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y13] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_abnormal_msg, "Safety_y13)  X:[%.3f] Y:[%.3f] Z:[%.3f] RejY:[%.3f] TargetY:[%.3f]", 
							COMI.Get_MotCurrentPos(MOTOR_ROBOT_X), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y), COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z), g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ), d_targetpos);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
					}
					return CTL_ERROR;
				}
			}
		}
		break;

	case MOTOR_TESTSITE_1_Z:
		if( g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_fwd, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_testsite1_cok_plate_bwd, IO_ON ) == IO_OFF ||
			g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_fwd_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_testsite1_cok_plate_bwd_chk, IO_ON ) == IO_OFF )
		{
			alarm.mstr_code = _T("452100");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 테스트사이트1 Z축 이동 에러 - 플레이트 전진상태 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Test Site 1 Z axis movement error - plate advances the state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "452100", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		break;

	case MOTOR_TESTSITE_2_Z:
		if( g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_fwd, IO_OFF ) == IO_ON ||
			g_ioMgr.get_out_bit( st_io.o_testsite2_cok_plate_bwd, IO_ON ) == IO_OFF ||
			g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_fwd_chk, IO_OFF ) == IO_ON ||
			g_ioMgr.get_in_bit( st_io.i_testsite2_cok_plate_bwd_chk, IO_ON ) == IO_OFF )
		{
			alarm.mstr_code = _T("452200");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] 테스트사이트2 Z축 이동 에러 - 플레이트 전진상태 !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Test Site 2 Z axis movement error - plate advances the state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "452200", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		break;

	case MOTOR_COK_ROBOT_Y:
		if( st_handler.md_common_teaching[CT_COK_Z_SAFETY] + COMI.md_allow_value[MOTOR_COK_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_COK_ROBOT_Z ) )
		{
			alarm.mstr_code = _T("450300");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] COK 로봇 Y축 이동 에러 - Z축 세이프티 위치 아님. !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Y axis robot error COK - Z axis is not a safety position !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450300", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		break;

	case MOTOR_COK_ROBOT_Z:
		break;
	}

	//2011.0201 COMI.mn_retry_cnt[n_axis] = 0;
	st_motor[n_axis].mn_retry_time_flag = CTL_NO;//2011.0201 추가 

	return BD_GOOD;
}

BOOL CCtlBd_Library::CreateFolder(LPCTSTR szPath)
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

int CCtlBd_Library::FileSizeCheck(char * cp_filename, int n_size, int n_check)
{
	CFileFind finder;
	long nLen=0, nflag=0;
	BOOL bContinue;
	
	if(bContinue = finder.FindFile(cp_filename))
	{	
		if (n_check == BD_YES)			// Size를 체크하는것이면...
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

int CCtlBd_Library::LogFile_Write(char * cp_dir_path, char * cp_msg)
{
//2011.0417	sing.Lock(); //2011.0417 

	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // 현재 년, 월, 일 정보 문자형으로 변환하여 저장할 변수 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // 현재 년, 월, 일 정보 저장 변수 
	CString mstr_file_name, mstr_dir_path, str_msg;		// 마지막으로 생성된 파일 이름 저장 변수 
	CString mstr_create_file;	// 알람 정보 저장할 파일에 대한 [폴더]+[파일명]+[확장자] 설정 변수 
	CString mstr_list_name, mstr_temp_data;  // 각 부분별 알람 발생 횟수 정보 저장할 로그 파일 정보 저장 변수 
	CString mstr_content;		// 한 라인에 출력할 문자열 정보 저장 변수 
	COleDateTime time_cur;		// 검사할 시간 정보 저장 변수 
	CTime m_time_current;		// 간략한 헝식의 현재 시간 정보 저장 변수
	char chr_buf[20]={0,};
	int mn_existence, nlength;			// 파일 존재 유무 설정 플래그 
	char cfileName[256]={0,};			// 검색할 파일 정보 설정 함수 
	FILE  *fp ;					// 파일에 대한 포인터 설정 변수 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	int Ret=0;

	nlength = strlen(cp_msg);
	if(nlength <= 0) //저장할 메세지가 없다 
	{
		return BD_ERROR;
	}
	str_msg = cp_msg;
 
	// **************************************************************************
	// 파일 이름으로 사용할 날짜 정보를 얻는다                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // 현재 시간 정보를 얻는다. 

	m_time_current = CTime::GetCurrentTime() ;  // 간략한 형식의 현재 시간 정보 얻는다. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// 날짜 정보를 문자형으로 변환하여 변수에 설정한다                           
	// **************************************************************************
	mstr_cur_year = LPCTSTR(_itoa( mn_cur_year, chr_buf, 10 ));  
	mstr_cur_month.Format("%02d", mn_cur_month);
	mstr_cur_day.Format("%02d", mn_cur_day);

	// **************************************************************************
	// 현재 시간 정보 얻는다                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// 현재 시간 정보를 설정한다. 
	mstr_cur_hour.Format("%d",time_cur.GetHour());	// 현재 시간 정보를 설정한다.
	mstr_cur_min.Format("%d",time_cur.GetMinute());	// 현재 분 정보를 설정한다. 
	mstr_cur_sec.Format("%d",time_cur.GetSecond());	// 현재 초 정보를 설정한다. 
	str_display_time = m_time_current.Format("%c");	// 리스트 파일에 출력할 타이틀 시간 정보 설정 
	// **************************************************************************


	mstr_dir_path = cp_dir_path;
	mstr_file_name = mstr_cur_year;
	mstr_file_name += mstr_cur_month; 
	mstr_file_name += mstr_cur_day; 
	mstr_create_file = mstr_dir_path + mstr_file_name;

	mstr_create_file += ".TXT";

	sprintf(cfileName, "%s", mstr_create_file);
	mn_existence = access(cfileName,0) ;	
	if (mn_existence == -1)
	{//해당 파일이 존재하지 않는 상태이다 
		CreateFolder(cp_dir_path); 
	}
	else 
	{//해당 파일이 이미 존재한다.
		//정상 
	}
	
	Ret = FileSizeCheck(cfileName, 1048576, BD_YES); //size and rename	
	if(Ret == 1) //file size over
	{
		BackupName = mstr_create_file + mstr_cur_hour + mstr_cur_min +  mstr_cur_sec + ".bak" ;
		rename(mstr_create_file, BackupName);
	}

	if ((fp = fopen(mstr_create_file,"a+")) == NULL)  
	{
		//AfxMessageBox("File open failure!..");
		sprintf(mc_normal_msg,"LogFile_Write a+ Error"); 
		
 		return BD_ERROR;
	}
	// **************************************************************************
	
	mstr_content += str_display_time;
	mstr_content += " : " + str_msg;

	fprintf(fp,"%s\n",mstr_content) ;
	if (ferror(fp))  
	{
		sprintf(mc_normal_msg,"LogFile_Write ferror return error");

		clearerr(fp); //2011.0418 

		fclose(fp) ;  //2011.0417  파일 종료

//2011.0417		sing.Unlock(); //2011.0417 추가 
		return BD_ERROR;
	}
	// **************************************************************************
	fclose(fp);  // 파일을 종료한다.

//2011.0417	sing.Unlock(); //2011.0417 추가 
	return BD_GOOD;
}

int CCtlBd_Library::Single_Move(int n_MotNum, double d_MotPos, int n_SpeedRate)
{
	int nFuncRet = BD_PROCEED;
	int nRet;
	
	switch(mn_single_motmove_step[n_MotNum])
	{
	case 0:
		nRet = COMI.Check_MotPosRange(n_MotNum, d_MotPos, COMI.md_allow_value[n_MotNum]);
		if (nRet == BD_GOOD) //이미 해당 위치에 와 있으면 동작하지 않고 모터 이동을 끝낸다 
		{
			nFuncRet = BD_GOOD;
			break;
		}
		
		mn_mot_retry_cnt[n_MotNum] = 0;
		mn_mot_max_retry_cnt = 3; //최대 3회 리트라이 동작을 수행한다 
		mn_single_motmove_step[n_MotNum] = 100;
		break;
		
	case 100:
		if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][n_MotNum] = d_MotPos;
		nRet = COMI.Start_SingleMove(n_MotNum, d_MotPos, n_SpeedRate);  // 해당 위치로 이동
		if (nRet == BD_GOOD)
		{
			mn_single_motmove_step[n_MotNum] = 200;
		}
		else if (nRet == BD_ERROR || nRet == BD_SAFETY)
		{
			mn_mot_retry_cnt[n_MotNum]++;
			
			if(mn_mot_retry_cnt[n_MotNum] > mn_mot_max_retry_cnt) 
			{
				mn_single_motmove_step[n_MotNum] = 0;
				nFuncRet = nRet;
			}
			else 
			{				
				mn_single_motmove_step[n_MotNum] = 100;
			}
		}
		else if (nRet == BD_RETRY)  // 동작 재시도
		{
			if(mn_mot_retry_cnt[n_MotNum] > mn_mot_max_retry_cnt) 
			{
				mn_single_motmove_step[n_MotNum] = 0;
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				mn_single_motmove_step[n_MotNum] = 100;
			}
		}
		break;
		
	case 200:
		nRet = COMI.Check_SingleMove(n_MotNum, d_MotPos);  // 이동 완료 확인
		if (nRet == BD_GOOD)  // 정상 완료
		{
			mn_single_motmove_step[n_MotNum] = 0;
			nFuncRet = nRet;
		}
		else if (nRet == BD_ERROR || nRet == BD_SAFETY)
		{
			mn_mot_retry_cnt[n_MotNum]++;
			
			if(mn_mot_retry_cnt[n_MotNum] > mn_mot_max_retry_cnt) 
			{
				mn_single_motmove_step[n_MotNum] = 0;
				nFuncRet = nRet;
			}
			else 
			{				
				mn_single_motmove_step[n_MotNum] = 100;
			}
		}
		else if (nRet == BD_RETRY)  // 동작 재시도
		{
			// 무한루프에 빠질수도 있지만 횟수를 지정해서 사용하면 문제 없다
			mn_mot_retry_cnt[n_MotNum]++;
			
			if(mn_mot_retry_cnt[n_MotNum] > mn_mot_max_retry_cnt) 
			{
				mn_single_motmove_step[n_MotNum] = 0;
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				mn_single_motmove_step[n_MotNum] = 100;
			}
		}
		break;
	}
	return nFuncRet;
}

int CCtlBd_Library::Linear_Move(int n_LinearMapIndex, long l_AxisCnt, long *lp_AxisNum, double *dp_PosList, double *dp_SpdRatio)
{
	int nFuncRet = BD_PROCEED;
	int nRet[4] = {0,}, nCnt =0, i=0;

	
	switch(mn_linear_motmove_step[n_LinearMapIndex])
	{
	case 0:

		nCnt = 0;
		st_linearmot[n_LinearMapIndex].l_AxisCnt = l_AxisCnt;
		for(i=0; i<l_AxisCnt; i++)
		{		
			st_linearmot[n_LinearMapIndex].lp_AxisNum[i] = lp_AxisNum[i]; 
			st_linearmot[n_LinearMapIndex].dp_PosList[i] = dp_PosList[i];

			nRet[i] = COMI.Check_MotPosRange(lp_AxisNum[i], dp_PosList[i], COMI.md_allow_value[lp_AxisNum[i]]);
			if(nRet[i] == BD_GOOD) nCnt++;
		}

		if(nCnt == l_AxisCnt)
		{//이미 모든 모터의 현재 위치가 이동하려는 위치와 허용오차 안에 있으면 동작할 필요가 없으니 끝내자.
			nFuncRet = BD_GOOD;
			break;
		}
		
		mn_linear_retry_cnt[n_LinearMapIndex] = 0;
		mn_linear_max_retry_cnt = 3; //최대 3회 리트라이 동작을 수행한다 
		mn_linear_motmove_step[n_LinearMapIndex] = 100;
		break;
		
	case 100:
		if(COMI.mn_run_status == dRUN)
		{
			st_work.md_reinstatement_Pos[0][lp_AxisNum[0]] = dp_PosList[0];
			st_work.md_reinstatement_Pos[0][lp_AxisNum[1]] = dp_PosList[1];
		}
		nRet[0] = COMI.Start_LinearMove(n_LinearMapIndex, st_linearmot[n_LinearMapIndex].dp_PosList, dp_SpdRatio);
		if (nRet[0] == BD_GOOD)
		{
			mn_linear_motmove_step[n_LinearMapIndex] = 200;
		}
		else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
		{
			mn_linear_retry_cnt[n_LinearMapIndex]++;
			
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				nFuncRet = nRet[0];
			}
			else 
			{				
				mn_linear_motmove_step[n_LinearMapIndex] = 100;
			}
		}
		else if (nRet[0] == BD_RETRY)  // 동작 재시도
		{
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				//900000 1 A "Retry 회수 에러초과(3회) 에러."
				alarm.mstr_code = _T("900000");
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				mn_linear_motmove_step[n_LinearMapIndex] = 100;
			}
		}
		break;
		
	case 200:
		nRet[0] = COMI.Check_LinearMove(n_LinearMapIndex, st_linearmot[n_LinearMapIndex].dp_PosList) ;
		if (nRet[0] == BD_GOOD)  // 정상 완료
		{
			mn_linear_motmove_step[n_LinearMapIndex] = 0;
			nFuncRet = nRet[0];
		}
		else if (nRet[0] == BD_ERROR || nRet[0] == BD_SAFETY)
		{
			mn_linear_retry_cnt[n_LinearMapIndex]++;
			
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				nFuncRet = nRet[0];
			}
			else 
			{				
				mn_linear_motmove_step[n_LinearMapIndex] = 100;
			}
		}
		else if (nRet[0] == BD_RETRY)  // 동작 재시도
		{
			// 무한루프에 빠질수도 있지만 횟수를 지정해서 사용하면 문제 없다
			mn_linear_retry_cnt[n_LinearMapIndex]++;
			
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				//900000 1 A "Retry 회수 에러초과(3회) 에러."
				alarm.mstr_code = _T("900000");
				nFuncRet = BD_ERROR;
			}
			else 
			{				
				mn_linear_motmove_step[n_LinearMapIndex] = 100;
			}
		}	
		break;
	}
	return nFuncRet;
}
