// CtlBd_Library.cpp: implementation of the CCtlBd_Library class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtlBd_Library.h"


#include "io.h"			//2011.0514  ���� ���� ���� �˻� �Լ� ȣ���ϱ� ���ؼ��� �ݵ�� �ʿ�
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
//�˶��߻� ó�����, ���� �� I/O ���� ���̺귯���� �ʱ�ȭ �� �����Ͽ� ��������ϴ� �Լ����� 
//
//���� �⺻�Լ� 
//1. #include "ComizoaPublic.h"
//2. #include "FastechPublic_IO.h"
///////////////////////////////////////////////////////////////////////////////

int CCtlBd_Library::Alarm_Error_Occurrence(int n_jampos, int n_run_status, char c_alarmcode[10])
{
	//�װ��� �˶��� �߻��ϸ� �� �����忡�� jamcode �� run status ������ �ٲ�� ��� �����ϰ� 
	//������ �ذ������� �����δ� �� �Լ��� �������� ����Ͽ� �˶����� �� ���� ���¸� ǥ���Լ� �ϳ��� 
	//����Ͽ� ��� ���� �� �˶� ������ ó���Ѵ� 
	//
	//�� �����忡 �����ִ� �˶� ó������� �� �Լ��� ��� ����Ѵ� 
   //
	//2013,0123
	if(COMI.mn_run_status == dINIT) alarm.stl_alarm_start_time = GetCurrentTime();
	CTL_Lib.mn_jampos = n_jampos; //jam�� �߻��� ��ġ�� �����Ѵ� 
	CTL_Lib.mn_run_status = n_run_status; //����� �������¸� �����Ѵ� 
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
	
	if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
	CTL_Lib.mn_jampos = n_jampos; //jam�� �߻��� ��ġ�� �����Ѵ� 
	CTL_Lib.mn_run_status = n_run_status; //����� �������¸� �����Ѵ� 
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
	
	if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
	{
		sprintf(st_msg.c_abnormal_msg, "[%d] [%s] [%s]", CTL_Lib.mn_jampos, alarm.mstr_code, st_alarm.mstr_cur_msg);
		st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
	}

	

	return BD_GOOD;
}


int CCtlBd_Library::Initialize_motor_board(int n_bd_type, CString s_filename)	//motor ���� �ʱ�ȭ�� ���
{
	int nRet = 0, i=0;

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//Motor Board Initialize 
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	if(n_bd_type == 0) //0:�Ľ��� ���带 ����ϴ� ����̸�, 1:Ŀ������ ���͸� ����ϴ� ��� 
	{		//���� ���庰 ����� �����Ѵ� 

		COMI.mn_totalmotboard_number = 3;  //���ͺ��尡 3�� �ִ� 
		COMI.mn_totalmotoraxis_number= 18; //������ �� ������ 18���̴�  
		COMI.mn_bd_axis_number[0] = 8;   //0�� ���ͺ���� 8��� �����̴�  
		COMI.mn_bd_axis_number[1] = 6;   //1�� ���ͺ���� 6��� �����̴� 
		COMI.mn_bd_axis_number[2] = 4;   //2�� ���ͺ���� 4��� �����̴� 
 	}
	else if(n_bd_type == 1) //1:Ŀ������ ���带 ����ϴ� ����̸�, 1:Ŀ������ ���͸� ����ϴ� ��� 
	{
		//1. ���� ���� �о���δ� 
		//2. ���� ���� ������ �����Ѵ� 
		nRet = COMI.Initialize_MotBoard(s_filename);			// MOTOR ���� �ʱ�ȭ ���� �˻��Ѵ�
		if (nRet == BD_ERROR)
		{
			sprintf(mc_normal_msg,"[MOTOR BOARD] �ʱ�ȭ ����.");
			LogFile_Write("c:\\AMT_LOG\\Motor\\", mc_normal_msg);
			return BD_ERROR;
		}		

		COMI.mn_totalmotboard_number = 3;  //���ͺ��尡 3�� �ִ� 
		COMI.mn_totalmotoraxis_number= 18; //������ �� ������ 18���̴�  
		COMI.mn_bd_axis_number[0] = 8;   //0�� ���ͺ���� 8��� �����̴�  
		COMI.mn_bd_axis_number[1] = 6;   //1�� ���ͺ���� 6��� �����̴� 
		COMI.mn_bd_axis_number[2] = 4;   //2�� ���ͺ���� 4��� �����̴� 

// 		if(COMI.mn_motorbd_init_end == BD_YES)
// 		{			
// 			for(i=0; i<COMI.mn_totalmotoraxis_number; i++)
// 			{//n_simul_mode => 0:�ùķ��̼� ��� ��Ȱ��, 1:�ùķ��̼� ��� Ȱ��
// 
// 				if(COMI.mn_simulation_mode == 1) //���͸� �������� �����Ͽ� ���� ���� ����� ������ �ⱸ���� �̵��Ѱ�ó�� �����Ѵ� 
// 				{//Ȱ��
// 					COMI.Set_Simulation_Mode(i, 1); 
// 				}
// 				else
// 				{//��Ȱ�� 
// 					COMI.Set_Simulation_Mode(i, 0); 
// 				}
// 			}		
//		}
	}

	return  BD_GOOD;
}

//�Ʒ� ������ I/O ���� �ʱ�ȭ�� �ʿ��� ������ ���� �������̴� 
//��� �ʿ��� ������ �����Ͽ� ����ϸ� �ȴ�.
int CCtlBd_Library::Initialize_io_Board(int n_bd_type)
{
	//if(n_bd_type == 0) //�Ľ��� HSSI I/O board 
	//{
	//}
	int i=0,j=0;
	INT HSSI_speed = 0;
	int Ret = 0;
	INT pMasterNo;
	

	/////////////////�ʱ⿡ �ʱ�ȭ�� //////////////////////////////////////////////////////////////////////
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_2] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_1] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety1_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety2_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety3_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety0_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety4_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety5_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety6_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety10_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
// 								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety7_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
								CtlBdFunc.ms_ErrMsg.Format("[LSafety8_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
							CtlBdFunc.ms_ErrMsg.Format("[LSafety9_3] Linear �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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

int CCtlBd_Library::Motor_SafetyCheck(int n_mode, int n_axis, double d_targetpos) 	//���� ���� ���� ���ý� ��� 
{
	//���� �����Ͽ��� �����Ͽ� ��� �����Ѵ� 

	// ��) 
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
//2011.0201 �ּ�ó�� 	int n_SetTime = 50; //091127 20->50���� ���� 100; //500ms ���� ��� ����Ǹ� �׶� ��μ� I/O ���� return 

	// **************************************************************************
	// ���� �˶� ���� �˻��Ѵ�                                                   
	// -> �˶� �߻��� ��� �˶� �����Ѵ�                                         
	// **************************************************************************
	//software limit �� ���� üũ ���� 
	if(d_targetpos < COMI.md_limit_position[n_axis][0] && st_handler.mn_virtual_mode == 0 ) //
	{
		sprintf(cJamcode, "%02d0004", n_axis);
		Alarm_Error_Occurrence(0, CTL_dWARNING, cJamcode);
		
		if (st_handler.cwnd_list != NULL)
		{  // ����Ʈ �� ȭ�� ����
			sprintf(st_msg.c_abnormal_msg, "Motor[%d] Software Limit Error_1[%4.3f] [%4.3f]", n_axis, COMI.md_limit_position[n_axis][0], d_targetpos);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return BD_ERROR;
	}	
	if(d_targetpos > COMI.md_limit_position[n_axis][1] && st_handler.mn_virtual_mode == 0 ) //
	{
		sprintf(cJamcode, "%02d0004", n_axis);
		Alarm_Error_Occurrence(1, CTL_dWARNING, cJamcode);
		
		if (st_handler.cwnd_list != NULL)
		{  // ����Ʈ �� ȭ�� ����
			sprintf(st_msg.c_abnormal_msg, "Motor[%d] Software Limit Error_2[%4.3f] [%4.3f]", n_axis, COMI.md_limit_position[n_axis][1], d_targetpos);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // ���� ���� ��� ��û
		}
		return BD_ERROR;
	}

	// **************************************************************************
	// ���� �Ŀ� ���� �˻��Ѵ�                                                   
	// -> ���� POWER OFF �� POWER ON ���·� �����                               
	// **************************************************************************
	if (g_comiMgr.Get_MotPower(n_axis) == CTL_ERROR )          // ���� POWER ���� �˻� �Լ� 
	{
		if (COMI.Set_MotPower(n_axis, CTL_ON) == CTLBD_RET_GOOD)       // ���� POWER ON ���� �Լ� 
		{
			if (st_handler.cwnd_list != NULL)
			{  // ����Ʈ �� ȭ�� ����
				sprintf(st_msg.c_normal_msg, "[MotorPowerOn_1] Axis=%d, rcnt=%d", n_axis, st_motor[n_axis].n_retry_cnt);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // ���� ���� ��� ��û
			}

			if (COMI.Get_MotPower(n_axis) == CTL_ERROR)  // ���� POWER ���� �˻� �Լ� 
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

	//���� �˶� �߻��� ó�� ���� 
	if (COMI.Get_MotAlarmStatus(n_axis) == CTL_ERROR)      // ���� ALARM ���� �˻� �Լ� 
	{
		if (COMI.Set_MotAlarmClear(n_axis) == CTL_GOOD)       // ���� ALARM CLEAR �Լ� 
		{
			//091119 james			Sleep(1000);  // ���� �ð� �Ŀ� ���� Ȯ���ϱ� ���� SLEEP ����Ѵ� 
			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
			{
				sprintf(st_msg.c_normal_msg, "[Alarm Reset] Axis=%d, rcnt=%d", n_axis, COMI.mn_retry_cnt[n_axis]);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
			}

			if (COMI.Get_MotAlarmStatus(n_axis) == CTL_ERROR )  // ���� ALARM ���� �˻� �Լ� 
			{
				if (COMI.mn_retry_cnt[n_axis] > MOT_RTY_CNT)
				{			
					sprintf(cJamcode, "%02d0002",  n_axis); alarm.mstr_code = _T(cJamcode); //091216 	alarm.mn_count_mode = MOT_DEBUG;	
					sprintf(alarm.mc_code, "%02d0002", n_axis);
					CtlBdFunc.ms_ErrMsg.Format("[Safety] Mot[%d] rcnt[%d] Get Alarm Check Error", n_axis, COMI.mn_retry_cnt[n_axis]);				
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, cJamcode, CtlBdFunc.ms_ErrMsg);
					//2011.0210 COMI.mn_retry_cnt[n_axis] = 0;  // �˶� ���� �õ� Ƚ�� 
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
				
				//2011.0210 COMI.mn_retry_cnt[n_axis] = 0;  // �˶� ���� �õ� Ƚ�� 
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
	// ������ ���� ��ġ�� üũ�Ѵ�. ��� �°� �ִ°� ����. ���ʿ� ������ ���� �ҷ��� ��� �ҵ�.                                           
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x1] Ʈ���̿��� X���� Ʈ���� �Ÿ����� ���� �������� - Z���� Safety�� �ƴ�" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety and Z Axis must be 5mm under. !!!" );//20130930
				// 				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);

				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x2] �κ� X�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety1] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety2] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety2] X axis robot move error - transfer location not rejected!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			//2014,0717
			if(COMI.mn_run_status == dRUN)
			{
				if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5��ġ
				{
					if( d_targetpos < 50/* || (g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X) < 50)*/ )
					{
						alarm.mstr_code = _T("100003");
						CtlBdFunc.ms_ErrMsg.Format("[Safety22] ��񸮼� ���ּ���(�ʱ�ȭ) !!!" );
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety22] Please reset device(Initialization) !!!");//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
						return CTL_ERROR;
					}
				}
			}
			if( d_targetpos >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] + 50 )
			{
				alarm.mstr_code = _T("100003");
				CtlBdFunc.ms_ErrMsg.Format("[Safety22] Pos : %.3f X�� �ִ� ���̻�� �����ҷ��� �մϴ�. !!!", d_targetpos );
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x3] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safet_x4] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety2] X axis robot move error - transfer location not rejected!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x5] Pos : %.3f X�� �ִ� ���̻�� �����ҷ��� �մϴ�. !!!", d_targetpos );
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety22] Please reset device(Initialization) !!!");//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x6] Pos : %.3f X�� �ּ� �����Ϸ� �����ҷ��� �մϴ�. !!!", d_targetpos );
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety23] X Axis going to minuum pos(MINUS) !!!");//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "100003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
 					CtlBdFunc.ms_ErrMsg.Format("[Safety_y1] �κ� X�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
 					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety ans Z Axis must be 5mm under !!!" );//20130930
 					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y2] �κ� X�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] X axis robot move error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
// 					CtlBdFunc.ms_ErrMsg.Format("[Safety39] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
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
// 					CtlBdFunc.ms_ErrMsg.Format("[Safety40] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety33] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety5] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety6] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );				
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
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety39] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
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
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety40] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y3] �κ� X�� �̵� ���� - Ʈ������ BIN ��ġ �ƴ� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety33] X axis robot move error - transfer position is not bined !!!" );
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x19] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x17] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety_x18] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );	
								if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety1] X axis robot move error - transfer location not supplied!" );//20130930						
								CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
								if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y4] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y4] X axis robot move error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y5] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y5] X axis robot move error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] ����1 ȸ���� �̵� ���� - Z�� ȸ�� ��ġ �ƴ� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] ����2 ȸ���� �̵� ���� - Z�� ȸ�� ��ġ �ƴ� !!!" );
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] ����3 ȸ���� �̵� ���� - Z�� ȸ�� ��ġ �ƴ� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] Ʈ������ Y�� �̵� ���� - Ʈ������ ������ �ƴ� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety] Ʈ������ Y�� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] Ʈ������ Y�� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] Transfer Y axis error - X axis robot collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453101", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_tr] Ʈ������ Y�� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Transfer Y axis error - X axis robot collision risk !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "453101", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety] Ʈ������ Ŭ���� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety] Ʈ������ Ŭ���� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_tr_clamp] Ʈ������ Ŭ���� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_tr_clamp] Transfer Clamp axis error - X axis robot collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "454100", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
// 			CtlBdFunc.ms_ErrMsg.Format("[Safety] Ʈ������ Ŭ���� �̵� ���� - �κ� X�� �浹 ���� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� Z�� �̵� ���� - �׸��� �ٿ���� !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Z axis robot error - gripper down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450200", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + 10.0f + (st_motor[MOTOR_ROBOT_Y].d_pos[27]*3)) )
		{
			if(d_targetpos > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.0f)
			{//470000 1 A "�κ� Z�� ������Ƽ üũ ����."
				alarm.mstr_code = _T("470000");
				CtlBdFunc.ms_ErrMsg.Format("[Safety15] �κ� Z�� ���� ��ġ���� �� ���������� �Ѵ�(�ִ� : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] + 10.0f);				
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
			{//470000 1 A "�κ� Z�� ������Ƽ üũ ����."
				alarm.mstr_code = _T("470000");
				CtlBdFunc.ms_ErrMsg.Format("[Safety16] Align Buffer�� �κ� Z�� ���� ��ġ���� �� ���������� �Ѵ�(�ִ� : %.3f or 30.0!!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK] + 5.0f);				
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
				{//470000 1 A "�κ� Z�� ������Ƽ üũ ����."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety17] �κ� Z�� ���� ��ġ���� �� ���������� �Ѵ�(�ִ� : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);				
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
				{//470000 1 A "�κ� Z�� ������Ƽ üũ ����."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety18] �κ� Z�� ���� ��ġ���� �� ���������� �Ѵ�(�ִ� : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_END] + 10.0f);				
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
				{//470000 1 A "�κ� Z�� ������Ƽ üũ ����."
					alarm.mstr_code = _T("470000");
					CtlBdFunc.ms_ErrMsg.Format("[Safety19] �κ� Z�� ���� ��ġ���� �� ���������� �Ѵ�(�ִ� : %.3f !!!)", st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_REJ_PLACE] + 10.0f);				
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
// 			CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� Z�� �̵� ���� - ��Ŀ �ٿ���� !!!" );				
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
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety90] �κ� X�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x7] �κ� X�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety_x8] �κ� X�� �̵� ���� - �׸��� �ٿ���� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety_x9] �κ� X�� �̵� ���� - ��Ŀ �ٿ���� !!!" );				

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
				CtlBdFunc.ms_ErrMsg.Format("[Safety30] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );	
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety7] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );	
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety8] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety8] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety8] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							// 						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							return CTL_ERROR;
						}
					}
				}
				else if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y) <  st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_ROBOT_Y])
				{
					//CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� X�� �̵� ���� - Ʈ������ ��ġ �ƴ� !!!" );				
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
					//2014,0717//������ �� cok ��ü
 					if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - (COMI.md_allow_value[MOTOR_TRAY_TR_Y] * 5)) &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + 10.f) )
					{
						if( st_handler.mn_cok_change != CTL_YES)//2014,1106
						{
							alarm.mstr_code = _T("455000");
							CtlBdFunc.ms_ErrMsg.Format("[Safety9] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety10] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety10] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety77] �κ� X�� �̵� ���� - Ʈ������ Ŭ���� ������Ƽ ��ġ �ƴ� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� X�� �̵� ���� - Ʈ������ Ŭ���� ������Ƽ ��ġ �ƴ� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety66] �κ� X�� �̵� ���� - Ʈ������ Ŭ���� ������Ƽ ��ġ �ƴ� !!!" );				
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x10] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );	
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x10] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] �κ� X�� �̵� ���� - Ʈ������ ����Ʈ ��ġ �ƴ� !!!" );	
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] robot to move the X axis error - transfer position is not rejected !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x11] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_x12] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x12] robot to move the X axis error - transfer supply position is not !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
					//CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� X�� �̵� ���� - Ʈ������ ��ġ �ƴ� !!!" );				
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "MM0X03", CtlBdFunc.ms_ErrMsg);
					//CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450003", CtlBdFunc.ms_ErrMsg);
					//return CTL_ERROR;
				}

			}

			if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] + 5.0f < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) &&
				st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) )
			{
	//�ˤФ�			CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� X�� �̵� ���� - Ʈ������ ���� ��ġ �ƴ� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety_13] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
							if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_13] XY axis robot move error - transfer collision risk !!!" );//20130930
							CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
							if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x14] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x14] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_x15] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x15] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_x16] �κ� X�� �̵� ���� - Ʈ������ Ŭ���� ������Ƽ ��ġ �ƴ� !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_x16] X axis robot move error - not a transfer clamp safety position !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450004", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y6]pos : %.3f �κ� Y�� �̵� ���� - Y�� �ִ���ġ�� ������� �� !!!",d_targetpos );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y6] Y axis robot error - Limit Pos exceed!!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y7]pos : %.3f �κ� Y�� �̵� ���� - Y�� �ּ���ġ�� ������� �� !!!",d_targetpos );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y7] Y axis robot error - Limit Pos exceed!!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
				CtlBdFunc.ms_ErrMsg.Format("[Safety_y8] �κ� Y�� �̵� ���� - Z�� Safety ��ġ �ƴ� !!!" );				
				if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y8] Y axis robot error - Z axis is not located Safety !!!" );//20130930
				CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450103", CtlBdFunc.ms_ErrMsg);
				if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] �κ� Y�� �̵� ���� - �׸��� �ٿ���� !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] Y axis robot error - Gripper down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450100", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] �κ� Y�� �̵� ���� - ��Ŀ �ٿ���� !!!" );	
			alarm.mstr_code = _T("450101");
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y9] Y axis robot error - Picker down state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450101", CtlBdFunc.ms_ErrMsg);
			if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
// 				CtlBdFunc.ms_ErrMsg.Format("[Safety] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety11] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
// 						CtlBdFunc.ms_ErrMsg.Format("[Safety11] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety11] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety12] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety12] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety12] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety13] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety13] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety13] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety14] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
								CtlBdFunc.ms_ErrMsg.Format("[Safety14] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety14] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
							CtlBdFunc.ms_ErrMsg.Format("[Safety14] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
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
						CtlBdFunc.ms_ErrMsg.Format("[Safety_y10] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
						if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y10] XY axis robot move error - transfer collision risk !!!" );//20130930
						CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
						if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y11] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y11] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y12] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y12] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
					CtlBdFunc.ms_ErrMsg.Format("[Safety_y13] �κ� XY�� �̵� ���� - Ʈ������ �浹 ���� !!!" );				
					if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety_y13] XY axis robot move error - transfer collision risk !!!" );//20130930
					CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "455000", CtlBdFunc.ms_ErrMsg);
					if (st_handler.cwnd_list != NULL)  // ����Ʈ �� ȭ�� ����
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] �׽�Ʈ����Ʈ1 Z�� �̵� ���� - �÷���Ʈ �������� !!!" );				
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
			CtlBdFunc.ms_ErrMsg.Format("[Safety] �׽�Ʈ����Ʈ2 Z�� �̵� ���� - �÷���Ʈ �������� !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Test Site 2 Z axis movement error - plate advances the state !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "452200", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}

		break;

	case MOTOR_COK_ROBOT_Y:
		if( st_handler.md_common_teaching[CT_COK_Z_SAFETY] + COMI.md_allow_value[MOTOR_COK_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_COK_ROBOT_Z ) )
		{
			alarm.mstr_code = _T("450300");
			CtlBdFunc.ms_ErrMsg.Format("[Safety] COK �κ� Y�� �̵� ���� - Z�� ������Ƽ ��ġ �ƴ�. !!!" );				
			if ( g_local.GetLocalType() == LOCAL_ENG ) CtlBdFunc.ms_ErrMsg.Format("[Safety] Y axis robot error COK - Z axis is not a safety position !!!" );//20130930
			CtlBdFunc.Send_Error_Message(MOT_ERR_CODE, n_axis, "450300", CtlBdFunc.ms_ErrMsg);
			return CTL_ERROR;
		}
		break;

	case MOTOR_COK_ROBOT_Z:
		break;
	}

	//2011.0201 COMI.mn_retry_cnt[n_axis] = 0;
	st_motor[n_axis].mn_retry_time_flag = CTL_NO;//2011.0201 �߰� 

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
		if (n_check == BD_YES)			// Size�� üũ�ϴ°��̸�...
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
		else						// Size�� Ȯ���ϴ°��̸�...
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

	CString mstr_cur_year, mstr_cur_month, mstr_cur_day, str_display_time; // ���� ��, ��, �� ���� ���������� ��ȯ�Ͽ� ������ ���� 
	int mn_cur_year, mn_cur_month, mn_cur_day, mn_cur_hour; // ���� ��, ��, �� ���� ���� ���� 
	CString mstr_file_name, mstr_dir_path, str_msg;		// ���������� ������ ���� �̸� ���� ���� 
	CString mstr_create_file;	// �˶� ���� ������ ���Ͽ� ���� [����]+[���ϸ�]+[Ȯ����] ���� ���� 
	CString mstr_list_name, mstr_temp_data;  // �� �κк� �˶� �߻� Ƚ�� ���� ������ �α� ���� ���� ���� ���� 
	CString mstr_content;		// �� ���ο� ����� ���ڿ� ���� ���� ���� 
	COleDateTime time_cur;		// �˻��� �ð� ���� ���� ���� 
	CTime m_time_current;		// ������ ����� ���� �ð� ���� ���� ����
	char chr_buf[20]={0,};
	int mn_existence, nlength;			// ���� ���� ���� ���� �÷��� 
	char cfileName[256]={0,};			// �˻��� ���� ���� ���� �Լ� 
	FILE  *fp ;					// ���Ͽ� ���� ������ ���� ���� 

	CString BackupName;
	CString mstr_cur_hour, mstr_cur_min, mstr_cur_sec;
	int Ret=0;

	nlength = strlen(cp_msg);
	if(nlength <= 0) //������ �޼����� ���� 
	{
		return BD_ERROR;
	}
	str_msg = cp_msg;
 
	// **************************************************************************
	// ���� �̸����� ����� ��¥ ������ ��´�                                   
	// **************************************************************************
	time_cur = COleDateTime::GetCurrentTime();  // ���� �ð� ������ ��´�. 

	m_time_current = CTime::GetCurrentTime() ;  // ������ ������ ���� �ð� ���� ��´�. 

	mn_cur_year = time_cur.GetYear();  

	mn_cur_month = time_cur.GetMonth();  

    mn_cur_day = time_cur.GetDay();  

	mn_cur_hour = time_cur.GetHour();
	// **************************************************************************

	// **************************************************************************
	// ��¥ ������ ���������� ��ȯ�Ͽ� ������ �����Ѵ�                           
	// **************************************************************************
	mstr_cur_year = LPCTSTR(_itoa( mn_cur_year, chr_buf, 10 ));  
	mstr_cur_month.Format("%02d", mn_cur_month);
	mstr_cur_day.Format("%02d", mn_cur_day);

	// **************************************************************************
	// ���� �ð� ���� ��´�                                                     
	// **************************************************************************
	mn_cur_hour = time_cur.GetHour();				// ���� �ð� ������ �����Ѵ�. 
	mstr_cur_hour.Format("%d",time_cur.GetHour());	// ���� �ð� ������ �����Ѵ�.
	mstr_cur_min.Format("%d",time_cur.GetMinute());	// ���� �� ������ �����Ѵ�. 
	mstr_cur_sec.Format("%d",time_cur.GetSecond());	// ���� �� ������ �����Ѵ�. 
	str_display_time = m_time_current.Format("%c");	// ����Ʈ ���Ͽ� ����� Ÿ��Ʋ �ð� ���� ���� 
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
	{//�ش� ������ �������� �ʴ� �����̴� 
		CreateFolder(cp_dir_path); 
	}
	else 
	{//�ش� ������ �̹� �����Ѵ�.
		//���� 
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

		fclose(fp) ;  //2011.0417  ���� ����

//2011.0417		sing.Unlock(); //2011.0417 �߰� 
		return BD_ERROR;
	}
	// **************************************************************************
	fclose(fp);  // ������ �����Ѵ�.

//2011.0417	sing.Unlock(); //2011.0417 �߰� 
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
		if (nRet == BD_GOOD) //�̹� �ش� ��ġ�� �� ������ �������� �ʰ� ���� �̵��� ������ 
		{
			nFuncRet = BD_GOOD;
			break;
		}
		
		mn_mot_retry_cnt[n_MotNum] = 0;
		mn_mot_max_retry_cnt = 3; //�ִ� 3ȸ ��Ʈ���� ������ �����Ѵ� 
		mn_single_motmove_step[n_MotNum] = 100;
		break;
		
	case 100:
		if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][n_MotNum] = d_MotPos;
		nRet = COMI.Start_SingleMove(n_MotNum, d_MotPos, n_SpeedRate);  // �ش� ��ġ�� �̵�
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
		else if (nRet == BD_RETRY)  // ���� ��õ�
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
		nRet = COMI.Check_SingleMove(n_MotNum, d_MotPos);  // �̵� �Ϸ� Ȯ��
		if (nRet == BD_GOOD)  // ���� �Ϸ�
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
		else if (nRet == BD_RETRY)  // ���� ��õ�
		{
			// ���ѷ����� �������� ������ Ƚ���� �����ؼ� ����ϸ� ���� ����
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
		{//�̹� ��� ������ ���� ��ġ�� �̵��Ϸ��� ��ġ�� ������ �ȿ� ������ ������ �ʿ䰡 ������ ������.
			nFuncRet = BD_GOOD;
			break;
		}
		
		mn_linear_retry_cnt[n_LinearMapIndex] = 0;
		mn_linear_max_retry_cnt = 3; //�ִ� 3ȸ ��Ʈ���� ������ �����Ѵ� 
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
		else if (nRet[0] == BD_RETRY)  // ���� ��õ�
		{
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				//900000 1 A "Retry ȸ�� �����ʰ�(3ȸ) ����."
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
		if (nRet[0] == BD_GOOD)  // ���� �Ϸ�
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
		else if (nRet[0] == BD_RETRY)  // ���� ��õ�
		{
			// ���ѷ����� �������� ������ Ƚ���� �����ؼ� ����ϸ� ���� ����
			mn_linear_retry_cnt[n_LinearMapIndex]++;
			
			if(mn_linear_retry_cnt[n_LinearMapIndex] > mn_linear_max_retry_cnt) 
			{
				mn_linear_motmove_step[n_LinearMapIndex] = 0;
				//900000 1 A "Retry ȸ�� �����ʰ�(3ȸ) ����."
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
