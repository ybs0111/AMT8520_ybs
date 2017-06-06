// Run_COK_Robot.cpp: implementation of the CRun_COK_Robot class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_COK_Robot.h"

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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CRun_COK_Robot, CObject, 1);	// 복구 동작을 위함...

CRun_COK_Robot	Run_COK_Robot;

CRun_COK_Robot::CRun_COK_Robot()
{	
	Thread_Variable_Initial();
}

CRun_COK_Robot::~CRun_COK_Robot()
{

}

void CRun_COK_Robot::Thread_Variable_Initial()
{
	mn_InitStep = 0;
	mn_RunStep = 0;

	m_step_cokLock = ESTEP_CYL_START;
	m_fbCokLock = FB_NONE;
}

void CRun_COK_Robot::Thread_Run()
{
	switch (COMI.mn_run_status)
	{
	case dINIT:
		Run_Initial();
		break;
		
	case dRUN:
		Run_Move();
		break;
		
	default:
		CTL_Lib.mn_single_motmove_step[MOTOR_COK_ROBOT_Y] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		CTL_Lib.mn_single_motmove_step[MOTOR_COK_ROBOT_Z] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		break;
	}

	Run_Move_CokLock();
}

void CRun_COK_Robot::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[INIT_COK_ROBOT] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case ECI_START:		mn_InitStep = ECI_LOCK;		break;
	case ECI_LOCK:		ECI_Lock();					break;
	case ECI_LOCK_CHK:	ECI_Lock_Chk();				break;
	case ECI_HOMECHK_Z:	ECI_HomeChk_Z();			break;
	case ECI_HOMECHK_Y:	ECI_HomeChk_Y();			break;
	case ECI_FINISH:	ECI_Finish();				break;
	}
}

void CRun_COK_Robot::Run_Move()
{
	
}

void CRun_COK_Robot::Serialize(CArchive &ar)
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


void CRun_COK_Robot::ECI_Lock()
{
	ReqCokLock( FORWARD );

	mn_InitStep = ECI_LOCK_CHK;
}

void CRun_COK_Robot::ECI_Lock_Chk()
{
	if( GetCokLock() != FB_NONE )
		return;


	mn_InitStep = ECI_HOMECHK_Z;
	Func.OnInitBtn( RBTN_COK_LOCK );
}

void CRun_COK_Robot::ECI_HomeChk_Z()
{
	int nRet = COMI.HomeCheck_Mot( MOTOR_COK_ROBOT_Z, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ECI_HOMECHK_Y;
		Func.OnInitBtn( RBTN_COK_Z );
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMCZ00");
		CTL_Lib.Alarm_Error_Occurrence(300, CTL_dWARNING, "441200");
	}
}

void CRun_COK_Robot::ECI_HomeChk_Y()
{
	int nRet = COMI.HomeCheck_Mot( MOTOR_COK_ROBOT_Y, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ECI_FINISH;
		Func.OnInitBtn( RBTN_COK_Y );
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMCY00");
		CTL_Lib.Alarm_Error_Occurrence(301, CTL_dWARNING, "441100");
	}
}

void CRun_COK_Robot::ECI_Finish()
{
	st_handler.mn_init_state[INIT_COK_ROBOT] = CTL_YES;
	mn_InitStep = ECI_START;

	Func.OnInitBtn( RBTN_COK );
}

void CRun_COK_Robot::Run_Move_CokLock()
{
	if( m_fbCokLock == FB_NONE ) return;

	int iOnOff = IO_ON;
	if( m_fbCokLock == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_cokLock )
	{
	case ESTEP_CYL_START:	m_step_cokLock = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:
		{
			g_ioMgr.set_out_bit( st_io.o_cok_stacker_lockunlock, iOnOff );
			m_step_cokLock = ESTEP_CYL_CHK;
			m_dwTimeCokLockStart = GetCurrentTime();
		}
		break;

	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_cok_stacker_lock_chk ) == iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_cok_stacker_unlock_chk ) != iOnOff )
			{
				m_step_cokLock = ESTEP_CYL_FINISH;
				m_dwTimeCokLock = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeCokLockStart > st_time.n_limit_time[E_WAIT_COK_LOCK] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_CokLock( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 302, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeCokLockStart < 0)
			{
				m_dwTimeCokLockStart = GetCurrentTime();
			}
		}
		break;

	case ESTEP_CYL_FINISH:
		{
			//2012,1220
			if(GetCurrentTime() - m_dwTimeCokLock < 0)
			{
				m_dwTimeCokLock = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeCokLock < st_time.n_wait_time[E_WAIT_COK_LOCK][iOnOff] )
				break;

			m_fbCokLock = FB_NONE;
			m_step_cokLock = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_COK_Robot::ReqCokLock( EFB efb )
{
	m_fbCokLock = efb;
	m_step_cokLock = ESTEP_CYL_START;
}
