// APartHandler_COK.cpp: implementation of the APartHandler_COK class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "handler.h"
#include "APartHandler_COK.h"
#include "../AMTRegistry.h"
#include "PartFunction.h"
#include "../ComizoaPublic.h"
#include "../IO_Manager.h"
#include "APartHandler.h"
#include "../Run_Reject_Tray_Transfer.h"
#include "APartNetworkManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APartHandler_COK g_handler_cok;

APartHandler_COK::APartHandler_COK()
{
	m_cct = CCT_NONE;
	m_nTargetCokIdx = -1;
	m_step_safety = ECCT_SAFETY_START;
	m_step_recv = ECCT_RECV_START;
	m_step_send = ECCT_SEND_START;
	m_step_ready = ECCT_READY_START;

	m_step_safety_old = ECCT_SAFETY_START;
	m_step_recv_old = ECCT_RECV_START;
	m_step_send_old = ECCT_SEND_START;

	st_handler.mn_cok_change = CTL_NO;
	LoadRegDatas();
}

APartHandler_COK::~APartHandler_COK()
{

}

void APartHandler_COK::Run_Move()
{

	switch( m_cct )
	{
	case CCT_COK_CHANGE:	Run_Move_Safety();		break;
	case CCT_RECV:			Run_Move_COK_RECV();	break;
	case CCT_SEND:			Run_Move_COK_SEND();	break;
	case CCT_WORK_READY:	Run_Move_Work_Ready();	break;
	}

	if( COMI.mn_run_status != dRUN )
	{
		m_step_safety_old = ECCT_SAFETY_START;
		m_step_recv_old = ECCT_RECV_START;
		m_step_send_old = ECCT_SEND_START;
		return;
	}
}

void APartHandler_COK::Run_Move_Safety()
{
	double dSpeedRatio[3];
	dSpeedRatio[0] = (double)st_basic.nRunSpeed;				// 1/10 속도로 복귀 한다.
	dSpeedRatio[1] = (double)st_basic.nRunSpeed;
	dSpeedRatio[2] = (double)st_basic.nRunSpeed;
	
	long lAxis[2];
	lAxis[0] = MOTOR_ROBOT_X;
	lAxis[1] = MOTOR_ROBOT_Y;
	
	if( m_step_safety_old != m_step_safety )
	{
		m_step_safety_old = m_step_safety;
		m_dwTime_Safety = GetCurrentTime();
		
		if( Run_Move_COK_SAFETY_Chk_Jam() == false )
			return;
	}
	
	if( Run_Move_Cok_SAFETY_Chk_Time() == false )
		return;
	
	switch( m_step_safety )
	{
	case ECCT_SAFETY_START:
		if(st_basic.mn_hotfix_pos <= 0 || st_basic.mn_hotfix_pos >= 6)
		{
			if (st_handler.cwnd_list != NULL)
			{
				sprintf(st_msg.c_abnormal_msg, "[ADMIN]Basica메뉴에서 HitFix 세팅값이 0이거나 6보다 큽니다.");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
			}
			CTL_Lib.Alarm_Error_Occurrence( 897, dWARNING, "630300" );

			break;
		}
	
		g_client_bpc.SetTestMode( "TestMode_HotFix" );//2012,0504
		m_step_safety = ECCT_ROBOT_Z_SAFETY;
		break;
	case ECCT_ROBOT_Z_SAFETY:
		{
			int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_Z, MOT_TIMEOUT);
			if( nRet == BD_GOOD )
			{
				Run_Reject_Tray_Transfer.ReqRail(BACKWARD);
				m_step_safety = ECCT_ROBOT_XY_SAFETY;
			}
			else if( nRet == BD_ERROR )
			{
				CTL_Lib.Alarm_Error_Occurrence(799, CTL_dWARNING, "440300");
			}
		}
		break;
		
// 	case ECCT_ROBOT_XY_SAFETY:
// 		{
// 			if(Run_Reject_Tray_Transfer.Bin_Stacker_Rail() == false)
// 			{
// 				CTL_Lib.Alarm_Error_Occurrence(896, CTL_dWARNING, "390200");
// 				break;
// 			}
// 			m_step_safety = ECCT_ROBOT_XY_SAFETY_MOVE;
// 		}
// 		break;
// 
// 	case ECCT_ROBOT_XY_SAFETY_MOVE:
// 		if( Run_Reject_Tray_Transfer.GetRail() != FB_NONE )
// 			return;
// 		g_robot.AddJob( PLACE, -1, -1, MPOS_INDEX_BIN1 );
// 		m_step_safety = ECCT_ROBOT_XY_SAFETY_CHK;
// 		break;
		
	case ECCT_ROBOT_XY_SAFETY:
		{
			if(Run_Reject_Tray_Transfer.GetRail() != FB_NONE) return;
			for (int i = 0; i < 3; i++)
			{
				st_handler.mn_cok_chgstate[i] = CTL_NO;
			}
			m_step_safety = ECCT_ROBOT_XY_SAFETY_CHK;
		}
		break;
		
	case ECCT_ROBOT_XY_SAFETY_CHK:
		if(st_handler.mn_cok_chgstate[0] != CTL_YES || st_handler.mn_cok_chgstate[1] != CTL_YES || st_handler.mn_cok_chgstate[2] != CTL_YES)
			return;
		
		if(Run_Reject_Tray_Transfer.Bin_Stacker_Rail() == false)
		{
			CTL_Lib.Alarm_Error_Occurrence(896, CTL_dWARNING, "390200");
			break;
		}
		m_step_safety = ECCT_ROBOT_XY_SAFETY_MOVE;
		break;
		
	case ECCT_ROBOT_XY_SAFETY_MOVE:
		if( Run_Reject_Tray_Transfer.GetRail() != FB_NONE )
			return;		
		
		//2013,0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN3 );
		}
		else
		{
			g_robot.AddJob( PLACE, -1, -1, -1, -1, MPOS_INDEX_BIN1 );
		}
		m_step_safety = ECCT_ROBOT_XY_SAFETY_MOVE_CHK;
		break;

	case ECCT_ROBOT_XY_SAFETY_MOVE_CHK:
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( g_robot.GetJobCount() == 0 && g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				Run_Bin_Stacker[0].ReqElvMoveRotateBase(FORWARD);
				Run_Bin_Stacker[1].ReqElvMoveRotateBase(FORWARD);
				Run_Bin_Stacker[2].ReqElvMoveRotateBase(FORWARD);
				m_step_safety = ECCT_ROBOT_TRANSFER_MOVE_SAFETY;
			}
		}
		else
		{
			if( g_robot.GetJobCount() == 0 && g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) < st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] - COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				Run_Bin_Stacker[0].ReqElvMoveRotateBase(FORWARD);
				Run_Bin_Stacker[1].ReqElvMoveRotateBase(FORWARD);
				Run_Bin_Stacker[2].ReqElvMoveRotateBase(FORWARD);
				m_step_safety = ECCT_ROBOT_TRANSFER_MOVE_SAFETY;
			}
		}
		break;
		
	case ECCT_ROBOT_TRANSFER_MOVE_SAFETY:
		if( Run_Reject_Tray_Transfer.GetRail() != FB_NONE )
			return;
		if( Run_Bin_Stacker[0].GetElvMoveRotateBaser() != FB_NONE )
			return;
		if( Run_Bin_Stacker[1].GetElvMoveRotateBaser() != FB_NONE )
			return;
		if( Run_Bin_Stacker[2].GetElvMoveRotateBaser() != FB_NONE )
			return;

		Run_Reject_Tray_Transfer.ReqMoveTransferToBin();
		m_step_safety = ECCT_ROBOT_TRANSFER_MOVE_SAFETY_CHK;
		break;
		
	case ECCT_ROBOT_TRANSFER_MOVE_SAFETY_CHK:
		if(Run_Reject_Tray_Transfer.GetMoveTransferToBin() == true)
		{
			m_step_safety = ECCT_RECV_ELEV_MOVE_READY;
		}
		break;

	case ECCT_RECV_ELEV_MOVE_READY://2013,1016
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_cct = CCT_NONE;
				st_handler.mn_cok_change = CTL_YES;
				m_step_safety = ECCT_SAFETY_START;
				CheckCokTypeChange();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "454100" );
				CTL_Lib.Alarm_Error_Occurrence( 833, CTL_dWARNING, strJamCode );
			}
		}
		break;
	}
	
}

void APartHandler_COK::Run_Move_COK_RECV()
{
	double dSpeedRatio[3];
	dSpeedRatio[0] = (double)st_basic.nRunSpeed;				// 1/10 속도로 복귀 한다.
	dSpeedRatio[1] = (double)st_basic.nRunSpeed;
	dSpeedRatio[2] = (double)st_basic.nRunSpeed;

	long lAxis[2];
	lAxis[0] = MOTOR_ROBOT_X;
	lAxis[1] = MOTOR_ROBOT_Y;
	
	int nSite = m_nTargetCokIdx / SITE_COK_CNT;
	int nIdx = m_nTargetCokIdx % SITE_COK_CNT;
	int nMotAxis = Run_Test_Site[nSite].GetMotID();


	if( m_step_recv_old != m_step_recv )
	{
		m_step_recv_old = m_step_recv;
		m_dwTime_Recv = GetCurrentTime();
		
		if( Run_Move_COK_RECV_Chk_Jam() == false )
			return;
	}

	if( Run_Move_Cok_RECV_Chk_Time() == false )
		return;

	switch( m_step_recv )
	{
	case ECCT_RECV_START:		m_step_recv = ECCT_RECV_ELEV_MOVE_Z_SAFETY;	break;

	case ECCT_RECV_ELEV_MOVE_Z_SAFETY:
		{
			if( COMI.Check_MotPosRange(MOTOR_COK_ROBOT_Y, GetPosCYAtCokStacker( m_nSite[m_nTargetCokIdx] ), COMI.md_allow_value[MOTOR_COK_ROBOT_Y]) == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_ELEV_MOVE_Y;
				break;
			}

			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, st_handler.md_common_teaching[CT_COK_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_ELEV_MOVE_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 800, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;
	case ECCT_RECV_ELEV_MOVE_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Y, GetPosCYAtCokStacker( m_nSite[m_nTargetCokIdx] ), st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_ELEV_MOVE_Z_TOP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 801, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_RECV_ELEV_MOVE_Z_TOP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, GetPosCZByCnt(m_nCokCnt[m_nSite[m_nTargetCokIdx]] ), st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_ELEV_Z_TOP_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 802, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_RECV_ELEV_Z_TOP_CHK:
		if( m_nCokCnt[m_nSite[m_nTargetCokIdx]] > 0 && g_ioMgr.get_in_bit( st_io.i_cok_stacker_top_chk[m_nSite[m_nTargetCokIdx]] ) == IO_ON )
			return;

		m_step_recv = ECCT_RECV_ELEV_DOWN_FLOOR;
		break;

	case ECCT_RECV_ELEV_DOWN_FLOOR:
		{
			double dTarget = GetPosCZByCnt(m_nCokCnt[m_nSite[m_nTargetCokIdx]] + 1 );
			if( m_nCokCnt[m_nSite[m_nTargetCokIdx]] >= 20 )
				dTarget = st_handler.md_common_teaching[CT_COK_Z_SAFETY];

			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_ELEV_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 803, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_RECV_ELEV_DOWN_CHK:
		if( g_ioMgr.get_in_bit( st_io.i_cok_stacker_top_chk[m_nSite[m_nTargetCokIdx]] ) == IO_OFF)
			return;

		m_step_recv = ECCT_RECV_BACK_PLATE;
		break;
		
	case ECCT_RECV_BACK_PLATE:
		Run_Test_Site[nSite].ReqPlate( BACKWARD );
		m_step_recv = ECCT_RECV_BACK_PLATE_CHK;
		break;

	case ECCT_RECV_BACK_PLATE_CHK:
		if( Run_Test_Site[nSite].GetPlate() != FB_NONE )
			return;

		m_step_recv = ECCT_RECV_INSERT_FORWARD;
		break;

	case ECCT_RECV_INSERT_FORWARD:
		{
			for( int i=0; i<INSERT_CNT; i++ )
			{
				Run_Test_Site[nSite].ReqInsert( i, FORWARD );
			}
			m_step_recv = ECCT_RECV_INSERT_FORWARD_CHK;			
		}	
		break;
	case ECCT_RECV_INSERT_FORWARD_CHK:
		{
			for( int i=0; i<INSERT_CNT; i++ )
			{
				if( Run_Test_Site[nSite].GetInsert( i ) != FB_NONE )
					return;
			}

			m_step_recv = ECCT_RECV_UNLOCK;
		}
		break;

	case ECCT_RECV_UNLOCK:
		Run_Test_Site[nSite].ReqLock( BACKWARD );
		m_step_recv = ECCT_RECV_UNLOCK_CHK;
		break;

	case ECCT_RECV_UNLOCK_CHK:
		if( Run_Test_Site[nSite].GetLock() != FB_NONE )
			return;

		m_step_recv = ECCT_RECV_ROBOT_Z_SAFTY;
		break;

	case ECCT_RECV_ROBOT_Z_SAFTY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_MOVESITE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 804, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_RECV_MOVESITE:
		{
			int nRet = CTL_Lib.Single_Move( nMotAxis, st_handler.md_common_teaching[CT_TSITE_1_Z + nSite], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_PICK_GV_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMS%d10", nSite+1 );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 805, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_RECV_PICK_GV_UP:
		{
			Run_Robot.ReqGripperDown( BACKWARD );
			for( int i=0; i<VACUUM_CNT; i++ )
			{
				Run_Robot.ReqVacDown(i, BACKWARD);
			}
			m_step_recv = ECCT_RECV_PICK_GV_UP_CHK;
		}
		break;
		
	case ECCT_RECV_PICK_GV_UP_CHK:
		{
			if( Run_Robot.GetGripperDown() != FB_NONE )	return;
			for( int i=0; i<VACUUM_CNT; i++ )
			{
				if( Run_Robot.GetVacDown(i) != FB_NONE )
					return;
			}
			m_step_recv = ECCT_RECV_PICK_ROBOT_XY_SITE;
		}
		break;

	case ECCT_RECV_PICK_ROBOT_XY_SITE:
		{
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PICK, 20 + m_nTargetCokIdx, -1 );
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PICK, 20 + m_nTargetCokIdx, -1 );

			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_PICK_GRIP_UNPICK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_abnormal_msg, "PICK_XY_SITE %d: [%f] [%f]", 20 + m_nTargetCokIdx, m_dPos[0], m_dPos[1]);
					Func.On_LogFile_Add(99, st_msg.c_abnormal_msg);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);
				}
				CTL_Lib.Alarm_Error_Occurrence( 806, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_RECV_PICK_GRIP_UNPICK:
		Run_Robot.ReqGripperOn( FORWARD );
		m_step_recv = ECCT_RECV_PICK_GRIP_UNPICK_CHK;
		break;

	case ECCT_RECV_PICK_GRIP_UNPICK_CHK:
		{
			if( Run_Robot.GetGripperOn() != FB_NONE )
				return;
		}
		m_step_recv = ECCT_RECV_PICK_ROBOT_Z_DOWN;
		break;

	case ECCT_RECV_PICK_ROBOT_Z_DOWN:
		{
			double dTarget = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, PICK, m_nTargetCokIdx + 20, -1 );

			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_PICK_GRIP_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 807, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_RECV_PICK_GRIP_DOWN:
		Run_Robot.ReqGripperDown( FORWARD );
		m_step_recv = ECCT_RECV_PICK_GRIP_DOWN_CHK;
		break;
	case ECCT_RECV_PICK_GRIP_DOWN_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return; 

		m_step_recv = ECCT_RECV_PICK_GRIP_PICK;
		break;

	case ECCT_RECV_PICK_GRIP_PICK:
		Run_Robot.ReqGripperOn( BACKWARD );
		m_step_recv = ECCT_RECV_PICK_GRIP_PICK_CHK;
		break;
		
	case ECCT_RECV_PICK_GRIP_PICK_CHK:
		{
			if( Run_Robot.GetGripperOn() != FB_NONE )
				return;
		}
		m_step_recv = ECCT_RECV_PICK_GRIP_UP;
		break;

	case ECCT_RECV_PICK_GRIP_UP:
		Run_Robot.ReqGripperDown( BACKWARD );
		m_step_recv = ECCT_RECV_PICK_GRIP_UP_CHK;
		break;

	case ECCT_RECV_PICK_GRIP_UP_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return; 

		m_step_recv = ECCT_RECV_PICK_ROBOT_Z_UP;

	case ECCT_RECV_PICK_ROBOT_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_MOVE_ROBOT_XY_COK_TACKER;
				
				// 데이터 이동..
				MoveDataSite2Robot();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 808, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_RECV_MOVE_ROBOT_XY_COK_TACKER:
		{
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE,MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE,MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);
			
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_PLACE_ROBOT_Z_COK_STACKER_PLACE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMXY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 809, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_COK_STACKER_PLACE:
		{
			double dTargetPos = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z,PLACE, MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);

			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, dTargetPos, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_PLACE_GRIPPER_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 810, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_RECV_PLACE_GRIPPER_DOWN:
		Run_Robot.ReqGripperDown( FORWARD );				//
		m_step_recv = ECCT_RECV_PLACE_GRIPPER_DOWN_CHK;
		break;

	case ECCT_RECV_PLACE_GRIPPER_DOWN_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return; 

		m_step_recv = ECCT_RECV_PLACE_GRIP_PALCE;
		break;

	case ECCT_RECV_PLACE_GRIP_PALCE:
		Run_Robot.ReqGripperOn( FORWARD );
		m_step_recv = ECCT_RECV_PLACE_GRIP_PALCE_CHK;
		break;

	case ECCT_RECV_PLACE_GRIP_PALCE_CHK:
		if( Run_Robot.GetGripperOn() != FB_NONE )
			return;

		m_step_recv = ECCT_RECV_PLACE_GRIPPER_UP;
		break;

	case ECCT_RECV_PLACE_GRIPPER_UP:
		Run_Robot.ReqGripperDown( BACKWARD );
		m_step_recv = ECCT_RECV_PLACE_GRIPPER_UP_CHK;
		break;

	case ECCT_RECV_PLACE_GRIPPER_UP_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return; 

		m_step_recv = ECCT_RECV_PLACE_ROBOT_Z_UP;
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_recv = ECCT_RECV_FINISH;
				
				if( Run_Robot.GetCokID() >= 0 )
				{
					// 데이터 이동...
					MoveDataRobot2CokStacker();
				}
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 811, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;
	
	case ECCT_RECV_FINISH:
		m_cct = CCT_NONE;
		m_nTargetCokIdx = -1;
		m_step_recv = ECCT_RECV_START;
		CheckCokTypeChange();
		break;
	}

}

void APartHandler_COK::Run_Move_COK_SEND()
{
	double dSpeedRatio[3];
	dSpeedRatio[0] = (double)st_basic.nRunSpeed;				// 1/10 속도로 복귀 한다.
	dSpeedRatio[1] = (double)st_basic.nRunSpeed;
	dSpeedRatio[2] = (double)st_basic.nRunSpeed;

	long lAxis[2];
	lAxis[0] = MOTOR_ROBOT_X;
	lAxis[1] = MOTOR_ROBOT_Y;
	
	int nSite = m_nTargetCokIdx / SITE_COK_CNT;
	int nIdx = m_nTargetCokIdx % SITE_COK_CNT;
	int nMotAxis = Run_Test_Site[nSite].GetMotID();

	if( m_step_send_old != m_step_send )
	{
		m_step_send_old = m_step_send;
		m_dwTime_Send = GetCurrentTime();
		
		if( Run_Move_COK_SEND_Chk_Jam() == false )
			return;
	}

	if( Run_Move_Cok_SEND_Chk_Time() == false )
		return;

	switch( m_step_send )
	{
	case ECCT_SEND_START:			m_step_send = ECCT_SEND_ELEV_Z_SAFETY;		break;
	case ECCT_SEND_ELEV_Z_SAFETY:	
		{
			if( COMI.Check_MotPosRange(MOTOR_COK_ROBOT_Y, GetPosCYAtCokStacker( st_basic.n_cok_type ), COMI.md_allow_value[MOTOR_COK_ROBOT_Y]) == BD_GOOD )
			{
				m_step_send = ECCT_SEND_ELEV_Y;
				break;
			}
			
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, st_handler.md_common_teaching[CT_COK_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_ELEV_Y;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 820, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_ELEV_Y:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Y, GetPosCYAtCokStacker( st_basic.n_cok_type ), st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_ELEV_Z_UNDER_FLOOR;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 821, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_ELEV_Z_UNDER_FLOOR:
		{
			double dTarget = GetPosCZByCnt(m_nCokCnt[st_basic.n_cok_type] + 1 );
			if( m_nCokCnt[st_basic.n_cok_type] >= 19 )
				dTarget = st_handler.md_common_teaching[CT_COK_Z_SAFETY];

			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_ELEV_Z_UNDER_FLOOR_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 822, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_ELEV_Z_UNDER_FLOOR_CHK:
		if( m_nCokCnt[st_basic.n_cok_type] < 20 && g_ioMgr.get_in_bit( st_io.i_cok_stacker_top_chk[st_basic.n_cok_type] ) == IO_OFF)
			return;

		m_step_send = ECCT_SEND_ELEV_Z_TOP;
		break;

	case ECCT_SEND_ELEV_Z_TOP:
		{
			double dTarget = GetPosCZByCnt(m_nCokCnt[st_basic.n_cok_type]);
			if( m_nCokCnt[st_basic.n_cok_type] > 20 )
				dTarget = st_handler.md_common_teaching[CT_COK_Z_SAFETY];

			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_ELEV_Z_TOP_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 823, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_ELEV_Z_TOP_CHK:
		if( m_nCokCnt[st_basic.n_cok_type] > 0 && g_ioMgr.get_in_bit( st_io.i_cok_stacker_top_chk[st_basic.n_cok_type] ) == IO_ON )
			return;

		m_step_send = ECCT_SEND_BACK_PLATE;
		break;

	case ECCT_SEND_BACK_PLATE:
		Run_Test_Site[nSite].ReqPlate( BACKWARD );
		m_step_send = ECCT_SEND_BACK_PLATE_CHK;
		break;

	case ECCT_SEND_BACK_PLATE_CHK:
		if( Run_Test_Site[nSite].GetPlate() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_INSERT_FORWARD;
		break;
		
	case ECCT_SEND_INSERT_FORWARD:
		{
			for( int i=0; i<INSERT_CNT; i++ )
			{
				Run_Test_Site[nSite].ReqInsert( i, FORWARD );
			}
			m_step_send = ECCT_SEND_INSERT_FORWARD_CHK;			
		}
		break;
		
	case ECCT_SEND_INSERT_FORWARD_CHK:
		{
			for( int i=0; i<INSERT_CNT; i++ )
			{
				if( Run_Test_Site[nSite].GetInsert( i ) != FB_NONE )
					return;
			}
			m_step_send = ECCT_SEND_UNLOCK;
		}
		break;

	case ECCT_SEND_UNLOCK:
		Run_Test_Site[nSite].ReqLock( BACKWARD );
		m_step_send = ECCT_SEND_UNLOCK_CHK;
		break;

	case ECCT_SEND_UNLOCK_CHK:
		if( Run_Test_Site[nSite].GetLock() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_ROBOT_Z_SAFTY;
		break;

	case ECCT_SEND_ROBOT_Z_SAFTY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_MOVESITE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 824, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_MOVESITE:
		{
			int nRet = CTL_Lib.Single_Move( nMotAxis, st_handler.md_common_teaching[CT_TSITE_1_Z + nSite], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PICK_GV_UP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMS%d10", nSite+1 );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 825, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_PICK_GV_UP:
		{
			Run_Robot.ReqGripperDown( BACKWARD );
			for( int i=0; i<VACUUM_CNT; i++ )
			{
				Run_Robot.ReqVacDown(i, BACKWARD);
			}
			m_step_send = ECCT_SEND_PICK_GV_UP_CHK;
		}
		break;
	
	case ECCT_SEND_PICK_GV_UP_CHK:
		{
			if( Run_Robot.GetGripperDown() != FB_NONE )	return;
			for( int i=0; i<VACUUM_CNT; i++ )
			{
				if( Run_Robot.GetVacDown(i) != FB_NONE )
					return;
			}
			m_step_send = ECCT_SEND_MOVE_ROBOT_XY_COK_STACKER;
		}
		break;
	
	case ECCT_SEND_MOVE_ROBOT_XY_COK_STACKER:
		{
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);
			
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PICK_GRIP_UNPICK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMXY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 826, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_PICK_GRIP_UNPICK:
		Run_Robot.ReqGripperOn( FORWARD );
		m_step_send = ECCT_SEND_PICK_GRIP_UNPICK_CHK;
		break;

	case ECCT_SEND_PICK_GRIP_UNPICK_CHK:
		if( Run_Robot.GetGripperOn() != FB_NONE )
			return;
		m_step_send = ECCT_SEND_PICK_ROBOT_Z_DOWN;
		break;

	case ECCT_SEND_PICK_ROBOT_Z_DOWN:
		{
			double dTargetPos = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z,PICK, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);

			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, dTargetPos, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PICK_GRIPPER_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 827, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_PICK_GRIPPER_DOWN:
		Run_Robot.ReqGripperDown( FORWARD );				//
		m_step_send = ECCT_SEND_PICK_GRIPPER_DOWN_CHK;
		break;
		
	case ECCT_SEND_PICK_GRIPPER_DOWN_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PICK_GRIP_PICK;
		break;

	case ECCT_SEND_PICK_GRIP_PICK:
		Run_Robot.ReqGripperOn( BACKWARD );
		m_step_send = ECCT_SEND_PICK_GRIP_PICK_CHK;
		break;

	case ECCT_SEND_PICK_GRIP_PICK_CHK:
		if( Run_Robot.GetGripperOn() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PICK_GRIPPER_UP;
		break;

	case ECCT_SEND_PICK_GRIPPER_UP:
		Run_Robot.ReqGripperDown( BACKWARD );
		m_step_send = ECCT_SEND_PICK_GRIPPER_UP_CHK;
		break;

	case ECCT_SEND_PICK_GRIPPER_UP_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PICK_ROBOT_Z_UP;
		break;

	case ECCT_SEND_PICK_ROBOT_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PLACE_ROBOT_XY_SITE;
				
				// 데이터 이동..
				MoveDataCokStacker2Robot();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 828, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_PLACE_ROBOT_XY_SITE:
		{
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, 20 + m_nTargetCokIdx, -1);
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, 20 + m_nTargetCokIdx, -1);

			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PLACE_ROBOT_Z_SITE_PLACE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMXY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 829, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_SITE_PLACE:
		{
			double dTargetPos = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z, PLACE, 20 + m_nTargetCokIdx, -1);

			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, dTargetPos, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_PLACE_GRIPPER_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 830, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_SEND_PLACE_GRIPPER_DOWN:
		Run_Robot.ReqGripperDown( FORWARD );
		m_step_send = ECCT_SEND_PLACE_GRIPPER_DOWN_CHK;
		break;

	case ECCT_SEND_PLACE_GRIPPER_DOWN_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PLACE_GRIP_PALCE;
		break;

	case ECCT_SEND_PLACE_GRIP_PALCE:
		Run_Robot.ReqGripperOn( FORWARD );
		m_step_send = ECCT_SEND_PLACE_GRIP_PALCE_CHK;
		break;

	case ECCT_SEND_PLACE_GRIP_PALCE_CHK:
		if( Run_Robot.GetGripperOn() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PLACE_GRIPPER_UP;
		break;

	case ECCT_SEND_PLACE_GRIPPER_UP:
		Run_Robot.ReqGripperDown( BACKWARD );
		m_step_send = ECCT_SEND_PLACE_GRIPPER_UP_CHK;
		break;

	case ECCT_SEND_PLACE_GRIPPER_UP_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )
			return;

		m_step_send = ECCT_SEND_PLACE_ROBOT_Z_UP;
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_send = ECCT_SEND_FINISH;
				MoveDataRobot2Site();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 831, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_SEND_FINISH:
		m_cct = CCT_NONE;
		m_nTargetCokIdx = -1;
		m_step_send = ECCT_SEND_START;
		CheckCokTypeChange();
		break;
	}
}

void APartHandler_COK::LoadRegDatas()
{
	for( int i=0; i<COK_TYPE_CNT; i++ )
	{
		CString strVal;
		strVal.Format( "%s_%d", REG_VAL_ITEM_CNT, i );
		m_nCokCnt[i] = AMTRegistry::RegReadInt( REG_KEY_COK, (LPCSTR)strVal, 20 );
	}

	for( int c=0; c<TSITE_COK_CNT; c++ )
	{
		CString strVal;
		strVal.Format( "%s_%d", REG_VAL_SITE_COK, c );
		m_nSite[c] = AMTRegistry::RegReadInt( REG_KEY_COK, (LPCSTR)strVal, -1 );
	}

	//2014,0310
	CString strHifix;
	strHifix.Format( "%s", REG_VAL_HIFIX_POS );
	m_nHifixpos = AMTRegistry::RegReadInt( REG_KEY_COK, (LPCSTR)strHifix, -1 );

}

int APartHandler_COK::GetSiteCok( int iIdx )
{
	return m_nSite[iIdx];
}

void APartHandler_COK::SetSiteCok( int iIdx, int iVal )
{
	if( m_nSite[iIdx] == iVal )
		return;

	CString strVal;
	strVal.Format( "%s_%d", REG_VAL_SITE_COK, iIdx );
	AMTRegistry::RegWriteInt( REG_KEY_COK, (LPCSTR)strVal, iVal );

	m_nSite[iIdx] = iVal;
}

int APartHandler_COK::GetHifixPos()
{
	return m_nHifixpos;
}

void APartHandler_COK::SetHifixPos( int iVal )
{
	if( m_nHifixpos == iVal )
		return;

	CString strVal;
	strVal.Format( "%s", REG_VAL_HIFIX_POS );
	AMTRegistry::RegWriteInt( REG_KEY_COK, (LPCSTR)strVal, iVal );

	m_nHifixpos = iVal;
}

void APartHandler_COK::CheckCokTypeChange()
{
	if( m_cct != CCT_NONE )
		return;

	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( m_nSite[i] > -1 && m_nSite[i] != st_basic.n_cok_type )
		{
			if(st_handler.mn_cok_change == CTL_NO)
			{
				m_cct = CCT_COK_CHANGE;
				return;
			}
			// Stacker에 놔야한다.
			m_cct = CCT_RECV;
			m_nTargetCokIdx = i;
			m_step_recv = ECCT_RECV_START;
			return;
		}
	}

	for( i=0; i<TSITE_COK_CNT; i++ )
	{
		if( m_nSite[i] == -1 )
		{
			if(st_handler.mn_cok_change == CTL_NO)
			{
				m_cct = CCT_COK_CHANGE;
				return;
			}
			// TEST SITE에 놔야한다. 
			m_cct = CCT_SEND;
			m_nTargetCokIdx = i;
			m_step_send = ECCT_SEND_START;
			return;
		}
	}

	// 작업 준비한다.
	m_cct = CCT_WORK_READY;
	m_step_ready = ECCT_READY_START;
}

void APartHandler_COK::MoveDataSite2Robot()
{
	// REGSTRY Save
	// Draw
	Run_Robot.SetCokID( m_nSite[m_nTargetCokIdx] );
	SetSiteCok( m_nTargetCokIdx, -1 );

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDC_SITE, m_nTargetCokIdx );
}

void APartHandler_COK::MoveDataRobot2CokStacker()
{
	int iCokID = Run_Robot.GetCokID();
	AddCokCnt( Run_Robot.GetCokID(), 1 );
	Run_Robot.SetCokID( -1 );

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, iCokID );
}

void APartHandler_COK::MoveDataCokStacker2Robot()
{
	Run_Robot.SetCokID( st_basic.n_cok_type );
	AddCokCnt( st_basic.n_cok_type, -1 );

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDC_COKSTACKER, st_basic.n_cok_type );
}

void APartHandler_COK::MoveDataRobot2Site()
{
	Run_Robot.SetCokID( -1 );
	SetSiteCok( m_nTargetCokIdx, st_basic.n_cok_type);

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDC_SITE, m_nTargetCokIdx );
}

void APartHandler_COK::Run_Move_Work_Ready()
{
	switch( m_step_ready )
	{
	case ECCT_READY_START:		m_step_ready = ECCT_READY_COK_CHK;			break;
	case ECCT_READY_COK_CHK:	m_step_ready = ECCT_READY_Z;				break;

	case ECCT_READY_Z:
		Run_Test_Site[0].ReqMoveToSocket();
		Run_Test_Site[1].ReqMoveToSocket();
		m_step_ready = ECCT_READY_Z_CHK;
		break;

	case ECCT_READY_Z_CHK:
		if( Run_Test_Site[0].IsMovingToSocket() )	return;
		if( Run_Test_Site[1].IsMovingToSocket() )	return;

		m_step_ready = ECCT_READY_INSERT_BACK;
			break;

	case ECCT_READY_INSERT_BACK:
		{
			for( int i=0; i<2; i++ )
			{
				if( Run_Test_Site[i].IsPlateForward() == false )
				{
					Run_Test_Site[i].ReqInsert(0, BACKWARD );
					Run_Test_Site[i].ReqInsert(1, BACKWARD );
					Run_Test_Site[i].ReqInsert(2, BACKWARD );
					Run_Test_Site[i].ReqInsert(3, BACKWARD );
					Run_Test_Site[i].ReqInsert(4, BACKWARD );
				}
			}

			m_step_ready = ECCT_READY_INSERT_BACK_CHK;
		}
		break;

	case ECCT_READY_INSERT_BACK_CHK:
		{
			for( int i=0; i<2; i++ )
			{
				if(Run_Test_Site[i].GetInsert(0) != FB_NONE )	return;
				if(Run_Test_Site[i].GetInsert(1) != FB_NONE )	return;
				if(Run_Test_Site[i].GetInsert(2) != FB_NONE )	return;
				if(Run_Test_Site[i].GetInsert(3) != FB_NONE )	return;
				if(Run_Test_Site[i].GetInsert(4) != FB_NONE )	return;
			}

			m_step_ready = ECCT_READY_LOCK;
		}
		break;

	case ECCT_READY_LOCK:
		Run_Test_Site[0].ReqLock(FORWARD);
		Run_Test_Site[1].ReqLock(FORWARD);
		m_step_ready = ECCT_READY_LOCK_CHK;
		break;

	case ECCT_READY_LOCK_CHK:
		if( Run_Test_Site[0].GetLock() != FB_NONE )	return;
		if( Run_Test_Site[1].GetLock() != FB_NONE )	return;
		m_step_ready = ECCT_READY_PLATE_FORWARD;
		break;
	
	case ECCT_READY_PLATE_FORWARD:
		Run_Test_Site[0].ReqPlate(FORWARD);
		Run_Test_Site[1].ReqPlate(FORWARD);
		m_step_ready = ECCT_READY_PLATE_FORWARD_CHK;
		break;

	case ECCT_READY_PLATE_FORWARD_CHK:
		if( Run_Test_Site[0].GetPlate() != FB_NONE )	return;
		if( Run_Test_Site[1].GetPlate() != FB_NONE )	return;
		m_step_ready = ECCT_READY_REJ_CHK;
		break;

	case ECCT_READY_REJ_CHK:
		{
			if( Run_Reject_Tray_Transfer.GetMoveToBin() == false ||
				Run_Reject_Tray_Transfer.GetMoveToRej() == false )
			{
				m_step_ready = ECCT_READY_FINISH;
				break;
			}

			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 ) iRejCnt = 1;
			if( dPos_work < 1.0f )	dPos_work = dPos_1;

			double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
			{
				m_step_ready = ECCT_READY_REJ_GUIDE_BACK;
				break;
			}

			m_step_ready = ECCT_READY_REJ_GUIDE_FORWARD;
		}
		break;

	case ECCT_READY_REJ_GUIDE_BACK:
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		m_step_ready = ECCT_READY_REJ_GUIDE_BACK_CHK;
		break;

	case ECCT_READY_REJ_GUIDE_BACK_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;

		m_step_ready = ECCT_READY_REJ_MOVE;
		break;

	case ECCT_READY_REJ_MOVE:
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
				m_step_ready = ECCT_READY_REJ_GUIDE_FORWARD;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMRS00" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 840, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case ECCT_READY_REJ_GUIDE_FORWARD:
		Run_Reject_Stacker.ReqGuide( FORWARD );
		m_step_ready = ECCT_READY_REJ_GUIDE_FORWARD_CHK;
		break;

	case ECCT_READY_REJ_GUIDE_FORWARD_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;

		m_step_ready = ECCT_READY_TR_CLAMP_SAFETY;
		break;

	case ECCT_READY_TR_CLAMP_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_TR_CLAMP, st_motor[MOTOR_TRAY_TR_CLAMP].d_pos[P_CLAMP_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_ready = ECCT_READY_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMTC02" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 841, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case ECCT_READY_FINISH:
		{
			m_cct = CCT_NONE;
			st_handler.mn_cok_change = CTL_NO;
			m_step_ready = ECCT_READY_START;
		}
		break;
	}
}

void APartHandler_COK::SetCokCnt( int iStacker, int nCnt )
{
	if( m_nCokCnt[iStacker] == nCnt )
		return;

	m_nCokCnt[iStacker] = nCnt;
	if( m_nCokCnt[iStacker] < 0 )
		m_nCokCnt[iStacker] = 0;

	if( m_nCokCnt[iStacker] > COK_ITEM_CNT )
		m_nCokCnt[iStacker] = COK_ITEM_CNT;

	CString strVal;
	strVal.Format( "%s_%d", REG_VAL_ITEM_CNT, iStacker );
	AMTRegistry::RegWriteInt( REG_KEY_COK, (LPCSTR)strVal, m_nCokCnt[iStacker] );
}

bool APartHandler_COK::Run_Move_COK_SAFETY_Chk_Jam()
{
	switch( m_step_safety)
	{
	case ECCT_SAFETY_START:
	case ECCT_ROBOT_Z_SAFETY:
	case ECCT_ROBOT_XY_SAFETY:
	case ECCT_ROBOT_XY_SAFETY_CHK:
	case ECCT_ROBOT_TRANSFER_MOVE_SAFETY:
	case ECCT_ROBOT_TRANSFER_MOVE_SAFETY_CHK:
		break;
	}
	return true;
}

void APartHandler_COK::AddCokCnt( int iStacker, int iAdd )
{
	m_nCokCnt[iStacker] += iAdd;
	if( m_nCokCnt[iStacker] < 0 )
		m_nCokCnt[iStacker] = 0;

	if( m_nCokCnt[iStacker] > COK_ITEM_CNT )
		m_nCokCnt[iStacker] = COK_ITEM_CNT;

	CString strVal;
	strVal.Format( "%s_%d", REG_VAL_ITEM_CNT, iStacker );
	AMTRegistry::RegWriteInt( REG_KEY_COK, (LPCSTR)strVal, m_nCokCnt[iStacker] );
}

bool APartHandler_COK::Run_Move_COK_RECV_Chk_Jam()
{
	int nSite = m_nTargetCokIdx / SITE_COK_CNT;

	switch( m_step_recv )
	{
	case ECCT_RECV_START:
	case ECCT_RECV_ELEV_MOVE_Z_SAFETY:
		break;

	case ECCT_RECV_ELEV_MOVE_Y:
		if( Chk_COK_Robot_Z_Safety == false )							return false;
		break;

	case ECCT_RECV_ELEV_MOVE_Z_TOP:
	case ECCT_RECV_ELEV_Z_TOP_CHK:
	case ECCT_RECV_ELEV_DOWN_FLOOR:
	case ECCT_RECV_ELEV_DOWN_CHK:
		if( Chk_COK_Robot_Y(m_nSite[m_nTargetCokIdx]) == false )								return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	
	case ECCT_RECV_BACK_PLATE:
	case ECCT_RECV_BACK_PLATE_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_RECV_INSERT_FORWARD:
	case ECCT_RECV_INSERT_FORWARD_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		break;

	case ECCT_RECV_UNLOCK:
	case ECCT_RECV_UNLOCK_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 0, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 1, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 2, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 3, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 4, IO_ON ) == false )		return false;
		break;


	case ECCT_RECV_ROBOT_Z_SAFTY:
		break;

	case ECCT_RECV_MOVESITE:
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_RECV_PICK_GV_UP:
	case ECCT_RECV_PICK_GV_UP_CHK:
		break;

	case ECCT_RECV_PICK_ROBOT_XY_SITE:
	case ECCT_RECV_PICK_GRIP_UNPICK:
	case ECCT_RECV_PICK_GRIP_UNPICK_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_RECV_PICK_ROBOT_Z_DOWN:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Pick() == false )								return false;
		break;

	case ECCT_RECV_PICK_GRIP_DOWN:
	case ECCT_RECV_PICK_GRIP_DOWN_CHK:
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Pick() == false )								return false;
		if( Chk_Z_Recv_Pick() == false )								return false;
		break;

	case ECCT_RECV_PICK_GRIP_PICK:
	case ECCT_RECV_PICK_GRIP_PICK_CHK:
		if( Run_Robot.Chk_Robot_GripperDn( IO_ON ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Pick() == false )								return false;
		if( Chk_Z_Recv_Pick() == false )								return false;
		break;

	case ECCT_RECV_PICK_GRIP_UP:
	case ECCT_RECV_PICK_GRIP_UP_CHK:
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Pick() == false )								return false;
		if( Chk_Z_Recv_Pick() == false )								return false;
		break;

	case ECCT_RECV_PICK_ROBOT_Z_UP:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Pick() == false )								return false;
		break;

	case ECCT_RECV_MOVE_ROBOT_XY_COK_TACKER:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_COK_STACKER_PLACE:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Place() == false )								return false;
		break;

	case ECCT_RECV_PLACE_GRIPPER_DOWN:
	case ECCT_RECV_PLACE_GRIPPER_DOWN_CHK:
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Place() == false )								return false;
		if( Chk_Z_Recv_Place() == false )								return false;
		break;

	case ECCT_RECV_PLACE_GRIP_PALCE:
	case ECCT_RECV_PLACE_GRIP_PALCE_CHK:
		if( Run_Robot.Chk_Robot_GripperDn( IO_ON ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Place() == false )								return false;
		if( Chk_Z_Recv_Place() == false )								return false;
		break;

	case ECCT_RECV_PLACE_GRIPPER_UP:
	case ECCT_RECV_PLACE_GRIPPER_UP_CHK:
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Place() == false )								return false;
		if( Chk_Z_Recv_Place() == false )								return false;
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_UP:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Recv_Place() == false )								return false;
		break;
	
	case ECCT_RECV_FINISH:
		break;
	}

	return true;
}

bool APartHandler_COK::Run_Move_COK_SEND_Chk_Jam()
{
	int nSite = m_nTargetCokIdx / SITE_COK_CNT;

	switch( m_step_send )
	{
	case ECCT_SEND_START:
	case ECCT_SEND_ELEV_Z_SAFETY:	
		break;

	case ECCT_SEND_ELEV_Y:
		if( Chk_COK_Robot_Z_Safety == false )							return false;
		break;

	case ECCT_SEND_ELEV_Z_UNDER_FLOOR:
	case ECCT_SEND_ELEV_Z_UNDER_FLOOR_CHK:
	case ECCT_SEND_ELEV_Z_TOP:
	case ECCT_SEND_ELEV_Z_TOP_CHK:
		if( Chk_COK_Robot_Y(st_basic.n_cok_type) == false )				return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_SEND_BACK_PLATE:
	case ECCT_SEND_BACK_PLATE_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_SEND_INSERT_FORWARD:
	case ECCT_SEND_INSERT_FORWARD_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		break;

	case ECCT_SEND_UNLOCK:
	case ECCT_SEND_UNLOCK_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 0, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 1, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 2, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 3, IO_ON ) == false )		return false;
		if( Run_Test_Site[nSite].Chk_Insert( nSite, 4, IO_ON ) == false )		return false;
		break;

	case ECCT_SEND_ROBOT_Z_SAFTY:
		break;

	case ECCT_SEND_MOVESITE:
		if( Run_Test_Site[nSite].Chk_Plate( nSite, IO_OFF ) == false )	return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_SEND_PICK_GV_UP:
	case ECCT_SEND_PICK_GV_UP_CHK:
		break;
	
	case ECCT_SEND_MOVE_ROBOT_XY_COK_STACKER:
	case ECCT_SEND_PICK_GRIP_UNPICK:
	case ECCT_SEND_PICK_GRIP_UNPICK_CHK:
		if( Chk_Robot_Z_Safety() == false )								return false;
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		break;

	case ECCT_SEND_PICK_ROBOT_Z_DOWN:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Pick() == false )								return false;
		break;

	case ECCT_SEND_PICK_GRIPPER_DOWN:
	case ECCT_SEND_PICK_GRIPPER_DOWN_CHK:
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Pick() == false )								return false;
		if( Chk_Z_Send_Pick() == false )								return false;
		break;

	case ECCT_SEND_PICK_GRIP_PICK:
	case ECCT_SEND_PICK_GRIP_PICK_CHK:
		if( Run_Robot.Chk_Robot_GripperDn( IO_ON ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Pick() == false )								return false;
		if( Chk_Z_Send_Pick() == false )								return false;
		break;

	case ECCT_SEND_PICK_GRIPPER_UP:
	case ECCT_SEND_PICK_GRIPPER_UP_CHK:
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Pick() == false )								return false;
		if( Chk_Z_Send_Pick() == false )								return false;
		break;

	case ECCT_SEND_PICK_ROBOT_Z_UP:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Pick() == false )								return false;
		break;

	case ECCT_SEND_PLACE_ROBOT_XY_SITE:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_Robot_Z_Safety() == false )								return false;
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_SITE_PLACE:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Place() == false )								return false;
		break;

	case ECCT_SEND_PLACE_GRIPPER_DOWN:
	case ECCT_SEND_PLACE_GRIPPER_DOWN_CHK:
		if( Run_Robot.Chk_Robot_GripperPick( IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Place() == false )								return false;
		if( Chk_Z_Send_Place() == false )								return false;
		break;

	case ECCT_SEND_PLACE_GRIP_PALCE:
	case ECCT_SEND_PLACE_GRIP_PALCE_CHK:
		if( Run_Robot.Chk_Robot_GripperDn( IO_ON ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Place() == false )								return false;
		if( Chk_Z_Send_Place() == false )								return false;
		break;

	case ECCT_SEND_PLACE_GRIPPER_UP:
	case ECCT_SEND_PLACE_GRIPPER_UP_CHK:
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Place() == false )								return false;
		if( Chk_Z_Send_Place() == false )								return false;
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_UP:
		if( Run_Robot.Chk_Robot_GripperDn( IO_OFF ) == false )			return false;
		if( Run_Robot.Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Run_Robot.Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_XY_Send_Place() == false )								return false;
		break;

	case ECCT_SEND_FINISH:
		break;
	}

	return true;
}

bool APartHandler_COK::Chk_COK_Robot_Z_Safety()
{
	if( g_comiMgr.Get_MotCurrentPos( MOTOR_COK_ROBOT_Z ) > st_handler.md_common_teaching[CT_COK_Z_SAFETY] + COMI.md_allow_value[MOTOR_COK_ROBOT_Z] )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "SMCZ00" );
		CTL_Lib.Alarm_Error_Occurrence( 850, dWARNING, "471000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_Robot_Z_Safety()
{
	if( g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) > st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "SM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 851, dWARNING, "470000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_COK_Robot_Y( int iIdx )
{
	if( COMI.Check_MotPosRange( MOTOR_COK_ROBOT_Y, GetPosCYAtCokStacker( iIdx ), COMI.md_allow_value[MOTOR_COK_ROBOT_Y] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMCY00" );
		CTL_Lib.Alarm_Error_Occurrence( 852, dWARNING, "460200" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_XY_Recv_Pick()
{
	double dx = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PICK, 20 + m_nTargetCokIdx, -1 );
	double dy = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PICK, 20 + m_nTargetCokIdx, -1 );

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_X, dx, COMI.md_allow_value[MOTOR_ROBOT_X] ) != BD_GOOD ||
		COMI.Check_MotPosRange( MOTOR_ROBOT_Y, dy, COMI.md_allow_value[MOTOR_ROBOT_Y] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMXY00" );
		CTL_Lib.Alarm_Error_Occurrence( 853, dWARNING, "464000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_Z_Recv_Pick()
{
	double dz = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, PICK, m_nTargetCokIdx + 20, -1 );

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_Z, dz, COMI.md_allow_value[MOTOR_ROBOT_Z] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 854, dWARNING, "460100" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_XY_Recv_Place()
{
	double dx = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE,MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);
	double dy = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE,MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_X, dx, COMI.md_allow_value[MOTOR_ROBOT_X] ) != BD_GOOD ||
		COMI.Check_MotPosRange( MOTOR_ROBOT_Y, dy, COMI.md_allow_value[MOTOR_ROBOT_Y] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMXY00" );
		CTL_Lib.Alarm_Error_Occurrence( 855, dWARNING, "464000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_Z_Recv_Place()
{
	double dz = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z,PLACE, MPOS_INDEX_COK_STACKER + Run_Robot.GetCokID(), -1);

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_Z, dz, COMI.md_allow_value[MOTOR_ROBOT_Z] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 856, dWARNING, "460100" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_XY_Send_Pick()
{
	double dx = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);
	double dy = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_X, dx, COMI.md_allow_value[MOTOR_ROBOT_X] ) != BD_GOOD ||
		COMI.Check_MotPosRange( MOTOR_ROBOT_Y, dy, COMI.md_allow_value[MOTOR_ROBOT_Y] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMXY00" );
		CTL_Lib.Alarm_Error_Occurrence( 857, dWARNING, "464000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_Z_Send_Pick()
{
	double dz = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z,PICK, MPOS_INDEX_COK_STACKER + st_basic.n_cok_type, -1);
	if( COMI.Check_MotPosRange( MOTOR_ROBOT_Z, dz, COMI.md_allow_value[MOTOR_ROBOT_Z] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 858, dWARNING, "460100" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_XY_Send_Place()
{
	double dx = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, 20 + m_nTargetCokIdx, -1);
	double dy = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, 20 + m_nTargetCokIdx, -1);

	if( COMI.Check_MotPosRange( MOTOR_ROBOT_X, dx, COMI.md_allow_value[MOTOR_ROBOT_X] ) != BD_GOOD ||
		COMI.Check_MotPosRange( MOTOR_ROBOT_Y, dy, COMI.md_allow_value[MOTOR_ROBOT_Y] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMXY00" );
		CTL_Lib.Alarm_Error_Occurrence( 859, dWARNING, "464000" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Chk_Z_Send_Place()
{
	double dz = g_handler.CalcTargetPosition(MOTOR_ROBOT_Z, PLACE, 20 + m_nTargetCokIdx, -1);
	if( COMI.Check_MotPosRange( MOTOR_ROBOT_Z, dz, COMI.md_allow_value[MOTOR_ROBOT_Z] ) != BD_GOOD )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 860, dWARNING, "460100" );
		return false;
	}

	return true;
}

bool APartHandler_COK::Run_Move_Cok_RECV_Chk_Time()
{
	switch( m_step_recv )
	{
	case ECCT_RECV_ELEV_MOVE_Z_SAFETY:
		if( InTime_Recv( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCZ00" );
			CTL_Lib.Alarm_Error_Occurrence( 861, dWARNING, "620300" );
			return false;
		}
		break;
	case ECCT_RECV_ELEV_MOVE_Y:
		if( InTime_Recv( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCY00" );
			CTL_Lib.Alarm_Error_Occurrence( 862, dWARNING, "620200" );
			return false;
		}
		break;

	case ECCT_RECV_ELEV_MOVE_Z_TOP:
		if( InTime_Recv( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCZ00" );
			CTL_Lib.Alarm_Error_Occurrence( 863, dWARNING, "620300" );
			return false;
		}
		break;

	case ECCT_RECV_ELEV_Z_TOP_CHK:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 864, dWARNING, "622001" );
			return false;
		}
		break;

	case ECCT_RECV_ELEV_DOWN_FLOOR:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 865, dWARNING, "622000" );
			return false;
		}
		break;

	case ECCT_RECV_ELEV_DOWN_CHK:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CCOK00" );
			CTL_Lib.Alarm_Error_Occurrence( 866, dWARNING, "320000" );
			return false;
		}
		break;

	case ECCT_RECV_ROBOT_Z_SAFTY:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 867, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_RECV_MOVESITE:
		if( InTime_Recv( 80000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 868, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_RECV_PICK_ROBOT_XY_SITE:
		if( InTime_Recv( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 869, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_RECV_PICK_ROBOT_Z_DOWN:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 870, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_RECV_PICK_ROBOT_Z_UP:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00MRZMT0" );
			CTL_Lib.Alarm_Error_Occurrence( 871, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_RECV_MOVE_ROBOT_XY_COK_TACKER:
		if( InTime_Recv( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 872, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_COK_STACKER_PLACE:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 873, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_RECV_PLACE_ROBOT_Z_UP:
		if( InTime_Recv( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 874, dWARNING, "620100" );
			return false;
		}
		break;
	}

	return true;
}

bool APartHandler_COK::Run_Move_Cok_SEND_Chk_Time()
{
	switch( m_step_send )
	{
	case ECCT_SEND_ELEV_Z_SAFETY:	
		if( InTime_Send( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCZ00" );
			CTL_Lib.Alarm_Error_Occurrence( 880, dWARNING, "620300" );
			return false;
		}
		break;

	case ECCT_SEND_ELEV_Y:
		if( InTime_Send( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCY00" );
			CTL_Lib.Alarm_Error_Occurrence( 881, dWARNING, "620200" );
			return false;
		}
		break;

	case ECCT_SEND_ELEV_Z_UNDER_FLOOR:
		if( InTime_Send( 60000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCZ00" );
			CTL_Lib.Alarm_Error_Occurrence( 882, dWARNING, "320000" );
			return false;
		}
		break;

	case ECCT_SEND_ELEV_Z_UNDER_FLOOR_CHK:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CCOK00" );
			CTL_Lib.Alarm_Error_Occurrence( 883, dWARNING, "320000" );
			return false;
		}
		break;

	case ECCT_SEND_ELEV_Z_TOP:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMCZ00" );
			CTL_Lib.Alarm_Error_Occurrence( 884, dWARNING, "620300" );
			return false;
		}
		break;

	case ECCT_SEND_ELEV_Z_TOP_CHK:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CCOK01" );
			CTL_Lib.Alarm_Error_Occurrence( 885, dWARNING, "320001" );
			return false;
		}
		break;

	case ECCT_SEND_ROBOT_Z_SAFTY:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 886, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_SEND_MOVESITE:
		if( InTime_Send( 80000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 887, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_SEND_MOVE_ROBOT_XY_COK_STACKER:
		if( InTime_Send( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 888, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_SEND_PICK_ROBOT_Z_DOWN:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 889, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_SEND_PICK_ROBOT_Z_UP:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 890, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_SEND_PLACE_ROBOT_XY_SITE:
		if( InTime_Send( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 891, dWARNING, "625000" );
			return false;
		}
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_SITE_PLACE:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 892, dWARNING, "620100" );
			return false;
		}
		break;

	case ECCT_SEND_PLACE_ROBOT_Z_UP:
		if( InTime_Send( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z000" );
			CTL_Lib.Alarm_Error_Occurrence( 893, dWARNING, "620100" );
			return false;
		}
		break;
	}

	return true;
}

bool APartHandler_COK::Run_Move_Cok_SAFETY_Chk_Time()
{
	switch(m_step_safety)
	{
	case ECCT_SAFETY_START:
// 		if( InTime_Safety( 3000 ) == false )
// 		{//630300 1 A "Basica메뉴에서 HotFix 세팅값이 0이거나 6보다 큽니다."
// 			CTL_Lib.Alarm_Error_Occurrence( 897, dWARNING, "630300" );
// 			return false;
// 		}
		break;
	case ECCT_ROBOT_Z_SAFETY:
		if( InTime_Safety( 60000 ) == false )
		{
			CTL_Lib.Alarm_Error_Occurrence( 894, dWARNING, "620100" );
			return false;
		}
		break;
	case ECCT_ROBOT_TRANSFER_MOVE_SAFETY_CHK:
		if( InTime_Safety( 60000 ) == false )
		{
			CTL_Lib.Alarm_Error_Occurrence( 895, dWARNING, "624000" );
			return false;
		}
		break;
	}
	return true;
}

bool APartHandler_COK::InTime_Safety( int iMilliSec )
{
	//2012,1220
	if( GetCurrentTime() - m_dwTime_Safety < 0 )
	{
		m_dwTime_Safety = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_Safety < iMilliSec )
		return true;
	
	return false;
}
bool APartHandler_COK::InTime_Recv( int iMilliSec )
{
	//2012,1220
	if( GetCurrentTime() - m_dwTime_Recv < 0 )
	{
		m_dwTime_Recv = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_Recv < iMilliSec )
		return true;

	return false;
}

bool APartHandler_COK::InTime_Send( int iMilliSec )
{
	//2012,1220
	if( GetCurrentTime() - m_dwTime_Send < 0 )
	{
		m_dwTime_Send = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_Send < iMilliSec )
		return true;

	return false;
}

void APartHandler_COK::ClearOldStep()
{
	m_step_recv_old = ECCT_RECV_START;
	m_step_send_old = ECCT_SEND_START;
}



