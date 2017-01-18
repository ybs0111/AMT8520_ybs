// Run_Robot.cpp: implementation of the CRun_Robot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Robot.h"

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

#include "AMTRegistry.h"
#include "SrcPart/APartRobot.h"
#include "SrcPart/APartHandler.h"
#include "SrcPart/APartTestSite.h"
#include "Run_Motors.h"
#include "AClient_Xgem.h"

#include "SrcPart/APartTray.h"//2015.0914

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CRun_Robot, CObject, 1);	// 복구 동작을 위함...

CRun_Robot	Run_Robot;

CRun_Robot::CRun_Robot()
{
	Thread_Variable_Initial();
}

CRun_Robot::~CRun_Robot()
{

}

void CRun_Robot::Thread_Variable_Initial()
{
	mn_InitStep = 0;
	mn_RunStep = 0;
	mn_safetyzone_step = 0;

	m_iCokId = AMTRegistry::RegReadInt( REG_KEY_COK, REG_VAL_ROBOT_COK, -1 );

	m_bInitSafety = false;
	m_bInitXHome = false;

	picker_step_old = PSTEP_NONE;

	st_work.mn_glipperr_updn = IO_OFF;
	for( int i = 0; i < 4; i++)
	{
		st_work.mn_picker_updn[i] = IO_OFF;
		st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
	}

	st_handler.mn_robot_motorX_safety = CTL_NO;

	mn_home_x = 0;
	md_MotFlag[0] = CTL_NO;
	md_MotFlag[1] = CTL_NO;
}

void CRun_Robot::Thread_Run()
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
		if (st_handler.mn_menu_num != 501)
		{
			
		}
		
		CTL_Lib.mn_single_motmove_step[MOTOR_ROBOT_X] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		CTL_Lib.mn_single_motmove_step[MOTOR_ROBOT_Y] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		CTL_Lib.mn_single_motmove_step[MOTOR_ROBOT_Z] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
// 		CTL_Lib.mn_single_motmove_step[MOTOR_GRIPPER_P] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.

		CTL_Lib.mn_linear_motmove_step[M_ROBOT_XY_INDEX] = 0;
		//2016.0202
		CTL_Lib.mn_single_motmove_step[MOTOR_PITCH_Z] = 0;

		picker_step_old = PSTEP_NONE;
		break;
	}
	Run_Move_Grip_Down();
	Run_Move_Grip_On();
	for( int iVac=0; iVac<VACUUM_CNT; iVac++ )
	{
		Run_Move_Vac_Down( iVac );
	}
}

void CRun_Robot::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[INIT_ROBOT] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case ERI_START:					mn_InitStep = ERI_MODULE_EMPTY_CHK;		break;
	case ERI_MODULE_EMPTY_CHK:		ERI_ModuleEmptyChk();					break;
	case ERI_VACUUM_OFF:			ERI_Vacuum_Off();						break;
	case ERI_EJECTOR_ON:
		g_ioMgr.set_out_bit( st_io.o_picker1_eject, IO_ON );
		g_ioMgr.set_out_bit( st_io.o_picker2_eject, IO_ON );
		g_ioMgr.set_out_bit( st_io.o_picker3_eject, IO_ON );
		g_ioMgr.set_out_bit( st_io.o_picker4_eject, IO_ON );
		mn_InitStep = ERI_EJECTOR_OFF;
		m_dwTimeInit = GetCurrentTime();

		break;

	case ERI_EJECTOR_OFF:
		//2012,1220
		if( GetCurrentTime() - m_dwTimeInit < 0 )
		{
			m_dwTimeInit = GetCurrentTime();
		}
		//2015.0819
// 		if( GetCurrentTime() - m_dwTimeInit < 100 )
// 			return;
		if( GetCurrentTime() - m_dwTimeInit < st_time.n_wait_time[E_WAIT_PICKER_EJECT][IO_ON] )
			return;

		g_ioMgr.set_out_bit( st_io.o_picker1_eject, IO_OFF );
		g_ioMgr.set_out_bit( st_io.o_picker2_eject, IO_OFF );
		g_ioMgr.set_out_bit( st_io.o_picker3_eject, IO_OFF );
		g_ioMgr.set_out_bit( st_io.o_picker4_eject, IO_OFF );
		mn_InitStep = ERI_PICKER_VACUUM_UP;

		break;


	case ERI_PICKER_VACUUM_UP:		ERI_PickerVacuumUp();					break;
	case ERI_PICKER_VACUUM_UP_CHK:	ERI_PickerVacuumUp_Chk();				break;
	case ERI_PICKER_OFF:			ERI_PickerOff();						break;
	case ERI_PICKER_OFF_CHK:		ERI_PickerOff_Chk();					break;
	case ERI_HOMECHK_Z:				ERI_HomeChk_Z();						break;
	case ERI_HOMECHK_P:				ERI_HomeChk_P();						break;//2014,0521
	case ERI_MOVECHK_P:				ERI_MoveChk_P();						break;
	case ERI_HOMECHK_ZP:			
		ERI_HomeChk_ZP();						break;
	case ERI_MOVECHK_ZP:			
		ERI_MoveChk_ZP();						break;
	case ERI_HOMECHK_X:				ERI_HomeChk_X();						break;
	case ERI_HOMECHK_Y:				ERI_HomeChk_Y();						break;
	case ERI_FINISH:				ERI_Finish();							break;
	}
}

void CRun_Robot::Run_Move()
{
	COMI.ml_axiscnt[M_ROBOT_XY_INDEX] = 2;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][0] = MOTOR_ROBOT_X;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][1] = MOTOR_ROBOT_Y;

	//////////////////////////////////////////////////////////////////////////
	int nJobCnt = g_robot.GetJobCount();
	if( nJobCnt == 0 )
		return;

//  	g_robot.EraseFirstJob();
// 	g_robot.EraseFirstJob();
// 	g_robot.EraseFirstJob();
// 	g_robot.EraseFirstJob();
// 	g_robot.EraseFirstJob();
// 	g_robot.EraseFirstJob();

	stJobPicker& job = g_robot.GetFirstJob();

	if( picker_step_old != job.picker_step )
	{
		picker_step_old = job.picker_step;
		m_dwTime_Robot = GetCurrentTime();
		if( nJobCnt == 0 )
			return;
			
		if( Run_Move_Chk_Jam( job ) == false )
			return;

	}

	if(	picker_step_old == PSTEP_PICK_VACUUM_ON_CHK || picker_step_old == PSTEP_PLACE_VACUUM_OFF_CHK ||
		picker_step_old == PSTEP_PICK_Z_UP || picker_step_old == PSTEP_PLACE_Z_UP)
	{
		if( Run_Move_Chk_Time( job ) == false )
			return;
	}
	if( Run_Move_Chk_Time() == false )
		return;

	Func.ThreadFunctionStepTrace(20, job.picker_step);

	switch( job.picker_step )
	{
	case PSTEP_NONE:	break;

	case PSTEP_PICK_READY:		job.picker_step = PSTEP_PICK_READY_CHK;		break;
	case PSTEP_PICK_READY_CHK:
		//ybs 2012.0225
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( job.GetPosByTarget() == MPOS_INDEX_BIN1)
			{
				job.picker_step = PSTEP_PICK_POGO_BACK;
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_REJ )
			{
				if( g_handler.GetStepTTrans() == ETT_TO_NONE )
					job.picker_step = PSTEP_PICK_POGO_BACK;
			}
			else
			//2015.0914
			{
				job.picker_step = PSTEP_PICK_POGO_BACK;
			
				if( job.GetPosByTarget() / 100 >= 1)
				{
					for( int j=0; j<TSITE_SOCKET_CNT; j++ )
					{
						if (st_handler.isTestRun[j] != CTL_YES)
						{
							st_handler.mn_InstandyBy_1[j] = CTL_YES;
						}	
					}
				}
			}
			///////////////
		}
		else
		{
			if( job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				job.picker_step = PSTEP_PICK_POGO_BACK;
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_REJ )
			{
				if( g_handler.GetStepTTrans() == ETT_TO_NONE )
					job.picker_step = PSTEP_PICK_POGO_BACK;
			}
			else
			//2015.0914
			{
				job.picker_step = PSTEP_PICK_POGO_BACK;
				
				if( job.GetPosByTarget() / 100 >= 1)
				{
					for( int j=0; j<TSITE_SOCKET_CNT; j++ )
					{
						if (st_handler.isTestRun[j] != CTL_YES)
						{
							st_handler.mn_InstandyBy_1[j] = CTL_YES;
						}
						
					}
				}
			}
			/////////////////
		}
		break;
		
		//2012,1225
	case PSTEP_PICK_POGO_BACK:
		if(st_handler.mn_pogo_type == CTL_YES && st_basic.n_mode_pogo == CTL_YES)
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
			{
				int nSite = job.iPosFirst / 5;
				int nIdx = job.iPosFirst % 5;
				Run_Test_Site[nSite].ReqPogo( nIdx, BACKWARD );
			}
			job.picker_step = PSTEP_PICK_POGO_BACK_CHK;
		}
		else
		{
			job.picker_step = PSTEP_PICK_INSERT_BACK;
		}
		break;
		
	case PSTEP_PICK_POGO_BACK_CHK:
 		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			if( Run_Test_Site[nSite].GetPogo( nIdx ) != FB_NONE )
				break;
		}
		job.picker_step = PSTEP_PICK_INSERT_BACK;
		break;

	case PSTEP_PICK_INSERT_BACK:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			Run_Test_Site[nSite].ReqInsert( nIdx, BACKWARD );
		}
		job.picker_step = PSTEP_PICK_INSERT_BACK_CHK;
		break;

	case PSTEP_PICK_INSERT_BACK_CHK:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			if( Run_Test_Site[nSite].GetInsert( nIdx ) != FB_NONE )
				break;
		}

////2015.0914 Test cnt = 2이고 Scrap_code = 0일 경우 트레이 위치에서 안착 상태 알람 띄움
		else if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && (job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3) && 
			st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES && (st_basic.n_retest_jig == CTL_YES || st_basic.n_retest_jig == CTL_NO))//jig : use, retest jig : use or Notuse
		{
			ATray tray;
			int nTarget;
			int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
			
			if (job.GetPosByTarget() == MPOS_INDEX_BIN1) nTarget = g_Tray.GetIdxByPos( TPOS_BIN1 );
			if (job.GetPosByTarget() == MPOS_INDEX_BIN2) nTarget = g_Tray.GetIdxByPos( TPOS_BIN2 );
			if (job.GetPosByTarget() == MPOS_INDEX_BIN3) nTarget = g_Tray.GetIdxByPos( TPOS_BIN3 );
			
			if(nTarget < 0)//nTarget == -1이면 strScrapCode = g_Tray.GetTray(nTarget).GetModule(i).GetScrapCode();에서 디버그 에러 발생함
			{
			}
			else
			{
				for( int i=0; i<iMdlCnt; i++ )
				{
					int nTestCnt = g_Tray.GetTray(nTarget).GetModule(i).GetTestCnt();
					CString strScrapCode = g_Tray.GetTray(nTarget).GetModule(i).GetScrapCode();
					////2015.0922
					CString strBinRetest = g_Tray.GetTray(nTarget).GetModule(i).GetBin();
					//if( nTestCnt > 2 && strScrapCode == "0")
					if( nTestCnt > 2 && (strScrapCode == "0" || strScrapCode == "4590" || strScrapCode == "4598") && strBinRetest == "")
					////
					{
						CString strJamCode;
						if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
						{
							strJamCode.Format( "8000%02d", i );//800000 ~ 
						}
						else if(job.GetPosByTarget() == MPOS_INDEX_BIN2)
						{
							strJamCode.Format( "8001%02d", i );//800100 ~ 
						}
						else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
						{
							strJamCode.Format( "8002%02d", i );//800200 ~ 
						}
						
						CTL_Lib.Alarm_Error_Occurrence( 930, CTL_dWARNING, strJamCode );
						break;
					}
				}
			}
		}
		else if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && (job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3) && 
			st_basic.mn_alignBuffer_type == CTL_NO && st_basic.n_retest_jig == CTL_YES)//jig : Notuse, retest jig : use
		{
			ATray tray;
			int nTarget;
			int iMdlCnt = st_basic.n_tray_x * st_basic.n_tray_y;
			
			if (job.GetPosByTarget() == MPOS_INDEX_BIN1) nTarget = g_Tray.GetIdxByPos( TPOS_BIN1 );
			if (job.GetPosByTarget() == MPOS_INDEX_BIN2) nTarget = g_Tray.GetIdxByPos( TPOS_BIN2 );
			if (job.GetPosByTarget() == MPOS_INDEX_BIN3) nTarget = g_Tray.GetIdxByPos( TPOS_BIN3 );

			if(nTarget < 0)//nTarget == -1이면 strScrapCode = g_Tray.GetTray(nTarget).GetModule(i).GetScrapCode();에서 디버그 에러 발생함
			{
			}
			else
			{
				for( int i=0; i<iMdlCnt; i++ )
				{
					int nTestCnt = g_Tray.GetTray(nTarget).GetModule(i).GetTestCnt();
					CString strScrapCode = g_Tray.GetTray(nTarget).GetModule(i).GetScrapCode();
					////2015.0922
					CString strBinRetest = g_Tray.GetTray(nTarget).GetModule(i).GetBin();
					//if( nTestCnt > 2 && strScrapCode == "0")
					if( nTestCnt > 2 && (strScrapCode == "0" || strScrapCode == "4590" || strScrapCode == "4598") && strBinRetest == "")
					////
					{
						CString strJamCode;
						if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
						{
							strJamCode.Format( "8000%02d", i );//800000 ~ 
						}
						else if(job.GetPosByTarget() == MPOS_INDEX_BIN2)
						{
							strJamCode.Format( "8001%02d", i );//800100 ~ 
						}
						else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
						{
							strJamCode.Format( "8002%02d", i );//800200 ~ 
						}
						
						CTL_Lib.Alarm_Error_Occurrence( 931, CTL_dWARNING, strJamCode );
						break;
					}
				}
			}
		}		
////
		job.picker_step = PSTEP_PICK_PICKER_UP1;
		break;

	case PSTEP_PICK_PICKER_UP1:
		Run_Robot.ReqGripperDown( BACKWARD );
		Run_Robot.ReqVacDown(0, BACKWARD );
		Run_Robot.ReqVacDown(1, BACKWARD );
		Run_Robot.ReqVacDown(2, BACKWARD );
		Run_Robot.ReqVacDown(3, BACKWARD );

		//job.picker_step = PSTEP_PICK_PICKER_UP1_CHK;//2014,1029
		if( job.bUpSkip )
			job.picker_step = PSTEP_PICK_PICKER_UP1_CHK;
		else
		{
			job.picker_step = PSTEP_PICK_PICKER_Z_UP;
			if(COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z) < st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + 0.2)
			{
				job.picker_step = PSTEP_PICK_PICKER_UP1_CHK;
			}
		}

		break;

	case PSTEP_PICK_PICKER_Z_UP://2014,0814
		{
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PICKER_UP1_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1600, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;
	
	case PSTEP_PICK_PICKER_UP1_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;
		if( Run_Robot.GetVacDown(0) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(1) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(2) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(3) != FB_NONE )	break;

		job.picker_step = PSTEP_PICK_ROBOT_Z_SAFETY;
 		if( job.bUpSkip )
 			job.picker_step = PSTEP_PICK_MDL_CALC;
		break;

	case PSTEP_PICK_PICKER_Z_UP_CHK://2014,1029
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				//job.picker_step = PSTEP_PICK_MDL_CALC;
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z;
				//2013,1031
				if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES)
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
						st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
					{
						job.picker_step = PSTEP_JIG_PICK_PITCH_Z;//PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC;
					}
				}
				else if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2014,0719
				{
					job.picker_step = PSTEP_PICK_FOR_PITCH_Z;
				}
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
//2012,1128
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 600, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PICK_ROBOT_Z_SAFETY;
			}
		}
		break;

	case PSTEP_PICK_ROBOT_Z_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				//job.picker_step = PSTEP_PICK_MDL_CALC;
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z;

////2015.0914
				// 				//2013,1031
				// 				if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES)
				// 				{
				// 					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
				// 						st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
				// 					{
				// 						job.picker_step = PSTEP_JIG_PICK_PITCH_Z;//PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC;
				// 					}
				// 				}
				
				if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES &&
					(st_basic.n_retest_jig == CTL_YES || st_basic.n_retest_jig == CTL_NO))//jig : use, retest jig : use or Notuse
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
						st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
					{
						job.picker_step = PSTEP_JIG_PICK_PITCH_Z;//PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC;
					}
				}
				else if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_NO &&
					st_basic.n_retest_jig == CTL_YES)//jig : Notuse, retest jig : use
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
						st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
					{
						COLORREF color[PICKER_CNT];
						int i;
						for( i=0; i<PICKER_CNT; i++ )
						{
							color[i] = CalcModuleColor( g_robot.GetPicker(i).GetModule());
							if(  color[i] == RED_L )
							{
								job.picker_step = PSTEP_JIG_PICK_PITCH_Z;//PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC;
							}
						}
					}
				}
////
				else if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2014,0719
				{
					job.picker_step = PSTEP_PICK_FOR_PITCH_Z;
				}
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 600, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_JIG_PICK_PITCH_Z:
		{
			//2014,0717
//			int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[ MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[ MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 692, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_MDL_CALC:
		md_MotFlag[0] = CTL_NO;
		md_MotFlag[1] = CTL_NO;
		job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MOVE_TARGET;
		{
			m_dPos[0] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG];
			m_dPos[1] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG];
			
			double dPos_Cur[2];
			dPos_Cur[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dPos_Cur[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
			
			//ybs 2012.0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				//dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START];
				//2016.0426
				if(m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] && m_dPos[1] >= st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] )
				{
					if( dPos_Cur[0] < ( st_handler.mn_safety - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
					{
						job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
					}
				}
				else
				{
					if(m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START])
					{
						if( ( dPos_Cur[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
					if(m_dPos[0] <= st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START])
					{
						if( ( dPos_Cur[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}				
				}
			}
			else
			{
				//dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END];
				//2016.0426
				if(m_dPos[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] && m_dPos[1] >= st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] )
				{
					if( dPos_Cur[0] > ( st_handler.mn_safety + COMI.md_allow_value[MOTOR_ROBOT_X] ) )
					{
						job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
					}
				}
				else
				{				
					if(m_dPos[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END])
					{
						if( ( dPos_Cur[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )

						{
							job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
					if(m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 5.0f)
					{
						if( ( dPos_Cur[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
				}
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_MDL_MOVE_START:
		{
			double dMdl[2];
			
			//ybs 2012.0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START];
				//ybs
				if(job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
				}
				else if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
				}
				else
				{
					dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
				}
			}
			else
			{
				dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END];
				//ybs
				if(job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
				}
				else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
				}
				else
				{
					dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
				}
			}
			
			
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;
			
			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );

			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_MOVE_TARGET;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 680, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_MOVE_TARGET:
		{
			m_dPos[0] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG];
			m_dPos[1] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG];
			
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;
			
			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;
			
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_JIG_PICK_PITCH_Z_CHK;//2014,0805 PSTEP_PICK_PLACE_JIG_FOR_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 681, CTL_dWARNING, alarm.mstr_code );
			}			
		}
		break;

	case PSTEP_JIG_PICK_PITCH_Z_CHK:
		{
			//2014,0802
			int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[ MOTOR_PITCH_Z].d_pos[P_PZ_ZIG] );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 680, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_JIG_PICK_PITCH_Z_RETRY;
			}
		}
		break;
		
	case PSTEP_JIG_PICK_PITCH_Z_RETRY:
		{
			//2014,0805
			int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[ MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 681, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;	

	case PSTEP_PICK_PLACE_JIG_FOR_Z_DOWN:
		m_zPlace = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PLACE];		
		
		if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPlace - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
			(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPlace + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
		{
			job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN;
		}
		else
		{
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPlace;
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPlace, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 682, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					Run_Robot.ReqVacDown( i, FORWARD );
				}
			}
		}		
		
		job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN_CHK;
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN_RETRY://2013,1119
		{
			m_zPlace = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PLACE];
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPlace;
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPlace, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 673, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPlace);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
				
				
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_VACUUM_OFF;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 683, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_VACUUM_OFF:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_vacuum[i], IO_OFF );
				}
			}
			
			job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_ON;
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_ON:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_eject[i], IO_ON );
				}
			}			
			m_dwTime_Robot = GetCurrentTime();
			job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF;
		}
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF:
		{
			if( GetCurrentTime() - m_dwTime_Robot < 0 )
			{
				m_dwTime_Robot = GetCurrentTime();
			}
			//2015.0819
// 			if( GetCurrentTime() - m_dwTime_Robot < 100 )	
// 				return;
			if( GetCurrentTime() - m_dwTime_Robot < st_time.n_wait_time[E_WAIT_PICKER_EJECT][IO_ON] )	
				return;
			
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_eject[i], IO_OFF );
				}
			}			
		}
		
		job.picker_step = PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK;
		break;

	case PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK:
		{
			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[0] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[1] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[2] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[3] ) == IO_ON ) break;
			
			m_dwTime_Robot = GetCurrentTime();
			job.picker_step = PSTEP_PICK_PICK_JIG_FOR_Z_DOWN;
		}
		break;


	case PSTEP_PICK_PICK_JIG_FOR_Z_DOWN:
		{
			if( GetCurrentTime() - m_dwTime_Robot < 0 )
			{
				m_dwTime_Robot = GetCurrentTime();
			}
			//2015.0819
// 			if( GetCurrentTime() - m_dwTime_Robot < 500 )	
// 				return;
			if( GetCurrentTime() - m_dwTime_Robot < st_time.n_wait_time[E_WAIT_PICKER_DN][IO_ON] )	
				return;
			
			m_zPickup = m_dPos[2] = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK];
			
			if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPickup - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
				(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPickup + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
			{
				job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_SKIP;
			}
			else
			{
				if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPickup;
				int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPickup, st_basic.nRunSpeed);
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 684, CTL_dWARNING, alarm.mstr_code );
				}
			}
			
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					Run_Robot.ReqVacDown( i, FORWARD );
				}
			}
			job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_CHK;
		}		
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_SKIP:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					Run_Robot.ReqVacDown( i, FORWARD );
				}
			}
			
		}
		
		job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_CHK;
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_RETRY://2013,1119
		{
			m_zPickup = m_dPos[2] = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK];
			
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPickup;
			int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPickup, st_basic.nRunSpeed);
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 674, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPickup);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{				
				if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
				
				job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_VACUUM_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 685, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_VACUUM_ON:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					g_ioMgr.set_out_bit( st_io.o_picker_vacuum[i], IO_ON );
			}
			job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK;
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK:
		if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[0] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[1] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[2] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[3] ) == IO_OFF) break;
		job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_UP;
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_UP:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					Run_Robot.ReqVacDown( i, BACKWARD );
				}
			}		
			job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_UP_CHK;
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_UP_CHK:
		{
			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
			if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
			if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
			if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
			
			
			job.picker_step = PSTEP_PICK_PICK_JIG_FOR_PICKER_Z_UP;
		}
		break;

	case PSTEP_PICK_PICK_JIG_FOR_PICKER_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				for(int i = 0; i < 4; i++)
				{
					st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
				}
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z;// 2014,0717PSTEP_PICK_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 686, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;


	case PSTEP_PICK_FOR_PITCH_Z://2014,0805
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nManualSpeed );
				int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nRunSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_MDL_CALC;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 693, CTL_dWARNING, alarm.mstr_code );
				}
			}
			else
			{
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nRunSpeed );
				int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nRunSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_MDL_CALC;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 693, CTL_dWARNING, alarm.mstr_code );
				}

			}
		}
		break;

	case PSTEP_PICK_MDL_CALC:
		//X,Y 위치가 reject tray와 어떤 연관 관계가 있는지 먼저 예측 
		//일단 reject tray가 safety 위치에 있는지 확이늘 해야 한다.
		//2016.0711
// 		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
// 			break;
		
		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
		{
			if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
			{
			}
			else
			{
				break;
			}
		}
		

		if( Run_Reject_Tray_Transfer.GetMoveToRej() != true ||
			Run_Reject_Tray_Transfer.GetMoveToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToSafety() != true || 
			Run_Reject_Tray_Transfer.GetMoveTransferToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToRej() != true )
		{
			if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
			{
			}
			else
			{
				break;
			}
		}

		//job.picker_step = PSTEP_PICK_MOVE_TARGET;
		{
			int nRet = RET_PROCEED;
			int nPickerIdx = job.iRobotFirst;
			if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )
			{
				if( job.iRobotFirst < 0 && job.iRobotSecond > 1 )
					nPickerIdx = job.iRobotSecond - 2;
			}
			else
			{
				if( job.iRobotFirst < 0 && job.iRobotSecond > 1 )
					nPickerIdx = job.iRobotSecond - 2;
			}
			
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PICK, job.iPosFirst, nPickerIdx);
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PICK, job.iPosFirst, nPickerIdx);

			double dPos_Cur[2], dMdl[2];
			dPos_Cur[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dPos_Cur[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );

			if(st_handler.mn_mirror_type == CTL_YES)
			{
				dMdl[0] = st_handler.md_safty_rbtx;
				dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT];

				if( job.GetPosByTarget() == MPOS_INDEX_BIN1)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_TRAY, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8801, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}
					
					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
					}								
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동						
						}
					}
					else if( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else if( dPos_Cur[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}
				}
				else if( job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_REJECT, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8802, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}


					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;
						}
					}
					else if( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}

				}
				else//TESTSITE
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_OUT, ESS_TESTSITE, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8803, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					

					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_BIN3;
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) )
					{//Tray 방향 OK//SOCKET 방향 제한
						if( m_dPos[1] >= dPos_Cur[1] )//갈려는 방햔이 현재 보다 크거나 같다면 X safety 이동
						{
							job.picker_step = PSTEP_PICK_MDL_BIN3;//Reject 위치에서 테스트사이트로 움직일 경우 X축 safety로 움직 후 Listmotion 이 적용되어야
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;
						}
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//TRAY3 번 위치
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_BIN3;
						}
						else
						{							
							if( m_dPos[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								//job.picker_step = PSTEP_PICK_MDL_BIN3;//Listmotion 이 적용되어야
								if( m_dPos[0] > ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
								{
									job.picker_step = PSTEP_PICK_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PICK_MDL_BIN3;							
								}
							}
							else if( m_dPos[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEX_START;//Listmotion 이 적용되어야
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}							
						}

					}
					else
					{
						job.picker_step = PSTEP_PICK_MOVE_TARGET;
					}					
				}
			}//if(st_handler.mn_mirror_type == CTL_YES)
			else
			{
			
				dMdl[0] = st_handler.md_safty_rbtx;
				dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT];
				
				if( job.GetPosByTarget() == MPOS_INDEX_BIN3)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_TRAY, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8804, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					
					
					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
					}								
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && //2016.0410
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동						
						}
					}
					else if( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else if( dPos_Cur[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}					
				}
				else if( job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_REJECT, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8805, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					

					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;
						}
					}
					else if( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PICK_MOVE_TARGET;//바로 이동
						}
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}
				}
				else//TESTSITE, TRAY
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_OUT, ESS_TESTSITE, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8806, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					

					//만일 BIN3에서 TRAY1,2 또는 TEST 갈때 생각
					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PICK_MDL_BIN3;
					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) )
					{//Tray 방향 OK//SOCKET 방향 제한
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_BIN3;
						}
						else
						{
							if( m_dPos[1] >= dPos_Cur[1] )//갈려는 방햔이 현재 보다 크거나 같다면 X safety 이동
							{
								job.picker_step = PSTEP_PICK_MDL_BIN3;//Reject 위치에서 테스트사이트로 움직일 경우 X축 safety로 움직 후 Listmotion 이 적용되어야
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//TRAY3 번 위치

						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PICK_MDL_BIN3;
						}
						else
						{
							if( m_dPos[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								//job.picker_step = PSTEP_PICK_MDL_BIN3;//Listmotion 이 적용되어야
								if( m_dPos[0] < ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) )
								{
									job.picker_step = PSTEP_PICK_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PICK_MDL_BIN3;							
								}

							}
							else if( m_dPos[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PICK_MDL_MOVEX_START;
							}
							else
							{
								job.picker_step = PSTEP_PICK_MOVE_TARGET;
							}
						}
					}
					else
					{
						job.picker_step = PSTEP_PICK_MOVE_TARGET;
					}
				}
			}			
		}
		break;
		
	case PSTEP_PICK_MDL_BIN3:
		{//무조건 safety영역으로 빠졌다가 REJECT_Y 가 safety로 빠졌다가 들어간다.
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, st_handler.md_safty_rbtx, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6695, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PICK_MDL_BIN3;
			}
		}
		break;
		
	case PSTEP_PICK_MDL_MOVEY_START:
		{

			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
			{
				job.picker_step = PSTEP_PICK_MDL_CALC;
				break;
			}
			else
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					m_dMdl[0] = st_handler.md_safty_rbtx;
					if( job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[1] = m_dPos[1];
					}
					else if( job.GetPosByTarget() == MPOS_INDEX_BIN1)
					{
						m_dMdl[1] = m_dPos[1];//st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
					}
					else
					{
						m_dMdl[1]  = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					}
				}
				else
				{
					m_dMdl[0] = st_handler.md_safty_rbtx;
					if( job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[1] = m_dPos[1];//st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
					}
					else if( job.GetPosByTarget() == MPOS_INDEX_BIN3)
					{
						m_dMdl[1] = m_dPos[1];//st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
					}
					else//TESTSITE
					{
						m_dMdl[1]  = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					}
				}

				long lAxis[2];
				lAxis[0] = MOTOR_ROBOT_X;
				lAxis[1] = MOTOR_ROBOT_Y;

				double dSpeedRatio[3];
				dSpeedRatio[0] = (double)st_basic.nRunSpeed;
				dSpeedRatio[1] = (double)st_basic.nRunSpeed;
				dSpeedRatio[2] = (double)st_basic.nRunSpeed;

				//int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );
				int nRet = COMI.Start_MultiMove( 2, lAxis, m_dMdl, dSpeedRatio );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_MDL_MOVEY_CHECK;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 2602, CTL_dWARNING, alarm.mstr_code );
				}
			}
		}		
		break;

	case PSTEP_PICK_MDL_MOVEX_START://X축 값은 갈려는 곳으로 보내고 Y값만 현재 값으로 해서 BIN3와 Reject 위치를 벗어난다 (Ovrride를 사용하기 위해)
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
			{
				job.picker_step = PSTEP_PICK_MDL_CALC;
				break;
			}
			else
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{				
					m_dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100;
					if( job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else if( job.GetPosByTarget() == MPOS_INDEX_BIN1)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else//TESTSITE
					{
						m_dMdl[0] = m_dPos[0];
					}
				}
				else
				{
					m_dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100;
					if( job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else if( job.GetPosByTarget() == MPOS_INDEX_BIN3)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else//TESTSITE
					{
						m_dMdl[0] = m_dPos[0];
					}
				}
				
				long lAxis[2];
				lAxis[0] = MOTOR_ROBOT_X;
				lAxis[1] = MOTOR_ROBOT_Y;
				
				double dSpeedRatio[3];
				dSpeedRatio[0] = (double)st_basic.nRunSpeed;
				dSpeedRatio[1] = (double)st_basic.nRunSpeed;
				dSpeedRatio[2] = (double)st_basic.nRunSpeed;
				
				//int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );
				int nRet = COMI.Start_MultiMove( 2, lAxis, m_dMdl, dSpeedRatio );
				if( nRet == BD_GOOD )
				{
					m_dwTimeMoveX = GetCurrentTime();
					job.picker_step = PSTEP_PICK_MDL_MOVEX_CHECK;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 602, CTL_dWARNING, alarm.mstr_code );
				}
			}

		}		
		break;

	case PSTEP_PICK_MDL_MOVEY_CHECK:
		{//근처에 도달했는지 체크한다.			
			double dMdl[2];
			dMdl[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
			
			if( ( dMdl[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100 ) ) && ( dMdl[1] >= (m_dMdl[1] - 50) ) )
			{
				job.picker_step = PSTEP_PICK_MOVE_OVERRIDEY_TARGET;
			}
		}
		break;
		
	case PSTEP_PICK_MDL_MOVEX_CHECK:
		{//근처에 도달했는지 체크한다.			
			double dMdl[2];
			dMdl[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );

			if( GetCurrentTime() - m_dwTimeMoveX < 0 )
				m_dwTimeMoveX = GetCurrentTime();

			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( m_dPos[0] > st_handler.md_safty_rbtx )
				{
					if( dMdl[0] >= ( st_handler.md_safty_rbtx - 100 ) )
					{
						m_dwTimeMoveX = GetCurrentTime();
						job.picker_step = PSTEP_PICK_MOVE_OVERRIDEX_TARGET;
					}
					else
					{
						if( GetCurrentTime() - m_dwTimeMoveX > 5000)
						{
							COMI.Set_MotStop(0, MOTOR_ROBOT_X);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
					}
				}
				else
				{
					job.picker_step = PSTEP_PICK_MDL_CALC;
				}

			}
			else
			{

				if( m_dPos[0] < st_handler.md_safty_rbtx )
				{
					if( dMdl[0] <= ( st_handler.md_safty_rbtx + 100 ) )
					{
						m_dwTimeMoveX = GetCurrentTime();
						job.picker_step = PSTEP_PICK_MOVE_OVERRIDEX_TARGET;
					}
					else
					{
						if( GetCurrentTime() - m_dwTimeMoveX > 5000)
						{
							COMI.Set_MotStop(0, MOTOR_ROBOT_X);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
					}
				}
				else
				{
					job.picker_step = PSTEP_PICK_MDL_CALC;
				}
			}
		}
		break;
		
	case PSTEP_PICK_MOVE_OVERRIDEY_TARGET://Y값이 일정 위치로 오면 X값을 Oveeride한다
		{
			long dwIsStopped = 0;
			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_X, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PICK_MOVE_X_TARGET;
			}
			else
			{
				int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_X, m_dPos[0]);
				
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_MOVE_X_TARGET_CHECK;
				}
				else
				{
					alarm.mstr_code = "801000";
					CTL_Lib.Alarm_Error_Occurrence( 6800, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
					job.picker_step = PSTEP_PICK_MDL_CALC;
				}
			}
		}
		break;

	case PSTEP_PICK_MOVE_OVERRIDEX_TARGET://X값이 일정 위치로 오면 Y값을 Oveeride한다
		{
			long dwIsStopped = 0;
			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
				//job.picker_step = PSTEP_PICK_MOVE_Y_TARGET;
// 			}
// 			else
// 			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					if( m_dPos[0] > 0 &&
						m_dPos[0] > st_handler.md_safty_rbtx &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) >= ( st_handler.md_safty_rbtx - 100 ) )
					{
						md_MotFlag[0] = CTL_NO;
						md_MotFlag[1] = CTL_NO;
						job.picker_step = PSTEP_PICK_MOVE_Y_TARGET;
						break;
						
						int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);					
						if( nRet == BD_GOOD )
						{
							job.picker_step = PSTEP_PICK_MOVE_Y_TARGET_CHECK;
						}
						else
						{
							alarm.mstr_code = "801000";
							CTL_Lib.Alarm_Error_Occurrence( 6801, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
					}
				}
				else
				{
					if( m_dPos[0] > 0 &&
						m_dPos[0] < st_handler.md_safty_rbtx &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= ( st_handler.md_safty_rbtx + 100 ) )
					{
						md_MotFlag[0] = CTL_NO;
						md_MotFlag[1] = CTL_NO;
						job.picker_step = PSTEP_PICK_MOVE_Y_TARGET;
						break;

						int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);					
						if( nRet == BD_GOOD )
						{
							job.picker_step = PSTEP_PICK_MOVE_Y_TARGET_CHECK;
						}
						else
						{
							alarm.mstr_code = "801000";
							CTL_Lib.Alarm_Error_Occurrence( 6801, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
							job.picker_step = PSTEP_PICK_MDL_CALC;
						}
					}
				}
			}


			/*
			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
				if( m_dPos[0] < st_handler.md_safty_rbtx &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= ( st_handler.md_safty_rbtx - 100 ) )
				{
					int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);					
					if( nRet == BD_GOOD )
					{
						job.picker_step = PSTEP_PICK_MOVE_Y_TARGET_CHECK;
					}
					else
					{
						alarm.mstr_code = "999000";
						CTL_Lib.Alarm_Error_Occurrence( 6610, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
						job.picker_step = PSTEP_PICK_MDL_CALC;
					}
				}
				else
				{
					COMI.Set_MotStop(0, MOTOR_ROBOT_X);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
					job.picker_step = PSTEP_PICK_MDL_CALC;

				}
			}*/
		}
		break;


	case PSTEP_PICK_MOVE_X_TARGET:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, m_dPos[0], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6611, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

		
	case PSTEP_PICK_MOVE_Y_TARGET:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Y, m_dPos[1], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6612, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_MOVE_X_TARGET_CHECK:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_X, m_dPos[0] );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z_CHK;;
				m_dwTime_Robot = GetCurrentTime();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_MDL_CALC;
				CTL_Lib.Alarm_Error_Occurrence( 6613, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_MOVE_Y_TARGET_CHECK:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Y, m_dPos[1] );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z_CHK;;
				m_dwTime_Robot = GetCurrentTime();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_MDL_CALC;
				CTL_Lib.Alarm_Error_Occurrence( 6614, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_MOVE_TARGET:
		
		//2016.0711
// 		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
// 			break;
		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
		{
			if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
			{
			}
			else
			{
				break;
			}
		}

		if( Run_Reject_Tray_Transfer.GetMoveToRej() != true ||
			Run_Reject_Tray_Transfer.GetMoveToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToSafety() != true || 
			Run_Reject_Tray_Transfer.GetMoveTransferToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToRej() != true )
		{
			if( job.iRobotFirst != -1 || job.iRobotSecond != -1 || job.iRobotThird != -1 || job.iRobotFourth != -1 )
			{
				if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
				{
				}
				else
				{
					break;
				}
			}
		}
		if( COMI.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
		{
			job.picker_step = PSTEP_PICK_MDL_CALC;//2016.0703 PSTEP_PICK_MDL_BIN3;
		}
		else
		{
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;


			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FOR_PITCH_Z_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 603, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_FOR_PITCH_Z_CHK://2014,0805
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{//2014,0802//2014,0805
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nRunSpeed );
				int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE] );
				if( nRet == BD_GOOD  )
				{
					job.picker_step = PSTEP_PICK_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 683, CTL_dWARNING, alarm.mstr_code );
					job.picker_step = PSTEP_PICK_FOR_PITCH_Z_RETRY;
				}
			}
			else
			{//2014,0802//2014,0805
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nRunSpeed );
				long dwIsStopped = 0,dwIsStopped1 = 0;
				g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_X, &dwIsStopped);
				g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped1);
				if( dwIsStopped == cmTRUE && dwIsStopped1 == cmTRUE )
				{
					int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY]);
// 					int nRet1 = COMI.Check_SingleMove( MOTOR_ROBOT_X, m_dPos[0] );
					if( nRet == BD_GOOD )
					{
						job.picker_step = PSTEP_PICK_Z_DOWN;
					}
// 						md_MotFlag[0] = CTL_YES;
// 					if( nRet1 == BD_GOOD )
// 						md_MotFlag[1] = CTL_YES;
// 					if( md_MotFlag[0] == CTL_YES && md_MotFlag[1] == CTL_YES)
					else if( nRet == BD_ERROR || nRet == BD_SAFETY)// || nRet1 == BD_ERROR || nRet1 == BD_SAFETY )
					{
						CTL_Lib.Alarm_Error_Occurrence( 684, CTL_dWARNING, alarm.mstr_code );
						job.picker_step = PSTEP_PICK_FOR_PITCH_Z_RETRY;
					}
				}
				
			}
		}
		break;
		
	case PSTEP_PICK_FOR_PITCH_Z_RETRY:
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{//2014,0802
				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nManualSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 685, CTL_dWARNING, alarm.mstr_code );
				}
			}
			else
			{//2014,0802
				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nManualSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 686, CTL_dWARNING, alarm.mstr_code );
				}
			}
		}
		break;


	case PSTEP_PICK_Z_DOWN:
		{
			m_zPos = m_dPos[2] = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, PICK, job.iPosFirst, job.iRobotFirst );

			if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPos - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
				(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPos + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
			{
				job.picker_step = PSTEP_PICK_PICKER_DOWN_SKIP;
			}
			else
			{
				if(COMI.mn_run_status == dRUN)	st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos;
				int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPos, st_basic.nRunSpeed);
				if( nRet == BD_GOOD )
				{
 					job.picker_step = PSTEP_PICK_PICKER_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 604, CTL_dWARNING, alarm.mstr_code );
				}
			}

		}
		break;

	case PSTEP_PICK_PICKER_DOWN:
		if( job.iRobotFirst != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFirst, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, FORWARD );
			}
		}
		
		if( job.iRobotSecond != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotSecond, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotSecond + 1, FORWARD );
			}
		}
		//2014,0709
		if( job.iRobotThird != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotThird, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotThird + 1, FORWARD );
			}
		}
		
		if( job.iRobotFourth != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFourth, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFourth + 1, FORWARD );
			}
		}
		
		job.picker_step = PSTEP_PICK_PICKER_DOWN_CHK;
		break;

	case PSTEP_PICK_PICKER_DOWN_SKIP:
		if( job.iRobotFirst != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFirst, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, FORWARD );
			}
		}
		
		if( job.iRobotSecond != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotSecond, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotSecond + 1, FORWARD );
			}
		}
		//2014,0709
		if( job.iRobotThird != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotThird, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotThird + 1, FORWARD );
			}
		}
		
		if( job.iRobotFourth != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFourth, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFourth + 1, FORWARD );
			}
		}
		
		job.picker_step = PSTEP_PICK_PICKER_DOWN_CHK;
		break;

	case PSTEP_PICK_PICKER_DOWN_RETRY://2013,1119
		{
			st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos;
			int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPos, st_basic.nRunSpeed);
			if( nRet == BD_GOOD )
			{
 				job.picker_step = PSTEP_PICK_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 672, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPos);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst ) != FB_NONE )		break;
				if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond ) != FB_NONE )		break;
				//2014,0709
				if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird ) != FB_NONE )		break;
				if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth ) != FB_NONE )		break;
				
				if( st_basic.n_pick_type == EPT_2V1M_2USE )
				{
					if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst + 1 ) != FB_NONE )		break;
					if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond + 1 ) != FB_NONE )		break;
					//2014,0709
					if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird + 1 ) != FB_NONE )		break;
					if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth + 1 ) != FB_NONE )		break;
				}
				
				//2013,0215
				mn_retry = 0;
				job.picker_step = PSTEP_PICK_VACUUM_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 687, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

		//2013,0215
	case PSTEP_PICK_VACUUM_ON_ZDOWN:
		{
			if(COMI.mn_run_status == dRUN)	st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos;
			int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPos, st_basic.nRunSpeed);
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 698, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;
		
	case PSTEP_PICK_VACUUM_ON_ZDOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPos);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_VACUUM_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 699, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_VACUUM_ON://2014,1023 2308
		if( job.iRobotFirst != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFirst], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFirst + 1], IO_ON );
			}
		}

		if( job.iRobotSecond != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotSecond], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotSecond + 1], IO_ON );
			}
		}
		if( job.iRobotThird != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotThird], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotThird + 1], IO_ON );
			}
		}
		
		if( job.iRobotFourth != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFourth], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFourth + 1], IO_ON );
			}
		}
		//2013,0215
		ml_picker_wait_time[0] = GetCurrentTime();
		job.picker_step = PSTEP_PICK_VACUUM_ON_CHK;
		break;

	case PSTEP_PICK_VACUUM_ON_CHK:
		if( st_basic.n_mode_device && job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst] ) == IO_OFF )
		{
			//2013,0215
			if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				ml_picker_wait_time[1] = GetCurrentTime();
				ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
				if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
				{
					mn_retry++;
 					st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
 					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					break;
				}
				else if(ml_picker_wait_time[2] < 0)
				{
					ml_picker_wait_time[0] = GetCurrentTime();
				}
				else if(mn_retry == 2)
				{
					mn_retry++;
					m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.0;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
				}
			}
			break;
		}
		if( st_basic.n_mode_device && job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond] ) == IO_OFF )
		{
			//2013,0215
			if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				ml_picker_wait_time[1] = GetCurrentTime();
				ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
				if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
				{
					mn_retry++;
					st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					break;
				}
				else if(ml_picker_wait_time[2] < 0)
				{
					ml_picker_wait_time[0] = GetCurrentTime();
				}
				else if(mn_retry == 2)
				{
					mn_retry++;
					m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
				}
			}
			break;
		}
		if( st_basic.n_mode_device && job.iRobotThird != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotThird] ) == IO_OFF )
		{
			//2013,0215
			if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				ml_picker_wait_time[1] = GetCurrentTime();
				ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
				if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
				{
					mn_retry++;
					st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					break;
				}
				else if(ml_picker_wait_time[2] < 0)
				{
					ml_picker_wait_time[0] = GetCurrentTime();
				}
				else if(mn_retry == 2)
				{
					mn_retry++;
					m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
				}
			}
			break;
		}
		if( st_basic.n_mode_device && job.iRobotFourth != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFourth] ) == IO_OFF )
		{
			//2013,0215
			if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				ml_picker_wait_time[1] = GetCurrentTime();
				ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
				if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
				{
					mn_retry++;
					st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					break;
				}
				else if(ml_picker_wait_time[2] < 0)
				{
					ml_picker_wait_time[0] = GetCurrentTime();
				}
				else if(mn_retry == 2)
				{
					mn_retry++;
					m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.5;
					job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
				}
			}
			break;
		}
		if( st_basic.n_pick_type == EPT_2V1M_2USE )
		{
			if( st_basic.n_mode_device && job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst + 1] ) == IO_OFF )
			{
				//2013,0215
				if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
				{
					ml_picker_wait_time[1] = GetCurrentTime();
					ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
					if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
					{
						mn_retry++;
						st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
						job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
						break;
					}
					else if(ml_picker_wait_time[2] < 0)
					{
						ml_picker_wait_time[0] = GetCurrentTime();
					}
					else if(mn_retry == 2)
					{
						mn_retry++;
						m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.5;
						job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					}
				}
				break;
			}
			if( st_basic.n_mode_device && job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond + 1] ) == IO_OFF )
			{
				//2013,0215
				if(job.GetPosByTarget() == MPOS_INDEX_BIN1 || job.GetPosByTarget() == MPOS_INDEX_BIN2 || job.GetPosByTarget() == MPOS_INDEX_BIN3)
				{
					ml_picker_wait_time[1] = GetCurrentTime();
					ml_picker_wait_time[2] = ml_picker_wait_time[1] - ml_picker_wait_time[0];
					if(ml_picker_wait_time[2] > 3000 && mn_retry < 2)
					{
						mn_retry++;
						st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) + 0.5;
						job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
						break;
					}
					else if(ml_picker_wait_time[2] < 0)
					{
						ml_picker_wait_time[0] = GetCurrentTime();
					}
					else if(mn_retry == 2)
					{
						mn_retry++;
						m_zPos = g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) - 1.5;
						job.picker_step = PSTEP_PICK_VACUUM_ON_ZDOWN;
					}
				}
				break;
			}
		}
		job.picker_step = PSTEP_PICK_PICKER_UP2;
		break;

	case PSTEP_PICK_PICKER_UP2:
		if( job.iRobotFirst != -1 )		
		{
			Run_Robot.ReqVacDown( job.iRobotFirst, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, BACKWARD );
			}
		}
		if( job.iRobotSecond != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotSecond, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotSecond + 1, BACKWARD );
			}
		}
		//2014,0709
		if( job.iRobotThird != -1 )		
		{
			Run_Robot.ReqVacDown( job.iRobotThird, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotThird + 1, BACKWARD );
			}
		}
		if( job.iRobotFourth != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFourth, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFourth + 1, BACKWARD );
			}
		}

		//job.picker_step = PSTEP_PICK_PICKER_UP2_CHK;
 		job.picker_step = PSTEP_PICK_Z_UP_READY;//2015.1119

		break;

	case PSTEP_PICK_Z_UP_READY:
		//2014,0814
		if( g_handler.IsSkipable_Z_Safety() == false)
		{
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_dPos[2] = st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY];//2014,1202
				job.picker_step = PSTEP_PICK_PICKER_UP2_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1605, CTL_dWARNING, alarm.mstr_code );
			}
		}
		else
		{
			job.picker_step = PSTEP_PICK_PICKER_UP2_CHK;
		}
		break;

	case PSTEP_PICK_PICKER_UP2_CHK:
		if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst ) != FB_NONE )			break;
		if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond ) != FB_NONE )			break;
		//2014,0709
		if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird ) != FB_NONE )			break;
		if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth ) != FB_NONE )			break;

		
		if( st_basic.n_pick_type == EPT_2V1M_2USE )
		{
			if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst + 1 ) != FB_NONE )			break;
			if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond + 1 ) != FB_NONE )			break;
			//2014,0709
			if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird + 1 ) != FB_NONE )			break;
			if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth + 1 ) != FB_NONE )			break;
		}
		
//		job.picker_step = PSTEP_PICK_Z_UP;//2014,1029
 		job.picker_step = PSTEP_PICK_Z_UP_READY_CHK;//2015.1119
		if( g_handler.IsSkipable_Z_Safety() )
		{
			job.picker_step = PSTEP_PICK_FINISH;
		}
		break;

	case PSTEP_PICK_Z_UP_READY_CHK:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY]);
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1606, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PICK_Z_UP;
			}
		}
		break;

	case PSTEP_PICK_Z_UP:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
//2012,1128
// 				CString strJamCode;
// 				strJamCode.Format( "MM0Z10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 605, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case PSTEP_PICK_FINISH:
		{
			AModule* pMdl_1;
			AModule* pMdl_2;
			AModule* pMdl_3;
			AModule* pMdl_4;
			int nTrayIdx = -1;
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
			{
				int nSite = job.iPosFirst / 5;
				int nIdx = job.iPosFirst % 5;
				pMdl_1 = &g_site.GetModule( job.iPosFirst * 2 );
				pMdl_2 = &g_site.GetModule( job.iPosFirst * 2 + 1);
	
				//2014,0717
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
				{
					if( job.iRobotFirst != -1 && job.iRobotFirst == 0)	st_modulemap.nDeviceRobotPicker[3][job.iRobotFirst] = CTL_YES;
					if( job.iRobotSecond != -1 && job.iRobotSecond == 0) st_modulemap.nDeviceRobotPicker[3][job.iRobotSecond] = CTL_YES;
				}
				//2015.0914
				Run_Test_Site[(job.iPosFirst * 2) / 10].isOutStandBy[(job.iPosFirst * 2) % 10] = CTL_NO;
				Run_Test_Site[(job.iPosFirst * 2 + 1) /10 ].isOutStandBy[(job.iPosFirst * 2 + 1) % 10] = CTL_NO;

			}
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN1 )
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PICK_FINISH1) TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );
				}
				else
				{
						pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
						pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
						pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird % MPOS_DIVISION );
						pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}

				//2013,0203
				if( job.iRobotFirst != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotFirst] = CTL_YES;
				if( job.iRobotSecond != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotSecond] = CTL_YES;
				//2014,0709
				if( job.iRobotThird != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotThird] = CTL_YES;
				if( job.iRobotFourth != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotFourth] = CTL_YES;
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN2 )
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PICK_FINISH2) TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );
				}
				else
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird % MPOS_DIVISION );
					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}
				//2013,0203
				if( job.iRobotFirst != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotFirst] = CTL_YES;
				if( job.iRobotSecond != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotSecond] = CTL_YES;
				//2014,0709
				if( job.iRobotThird != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotThird] = CTL_YES;
				if( job.iRobotFourth != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotFourth] = CTL_YES;
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN3 )//ybs
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PICK_FINISH3) TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );

				}
				else
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird % MPOS_DIVISION );
					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}
				//2013,0203
				if( job.iRobotFirst != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotFirst] = CTL_YES;
				if( job.iRobotSecond != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotSecond] = CTL_YES;
				//2014,0709
				if( job.iRobotThird != -1 )	st_modulemap.nDeviceRobotPicker[3][job.iRobotThird] = CTL_YES;
				if( job.iRobotFourth != -1 ) st_modulemap.nDeviceRobotPicker[3][job.iRobotFourth] = CTL_YES;
			}
			
			if( job.iRobotFirst != -1 )		{ g_robot.GetPicker(job.iRobotFirst).SetModule( *pMdl_1 ); pMdl_1->EraseData(); }
			if( job.iRobotSecond != -1 )	{ g_robot.GetPicker(job.iRobotSecond).SetModule( *pMdl_2 ); pMdl_2->EraseData(); }
			//2014,0709
			if( job.iRobotThird != -1 )		{ g_robot.GetPicker(job.iRobotThird).SetModule( *pMdl_3 ); pMdl_3->EraseData(); }
			if( job.iRobotFourth != -1 )	{ g_robot.GetPicker(job.iRobotFourth).SetModule( *pMdl_4 ); pMdl_4->EraseData(); }
			
			// UI 업데이트
			if( st_handler.cwnd_main )
			{
				if( nTrayIdx > -1 )	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_INIT, nTrayIdx );
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_SEL_TRAY);
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDR_PICKER);

				if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);

				if( job.GetPosByTarget() == MPOS_INDEX_BIN1 )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_0 );

				if( job.GetPosByTarget() == MPOS_INDEX_BIN2 )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_1 );

				if( job.GetPosByTarget() == MPOS_INDEX_BIN3 )//ybs
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_2 );
			}
			
			g_robot.EraseFirstJob();

		}
		
		break;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
	case PSTEP_PLACE_READY:		job.picker_step = PSTEP_PLACE_READY_CHK;		break;
	case PSTEP_PLACE_READY_CHK:
// 		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
// 		{
// 			int nSite = job.iPosFirst / 5;
// 			int nIdx = job.iPosFirst % 5;
// 			if( nSite == 0 )
// 			{
// 				if( g_ioMgr.get_in_bit( st_io.o_testsite1_cok_plate_fwd ) == IO_OFF )			return;
// 				if( g_ioMgr.get_in_bit( st_io.o_testsite1_cok_plate_bwd ) == IO_ON )			return;
// 			}
// 			else
// 			{
// 				if( g_ioMgr.get_in_bit( st_io.o_testsite2_cok_plate_fwd ) == IO_OFF )	return;
// 				if( g_ioMgr.get_in_bit( st_io.o_testsite2_cok_plate_bwd ) == IO_ON )	return;
// 			}
// 		}

		job.picker_step = PSTEP_PLACE_REJECT_Z_CHK;
		break;

	case PSTEP_PLACE_REJECT_Z_CHK:
		job.picker_step = PSTEP_PLACE_POGO_BACK;

////2015.0914
		// 		//2013,1031
		// 		if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES)
		// 		{
		// 			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
		// 				st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
		// 			{
		// 				job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP; //2013.0131
		// 			}
		// 		}
		
		if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES &&
			(st_basic.n_retest_jig == CTL_YES || st_basic.n_retest_jig == CTL_NO))//jig : use, retest jig : use or Notuse
		{
			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
				st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP; //2013.0131
			}
		}
		else if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_NO &&
			st_basic.n_retest_jig == CTL_YES)//jig : Notuse, retest jig : use
		{
			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES ||
				st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES || st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES)
			{
				COLORREF color[PICKER_CNT];
				int i;
				for( i=0; i<PICKER_CNT; i++ )
				{
					color[i] = CalcModuleColor( g_robot.GetPicker(i).GetModule());
					if(  color[i] == RED_L )
					{
						job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP; //2013.0131
					}
				}
			}
		}
////
		if( job.GetPosByTarget() == MPOS_INDEX_REJ )
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 )	iRejCnt = 1;
			if( dPos_work < 1.0f )	dPos_work = dPos_1;
			double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);

			if( COMI.Check_MotPosRange( MOTOR_TRAY_STACKER, dTarget, COMI.md_allow_value[MOTOR_TRAY_STACKER] ) != BD_GOOD )
				job.picker_step = PSTEP_PLACE_REJECT_GUIDE_BACK;
		}
		break;

	case PSTEP_PLACE_REJECT_GUIDE_BACK:
		Run_Reject_Stacker.ReqGuide( BACKWARD );
		job.picker_step = PSTEP_PLACE_REJECT_GUIDE_BACK_CHK;
		break;

	case PSTEP_PLACE_REJECT_GUIDE_BACK_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;

		job.picker_step = PSTEP_PLACE_REJECT_Z_MOVE;
		break;

	case PSTEP_PLACE_REJECT_Z_MOVE:
		if( job.GetPosByTarget() == MPOS_INDEX_REJ )
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			int iRejCnt = g_Tray.GetRejCnt();
			if( iRejCnt == 0 )	iRejCnt = 1;
			if( dPos_work < 1.0f )	dPos_work = dPos_1;
			double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
			
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				//Run_Reject_Tray_Transfer.ReqMoveTransferToRej();//ybs
				job.picker_step = PSTEP_PLACE_REJECT_GUIDE_FORWARD;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 606, CTL_dWARNING, alarm.mstr_code );
			}
		}
		else
		{
			job.picker_step = PSTEP_PLACE_REJECT_GUIDE_FORWARD;
		}
		break;

	case PSTEP_PLACE_REJECT_GUIDE_FORWARD:
		Run_Reject_Stacker.ReqGuide( FORWARD );
		job.picker_step = PSTEP_PLACE_REJECT_GUIDE_FORWARD_CHK;
		break;

	case PSTEP_PLACE_REJECT_GUIDE_FORWARD_CHK:
		if( Run_Reject_Stacker.GetGuide() != FB_NONE )
			return;
		
		if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_YES && st_basic.n_mode_jig == CTL_YES &&
			(st_basic.n_retest_jig == CTL_YES || st_basic.n_retest_jig == CTL_NO))//jig : use, retest jig : use or Notuse
		{
			job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP;
		}
		else if( st_basic.n_pick_type != EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_TESTSITE && st_basic.mn_alignBuffer_type == CTL_NO &&
			st_basic.n_retest_jig == CTL_YES)//jig : Notuse, retest jig : use
		{
			job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP;
		}
////		
		
		else if(st_basic.n_pick_type == EPT_1V1M_4USE_2 && job.GetPosByTarget() == MPOS_INDEX_JIG)//2.5인치
		{
			job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP;
		}
		else
		{
			job.picker_step = PSTEP_PLACE_POGO_BACK;
		}
		break;

	case PSTEP_PLACE_JIG_FOR_PICKERUP:
		Run_Robot.ReqGripperDown( BACKWARD );
		Run_Robot.ReqVacDown(0, BACKWARD );
		Run_Robot.ReqVacDown(1, BACKWARD );
		Run_Robot.ReqVacDown(2, BACKWARD );
		Run_Robot.ReqVacDown(3, BACKWARD );

		job.picker_step = PSTEP_PLACE_JIG_FOR_PICKERUP_CHK;
		break;

	case PSTEP_PLACE_JIG_FOR_PICKERUP_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;
		if( Run_Robot.GetVacDown(0) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(1) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(2) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(3) != FB_NONE )	break;

		{
			double dCurentPos = COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z);
			if(dCurentPos <  st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z])
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_CALC;
			}
			else
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_ROBOT_Z_SAFETY;
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_ROBOT_Z_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
//				job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_CALC;//2014,1029
				job.picker_step = PSTEP_PLACE_PITCH_JIG;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 607, CTL_dWARNING, alarm.mstr_code );
			}
		}		
		break;

	case PSTEP_PLACE_PITCH_JIG://2014,0806
		{
			job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_CALC;//2014,0815
			break;
			int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 607, CTL_dWARNING, alarm.mstr_code );
			}
		}		
		break;


	case PSTEP_PLACE_JIG_FOR_MDL_CALC:
		md_MotFlag[0] = CTL_NO;
		md_MotFlag[1] = CTL_NO;
		job.picker_step = PSTEP_PLACE_JIG_FOR_MOVE_TARGET;
		{
			m_dPos[0] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG];
			m_dPos[1] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG];

			double dPos_Cur[2];
			dPos_Cur[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dPos_Cur[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
			
			//ybs 2012.0225 
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				//2016.0426
				if( m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] && m_dPos[1] >= st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] )
				{
					if( dPos_Cur[0] < ( st_handler.mn_safety - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
					{
						job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
					}
				}
				else
				{
					//2016.0410
					if( m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] )
					{
						if( ( dPos_Cur[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] )  &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
					if(m_dPos[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 5.0f)
					{
						if( ( dPos_Cur[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] )  &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
						}
						
					}
				}
			}
			else
			{
				//dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END];
				//2016.0426
				if(m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] && m_dPos[1] >= st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2] )
				{
					if( dPos_Cur[0] < ( st_handler.mn_safety - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
					{
						job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
					}
				}
				else
				{				
					if(m_dPos[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END])
					{
						if( ( dPos_Cur[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )

						{
							job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
					if(m_dPos[0] > st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 5.0f)
					{
						if( ( dPos_Cur[0] < st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] ) &&
							( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) )
						{
							job.picker_step = PSTEP_PLACE_JIG_FOR_MDL_MOVE_START;
						}
					}
				}
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_MDL_MOVE_START:
		{
			double dMdl[2];

			//ybs 2012.0225
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START];
				//ybs
				if(job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
				}
				else if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
				}
				else
				{
					dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
				}
			}
			else
			{
				dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END];
				//ybs
				if(job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
				}
				else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
				{
					dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
				}
				else
				{
					dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
				}
			}

							
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;
							
			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_MOVE_TARGET;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 609, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_MOVE_TARGET:
		{
// 		m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, job.iPosFirst, job.iRobotFirst );
// 		m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, job.iPosFirst, job.iRobotFirst );
			m_dPos[0] = st_motor[MOTOR_ROBOT_X].d_pos[P_XY_JIG];
			m_dPos[1] = st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_JIG];
			
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;
			
			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;
			
			//2016.0202
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );
 			int nRet1 = CTL_Lib.Single_Move(MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed);  // 해당 위치로 이동

			if( nRet == BD_GOOD )
				md_MotFlag[0] = CTL_YES;
			if( nRet1 == BD_GOOD )
				md_MotFlag[1] = CTL_YES;

			if( md_MotFlag[0] == CTL_YES && md_MotFlag[1] == CTL_YES)
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PLACE_JIG_FOR_PITCH_Z;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY || nRet1 == BD_ERROR || nRet1 == BD_SAFETY )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				CTL_Lib.Alarm_Error_Occurrence( 610, CTL_dWARNING, alarm.mstr_code );
			}

		}
		break;

	case PSTEP_PLACE_JIG_FOR_PITCH_Z:
		{//2014,0717
			job.picker_step = PSTEP_PLACE_JIG_FOR_Z_DOWN;//2014,1029
			break;

			int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_zPlace = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PLACE];
				
				
				if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPlace - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
					(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPlace + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
				{
					job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN;
				}
				else
				{
					if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPlace;
					int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPlace, st_basic.nRunSpeed );
					if( nRet == BD_GOOD )
					{
						job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN;
					}
					else if( nRet == BD_ERROR || nRet == BD_SAFETY )
					{
						CTL_Lib.Alarm_Error_Occurrence( 611, CTL_dWARNING, alarm.mstr_code );
					}
				}			
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 695, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_PITCH_Z_RETRY://2014,1029
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_ZIG], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 695, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_Z_DOWN:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_ZIG] );
			if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 695, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PLACE_JIG_FOR_PITCH_Z_RETRY;
				break;
			}
			else if( nRet == BD_PROCEED || nRet != BD_GOOD )
			{
				break;
			}

		}

// 		m_zPos = g_handler.CalcTargetPosition() MOTOR_ROBOT_Z, PLACE, job.iPosFirst, job.iRobotFirst );
		m_zPlace = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PLACE];
		
		
		if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPlace - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
			(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPlace + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
		{
			job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN;
		}
		else
		{
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPlace;
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPlace, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 611, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_PICKER_DOWN:
		{
			//2014,0717
			if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
			{
				for (int i = 0; i < 2; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}
		}

		job.picker_step =PSTEP_PLACE_JIG_FOR_PICKER_DOWN_CHK;
		break;

	case PSTEP_PLACE_JIG_FOR_PICKER_DOWN_RETRY:
		{
			m_zPlace = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PLACE];	
			
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPlace;
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPlace, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 675, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPlace);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
				}
				else
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
				}
				
				job.picker_step = PSTEP_PLACE_JIG_FOR_VACUUM_OFF;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PLACE_JIG_FOR_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 657, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_JIG_FOR_VACUUM_OFF:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_vacuum[i], IO_OFF );
				}
			}
			
			job.picker_step = PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_ON;
		}
		break;

	case PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_ON:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_eject[i], IO_ON );
				}
			}			
			m_dwTime_Robot = GetCurrentTime();
			job.picker_step = PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF;
		}
		break;

	case PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF:
		{
			if( GetCurrentTime() - m_dwTime_Robot < 0 )
			{
				m_dwTime_Robot = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTime_Robot < st_time.n_wait_time[E_WAIT_PICKER_EJECT][IO_ON] )	
				return;

			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
				{
					g_ioMgr.set_out_bit( st_io.o_picker_eject[i], IO_OFF );
				}
			}			

		}		
		job.picker_step = PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK;		
		break;

	case PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK:
		{
			if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[0] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[1] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[2] ) == IO_ON ) break;
			if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[3] ) == IO_ON ) break;

			m_dwTime_Robot = GetCurrentTime();
			job.picker_step = PSTEP_PICK_JIG_FOR_Z_DOWN;
		}
		break;

	case PSTEP_PICK_JIG_FOR_Z_DOWN:
		{
// 			m_zPos = m_dPos[2] = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, PICK, job.iPosFirst, job.iRobotFirst );
			if( GetCurrentTime() - m_dwTime_Robot < 0 )
			{
				m_dwTime_Robot = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTime_Robot < st_time.n_wait_time[E_WAIT_PICKER_DN][IO_ON] )	
				return;

			m_zPickup = m_dPos[2] = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK];
			
			if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPickup - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
				(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPickup + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
			{
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN_SKIP;
			}
			else
			{
				if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPickup;
				int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPickup, st_basic.nRunSpeed);
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 604, CTL_dWARNING, alarm.mstr_code );
				}
			}
			
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_DOWN:
		{
			if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
			{
				for (int i = 0; i < 2; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}
			job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN_CHK;
		}		
		break;
		
	case PSTEP_PICK_JIG_FOR_PICKER_DOWN_SKIP:
// 		if( job.iRobotFirst != -1 )
// 		{
// 			Run_Robot.ReqVacDown( job.iRobotFirst, FORWARD );
// 			if( st_basic.n_pick_type == EPT_2V1M_2USE )
// 			{
// 				Run_Robot.ReqVacDown( job.iRobotFirst + 1, FORWARD );
// 			}
// 		}
// 		
// 		if( job.iRobotSecond != -1 )
// 		{
// 			Run_Robot.ReqVacDown( job.iRobotSecond, FORWARD );
// 			if( st_basic.n_pick_type == EPT_2V1M_2USE )
// 			{
// 				Run_Robot.ReqVacDown( job.iRobotSecond + 1, FORWARD );
// 			}
// 		}
		{
			//2014,0717
			if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
			{
				for (int i = 0; i < 2; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, FORWARD );
					}
				}
			}

		}
		
		job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN_CHK;
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_DOWN_RETRY:
		{
			m_zPickup = m_dPos[2] = st_motor[MOTOR_ROBOT_Z].d_pos[P_XY_JIG_PICK];
			
			if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPickup;
			int nRet = COMI.Start_SingleMove(MOTOR_ROBOT_Z, m_zPickup, st_basic.nRunSpeed);
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 676, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPickup);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
				}
				else
				{
					if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
					if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
				}

				
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_VACUUM_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 688, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_VACUUM_ON:
		{
			for (int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					g_ioMgr.set_out_bit( st_io.o_picker_vacuum[i], IO_ON );
			}
			job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK;
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK:
		//2014,0717
		if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[0] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[1] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[2] ) == IO_OFF) break;
		if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[3] ) == IO_OFF) break;
		job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_UP;
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_UP:
// 		if( job.iRobotFirst != -1 )		
// 		{
// 			Run_Robot.ReqVacDown( job.iRobotFirst, BACKWARD );
// 			if( st_basic.n_pick_type == EPT_2V1M_2USE )
// 			{
// 				Run_Robot.ReqVacDown( job.iRobotFirst + 1, BACKWARD );
// 			}
// 		}
// 		if( job.iRobotSecond != -1 )
// 		{
// 			Run_Robot.ReqVacDown( job.iRobotSecond, BACKWARD );
// 			if( st_basic.n_pick_type == EPT_2V1M_2USE )
// 			{
// 				Run_Robot.ReqVacDown( job.iRobotSecond + 1, BACKWARD );
// 			}
// 		}
		{
			//2014,0717
			if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
			{
				for (int i = 0; i < 2; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, BACKWARD );
					}
				}		
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES)
					{
						Run_Robot.ReqVacDown( i, BACKWARD );
					}
				}		
			}
//			job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_UP_CHK;//2014,1029
			job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_UP_CHK_BEFORE_Z_UP;
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_UP_CHK_BEFORE_Z_UP:
		{
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_UP_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1605, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;


	case PSTEP_PICK_JIG_FOR_PICKER_UP_CHK:
		{
			//2014,0717
			if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
			{
				if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
			}
			else
			{
				if(st_modulemap.nDeviceRobotPicker[3][0] == CTL_YES && Run_Robot.GetVacDown( 0 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][1] == CTL_YES && Run_Robot.GetVacDown( 1 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][2] == CTL_YES && Run_Robot.GetVacDown( 2 ) != FB_NONE ) break;
				if(st_modulemap.nDeviceRobotPicker[3][3] == CTL_YES && Run_Robot.GetVacDown( 3 ) != FB_NONE ) break;
			}			
			job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_Z_UP;
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_Z_UP:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] );
			if( nRet == BD_GOOD )
			{
				//2014,0717
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
				{
					for(int i = 0; i < 2; i++)
					{
						st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
					}
				}
				else
				{
					for(int i = 0; i < 4; i++)
					{
						st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
					}
				}
				job.picker_step = PSTEP_PLACE_POGO_BACK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 605, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PICK_JIG_FOR_PICKER_Z_UP_RETRY;
			}
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_Z_UP_RETRY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				//2014,0717
				if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
				{
					for(int i = 0; i < 2; i++)
					{
						st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
					}
				}
				else
				{
					for(int i = 0; i < 4; i++)
					{
						st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
					}
				}
				job.picker_step = PSTEP_PLACE_POGO_BACK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1606, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;


	case PSTEP_PLACE_POGO_BACK:
		if(st_basic.n_mode_pogo == CTL_YES)
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
			{
				int nSite = job.iPosFirst / 5;
				int nIdx = job.iPosFirst % 5;
				Run_Test_Site[nSite].ReqPogo( nIdx, BACKWARD );
			}
			for(int i = 0; i < 4; i++)
			{
				st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
			}
			job.picker_step = PSTEP_PLACE_POGO_BACK_CHK;
		}
		else
		{
			for(int i = 0; i < 4; i++)
			{
				st_modulemap.nDeviceRobotPicker[3][i] = CTL_NO;
			}
			job.picker_step = PSTEP_PLACE_INSERT_BACK;
		}
		break;
		
	case PSTEP_PLACE_POGO_BACK_CHK:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			if( Run_Test_Site[nSite].GetPogo( nIdx ) != FB_NONE )
				break;
		}
		job.picker_step = PSTEP_PLACE_INSERT_BACK;
		break;

	case PSTEP_PLACE_INSERT_BACK:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			Run_Test_Site[nSite].ReqInsert( nIdx, BACKWARD );
		}
		job.picker_step = PSTEP_PLACE_INSERT_BACK_CHK;
		break;

	case PSTEP_PLACE_INSERT_BACK_CHK:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
			int nSite = job.iPosFirst / 5;
			int nIdx = job.iPosFirst % 5;
			if( Run_Test_Site[nSite].GetInsert( nIdx ) != FB_NONE )
				break;
		}
		job.picker_step = PSTEP_PLACE_PICKER_UP1;
		break;

	case PSTEP_PLACE_PICKER_UP1:
		Run_Robot.ReqGripperDown( BACKWARD );
		Run_Robot.ReqVacDown(0, BACKWARD );
		Run_Robot.ReqVacDown(1, BACKWARD );
		Run_Robot.ReqVacDown(2, BACKWARD );
		Run_Robot.ReqVacDown(3, BACKWARD );

		job.picker_step = PSTEP_PLACE_PICKER_UP1_CHK;
		break;

	case PSTEP_PLACE_PICKER_UP1_CHK:
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;
		if( Run_Robot.GetVacDown(0) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(1) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(2) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(3) != FB_NONE )	break;

		job.picker_step = PSTEP_PLACE_ROBOT_Z_SAFETY;
		if( job.bUpSkip )
			job.picker_step = PSTEP_PLACE_MDL_CALC;
		break;

	case PSTEP_PLACE_ROBOT_Z_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z;//PSTEP_PLACE_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 607, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case PSTEP_PLACE_SITE_FOR_PITCH_Z:
		{//2014,1029
// 			job.picker_step = PSTEP_PLACE_MDL_CALC;//2014,0815
// 			break;
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nManualSpeed );
				int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nRunSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_MDL_CALC;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 695, CTL_dWARNING, alarm.mstr_code );
				}
			}
			else
			{
// 				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nManualSpeed );
				int nRet = COMI.Start_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nRunSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_MDL_CALC;
					//2014,0717
//					if(st_basic.n_pick_type == EPT_1V1M_4USE_2)
//					{
//						job.picker_step = PSTEP_PLACE_
//					}
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 696, CTL_dWARNING, alarm.mstr_code );
				}
			}
		}
		break;		

	case PSTEP_PLACE_MDL_CALC:

		//2016.0703
// 		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
// 			break;

		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
		{
			if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
			{
			}
			else
			{
				break;
			}
		}

		if( Run_Reject_Tray_Transfer.GetMoveToRej() != true ||
			Run_Reject_Tray_Transfer.GetMoveToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToSafety() != true || 
			Run_Reject_Tray_Transfer.GetMoveTransferToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToRej() != true )
		{
			if( job.iRobotFirst != -1 || job.iRobotSecond != -1 || job.iRobotThird != -1 || job.iRobotFourth != -1 )
			{
				break;
			}
		}

		//job.picker_step = PSTEP_PLACE_MOVE_TARGET;
		{
			int nRet = RET_PROCEED;
			m_dPos[0] = g_handler.CalcTargetPosition(MOTOR_ROBOT_X, PLACE, job.iPosFirst, job.iRobotFirst );
			m_dPos[1] = g_handler.CalcTargetPosition(MOTOR_ROBOT_Y, PLACE, job.iPosFirst, job.iRobotFirst );

			double dPos_Cur[2], dMdl[2];
			dPos_Cur[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dPos_Cur[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );


			if(st_handler.mn_mirror_type == CTL_YES)
			{
				dMdl[0] = st_handler.md_safty_rbtx;
				dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT];
				if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_TRAY, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8807, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					

					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;//바로 이동
						}
					}
					else if( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}						
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else if( dPos_Cur[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}
						}
					}
				}
				else if(job.GetPosByTarget() == MPOS_INDEX_REJ)
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_REJECT, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8808, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}

					
					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}
					}
					else if(  dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}
						
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}
						}
					}
				}
				else//TESTSITE, TRAY
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_OUT, ESS_TESTSITE, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8809, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					
					
// 					if( ( dPos_Cur[0] < ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
// 						( dPos_Cur[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
// 					{
// 						job.picker_step = PSTEP_PLACE_MDL_BIN3;
// 					}
					if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//Tray 방향 OK//SOCKET 방향 제한
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_BIN3;
						}
						else
						{
							if( m_dPos[1] >= dPos_Cur[1] )//갈려는 방햔이 현재 보다 크거나 같다면 X safety 이동
							{
								job.picker_step = PSTEP_PLACE_MDL_BIN3;
							}
							else
							{
								if( m_dPos[0] > ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
								{
									job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PLACE_MOVE_TARGET;
								}
							}
						}
					}
					else if( ( dPos_Cur[0] < ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//TRAY3 번 위치
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_BIN3;
						}
						else
						{
							if( m_dPos[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
// 								job.picker_step = PSTEP_PLACE_MDL_BIN3;
								if( m_dPos[0] > ( dMdl[0] - COMI.md_allow_value[MOTOR_ROBOT_X] ) )
								{
									job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PLACE_MDL_BIN3;
								}
							}
							else if( m_dPos[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] + (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}

						}

					}
					else
					{
						job.picker_step = PSTEP_PLACE_MOVE_TARGET;
					}
				}
			}
			else
			{
				dMdl[0] = st_handler.md_safty_rbtx;
				dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT];

				if( job.GetPosByTarget() == MPOS_INDEX_BIN3 )
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_TRAY, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8810, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					

					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다
					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;//바로 이동
						}
					}
					else if( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}						
					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else if( dPos_Cur[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}
						}
					}
				}
				else if( job.GetPosByTarget() == MPOS_INDEX_REJ )
				{
					nRet = Robot_AutoMove_Safety_Zone( ESM_IN, ESS_REJECT, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8811, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}					
					
					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) ) 
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 한다.
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}
					}
					else if(  dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) //Reject 위치 일 것이다 2016.0401
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;//다시 계산 혹시 누가 옮겼을 경우를 대비해서
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MOVE_TARGET;
						}

					}
					else
					{
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
						else
						{
							if( dPos_Cur[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEY_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}
						}
					}
				}			
				else//TESTSITE, TRAY
				{ 
					nRet = Robot_AutoMove_Safety_Zone( ESM_OUT, ESS_TESTSITE, 0);
					if( nRet != RET_GOOD ) 
					{
						if( nRet == RET_ERROR )
						{
							CTL_Lib.Alarm_Error_Occurrence( 8811, CTL_dWARNING, alarm.mstr_code );
						}
						break;
					}

					//if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
					//	( dPos_Cur[1] < ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					//{
					//	job.picker_step = PSTEP_PLACE_MDL_BIN3;
					//}
					if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] > ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) ) &&
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//Tray 방향 OK//SOCKET 방향 제한
						
						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_BIN3;
						}
						else
						{
							if( m_dPos[1] >= dPos_Cur[1] )//갈려는 방햔이 현재 보다 크거나 같다면 X safety 이동
							{
								job.picker_step = PSTEP_PLACE_MDL_BIN3;
							}
							else
							{
								if( m_dPos[0] < ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) )//ybs 20160411
								{
									job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PLACE_MOVE_TARGET;
								}
							}
						}						

					}
					else if( ( dPos_Cur[0] > ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) && 
						( dPos_Cur[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_X] ) ) )
					{//TRAY3 번 위치

						if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
						{
							job.picker_step = PSTEP_PLACE_MDL_BIN3;
						}
						else
						{
							if( m_dPos[0] > (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_2_4] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )//갈려는 방향이 SOCKET 3번보다 큰가? X safety 이동
							{
	// 							job.picker_step = PSTEP_PLACE_MDL_BIN3;
								if( m_dPos[0] < ( dMdl[0] + COMI.md_allow_value[MOTOR_ROBOT_X] ) )
								{
									job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
								}
								else
								{
									job.picker_step = PSTEP_PLACE_MDL_BIN3;
								}
							}
							else if( m_dPos[0] < (st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - (20*COMI.md_allow_value[MOTOR_ROBOT_X]) ) &&
								m_dPos[1] > (st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_3] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
							{
								job.picker_step = PSTEP_PLACE_MDL_MOVEX_START;
							}
							else
							{
								job.picker_step = PSTEP_PLACE_MOVE_TARGET;
							}
						}

					}
					else
					{
						job.picker_step = PSTEP_PLACE_MOVE_TARGET;
					}
				}
			}			
		}
		break;

	case PSTEP_PLACE_MDL_BIN3:
		{//무조건 safety영역으로 빠졌다가 REJECT_Y 가 safety로 빠졌다가 들어간다.
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, st_handler.md_safty_rbtx, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_MDL_CALC;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 7695, CTL_dWARNING, alarm.mstr_code );
				job.picker_step = PSTEP_PLACE_MDL_BIN3;
			}
		}
		break;
		
	case PSTEP_PLACE_MDL_MOVEY_START:
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
			{
				job.picker_step = PSTEP_PLACE_MDL_CALC;
				break;
			}
			else
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					m_dMdl[0] = st_handler.md_safty_rbtx;
					if(job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[1] = m_dPos[1];
					}
					else if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
					{
						m_dMdl[1] = m_dPos[1];
					}
					else
					{
						m_dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					}
				}
				else
				{
					m_dMdl[0] = st_handler.md_safty_rbtx;
					if(job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[1] = m_dPos[1];
					}
					else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
					{
						m_dMdl[1] = m_dPos[1];
					}
					else
					{
						m_dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					}
				}


				long lAxis[2];
				lAxis[0] = MOTOR_ROBOT_X;
				lAxis[1] = MOTOR_ROBOT_Y;

				double dSpeedRatio[3];
				dSpeedRatio[0] = (double)st_basic.nRunSpeed;
				dSpeedRatio[1] = (double)st_basic.nRunSpeed;
				dSpeedRatio[2] = (double)st_basic.nRunSpeed;

				//int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );
				int nRet = COMI.Start_MultiMove( 2, lAxis, m_dMdl, dSpeedRatio );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_MDL_MOVEY_CHECK;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 1610, CTL_dWARNING, alarm.mstr_code );
				}

			}
		}
		break;

	case PSTEP_PLACE_MDL_MOVEX_START:
		{
			if( g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] )
			{
				job.picker_step = PSTEP_PLACE_MDL_CALC;
			}
			else
			{
				//double dMdl[2];
				if(st_handler.mn_mirror_type == CTL_YES)
				{
// 					m_dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100;//COMI.Get_MotCurrentPos( MOTOR_ROBOT_Y );
// 					if(job.GetPosByTarget() == MPOS_INDEX_REJ)
// 					{
// 						m_dMdl[0] = m_dPos[0];
// 					}
// 					else if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
// 					{
// 						m_dMdl[0] = m_dPos[0];
// 					}
// 					else
// 					{
// 						dMdl[0] = m_dPos[0];
// 					}
					//2016.0411
					m_dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100;//COMI.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					if(job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else if(job.GetPosByTarget() == MPOS_INDEX_BIN1)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else
					{
						m_dMdl[0] = m_dPos[0];
					}
				}
				else
				{
					m_dMdl[1] = st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100;//COMI.Get_MotCurrentPos( MOTOR_ROBOT_Y );
					if(job.GetPosByTarget() == MPOS_INDEX_REJ)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else if(job.GetPosByTarget() == MPOS_INDEX_BIN3)
					{
						m_dMdl[0] = m_dPos[0];
					}
					else
					{
						m_dMdl[0] = m_dPos[0];
					}
				}
				
				
				long lAxis[2];
				lAxis[0] = MOTOR_ROBOT_X;
				lAxis[1] = MOTOR_ROBOT_Y;
				
				double dSpeedRatio[3];
				dSpeedRatio[0] = (double)st_basic.nRunSpeed;
				dSpeedRatio[1] = (double)st_basic.nRunSpeed;
				dSpeedRatio[2] = (double)st_basic.nRunSpeed;
				
				//int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dMdl, dSpeedRatio );
				int nRet = COMI.Start_MultiMove( 2, lAxis, m_dMdl, dSpeedRatio );
			
				if( nRet == BD_GOOD )
				{
					m_dwTimeMoveY = GetCurrentTime();
					job.picker_step = PSTEP_PLACE_MDL_MOVEX_CHECK;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 1611, CTL_dWARNING, alarm.mstr_code );
				}
			}


		}
		break;

	case PSTEP_PLACE_MDL_MOVEY_CHECK:
		{//근처에 도달했는지 체크한다.			
			double dMdl[2];
			dMdl[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
			
			if( ( dMdl[1] <= ( st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + 100 ) ) && ( dMdl[1] >= (m_dMdl[1] - 50) ) )
			{
				job.picker_step = PSTEP_PLACE_MOVE_OVERRIDEY_TARGET;
			}
		}
		break;

	case PSTEP_PLACE_MDL_MOVEX_CHECK:
		{//근처에 도달했는지 체크한다.			
			double dMdl[2];
			dMdl[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
			dMdl[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
			
			if( GetCurrentTime() - m_dwTimeMoveY < 0 )
				m_dwTimeMoveY = GetCurrentTime();
			
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if( m_dPos[0] > st_handler.md_safty_rbtx )
				{
					if( dMdl[0] >= ( st_handler.md_safty_rbtx - 100 ) )
					{
						m_dwTimeMoveY = GetCurrentTime();
						job.picker_step = PSTEP_PLACE_MOVE_OVERRIDEX_TARGET;
					}
					else
					{
						if( GetCurrentTime() - m_dwTimeMoveY > 5000)
						{
							COMI.Set_MotStop(0, MOTOR_ROBOT_X);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
					}
				}
				else
				{
					job.picker_step = PSTEP_PLACE_MDL_CALC;
				}

			}
			else
			{
				if( m_dPos[0] < st_handler.md_safty_rbtx )
				{
					if( dMdl[0] <= ( st_handler.md_safty_rbtx + 100 ) )
					{
						m_dwTimeMoveY = GetCurrentTime();
						job.picker_step = PSTEP_PLACE_MOVE_OVERRIDEX_TARGET;
					}
					else
					{
						if( GetCurrentTime() - m_dwTimeMoveY > 5000)
						{
							COMI.Set_MotStop(0, MOTOR_ROBOT_X);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
							COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
							job.picker_step = PSTEP_PLACE_MDL_CALC;
						}
					}
				}
				else
				{
					job.picker_step = PSTEP_PLACE_MDL_CALC;
				}

			}
		}
		break;


	case PSTEP_PLACE_MOVE_OVERRIDEY_TARGET:
		{
			long dwIsStopped = 0;

			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_X, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PLACE_MOVE_X_TARGET;
			}
			else
			{
				int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_X, m_dPos[0]);
				
				if( nRet == BD_GOOD )
				{
					md_MotFlag[0] = CTL_NO;
					md_MotFlag[1] = CTL_NO;
					job.picker_step = PSTEP_PLACE_MOVE_X_TARGET_CHECK;
				}
				else
				{
					alarm.mstr_code = "801000";
					CTL_Lib.Alarm_Error_Occurrence( 6802, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
				}
			}
		}
		break;

	case PSTEP_PLACE_MOVE_OVERRIDEX_TARGET:
		{
			long dwIsStopped = 0;
			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
// 				job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET;
// 			}
// 			else
// 			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					if( m_dPos[0] > 0 &&
						m_dPos[0] > st_handler.md_safty_rbtx &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) >= ( st_handler.md_safty_rbtx - 100 ) )
					{
						md_MotFlag[0] = CTL_NO;
						md_MotFlag[1] = CTL_NO;
						job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET;
						break;
						
						int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);
						
						if( nRet == BD_GOOD )
						{
							job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET_CHECK;
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
							alarm.mstr_code = "801000";
							CTL_Lib.Alarm_Error_Occurrence( 6803, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
						}
					}
				}
				else
				{
					if( m_dPos[0] > 0 &&
						m_dPos[0] < st_handler.md_safty_rbtx &&
						g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= ( st_handler.md_safty_rbtx + 100 ) )
					{
						md_MotFlag[0] = CTL_NO;
						md_MotFlag[1] = CTL_NO;
						job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET;
						break;

						int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);
						
						if( nRet == BD_GOOD )
						{
							job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET_CHECK;
						}
						else
						{
							job.picker_step = PSTEP_PLACE_MDL_CALC;
							alarm.mstr_code = "801000";
							CTL_Lib.Alarm_Error_Occurrence( 6803, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
						}
					}
				}
			}

			/*
			g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped);
			if( dwIsStopped == cmTRUE )
			{
				if( m_dPos[0] < st_handler.md_safty_rbtx &&
					g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ) <= ( st_handler.md_safty_rbtx - 100 ) )
				{
					int nRet = COMI.Set_Override_Move_Pos(MOTOR_ROBOT_Y, m_dPos[1]);
					
					if( nRet == BD_GOOD )
					{
						job.picker_step = PSTEP_PLACE_MOVE_Y_TARGET_CHECK;
					}
					else
					{
						alarm.mstr_code = "999000";
						CTL_Lib.Alarm_Error_Occurrence( 6610, CTL_dWARNING, alarm.mstr_code );//cmmErrShowLast(NULL); // 에러 발생하였으면 에러 디스플레이.
					}
				}
				else
				{
					COMI.Set_MotStop(0, MOTOR_ROBOT_X);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Y);
					COMI.Set_MotStop(0, MOTOR_ROBOT_Z);
					job.picker_step = PSTEP_PLACE_MDL_CALC;

				}
			}*/
		}
		break;


	case PSTEP_PLACE_MOVE_X_TARGET:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, m_dPos[0], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK;//PSTEP_PLACE_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6615, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_MOVE_Y_TARGET:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Y, m_dPos[1], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK;//PSTEP_PLACE_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6616, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_MOVE_X_TARGET_CHECK:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_X, m_dPos[0] );
			if( nRet == BD_GOOD )
			{
				md_MotFlag[0] = CTL_NO;
				md_MotFlag[1] = CTL_NO;
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK;//PSTEP_PLACE_Z_DOWN;
				m_dwTime_Robot = GetCurrentTime();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PLACE_MDL_CALC;
				CTL_Lib.Alarm_Error_Occurrence( 6617, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_MOVE_Y_TARGET_CHECK:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Y, m_dPos[1] );
			int nRet1 = COMI.Check_SingleMove( MOTOR_ROBOT_X, m_dPos[0] );
			if( nRet == BD_GOOD )
				md_MotFlag[0] = CTL_YES;
			if( nRet1 == BD_GOOD )
				md_MotFlag[1] = CTL_YES;

			if( md_MotFlag[0] == CTL_YES && md_MotFlag[1] == CTL_YES)
			{
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK;//PSTEP_PLACE_Z_DOWN;
				m_dwTime_Robot = GetCurrentTime();
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY || nRet1 == BD_ERROR || nRet1 == BD_SAFETY )
			{
				job.picker_step = PSTEP_PLACE_MDL_CALC;
				CTL_Lib.Alarm_Error_Occurrence( 6610, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_MOVE_TARGET:

		//2016.0703
// 		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
// 			break;
		if( g_handler.GetStepTTrans() != ETT_TO_NONE )
		{
			if(Run_Reject_Tray_Transfer.GetMoveToBin() != true && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_ROBOT_MOVE_CHK)
			{
			}
			else
			{
				break;
			}
		}

		if( Run_Reject_Tray_Transfer.GetMoveToRej() != true ||
			Run_Reject_Tray_Transfer.GetMoveToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToSafety() != true || 
			Run_Reject_Tray_Transfer.GetMoveTransferToBin() != true ||
			Run_Reject_Tray_Transfer.GetMoveTransferToRej() != true )
		{
			if( job.iRobotFirst != -1 || job.iRobotSecond != -1 || job.iRobotThird != -1 || job.iRobotFourth != -1 )
			{
				break;
			}
		}
		if( COMI.Get_MotCurrentPos( MOTOR_TRAY_TR_Y ) < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
		{
			job.picker_step = PSTEP_PLACE_MDL_CALC;//2016.0703 PSTEP_PLACE_MDL_BIN3;
		}
		else
		{
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;

			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nRunSpeed;
			dSpeedRatio[1] = (double)st_basic.nRunSpeed;
			dSpeedRatio[2] = (double)st_basic.nRunSpeed;

			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, m_dPos, dSpeedRatio );

			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK;//PSTEP_PLACE_Z_DOWN;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 690, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_SITE_FOR_PITCH_Z_CHK://2014,0805
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{
				int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE] );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 667, CTL_dWARNING, alarm.mstr_code );
					job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_RETRY;
				}
			}
			else
			{
				long dwIsStopped = 0,dwIsStopped1 = 0;
				g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_X, &dwIsStopped);
				g_comiMgr._cmmSxIsDone(MOTOR_ROBOT_Y, &dwIsStopped1);
				if( dwIsStopped == cmTRUE && dwIsStopped1 == cmTRUE )
				{
					int nRet = COMI.Check_SingleMove( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY] );
					if( nRet == BD_GOOD )
					{
						job.picker_step = PSTEP_PLACE_Z_DOWN;
					}
					else if( nRet == BD_ERROR || nRet == BD_SAFETY )
					{
						CTL_Lib.Alarm_Error_Occurrence( 668, CTL_dWARNING, alarm.mstr_code );
						job.picker_step = PSTEP_PLACE_SITE_FOR_PITCH_Z_RETRY;
					}
				}
			}
		}
		break;		
		
	case PSTEP_PLACE_SITE_FOR_PITCH_Z_RETRY://2014,0802
		{
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE)
			{
				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SITE], st_basic.nManualSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 669, CTL_dWARNING, alarm.mstr_code );
				}
			}
			else
			{
				int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_TRAY], st_basic.nManualSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_Z_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 670, CTL_dWARNING, alarm.mstr_code );
				}
			}
		}
		break;

	case PSTEP_PLACE_Z_DOWN:
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE || job.GetPosByTarget() == MPOS_INDEX_REJ)
		{
			if( job.iRobotFirst == -1 && job.iRobotSecond == -1 )
			{
				job.picker_step = PSTEP_PLACE_FINISH;
				break;
			}
		}
		else
		{
			if( job.iRobotFirst == -1 && job.iRobotSecond == -1 && job.iRobotThird == -1 && job.iRobotFourth == -1 )
			{
				job.picker_step = PSTEP_PLACE_FINISH;
				break;
			}
		}

		//2014,0709 체크가 필요함. Testsite에만 가능한지?
		if( job.iRobotFirst == -1 && job.iRobotSecond == -1 )
		{
			job.picker_step = PSTEP_PLACE_FINISH;
			break;
		}

		{
			m_zPos = g_handler.CalcTargetPosition( MOTOR_ROBOT_Z, PLACE, job.iPosFirst, job.iRobotFirst );


			if( (g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) > (m_zPos - COMI.md_allow_value[MOTOR_ROBOT_Z])) &&
				(g_comiMgr.Get_MotCurrentPos(MOTOR_ROBOT_Z) < (m_zPos + COMI.md_allow_value[MOTOR_ROBOT_Z])) )
			{
				job.picker_step = PSTEP_PLACE_PICKER_DOWN;
			}
			else
			{
// 				int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, m_dPos[2], st_basic.nRunSpeed );
				if(COMI.mn_run_status == dRUN) st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos;

				int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPos, st_basic.nRunSpeed );
				if( nRet == BD_GOOD )
				{
					job.picker_step = PSTEP_PLACE_PICKER_DOWN;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CTL_Lib.Alarm_Error_Occurrence( 611, CTL_dWARNING, alarm.mstr_code );
				}
			}


		}
		break;

	case PSTEP_PLACE_PICKER_DOWN:
		if( job.iRobotFirst != -1 )		
		{
			Run_Robot.ReqVacDown( job.iRobotFirst, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, FORWARD );
			}
		}

		if( job.iRobotSecond != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotSecond, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotSecond + 1, FORWARD );
			}
		}
		//2014,0709
		if( job.iRobotThird != -1 )		
		{
			Run_Robot.ReqVacDown( job.iRobotThird, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotThird + 1, FORWARD );
			}
		}

		if( job.iRobotFourth != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFourth, FORWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFourth + 1, FORWARD );
			}
		}


		job.picker_step = PSTEP_PLACE_PICKER_DOWN_CHK;
		break;

	case PSTEP_PLACE_PICKER_DOWN_RETRY://2013,1119
		{
			st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z] = m_zPos;
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, m_zPos, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_PICKER_DOWN_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 677, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_PICKER_DOWN_CHK:
		{
			int nRet = COMI.Check_SingleMove(MOTOR_ROBOT_Z, m_zPos);  // 이동 완료 확인
			if( nRet == BD_GOOD )
			{
				if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst ) != FB_NONE )		break;
				if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond ) != FB_NONE )		break;
				//2014,0709
				if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird ) != FB_NONE )		break;
				if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth ) != FB_NONE )		break;

				if( st_basic.n_pick_type == EPT_2V1M_2USE )
				{
					if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst + 1) != FB_NONE )		break;
					if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond + 1 ) != FB_NONE )		break;
					//2014,0709
					if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird + 1) != FB_NONE )		break;
					if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth + 1 ) != FB_NONE )		break;
				}
				
				job.picker_step = PSTEP_PLACE_VACUUM_OFF;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PLACE_PICKER_DOWN_RETRY;//2013,1119
				CTL_Lib.Alarm_Error_Occurrence( 652, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;

	case PSTEP_PLACE_VACUUM_OFF://2014,2013 2332
		if( job.iRobotFirst != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFirst], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFirst + 1], IO_OFF );
			}
		}
		if( job.iRobotSecond != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotSecond], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotSecond + 1], IO_OFF );
			}
		}
		if( job.iRobotThird != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotThird], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotThird + 1], IO_OFF );
			}
		}
		if( job.iRobotFourth != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFourth], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_vacuum[job.iRobotFourth + 1], IO_OFF );
			}
		}
		job.picker_step = PSTEP_PLACE_VACUUM_EJECTOR_ON;
		break;

	case PSTEP_PLACE_VACUUM_EJECTOR_ON://2014,2013 2332
		if( job.iRobotFirst != -1 )		
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFirst], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFirst + 1], IO_ON );
			}
		}

		if( job.iRobotSecond != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotSecond], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotSecond + 1], IO_ON );
			}
		}
		if( job.iRobotThird != -1 )		
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotThird], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotThird + 1], IO_ON );
			}
		}
		
		if( job.iRobotFourth != -1 )
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFourth], IO_ON );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFourth + 1], IO_ON );
			}
		}
		m_dwTime_Robot = GetCurrentTime();
		job.picker_step = PSTEP_PLACE_VACUUM_EJECTOR_OFF;
		break;//2013,0131

	case PSTEP_PLACE_VACUUM_EJECTOR_OFF://2014,2013 2332
		//2012,1220
		if( GetCurrentTime() - m_dwTime_Robot < 0 )
		{
			m_dwTime_Robot = GetCurrentTime();
		}
		//2015.0819
// 		if( GetCurrentTime() - m_dwTime_Robot < 100 )	
// 			return;
		if( GetCurrentTime() - m_dwTime_Robot < st_time.n_wait_time[E_WAIT_PICKER_EJECT][IO_ON] )	
			return;

		if( job.iRobotFirst != -1 )		
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFirst], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFirst + 1], IO_OFF );
			}
		}
		if( job.iRobotSecond != -1 )	
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotSecond], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotSecond + 1], IO_OFF );
			}
		}
		if( job.iRobotThird != -1 )		
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotThird], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotThird + 1], IO_OFF );
			}
		}
		if( job.iRobotFourth != -1 )	
		{
			g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFourth], IO_OFF );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				g_ioMgr.set_out_bit( st_io.o_picker_eject[job.iRobotFourth + 1], IO_OFF );
			}
		}
		job.picker_step = PSTEP_PLACE_VACUUM_OFF_CHK;
		break;

	case PSTEP_PLACE_VACUUM_OFF_CHK://2014,2013 2332
		if( st_basic.n_mode_device && job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst] ) == IO_ON )		break;
		if( st_basic.n_mode_device && job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond] ) == IO_ON )	break;
		if( st_basic.n_mode_device && job.iRobotThird != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotThird] ) == IO_ON )		break;
		if( st_basic.n_mode_device && job.iRobotFourth != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFourth] ) == IO_ON )	break;

		if( st_basic.n_pick_type == EPT_2V1M_2USE )
		{
			if( st_basic.n_mode_device && job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst + 1] ) == IO_ON )		break;
			if( st_basic.n_mode_device && job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond + 1] ) == IO_ON )	break;
		}
		job.picker_step = PSTEP_PLACE_PICKER_UP2;
		break;

	case PSTEP_PLACE_PICKER_UP2:
		if( job.iRobotFirst != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFirst, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, BACKWARD );
			}
		}
		if( job.iRobotSecond != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotSecond, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotSecond + 1, BACKWARD );
			}
		}
		//2014,0709
		if( job.iRobotThird != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotThird, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFirst + 1, BACKWARD );
			}
		}
		if( job.iRobotFourth != -1 )
		{
			Run_Robot.ReqVacDown( job.iRobotFourth, BACKWARD );
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				Run_Robot.ReqVacDown( job.iRobotFourth + 1, BACKWARD );
			}
		}

		job.picker_step = PSTEP_PLACE_PICKER_UP2_CHK;
 		if( g_handler.IsSkipable_Z_Safety() == false)//2014,1029
 		{
 			job.picker_step = PSTEP_PLACE_PICKER_UP2_BEFORE_Z_UP;//2014,0814
 		}

		break;


	case PSTEP_PLACE_PICKER_UP2_BEFORE_Z_UP://2014,0814
		{
			int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_PICKER_UP2_CHK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 1612, CTL_dWARNING, alarm.mstr_code );
			}
		}		
		break;


	case PSTEP_PLACE_PICKER_UP2_CHK:
		if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst ) != FB_NONE )		break;
		if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond ) != FB_NONE )		break;
		//2014,0709
		if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird ) != FB_NONE )		break;
		if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth ) != FB_NONE )		break;

		if( st_basic.n_pick_type == EPT_2V1M_2USE )
		{
			if( job.iRobotFirst != -1 && Run_Robot.GetVacDown( job.iRobotFirst + 1 ) != FB_NONE )		break;
			if( job.iRobotSecond != -1 && Run_Robot.GetVacDown( job.iRobotSecond + 1 ) != FB_NONE )		break;
			//2014,0709
			if( job.iRobotThird != -1 && Run_Robot.GetVacDown( job.iRobotThird + 1 ) != FB_NONE )		break;
			if( job.iRobotFourth != -1 && Run_Robot.GetVacDown( job.iRobotFourth + 1 ) != FB_NONE )		break;
		}

		job.picker_step = PSTEP_PLACE_Z_UP;
		if( g_handler.IsSkipable_Z_Safety() )
		{
			job.picker_step = PSTEP_PLACE_FINISH;
		}
		break;

	case PSTEP_PLACE_Z_UP:
		{
			int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_DVC_CHECK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				job.picker_step = PSTEP_PLACE_Z_UP_RETRY;
				CTL_Lib.Alarm_Error_Occurrence( 612, CTL_dWARNING, alarm.mstr_code );
			}
		}		
		break;

	case PSTEP_PLACE_Z_UP_RETRY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				job.picker_step = PSTEP_PLACE_DVC_CHECK;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 612, CTL_dWARNING, alarm.mstr_code );
			}
		}		
		break;


	case PSTEP_PLACE_DVC_CHECK:
		job.picker_step = PSTEP_PLACE_FINISH;
		if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
		{
// 			st_io.i_testsite1_dvc1_check							= 1412;				//PS1411
// 			st_io.i_testsite1_dvc2_check							= 1413;				//PS1411
// 			st_io.i_testsite2_dvc1_check							= 1414;				//PS1411
// 			st_io.i_testsite2_dvc2_check							= 1415;				//PS1411

			if(st_basic.n_mode_product == CTL_YES)
			{
				int nSite = job.iPosFirst / 5;
				if(st_basic.n_mode_product_site1 == CTL_YES && nSite == 0)
				{
					if(st_basic.n_mode_product_site1_mode == 1)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite1_dvc1_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 693, dWARNING, "670000" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}
					else if(st_basic.n_mode_product_site1_mode == 2)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite1_dvc2_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 694, dWARNING, "670000" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}
					else if(st_basic.n_mode_product_site1_mode == 3)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite1_dvc1_check, IO_OFF) == IO_ON ||
							g_ioMgr.get_in_bit( st_io.i_testsite1_dvc2_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 695, dWARNING, "670000" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}
				}
				else if(st_basic.n_mode_product_site2 == CTL_YES && nSite == 1)
				{
					if(st_basic.n_mode_product_site2_mode == 1)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite2_dvc1_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 696, dWARNING, "670001" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}
					else if(st_basic.n_mode_product_site2_mode == 2)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite2_dvc2_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 697, dWARNING, "670001" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}
					else if(st_basic.n_mode_product_site2_mode == 3)
					{
						if(g_ioMgr.get_in_bit( st_io.i_testsite2_dvc1_check, IO_OFF) == IO_ON ||
							g_ioMgr.get_in_bit( st_io.i_testsite2_dvc2_check, IO_OFF) == IO_ON)
						{
							CTL_Lib.Alarm_Error_Occurrence( 698, dWARNING, "670001" );
							job.picker_step = PSTEP_PLACE_DVC_CHECK;
							
						}
					}					
				}
			}
		}
		break;

	case PSTEP_PLACE_FINISH:
		{
			AModule* pMdl_1 = NULL;
			AModule* pMdl_2 = NULL;
			AModule* pMdl_3 = NULL;
			AModule* pMdl_4 = NULL;
			int nTrayIdx = -1;
			if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
			{
				int nSite = job.iPosFirst / 5;
				int nIdx = job.iPosFirst % 5;
				pMdl_1 = &g_site.GetModule( job.iPosFirst * 2 );
				pMdl_2 = &g_site.GetModule( job.iPosFirst * 2 + 1);

				//2014,1205
				if(st_basic.mn_mode_xgem == CTL_YES)
				{
					g_client_xgem.SetLotStartStatus("", "");
				}

			}
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN1 )
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PLACE_FINISH1) TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );
				}
				else
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird  % MPOS_DIVISION );
					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN2 )
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PLACE_FINISH2)TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );
				}
				else
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird  % MPOS_DIVISION );
					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}
			}
			//ybs
			else if( job.GetPosByTarget() == MPOS_INDEX_BIN3)
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PLACE_FINISH3)TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst + 1) % MPOS_DIVISION );
// 					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x) % MPOS_DIVISION );
// 					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( (job.iPosFirst - st_basic.n_tray_x + 1) % MPOS_DIVISION );
				}
				else
				{
					pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
					pMdl_2 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosSecond % MPOS_DIVISION );
					pMdl_3 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosThird  % MPOS_DIVISION );
					pMdl_4 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFourth % MPOS_DIVISION );
				}
			}
			else if( job.GetPosByTarget() == MPOS_INDEX_REJ )
			{
				nTrayIdx = g_Tray.GetIdxByPos( TPOS_REJECT );
				//////////////////////////////////////////////////////////////////////////
				//2017.0102
				if(nTrayIdx < 0 || job.iPosFirst < 0 || job.iPosFirst > 3 )
				{
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						m_str_msg.Format("PLACE_FINISH4)TrayIdx :[%d] job.iPosFirst:[%d, %d]", nTrayIdx, job.iPosFirst);
						Func.On_LogFile_Add(99, m_str_msg);
					}
				}
				//////////////////////////////////////////////////////////////////////////

				pMdl_1 = &g_Tray.GetTray( nTrayIdx ).GetModule( job.iPosFirst % MPOS_DIVISION );
				g_Tray.GetTray( nTrayIdx ).SetEmptyTray( false );
				if( g_Tray.GetTray( nTrayIdx ).GetStrLotID() == "" )
					g_Tray.GetTray( nTrayIdx ).SetLotID( g_robot.GetPicker(job.iRobotFirst).GetModule().GetLotNo() );
			}

			if( pMdl_1 == NULL && pMdl_2 == NULL )
			{
				MessageBox( NULL, "ㅠㅠ", "^^", MB_OK );
			}
			
			if( job.iRobotFirst != -1 && pMdl_1)
			{
				*pMdl_1 = g_robot.GetPicker(job.iRobotFirst).GetModule();
				g_robot.GetPicker(job.iRobotFirst).GetModule().EraseData();
				if( job.GetPosByTarget() != MPOS_INDEX_REJ ) //2016.0711
					pMdl_1->CalcModuleState_PlaceTray();
			}
			
			if( job.iRobotSecond != -1 && pMdl_2)
			{
				*pMdl_2 = g_robot.GetPicker(job.iRobotSecond).GetModule();
				g_robot.GetPicker(job.iRobotSecond).GetModule().EraseData();
				pMdl_2->CalcModuleState_PlaceTray();
			}
			//2014,0709
			if( job.iRobotThird != -1 && pMdl_3)
			{
				*pMdl_3 = g_robot.GetPicker(job.iRobotThird).GetModule();
				g_robot.GetPicker(job.iRobotThird).GetModule().EraseData();
				pMdl_3->CalcModuleState_PlaceTray();
			}
			
			if( job.iRobotFourth != -1 && pMdl_4)
			{
				*pMdl_4 = g_robot.GetPicker(job.iRobotFourth).GetModule();
				g_robot.GetPicker(job.iRobotFourth).GetModule().EraseData();
				pMdl_4->CalcModuleState_PlaceTray();
			}

			// UI 업데이트
			if( st_handler.cwnd_main )
			{
				if( nTrayIdx > -1 )	st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_INIT, nTrayIdx );
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_SEL_TRAY);
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDR_PICKER);

				if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);

				if( job.GetPosByTarget() == MPOS_INDEX_BIN1 )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_0 );

				if( job.GetPosByTarget() == MPOS_INDEX_BIN2 )
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_1 );

				if( job.GetPosByTarget() == MPOS_INDEX_BIN3 )//ybs
					st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDT_BINTRAY_2 );
			}
			g_robot.EraseFirstJob();

			g_handler.CalcTrayMoveNext_fromRejFull();
			g_handler.CalcTrayMoveNext_fromBin3();
			g_handler.CalcTrayMoveNext_fromReject();

			g_handler.CalcTrayMoveNext_fromBin();

		}
		
		break;		
	}

}

void CRun_Robot::Serialize(CArchive &ar)
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
		
//		mn_retry_cnt = 0;
	}
}

void CRun_Robot::ERI_Vacuum_Off()
{
	g_ioMgr.set_out_bit( st_io.o_picker1_vacuum, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_picker2_vacuum, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_picker3_vacuum, IO_OFF );
	g_ioMgr.set_out_bit( st_io.o_picker4_vacuum, IO_OFF );

	mn_InitStep = ERI_EJECTOR_ON;
}

void CRun_Robot::ERI_ModuleEmptyChk()
{
	if( g_ioMgr.get_in_bit( st_io.i_picker1_vacuum_chk ) == IO_ON ||
		g_ioMgr.get_in_bit( st_io.i_picker2_vacuum_chk ) == IO_ON ||
		g_ioMgr.get_in_bit( st_io.i_picker3_vacuum_chk ) == IO_ON ||
		g_ioMgr.get_in_bit( st_io.i_picker4_vacuum_chk ) == IO_ON )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CIT000" );
		CTL_Lib.Alarm_Error_Occurrence( 613, dWARNING, "330000" );
		return;
	}

	mn_InitStep = ERI_VACUUM_OFF;
	Func.OnInitBtn( RBTN_ROBOT_MODULE_EMPTY );
}

void CRun_Robot::ERI_PickerVacuumUp()
{
	ReqGripperDown( BACKWARD );
	ReqVacDown( 0, BACKWARD );
	ReqVacDown( 1, BACKWARD );
	ReqVacDown( 2, BACKWARD );
	ReqVacDown( 3, BACKWARD );

	mn_InitStep = ERI_PICKER_VACUUM_UP_CHK;
}

void CRun_Robot::ERI_PickerVacuumUp_Chk()
{
	if( GetGripperDown() != FB_NONE )	return;
	
	if( GetVacDown(0) != FB_NONE )		return;
	if( GetVacDown(1) != FB_NONE )		return;
	if( GetVacDown(2) != FB_NONE )		return;
	if( GetVacDown(3) != FB_NONE )		return;

	mn_InitStep = ERI_PICKER_OFF;
}

void CRun_Robot::ERI_PickerOff()
{
	ReqGripperOn( FORWARD );

	mn_InitStep = ERI_PICKER_OFF_CHK;
}

void CRun_Robot::ERI_PickerOff_Chk()
{
	if( GetGripperOn() != FB_NONE )
		return;

	mn_InitStep = ERI_HOMECHK_Z;
	Func.OnInitBtn( RBTN_ROBOT_PICKER_VACUUM );
}

void CRun_Robot::ERI_HomeChk_Z()
{
	int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_Z, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ERI_HOMECHK_P;
		Func.OnInitBtn( RBTN_ROBOT_Z );

		m_bInitSafety = true;
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HM0Z00");
		CTL_Lib.Alarm_Error_Occurrence(614, CTL_dWARNING, "440300");
	}
}

void CRun_Robot::ERI_HomeChk_P()
{
// 	int nRet = COMI.HomeCheck_Mot( MOTOR_GRIPPER_P, MOT_TIMEOUT);
// 	if( nRet == BD_GOOD )
// 	{
		mn_InitStep = ERI_MOVECHK_P;
// 	}
// 	else if( nRet == BD_ERROR )
// 	{//440400 1 A "로봇 Pitch축 홈체크 에러."
// 		CTL_Lib.Alarm_Error_Occurrence(615, CTL_dWARNING, "440400");
// 	}
}

void CRun_Robot::ERI_MoveChk_P()
{
// 	int nRet = CTL_Lib.Single_Move( MOTOR_GRIPPER_P, st_motor[MOTOR_GRIPPER_P].d_pos[P_P_SAFETY], st_basic.nManualSpeed );
// 	if( nRet == BD_GOOD )
// 	{
		mn_InitStep = ERI_HOMECHK_ZP;//2014,0717 ERI_HOMECHK_X;
		Func.OnInitBtn( RBTN_ROBOT_P );
// 	}
// 	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
// 	{
// 		CTL_Lib.Alarm_Error_Occurrence( 609, CTL_dWARNING, alarm.mstr_code );
// 	}
}

void CRun_Robot::ERI_HomeChk_ZP()
{
	int nRet = COMI.HomeCheck_Mot( MOTOR_PITCH_Z, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ERI_MOVECHK_ZP;
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CTL_Lib.Alarm_Error_Occurrence( 691, CTL_dWARNING, alarm.mstr_code );
	}
}

void CRun_Robot::ERI_MoveChk_ZP()
{
	int nRet = CTL_Lib.Single_Move( MOTOR_PITCH_Z, st_motor[MOTOR_PITCH_Z].d_pos[P_PZ_SAFETY], st_basic.nManualSpeed );
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ERI_HOMECHK_X;//2014,0717 ERI_HOMECHK_X;
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CTL_Lib.Alarm_Error_Occurrence( 692, CTL_dWARNING, alarm.mstr_code );
	}
}

void CRun_Robot::ERI_HomeChk_X()
{
	//2012,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		//2012,0307
		if(st_handler.mn_robot_motorX_safety == CTL_NO)
		{
			if(g_ioMgr.get_in_bit(st_io.i_safety_motorx_chk, IO_ON) == IO_ON)
			{
				st_handler.mn_robot_motorX_safety = CTL_YES;
				cmmSxStop(MOTOR_ROBOT_X, FALSE, TRUE);
			}
			else if(mn_home_x == 0)
			{
				mn_home_x = 1;
				EMOTRET motRet = GETMOTOR( (ENUM_MOTOR)MOTOR_ROBOT_X ).JogMoveStart( PLUS );			
				if( motRet == MOTRET_ERROR )
				{
					cmmSxStop(MOTOR_ROBOT_X, FALSE, TRUE);
					CTL_Lib.Alarm_Error_Occurrence(658, CTL_dWARNING, "440100");
				}
			}
		}
		if(st_handler.mn_robot_motorX_safety != CTL_YES) return;
		int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_Y, MOT_TIMEOUT);
		if( nRet == BD_GOOD )
		{
			mn_InitStep = ERI_HOMECHK_Y;
			Func.OnInitBtn( RBTN_ROBOT_Y );
			
			m_bInitXHome = true;
		}
		else if( nRet == BD_ERROR )
		{
			// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HM0X00");
			CTL_Lib.Alarm_Error_Occurrence(615, CTL_dWARNING, "440100");
		}
	}
	else
	{
		int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_X, MOT_TIMEOUT);
		if( nRet == BD_GOOD )
		{
			mn_InitStep = ERI_HOMECHK_Y;
			Func.OnInitBtn( RBTN_ROBOT_X );
			
			m_bInitXHome = true;
		}
		else if( nRet == BD_ERROR )
		{
			// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HM0X00");
			CTL_Lib.Alarm_Error_Occurrence(615, CTL_dWARNING, "440100");
		}
	}
}

void CRun_Robot::ERI_HomeChk_Y()
{
	//2012,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(st_handler.mn_robot_motorX_safety != CTL_YES || st_handler.mn_robot_motorRej_safety != CTL_YES) return;
		
		int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_X, MOT_TIMEOUT);
		if( nRet == BD_GOOD )
		{
			mn_InitStep = ERI_FINISH;
			Func.OnInitBtn( RBTN_ROBOT_Y );
		}
		else if( nRet == BD_ERROR )
		{
			// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HM0Y00");
			CTL_Lib.Alarm_Error_Occurrence(616, CTL_dWARNING, "440200");
		}
	}
	else
	{
		int nRet = COMI.HomeCheck_Mot( MOTOR_ROBOT_Y, MOT_TIMEOUT);
		if( nRet == BD_GOOD )
		{
			mn_InitStep = ERI_FINISH;
			Func.OnInitBtn( RBTN_ROBOT_Y );
		}
		else if( nRet == BD_ERROR )
		{
//			CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HM0Y00");
			CTL_Lib.Alarm_Error_Occurrence(616, CTL_dWARNING, "440200");
		}
	}
}

void CRun_Robot::ERI_Finish()
{
	st_handler.mn_init_state[INIT_ROBOT] = CTL_YES;
	mn_InitStep = ERI_START;
	Func.OnInitBtn( RBTN_ROBOT );
}

void CRun_Robot::ReqGripperDown( EFB efb )
{
	m_fbGripDown = efb;
	m_step_grip_down = ESTEP_CYL_START;
}

void CRun_Robot::ReqVacDown( int iIdx, EFB efb )
{
	m_fbVacDown[iIdx] = efb;
	m_step_vac_down[iIdx] = ESTEP_CYL_START;
}

void CRun_Robot::Run_Move_Grip_Down()
{
	if( m_fbGripDown == FB_NONE )	return;
	int iOnOff = IO_ON;
	if( m_fbGripDown == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(22, m_step_grip_down);
	switch( m_step_grip_down )
	{
	case ESTEP_CYL_START:	m_step_grip_down = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:
		{
			g_ioMgr.set_out_bit( st_io.o_gripper_updn, iOnOff );
			//2013,0203
			if(COMI.mn_run_status == dRUN) st_work.mn_glipperr_updn = iOnOff;
			m_step_grip_down = ESTEP_CYL_CHK;
			m_dwTimeGripDownStart = GetCurrentTime();
		}
		break;

	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk ) == iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_gripper_up_chk ) != iOnOff )
			{
				m_step_grip_down = ESTEP_CYL_FINISH;
				m_dwTimeGripDown = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeGripDownStart > st_time.n_limit_time[E_WAIT_GRIPPER_DN] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_GripperDn_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 617, dWARNING, (LPCSTR)strJamCode );
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimeGripDownStart < 0 )
			{
				m_dwTimeGripDownStart = GetCurrentTime();
			}

		}
		break;

	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimeGripDown < 0 )
			{
				m_dwTimeGripDown = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeGripDown < st_time.n_wait_time[E_WAIT_GRIPPER_DN][iOnOff] )
				return;

			m_fbGripDown = FB_NONE;
			m_step_grip_down = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Robot::Run_Move_Vac_Down( int iIdx )
{
	if( m_fbVacDown[iIdx] == FB_NONE )	return;
	int iOnOff = IO_ON;
	if( m_fbVacDown[iIdx] == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(24+iIdx, m_step_vac_down[iIdx]);
	switch( m_step_vac_down[iIdx] )
	{
	case ESTEP_CYL_START:	m_step_vac_down[iIdx] = ESTEP_CYL_MOVE; break;
	case ESTEP_CYL_MOVE:	
		{
			int iDown = st_io.o_picker_updn[iIdx];
			g_ioMgr.set_out_bit( iDown, iOnOff );
			//2013,0202
			if(COMI.mn_run_status == dRUN) st_work.mn_picker_updn[iIdx] = iOnOff;
			m_step_vac_down[iIdx] = ESTEP_CYL_CHK;
			m_dwTimeVacDownStart[iIdx] = GetCurrentTime();
		}
		break;

	case ESTEP_CYL_CHK:
		{
			int iUpChk = IO_OFF, iDnChk = IO_OFF;
			iUpChk = st_io.i_picker_up_chk[iIdx];
			iDnChk = st_io.i_picker_dn_chk[iIdx];


			if( g_ioMgr.get_in_bit( iDnChk ) == iOnOff &&
				g_ioMgr.get_in_bit( iUpChk ) != iOnOff )
			{
				m_step_vac_down[iIdx] = ESTEP_CYL_FINISH;
				m_dwTimeVacDown[iIdx] = GetCurrentTime();
				break;
			}
			if( GetCurrentTime() - m_dwTimeVacDownStart[iIdx] > st_time.n_limit_time[E_WAIT_PICKER_DN] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_VacuumDn_Time( iIdx, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 618, dWARNING, (LPCSTR)strJamCode );
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimeVacDownStart[iIdx] < 0 )
			{
				m_dwTimeVacDownStart[iIdx] = GetCurrentTime();
			}
		}
		break;

	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimeVacDown[iIdx] < 0 )
			{
				m_dwTimeVacDown[iIdx] = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeVacDown[iIdx] < st_time.n_wait_time[E_WAIT_PICKER_DN][iOnOff] )
				return;

			m_fbVacDown[iIdx] = FB_NONE;
			m_step_vac_down[iIdx] = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Robot::Run_Move_Grip_On()
{
	if( m_fbGripOn == FB_NONE )	return;
	int iOnOff = IO_ON;
	if( m_fbGripOn == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(23, m_step_grip_on);
	switch( m_step_grip_on )
	{
	case ESTEP_CYL_START:	m_step_grip_on = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:
		{
			g_ioMgr.set_out_bit( st_io.o_gripper, iOnOff );
			m_step_grip_on = ESTEP_CYL_CHK;
			m_dwTimeGripOnStart = GetCurrentTime();
		}
		break;

	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_gripper_on_chk ) != iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_gripper_off_chk ) == iOnOff )
			{
				m_step_grip_on = ESTEP_CYL_FINISH;
				m_dwTimeGripOn = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeGripOnStart > st_time.n_limit_time[E_WAIT_GRIPPER_ON] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_GripperOn_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 619, dWARNING, (LPCSTR)strJamCode );
				}
			}//2012,1220
			else if( GetCurrentTime() - m_dwTimeGripOnStart < 0 )
			{
				m_dwTimeGripOnStart = GetCurrentTime();
			}
		}
		break;

	case ESTEP_CYL_FINISH:
		{//2012,1220
			if( GetCurrentTime() - m_dwTimeGripOn < 0 )
			{
				m_dwTimeGripOn = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeGripOn < st_time.n_wait_time[E_WAIT_GRIPPER_ON][iOnOff] )
				return;

			m_fbGripOn = FB_NONE;
			m_step_grip_on = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Robot::ReqGripperOn( EFB efb )
{
	m_fbGripOn= efb;
	m_step_grip_on = ESTEP_CYL_START;
}

void CRun_Robot::SetCokID( int iID )
{
	AMTRegistry::RegWriteInt( REG_KEY_COK, REG_VAL_ROBOT_COK, iID );
	 m_iCokId = iID;
}

bool CRun_Robot::Run_Move_Chk_Jam( stJobPicker job )
{
	CString strJamCode;

	Func.ThreadFunctionStepTrace(21, job.picker_step);
	switch( job.picker_step )
	{
	case PSTEP_NONE:
	case PSTEP_PICK_READY:
	case PSTEP_PICK_READY_CHK:
		break;

	case PSTEP_PICK_INSERT_BACK:
	case PSTEP_PICK_INSERT_BACK_CHK:
		//if( Chk_Robot_Z_Safety() == false )					return false;
		break;

	case PSTEP_PICK_PICKER_UP1:
	case PSTEP_PICK_PICKER_UP1_CHK:
	case PSTEP_PICK_ROBOT_Z_SAFETY:
		break;

	case PSTEP_PICK_MDL_CALC:
	case PSTEP_PICK_MDL_MOVEY_START:
	case PSTEP_PICK_MOVE_TARGET:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case PSTEP_PICK_Z_DOWN:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;

		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;

		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_OFF ) == false)		return false;
		break;

	case PSTEP_PICK_PICKER_DOWN:
	case PSTEP_PICK_PICKER_DOWN_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
// 		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_OFF ) == false )		return false;
		break;

	case PSTEP_PICK_VACUUM_ON:
	case PSTEP_PICK_VACUUM_ON_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		
		{
			for( int i=0; i<PICKER_CNT; i++ )
			{
				int iOn = IO_OFF;
				if( job.iRobotFirst == i || job.iRobotSecond == i || job.iRobotThird == i || job.iRobotFourth == i )
					iOn = IO_ON;

				if( st_basic.n_pick_type == EPT_2V1M_2USE && (i % 2 == 1) &&
					( job.iRobotFirst == i - 1 || job.iRobotSecond == i - 1  || job.iRobotThird == i - 1  || job.iRobotFourth == i - 1 ) )
				{
					iOn = IO_ON;
				}
				
				if( Chk_Robot_PickerDn( i, iOn ) == false )		return false;
			}
		}
		
		if( Chk_Robot_XY_Pos() == false )					return false;
		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case PSTEP_PICK_PICKER_UP2:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;
	
	case PSTEP_PICK_PICKER_UP2_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
// 		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PICK_Z_UP:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PICK_FINISH:
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
	case PSTEP_PLACE_READY:
	case PSTEP_PLACE_READY_CHK:
	case PSTEP_PLACE_REJECT_Z_CHK:
	case PSTEP_PLACE_REJECT_GUIDE_BACK:
	case PSTEP_PLACE_REJECT_GUIDE_BACK_CHK:
		break;
	case PSTEP_PLACE_REJECT_Z_MOVE:
		if( Run_Reject_Stacker.Chk_Guide( IO_OFF ) == false )	return false;
		break;

	case PSTEP_PLACE_REJECT_GUIDE_FORWARD:
	case PSTEP_PLACE_REJECT_GUIDE_FORWARD_CHK:
		break;

	case PSTEP_PLACE_INSERT_BACK:
	case PSTEP_PLACE_INSERT_BACK_CHK:
		//if( Chk_Robot_Z_Safety() == false )					return false;
		break;

	case PSTEP_PLACE_PICKER_UP1:
	case PSTEP_PLACE_PICKER_UP1_CHK:
	case PSTEP_PLACE_ROBOT_Z_SAFETY:
	case PSTEP_PLACE_JIG_FOR_PICKERUP:
	case PSTEP_PLACE_JIG_FOR_PICKERUP_CHK:
	case PSTEP_PLACE_JIG_FOR_ROBOT_Z_SAFETY:
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_UP:
	case PSTEP_PICK_JIG_FOR_PICKER_UP_CHK:
	case PSTEP_PICK_JIG_FOR_PICKER_Z_UP:
		{
			for(int i = 0; i < 4; i++)
			{
				if(st_modulemap.nDeviceRobotPicker[3][i] == CTL_YES &&
					g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[i] ) != IO_ON )
				{
					CString strJamCode = GetJamCode_ModuleExist_Chk( i, IO_ON );
					CTL_Lib.Alarm_Error_Occurrence( 638, dWARNING, strJamCode );
					return false;
				}

			}
		}
		break;

	case PSTEP_PLACE_MDL_CALC:
	case PSTEP_PLACE_MDL_MOVEY_START:
	case PSTEP_PLACE_MDL_MOVEX_START:
	case PSTEP_PLACE_MOVE_TARGET:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PLACE_Z_DOWN:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;

		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PLACE_PICKER_DOWN:
	case PSTEP_PLACE_PICKER_DOWN_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
// 		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_ON ) == false )		return false;
		break;

	case PSTEP_PLACE_VACUUM_OFF:
	case PSTEP_PLACE_VACUUM_EJECTOR_ON:
	case PSTEP_PLACE_VACUUM_EJECTOR_OFF:
	case PSTEP_PLACE_VACUUM_OFF_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		
		{
			for( int i=0; i<PICKER_CNT; i++ )
			{
				int iOn = IO_OFF;
				//2014,0709
//				if( job.iRobotFirst == i || job.iRobotSecond == i )
				if( job.iRobotFirst == i || job.iRobotSecond == i || job.iRobotThird == i || job.iRobotFourth == i )
					iOn = IO_ON;
				
//				if( st_basic.n_pick_type == EPT_2V1M_2USE &&
//					(i % 2 == 1) &&
//					( job.iRobotFirst == i - 1 || job.iRobotSecond == i - 1 ) )
				//2014,0709
				if( st_basic.n_pick_type == EPT_2V1M_2USE &&
					(i % 2 == 1) &&
					( job.iRobotFirst == i - 1 || job.iRobotSecond == i - 1 || job.iRobotThird == i - 1 || job.iRobotFourth == i - 1 ) )
				{
					iOn = IO_ON;
				}

				if( Chk_Robot_PickerDn( i, iOn ) == false )		return false;
			}
		}
		
		if( Chk_Robot_XY_Pos() == false )					return false;
// 		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case PSTEP_PLACE_PICKER_UP2:
	case PSTEP_PLACE_PICKER_UP2_CHK:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
// 		if( Chk_Robot_Z_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_OFF ) == false )		return false;
		break;

	case PSTEP_PLACE_Z_UP:
		if( Chk_Robot_GripperDn( IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 0, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 1, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 2, IO_OFF ) == false )		return false;
		if( Chk_Robot_PickerDn( 3, IO_OFF ) == false )		return false;
		if( Chk_Robot_XY_Pos() == false )					return false;
		
		if( Chk_Robot_TS_Plate( job, IO_ON) == false )		return false;
		if( Chk_Robot_Insert( job, IO_OFF ) == false )		return false;
		if( Chk_Robot_TS_Lock( job, IO_ON ) == false )		return false;
		if( Chk_Robot_HF_Clamp( job, IO_OFF ) == false )	return false;
		if( Chk_Robot_Cok( job, IO_OFF ) == false )			return false;
		
		if( Chk_Robot_Guide( job, IO_ON) == false )			return false;
		if( Chk_Robot_Pusher( job, IO_ON) == false )		return false;
		if( Chk_Robot_Tray( job, IO_ON) == false )			return false;
		if( Chk_Robot_Rej_Z_Pos( job ) == false )			return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		if( Chk_Robot_Module( job, IO_OFF ) == false )		return false;
		break;

	case PSTEP_PLACE_FINISH:
		break;		
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Z_Safety()
{
	if( st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] + COMI.md_allow_value[MOTOR_ROBOT_Z] < g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "SM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 620, dWARNING, "470000" );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_GripperDn( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_gripper_updn, iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_gripper_dn_chk ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_gripper_up_chk ) != iOn )
	{
		return true;
	}

	CString strJamCode = GetJamCode_GripperDn_Chk( iOn );
	CTL_Lib.Alarm_Error_Occurrence( 621, dWARNING, strJamCode );
	return false;
}

bool CRun_Robot::Chk_Robot_PickerDn( int iIdx, int iOn )
{
	if(	g_ioMgr.get_out_bit( st_io.o_picker_updn[iIdx], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_picker_dn_chk[iIdx] ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_picker_up_chk[iIdx] ) != iOn )
	{
		return true;
	}

	CString strJamCode = GetJamCode_VacuumDn_Chk( iIdx, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 622, dWARNING, strJamCode );
	return false;

}

bool CRun_Robot::Chk_Robot_XY_Pos()
{
	if( abs(m_dPos[0] - g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X )) > COMI.md_allow_value[MOTOR_ROBOT_X] ||
		abs(m_dPos[1] - g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y )) > COMI.md_allow_value[MOTOR_ROBOT_Y] )
	{
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Robot_XY_Pos)  X:[%.3f] Y:[%.3f] m_X:[%.3f] m_Y:[%.3f]", 
				g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X ), g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y ), m_dPos[0], m_dPos[1]);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
		}

// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMXY00" );
		CTL_Lib.Alarm_Error_Occurrence( 623, dWARNING, "464000" );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Z_Pos()
{
	if( abs( m_dPos[2] - g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Z ) ) > COMI.md_allow_value[MOTOR_ROBOT_Z] )
	{
// 		CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PM0Z00" );
		CTL_Lib.Alarm_Error_Occurrence( 624, dWARNING, "460100" );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_TS_Plate( stJobPicker job, int iOn )
{
	if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
	{
		int nSite = job.iPosFirst / 5;

		if( g_ioMgr.get_out_bit( st_io.o_testsite_cok_plate_fwd[nSite], iOn ) == iOn &&
			g_ioMgr.get_out_bit( st_io.o_testsite_cok_plate_bwd[nSite], !iOn ) != iOn &&
			g_ioMgr.get_in_bit( st_io.i_testsite_cok_plate_fwd_chk[nSite], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_testsite_cok_plate_bwd_chk[nSite], !iOn ) != iOn)
		{
			return true;
		}

		CString strJamCode = GetJamCode_TS_Plate_Chk( nSite, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 625, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Insert( stJobPicker job, int iOn )
{
	if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
	{
		int nSite = job.iPosFirst / 5;
		int nIdx = job.iPosFirst % 5;

		int nO_InsertFwd = st_io.o_testsite1_cok_insert_fwdbwd[nIdx];
		int nI_InsertFwdChk = st_io.i_testsite1_cok_insert_fwd_chk[nIdx];
		int nI_InsertBwdChk = st_io.i_testsite1_cok_insert_bwd_chk[nIdx];
		if( nSite == 1 )
		{
			nO_InsertFwd = st_io.o_testsite2_cok_insert_fwdbwd[nIdx];
			nI_InsertFwdChk = st_io.i_testsite2_cok_insert_fwd_chk[nIdx];
			nI_InsertBwdChk = st_io.i_testsite2_cok_insert_bwd_chk[nIdx];
		}

		if( g_ioMgr.get_out_bit( nO_InsertFwd, iOn ) == iOn &&
			g_ioMgr.get_in_bit( nI_InsertFwdChk, iOn ) == iOn &&
			g_ioMgr.get_in_bit( nI_InsertBwdChk, !iOn ) != iOn )
		{
			return true;
		}

		CString strJamCode = GetJamCode_TS_Insert_Chk( nSite, nIdx, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 626, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_TS_Lock( stJobPicker job, int iOn )
{
	if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
	{
		int nSite = job.iPosFirst / 5;

		int nO_TS_Lock = st_io.o_testsite1_cok_lockunlock;
		int nI_TS_LockChk = st_io.i_testsite1_cok_lock_chk;
		int nI_TS_UnlockChk = st_io.i_testsite1_cok_unlock_chk;
		if( nSite == 1 )
		{
			nO_TS_Lock = st_io.o_testsite2_cok_lockunlock;
			nI_TS_LockChk = st_io.i_testsite2_cok_lock_chk;
			nI_TS_UnlockChk = st_io.i_testsite2_cok_unlock_chk;
		}

		if( g_ioMgr.get_out_bit( nO_TS_Lock, iOn ) == iOn &&
			g_ioMgr.get_in_bit( nI_TS_LockChk, iOn ) == iOn &&
			g_ioMgr.get_in_bit( nI_TS_UnlockChk, !iOn ) != iOn )
		{
			return true;
		}

		CString strJamCode = GetJamCode_TS_Lock_Chk( nSite, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 627, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_HF_Clamp( stJobPicker job, int iOn )
{
	if( job.GetPosByTarget() == MPOS_INDEX_TESTSITE )
	{
		int nSite = job.iPosFirst / 5;

		int nO_TS_Clamp = st_io.o_testsite1_hifix_clamp;
		int nI_TS_ClampChk = st_io.i_testsite1_hifix_clamp_chk;
		int nI_TS_HiFixChk = st_io.i_testsite1_hifix_chk; 
		if( nSite == 1 )
		{
			nO_TS_Clamp = st_io.o_testsite2_hifix_clamp;
			nI_TS_ClampChk = st_io.i_testsite2_hifix_clamp_chk;
			nI_TS_HiFixChk = st_io.i_testsite2_hifix_chk;
		}

		if( g_ioMgr.get_out_bit( nO_TS_Clamp, iOn ) == iOn &&
			g_ioMgr.get_in_bit( nI_TS_ClampChk, !iOn ) != iOn &&
			g_ioMgr.get_in_bit( nI_TS_HiFixChk, !iOn ) != iOn )
		{
			return true;
		}

		CString strJamCode = GetJamCode_HF_Clamp_Chk( nSite, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 628, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Cok( stJobPicker job, int iOn )
{
	// 로봇 f,s 에 해당되는 COK 존재 여부 확인
	return true;
}

bool CRun_Robot::Chk_Robot_Guide( stJobPicker job, int iOn )
{
	if( job.iRobotFirst < 0 && job.iRobotSecond )
		return true;
	int nIdx = -1;
	switch( job.GetPosByTarget() )
	{
	case MPOS_INDEX_BIN1:		nIdx = 0;	break;
	case MPOS_INDEX_BIN2:		nIdx = 1;	break;
	case MPOS_INDEX_BIN3:		nIdx = 2;	break;//ybs
	}

	if( nIdx > -1 )
	{
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_guide_fwdbwd[nIdx], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_fwd_chk[nIdx], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_bwd_chk[nIdx], !iOn ) != iOn )
		{		
			return true;
		}

		CString strJamCode = GetJamCode_TrayGuide_Chk( nIdx, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 629, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Pusher( stJobPicker job, int iOn )
{
	if( job.iRobotFirst < 0 && job.iRobotSecond )
		return true;

	int nIdx = -1;
	switch( job.GetPosByTarget() )
	{
	case MPOS_INDEX_BIN1:		nIdx = 0;	break;
	case MPOS_INDEX_BIN2:		nIdx = 1;	break;
	case MPOS_INDEX_BIN3:		nIdx = 2;	break;//ybs
	}

	if( nIdx > -1 )
	{
// 		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_pusher_fwdbwd[nIdx], iOn ) == iOn &&
// 			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_fwd_chk[nIdx], iOn ) == iOn &&
// 			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_bwd_chk[nIdx], !iOn ) != iOn )
// 		{
// 			return true;
// 		}
		//2017.0104
		if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_pusher_fwdbwd[nIdx], iOn ) == iOn &&
			g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[nIdx], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_fwd_chk[nIdx], iOn ) == iOn &&
			g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_bwd_chk[nIdx], !iOn ) != iOn )
		{
			return true;
		}

		CString strJamCode = GetJamCode_TrayPusher_Chk( nIdx, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 630, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Tray( stJobPicker job, int iOn )
{
	if( job.iRobotFirst < 0 && job.iRobotSecond )
		return true;

	int nIdx = -1;
	switch( job.GetPosByTarget() )
	{
	case MPOS_INDEX_BIN1:		nIdx = 0;	break;
	case MPOS_INDEX_BIN2:		nIdx = 1;	break;
	case MPOS_INDEX_BIN3:		nIdx = 2;	break;//ybs
	}

	if( nIdx > -1 )
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_tray_chk[nIdx], iOn ) == iOn )
		{
			return true;
		}

		CString strJamCode = GetJamCode_TrayExist_Chk( nIdx, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 631, dWARNING, strJamCode );
		return false;
	}

	return true;
}

bool CRun_Robot::Chk_Robot_Rej_Z_Pos( stJobPicker job )
{
	if( job.GetPosByTarget() == MPOS_INDEX_REJ )
	{
		double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
		double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
		double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
		int iRejCnt = g_Tray.GetRejCnt();
		if( iRejCnt == 0 ) iRejCnt = 1;
		if( dPos_work < 1.0f )	dPos_work = dPos_1;
		double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(iRejCnt - 1);
		
		if( abs(dTarget - g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_STACKER )) > COMI.md_allow_value[MOTOR_TRAY_STACKER] + 5 )//2012,1225
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "PMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 632, dWARNING, "462000" );
			return false;
		}
	}
	return true;
}

bool CRun_Robot::Run_Move_Chk_Time(stJobPicker job)
{
	bool bRet = true;
// 	if(picker_step_old == PSTEP_PICK_VACUUM_ON_CHK || picker_step_old == PSTEP_PICK_PICKER_UP2 || 
// 		picker_step_old == PSTEP_PICK_PICKER_UP2_CHK || picker_step_old == PSTEP_PICK_Z_UP)
	if(picker_step_old == PSTEP_PICK_VACUUM_ON_CHK || picker_step_old == PSTEP_PICK_Z_UP || picker_step_old == PSTEP_PICK_PICKER_UP2_CHK || picker_step_old == PSTEP_PICK_PICKER_UP2)
	{
 		if( (picker_step_old == PSTEP_PICK_VACUUM_ON_CHK && InTimeRobot( 8000 ) == false) || picker_step_old != PSTEP_PICK_VACUUM_ON_CHK)
 		{
			int nPick=0;
			if(job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst] ) == IO_OFF)
			{
				nPick = job.iRobotFirst + 1;
			}
			else if(job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond] ) == IO_OFF)
			{
				nPick = job.iRobotSecond + 1;
			}
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				if( job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst + 1] ) == IO_OFF )
				{
					nPick = job.iRobotFirst + 1;
				}
				else if( job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond + 1] ) == IO_OFF )
				{
					nPick = job.iRobotSecond + 1;
				}
			}
			//ybs 2012,1128
			int nPos = Func.GetCurrentRbtPos();
			CString strJamCode;
			// 			if (nPos < 0) strJamCode = _T("TITM000");
			// 			else 		  strJamCode.Format("TITM%d",nPos+nPick);
			if (nPos >= 0 && nPick > 0)
			{
				if (nPos < 0) strJamCode = _T("610000");
				else 		  strJamCode.Format("610%03d",nPos+nPick);
				CTL_Lib.Alarm_Error_Occurrence( 633, dWARNING, strJamCode);//"TITM01" );
				bRet = false;
			}
 		}
	}
// 	else if(picker_step_old == PSTEP_PLACE_VACUUM_OFF_CHK || picker_step_old == PSTEP_PLACE_PICKER_UP2 ||
// 		picker_step_old == PSTEP_PLACE_PICKER_UP2_CHK || picker_step_old == PSTEP_PLACE_Z_UP)
	else if(picker_step_old == PSTEP_PLACE_VACUUM_OFF_CHK || picker_step_old == PSTEP_PLACE_Z_UP || picker_step_old == PSTEP_PLACE_PICKER_UP2_CHK || picker_step_old == PSTEP_PLACE_PICKER_UP2)
	{
 		if( (picker_step_old == PSTEP_PLACE_VACUUM_OFF_CHK && InTimeRobot( 10000 ) == false) || picker_step_old != PSTEP_PLACE_VACUUM_OFF_CHK )
 		{
			int nPick=0;
			if(job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst] ) == IO_ON)
			{
				nPick = job.iRobotFirst + 1;
			}
			else if(job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond] ) == IO_ON)
			{
				nPick = job.iRobotSecond + 1;
			}
			if( st_basic.n_pick_type == EPT_2V1M_2USE )
			{
				if( job.iRobotFirst != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst + 1] ) == IO_ON )
				{
					nPick = job.iRobotFirst + 1;
				}
				else if( job.iRobotSecond != -1 && g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond + 1] ) == IO_ON )
				{
					nPick = job.iRobotSecond + 1;
				}
			}
			//ybs 2012,1128
			int nPos = Func.GetCurrentRbtPos();
			CString strJamCode;
			// 			if (nPos < 0) strJamCode = _T("TITP000");
			// 			else 		  strJamCode.Format("TITP%d",nPos+nPick);
			if (nPos > 0 && nPick > 0)
			{
				if (nPos < 0) strJamCode = _T("600000");
				else 		  strJamCode.Format("600%03d",nPos+nPick);
				CTL_Lib.Alarm_Error_Occurrence( 634, dWARNING, strJamCode);
				bRet = false;
			}
			//			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TITM00" );
			//			return false;
 		}
	}
	return bRet;
}

bool CRun_Robot::Chk_Robot_Module( stJobPicker job, int iOn )
{
	if( job.iRobotFirst > -1 )
	{
		if( g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFirst] ) != iOn )
		{
			CString strJamCode = GetJamCode_ModuleExist_Chk( job.iRobotFirst, iOn );
			CTL_Lib.Alarm_Error_Occurrence( 635, dWARNING, strJamCode );
			return false;
		}
	}

	if( job.iRobotSecond > -1 )
	{
		if( g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotSecond] ) != iOn )
		{
			CString strJamCode = GetJamCode_ModuleExist_Chk( job.iRobotSecond, iOn );
			CTL_Lib.Alarm_Error_Occurrence( 636, dWARNING, strJamCode );
			return false;
		}
	}
	//2015.1119
	if( job.iRobotThird > -1 )
	{
		if( g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotThird] ) != iOn )
		{
			CString strJamCode = GetJamCode_ModuleExist_Chk( job.iRobotThird, iOn );
			CTL_Lib.Alarm_Error_Occurrence( 637, dWARNING, strJamCode );
			return false;
		}
	}
	
	if( job.iRobotFourth > -1 )
	{
		if( g_ioMgr.get_in_bit( st_io.i_picker_vacuum_chk[job.iRobotFourth] ) != iOn )
		{
			CString strJamCode = GetJamCode_ModuleExist_Chk( job.iRobotFourth, iOn );
			CTL_Lib.Alarm_Error_Occurrence( 638, dWARNING, strJamCode );
			return false;
		}
	}
	return true;
}

bool CRun_Robot::Run_Move_Chk_Time()
{
	switch( picker_step_old )
	{
	case PSTEP_PICK_ROBOT_Z_SAFETY:
 		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 640, dWARNING, "620100" );
			return false;
 		}
		break;

	case PSTEP_PICK_MDL_MOVEY_START:
	case PSTEP_PICK_MDL_MOVEX_START:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 641, dWARNING, "625000" );
			return false;
 		}
		break;

	case PSTEP_PICK_MOVE_TARGET:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 642, dWARNING, "625000" );
			return false;
 		}
		break;

	case PSTEP_PICK_Z_DOWN:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 643, dWARNING, "620100" );
			return false;
 		}
		break;

	case PSTEP_PICK_VACUUM_ON_CHK:
		if( InTimeRobot( 10000 ) == false )
 		{
 			CTL_Lib.Alarm_Error_Occurrence( 652, dWARNING, "610000" );
			return false;
 		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK:
	case PSTEP_PICK_PICK_JIG_FOR_PICKER_VACUUM_ON_CHK://2013,0517
		if( InTimeRobot( 10000 ) == false )
		{//611000 1 A "지그버퍼사이트에서 자재 들기 제한시간 에러."
			CTL_Lib.Alarm_Error_Occurrence( 653, dWARNING, "611000" );
			return false;
		}
		break;

	case PSTEP_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK:
	case PSTEP_PICK_PLACE_JIG_FOR_VACUUM_EJECTOR_OFF_CHK://2013,0517
		if( InTimeRobot( 10000 ) == false )
		{//6611003 1 A "지그버퍼사이트에서 자재 놓기 제한시간 에러."
			CTL_Lib.Alarm_Error_Occurrence( 654, dWARNING, "6611003" );
			return false;
		}
		break;

	case PSTEP_PLACE_JIG_FOR_PICKER_DOWN_CHK:
	case PSTEP_PICK_JIG_FOR_PICKER_DOWN_CHK:
		if( InTimeRobot( 10000 ) == false )
		{//611001 1 A "지그버퍼사이트에서 Picker down 제한시간 에러."
			CTL_Lib.Alarm_Error_Occurrence( 655, dWARNING, "611001" );
			return false;
		}
		break;

	case PSTEP_PICK_JIG_FOR_PICKER_UP_CHK:
		if( InTimeRobot( 10000 ) == false )
		{//611002 1 A "지그버퍼사이트에서 Picker up 제한시간 에러."
			CTL_Lib.Alarm_Error_Occurrence( 656, dWARNING, "611002" );
			return false;
		}
		break;

	case PSTEP_PICK_Z_UP:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 644, dWARNING, "620100" );
			return false;
 		}
		break;

//////////////////////////////////////////////////////////////////////////

	case PSTEP_PLACE_REJECT_Z_MOVE:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMRS00" );
			CTL_Lib.Alarm_Error_Occurrence( 645, dWARNING, "622000" );
			return false;
 		}
		break;

	case PSTEP_PLACE_ROBOT_Z_SAFETY:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 646, dWARNING, "620100" );
			return false;
 		}
		break;

	case PSTEP_PLACE_MDL_MOVEY_START:
	case PSTEP_PLACE_MDL_MOVEX_START:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 647, dWARNING, "625000" );
			return false;
 		}
		break;

	case PSTEP_PLACE_MOVE_TARGET:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMXY00" );
			CTL_Lib.Alarm_Error_Occurrence( 648, dWARNING, "625000" );
			return false;
 		}
		break;

	case PSTEP_PLACE_Z_DOWN:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 649, dWARNING, "620100" );
			return false;
 		}
		break;

	case PSTEP_PLACE_VACUUM_OFF_CHK:
		if( InTimeRobot( 10000 ) == false )
 		{
 			CTL_Lib.Alarm_Error_Occurrence( 651, dWARNING, "600000" );
			return false;
 		}
		break;

	case PSTEP_PLACE_Z_UP:
		if( InTimeRobot( 10000 ) == false )
 		{
//  			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TM0Z00" );
			CTL_Lib.Alarm_Error_Occurrence( 650, dWARNING, "620100" );
			return false;
 		}
		break;
	}

	return true;
}

bool CRun_Robot::InTimeRobot( int iMilliSec )
{
	DWORD dwtime = GetCurrentTime();
	//2012,1220
	if( GetCurrentTime() - m_dwTime_Robot < 0 )
	{
		m_dwTime_Robot = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_Robot < iMilliSec )
		return true;

	return false;
}

bool CRun_Robot::Chk_Robot_GripperPick( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_gripper, iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_gripper_off_chk ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_gripper_on_chk ) != iOn )
	{
		return true;
	}

	GetJamCode_GripperOn_Chk( iOn );
	return false;
}

void CRun_Robot::OnRobot_FinalPos()
{
	mn_reinstate_step = 0;
}

void CRun_Robot::Run_Reinstate()
{
	if(st_work.mn_reinstate_xyz_robot == CTL_YES) return;
	int i = 0, m_nRecovery = 0;

	switch(mn_reinstate_step)
	{
	case 0:
		m_dcurr_pos[0] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_X);
		m_dcurr_pos[1] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_Y);
		m_dcurr_pos[2] = COMI.Get_MotCurrentPos(MOTOR_ROBOT_Z);
		
		mn_reinstate_step = 100;
		break;

	case 100:
		mn_reinstate_pos[0] = COMI.Check_MotPosRange(MOTOR_ROBOT_X, st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_X], COMI.md_allow_value[MOTOR_ROBOT_X]); 
		mn_reinstate_pos[1] = COMI.Check_MotPosRange(MOTOR_ROBOT_Y, st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y], COMI.md_allow_value[MOTOR_ROBOT_Y]);
		mn_reinstate_pos[2] = COMI.Check_MotPosRange(MOTOR_ROBOT_Z, st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z], COMI.md_allow_value[MOTOR_ROBOT_Z]);
		
		m_nRecovery = 0;
		for(i=0; i<3; i++)
		{
			if(mn_reinstate_pos[i] == CTLBD_RET_ERROR)
			{
				m_nRecovery++;
			}
		}
		
		if(m_nRecovery > 0)
		{
			mn_reinstate_step = 200;
		}
		else
		{
			mn_reinstate_step = 2200;
		}
		break;

	case 200:
		if(mn_reinstate_pos[0] == CTLBD_RET_ERROR || mn_reinstate_pos[1] == CTLBD_RET_ERROR || mn_reinstate_pos[2] == CTLBD_RET_ERROR)
		{
			mn_reinstate_step = 300;
		}
		else
		{
			mn_reinstate_step = 4000;
		}
		break;

	case 300:
		Run_Robot.ReqGripperDown( BACKWARD );
		Run_Robot.ReqVacDown(0, BACKWARD );
		Run_Robot.ReqVacDown(1, BACKWARD );
		Run_Robot.ReqVacDown(2, BACKWARD );
		Run_Robot.ReqVacDown(3, BACKWARD );
		mn_reinstate_step = 400;
		break;

	case 400:
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;
		if( Run_Robot.GetVacDown(0) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(1) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(2) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(3) != FB_NONE )	break;		
		mn_reinstate_step = 500;
		break;

	case 500:
		{	
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], st_basic.nManualSpeed );
			
			if(nRet == CTLBD_RET_GOOD)
			{
				mn_reinstate_step = 1000;
			}
			else if(nRet == CTLBD_RET_RETRY)
			{
				mn_reinstate_step = 500;
			}
			else if(nRet != CTLBD_RET_PROCEED)
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 500;
			}
		}
		break;

	case 1000:
		if(m_dcurr_pos[0] > st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + COMI.md_allow_value[MOTOR_ROBOT_X] + 5.0f)
		{
			mn_reinstate_step = 1100;
		}
		else
		{
			mn_reinstate_step = 2000;
		}
		break;

	case 1100:
		{	
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END], st_basic.nManualSpeed );
			
			if(nRet == CTLBD_RET_GOOD)
			{
				mn_reinstate_step = 1200;
			}
			else if(nRet == CTLBD_RET_RETRY)
			{
				mn_reinstate_step = 1100;
			}
			else if(nRet != CTLBD_RET_PROCEED)
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 1100;
			}
		}
		break;

	case 1200:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END], st_basic.nManualSpeed );
			
			if(nRet == CTLBD_RET_GOOD)
			{
				mn_reinstate_step = 1300;
			}
			else if(nRet == CTLBD_RET_RETRY)
			{
				mn_reinstate_step = 1100;
			}
			else if(nRet != CTLBD_RET_PROCEED)
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 1100;
			}
		}
		break;

	case 1300:
		{//Y부터 이동후 Transfer 이동후 X를 이동한다.
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Y, st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y], st_basic.nManualSpeed );
			
			if(nRet == CTLBD_RET_GOOD)
			{
				mn_reinstate_step = 1400;
			}
			else if(nRet == CTLBD_RET_RETRY)
			{
				mn_reinstate_step = 1100;
			}
			else if(nRet != CTLBD_RET_PROCEED)
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 1100;
			}
		}
		break;

	case 1400:
		st_work.mn_xyz_safety = CTL_YES;
		mn_reinstate_step = 1500;
		break;

	case 1500:
		if(st_work.mn_reinstate_reject_robot == CTL_YES)
		{
			mn_reinstate_step = 2000;
		}
		break;

	case 2000:
		{
			double dPos_Cur[2];
			dPos_Cur[0] = st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_X];
			dPos_Cur[1] = st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Y];
			
			long lAxis[2];
			lAxis[0] = MOTOR_ROBOT_X;
			lAxis[1] = MOTOR_ROBOT_Y;
			
			double dSpeedRatio[3];
			dSpeedRatio[0] = (double)st_basic.nManualSpeed;
			dSpeedRatio[1] = (double)st_basic.nManualSpeed;
			dSpeedRatio[2] = (double)st_basic.nManualSpeed;
			
			int nRet = CTL_Lib.Linear_Move( M_ROBOT_XY_INDEX, 2, lAxis, dPos_Cur, dSpeedRatio );
			if( nRet == BD_GOOD )
			{
				mn_reinstate_step = 2100;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 2000;
			}
		}		
		break;

	case 2100:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_Z, st_work.md_reinstatement_Pos[0][MOTOR_ROBOT_Z], st_basic.nManualSpeed );
			
			if(nRet == CTLBD_RET_GOOD)
			{
				mn_reinstate_step = 2200;
			}
			else if(nRet == CTLBD_RET_RETRY)
			{
				mn_reinstate_step = 2100;
			}
			else if(nRet != CTLBD_RET_PROCEED)
			{
				COMI.mn_run_status = dWARNING;
				mn_reinstate_step = 2100;
			}
		}
		break;

	case 2200:
		{
			for( int i = 0; i < 4; i++)
			{
				if(st_work.mn_picker_updn[i] == IO_ON)
				{
					ReqVacDown( i, FORWARD);
				}
				else
				{
					ReqVacDown( i, BACKWARD);
				}
			}
			if(st_work.mn_glipperr_updn == IO_ON)	ReqGripperDown(FORWARD);
			else                                    ReqGripperDown(BACKWARD);
		}
		mn_reinstate_step = 2300;
		break;

	case 2300:
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;
		if( Run_Robot.GetVacDown(0) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(1) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(2) != FB_NONE )	break;
		if( Run_Robot.GetVacDown(3) != FB_NONE )	break;
		mn_reinstate_step = 5000;
		break;

	case 5000:
		st_work.mn_xyz_safety = CTL_YES;
		st_work.mn_reinstate_xyz_robot = CTL_YES;
		break;
	}
}


int CRun_Robot::Robot_AutoMove_Safety_Zone(int n_mode, int n_site, int n_flag)
{
	int Ret_1=0, Ret_2=0, FuncRet = RET_PROCEED, i=0;
	double dCurrentPosX =0, dCurrentPosY =0, dCurrentPosT=0;	
	//n_mode -> ESM_IN == 0 => 로보트가 REJECT, REJECT, COK로 들어 가는 동작 할 예정 
	//n_mode -> ESM_OUT == 1 => 로보트가 REJECT, TRAY3, COK 이외의 작업을 동작 할 예정
	
	//n_site == 0 => REJECT
	//n_site == 1 => TRAY3
	//n_site == 2 => COK
	
	//n_flag == RESERVED

	dCurrentPosX = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
	dCurrentPosY = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );
	dCurrentPosT = g_comiMgr.Get_MotCurrentPos( MOTOR_TRAY_TR_Y );

	Func.ThreadFunctionStepTrace(28, mn_safetyzone_step);
	switch( mn_safetyzone_step)
	{
	case 0:
		mn_safetyzone_step = 1000;
		break;

	case 1000:
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( dCurrentPosX < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X])
			{
				//Y축이 COK, REJECT, TRAY3 위치와 REJECT 로봇 위치가 COK,REJECT,TRAY 위치에 따른 
				if( n_mode == ESM_IN )
				{//이미 x축이 Safety_limit안으로 들어간 로봇
					mn_safetyzone_step = 2000;
				}
				else//ESM_OUT
				{//나오는 순간
					mn_safetyzone_step = 3000;
				}
			}
			else
			{
				if( n_mode == ESM_IN )//SAFETY_LIMT 밖에서 SAFETY 안으로 들어갈려고 한다
				{
					mn_safetyzone_step = 4000;
				}
				else//SAFETY_LIMT 밖에서 SAFETY_LIMT 밖으로 움직인다
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
			}
		}
		else
		{
			if( dCurrentPosX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				//Y축이 COK, REJECT, TRAY3 위치와 REJECT 로봇 위치가 COK,REJECT,TRAY 위치에 따른 
				if( n_mode == ESM_IN )
				{//이미 x축이 Safety_limit안으로 들어간 로봇
					mn_safetyzone_step = 2000;
				}
				else
				{//나오는 순간
					mn_safetyzone_step = 3000;
				}
			}
			else
			{//현재 안전 위치에 있으나
				if( n_mode == ESM_IN )//SAFETY_LIMT 밖에서 SAFETY 안으로 들어갈려고 한다
				{
					mn_safetyzone_step = 4000;
				}
				else//SAFETY_LIMT 밖에서 SAFETY_LIMT 밖으로 움직인다
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}

				}
			}
		}
		break;

		//////////////////////////////////////////////////////////////////////////
		// 이미 x축이 Safety_limit 안에 들어간 상태
		// Y축이 Safety_limit안으로 들어간 로봇인지와 Transfet로봇 위치 체크
		// 현재 Y축 위치에 따라 COK, REJECT, TRAY3 어디로 가는지 알아야 한다.
		//////////////////////////////////////////////////////////////////////////
	case 2000:
		//Y축의 safety_limit위치에 따라 일단 구분한다

		//1.safety_limit위치 위에 존재할 경우
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( dCurrentPosX < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( n_site == ESS_REJECT )
				{
					//y축이 safety를 벗어났다면 일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}//이미 리젝 안으로 들어간 로봇
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3650, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y])
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3651, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
						else//dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
						{
							mn_safetyzone_step = 5000;//BIn3위치에서 Rejct으로 움직일려고한다.
						}
					}
					else
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3652, CTL_dWARNING, alarm.mstr_code );
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//Bin3위치에서 Rejct으로 움직일려고한다.
						}
					}
				}
				else if( n_site == ESS_TRAY)
				{
					//y축이 safety를 벗어났다면 일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}//2016.0401
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3653, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3654, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;
						}
					}
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3655, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> BIN3
						}
					}
				}
				else  //n_site == ESS_TESTSITE
				{
					
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3656, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}//XY가 Reject tray에 있을 때			
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3657, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;
						}
					}
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )BIN3일때 -> BIN3으로 이동
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3658, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> BIN3
						}
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
			}
		}
		else
		{
			if( dCurrentPosX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_Y] )
			{
				if( n_site == ESS_REJECT )
				{
					//y축이 safety를 벗어났다면 일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}//이미 리젝 안으로 들어간 로봇
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3659, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y])
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3660, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
						else//dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
						{
							mn_safetyzone_step = 5000;//BIn3위치에서 Rejct으로 움직일려고한다.
						}
					}
					else// if( dCurrentPosY < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3661, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//Bin3위치에서 Rejct으로 움직일려고한다.
						}
					}
				}
				else if( n_site == ESS_TRAY)
				{
					//y축이 safety를 벗어났다면 일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}//2016.0401
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3662, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3663, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;
						}
					}
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )BIN3일때 -> BIN3으로 이동
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3664, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> BIN3
						}
					}
				}
				else  //n_site == ESS_TESTSITE 
				{
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3665, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}//XY가 Reject tray에 있을 때			
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3666, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;
						}
					}	
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )BIN3일때 -> BIN3으로 이동
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3667, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> BIN3
						}
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
			}
		}
		break;

		//x축을 safety로 일단 뺀다
	case 2100:
		{//무조건 safety영역으로 빠졌다가 REJECT_Y 가 safety로 빠졌다가 들어간다.
			int nRet = CTL_Lib.Single_Move( MOTOR_ROBOT_X, st_handler.md_safty_rbtx, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				mn_safetyzone_step = 1000;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CTL_Lib.Alarm_Error_Occurrence( 6695, CTL_dWARNING, alarm.mstr_code );
				mn_safetyzone_step = 2100;
				FuncRet = RET_ERROR;
			}
		}
		break;

	case 3000:
		//////////////////////////////////////////////////////////////////////////
		// 이미 x축이 Safety_limit 안에 들어간 상태에서 나오려는 순간
		// 무조건 X축의 safety_limit로 나와야 하는데 간섭구간인지 먼저 확인한다,
		//////////////////////////////////////////////////////////////////////////
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( dCurrentPosX < st_handler.md_safty_rbtx - COMI.md_allow_value[MOTOR_ROBOT_Y] )
			{
				if( n_site == ESS_REJECT )
				{
					//일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] +200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{					
						if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3668, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{					
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3669, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
						else//dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
						{
							mn_safetyzone_step = 5000;//Reject위치에서 밖으로 나갈려고 한다.	
						}
					}
					else// if( dCurrentPosY < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3670, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{//나와도 상관없다
							mn_safetyzone_step = 5000;//Bin3위치에서 밖으로 나갈려고 한다.						
						}
					}
				}
				else if( n_site == ESS_TRAY )
				{
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3671, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_ROBOT_Y ] ) &&
							dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 6698, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
							mn_safetyzone_step = 2100;//안전위치로 이동 후 다시 계산한다.
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;//BIN3 위치에서 밖으로 나갈려고 한다.
						}
					}
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) BIN3일때 -> 벆으로 이동
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{						
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 6699, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> 빡으로 이동
						}
					}
				}
				else//n_site == ESS_TESTSITE
				{
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3672, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}					
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");
							CTL_Lib.Alarm_Error_Occurrence( 3673, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//safety이동 if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;//BIN3에서 밖으로 나간다
						}
					}
					else//XY가 BIN3에 있을 때
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3674, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> 밖으로
						}
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
			}			
		}
		else
		{
			if( dCurrentPosX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( n_site == ESS_REJECT )
				{
					//일단 x축은 safety로 빠진뒤에 동작한다.
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;
					}
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] +200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{					
						if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3675, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
					}
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{					
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3676, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;//안전 위치로 빠진 뒤에 다시 한번위치 체크하여 Reject Y를 움직인다.
						}
						else//dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y])
						{
							mn_safetyzone_step = 5000;//Reject위치에서 밖으로 나갈려고 한다.	
						}
					}
					else// if( dCurrentPosY < ( st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3677, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{//나와도 상관없다
							mn_safetyzone_step = 5000;//Bin3위치에서 밖으로 나갈려고 한다.						
						}
					}
				}
				else if( n_site == ESS_TRAY )
				{
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3678, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_ROBOT_Y ] ) &&
						    dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 6698, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
							mn_safetyzone_step = 2100;//안전위치로 이동 후 다시 계산한다.
						}
						else//if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;//BIN3 위치에서 밖으로 나갈려고 한다.
						}
					}
					else// if( dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] ) BIN3일때 -> 벆으로 이동
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{						
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 6699, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> 빡으로 이동
						}
					}
				}
				else//n_site == ESS_TESTSITE, TRAY
				{
					if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						mn_safetyzone_step = 2100;//다시 계산한다.
					}//Y가 Reject 위치에 있을 때
					else if( dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] &&
						dCurrentPosY < st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + 200*COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3679, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else
						{
							mn_safetyzone_step = 2100;//안전위치로 빠진뒤에 다시 계산한다.
						}
					}					
					else if( dCurrentPosY <= st_handler.md_safty_rbty[P_XY_SAFETY_REJ_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] && 
						dCurrentPosY > st_handler.md_safty_rbty[P_XY_SAFETY_TRAY_LIMIT] + COMI.md_allow_value[MOTOR_ROBOT_Y] )
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT > ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");
							CTL_Lib.Alarm_Error_Occurrence( 3680, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_BIN] + COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 2100;
						}
						else//safety이동 if( dCurrentPosT > (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{
							mn_safetyzone_step = 5000;//BIN3에서 밖으로 나간다
						}
					}
					else//XY가 BIN3에 있을 때
					{
						if( dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) && 
							dCurrentPosT < ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//error 충돌
							alarm.mstr_code = _T("450004");//트랜스퍼 클램프 세이프티 위치 아님. 강제로 움직여야 함.
							CTL_Lib.Alarm_Error_Occurrence( 3681, CTL_dWARNING, alarm.mstr_code );
							mn_safetyzone_step = 0;
							FuncRet = RET_ERROR;
						}
						else if( dCurrentPosT <= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y] ) &&
							dCurrentPosT >= ( st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_REJ] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
						{//REJECT 위치
							Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
							mn_safetyzone_step = 4100;
						}
						else
						{
							mn_safetyzone_step = 5000;//BIN3일때 -> 밖으로
						}
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
			}
		}
		break;

		
		//////////////////////////////////////////////////////////////////////////
		// X축이 안전 위치에 있으나
		// SAFETY_LIMT 밖에서 SAFETY 안으로 들어갈려고 한다
		// Y축이 Safety_limit안으로 들어간 로봇인지와 Transfet로봇 위치 체크
		// 현재 Y축 위치에 따라 COK, REJECT, TRAY3 어디로 가는지 알아야 한다.
		//////////////////////////////////////////////////////////////////////////
	case 4000:
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if( dCurrentPosX > st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_Y] )
			{
				if( n_site == ESS_REJECT )
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
				else if( n_site == ESS_TRAY)
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
				else// if( n_site == ESS_COK)
				{//이부분이 실행될수 없다
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
				break;
			}
			
		}
		else
		{
			if( dCurrentPosX < st_handler.md_safty_rbtx + COMI.md_allow_value[MOTOR_ROBOT_X] )
			{
				if( n_site == ESS_REJECT )
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
				else if( n_site == ESS_TRAY)
				{
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
				else// if( n_site == ESS_COK)
				{//이부분이 실행될수 없다
					if( dCurrentPosT < (st_motor[MOTOR_TRAY_TR_Y].d_pos[P_TRANSFER_Y_SAFETY] - COMI.md_allow_value[MOTOR_TRAY_TR_Y]) )
					{//XY로보트가 움직이기전에 Tray Transfer 로봇을 safety로 움직인다.
						Run_Reject_Tray_Transfer.ReqMoveTransferToSafety();
						mn_safetyzone_step = 4100;
					}
					else
					{
						mn_safetyzone_step = 5000;
					}
				}
			}
			else
			{
				mn_safetyzone_step = 1000;
				break;
			}			
		}


		break;

	case 4100:
		if( Run_Reject_Tray_Transfer.GetMoveTransferToSafety() == true )
		{
			mn_safetyzone_step = 1000;
		}
		break;

	case 5000:
		mn_safetyzone_step = 0;
		FuncRet = RET_GOOD;

		break;
	}

	return FuncRet;
}


int CRun_Robot::Robot_AutoMoveY_Safety_Zone(int n_mode, int n_site, int n_flag)
{
	int Ret_1=0, Ret_2=0, FuncRet = RET_PROCEED, i=0;
	double dCurrentPos =0;
	
	//n_mode == 0 => 로보트가 REJECT, REJECT, COK 로 들어 가는 동작 할 예정 
	//n_mode == 1 => 로보트가 REJECT, TRAY3, COK 이외의 작업을 동작 할 예정
	
	//n_site == 0 => REJECT
	//n_site == 1 => TRAY3
	//n_site == 2 => COK
	
	//n_flag == RESERVED
	switch( mn_safetyzoneY_step)
	{
	case 0:
		break;
	}

	return FuncRet;
}