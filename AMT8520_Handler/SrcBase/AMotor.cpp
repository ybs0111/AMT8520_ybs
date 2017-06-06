// AMotor.cpp: implementation of the AMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "handler.h"
#include "AMotor.h"
#include "../ComizoaPublic.h"

#include "..\Cmmsdk.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AMotor::AMotor()
{
	m_step = MMSTEP_NONE;
	m_bStopReq = false;
	m_MotMoveMode = MMMODE_NONE;

	m_dMotGotoPos = 0;
	m_dMotGotoSpeed = 1;

	m_nMotorId = -1;
}

AMotor::~AMotor()
{

}

EMOTRET AMotor::MotorMoving_repeat()
{
	EMOTRET ret = MOTRET_PROCEED;

	if( m_bStopReq )
	{
		if( m_step < MMSTEP_STOP )
		{
			m_step = MMSTEP_STOP;
		}
	}

	switch( m_step )
	{
	case MMSTEP_NONE:														break;

	case MMSTEP_MOVE_START:		MmStep_Move_Start();						break;
	case MMSTEP_MOVE_CHECK:		MmStep_Move_Check();						break;
	case MMSTEP_MOVE_FINISH:	MmStep_Move_Finish();	ret = MOTRET_GOOD;	break;

	case MMSTEP_COKY_Z_SAFETY:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Z, st_handler.md_common_teaching[CT_COK_Z_SAFETY], (int)m_dMotGotoSpeed );
			if( nRet == BD_GOOD )
			{
				m_step = MMSTEP_COKY_START;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCZ10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 702, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case MMSTEP_COKY_START:
		{
			int nRet = CTL_Lib.Single_Move( MOTOR_COK_ROBOT_Y, m_dMotGotoPos, (int)m_dMotGotoSpeed );
			if( nRet == BD_GOOD )
			{
				m_step = MMSTEP_COKY_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMCY10" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 703, CTL_dWARNING, alarm.mstr_code );
			}
		}
		
		break;

	case MMSTEP_COKY_FINISH:	MmStep_Move_Finish();	ret = MOTRET_GOOD;	break;

	case MMSTEP_LINEAR_PICKER_UP:
		Run_Robot.ReqVacDown( 3, BACKWARD );
		Run_Robot.ReqVacDown( 2, BACKWARD );
		Run_Robot.ReqVacDown( 1, BACKWARD );
		Run_Robot.ReqVacDown( 0, BACKWARD );
		Run_Robot.ReqGripperDown( BACKWARD );
		m_step = MMSTEP_LINEAR_PICKER_UP_CHK;
		break;

	case MMSTEP_LINEAR_PICKER_UP_CHK:
		if( Run_Robot.GetVacDown( 3 ) != FB_NONE )	break;
		if( Run_Robot.GetVacDown( 2 ) != FB_NONE )	break;
		if( Run_Robot.GetVacDown( 1 ) != FB_NONE )	break;
		if( Run_Robot.GetVacDown( 0 ) != FB_NONE )	break;
		if( Run_Robot.GetGripperDown() != FB_NONE )	break;

		m_step = MMSTEP_LINEAR_SAFETY;
		break;

	case MMSTEP_LINEAR_SAFETY:	MmStep_Linear_Safety();						break;
	case MMSTEP_LINEAR_SAFETY_CHK:	MmStep_Linear_Safety_Chk();				break;
	case MMSTEP_LINEAR_MDL_CALC:	MmStep_Linear_Mdl_Calc();				break;
	case MMSTEP_LINEAR_MDL_START:	MmStep_Linear_Mdl_Start();				break;
	case MMSTEP_LINEAR_MDL_CHK:		MmStep_Linear_Mdl_Chk();				break;
	case MMSTEP_LINEAR_START:	MmStep_Linear_Start();						break;
	case MMSTEP_LINEAR_CHK:		MmStep_Linear_Chk();						break;
	case MMSTEP_LINEAR_FINISH:	MmStep_Linear_Finish();	ret = MOTRET_GOOD;	break;
	
	case MMSTEP_HOME_HOMECHECK:	ret = MmStep_Home_HomeCheck();				break;
	
	case MMSTEP_STOP:			MmStep_Stop();								break;
	case MMSTEP_STOP_FINISH:	MmStep_Stop_Finish();	ret = MOTRET_GOOD;	break;
	
	case MMSTEP_MOVE_FAIL:		MmStep_Move_Fail();		ret = MOTRET_ERROR;	break;
	case MMSTEP_STOP_FAIL:		MmStep_Stop_Fail();		ret = MOTRET_ERROR;	break;
	}

	return ret;
}

void AMotor::MmStep_Move_Start()
{
	int nRet = COMI.Start_SingleMove( m_nMotorId, m_dMotGotoPos, (int)m_dMotGotoSpeed );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_MOVE_CHECK;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Move_Check()
{
	int nRet = COMI.Check_SingleMove( m_nMotorId, m_dMotGotoPos );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_MOVE_FINISH;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Move_Finish()
{
	// "[" + m_str_cmd_motor + "] Motor completed transfer."
	_repeat_Finish();
	_finish_manual();
}

EMOTRET AMotor::MmStep_Home_HomeCheck()
{
	EMOTRET ret = MOTRET_PROCEED;
	int nRet = COMI.HomeCheck_Mot( m_nMotorId, MOT_TIMEOUT);

	if( nRet == MOTRET_GOOD )
	{
		_repeat_Finish();
		_finish_manual();
		ret = MOTRET_GOOD;
	}
	else if( nRet == MOTRET_ERROR )
	{
		_repeat_Finish();
		_finish_manual();
		ret = MOTRET_ERROR;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_NONE;
		_finish_manual();
		ret = MOTRET_ERROR;
	}

	return ret;
}
	
void AMotor::MmStep_Stop()
{
	int nRet = COMI.Set_MotStop( MOTSTOP_SLOWDOWN, m_nMotorId );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_STOP_FINISH;
	}
	else
	{
		// "[" + m_str_cmd_motor + "] Though canceled transfer of motor, error happened."
		m_step = MMSTEP_STOP_FAIL;
	}
}

void AMotor::MmStep_Stop_Finish()
{
	// "[" + m_str_cmd_motor + "] Motor shut down during transfer."
	_repeat_Finish();
	_finish_manual();
}

void AMotor::MmStep_Move_Fail()
{
	// "[" + m_str_cmd_motor + "] Transfer command of motor failed."
	_repeat_Finish();
	_finish_manual();
}
	
void AMotor::MmStep_Stop_Fail()
{
	// "[" + m_str_cmd_motor + "] Transfer command of motor failed."
	_repeat_Finish();
	_finish_manual();
}

void AMotor::_repeat_Finish()
{
	if( m_bStopReq )
	{
		m_bStopReq = false;
	}

	m_step = MMSTEP_NONE;
}

void AMotor::_finish_manual()
{
	if( m_MotMoveMode == MMMODE_MANUAL ||
		m_MotMoveMode == MMMODE_HOME )
	{
		if( st_handler.cwnd_motorAxis )
		{
			st_handler.cwnd_motorAxis->PostMessage( WM_MOTOR_COMPLETION, m_MotMoveMode, 0 );
		}
		if( st_handler.cwnd_motor_with_io )
			st_handler.cwnd_motor_with_io->PostMessage( WM_MOTOR_COMPLETION );

		m_MotMoveMode = MMMODE_NONE;
	}
	else if( m_MotMoveMode == MMMODE_WITH_IO_MOVE )
	{
		if( st_handler.cwnd_motor_with_io )
			st_handler.cwnd_motor_with_io->PostMessage( WM_MOTOR_COMPLETION );
	}
}

void AMotor::MotorStepClear()
{
	m_bStopReq = false;
	m_step = MMSTEP_NONE;
}

void AMotor::MotorMoveStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode )
{
	m_dMotGotoPos = dPos;
	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_MOVE_START;
}

void AMotor::MotorHomeStart()
{
	m_MotMoveMode = MMMODE_HOME;
	m_step = MMSTEP_HOME_HOMECHECK;
}

EMOTRET AMotor::JogMoveStart( long lDir )
{
	COMI.Set_MotSpeed(MOT_SPD_JOG, m_nMotorId, cmSMODE_KEEP, COMI.md_spd_jog[m_nMotorId], COMI.md_spd_vel[m_nMotorId][1], COMI.md_spd_vel[m_nMotorId][2]);		// 축의 속도를 변경한다.
	
	long nRet = cmmSxVMoveStart(m_nMotorId, lDir);
	if ( nRet != cmERR_NONE)
	{ 
		return MOTRET_ERROR;
	}
	
	return MOTRET_GOOD;
}

void AMotor::MmStep_Linear_Safety()
{
	int nRet = COMI.Start_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY], (int)m_dMotGotoSpeed );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_SAFETY_CHK;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Safety_Chk()
{
	int nRet = COMI.Check_SingleMove( MOTOR_ROBOT_Z, st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SAFETY] );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_MDL_CALC;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Mdl_Calc()
{
	m_step = MMSTEP_LINEAR_START;

	double dPos[2], dMdl[2];
	dPos[0] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_X );
	dPos[1] = g_comiMgr.Get_MotCurrentPos( MOTOR_ROBOT_Y );

	//ybs 2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 5.0f;
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		if( (dPos[0] < dMdl[0] && m_dLinearPos[0] > dMdl[0]) ||
			(dPos[0] > dMdl[0] && m_dLinearPos[0] < dMdl[0] ) )
		{
			double dx = m_dLinearPos[0] - dPos[0];
			double dy = m_dLinearPos[1] - dPos[1];
			
			if( dx == 0 )
			{
				return;
			}
			
			//2013,0225
			//if( dy / dx * (dMdl[0] - dPos[0]) + dPos[1] < dMdl[1] )
			m_step = MMSTEP_LINEAR_MDL_START;
		}
	}
	else
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 5.0f;
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		if( (dPos[0] < dMdl[0] && m_dLinearPos[0] > dMdl[0]) ||
			(dPos[0] > dMdl[0] && m_dLinearPos[0] < dMdl[0] ) )
		{
			double dx = m_dLinearPos[0] - dPos[0];
			double dy = m_dLinearPos[1] - dPos[1];
			
			if( dx == 0 )
			{
				return;
			}
			
			if( dy / dx * (dMdl[0] - dPos[0]) + dPos[1] < dMdl[1] )
				m_step = MMSTEP_LINEAR_MDL_START;
		}
	}
}

void AMotor::MmStep_Linear_Mdl_Start()
{
	double dMdl[2];
	//ybs 2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 5.0f;
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		if(m_dLinearPos[1] > (dMdl[1] - 5.0f))
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		}
		else
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END])/2;
		}
	}
	else
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 5.0f;
		
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		//2013,0131
		if(m_dLinearPos[1] > (dMdl[1] - 5.0f))
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		}
		else
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
		}
	}

	COMI.ml_axiscnt[M_ROBOT_XY_INDEX] = 2;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][0] = MOTOR_ROBOT_X;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][1] = MOTOR_ROBOT_Y;

	double dSppedRatio[3];
	dSppedRatio[0] = m_dMotGotoSpeed;				// 1/10 속도로 복귀 한다.
	dSppedRatio[1] = m_dMotGotoSpeed;
	dSppedRatio[2] = m_dMotGotoSpeed;
	int nRet = COMI.Start_LinearMove(M_ROBOT_XY_INDEX, dMdl, dSppedRatio );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_MDL_CHK;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Mdl_Chk()
{
	double dMdl[2];
	//ybs 2013,0225
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] - 5.0f;
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		//2013,0131
		if(m_dLinearPos[1] > (dMdl[1] - 5.0f))
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		}
		else
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END])/2;
		}
	}
	else
	{
		dMdl[0] = st_motor[ MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] + 5.0f;
		dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		
		//2013,0131
		if(m_dLinearPos[1] > (dMdl[1] - 5.0f))
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_REJ_END];
		}
		else
		{
			dMdl[1] = st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END] + (st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] - st_motor[ MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_END])/2;
		}
	}

	int nRet = COMI.Check_LinearMove( M_ROBOT_XY_INDEX, dMdl );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_START;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Start()
{
	COMI.ml_axiscnt[M_ROBOT_XY_INDEX] = 2;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][0] = MOTOR_ROBOT_X;
	COMI.mp_axisnum[M_ROBOT_XY_INDEX][1] = MOTOR_ROBOT_Y;

	double dSppedRatio[3];
	dSppedRatio[0] = m_dMotGotoSpeed;				// 1/10 속도로 복귀 한다.
	dSppedRatio[1] = m_dMotGotoSpeed;
	dSppedRatio[2] = m_dMotGotoSpeed;
	int nRet = COMI.Start_LinearMove(M_ROBOT_XY_INDEX, m_dLinearPos, dSppedRatio );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_CHK;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Chk()
{
	int nRet = COMI.Check_LinearMove( M_ROBOT_XY_INDEX, m_dLinearPos );
	if( nRet == MOTRET_GOOD )
	{
		m_step = MMSTEP_LINEAR_FINISH;
	}
	else if( nRet == MOTRET_ERROR )
	{
		// "[" + m_str_cmd_motor + "] Motor failed to move."
		m_step = MMSTEP_MOVE_FAIL;
	}
	else if( nRet == MOTRET_SAFETY )
	{
		// "[SAFETY] Do not move being thought that is not safe at relevant motor difference."
		m_step = MMSTEP_MOVE_FAIL;
	}
}

void AMotor::MmStep_Linear_Finish()
{
	_repeat_Finish();
	_finish_manual();
}

void AMotor::MotorLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode /*= MMMODE_NONE */ )
{
	m_dLinearPos[0] = dPosX;
	m_dLinearPos[1] = dPosY;

	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_LINEAR_PICKER_UP;
}

void AMotor::MotorCokYStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode /*= MMMODE_NONE */ )
{
	m_dMotGotoPos = dPos;
	m_dMotGotoSpeed = dSpeed;
	m_MotMoveMode = motMoveMode;

	m_step = MMSTEP_COKY_Z_SAFETY;
}

bool AMotor::CheckTeachSafety( int nAxis, int nPos, double dVal )
{
	if( nAxis == MOTOR_ROBOT_X )
	{
		if( nPos == P_XY_SITE1_START || 
			nPos == P_XY_SITE1_END || 
			nPos == P_XY_SITE_1_2 ||
			nPos == P_XY_SITE_1_3 || 
			nPos == P_XY_SITE_1_4 )
		{
			if( nPos != P_XY_SITE1_START && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - dVal ) > 10.0f )		return false;
			if( nPos != P_XY_SITE1_END && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_END] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_1_2 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_2] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_1_3 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_3] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_1_4 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_1_4] - dVal ) > 10.0f )			return false;
		}

		if( nPos == P_XY_SITE2_START || 
			nPos == P_XY_SITE2_END || 
			nPos == P_XY_SITE_2_2 ||
			nPos == P_XY_SITE_2_3 || 
			nPos == P_XY_SITE_2_4 )
		{
			if( nPos != P_XY_SITE2_START && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_START] - dVal ) > 10.0f )		return false;
			if( nPos != P_XY_SITE2_END && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE2_END] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_2_2 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_2] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_2_3 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_3] - dVal ) > 10.0f )			return false;
			if( nPos != P_XY_SITE_2_4 && abs( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE_2_4] - dVal ) > 10.0f )			return false;
		}

		if( nPos == P_XY_REJ_START ||
			nPos == P_XY_REJ_END || 
			nPos == P_XY_REJ_CALC )
		{
			//2013,0225
			if (st_handler.mn_mirror_type == CTL_YES)
			{
				if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_START] < dVal )	
					return false;
			}
			else
			{
				if( st_motor[MOTOR_ROBOT_X].d_pos[P_XY_TRAY2_END] > dVal )	
					return false;
			}
		}
	}
	else if( nAxis == MOTOR_ROBOT_Y )
	{
//		if( nPos == P_XY_TRAY1_START ||
//			nPos == P_XY_TRAY1_END ||
//			nPos == P_XY_TRAY1_CALC ||
//			nPos == P_XY_TRAY2_START ||
//			nPos == P_XY_TRAY2_END ||
//			nPos == P_XY_TRAY2_CALC )
//		{
//			if( nPos != P_XY_TRAY1_START && st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 50.0f < dVal )					return false;
//			if( nPos != P_XY_TRAY2_START && st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + 50.0f < dVal )					return false;
//			if( st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] - 50.0f < dVal )													return false;
//			if( st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] - 50.0f < dVal )													return false;
//		}
//ybs
		if( nPos == P_XY_TRAY1_START ||
			nPos == P_XY_TRAY1_END ||
			nPos == P_XY_TRAY1_CALC ||
			nPos == P_XY_TRAY2_START ||
			nPos == P_XY_TRAY2_END ||
			nPos == P_XY_TRAY2_CALC ||
			nPos == P_XY_TRAY3_START ||
			nPos == P_XY_TRAY3_END ||
			nPos == P_XY_TRAY3_CALC )
		{
			if( nPos != P_XY_TRAY1_START && st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START] + 50.0f < dVal )					return false;
			if( nPos != P_XY_TRAY2_START && st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY2_START] + 50.0f < dVal )					return false;
			if( nPos != P_XY_TRAY3_START && st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY3_START] + 50.0f < dVal )					return false;
			if( st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_END] - 50.0f < dVal )													return false;
			if( st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE2_END] - 50.0f < dVal )													return false;
		}

	}

	return true;
}



