// Run_Reject_Stacker.cpp: implementation of the CRun_Reject_Stacker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Reject_Stacker.h"

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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CRun_Reject_Stacker, CObject, 1);	// 복구 동작을 위함...

CRun_Reject_Stacker	Run_Reject_Stacker;

CRun_Reject_Stacker::CRun_Reject_Stacker()
{
	Thread_Variable_Initial();
}

CRun_Reject_Stacker::~CRun_Reject_Stacker()
{

}

void CRun_Reject_Stacker::Thread_Variable_Initial()
{
	mn_InitStep = 0;

	m_step_guide = ESTEP_CYL_START;
	m_fbGuide = FB_NONE;

	//2013,1028 China
	m_step_seperator = ESTEP_CYL_START;
	m_fbSeperator = FB_NONE;

	m_nTrayMax = 5;
}

void CRun_Reject_Stacker::Thread_Run()
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
		if (st_handler.mn_menu_num != 501)
		{
			
		}
		
		CTL_Lib.mn_single_motmove_step[MOTOR_TRAY_STACKER] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		break;
	}

	Run_Move_Guide();
	if(CHINA_VER == 1) Run_Move_Seperator();//20131029
}

void CRun_Reject_Stacker::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[INIT_REJECT_STACKER] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case ERSI_START:		mn_InitStep = ERSI_GUIDE_BACK;	break;
	case ERSI_GUIDE_BACK:
		ReqGuide( BACKWARD );
		if(CHINA_VER == 1)
		{
			ReqSeperator( BACKWARD );
		}
		mn_InitStep = ERSI_GUIDE_BACK_CHK;
		break;

	case ERSI_GUIDE_BACK_CHK:
		if(CHINA_VER == 1)
		{
			if( GetSeperator() != FB_NONE )
				return;
		}

		if( GetGuide() != FB_NONE )
			return;

		mn_InitStep = ERSI_HOMECHK_Z;
		break;

	case ERSI_HOMECHK_Z:	ERSI_HomeChk_Z();				break;

	case ERSI_CNT_MOVETO_TRAY_MAX:
		{
			double dPos_1 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_1EA];
			double dPos_5 = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_TRAY_5EA];
			double dPos_work = st_motor[MOTOR_TRAY_STACKER].d_pos[P_REJ_1EA_WORK];
			if( dPos_work < 1.0f )	dPos_work = dPos_1;

			double dTarget = dPos_work + (dPos_5 - dPos_1) / 4.0f * (double)(m_nTrayMax - 1);
			
			int nRet = CTL_Lib.Single_Move( MOTOR_TRAY_STACKER, dTarget, st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				mn_InitStep = ERSI_CNT_CHK_TRAY_MAX;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
// 				CString strJamCode;
// 				strJamCode.Format( "MMRS00" );
// 				CTL_Lib.Alarm_Error_Occurrence( 0, CTL_dWARNING, strJamCode );
				CTL_Lib.Alarm_Error_Occurrence( 500, CTL_dWARNING, alarm.mstr_code );
			}
		}
		break;
		
	case ERSI_CNT_CHK_TRAY_MAX:
		if( g_ioMgr.get_in_bit( st_io.i_reject_elevator_tray_chk ) == IO_ON )
		{
			// 트레이 추가.
			for( int i=0; i<m_nTrayMax; i++ )
			{
				ATray tray;
				tray.SetTargetPos( TPOS_REJECT );
				g_Tray.AddTray( tray );
				g_Tray.TrayMoved( TPOS_NONE, TPOS_REJECT );
			}

			m_nTrayMax = 0;
			mn_InitStep = ERSI_FINISH;
		}
		else
		{
			m_nTrayMax--;
			if( m_nTrayMax == 0 )
				mn_InitStep = ERSI_ZERO_DOWN;
			else
				mn_InitStep = ERSI_CNT_MOVETO_TRAY_MAX;
		}
		break;

	case ERSI_ZERO_DOWN:
		{
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
					mn_InitStep = ERSI_FINISH;
				}
				else if( nRet == BD_ERROR || nRet == BD_SAFETY )
				{
					CString strJamCode;
// 					strJamCode.Format( "MMTC00" );
					strJamCode.Format( "454100" );
					CTL_Lib.Alarm_Error_Occurrence( 501, CTL_dWARNING, strJamCode );
				}
			}
			else
			{
				mn_InitStep = ERSI_FINISH;
			}
		}
		break;

	case ERSI_FINISH:		ERSI_Finish();					break;
	}
}

void CRun_Reject_Stacker::Run_Move()
{
}

void CRun_Reject_Stacker::Serialize(CArchive &ar)
{
	int i = 0, j = 0;
	
	CObject::Serialize(ar);
	
	if(ar.IsStoring())				// 저장하기
	{
		
	}
	else							// 불러오기
	{	

	}
}


void CRun_Reject_Stacker::ERSI_HomeChk_Z()
{
	int nRet = COMI.HomeCheck_Mot( MOTOR_TRAY_STACKER, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = ERSI_CNT_MOVETO_TRAY_MAX;
		Func.OnInitBtn( RBTN_RS_Z );
	}
	else if( nRet == BD_ERROR )
	{
// 		CTL_Lib.Alarm_Error_Occurrence(0, CTL_dWARNING, "HMRS00");
		CTL_Lib.Alarm_Error_Occurrence(502, CTL_dWARNING, "443400");
	}
}

void CRun_Reject_Stacker::ERSI_Finish()
{
	st_handler.mn_init_state[INIT_REJECT_STACKER] = CTL_YES;
	mn_InitStep = ERSI_START;

	Func.OnInitBtn( RBTN_RS_CNT_TRAY );
	Func.OnInitBtn( RBTN_REJECT_STACKER );
}

void CRun_Reject_Stacker::ReqGuide( EFB efb )
{
	m_fbGuide = efb;
	m_step_guide = ESTEP_CYL_START;
}

void CRun_Reject_Stacker::Run_Move_Guide()
{
	if( m_fbGuide == FB_NONE )	return;
	
	int iOnOff = IO_ON;
	if( m_fbGuide == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_guide )
	{
	case ESTEP_CYL_START:	m_step_guide = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			g_ioMgr.set_out_bit( st_io.o_reject_stacker_guide, iOnOff );
			m_step_guide = ESTEP_CYL_CHK;
			m_dwTimeGuideStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_forward_chk ) == iOnOff &&
				g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk ) != iOnOff )
			{
				m_step_guide = ESTEP_CYL_FINISH;
				m_dwTimeGuide = GetCurrentTime();
				break;
			}

			//2013,0104
			st_time.n_limit_time[E_WAIT_REJ_STACKER_GUIDE] = 10000;
			if( GetCurrentTime() - m_dwTimeGuideStart > st_time.n_limit_time[E_WAIT_REJ_STACKER_GUIDE] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Rej_Guide_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 503, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeGuideStart < 0)
			{
				m_dwTimeGuideStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimeGuide < 0)
			{
				m_dwTimeGuide = GetCurrentTime();
			}
			//2013,0104
			st_time.n_wait_time[E_WAIT_REJ_STACKER_GUIDE][iOnOff] = 200;
			if( GetCurrentTime() - m_dwTimeGuide < st_time.n_wait_time[E_WAIT_REJ_STACKER_GUIDE][iOnOff] )
				return;

			m_fbGuide= FB_NONE;
			m_step_guide = ESTEP_CYL_START;
		}
		break;
	}
}

bool CRun_Reject_Stacker::Chk_Guide( int iIO )
{
	CString strJamCode;
	//2013,1119
//	if( g_ioMgr.get_out_bit( st_io.o_reject_stacker_guide, IO_ON ) != iIO ||
//		g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_forward_chk ) != iIO ||
//		g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk ) == iIO )
//	{
//		strJamCode.Format( "29000%d", iIO );
//		CTL_Lib.Alarm_Error_Occurrence( 504, dWARNING, strJamCode );
//		return false;
//	}
	
	if( g_ioMgr.get_out_bit( st_io.o_reject_stacker_guide, !iIO ) != iIO )
	{
		strJamCode.Format( "29000%d", iIO );
		CTL_Lib.Alarm_Error_Occurrence( 504, dWARNING, strJamCode );
		return false;
	}
//	if( g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_forward_chk, 0, 20 ) != iIO )
//	{
//		strJamCode.Format( "29000%d", iIO );
//		CTL_Lib.Alarm_Error_Occurrence( 511, dWARNING, strJamCode );
//		return false;
//	}
//	if(	g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk, 0, 20 ) == iIO )
//	{
//		strJamCode.Format( "29000%d", iIO );
//		CTL_Lib.Alarm_Error_Occurrence( 512, dWARNING, strJamCode );
//		return false;
//	}


	return true;
}

void CRun_Reject_Stacker::ReqSeperator( EFB efb )
{
	m_fbSeperator = efb;
	m_step_seperator = ESTEP_CYL_START;
}

void CRun_Reject_Stacker::Run_Move_Seperator()
{
	if( m_fbSeperator == FB_NONE )	return;
	
	int iOnOff = IO_ON;
	if( m_fbSeperator == BACKWARD )
		iOnOff = IO_OFF;

	switch( m_step_seperator )
	{
	case ESTEP_CYL_START:	m_step_seperator = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			g_ioMgr.set_out_bit( st_io.o_reject_stacker_seperator, iOnOff );
			m_step_seperator = ESTEP_CYL_CHK;
			m_dwTimeSeperatorStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			if( g_ioMgr.get_in_bit( st_io.i_reject_stacker_seperator_backward_chk ) != iOnOff )
			{
				m_step_seperator = ESTEP_CYL_FINISH;
				m_dwTimeGuide = GetCurrentTime();
				break;
			}

			st_time.n_limit_time[E_WAIT_REJ_STACKER_GUIDE] = 10000;
			if( GetCurrentTime() - m_dwTimeSeperatorStart > st_time.n_limit_time[E_WAIT_REJ_STACKER_GUIDE] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Rej_Seperator_Time( iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 505, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeSeperatorStart < 0)
			{
				m_dwTimeSeperatorStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimeSeperator < 0)
			{
				m_dwTimeSeperator = GetCurrentTime();
			}
			//2013,0104
			st_time.n_wait_time[E_WAIT_REJ_STACKER_GUIDE][iOnOff] = 200;
			if( GetCurrentTime() - m_dwTimeSeperator < st_time.n_wait_time[E_WAIT_REJ_STACKER_GUIDE][iOnOff] )
				return;

			m_fbSeperator= FB_NONE;
			m_step_seperator = ESTEP_CYL_START;
		}
		break;
	}
}

bool CRun_Reject_Stacker::Chk_Seperator( int iIO )
{
	if( g_ioMgr.get_out_bit( st_io.o_reject_stacker_seperator, IO_ON ) != iIO ||
		g_ioMgr.get_in_bit( st_io.i_reject_stacker_guide_backward_chk ) == iIO )
	{
		CString strJamCode;
// 		strJamCode.Format( "C0RG0%d", iIO );
		strJamCode.Format( "29001%d", iIO );
		CTL_Lib.Alarm_Error_Occurrence( 506, dWARNING, strJamCode );
		return false;
	}

	return true;
}