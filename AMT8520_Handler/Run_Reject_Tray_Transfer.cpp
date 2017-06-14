// Run_Reject_Tray_Transfer.cpp: implementation of the CRun_Reject_Tray_Transfer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Reject_Tray_Transfer.h"

#include "FastechPublic_IO.h"
#include "ComizoaPublic.h"
#include "Public_Function.h"
#include "CtlBd_Function.h"
#include "CtlBd_Library.h"

#include "Cmmsdk.h"
#include "CmmsdkDef.h"

#include "math.h"
#include "io.h" 

#include "COMI_Manager.h"
#include "IO_Manager.h"
#include "AMTLotManager.h"

#include "SrcPart/APartTray.h"
#include "SrcPart/APartRobot.h"
#include "SrcPart/APartHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CRun_Reject_Tray_Transfer, CObject, 1);	// 복구 동작을 위함...

CRun_Reject_Tray_Transfer	Run_Reject_Tray_Transfer;

CRun_Reject_Tray_Transfer::CRun_Reject_Tray_Transfer()
{
	Thread_Variable_Initial();
}	

CRun_Reject_Tray_Transfer::~CRun_Reject_Tray_Transfer()
{

}

void CRun_Reject_Tray_Transfer::Thread_Variable_Initial()
{
	mn_InitStep = 0;
	mn_RunStep = 0;
	mn_reinstate_step = 0;

	m_step_to_rej = ETT_REJ_NONE;
	m_step_to_bin = ETT_BIN_NONE;
	m_step_transfer_to_rej = ETT_REJ_NONE;//ybs

	m_bInitVarBackward = false;

	m_step_cylz = ESTEP_CYL_START;
	m_fbCylZ = FB_NONE;

	m_step_rail = ESTEP_CYL_START;
	m_fbRail = FB_NONE;

	m_step_to_bin_old = ETT_BIN_NONE;
	m_step_to_rej_old = ETT_REJ_NONE;

	m_step_transfer_to_rej_old = ETT_REJ_NONE;//ybs
	m_step_transfer_to_bin_old = ETT_BIN_NONE;

	m_step_transfer_to_safety_old = ETT_SAFETY_NONE;
	m_step_transfer_to_safety = ETT_SAFETY_NONE;
	m_step_transfer_to_bin = ETT_BIN_NONE;
	m_step_transfer_to_rej = ETT_REJ_NONE;


	st_work.mn_transfer_cyl = IO_OFF;

	mn_clamp_retry_cnt = 0;
}

void CRun_Reject_Tray_Transfer::Thread_Run()
{
	switch (COMI.mn_run_status)
	{
	case dINIT:
		Run_Initial();
		break;
		
	case dRUN:
		Run_Move();
		break;
		
	case dREINSTATE:
		Run_Reinstate();
		break;

	default:
		OnRobot_FinalPos();
		CTL_Lib.mn_single_motmove_step[MOTOR_TRAY_TR_Y] = 0;					// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		CTL_Lib.mn_single_motmove_step[MOTOR_TRAY_TR_CLAMP] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.

		m_step_to_bin_old = ETT_BIN_NONE;
		m_step_to_rej_old = ETT_REJ_NONE;
		m_step_transfer_to_rej_old = ETT_REJ_NONE;//ybs
		m_step_transfer_to_bin_old = ETT_BIN_NONE;
		m_step_transfer_to_safety_old = ETT_SAFETY_NONE;
		break;
	}

	Run_Move_Cyl_Z();
	Run_Move_Rail();

}

void CRun_Reject_Tray_Transfer::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[INIT_REJECT_TRAY_TRANSFER] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case ETI_START:						mn_InitStep = ETI_ROBOT_CHK;		break;
	case ETI_ROBOT_CHK:					ETI_Robot_Chk();					break;
	case ETI_Z_UP:						ETI_Z_Up();							break;
	case ETI_Z_UP_CHK:					ETI_Z_Up_Chk();						break;
	case ETI_BIN_STACKER_BACKWARD:		ETI_Bin_Stacker_Backward();			break;
	case ETI_BIN_STACKER_BACKWARD_CHK:	ETI_Bin_Stacker_Backward_Chk();		break;
	case ETI_HOMECHK_CLAMP:				ETI_HomeChk_Clamp();				break;
	case ETI_HOMECHK_Y:					ETI_HomeChk_Y();					break;
	case ETI_HOMECHK_CLAMP1:			ETI_HomeChk_Clamp1();				break;
	case ETI_CLAMP_SAFETY:				
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				mn_InitStep = ETI_BIN_STACKER_RAIL;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 510, CTL_dWARNING, strJamCode );
			}
		}
		break;
	case ETI_BIN_STACKER_RAIL:			ETI_Bin_Stacker_Rail();				break;
	case ETI_BIN_STACKER_RAIL_CHK:		ETI_Bin_Stacker_Rail_Chk();			break;
	case ETI_FINISH:					ETI_Finish();						break;
	}
}

void CRun_Reject_Tray_Transfer::Run_Move()
{
	Run_Move_To_Rej();
	Run_Move_To_Bin();
	Run_Move_Transfer_To_Rej();
	Run_Move_Transfer_To_Bin();
	Run_Move_Transfer_To_Safety();
}

void CRun_Reject_Tray_Transfer::Serialize(CArchive &ar)
{
	int i = 0, j = 0;
	
	CObject::Serialize(ar);
	
	if(ar.IsStoring())				// 저장하기
	{
		ar << mn_RunStep;
	}
	else							// 불러오기
	{	
		ar >> mn_RunStep;
		
		mn_retry_cnt = 0;
	}
}

void CRun_Reject_Tray_Transfer::ETI_Robot_Chk()
{
	if( Run_Robot.m_bInitXHome == false )
		return;

	mn_InitStep = ETI_Z_UP;
}

void CRun_Reject_Tray_Transfer::ETI_HomeChk_Clamp()
{
	//2013,0307
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(st_handler.mn_robot_motorX_safety != CTL_YES) return;
	}

	int nRet = COMI.HomeCheck_Mot( MOTOR_TRAY_TR_CLAMP, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ETI_HOMECHK_Y;
		Func.OnInitBtn( RBTN_TT_CLAMP );
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMTC00");
		CTL_Lib.Alarm_Error_Occurrence(511, CTL_dWARNING, "445100");
	}
}

void CRun_Reject_Tray_Transfer::ETI_HomeChk_Clamp1()
{
	//2013,0307
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(st_handler.mn_robot_motorX_safety != CTL_YES) return;
	}
	
	int nRet = COMI.HomeCheck_Mot( MOTOR_TRAY_TR_CLAMP, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ETI_CLAMP_SAFETY;
;
// 		Func.OnInitBtn( RBTN_TT_CLAMP );
	}
	else if( nRet == BD_ERROR )
	{
		// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMTC00");
		CTL_Lib.Alarm_Error_Occurrence(511, CTL_dWARNING, "445100");
	}
}

void CRun_Reject_Tray_Transfer::ETI_Z_Up()
{
	ReqCylZ( BACKWARD );
	mn_InitStep = ETI_Z_UP_CHK;
}

void CRun_Reject_Tray_Transfer::ETI_Z_Up_Chk()
{
	if( GetCylZ() != FB_NONE )
		return;

	mn_InitStep = ETI_BIN_STACKER_BACKWARD;
}

void CRun_Reject_Tray_Transfer::ETI_HomeChk_Y()
{
	//2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(st_handler.mn_robot_motorX_safety != CTL_YES) return;
	}
	int nRet = COMI.HomeCheck_Mot( MOTOR_TRAY_TR_Y, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ETI_HOMECHK_CLAMP1;
		Func.OnInitBtn( RBTN_TT_Y );
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMTY00");
		CTL_Lib.Alarm_Error_Occurrence(512, CTL_dWARNING, "445200");
	}
}

void CRun_Reject_Tray_Transfer::ETI_Bin_Stacker_Rail()
{
	if( st_handler.mn_init_state[INIT_BIN_STACKER_1] == CTL_NO )		return;
	if( st_handler.mn_init_state[INIT_BIN_STACKER_2] == CTL_NO )		return;
	if( st_handler.mn_init_state[INIT_BIN_STACKER_3] == CTL_NO )		return;//ybs
	if( st_handler.mn_init_state[INIT_BIN_STACKER_REJECT] == CTL_NO )	return;

	// 0, 180 = FORWARD,
	if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180 )
	{
		ReqRail( FORWARD );
	}
	else
	{
		ReqRail( BACKWARD );
	}
	
	mn_InitStep = ETI_BIN_STACKER_RAIL_CHK;
}

void CRun_Reject_Tray_Transfer::ETI_Bin_Stacker_Rail_Chk()
{
	if( GetRail() != FB_NONE )
		return;
	
	mn_InitStep = ETI_FINISH;
	Func.OnInitBtn( RBTN_STACKER_RAIL );
}

void CRun_Reject_Tray_Transfer::ETI_Finish()
{
	//2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(st_handler.mn_robot_motorX_safety != CTL_YES) return;
	}
	int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY], st_basic.nRunSpeed );
	if( nRet == BD_GOOD )
	{
		st_handler.mn_robot_motorRej_safety = CTL_YES;
		st_handler.mn_init_state[INIT_REJECT_TRAY_TRANSFER] = CTL_YES;
		mn_InitStep = ETI_START;
		
		Func.OnInitBtn( RBTN_TRAY_TRANSFER );
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CString strJamCode;
		// 				strJamCode.Format( "MMTY00" );
		strJamCode.Format( "453100" );
		CTL_Lib.Alarm_Error_Occurrence( 592, CTL_dWARNING, strJamCode );
	}

	Func.OnInitBtn( RBTN_TRAY_TRANSFER );
}

void CRun_Reject_Tray_Transfer::ReqMoveToRej()
{
	if( m_step_to_rej == ETT_REJ_NONE )
		m_step_to_rej = ETT_REJ_READY;
}

void CRun_Reject_Tray_Transfer::Run_Move_To_Rej()
{
	if( m_step_to_rej_old != m_step_to_rej )
	{
		m_step_to_rej_old =  m_step_to_rej;
		m_dwTime_ToRej = GetCurrentTime();
		
		if( Run_Move_To_Rej_ChkJam() == false )
			return;
	}

	if( Run_Move_To_Rej_ChkTime() == false )
		return;

	Func.ThreadFunctionStepTrace(10, m_step_to_rej);
	switch( m_step_to_rej )
	{
	case ETT_REJ_NONE:				break;

	case ETT_REJ_READY:
		//g_Tray.TrayMoved( TPOS_BIN3, TPOS_BIN_TO_REJECT );
		//2013,1020
		if(st_handler.mn_mirror_type == CTL_YES) g_Tray.TrayMoved( TPOS_BIN1, TPOS_BIN_TO_REJECT );
		else									 g_Tray.TrayMoved( TPOS_BIN3, TPOS_BIN_TO_REJECT );
		m_step_to_rej = ETT_REJ_ROBOT_CHK;
		break;

	case ETT_REJ_ROBOT_CHK:
		{
			double dX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			//ybs 2013,0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( dX <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_to_rej = ETT_REJ_ROBOT_MOVE;
				}
				else
				{
					m_step_to_rej = ETT_REJ_UNCLAMP;
				}
			}
			else
			{
				if( dX >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_to_rej = ETT_REJ_ROBOT_MOVE;
				}
				else
				{
					m_step_to_rej = ETT_REJ_UNCLAMP;
				}
			}
		}
		break;
	
	case ETT_REJ_ROBOT_MOVE:
		//ybs 2013,0225
//		if(st_handler.mn_mirror_type == CTL_YES) g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN3 );
//		else                                     g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN1 );
		//2014,0709
		if(st_handler.mn_mirror_type == CTL_YES) g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN3 );
		else                                     g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN1 );

		m_step_to_rej = ETT_REJ_ROBOT_MOVE_CHK;
		break;

	case ETT_REJ_ROBOT_MOVE_CHK:
		//2012,1215
// 		if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY3_END] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_to_rej = ETT_REJ_UNCLAMP;
		}
		else
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY1_END] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_to_rej = ETT_REJ_UNCLAMP;
		}
		break;

	case ETT_REJ_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_READY_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC01" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 513, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_READY_UP:
		ReqCylZ( BACKWARD );
		m_step_to_rej = ETT_REJ_READY_UP_CHK;
		break;

	case ETT_REJ_READY_UP_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_to_rej = ETT_REJ_STACKER_GUIDE_BACK;
		break;
		
	case ETT_REJ_STACKER_GUIDE_BACK:
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		m_step_to_rej = ETT_REJ_STACKER_GUIDE_BACK_CHK;
		break;
		
	case ETT_REJ_STACKER_GUIDE_BACK_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_to_rej = ETT_REJ_STACKER_CHK;
		break;

	case ETT_REJ_STACKER_CHK:
		m_step_to_rej = ETT_REJ_PICK_MOVE_Y;
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt() + 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_STACKER_MOVE;
			}
		}
		break;
		
	

	case ETT_REJ_STACKER_MOVE:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt() + 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);

			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PICK_MOVE_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
//2012,1128
// 				CString strJamCode;
// 				strJamCode.Format( "MMRS00" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 514, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ETT_REJ_PICK_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PICK_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY00" );
				strJamCode.Format( "453100" );
				CTL_Lib.Alarm_Error_Occurrence( 515, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_PICK_DOWN:
		ReqCylZ( FORWARD );
		m_step_to_rej = ETT_REJ_PICK_DOWN_CHK;
		break;

	case ETT_REJ_PICK_DOWN_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_to_rej = ETT_REJ_PICK_CLAMP;
		break;

	case ETT_REJ_PICK_CLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_CLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PICK_CLAMP_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 516, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_PICK_CLAMP_CHK:
		if( g_ioMgr.get_in_bit( st_io.i_reject_transfer_tray_chk ) == IO_ON )		return;
		m_step_to_rej = ETT_REJ_PICK_UP;
		break;

	case ETT_REJ_PICK_UP:
		ReqCylZ( BACKWARD );
		m_step_to_rej = ETT_REJ_PICK_UP_CHK;
		break;

	case ETT_REJ_PICK_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_rej = ETT_REJ_PLACE_MOVE_Y;
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PLACE_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY01" );
				strJamCode.Format( "453101" );
				CTL_Lib.Alarm_Error_Occurrence( 517, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_PLACE_DOWN:
		ReqCylZ( FORWARD );
		m_step_to_rej = ETT_REJ_PLACE_DOWN_CHK;
		break;

	case ETT_REJ_PLACE_DOWN_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_rej = ETT_REJ_PLACE_UNCLAMP;
		break;

	case ETT_REJ_PLACE_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PLACE_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC01" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 518, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_PLACE_UP:
		ReqCylZ( BACKWARD );
		m_step_to_rej = ETT_REJ_PLACE_UP_CHK;
		break;

	case ETT_REJ_PLACE_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_rej = ETT_REJ_PLACE_Y_BIN;
		break;

	case ETT_REJ_PLACE_Y_BIN:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_PLACE_CLAMP_SAFETY;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY00" );
				strJamCode.Format( "453100" );
				CTL_Lib.Alarm_Error_Occurrence( 519, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_REJ_PLACE_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_MOVE_WORK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC02" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 520, CTL_dWARNING, strJamCode );
			}
		}
		break;


	case ETT_REJ_MOVE_WORK:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			if( dPos_work < 1.0f )	dPos_work = dPos_1;

			double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_rej = ETT_REJ_STACKER_GUIDE_FORWARD;
				break;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
//2012,1128
// 				CString strJamCode;
// 				strJamCode.Format( "MMRS00" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 521, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ETT_REJ_STACKER_GUIDE_FORWARD:
		Run_Reject_Stacker.ReqGuide( FORWARD );
		m_step_to_rej = ETT_REJ_STACKER_GUIDE_FORWARD_CHK;
		break;
		
	case ETT_REJ_STACKER_GUIDE_FORWARD_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_to_rej = ETT_REJ_FINISH;
		break;

	case ETT_REJ_FINISH:
		g_Tray.TrayMoved( TPOS_BIN_TO_REJECT, TPOS_REJECT );
		m_step_to_rej = ETT_REJ_NONE;
		break;
	}
}

void CRun_Reject_Tray_Transfer::Run_Move_Cyl_Z()
{
	if( m_fbCylZ == FB_NONE )	return;
	
	int iOnOff = IO_ON;
	if( m_fbCylZ == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_cylz )
	{
	case ESTEP_CYL_START:	m_step_cylz = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			g_ioMgr.set_out_bit( st_io.o_reject_transfer_updn, iOnOff );
			//2013,0203
			if(COMI.mn_run_status == dRUN) st_work.mn_transfer_cyl = iOnOff;
			m_step_cylz = ESTEP_CYL_CHK;
			m_dwTimeZStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_reject_transfer_dn_chk ) == iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_reject_transfer_up_chk ) != iOnOff )
			{
				m_step_cylz = ESTEP_CYL_FINISH;
				m_dwTimeZ = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeZStart > st_time.n_limit_time[E_WAIT_TRANSFER_DN] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamcode_Rej_Transfer_Down_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 522, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeZStart < 0)
			{
				m_dwTimeZStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimeZ < 0)
			{
				m_dwTimeZ = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeZ < st_time.n_wait_time[E_WAIT_TRANSFER_DN][iOnOff] )
				return;

			m_fbCylZ= FB_NONE;
			m_step_cylz = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Reject_Tray_Transfer::ReqCylZ( EFB efb )
{
	m_fbCylZ = efb;
	m_step_cylz = ESTEP_CYL_START;
}

void CRun_Reject_Tray_Transfer::Run_Move_To_Bin()
{
	if( m_step_to_bin_old != m_step_to_bin )
	{
		m_step_to_bin_old =  m_step_to_bin;
		m_dwTime_ToBin = GetCurrentTime();
			
		if( Run_Move_To_Bin_ChkJam() == false )
			return;
	}

	if( Run_Move_To_Bin_ChkTime() == false )
		return;

	Func.ThreadFunctionStepTrace(11, m_step_to_bin);
	switch( m_step_to_bin )
	{
	case ETT_BIN_NONE:				break;

	case ETT_BIN_READY:
		g_Tray.TrayMoved( TPOS_REJECT, TPOS_REJECT_TO_BIN );
		m_step_to_bin = ETT_BIN_ROBOT_CHK;
		break;

	case ETT_BIN_ROBOT_CHK:
		{
			double dX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			//2013,0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( dX < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + 10.0f + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_to_bin = ETT_BIN_ROBOT_MOVE;
				}
				else
				{
					m_step_to_bin = ETT_BIN_UNCLAMP;
				}
			}
			else
			{
				if( dX >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - 10.0f - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_to_bin = ETT_BIN_ROBOT_MOVE;
				}
				else
				{
					m_step_to_bin = ETT_BIN_UNCLAMP;
				}
			}
		}
		break;
	
	case ETT_BIN_ROBOT_MOVE:
		//2013,0225
//		if(st_handler.mn_mirror_type == CTL_YES)
//		{
//			g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN3 );
//		}
//		else
//		{
//			g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN1 );
//		}
		//2014,0709
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN3 );
		}
		else
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN1 );
		}
		m_step_to_bin = ETT_BIN_ROBOT_MOVE_CHK;
		break;

	case ETT_BIN_ROBOT_MOVE_CHK:
		//2012,1215
//		if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_to_bin = ETT_BIN_UNCLAMP;
		}
		else
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_to_bin = ETT_BIN_UNCLAMP;
		}
		break;

	case ETT_BIN_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_READY_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC01" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 523, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_READY_UP:
		ReqCylZ( BACKWARD );
		m_step_to_bin = ETT_BIN_READY_UP_CHK;
		break;

	case ETT_BIN_READY_UP_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_to_bin = ETT_BIN_STACKER_GUIDE_BACK;
		break;

	case ETT_BIN_STACKER_GUIDE_BACK:
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		//2013,1028
		if(CHINA_VER == 1)
		{
			Run_Reject_Stacker.ReqSeperator( FORWARD );
		}
		m_step_to_bin = ETT_BIN_STACKER_GUIDE_BACK_CHK;
		break;
		
	case ETT_BIN_STACKER_GUIDE_BACK_CHK:
		//2013,1028
		if(CHINA_VER == 1)
		{
			if( Run_Reject_Stacker.GetSeperator() != FB_NONE )
				return;
		}
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_to_bin = ETT_BIN_STACKER_CHK;
		break;

	case ETT_BIN_STACKER_CHK:
		m_step_to_bin = ETT_BIN_PICK_MOVE_Y;
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_STACKER_MOVE;
			}
		}
		break;
		
	case ETT_BIN_STACKER_MOVE:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);

			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PICK_MOVE_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
//2012,1128
// 				CString strJamCode;
// 				strJamCode.Format( "MMRS00" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 524, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ETT_BIN_PICK_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PICK_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY01" );
				strJamCode.Format( "453101" );
				CTL_Lib.Alarm_Error_Occurrence( 526, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_PICK_DOWN:
		ReqCylZ( FORWARD );
		m_step_to_bin = ETT_BIN_PICK_DOWN_CHK;
		break;

	case ETT_BIN_PICK_DOWN_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_to_bin = ETT_BIN_PICK_CLAMP;
		break;

		//2013,1221
	case ETT_BIN_CLAMP_HOME:
		{
			int nRet = COMI.HomeCheck_Mot( MOTOR_TRAY_TR_CLAMP, MOT_TIMEOUT);
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PICK_CLAMP;
			}
			else if( nRet == BD_ERROR )
			{
				CTL_Lib.Alarm_Error_Occurrence(5260, CTL_dWARNING, "445100");
			}
		}
		break;


	case ETT_BIN_PICK_CLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_CLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PICK_CLAMP_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 526, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_PICK_CLAMP_CHK:
//		if( g_ioMgr.get_in_bit( st_io.i_reject_transfer_tray_chk ) == IO_ON )		return;
		mn_clamp_retry_cnt = 0;
		m_step_to_bin = ETT_BIN_PICK_UP;
		break;

	case ETT_BIN_PICK_UP:
		ReqCylZ( BACKWARD );
		m_step_to_bin = ETT_BIN_PICK_UP_CHK;
		break;

	case ETT_BIN_PICK_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_bin = ETT_BIN_PLACE_MOVE_Y;
		break;

	case ETT_BIN_PLACE_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PLACE_GUIDE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY00" );
				strJamCode.Format( "453100" );
				CTL_Lib.Alarm_Error_Occurrence( 527, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_PLACE_GUIDE:
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			Run_Bin_Stacker[0].ReqGuide( FORWARD );
		}
		else
		{
			Run_Bin_Stacker[2].ReqGuide( FORWARD );
		}
		m_step_to_bin = ETT_BIN_PLACE_GUIDE_CHK;
		break;

	case ETT_BIN_PLACE_GUIDE_CHK:
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( Run_Bin_Stacker[0].GetGuide() != FB_NONE )
				return;
		}
		else
		{
			if( Run_Bin_Stacker[2].GetGuide() != FB_NONE )
				return;
		}

		m_step_to_bin = ETT_BIN_PLACE_DOWN;
		break;

	case ETT_BIN_PLACE_DOWN:
		//2013,1028
		if(CHINA_VER == 1)
		{
			Run_Reject_Stacker.ReqSeperator( BACKWARD );
		}
		ReqCylZ( FORWARD );
		m_step_to_bin = ETT_BIN_PLACE_DOWN_CHK;
		break;

	case ETT_BIN_PLACE_DOWN_CHK:
		//2013,1028 reopen
		if(CHINA_VER == 1)
		{
			if( Run_Reject_Stacker.GetSeperator() != FB_NONE )
				return;
		}
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_bin = ETT_BIN_PLACE_UNCLAMP;
		break;

	case ETT_BIN_PLACE_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_PLACE_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC01" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 528, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_PLACE_UP:
		ReqCylZ( BACKWARD );
		m_step_to_bin = ETT_BIN_PLACE_UP_CHK;
		break;

	case ETT_BIN_PLACE_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_to_bin = ETT_BIN_PLACE_CLAMP_SAFETY;
		break;

	case ETT_BIN_PLACE_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_REJ_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC02" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 529, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case ETT_BIN_REJ_DOWN:
		if( g_Tray.GetStopRejCnt() == 0 )
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			if( dPos_work < 1.0f )	dPos_work = dPos_1;

			double dTarget = dPos_work + (dPos_5 - dPos_1);

			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_to_bin = ETT_BIN_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 530, CTL_dWARNING, strJamCode );
			}
		}
		else
		{
			m_step_to_bin = ETT_BIN_FINISH;
		}
		break;

	case ETT_BIN_FINISH:
		if(st_handler.mn_mirror_type == CTL_YES)
		{//0822
			g_Tray.TrayMoved( TPOS_REJECT_TO_BIN, TPOS_BIN1 );
			int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
			if(iBin1 > -1)
			{
				g_Tray.GetTray(iBin1).SetProcessID(st_handler.m_strProcessID);
				g_Tray.GetTray(iBin1).SetPropertyEnd(st_handler.m_strRPropEnd);
				//2017.0602
				if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES )
				{
					g_Tray.GetTray(iBin1).SetBufferNo(100 + g_Tray.GetRejTrayNo() );
					g_Tray.GetTray(iBin1).SetProductCnt(g_Tray.GetTray(iBin1).GetModuleCnt());
					g_Tray.AddRejTrayNo();
				}
			}

		}
		else
		{
			g_Tray.TrayMoved( TPOS_REJECT_TO_BIN, TPOS_BIN3 );
			int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
			if(iBin3 > -1)
			{
				g_Tray.GetTray(iBin3).SetProcessID(st_handler.m_strProcessID);
				g_Tray.GetTray(iBin3).SetPropertyEnd(st_handler.m_strRPropEnd);
				//2017.0602
				if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES )
				{
					g_Tray.GetTray(iBin3).SetBufferNo(100 + g_Tray.GetRejTrayNo());
					g_Tray.GetTray(iBin3).SetProductCnt(g_Tray.GetTray(iBin3).GetModuleCnt());
					g_Tray.AddRejTrayNo();
				}
			}
		}
		st_handler.mb_rej_3stacker = TRUE;
		m_step_to_bin = ETT_BIN_NONE;
		break;
	}
}

void CRun_Reject_Tray_Transfer::ReqMoveToBin()
{
	if( m_step_to_bin == ETT_BIN_NONE )
		m_step_to_bin = ETT_BIN_READY;
}

void CRun_Reject_Tray_Transfer::ETI_Bin_Stacker_Backward()
{
	ReqRail( BACKWARD );
	
	mn_InitStep = ETI_BIN_STACKER_BACKWARD_CHK;
}

void CRun_Reject_Tray_Transfer::ETI_Bin_Stacker_Backward_Chk()
{
	if( GetRail() != FB_NONE )
		return;
	
	mn_InitStep = ETI_HOMECHK_CLAMP;
	m_bInitVarBackward = true;
}

void CRun_Reject_Tray_Transfer::Run_Move_Rail()
{
	if( m_fbRail == FB_NONE )	return;

	int iOnOff = IO_ON;
	if( m_fbRail == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_rail )
	{
	case ESTEP_CYL_START:	m_step_rail = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:
		{
			g_ioMgr.set_out_bit( st_io.o_bin_stacker_rail_variable_fwd, iOnOff );
			g_ioMgr.set_out_bit( st_io.o_bin_stacker_rail_variable_bwd, !iOnOff );
			m_step_rail = ESTEP_CYL_CHK;
			m_dwTimeRailStart = GetCurrentTime();
		}
		break;

	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_fwd_chk ) == iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_bwd_chk ) != iOnOff )
			{
				m_step_rail = ESTEP_CYL_FINISH;
				m_dwTimeRail = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeRailStart > st_time.n_limit_time[E_WAIT_RAIL] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Rail_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 531, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeRailStart < 0)
			{
				m_dwTimeRailStart = GetCurrentTime();
			}
		}
		break;

	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimeRail < 0)
			{
				m_dwTimeRail = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeRail < st_time.n_wait_time[E_WAIT_RAIL][iOnOff] )
				break;

			m_fbRail = FB_NONE;
			m_step_rail = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Reject_Tray_Transfer::ReqRail( EFB efb )
{
	m_fbRail = efb;
	m_step_rail = ESTEP_CYL_START;
}

bool CRun_Reject_Tray_Transfer::Run_Move_To_Rej_ChkJam()
{
	switch( m_step_to_rej )
	{
	case ETT_REJ_NONE:
	case ETT_REJ_READY:
	case ETT_REJ_ROBOT_CHK:
	case ETT_REJ_ROBOT_MOVE:
	case ETT_REJ_ROBOT_MOVE_CHK:
		break;

	case ETT_REJ_UNCLAMP:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		//if( Chk_TransferTray( IO_ON ) == false )					return false;
		break;

	case ETT_REJ_READY_UP:
	case ETT_REJ_READY_UP_CHK:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		//if( Chk_TransferTray( IO_ON ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		break;

	case ETT_REJ_STACKER_MOVE:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		//if( Chk_TransferTray( IO_ON ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PICK_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		//if( Chk_TransferTray( IO_ON ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PICK_DOWN:
	case ETT_REJ_PICK_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PICK_CLAMP:
	case ETT_REJ_PICK_CLAMP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;
	case ETT_REJ_PICK_UP:
	case ETT_REJ_PICK_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_DOWN:
	case ETT_REJ_PLACE_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_UNCLAMP:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_UP:
	case ETT_REJ_PLACE_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_Y_BIN:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_MOVE_WORK:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;

	case ETT_REJ_FINISH:
		break;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Chk_Transfer_Y( int nPos )
{
//	if( nPos == P_TRANSFER_Y_BIN )
//	{
//		if( st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) )
//		{
//			CString strJamCode = GetJamCode_Transfer_Y( nPos );
//			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, strJamCode );
//			return false;
//		}
//	}
//	else if( abs( st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] - g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) ) > COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
//	{
//		CString strJamCode = GetJamCode_Transfer_Y( nPos );
//		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, strJamCode );
//		return false;
//	}

	if( nPos == P_TRANSFER_Y_BIN )
	{
		//ybs
		if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] + COMI.md_allow_value[MOTOR_TRAY_TR_Y])
		{
		}
		else if( st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] + COMI.md_allow_value[MOTOR_TRAY_TR_Y] < g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) )
		{
			CString strJamCode = GetJamCode_Transfer_Y( nPos );
			CTL_Lib.Alarm_Error_Occurrence( 532, dWARNING, strJamCode );
			return false;
		}
	}
	else if( abs( st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] - g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_Y) ) > COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
	{
		//ybs
		if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[nPos] + COMI.md_allow_value[MOTOR_TRAY_TR_Y])
		{
		}
		else
		{
			CString strJamCode = GetJamCode_Transfer_Y( nPos );
			CTL_Lib.Alarm_Error_Occurrence( 533, dWARNING, strJamCode );
			return false;
		}
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Chk_Transfer_Dn( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_reject_transfer_updn, iOn) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_reject_transfer_dn_chk, iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_reject_transfer_up_chk, !iOn ) != iOn )
	{
		return true;
	}

	CString strJamCode = GetJamCode_Transfer_Dn( iOn );
	CTL_Lib.Alarm_Error_Occurrence( 534, dWARNING, strJamCode );
	return false;
}

bool CRun_Reject_Tray_Transfer::Chk_Guide( int iOn )
{
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_guide_fwdbwd[0], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_fwd_chk[0], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_bwd_chk[0], !iOn ) != iOn )
		{
			return true;
		}

		
		CString strJamCode = GetJamCode_TrayGuide_Chk( 0, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 535, dWARNING, strJamCode );
	}
	else
	{
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_guide_fwdbwd[2], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_fwd_chk[2], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_bwd_chk[2], !iOn ) != iOn )
		{
			return true;
		}
		
		CString strJamCode = GetJamCode_TrayGuide_Chk( 2, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 535, dWARNING, strJamCode );
	}
	return false;	
}

bool CRun_Reject_Tray_Transfer::Chk_TrayExist( int iOn )
{
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_tray_chk[0], iOn ) == iOn )
		{
			return true;
		}
		
		CString strJamCode = GetJamCode_TrayExist_Chk( 0, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 536, dWARNING, strJamCode );
	}
	else
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_tray_chk[2], iOn ) == iOn )
		{
			return true;
		}
		
		CString strJamCode = GetJamCode_TrayExist_Chk( 2, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 536, dWARNING, strJamCode );
	}
	return false;	
}

bool CRun_Reject_Tray_Transfer::Chk_TransferTray( int iOn )
{
	if( 1 || g_ioMgr.get_in_bit( st_io.i_reject_transfer_tray_chk, iOn ) == iOn )
	{
		return true;
	}

	CString strJamCode = GetJamCode_TransferTray( iOn );
	CTL_Lib.Alarm_Error_Occurrence( 537, dWARNING, strJamCode );
	return false;
}

bool CRun_Reject_Tray_Transfer::Chk_Clamp( int nPos )
{
	if( abs( st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[nPos] - g_comiMgr.Get_MotCurrentPos(MOTOR_TRAY_TR_CLAMP) ) > COMI.md_allow_value[MOTOR_TRAY_TR_CLAMP] )
	{
		CString strJamCode = GetJamCode_Transfer_C( nPos );
		CTL_Lib.Alarm_Error_Occurrence( 538, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Chk_Reject_Z( bool bAfterPlace )
{
	double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
	double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
	int iRejCnt = g_Tray.GetRejCnt();
	if( bAfterPlace == false )
		iRejCnt++;

	double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
//ybs
// 	if( abs( dTarget - g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_STACKER ) ) > COMI.md_allow_value[ MOTOR_TRAY_STACKER] )
	if( abs( dTarget - g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_STACKER ) ) > (COMI.md_allow_value[ MOTOR_TRAY_STACKER] + 7)) //2012,1229
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMRS00" );
		CTL_Lib.Alarm_Error_Occurrence( 539, dWARNING, "462000" );
		return false;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_To_Bin_ChkJam()
{
	switch( m_step_to_bin )
	{
	case ETT_BIN_NONE:
	case ETT_BIN_READY:
	case ETT_BIN_ROBOT_CHK:
	case ETT_BIN_ROBOT_MOVE:
	case ETT_BIN_ROBOT_MOVE_CHK:
		break;

	case ETT_BIN_UNCLAMP:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_ON ) == false )					return false;
		break;

	case ETT_BIN_READY_UP:
	case ETT_BIN_READY_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		break;

	case ETT_BIN_STACKER_MOVE:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_ON ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PICK_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PICK_DOWN:
	case ETT_BIN_PICK_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PICK_CLAMP:
	case ETT_BIN_PICK_CLAMP_CHK:
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PICK_UP:
	case ETT_BIN_PICK_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PLACE_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PLACE_GUIDE:
	case ETT_BIN_PLACE_GUIDE_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PLACE_DOWN:
	case ETT_BIN_PLACE_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_CLAMP ) == false )					return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PLACE_UNCLAMP:
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_PLACE_UP:
	case ETT_BIN_PLACE_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_ON ) == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Guide( IO_ON ) == false )							return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_BIN_FINISH:
		break;
	}
	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_To_Rej_ChkTime()
{
	switch( m_step_to_rej )
	{
	case ETT_REJ_UNCLAMP:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 540, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_REJ_STACKER_MOVE:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 541, dWARNING, "622000" );
			return false;
		}
		break;

	case ETT_REJ_PICK_MOVE_Y:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 542, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_REJ_PICK_CLAMP:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC01" );
			CTL_Lib.Alarm_Error_Occurrence( 543, dWARNING, "623001" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 544, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_UNCLAMP:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 545, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_Y_BIN:
		if( InTime_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 546, dWARNING, "624000" );
			return false;
		}
		break;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_To_Bin_ChkTime()
{
	switch( m_step_to_bin )
	{
	case ETT_BIN_UNCLAMP:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 550, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_BIN_PICK_CLAMP_CHK:
		if( InTime_ToBin( 10000 ) == false )
		{
			// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CString strJamCode = GetJamCode_TransferTray( IO_ON );
			CTL_Lib.Alarm_Error_Occurrence( 537, dWARNING, strJamCode );
			return false;
		}
		break;

	case ETT_BIN_STACKER_MOVE:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 551, dWARNING, "622000" );
			return false;
		}
		break;

	case ETT_BIN_PICK_MOVE_Y:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 552, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_BIN_PICK_CLAMP:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC01" );
			CTL_Lib.Alarm_Error_Occurrence( 553, dWARNING, "623001" );
			mn_clamp_retry_cnt++;
			if( mn_clamp_retry_cnt > 2)
			{
				mn_clamp_retry_cnt = 0;
				return false;
			}
			else
			{
				COMI.Set_MotStop(0, MOTOR_TRAY_TR_CLAMP);				
				m_step_to_bin = ETT_BIN_CLAMP_HOME;
			}
		}
		break;

	case ETT_BIN_PLACE_MOVE_Y:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 554, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_BIN_PLACE_UNCLAMP:
		if( InTime_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 555, dWARNING, "623000" );
			return false;
		}
		break;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::InTime_ToRej( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_ToRej < 0)
	{
		m_dwTime_ToRej = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_ToRej < iMilliSec )
		return true;

	return false;
}

bool CRun_Reject_Tray_Transfer::InTime_Transfer_ToRej( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_transfer_ToRej < 0)
	{
		m_dwTime_transfer_ToRej = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_transfer_ToRej < iMilliSec )
		return true;

	return false;
}

bool CRun_Reject_Tray_Transfer::InTime_ToBin( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_ToBin < 0)
	{
		m_dwTime_ToBin = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_ToBin < iMilliSec )
		return true;

	return false;
}

bool CRun_Reject_Tray_Transfer::InTime_Transfer_ToBin( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_transfer_ToBin < 0)
	{
		m_dwTime_transfer_ToBin = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_transfer_ToBin < iMilliSec )
		return true;

	return false;
}

//ybs
//BIN3에 트레이 로드 언로드시에 Transfer를 Reject로 이동시킴.
void CRun_Reject_Tray_Transfer::ReqMoveTransferToRej()
{
	if( m_step_transfer_to_rej == ETT_REJ_NONE )
		m_step_transfer_to_rej = ETT_REJ_READY;
}

void CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Rej()
{
	if( m_step_transfer_to_rej_old != m_step_transfer_to_rej )
	{
		m_step_transfer_to_rej_old =  m_step_transfer_to_rej;
		m_dwTime_transfer_ToRej = GetCurrentTime();
		
		if( Run_Move_Transfer_To_Rej_ChkJam() == false )
			return;
	}

	if( Run_Move_Transfer_To_Rej_ChkTime() == false )
		return;

	Func.ThreadFunctionStepTrace(12, m_step_transfer_to_rej);
	switch( m_step_transfer_to_rej )
	{
	case ETT_REJ_NONE:				break;

	case ETT_REJ_READY:
//		g_Tray.TrayMoved( TPOS_BIN3, TPOS_BIN_TO_REJECT );
		m_step_transfer_to_rej = ETT_REJ_ROBOT_CHK;
		break;

	case ETT_REJ_ROBOT_CHK:
		{
			double dX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			//ybs 2013,0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( dX <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_rej = ETT_REJ_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_rej = ETT_REJ_READY_UP;
				}
			}
			else
			{
				if( dX >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_rej = ETT_REJ_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_rej = ETT_REJ_READY_UP;
				}
			}
		}
		break;
	
	case ETT_REJ_ROBOT_MOVE:
		//g_robot.AddJob( PICK, -1, -1, MPOS_INDEX_BIN2 );
		m_step_transfer_to_rej = ETT_REJ_ROBOT_MOVE_CHK;
		break;

	case ETT_REJ_ROBOT_MOVE_CHK://ybs
		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_transfer_to_rej = ETT_REJ_READY_UP;
			else if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] + COMI.md_allow_value[MOTOR_TRAY_TR_Y])
			{
				if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END])
				{
					m_step_transfer_to_rej = ETT_REJ_READY_UP;
				}
			}
			
		}
		else
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_transfer_to_rej = ETT_REJ_READY_UP;
			else if(g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) > st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] &&
				g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] + COMI.md_allow_value[MOTOR_TRAY_TR_Y])
			{
				if(g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ) < st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END])
				{
					m_step_transfer_to_rej = ETT_REJ_READY_UP;
				}
			}
		}
		break;

	case ETT_REJ_READY_UP:
		ReqCylZ( BACKWARD );
		m_step_transfer_to_rej = ETT_REJ_READY_UP_CHK;
		break;

	case ETT_REJ_READY_UP_CHK:
		if( GetCylZ() != FB_NONE )		return;
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_rej = ETT_REJ_PLACE_MOVE_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 560, CTL_dWARNING, strJamCode );
			}
		}		
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_rej = ETT_REJ_PLACE_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY01" );
				strJamCode.Format( "453101" );
				CTL_Lib.Alarm_Error_Occurrence( 561, CTL_dWARNING, strJamCode );
			}
		}		
		break;

	case ETT_REJ_PLACE_UP:
		ReqCylZ( BACKWARD );
		m_step_transfer_to_rej = ETT_REJ_PLACE_UP_CHK;
		break;

	case ETT_REJ_PLACE_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_transfer_to_rej = ETT_REJ_PLACE_CLAMP_SAFETY;
		break;

	case ETT_REJ_PLACE_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_rej = ETT_REJ_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC02" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 562, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case ETT_REJ_FINISH:
//		g_Tray.TrayMoved( TPOS_BIN_TO_REJECT, TPOS_REJECT );
		m_step_transfer_to_rej = ETT_REJ_NONE;
		break;
	}
}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Rej_ChkTime()
{
	switch( m_step_transfer_to_rej )
	{
	case ETT_REJ_UNCLAMP:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 570, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_REJ_STACKER_MOVE:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 571, dWARNING, "622000" );
			return false;
		}
		break;

	case ETT_REJ_PICK_MOVE_Y:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 572, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_REJ_PICK_CLAMP:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC01" );
			CTL_Lib.Alarm_Error_Occurrence( 573, dWARNING, "623001" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 574, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_UNCLAMP:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 575, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_REJ_PLACE_Y_BIN:
		if( InTime_Transfer_ToRej( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 576, dWARNING, "624000" );
			return false;
		}
		break;
	}

	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Rej_ChkJam()
{
	switch( m_step_transfer_to_rej )
	{
	case ETT_REJ_NONE:
	case ETT_REJ_READY:
	case ETT_REJ_ROBOT_CHK:
	case ETT_REJ_ROBOT_MOVE:
	case ETT_REJ_ROBOT_MOVE_CHK:
		break;

	case ETT_REJ_UNCLAMP:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_REJ_READY_UP:
	case ETT_REJ_READY_UP_CHK:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
// 		if( Chk_Guide( IO_ON ) == false )							return false;
		break;

	case ETT_REJ_STACKER_MOVE:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		break;

	case ETT_REJ_PICK_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		break;

	case ETT_REJ_PICK_DOWN:
	case ETT_REJ_PICK_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
// 		if( Chk_Guide( IO_ON ) == false )							return false;
		break;

	case ETT_REJ_PICK_CLAMP:
	case ETT_REJ_PICK_CLAMP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
// 		if( Chk_Guide( IO_ON ) == false )							return false;
		break;

	case ETT_REJ_PICK_UP:
	case ETT_REJ_PICK_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Reject_Z( false ) == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		break;

	case ETT_REJ_PLACE_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_REJ_PLACE_DOWN:
	case ETT_REJ_PLACE_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		break;

	case ETT_REJ_PLACE_UNCLAMP:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TrayExist( IO_OFF ) == false )						return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;
		break;

	case ETT_REJ_PLACE_UP:
	case ETT_REJ_PLACE_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_REJ_PLACE_Y_BIN:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_REJ_MOVE_WORK:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )		return false;

	case ETT_REJ_FINISH:
		break;
	}

	return true;
}

BOOL CRun_Reject_Tray_Transfer::Bin_Stacker_Rail()
{
	if( st_handler.mn_init_state[INIT_BIN_STACKER_1] == CTL_NO )
	{
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "[BIN_STACKER_1] 초기화가 되어 있지 않습니다.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;
	}
	if( st_handler.mn_init_state[INIT_BIN_STACKER_2] == CTL_NO )
	{
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "[BIN_STACKER_2] 초기화가 되어 있지 않습니다.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;
	}
	if( st_handler.mn_init_state[INIT_BIN_STACKER_3] == CTL_NO )
	{
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "[BIN_STACKER_3] 초기화가 되어 있지 않습니다.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;//ybs
	}
	if( st_handler.mn_init_state[INIT_BIN_STACKER_REJECT] == CTL_NO )
	{
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "[BIN_STACKER_REJECT] 초기화가 되어 있지 않습니다.");
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
		return false;
	}
	
	// 0, 180 = FORWARD,
	if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180 )
	{
		ReqRail( FORWARD );
	}
	else
	{
		ReqRail( BACKWARD );
	}
	
	return true;
}

void CRun_Reject_Tray_Transfer::ReqMoveTransferToBin()
{
	if( m_step_transfer_to_bin == ETT_BIN_NONE )
		m_step_transfer_to_bin = ETT_BIN_READY;
}

void CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Bin()
{
	if( m_step_transfer_to_bin_old != m_step_transfer_to_bin )
	{
		m_step_transfer_to_bin_old =  m_step_transfer_to_bin;
		m_dwTime_transfer_ToBin = GetCurrentTime();
			
		if( Run_Move_Transfer_To_Bin_ChkJam() == false )
			return;
	}

	if( Run_Move_Transfer_To_Bin_ChkTime() == false )
		return;

	Func.ThreadFunctionStepTrace(13, m_step_transfer_to_bin);
	switch( m_step_transfer_to_bin )
	{
	case ETT_BIN_NONE:				break;

	case ETT_BIN_READY:
		//g_Tray.TrayMoved( TPOS_REJECT, TPOS_REJECT_TO_BIN );
		m_step_transfer_to_bin = ETT_BIN_ROBOT_CHK;
		break;

	case ETT_BIN_ROBOT_CHK:
		{
			double dX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			//2013,0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( dX < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_bin = ETT_BIN_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_bin = ETT_BIN_READY_UP;
				}
			}
			else
			{
				if( dX >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_bin = ETT_BIN_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_bin = ETT_BIN_READY_UP;
				}
				
			}
		}
		break;
	
	case ETT_BIN_ROBOT_MOVE:
		//g_robot.AddJob( PICK, -1, -1, MPOS_INDEX_BIN2 );
		m_step_transfer_to_bin = ETT_BIN_ROBOT_MOVE_CHK;
		break;

	case ETT_BIN_ROBOT_MOVE_CHK:
		//ybs 2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_transfer_to_bin = ETT_BIN_READY_UP;
		}
		else
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
				m_step_transfer_to_bin = ETT_BIN_READY_UP;
		}
		break;

	case ETT_BIN_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_bin = ETT_BIN_READY_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 580, CTL_dWARNING, strJamCode );
			}
		}		
		break;
	
	case ETT_BIN_READY_UP:
		ReqCylZ( BACKWARD );
		m_step_transfer_to_bin = ETT_BIN_READY_UP_CHK;
		break;

	case ETT_BIN_READY_UP_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_transfer_to_bin = ETT_BIN_STACKER_GUIDE_BACK;
		break;

	case ETT_BIN_STACKER_GUIDE_BACK:
		//2013,1028
		if( CHINA_VER == 1)
		{
			if( g_ioMgr.get_out_bit( st_io.o_reject_stacker_guide, IO_ON ) == IO_ON &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_forward_chk ) == IO_ON &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk ) == IO_OFF &&
				g_ioMgr.get_out_bit( st_io.o_reject_stacker_seperator, IO_OFF) == IO_OFF &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_seperator_backward_chk ) == IO_ON)
			{
				m_step_transfer_to_bin = ETT_BIN_PLACE_MOVE_Y;
				break;;
			}
		}
		else
		{
			if( g_ioMgr.get_out_bit( st_io.o_reject_stacker_guide, IO_ON ) == IO_ON &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_forward_chk ) == IO_ON &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk ) == IO_OFF )
			{
				m_step_transfer_to_bin = ETT_BIN_PLACE_MOVE_Y;
				break;;
			}
		}
		//2013,1028
		if( CHINA_VER == 1)
		{
			Run_Reject_Stacker.ReqSeperator( BACKWARD );
		}
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		m_step_transfer_to_bin = ETT_BIN_STACKER_GUIDE_BACK_CHK;
		break;
		
	case ETT_BIN_STACKER_GUIDE_BACK_CHK:
		//2013,1028
		if( CHINA_VER == 1)
		{
			if( Run_Reject_Stacker.GetSeperator() != FB_NONE )
				return;
		}
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_transfer_to_bin = ETT_BIN_STACKER_CHK;
		break;

	case ETT_BIN_STACKER_CHK:
		m_step_transfer_to_bin = ETT_BIN_STACKER_GUIDE_FORWARD;
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
			{
				m_step_transfer_to_bin = ETT_BIN_STACKER_MOVE;
			}
		}
		break;

	case ETT_BIN_STACKER_MOVE:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_bin = ETT_BIN_STACKER_GUIDE_FORWARD;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 524, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ETT_BIN_STACKER_GUIDE_FORWARD:
		Run_Reject_Stacker.ReqGuide( FORWARD );
		m_step_transfer_to_bin = ETT_BIN_STACKER_GUIDE_FORWARD_CHK;
		break;

	case ETT_BIN_STACKER_GUIDE_FORWARD_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_transfer_to_bin = ETT_BIN_PLACE_MOVE_Y;
		break;
	
	case ETT_BIN_PLACE_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_bin = ETT_BIN_PLACE_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTY00" );
				strJamCode.Format( "453100" );
				CTL_Lib.Alarm_Error_Occurrence( 581, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_BIN_PLACE_UP:
		ReqCylZ( BACKWARD );
		m_step_transfer_to_bin = ETT_BIN_PLACE_UP_CHK;
		break;

	case ETT_BIN_PLACE_UP_CHK:
		if( GetCylZ() != FB_NONE )			return;
		m_step_transfer_to_bin = ETT_BIN_PLACE_CLAMP_SAFETY;
		break;

	case ETT_BIN_PLACE_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_bin = ETT_BIN_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC02" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 582, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case ETT_BIN_FINISH:
//		g_Tray.TrayMoved( TPOS_REJECT_TO_BIN, TPOS_BIN3 );
		m_step_transfer_to_bin = ETT_BIN_NONE;
		break;
	}
}

bool CRun_Reject_Tray_Transfer::InTime_Transfer_ToSafety( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_transfer_ToSafety < 0)
	{
		m_dwTime_transfer_ToSafety = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_transfer_ToSafety < iMilliSec )
		return true;

	return false;
}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Safety_ChkJam()
{
	switch(m_step_transfer_to_safety)
	{
	case ETT_SAFETY_NONE:
	case ETT_SAFETY_READY:
	case ETT_SAFETY_ROBOT_CHK:
	case ETT_SAFETY_ROBOT_MOVE:
		break;

	case ETT_SAFETY_UNCLAMP:
		if( Chk_Transfer_Dn( IO_OFF ) == false )				return false;
// 		if( Chk_TrayExist( IO_ON ) == false )						return false;
		break;

	case ETT_SAFETY_READY_UP:
	case ETT_SAFETY_READY_UP_CHK:
// 		if( Chk_TrayExist( IO_ON ) == false )					return false;
		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )				return false;
		break;

	case ETT_SAFETY_STACKER_MOVE:
		if( Chk_Transfer_Dn( IO_OFF ) == false )				return false;
//		if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )				return false;
		break;

	case ETT_SAFETY_FINISH:
		if( Chk_Transfer_Dn( IO_OFF ) == false )				return false;
		if( Chk_Clamp( P_CLAMP_SAFETY ) == false )				return false;
// 		if( Chk_Reject_Z( false ) == false )					return false;
		break;
	}
	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Safety_ChkTime()
{
	switch( m_step_transfer_to_safety )
	{
	case ETT_SAFETY_UNCLAMP:
		if( InTime_Transfer_ToSafety ( 10000 ) == false )
		{
			CTL_Lib.Alarm_Error_Occurrence( 1001, dWARNING, "623000" );
		}
		break;
	
	case ETT_SAFETY_STACKER_MOVE:
		if( InTime_Transfer_ToSafety( 10000 ) == false )
		{
			CTL_Lib.Alarm_Error_Occurrence( 1002, dWARNING, "622000" );
		}
		break;

	case ETT_SAFETY_CLAMP_SAFETY:
		if( InTime_Transfer_ToSafety( 10000 ) == false )
		{//623002 1 A "트랜스퍼 Safety 제한시간 에러."
			CTL_Lib.Alarm_Error_Occurrence( 1004, dWARNING, "623002" );
		}
		break;
		
		
	case ETT_SAFETY_MOVE_Y:
		if( InTime_Transfer_ToSafety( 10000 ) == false )
		{
			CTL_Lib.Alarm_Error_Occurrence( 1003, dWARNING, "624000" );
		}
		break;

	}

	return true;
}

void CRun_Reject_Tray_Transfer::ReqMoveTransferToSafety()
{
	if( m_step_transfer_to_safety == ETT_SAFETY_NONE )
		m_step_transfer_to_safety = ETT_SAFETY_READY;
}

void CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Safety()
{
	if( m_step_transfer_to_safety_old != m_step_transfer_to_safety )
	{
		m_step_transfer_to_safety_old =  m_step_transfer_to_safety;
		m_dwTime_transfer_ToSafety = GetCurrentTime();
			
		if( Run_Move_Transfer_To_Safety_ChkJam() == false )
			return;
	}

	if( Run_Move_Transfer_To_Safety_ChkTime() == false )
		return;

	Func.ThreadFunctionStepTrace(14, m_step_transfer_to_safety);
	switch( m_step_transfer_to_safety )
	{
	case ETT_SAFETY_NONE:				break;

	case ETT_SAFETY_READY:
		//g_Tray.TrayMoved( TPOS_REJECT, TPOS_REJECT_TO_BIN );
		m_step_transfer_to_safety = ETT_SAFETY_ROBOT_CHK;
		break;

	case ETT_SAFETY_ROBOT_CHK:
		{
			double dX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );

			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( dX < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_END] + COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_safety = ETT_SAFETY_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_safety = ETT_SAFETY_UNCLAMP;
				}
			}
			else
			{
				if( dX >= st_motor[MOTOR_ROBOT_X].d_pos[P_XY_REJ_START] - COMI.md_allow_value[MOTOR_ROBOT_X] )
				{
					m_step_transfer_to_safety = ETT_SAFETY_ROBOT_MOVE;
				}
				else
				{
					m_step_transfer_to_safety = ETT_SAFETY_UNCLAMP;
				}
				
			}
//			if( dX > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_MOVE_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_X] )
//			{
//				m_step_transfer_to_safety = ETT_SAFETY_ROBOT_MOVE;
//			}
//			else
//			{
//				m_step_transfer_to_safety = ETT_SAFETY_UNCLAMP;
//			}
		}
		break;
	
	case ETT_SAFETY_ROBOT_MOVE:
//		if(st_handler.mn_mirror_type == CTL_YES)
//		{
//			g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN3 );
//		}
//		else
//		{
//			g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN1 );
//		}
		//2014,0709
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN3 );
		}
		else
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN1 );
		}

		m_step_transfer_to_safety = ETT_SAFETY_ROBOT_MOVE_CHK;
		break;

	case ETT_SAFETY_ROBOT_MOVE_CHK:
		m_step_transfer_to_safety = ETT_SAFETY_UNCLAMP;
		break;

	case ETT_SAFETY_UNCLAMP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_UNCLAMP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_safety = ETT_SAFETY_READY_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMTC00" );
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 589, CTL_dWARNING, strJamCode );
			}
		}		
		break;
	
	case ETT_SAFETY_READY_UP:
		ReqCylZ( BACKWARD );
		m_step_transfer_to_safety = ETT_SAFETY_READY_UP_CHK;
		break;

	case ETT_SAFETY_READY_UP_CHK:
		if( GetCylZ() != FB_NONE )		return;
		m_step_transfer_to_safety = ETT_SAFETY_STACKER_GUIDE_BACK;
		break;

	case ETT_SAFETY_STACKER_GUIDE_BACK:
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		m_step_transfer_to_safety = ETT_SAFETY_STACKER_GUIDE_BACK_CHK;
		break;
		
	case ETT_SAFETY_STACKER_GUIDE_BACK_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		m_step_transfer_to_safety = ETT_SAFETY_STACKER_CHK;
		break;

	case ETT_SAFETY_STACKER_CHK:
		m_step_transfer_to_safety = ETT_SAFETY_CLAMP_SAFETY;
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
			{
				m_step_transfer_to_safety = ETT_SAFETY_STACKER_MOVE;
			}
		}
		break;

	case ETT_SAFETY_STACKER_MOVE:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			double dTarget = dPos_1 + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_safety = ETT_SAFETY_CLAMP_SAFETY;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 519, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ETT_SAFETY_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_safety = ETT_SAFETY_MOVE_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 520, CTL_dWARNING, strJamCode );
			}
		}
		break;


	case ETT_SAFETY_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_transfer_to_safety = ETT_SAFETY_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "453100" );
				CTL_Lib.Alarm_Error_Occurrence( 581, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case ETT_SAFETY_FINISH:
//		g_Tray.TrayMoved( TPOS_REJECT_TO_BIN, TPOS_BIN3 );
		m_step_transfer_to_safety = ETT_SAFETY_NONE;
		break;
	}

}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Bin_ChkJam()
{
	switch( m_step_transfer_to_bin )
	{
	case ETT_BIN_NONE:
	case ETT_BIN_READY:
	case ETT_BIN_ROBOT_CHK:
	case ETT_BIN_ROBOT_MOVE:
	case ETT_BIN_ROBOT_MOVE_CHK:
		break;

	case ETT_BIN_UNCLAMP:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_BIN_READY_UP:
	case ETT_BIN_READY_UP_CHK:
		//if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		break;

	case ETT_BIN_STACKER_MOVE:
		//if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_ON ) == false )					return false;
		break;

	case ETT_BIN_PICK_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PICK_DOWN:
	case ETT_BIN_PICK_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PICK_CLAMP:
	case ETT_BIN_PICK_CLAMP_CHK:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;//ybs IO_ON -> IO_OFF
//		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;//ybs P_TRANSFER_Y_REJ->P_TRANSFER_Y_BIN
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Reject_Z( true ) == false)							return false;
		break;

	case ETT_BIN_PICK_UP:
	case ETT_BIN_PICK_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_REJ ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PLACE_MOVE_Y:
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		//if( Chk_TransferTray( IO_OFF ) == false )					return false;//ybs 트레이 감지 제거 필요없음
		//if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PLACE_GUIDE:
	case ETT_BIN_PLACE_GUIDE_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( Chk_Transfer_Dn( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PLACE_DOWN:
	case ETT_BIN_PLACE_DOWN_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_TransferTray( IO_OFF ) == false )					return false;
		//if( Chk_Reject_Z( true ) == false )							return false;
		//if( Chk_Guide( IO_ON ) == false )							return false;
		break;

	case ETT_BIN_PLACE_UNCLAMP:
		if( Chk_Transfer_Dn( IO_ON ) == false )						return false;
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		if( Chk_Reject_Z( true ) == false )							return false;
		break;

	case ETT_BIN_PLACE_UP:
	case ETT_BIN_PLACE_UP_CHK:
		if( Chk_Transfer_Y( P_TRANSFER_Y_BIN ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )						return false;
		//if( Chk_Reject_Z( true ) == false )							return false;
		//if( Chk_Clamp( P_CLAMP_UNCLAMP ) == false )					return false;
		break;

	case ETT_BIN_FINISH:
		break;
	}
	return true;
}

bool CRun_Reject_Tray_Transfer::Run_Move_Transfer_To_Bin_ChkTime()
{
	switch( m_step_transfer_to_bin )
	{
	case ETT_BIN_UNCLAMP:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 583, dWARNING, "623000" );
			return false;
		}
		break;

	case ETT_BIN_STACKER_MOVE:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 584, dWARNING, "622000" );
			return false;
		}
		break;

	case ETT_BIN_PICK_MOVE_Y:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 585, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_BIN_PICK_CLAMP:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC01" );
			CTL_Lib.Alarm_Error_Occurrence( 586, dWARNING, "623001" );
			return false;
		}
		break;

	case ETT_BIN_PLACE_MOVE_Y:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTY00" );
			CTL_Lib.Alarm_Error_Occurrence( 587, dWARNING, "624000" );
			return false;
		}
		break;

	case ETT_BIN_PLACE_UNCLAMP:
		if( InTime_Transfer_ToBin( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMTC00" );
			CTL_Lib.Alarm_Error_Occurrence( 588, dWARNING, "623000" );
			return false;
		}
		break;
	}

	return true;
}

void CRun_Reject_Tray_Transfer::OnRobot_FinalPos()
{
	mn_reinstate_step = 0;
}

void CRun_Reject_Tray_Transfer::Run_Reinstate()
{
	if(st_work.mn_reinstate_reject_robot == CTL_YES) return;
	int i=0, m_nRecovery = 0;

	switch(mn_reinstate_step)
	{
	case 0:
		if(st_work.mn_xyz_safety == CTL_YES)
		{
			mn_reinstate_step = 100;
		}
		break;

	case 100:
		m_dcurr_pos[0] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_X);
		m_dcurr_pos[1] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y);
		mn_reinstate_step = 200;
		break;

	case 200:
		mn_reinstate_pos[0] = COMI.Check_MotPosRange(MOTOR_TRAY_TR_Y, st_work.md_reinstatement_Pos[0][MOTOR_TRAY_TR_Y], COMI.md_allow_value[MOTOR_TRAY_TR_Y]); 
		mn_reinstate_pos[1] = COMI.Check_MotPosRange(MOTOR_TRAY_TR_CLAMP, st_work.md_reinstatement_Pos[0][MOTOR_TRAY_TR_CLAMP], COMI.md_allow_value[MOTOR_TRAY_TR_CLAMP]);
		m_nRecovery = 0;
		for(i=0; i<2; i++)
		{
			if(mn_reinstate_pos[i] == CTLBD_RET_ERROR)
			{
				m_nRecovery++;
			}
		}
		if(m_nRecovery > 0)
		{
			mn_reinstate_step = 300;
		}
		else
		{
			mn_reinstate_step = 5000;
		}
		break;
		
	case 300: //일단 실린더를 업한다
		mn_reinstate_pos[0] = COMI.Check_MotPosRange(MOTOR_TRAY_TR_Y, st_work.md_reinstatement_Pos[0][MOTOR_TRAY_TR_Y], COMI.md_allow_value[MOTOR_TRAY_TR_Y]);
		if(mn_reinstate_pos[0] == CTLBD_RET_ERROR)
		{
			mn_reinstate_step = 400;
		}
		else
		{
			mn_reinstate_step = 500;
		}
		break;

	case 400:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nManualSpeed );
			if( nRet == BD_GOOD )
			{
				mn_reinstate_step = 500;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 589, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case 500:
		ReqCylZ( BACKWARD );
		mn_reinstate_step = 600;
		break;

	case 600:
		if( GetCylZ() != FB_NONE )			return;
		mn_reinstate_step = 700;
		break;

	case 700:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_Y, st_work.md_reinstatement_Pos[0][MOTOR_TRAY_TR_Y], st_basic.nManualSpeed );
			if( nRet == BD_GOOD )
			{
				mn_reinstate_step = 1000;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 590, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case 1000:
		if(st_work.mn_transfer_cyl == IO_ON) ReqCylZ( FORWARD );
		else                                 ReqCylZ( BACKWARD );
		mn_reinstate_step = 1100;
		break;
		
	case 1100:
		if( GetCylZ() != FB_NONE )			return;
		mn_reinstate_step = 1200;
		break;

	case 1200:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_work.md_reinstatement_Pos[0][MOTOR_TRAY_TR_CLAMP], st_basic.nManualSpeed );
			if( nRet == BD_GOOD )
			{
				mn_reinstate_step = 5000;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 591, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case 5000:
		st_work.mn_reinstate_reject_robot = CTL_YES;
		break;
	}
}