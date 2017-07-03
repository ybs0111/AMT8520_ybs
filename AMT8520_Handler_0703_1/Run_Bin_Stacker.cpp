// Run_Bin_Stacker.cpp: implementation of the CRun_Bin_Stacker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Bin_Stacker.h"
#include "ComizoaPublic.h"
#include "IO_Manager.h"
#include "SrcPart/APartTray.h"
#include "SrcPart/APartNetworkManager.h"
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
IMPLEMENT_SERIAL(CRun_Bin_Stacker, CObject, 1);	// 복구 동작을 위함...

CRun_Bin_Stacker Run_Bin_Stacker[BIN_STACKER_CNT];

CRun_Bin_Stacker::CRun_Bin_Stacker()
{
	Thread_Variable_Initial();
}

CRun_Bin_Stacker::~CRun_Bin_Stacker()
{

}

void CRun_Bin_Stacker::Thread_Variable_Initial()
{
	mn_InitStep = 0;
	mn_RunStep = 0;
	mn_CokChgStep = 0;

	m_step_recvTray = RECV_TRAY_NONE;
	m_step_TrayUp = TRAY_UP_NONE;
	m_step_TrayDown = TRAY_DOWN_NONE;
	m_step_TraySend = TRAY_SEND_NONE;

	m_step_recvTray_old = RECV_TRAY_NONE;
	m_step_TrayUp_old = TRAY_UP_NONE;
	m_step_TrayDown_old = TRAY_DOWN_NONE;
	m_step_TraySend_old = TRAY_SEND_NONE;


	m_step_align = ESTEP_CYL_START;
	m_fbAlign = FB_NONE;

	m_step_stopper = ESTEP_CYL_START;
	m_fbStopper = FB_NONE;

	m_step_clamp = ESTEP_CYL_START;
	m_fbClamp = FB_NONE;

	m_step_guide = ESTEP_CYL_START;
	m_fbGuide = FB_NONE;

	m_step_pusher = ESTEP_CYL_START;
	m_fbPusher = FB_NONE;

// 	m_nfront_smema = CTL_NO;

	m_step_ElvBase = ESTEP_CYL_START;
	m_fbElvBase = FB_NONE;

	for (int i = 0; i < 3; i++)
	{
		st_work.mn_tray_pusher[i] = IO_OFF;
		st_work.mn_tray_guide[i] = IO_OFF;
		st_handler.mn_cok_chgstate[i] = CTL_YES;
	}
}

void CRun_Bin_Stacker::Thread_Run()
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
		OnBinStacker_FinalPos();
		CTL_Lib.mn_single_motmove_step[m_nMotor_Z] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		CTL_Lib.mn_single_motmove_step[m_nMotor_R] = 0;				// 정지나 알람이 발생하면 싱글 무브 스텝은 0으로 복귀해서 다시 시작할 준비를 한다.
		m_step_recvTray_old = RECV_TRAY_NONE;
		m_step_TrayUp_old = TRAY_UP_NONE;
		m_step_TrayDown_old = TRAY_DOWN_NONE;
		m_step_TraySend_old = TRAY_SEND_NONE;
		break;
	}

	//ybs 2012,1128
	if(m_nSiteID == 0 && m_step_recvTray >= RECV_TRAY_READY && m_step_recvTray <= RECV_TRAY_DIRECTION_CHK)
	{
		Run_Move_RecvTray();
	}
	//if(m_nSiteID == 2 && g_handler.GetMachinePos() == EMPOS_FRONT && m_step_TraySend >= TRAY_SEND_NEXT_CHK)
	if(m_nSiteID == 2 && m_step_TraySend >= TRAY_SEND_READY && m_step_TraySend <= TRAY_SEND_BUFFER_IN_CHK)
	{
		Run_Move_TraySend();
	}
	Run_Move_Align();
	Run_Move_Stopper();
	Run_Move_Clamp();
	Run_Move_Guide();
	Run_Move_Pusher();
	Run_MoveElvRotateBase();//2013,0131
}

void CRun_Bin_Stacker::Run_Initial()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_init_state[m_nSiteID + INIT_BIN_STACKER_1] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_InitStep)
	{
	case EBSI_START:					mn_InitStep = EBSI_CONVEYOR_STOP;	
		//if( st_handler.mn_virtual_supply )	mn_InitStep = EBSI_FINISH;
		break;

	case EBSI_CONVEYOR_STOP:			EBSI_Conveyor_Stop();				break;
	case EBSI_STOPPER_UP:				EBSI_Stopper_Up();					break;
	case EBSI_STOPPER_UP_CHK:			EBSI_Stopper_Up_Chk();				break;
	case EBSI_TRAY_ALIGN_BWD:			EBSI_Tray_Align_Bwd();				break;
	case EBSI_TRAY_ALIGN_BWD_CHK:		EBSI_Tray_Align_Bwd_Chk();			break;
	case EBSI_RAIL_GUIDE_BACK:			EBSI_Rail_Guide_Back();				break;
	case EBSI_RAIL_GUIDE_BACK_CHK:		EBSI_Rail_Guide_Back_Chk();			break;
	case EBSI_RAIL_PUSHER_BACK:			EBSI_Rail_Pusher_Back();			break;
	case EBSI_RAIL_PUSHER_BACK_CHK:		EBSI_Rail_Pusher_Back_Chk();		break;
	case EBSI_ELEVATOR_UNCLAMP:			EBSI_Elevator_Unclamp();			break;
	case EBSI_ELEVATOR_UNCLAMP_CHK:		EBSI_Elevator_Unclamp_Chk();		break;
	case EBSI_HOMECHK_Z:				EBSI_HomeChk_Z();					break;
	case EBSI_ELEVATOR_UP_FOR_R:		EBSI_Elevator_Up_For_R();			break;
	case EBSI_HOMECHK_R:				EBSI_HomeChk_R();					break;
	case EBSI_ROTATE_BASE:				EBSI_Rotate_Base();					break;
	case EBSI_ELEVATOR_BASE:			EBSI_Elevator_Base();				break;
	case EBSI_FINISH:					EBSI_Finish();						break;
	}
}

void CRun_Bin_Stacker::Run_Move()
{
	Run_Move_RecvTray();
	Run_Move_TrayUp();
	Run_Move_TrayDown();
	Run_Move_TraySend();

	Run_Cok_Change();//2013,0215
}

void CRun_Bin_Stacker::Serialize(CArchive &ar)
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

void CRun_Bin_Stacker::EBSI_Conveyor_Stop()
{
	int iConv = st_io.o_bin_conveyor[m_nSiteID];
	g_ioMgr.set_out_bit( iConv, IO_OFF );

	mn_InitStep = mn_CokChgStep = EBSI_STOPPER_UP;
}

void CRun_Bin_Stacker::EBSI_Stopper_Up()
{
	ReqStopper( FORWARD );
	
	mn_InitStep = mn_CokChgStep = EBSI_STOPPER_UP_CHK;
}

void CRun_Bin_Stacker::EBSI_Stopper_Up_Chk()
{
	if( GetStopper() != FB_NONE )
		return;

	mn_InitStep = mn_CokChgStep = EBSI_TRAY_ALIGN_BWD;
}

void CRun_Bin_Stacker::EBSI_Tray_Align_Bwd()
{
	ReqAlign( BACKWARD );
	mn_InitStep = mn_CokChgStep = EBSI_TRAY_ALIGN_BWD_CHK;
}

void CRun_Bin_Stacker::EBSI_Tray_Align_Bwd_Chk()
{
	if( GetAlign() != FB_NONE )
		return;

	mn_InitStep = mn_CokChgStep = EBSI_RAIL_GUIDE_BACK;

	Func.OnInitBtn( RBTN_BS1_CONV + (m_nSiteID) * 5);
}

void CRun_Bin_Stacker::EBSI_Rail_Guide_Back()
{
	ReqGuide( BACKWARD );

	mn_InitStep = mn_CokChgStep = EBSI_RAIL_GUIDE_BACK_CHK;
}

void CRun_Bin_Stacker::EBSI_Rail_Guide_Back_Chk()
{
	if( GetGuide() != FB_NONE )
		return;

	mn_InitStep = mn_CokChgStep = EBSI_RAIL_PUSHER_BACK;
}

void CRun_Bin_Stacker::EBSI_Rail_Pusher_Back()
{
	if( (m_nSiteID + INIT_BIN_STACKER_1) == INIT_BIN_STACKER_REJECT )
	{
		mn_InitStep = mn_CokChgStep = EBSI_ELEVATOR_UNCLAMP;
		Func.OnInitBtn( RBTN_BS1_RAIL + (m_nSiteID) * 5);
		return;  //ybs
	}
	ReqPusher( BACKWARD );

	mn_InitStep = mn_CokChgStep = EBSI_RAIL_PUSHER_BACK_CHK;
}

void CRun_Bin_Stacker::EBSI_Rail_Pusher_Back_Chk()
{
	if( GetPusher() != FB_NONE )
		return;

	mn_InitStep = mn_CokChgStep = EBSI_ELEVATOR_UNCLAMP;
	Func.OnInitBtn( RBTN_BS1_RAIL + (m_nSiteID) * 5);
}

void CRun_Bin_Stacker::EBSI_Elevator_Unclamp()
{
	ReqClamp( BACKWARD );

	mn_InitStep = mn_CokChgStep = EBSI_ELEVATOR_UNCLAMP_CHK;
}

void CRun_Bin_Stacker::EBSI_Elevator_Unclamp_Chk()
{
	if( GetClamp() != FB_NONE )
		return;

	if( Run_Reject_Tray_Transfer.m_bInitVarBackward )
	{
		mn_InitStep = mn_CokChgStep = EBSI_HOMECHK_Z;
	}
	
}

void CRun_Bin_Stacker::EBSI_HomeChk_Z()
{
	int nRet = COMI.HomeCheck_Mot( m_nMotor_Z, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = mn_CokChgStep = EBSI_ELEVATOR_UP_FOR_R;
		Func.OnInitBtn( RBTN_BS1_Z + (m_nSiteID) * 5);
	}
	else if( nRet == BD_ERROR || nRet ==BD_SAFETY )
	{
		CString strJamCode;
// 		strJamCode.Format( "HMZ%d00", m_nSiteID+1 );
		strJamCode.Format( "446%d00", m_nSiteID+1 );
		CTL_Lib.Alarm_Error_Occurrence(20, CTL_dWARNING, strJamCode );
	}
}

void CRun_Bin_Stacker::EBSI_Elevator_Up_For_R()
{
	int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_ROTATE], st_basic.nRunSpeed );
	if( nRet == BD_GOOD )
	{
		mn_InitStep = mn_CokChgStep = EBSI_HOMECHK_R;
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CString strJamCode;
// 		strJamCode.Format( "MMZ%d00", m_nSiteID+1 );
		strJamCode.Format( "446%d00", m_nSiteID+1 );
		CTL_Lib.Alarm_Error_Occurrence( 21, CTL_dWARNING, strJamCode );
	}
}

void CRun_Bin_Stacker::EBSI_HomeChk_R()
{
	int nRet = COMI.HomeCheck_Mot( m_nMotor_R, MOT_TIMEOUT);
	if( nRet == BD_GOOD )
	{
		mn_InitStep = mn_CokChgStep = EBSI_ROTATE_BASE;
		Func.OnInitBtn( RBTN_BS1_R + (m_nSiteID) * 5);
	}
	else if( nRet == BD_ERROR )
	{
		CString strJamCode;
// 		strJamCode.Format( "HMR%d00", m_nSiteID+1 );
		strJamCode.Format( "443%d00", m_nSiteID+1 );
		CTL_Lib.Alarm_Error_Occurrence( 22, CTL_dWARNING, strJamCode );
	}
}
void CRun_Bin_Stacker::EBSI_Rotate_Base()
{
	int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_BASE], st_basic.nRunSpeed );
	if( nRet == BD_GOOD )
	{
		mn_InitStep = mn_CokChgStep = EBSI_ELEVATOR_BASE;
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CString strJamCode;
// 		strJamCode.Format( "MMR%d01", m_nSiteID+1 );
		strJamCode.Format( "451%d00", m_nSiteID+1 );
		CTL_Lib.Alarm_Error_Occurrence( 23, CTL_dWARNING, strJamCode );
	}
}

void CRun_Bin_Stacker::EBSI_Elevator_Base()
{
	int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_BASE], st_basic.nRunSpeed );
	if( nRet == BD_GOOD )
	{
		mn_InitStep = mn_CokChgStep = EBSI_FINISH;
	}
	else if( nRet == BD_ERROR || nRet == BD_SAFETY )
	{
		CString strJamCode;
// 		strJamCode.Format( "MMZ%d01", m_nSiteID+1 );
		strJamCode.Format( "446%d01", m_nSiteID+1 );
		CTL_Lib.Alarm_Error_Occurrence( 25, CTL_dWARNING, strJamCode );
	}
}

void CRun_Bin_Stacker::EBSI_Finish()
{
	st_handler.mn_init_state[m_nSiteID + INIT_BIN_STACKER_1] = CTL_YES;
	if(m_nSiteID == 2 && st_handler.mn_init_state[m_nSiteID + INIT_BIN_STACKER_1] == CTL_YES)//ybs
	{
		st_handler.mn_init_state[3 + INIT_BIN_STACKER_1] = CTL_YES;
	}
	mn_InitStep = mn_CokChgStep = EBSI_START;
	Func.OnInitBtn( RBTN_BIN_STACKER1 + (m_nSiteID) * 5);
}

void CRun_Bin_Stacker::ReqRecvTray()
{
	if( m_step_recvTray == RECV_TRAY_NONE )
		m_step_recvTray = RECV_TRAY_READY;

	if( g_handler.GetMachinePos() == EMPOS_REAR )
		g_client_front.SendRecvState();
}

void CRun_Bin_Stacker::Run_Move_RecvTray()
{
	if( m_step_recvTray_old != m_step_recvTray )
	{
		m_step_recvTray_old = m_step_recvTray;
		m_dwTime_RecvTray = GetCurrentTime();

		if( Run_Move_Chk_Jam_RecvTray() == false )
			return;
	}

// 	//2013,0822 ybs
// 	if(g_handler.GetMachinePos() == EMPOS_REAR && m_nSiteID == 0 && 
// 		g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk ) == IO_OFF)
// 	{
// 		m_dwTime_RecvTray = GetCurrentTime();
// 		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
// 		return;
// 	}

	if( Run_Move_Chk_Time_RecvTray() == false )
 		return;

	Func.ThreadFunctionStepTrace(0 + m_nSiteID, m_step_recvTray);

	switch( m_step_recvTray )
	{
	case RECV_TRAY_NONE:	break;
		
	case RECV_TRAY_READY:
		{
			if( m_nSiteID == 0 )
			{
				g_server_front.SetBufferIn( NR_WAITING );
			}
			else
			{
				g_Tray.TrayMoved( ETRAY_POS(TPOS_FRONT + m_nSiteID), ETRAY_POS(TPOS_FRONT_TO_CONV1 + m_nSiteID) );
			}

			ReqAlign( BACKWARD );
			ReqStopper( FORWARD );
			m_step_recvTray = RECV_TRAY_READY_CHK;
		}
		break;

	case RECV_TRAY_READY_CHK:
		{
			if( GetAlign() != FB_NONE )				return;
			if( GetStopper() != FB_NONE )			return;

			m_step_recvTray = RECV_TRAY_CONV_ON;
		}
		break;
	case RECV_TRAY_CONV_ON:
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );

		if( m_nSiteID == 0 )			m_step_recvTray = RECV_TRAY_IN_CHK;
		else							m_step_recvTray = RECV_TRAY_STOP_CHK;
		
		break;

	case RECV_TRAY_IN_CHK:
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_in_chk ) == IO_ON )
		{
			m_step_recvTray = RECV_TRAY_STOP_CHK;
		}
// 		if( g_server_front.GetBufferInReceived())
// 		{
// 			m_nfront_smema = CTL_YES;
// 		}
		break;

	case RECV_TRAY_STOP_CHK:
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray_stop_position_chk[m_nSiteID] ) == IO_ON )
		{
			m_step_recvTray = RECV_TRAY_CONV_OFF;
		}
// 		if( m_nSiteID == 0 && g_server_front.GetBufferInReceived())
// 		{
// 			m_nfront_smema = CTL_YES;
// 		}
		break;

	case RECV_TRAY_CONV_OFF:
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
		
		if( m_nSiteID == 0 )
		{
			m_step_recvTray = RECV_TRAY_BUFFER_IN_CHK;
			st_handler.m_nfront_request = CTL_CHANGE;
		}
		else					m_step_recvTray = RECV_TRAY_DIRECTION_CHK;

		break;

	case RECV_TRAY_BUFFER_IN_CHK:
		if( g_server_front.GetBufferInReceived())
		{
// 			m_nfront_smema = CTL_YES;
			g_server_front.SetBufferIn( NR_NONE );
			m_step_recvTray = RECV_TRAY_DIRECTION_CHK;
		}
		break;

	case RECV_TRAY_DIRECTION_CHK:
		//if( m_nSiteID != 0 || (g_ioMgr.get_in_bit( st_io.i_bin_tray1_direction_chk ) == IO_OFF) )
		{
			m_step_recvTray = RECV_TRAY_FINISH;
		}
		break;
	case RECV_TRAY_FINISH:
		{
			g_Tray.TrayMoved( ETRAY_POS(TPOS_FRONT_TO_CONV1 + m_nSiteID), ETRAY_POS(TPOS_CONV1 + m_nSiteID) );
			m_step_recvTray = RECV_TRAY_NONE;
			
			if( g_handler.GetMachinePos() == EMPOS_REAR )
				g_client_front.SendRecvState();

 			if(m_nSiteID == 0)
			{
				st_handler.m_nfront_request = CTL_FREE;
				//2017.0602
				if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
				{
					int nTrayIdx = g_Tray.GetIdxByPos( ETRAY_POS(TPOS_CONV1 + m_nSiteID) );
					if( nTrayIdx > -1 )
					{
						g_client_xgem.SetTrayLoadingEnd( g_Tray.GetTray(nTrayIdx).GetStrLotID(), g_Tray.GetTray(nTrayIdx).GetStrPartID(), g_Tray.GetTray(nTrayIdx).GetBufferNo() );

						//전장비에서 생산된 자재가 올때 일단 로딩 후 언로딩한다
						if( g_Tray.GetTray(nTrayIdx).IsAllState( EMS_GOOD ) || g_Tray.GetTray(nTrayIdx).IsAllState( EMS_FAIL ) )
						{
							g_client_xgem.SetUldTrayStart( g_Tray.GetTray(nTrayIdx).GetStrLotID(), g_Tray.GetTray(nTrayIdx).GetStrPartID(), g_Tray.GetTray(nTrayIdx).GetBufferNo() );
						}
						else
						{
							g_client_xgem.SetTrayMoveLdBufferStart( g_Tray.GetTray(nTrayIdx).GetBufferNo(), st_basic.n_tray_x * st_basic.n_tray_y, g_Tray.GetTray(nTrayIdx).GetProductCnt() );
						}
					}
				}
			}

		}
		break;
	}
}

void CRun_Bin_Stacker::ReqAlign( EFB efb )
{
	m_fbAlign = efb;
	m_step_align = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::Run_Move_Align()
{
	if( m_fbAlign == FB_NONE )	return;
	if (m_nSiteID == 3) return;//ybs
	
	int iOnOff = IO_ON;
	if( m_fbAlign == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(4, m_step_align);
	switch( m_step_align )
	{
	case ESTEP_CYL_START:	m_step_align = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			int iAlign = st_io.o_bin_tray_align_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iAlign, iOnOff );
			m_step_align = ESTEP_CYL_CHK;
			m_dwTimeAlignStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iAlignFwd = st_io.i_bin_tray_align_fwd_chk[m_nSiteID];
			int iAlignBwd = st_io.i_bin_tray_align_bwd_chk[m_nSiteID];
			
			if( g_ioMgr.get_in_bit( iAlignFwd ) == iOnOff &&
				g_ioMgr.get_in_bit( iAlignBwd ) != iOnOff )
			{
				m_step_align = ESTEP_CYL_FINISH;
				m_dwTimeAlign = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeAlignStart > st_time.n_limit_time[E_WAIT_ALIGN] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Align_Time( m_nSiteID, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 26, dWARNING, strJamCode );
				}
			}
			else if(GetCurrentTime() - m_dwTimeAlignStart < 0)
			{
				m_dwTimeAlignStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{
			//2012,1220
			if(GetCurrentTime() - m_dwTimeAlign < 0)
			{
				m_dwTimeAlign = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeAlign < st_time.n_wait_time[E_WAIT_ALIGN][iOnOff] )
				return;

			m_fbAlign = FB_NONE;
			m_step_align = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Bin_Stacker::Run_Move_Stopper()
{
	if( m_fbStopper == FB_NONE )	return;
	if (m_nSiteID == 3) return;//ybs
	
	int iOnOff = IO_ON;
	if( m_fbStopper == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(5, m_step_stopper);
	switch( m_step_stopper )
	{
	case ESTEP_CYL_START:	m_step_stopper = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			int iStopper = st_io.o_bin_stopper_updn[m_nSiteID];
			g_ioMgr.set_out_bit( iStopper, iOnOff );
			m_step_stopper = ESTEP_CYL_CHK;
			m_dwTimeStopperStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iStopperUp = st_io.i_bin_stopper_up_chk[m_nSiteID];
			int iStopperDn = st_io.i_bin_stopper_dn_chk[m_nSiteID];
			
			if( g_ioMgr.get_in_bit( iStopperUp ) == iOnOff &&
				g_ioMgr.get_in_bit( iStopperDn ) != iOnOff )
			{
				m_step_stopper = ESTEP_CYL_FINISH;
				m_dwTimeStopper = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeStopperStart > st_time.n_limit_time[E_WAIT_STOPPER] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Stopper_Time( m_nSiteID, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 27, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeStopperStart < 0)
			{
				m_dwTimeStopperStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimeStopper < 0)
			{
				m_dwTimeStopper = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeStopper < st_time.n_wait_time[E_WAIT_STOPPER][iOnOff] )
				return;

			m_fbStopper = FB_NONE;
			m_step_stopper = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Bin_Stacker::ReqStopper( EFB efb )
{
	m_fbStopper = efb;
	m_step_stopper = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::ReqTrayUp()
{
	if( m_step_TrayUp == TRAY_UP_NONE )
		m_step_TrayUp = TRAY_UP_READY;
}

void CRun_Bin_Stacker::Run_Move_TrayUp()
{
	if( m_step_TrayUp_old != m_step_TrayUp )
	{
		m_step_TrayUp_old = m_step_TrayUp;
		m_dwTime_TrayUp = GetCurrentTime();

		if( Run_Move_Chk_Jam_TrayUp() == false )
			return;
	}

	if( Run_Move_Chk_Time_TrayUp() == false )
		return;

	Func.ThreadFunctionStepTrace(1, m_step_TrayUp);
	switch( m_step_TrayUp )
	{
	case TRAY_UP_NONE:	break;

	case TRAY_UP_READY:
		m_nUpBeat = 0;
		g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV1 + m_nSiteID), ETRAY_POS(TPOS_CONV_TO_BIN1 + m_nSiteID) );
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
		ReqStopper( FORWARD );
		ReqAlign( BACKWARD );
		ReqClamp( BACKWARD );
		m_step_TrayUp = TRAY_UP_READY_CHK;
		break;

	case TRAY_UP_READY_CHK:
		if( GetAlign() != FB_NONE )		return;
		if( GetStopper() != FB_NONE )	return;
		if( GetClamp() != FB_NONE )		return;
		
		m_step_TrayUp = TRAY_UP_ALIGN;
		break;

	case TRAY_UP_ALIGN:
		ReqAlign( FORWARD );
		m_step_TrayUp = TRAY_UP_ALIGN_CHK;
		break;

	case TRAY_UP_ALIGN_CHK:
		if( GetAlign() != FB_NONE )		return;
		m_step_TrayUp = TRAY_UP_ALIGN_BACK;
		break;

	case TRAY_UP_ALIGN_BACK:
		if( GetCurrentTime() - m_dwTime_TrayUp > 1000 )
		{
			ReqAlign( BACKWARD );
			m_step_TrayUp = TRAY_UP_ALIGN_BACK_CHK;
		}//2012,1220
		else if(GetCurrentTime() - m_dwTime_TrayUp < 0)
		{
			m_dwTime_TrayUp = GetCurrentTime();
		}
		break;

	case TRAY_UP_ALIGN_BACK_CHK:
		if( GetAlign() != FB_NONE )		return;
		m_step_TrayUp = TRAY_UP_ELEV_MOVE_PUT_ON;
		break;
	
	case TRAY_UP_ELEV_MOVE_PUT_ON:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_PUT_ON], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_CLAMP_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d02", m_nSiteID+1 );
				strJamCode.Format( "446%d02", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 28, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_CLAMP_ON:
		ReqClamp( FORWARD );
		m_step_TrayUp = TRAY_UP_ELEV_CLAMP_ON_CHK;
		break;

	case TRAY_UP_ELEV_CLAMP_ON_CHK:
		if( GetClamp() != FB_NONE )		return;

		m_step_TrayUp = TRAY_UP_ELEV_MOVE_ROTATE;
		break;

	case TRAY_UP_ELEV_MOVE_ROTATE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_ROTATE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d00", m_nSiteID+1 );
				strJamCode.Format( "446%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 29, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_ROTATE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_PLACE_READY;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMR%d01", m_nSiteID+1 );
				strJamCode.Format( "451%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 30, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_PLACE_READY:
		ReqGuide( BACKWARD );
		ReqPusher( BACKWARD );
		m_step_TrayUp = TRAY_UP_ELEV_PLACE_READY_CHK;
		break;

	case TRAY_UP_ELEV_PLACE_READY_CHK:
		if( GetGuide() != FB_NONE )		return;
		if( GetPusher() != FB_NONE )	return;
		m_step_TrayUp = TRAY_UP_ELEV_MOVE_PLACE_TOP;
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_TOP:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_TOP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_GUIDE_CLAMP;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d03", m_nSiteID+1 );
				strJamCode.Format( "446%d03", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 31, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_GUIDE_CLAMP:
		ReqGuide( FORWARD );
		ReqClamp( BACKWARD );
		m_step_TrayUp = TRAY_UP_ELEV_GUIDE_CLAMP_CHK;
		break;

	case TRAY_UP_ELEV_GUIDE_CLAMP_CHK:
		if( GetGuide() != FB_NONE )		return;
		if( GetClamp() != FB_NONE )		return;
		
		m_step_TrayUp = TRAY_UP_ELEV_MOVE_PLACE_BOTTOM;
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_BOTTOM:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_WAIT], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_PUSHER_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d04", m_nSiteID+1 );
				strJamCode.Format( "446%d04", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 32, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

// 	case TRAY_UP_ELEV_PUSHER_ON:
// 		ReqPusher( FORWARD );
// 		m_step_TrayUp = TRAY_UP_ELEV_PUSHER_ON_CHK;
// 		break;
// 
// 	case TRAY_UP_ELEV_PUSHER_ON_CHK:
// 		if( GetPusher() != FB_NONE )	return;
// 		g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV_TO_BIN1 + m_nSiteID), ETRAY_POS(TPOS_BIN1 + m_nSiteID) );
// 		m_step_TrayUp = TRAY_UP_ELEV_MOVE_ROTATE_BASE;
// 		break;

	case TRAY_UP_ELEV_PUSHER_ON:
		ReqPusher( FORWARD );
		m_step_TrayUp = TRAY_UP_ELEV_PUSHER_ON_CHK;
		break;
		
	case TRAY_UP_ELEV_PUSHER_ON_CHK:
		if( GetPusher() != FB_NONE )	return;
		if( m_nUpBeat >= st_handler.mn_beat_cnt )
		{
			g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV_TO_BIN1 + m_nSiteID), ETRAY_POS(TPOS_BIN1 + m_nSiteID) );
			m_step_TrayUp = TRAY_UP_ELEV_MOVE_ROTATE_BASE;
			break;
		}
		
		m_nUpBeat++;
		m_step_TrayUp = TRAY_UP_ELEV_BEAT;
		break;
		
	case TRAY_UP_ELEV_BEAT:
		ReqPusher( BACKWARD );
		m_step_TrayUp = TRAY_UP_ELEV_BEAT_CHK;
		break;
		
	case TRAY_UP_ELEV_BEAT_CHK:
		if( GetPusher() != FB_NONE )	return;
		m_step_TrayUp = TRAY_UP_ELEV_PUSHER_ON;
		break;

	case TRAY_UP_ELEV_MOVE_ROTATE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_ROTATE_BASE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d00", m_nSiteID+1 );
				strJamCode.Format( "446%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 33, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_ROTATE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_BASE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_ELEV_MOVE_BASE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMR%d00", m_nSiteID+1 );
				strJamCode.Format( "451%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 34, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_ELEV_MOVE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_BASE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayUp = TRAY_UP_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d01", m_nSiteID+1 );
				strJamCode.Format( "446%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 35, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_UP_FINISH:
		m_step_TrayUp = TRAY_UP_NONE;

		//2017.0602
		if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
		{
			int nTrayIdx = g_Tray.GetIdxByPos( ETRAY_POS(TPOS_BIN1 + m_nSiteID) );
			if( nTrayIdx > -1 )
			{
				g_client_xgem.SetTrayMoveLdBufferEnd( g_Tray.GetTray(nTrayIdx).GetBufferNo(), st_basic.n_tray_x * st_basic.n_tray_y, g_Tray.GetTray(nTrayIdx).GetProductCnt() );
			}
		}
		break;
	}
}

void CRun_Bin_Stacker::Run_Move_TrayDown()
{
	if( m_step_TrayDown_old != m_step_TrayDown )
	{
		m_step_TrayDown_old = m_step_TrayDown;
		m_dwTime_TrayDown = GetCurrentTime();

		
		if( Run_Move_Chk_Jam_TrayDown() == false )
			return;
	}

	if( Run_Move_Chk_Time_TrayDown() == false )
		return;

	Func.ThreadFunctionStepTrace(9, m_step_TrayDown);
	switch( m_step_TrayDown )
	{
	case TRAY_DOWN_NONE:	break;

	case TRAY_DOWN_READY:
		m_nDownBeat = 0;
		g_Tray.TrayMoved( ETRAY_POS(TPOS_BIN1 + m_nSiteID), ETRAY_POS(TPOS_BIN_TO_CONV1 + m_nSiteID) );
		//ReqPusher( BACKWARD );
		ReqClamp( BACKWARD );
		ReqStopper( BACKWARD );
		ReqAlign( BACKWARD );
		m_step_TrayDown = TRAY_DOWN_READY_CHK;

		//2017.0602
		if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
		{
			int nTrayIdx = g_Tray.GetIdxByPos( ETRAY_POS(TPOS_BIN1 + m_nSiteID) );
			if( nTrayIdx > -1 )
			{
				g_client_xgem.SetUldTrayStart( g_Tray.GetTray(nTrayIdx).GetStrPartID(), g_Tray.GetTray(nTrayIdx).GetStrLotID(), g_Tray.GetTray(nTrayIdx).GetBufferNo() );
			}				
		}

		break;

	case TRAY_DOWN_READY_CHK:
		//if( GetPusher() != FB_NONE )	return;
		if( GetClamp() != FB_NONE )		return;
		if( GetStopper() != FB_NONE )	return;
		if( GetAlign() != FB_NONE )		return;

		//2013,0822
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray_stop_position_chk[m_nSiteID], 0, 50 ) == IO_OFF )
		{
			m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_ROTATE;
		}
		break;

	case TRAY_DOWN_ELEV_MOVE_ROTATE:
		{
			//2013,0822
			if( g_ioMgr.get_in_bit( st_io.i_bin_tray_stop_position_chk[m_nSiteID]) == IO_ON )
			{
				CString strJamCode;
				strJamCode.Format( "300%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 928, CTL_dWARNING, strJamCode );
				break;
			}

			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_ROTATE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d00", m_nSiteID+1 );
				strJamCode.Format( "446%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 100, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_ELEV_ROTATE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_WAIT;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMR%d01", m_nSiteID+1 );
				strJamCode.Format( "451%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 101, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_ELEV_MOVE_WAIT:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_WAIT], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_BEAT_READY;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d04", m_nSiteID+1 );
				strJamCode.Format( "446%d04", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 102, CTL_dWARNING, strJamCode );
			}
		}
		break;

	case TRAY_DOWN_BEAT_READY:
		ReqPusher( BACKWARD );
		m_step_TrayDown = TRAY_DOWN_BEAT_READY_CHK;
		break;

	case TRAY_DOWN_BEAT_READY_CHK:
		if( GetPusher() != FB_NONE )	return;

		if( m_nDownBeat >= st_handler.mn_beat_cnt )
		{
			m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_PLACE_TOP;
			break;
		}

		m_nDownBeat++;
		m_step_TrayDown = TRAY_DOWN_BEAT;
		break;

	case TRAY_DOWN_BEAT:
		ReqPusher( FORWARD );
		m_step_TrayDown = TRAY_DOWN_BEAT_CHK;
		break;

	case TRAY_DOWN_BEAT_CHK:
		if( GetPusher() != FB_NONE )	return;
		m_step_TrayDown = TRAY_DOWN_BEAT_READY;
		break;

	case TRAY_DOWN_ELEV_MOVE_PLACE_TOP:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_TOP], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_CLAMP_GUIDE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d03", m_nSiteID+1 );
				strJamCode.Format( "446%d03", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 103, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_ELEV_CLAMP_GUIDE:
		ReqClamp( FORWARD );
		ReqGuide( BACKWARD );
		m_step_TrayDown = TRAY_DOWN_ELEV_CLAMP_GUIDE_CHK;
		break;

	case TRAY_DOWN_ELEV_CLAMP_GUIDE_CHK:
		if( GetClamp() != FB_NONE )		return;
		if( GetGuide() != FB_NONE )		return;
		m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_ROTATE_BASE;
		break;

	case TRAY_DOWN_ELEV_MOVE_ROTATE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_ROTATE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_ROTATE_BASE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d00", m_nSiteID+1 );
				strJamCode.Format( "446%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 105, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_ELEV_ROTATE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_BASE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_PUT_ON;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMR%d01", m_nSiteID+1 );
				strJamCode.Format( "451%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 106, CTL_dWARNING, strJamCode );
			}
		}
		
		break;
	
	case TRAY_DOWN_ELEV_MOVE_PUT_ON:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_PUT_ON], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_ELEV_CLAMP_OFF;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d02", m_nSiteID+1 );
				strJamCode.Format( "446%d02", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 107, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_ELEV_CLAMP_OFF:
		ReqClamp( BACKWARD );
		m_step_TrayDown = TRAY_DOWN_ELEV_CLAMP_OFF_CHK;
		break;

	case TRAY_DOWN_ELEV_CLAMP_OFF_CHK:
		if( GetClamp() != FB_NONE )		return;
		m_step_TrayDown = TRAY_DOWN_ELEV_MOVE_BASE;
		break;

	case TRAY_DOWN_ELEV_MOVE_BASE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_BASE], st_basic.nRunSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_TrayDown = TRAY_DOWN_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
// 				strJamCode.Format( "MMZ%d01", m_nSiteID+1 );
				strJamCode.Format( "446%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 108, CTL_dWARNING, strJamCode );
			}
		}
		
		break;

	case TRAY_DOWN_FINISH:
		g_Tray.TrayMoved( ETRAY_POS(TPOS_BIN_TO_CONV1 + m_nSiteID), ETRAY_POS(TPOS_CONV1 + m_nSiteID) );
		m_step_TrayDown = TRAY_DOWN_NONE;
		break;
	}
}

bool CRun_Bin_Stacker::Run_Move_Chk_Jam_TraySend()
{
	switch( m_step_TraySend )
	{
	case TRAY_SEND_NONE:
	case TRAY_SEND_READY:
	case TRAY_SEND_READY_CHK:
		break;

	case TRAY_SEND_NEXT_CHK:
	case TRAY_SEND_CONV_ON:
	case TRAY_SEND_ARRIVE_CHK:
	case TRAY_SEND_ARRIVE_OFF_CHK:
	case TRAY_SEND_CONV_OFF:
	case TRAY_SEND_BUFFER_IN:
	case TRAY_SEND_BUFFER_IN_CHK:
		if( Chk_Align( IO_OFF ) == false )						return false;
		if( Chk_Stopper( IO_OFF ) == false )					return false;
		if( Chk_Clamp( IO_OFF ) == false )						return false;
		if( Chk_Elev_Z( P_BIN_Z_BASE ) == false )				return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )				return false;
		break;

	case TRAY_SEND_FINISH:
		break;
	}
	
	return true;
}

void CRun_Bin_Stacker::Run_Move_Clamp()
{
	if( m_fbClamp == FB_NONE )	return;
	if (m_nSiteID == 3) return;//ybs
	
	int iOnOff = IO_ON;
	if( m_fbClamp == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(6, m_step_clamp);
	switch( m_step_clamp )
	{
	case ESTEP_CYL_START:	m_step_clamp = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			int iClamp = st_io.o_bin_elevator_tray_clamp[m_nSiteID];
			g_ioMgr.set_out_bit( iClamp, iOnOff );
			m_step_clamp = ESTEP_CYL_CHK;
			m_dwTimeClampStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iClampChk = st_io.i_bin_elevator_tray_clamp_chk[m_nSiteID];
//			if ( g_local.GetLocalType() == LOCAL_ENG ) 
//			{
//				if( g_ioMgr.get_in_bit( iClampChk ) == iOnOff ) //20130930
//				{
//					m_step_clamp = ESTEP_CYL_FINISH;
//					m_dwTimeClamp = GetCurrentTime();
//					break;
//				}
//			}
//			else
//			{
				if( g_ioMgr.get_in_bit( iClampChk ) != iOnOff )
				{
					m_step_clamp = ESTEP_CYL_FINISH;
					m_dwTimeClamp = GetCurrentTime();
					break;
				}
//			}

			if( GetCurrentTime() - m_dwTimeClampStart > st_time.n_limit_time[E_WAIT_CLAMP] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_Clamp_Time( m_nSiteID, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 109, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimeClampStart < 0)
			{
				m_dwTimeClampStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{
			//2012,1220
			if(GetCurrentTime() - m_dwTimeClamp < 0)
			{
				m_dwTimeClamp = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimeClamp < st_time.n_wait_time[E_WAIT_CLAMP][iOnOff] )
				return;

			m_fbClamp = FB_NONE;
			m_step_clamp = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Bin_Stacker::ReqClamp( EFB efb )
{
	m_fbClamp = efb;
	m_step_clamp = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::Run_Move_Guide()
{
	if( m_fbGuide == FB_NONE )	return;
	if (m_nSiteID == 3) return;//ybs
	
	int iOnOff = IO_ON;
	if( m_fbGuide == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(7, m_step_guide);
	switch( m_step_guide )
	{
	case ESTEP_CYL_START:	m_step_guide = ESTEP_CYL_MOVE;	break;
	case ESTEP_CYL_MOVE:	
		{
			int iGuide = st_io.o_bin_stacker_rail_guide_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iGuide, iOnOff );
			
			m_step_guide = ESTEP_CYL_CHK;
			m_dwTimeGuideStart = GetCurrentTime();
		}
		break;
	case ESTEP_CYL_CHK:
		{
			int iFwd = st_io.i_bin_stacker_rail_guide_fwd_chk[m_nSiteID];
			int iBwd = st_io.i_bin_stacker_rail_guide_bwd_chk[m_nSiteID];
			
			if( g_ioMgr.get_in_bit( iFwd ) == iOnOff &&
				g_ioMgr.get_in_bit( iBwd ) != iOnOff )
			{
				m_step_guide = ESTEP_CYL_FINISH;
				m_dwTimeGuide = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimeGuideStart > st_time.n_limit_time[E_WAIT_GUIDE] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TrayGuide_Time( m_nSiteID, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 110, dWARNING, strJamCode );
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
			if( GetCurrentTime() - m_dwTimeGuide < st_time.n_wait_time[E_WAIT_GUIDE][iOnOff] )
				return;

			m_fbGuide = FB_NONE;
			m_step_guide = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Bin_Stacker::Run_Move_Pusher()
{
	if( m_fbPusher == FB_NONE )	return;
	if (m_nSiteID == 3) return;//ybs
	
	int iOnOff = IO_ON;
	if( m_fbPusher == BACKWARD )
		iOnOff = IO_OFF;

	Func.ThreadFunctionStepTrace(8, m_step_pusher);
	switch( m_step_pusher )
	{
	case ESTEP_CYL_START:	m_step_pusher = ESTEP_CYL_MOVE;	break;
	/*
	case ESTEP_CYL_MOVE:	
		{
			int iPusher = st_io.o_bin_stacker_rail_pusher_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iPusher, iOnOff );

			//2017.0104
			int iPusher1 = st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iPusher1, iOnOff );
			///////////////////////////////////////////////////////////////


			//2013,0203
			if(COMI.mn_run_status == dRUN) st_work.mn_tray_pusher[m_nSiteID] = iOnOff;
			m_step_pusher = ESTEP_CYL_CHK;
			m_dwTimePusherStart = GetCurrentTime();
		}
		break;*/
	/////////////////////////////////////////////////////////////////////////////
	//2017.0117
	case ESTEP_CYL_MOVE:	
		{
			int iPusher = st_io.o_bin_stacker_rail_pusher_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iPusher, iOnOff );

			m_dwTimeRearPushStart = GetCurrentTime();
			m_step_pusher = (ENUM_STEP_CYL)1100;
		}
		break;

	case 1100:
		{
			if( iOnOff != IO_OFF )
			{
				if( GetCurrentTime() - m_dwTimeRearPushStart <= 0)
				{
					m_dwTimeRearPushStart = GetCurrentTime();
				}
				if( GetCurrentTime() - m_dwTimeRearPushStart < st_time.n_wait_time[E_WAIT_PUSHER][iOnOff] )
				{
					break;
				}
			}


			//2017.0104
			int iPusher1 = st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[m_nSiteID];
			g_ioMgr.set_out_bit( iPusher1, iOnOff );

			
			//2013,0203
			if(COMI.mn_run_status == dRUN) st_work.mn_tray_pusher[m_nSiteID] = iOnOff;
			m_step_pusher = ESTEP_CYL_CHK;
			m_dwTimePusherStart = GetCurrentTime();
		}
		break;	
	
	/////////////////////////////////////////////////////////////////////////////
	case ESTEP_CYL_CHK:
		{
			int iFwd = st_io.i_bin_stacker_rail_pusher_fwd_chk[m_nSiteID];
			int iBwd = st_io.i_bin_stacker_rail_pusher_bwd_chk[m_nSiteID];
			
			if( g_ioMgr.get_in_bit( iFwd ) == iOnOff &&
				g_ioMgr.get_in_bit( iBwd ) != iOnOff )
			{
				m_step_pusher = ESTEP_CYL_FINISH;
				m_dwTimePusher = GetCurrentTime();
				break;
			}

			if( GetCurrentTime() - m_dwTimePusherStart > st_time.n_limit_time[E_WAIT_PUSHER] )
			{
				if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
				{
					CString strJamCode = GetJamCode_TrayPusher_Time( m_nSiteID, iOnOff );
					CTL_Lib.Alarm_Error_Occurrence( 111, dWARNING, strJamCode );
				}
			}//2012,1220
			else if(GetCurrentTime() - m_dwTimePusherStart < 0)
			{
				m_dwTimePusherStart = GetCurrentTime();
			}
		}
		break;
	case ESTEP_CYL_FINISH:
		{//2012,1220
			if(GetCurrentTime() - m_dwTimePusher < 0)
			{
				m_dwTimePusher = GetCurrentTime();
			}
			if( GetCurrentTime() - m_dwTimePusher < st_time.n_wait_time[E_WAIT_PUSHER][iOnOff] )
				return;

			m_fbPusher= FB_NONE;
			m_step_pusher = ESTEP_CYL_START;
		}
		break;
	}
}

void CRun_Bin_Stacker::ReqGuide( EFB efb )
{
	m_fbGuide = efb;
	m_step_guide = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::ReqPusher( EFB efb )
{
	if( (m_nSiteID + INIT_BIN_STACKER_1) == INIT_BIN_STACKER_REJECT )
		return;  //ybs

	m_fbPusher = efb;
	m_step_pusher = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::ReqTrayDown()
{
	if( m_step_TrayDown == TRAY_DOWN_NONE )
		m_step_TrayDown = TRAY_DOWN_READY;
}

void CRun_Bin_Stacker::Run_Move_TraySend()
{
	if( m_step_TraySend_old != m_step_TraySend )
	{
		m_step_TraySend_old = m_step_TraySend;
		m_dwTime_TraySend = GetCurrentTime();
		
		if( Run_Move_Chk_Jam_TraySend() == false )
			return;
	}
// 	//2013,0822 ybs
// 	if(g_handler.GetMachinePos() == EMPOS_FRONT && m_nSiteID == 2 && m_step_TraySend >= TRAY_SEND_NEXT_CHK &&
// 		g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) != IO_ON)
// 	{
// 		m_dwTime_TraySend = GetCurrentTime();
// 		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
// 		return;
// 	}

	if( Run_Move_Chk_Time_TraySend() == false )
		return;

	Func.ThreadFunctionStepTrace( 15 + m_nSiteID, m_step_TraySend);
	switch( m_step_TraySend )
	{
	case TRAY_SEND_NONE:	break;

	case TRAY_SEND_READY:
		g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV1 + m_nSiteID), ETRAY_POS(TPOS_CONV1_TO_CONV2 + m_nSiteID) );
		ReqStopper( BACKWARD );
		ReqAlign( BACKWARD );
		m_step_TraySend = TRAY_SEND_READY_CHK;
		break;

	case TRAY_SEND_READY_CHK:
		if( GetStopper() != FB_NONE )		return;
		if( GetAlign() != FB_NONE )			return;
		//2013,0910
		if(m_nSiteID == 2 && g_handler.GetMachinePos() == EMPOS_FRONT)
		{
			if(g_client_next.getBufferReady() == true)
			{
				m_step_TraySend = TRAY_SEND_BUFFER_READY;
				break;
			}
		}
		m_step_TraySend = TRAY_SEND_NEXT_CHK;
		break;

	case TRAY_SEND_BUFFER_READY:
		if(g_client_next.getBufferReady() != true)
		{
			m_step_TraySend = TRAY_SEND_NEXT_CHK;
		}
		break;

	case TRAY_SEND_NEXT_CHK:
		if( m_nSiteID + INIT_BIN_STACKER_1 != INIT_BIN_STACKER_3) //INIT_BIN_STACKER_REJECT ybs
		{
			if( g_ioMgr.get_in_bit( st_io.i_bin_tray_align_bwd_chk[m_nSiteID + 1] ) == IO_OFF )		return;
			if( g_ioMgr.get_in_bit( st_io.i_bin_tray_align_fwd_chk[m_nSiteID + 1] ) == IO_ON )		return;

			if( g_ioMgr.get_in_bit( st_io.i_bin_stopper_dn_chk[m_nSiteID + 1] ) == IO_ON )			return;
			if( g_ioMgr.get_in_bit( st_io.i_bin_stopper_up_chk[m_nSiteID + 1] ) == IO_OFF )			return;
		}
		else
		{
			if( g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, IO_OFF ) == IO_OFF )			return;
		}

		if( m_nSiteID == 2 )
		{
			g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_ON );
		}

		m_step_TraySend = TRAY_SEND_CONV_ON;
		break;

	case TRAY_SEND_CONV_ON:
		g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
		m_step_TraySend = TRAY_SEND_ARRIVE_CHK;
		break;

	case TRAY_SEND_ARRIVE_CHK:
		if( m_nSiteID == 2 )
		{
			int nIO = g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, IO_ON );
			//2013,0912
// 			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], nIO );
			if(g_handler.GetMachinePos() == EMPOS_REAR)
			{
				g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
			}
			else
			{
				g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], nIO );
			}
		}
		else
		{
			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
		}

		if( g_ioMgr.get_in_bit( st_io.i_bin_tray_send_arrive_chk[m_nSiteID], IO_ON ) == IO_ON)
		{
			m_step_TraySend = TRAY_SEND_CONV_OFF;
			if( m_nSiteID == 2 )
			{
				m_step_TraySend = TRAY_SEND_ARRIVE_OFF_CHK;
			}
		}
		break;

	case TRAY_SEND_ARRIVE_OFF_CHK:
		if( m_nSiteID == 2 )
		{
			int nIO = g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, IO_ON );
// 			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], nIO );
			//2013,0917
			if(g_handler.GetMachinePos() == EMPOS_REAR)
			{
				g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
			}
			else
			{
				g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], nIO );
			}
		}
		else 
		{
			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_ON );
		}

//  		if( g_ioMgr.get_in_bit( st_io.i_bin_tray_send_arrive_chk[m_nSiteID] ) == IO_OFF )
// 		{
// 			m_step_TraySend = TRAY_SEND_CONV_MORE;
// 			g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_OFF );
// 			ml_wait_time[0] = GetCurrentTime();
// 		}
		//2013,0818
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray_send_arrive_chk[m_nSiteID], 0, 500 ) == IO_OFF )
		{
			m_step_TraySend = TRAY_SEND_CONV_MORE;
// 			//2013,0822
// 			if(g_handler.GetMachinePos() == EMPOS_REAR)
// 			{
				g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_OFF );
// 			}
			ml_wait_time[0] = GetCurrentTime();
		}

		break;

	case TRAY_SEND_CONV_MORE:
		ml_wait_time[1] = GetCurrentTime();
		ml_wait_time[2] = ml_wait_time[1] - ml_wait_time[0];
		if(ml_wait_time[2] > 2500 && m_nSiteID != 2)
		{
			m_step_TraySend = TRAY_SEND_CONV_OFF;
		}
		else if(m_nSiteID == 2)//2013,0123
		{
			if(g_handler.GetMachinePos() == EMPOS_FRONT)
			{
				if(ml_wait_time[2] > 2000)
				{
					m_step_TraySend = TRAY_SEND_CONV_OFF;
				}
				else if(ml_wait_time[2] < 0)
				{
					ml_wait_time[0] = GetCurrentTime();
				}
			}
			else if(g_handler.GetMachinePos() == EMPOS_REAR)
			{
				if(ml_wait_time[2] > 2000)
				{
					m_step_TraySend = TRAY_SEND_CONV_OFF;
				}
				else if(ml_wait_time[2] < 0)
				{
					ml_wait_time[0] = GetCurrentTime();
				}
			}
			else
			{
				if(ml_wait_time[2] > 2500)
				{
					m_step_TraySend = TRAY_SEND_CONV_OFF;
				}
				else if(ml_wait_time[2] < 0)
				{
					ml_wait_time[0] = GetCurrentTime();
				}
			}
			
		}
		else if(ml_wait_time[2] < 0)
		{
			ml_wait_time[0] = GetCurrentTime();
		}
		// 		if( InTime_TraySend( 2000 ) == false )
		// 		{
		// 			m_step_TraySend = TRAY_SEND_CONV_OFF;
		// 		}
		break;

	case TRAY_SEND_CONV_OFF:
		if( m_nSiteID != 2 )//2013,0123
		{
			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
		}
		m_step_TraySend = TRAY_SEND_FINISH;
		ml_wait_time[0] = GetCurrentTime();		
		
		if( m_nSiteID == 2 )
		{
			m_step_TraySend = TRAY_SEND_BUFFER_IN;
		}
		break;

	case TRAY_SEND_BUFFER_IN:
		g_client_next.SetBufferIn();
		m_step_TraySend = TRAY_SEND_BUFFER_IN_CHK;
		break;

	case TRAY_SEND_BUFFER_IN_CHK:
		if( g_client_next.GetBufferIn() )
		{
			//2013,0201
// 			if( GetCurrentTime() - m_dwTime_TraySend > 5000 )
// 			{
// 				g_client_next.SetBufferIn();
// 			}
			break;
		}

		if( m_nSiteID == 2 )//2013,0123
		{
			g_ioMgr.set_out_bit( st_io.o_bin_conveyor[m_nSiteID], IO_OFF );
		}
		m_step_TraySend = TRAY_SEND_FINISH;
		ml_wait_time[0] = GetCurrentTime();		
		break;

	case TRAY_SEND_FINISH:
		//2013,0823
		g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_OFF );
		if( m_nSiteID == 2 )//2013,0123
		{
			ml_wait_time[1] = GetCurrentTime();
			ml_wait_time[2] = ml_wait_time[1] - ml_wait_time[0];
			if(ml_wait_time[2] < 0)	ml_wait_time[0] = GetCurrentTime();			
			if(ml_wait_time[2] < 1000) break;
			//2017.0602
			if( st_basic.mn_mode_xgem == CTL_YES && st_handler.mn_xgem_mode == CTL_YES)
			{
				int nTrayIdx = g_Tray.GetIdxByPos( ETRAY_POS(TPOS_CONV1_TO_CONV2 + m_nSiteID) );
				if( nTrayIdx > -1 )
				{
					g_client_xgem.SetUldTrayEnd( g_Tray.GetTray(nTrayIdx).GetStrPartID(), g_Tray.GetTray(nTrayIdx).GetStrLotID(), g_Tray.GetTray(nTrayIdx).GetBufferNo() );
				}				
			}
		
		}
		g_Tray.TrayMoved( ETRAY_POS(TPOS_CONV1_TO_CONV2 + m_nSiteID), ETRAY_POS(TPOS_CONV1 + m_nSiteID + 1) );
		m_step_TraySend = TRAY_SEND_NONE;

		break;
	}
}

void CRun_Bin_Stacker::ReqTraySend()
{
	if( m_step_TraySend == TRAY_SEND_NONE )
		m_step_TraySend = TRAY_SEND_READY;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Jam_RecvTray()
{
	switch( m_step_recvTray )
	{
	case RECV_TRAY_NONE:
	case RECV_TRAY_READY:
	case RECV_TRAY_READY_CHK:
		break;

	case RECV_TRAY_CONV_ON:
	case RECV_TRAY_IN_CHK:
	case RECV_TRAY_STOP_CHK:
	case RECV_TRAY_CONV_OFF:
	case RECV_TRAY_BUFFER_IN_CHK:
	case RECV_TRAY_DIRECTION_CHK:
		if( Chk_Align( IO_OFF ) == false )						return false;
		if( Chk_Stopper( IO_ON ) == false )						return false;
		if( Chk_Clamp( IO_OFF ) == false )						return false;
		if( Chk_Elev_Z( P_BIN_Z_BASE ) == false )	return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )	return false;
		break;

	case RECV_TRAY_FINISH:
		break;
	}
	
	return true;
}

bool CRun_Bin_Stacker::Chk_Align( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_bin_tray_align_fwdbwd[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_tray_align_fwd_chk[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_tray_align_bwd_chk[m_nSiteID], !iOn ) != iOn )
	{
		return true;
	}
	
	if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
	{
		CString strJamCode = GetJamCode_AlignChk( m_nSiteID, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 200, dWARNING, strJamCode );
	}
	return false;

}

bool CRun_Bin_Stacker::Chk_Stopper( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_bin_stopper_updn[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stopper_up_chk[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stopper_dn_chk[m_nSiteID], !iOn ) != iOn )
	{
		return true;
	}

	if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
	{
		CString strJamCode = GetJamCode_StopperChk( m_nSiteID, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 210, dWARNING, strJamCode );
	}
	return false;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Jam_TrayUp()
{
	switch( m_step_TrayUp )
	{
	case TRAY_UP_NONE:
		break;

	case TRAY_UP_READY:
	case TRAY_UP_READY_CHK:
	case TRAY_UP_ALIGN:
	case TRAY_UP_ALIGN_CHK:
	case TRAY_UP_ALIGN_BACK:
	case TRAY_UP_ALIGN_BACK_CHK:
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_BASE ) == false )	return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )	return false;
		if( Chk_Conv_Tray( IO_ON ) == false )					return false;
		if( Chk_Bin_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;
	
	case TRAY_UP_ELEV_MOVE_PUT_ON:
	case TRAY_UP_ELEV_CLAMP_ON:
	case TRAY_UP_ELEV_CLAMP_ON_CHK:
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )	return false;
		if( Chk_Bin_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_MOVE_ROTATE:
		if( Chk_Clamp( IO_ON ) == false )			return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )	return false;
		if( Chk_Bin_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_ROTATE:
		if( Chk_Clamp( IO_ON ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Bin_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_PLACE_READY:
	case TRAY_UP_ELEV_PLACE_READY_CHK:
		if( Chk_Clamp( IO_ON ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_ROTATE ) == false )	return false;
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Bin_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_TOP:
		if( Chk_Clamp( IO_ON ) == false )			return false;
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Pusher( IO_OFF ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_GUIDE_CLAMP:
	case TRAY_UP_ELEV_GUIDE_CLAMP_CHK:
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Pusher( IO_OFF ) == false )						return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_BOTTOM:
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Pusher( IO_OFF ) == false )						return false;
		if( Chk_Guide( IO_ON ) == false )						return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_PUSHER_ON:
	case TRAY_UP_ELEV_PUSHER_ON_CHK:
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_WAIT ) == false )	return false;
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Guide( IO_ON ) == false )						return false;
		if( Chk_Bin_Tray( IO_ON ) == false )					return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_MOVE_ROTATE_BASE:
		if( Chk_Bin_Tray( IO_ON ) == false )					return false;
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Pusher( IO_ON ) == false )						return false;
		if( Chk_Guide( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_ROTATE_BASE:
		if( Chk_Bin_Tray( IO_ON ) == false )					return false;
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_ROTATE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Pusher( IO_ON ) == false )						return false;
		if( Chk_Guide( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_ELEV_MOVE_BASE:
		if( Chk_Bin_Tray( IO_ON ) == false )					return false;
		if( Chk_Clamp( IO_OFF ) == false )			return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )	return false;
		if( Chk_Conv_Tray( IO_OFF ) == false )					return false;
		if( Chk_Pusher( IO_ON ) == false )						return false;
		if( Chk_Guide( IO_ON ) == false )						return false;
		if( g_handler.Chk_VarRail() == false )				return false;
		break;

	case TRAY_UP_FINISH:
		break;
	}

	return true;
}

bool CRun_Bin_Stacker::Chk_Clamp( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_bin_elevator_tray_clamp[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_elevator_tray_clamp_chk[m_nSiteID], !iOn ) != iOn )
	{
		return true;
	}

	if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
	{
		CString strJamCode = GetJamCode_ClampChk( m_nSiteID, iOn );
		CTL_Lib.Alarm_Error_Occurrence( 202, dWARNING, strJamCode );
	}
	return false;
}

bool CRun_Bin_Stacker::Chk_Elev_Z( int nPos )
{
	if( abs( st_motor[m_nMotor_Z].d_pos[nPos] - g_comiMgr.Get_MotCurrentPos(m_nMotor_Z) ) > COMI.md_allow_value[m_nMotor_Z] )
	{
		if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
		{
			CString strJamCode = GetJamCode_Elev_Z(m_nSiteID, nPos );
			CTL_Lib.Alarm_Error_Occurrence( 203, dWARNING, strJamCode );
		}
		return false;
	}

	return true;
}

bool CRun_Bin_Stacker::Chk_Elev_R( int nPos )
{
	if( abs( st_motor[m_nMotor_R].d_pos[nPos] - g_comiMgr.Get_MotCurrentPos(m_nMotor_R) ) > COMI.md_allow_value[m_nMotor_R] )
	{
		if( COMI.mn_run_status == dRUN || COMI.mn_run_status == dINIT )
		{
			CString strJamCode = GetJamCode_Elev_R(m_nSiteID, nPos );
			CTL_Lib.Alarm_Error_Occurrence( 204, dWARNING, strJamCode );
		}
		return false;
	}

	return true;
}

bool CRun_Bin_Stacker::Chk_Conv_Tray( int iOn )
{
	if( g_ioMgr.get_in_bit( st_io.i_bin_tray_stop_position_chk[m_nSiteID], iOn ) == iOn )
	{
// 		if( m_nSiteID == 0 && iOn == IO_ON )
// 		{
// 			if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_direction_chk, iOn ) == IO_ON )
// 			{
// 				CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "CTDR00");
// 				return false;
// 			}
// 		}

		return true;
	}

	CString strJamCode = GetJamCode_ConvTray_Chk( m_nSiteID, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 205, dWARNING, strJamCode );

	return false;
}

bool CRun_Bin_Stacker::Chk_Pusher( int iOn )
{
//	if( m_nSiteID == 2 )
	if( m_nSiteID >= 2 ) //ybs
		return true;

// 	if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_pusher_fwdbwd[m_nSiteID], iOn ) == iOn &&
// 		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_fwd_chk[m_nSiteID], iOn ) == iOn &&
// 		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_bwd_chk[m_nSiteID], !iOn ) != iOn )
// 	{
// 		return true;
// 	}
	//2017.0104
	if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_pusher_fwdbwd[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_out_bit( st_io.o_bin_rear_stacker_rail_pusher_fwdbwd[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_fwd_chk[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_pusher_bwd_chk[m_nSiteID], !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TrayPusher_Chk( m_nSiteID, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 206, dWARNING, strJamCode );
	return false;	
}

bool CRun_Bin_Stacker::Chk_Guide( int iOn )
{
	if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_guide_fwdbwd[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_fwd_chk[m_nSiteID], iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_guide_bwd_chk[m_nSiteID], !iOn ) != iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TrayGuide_Chk( m_nSiteID, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 207, dWARNING, strJamCode );
	return false;
}

bool CRun_Bin_Stacker::Chk_Bin_Tray( int iOn )
{
	if( g_ioMgr.get_in_bit( st_io.i_bin_stacker_tray_chk[m_nSiteID], iOn ) == iOn )
	{
		return true;
	}
	
	CString strJamCode = GetJamCode_TrayExist_Chk( m_nSiteID, iOn );
	CTL_Lib.Alarm_Error_Occurrence( 208, dWARNING, strJamCode );
	return false;	
}

bool CRun_Bin_Stacker::Run_Move_Chk_Jam_TrayDown()
{
	switch( m_step_TrayDown )
	{
	case TRAY_DOWN_NONE:
		break;

	case TRAY_DOWN_READY:
	case TRAY_DOWN_READY_CHK:
		if( Chk_Conv_Tray( IO_OFF ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_BASE ) == false )		return false;
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )		return false;
		if( Chk_Guide( IO_ON ) == false )				return false;
		if( Chk_Bin_Tray( IO_ON ) == false )			return false;
		break;

	case TRAY_DOWN_ELEV_MOVE_ROTATE:
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )		return false;
		//if( Chk_Pusher( IO_OFF ) == false )				return false;
		if( Chk_Clamp( IO_OFF ) == false )				return false;
		if( Chk_Stopper( IO_OFF ) == false )			return false;
		if( Chk_Align( IO_OFF ) == false )				return false;
		if( Chk_Guide( IO_ON ) == false )				return false;
		if( Chk_Bin_Tray( IO_ON ) == false )			return false;
		break;

	case TRAY_DOWN_ELEV_ROTATE:
		//2012,1223
		if( Chk_Bin_Tray( IO_ON ) == false )			return false;
		if( Chk_Elev_Z( P_BIN_Z_ROTATE ) == false )		return false;
		//if( Chk_Pusher( IO_OFF ) == false )				return false;
		if( Chk_Clamp( IO_OFF ) == false )				return false;
		if( Chk_Guide( IO_ON ) == false )				return false;
		if( Chk_Bin_Tray( IO_ON ) == false )			return false;
		break;

	case TRAY_DOWN_ELEV_MOVE_PLACE_TOP:
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )		return false;
		if( Chk_Pusher( IO_OFF ) == false )				return false;
		if( Chk_Clamp( IO_OFF ) == false )				return false;
		if( Chk_Guide( IO_ON ) == false )				return false;

		break;

	case TRAY_DOWN_ELEV_CLAMP_GUIDE:
	case TRAY_DOWN_ELEV_CLAMP_GUIDE_CHK:
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )		return false;
		if( Chk_Pusher( IO_OFF ) == false )				return false;
		if( Chk_Elev_Z( P_BIN_Z_TOP ) == false )		return false;
		break;

	case TRAY_DOWN_ELEV_MOVE_ROTATE_BASE:
		if( Chk_Elev_R( P_BIN_R_ROTATE ) == false )		return false;
		if( Chk_Pusher( IO_OFF ) == false )				return false;
		if( Chk_Guide( IO_OFF ) == false )				return false;
		if( Chk_Clamp( IO_ON ) == false )				return false;
		break;

	case TRAY_DOWN_ELEV_ROTATE_BASE:
		if( Chk_Elev_Z( P_BIN_Z_ROTATE ) == false )		return false;
		if( Chk_Clamp( IO_ON ) == false )				return false;
		break;
	
	case TRAY_DOWN_ELEV_MOVE_PUT_ON:
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )		return false;
		if( Chk_Clamp( IO_ON ) == false )				return false;
		break;

	case TRAY_DOWN_ELEV_CLAMP_OFF:
	case TRAY_DOWN_ELEV_CLAMP_OFF_CHK:
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )		return false;
		if( Chk_Elev_Z( P_BIN_Z_PUT_ON ) == false )		return false;
		break;

	case TRAY_DOWN_ELEV_MOVE_BASE:
		if( Chk_Elev_R( P_BIN_R_BASE ) == false )		return false;
		if( Chk_Clamp( IO_OFF ) == false )				return false;
		break;

	case TRAY_DOWN_FINISH:
		break;
	}

	return true;
}

bool CRun_Bin_Stacker::CalcRearSmema()
{
	int nTraySendEndPos = TRAY_SEND_FINISH;
	if( st_handler.mn_virtual_mode )
	{
		nTraySendEndPos = TRAY_SEND_CONV_OFF;
	}

//	if( (m_step_TraySend >= TRAY_SEND_ARRIVE_CHK && m_step_TraySend < nTraySendEndPos) &&
//		m_nSiteID == 2 && COMI.mn_run_status == dRUN )
	//ybs
// 	if( (m_step_TraySend >= TRAY_SEND_ARRIVE_CHK && m_step_TraySend < nTraySendEndPos) &&
// 		m_nSiteID == 2 && COMI.mn_run_status == dRUN )
	//2013,0826
	if( (m_step_TraySend >= TRAY_SEND_ARRIVE_CHK && m_step_TraySend < nTraySendEndPos) &&
		m_nSiteID == 2)
	{
		//return false;
		g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_ON );
		return true;
	}

	g_ioMgr.set_out_bit( st_io.o_rear_shuttle_transfer_signal, IO_OFF );
	return false;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Time_RecvTray()
{
	switch( m_step_recvTray )
	{
	case RECV_TRAY_IN_CHK:
		if( InTime_RecvTray( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TTIN00" );
			CTL_Lib.Alarm_Error_Occurrence( 209, dWARNING, "650000" );
			return false;
		}
		break;
	case RECV_TRAY_STOP_CHK:
		if( InTime_RecvTray( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TTST00" );
			CString strJamcode;
			strJamcode.Format( "%d", 660000 + m_nSiteID );
			CTL_Lib.Alarm_Error_Occurrence( 210, dWARNING, strJamcode );
			return false;
		}
		break;

	case RECV_TRAY_BUFFER_IN_CHK:
		if( InTime_RecvTray( 20000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "EBIN00" );
			CTL_Lib.Alarm_Error_Occurrence( 211, dWARNING, "390500" );
			return false;
		}
		break;
	}
	
	return true;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Time_TrayUp()
{
	switch( m_step_TrayUp )
	{
	case TRAY_UP_ELEV_MOVE_PUT_ON:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ101" );
			CTL_Lib.Alarm_Error_Occurrence( 220, dWARNING, "626101" );
			return false;
		}
		break;
	case TRAY_UP_ELEV_MOVE_ROTATE:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ102" );
			CTL_Lib.Alarm_Error_Occurrence( 221, dWARNING, "626102" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_ROTATE:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMR101" );
			CTL_Lib.Alarm_Error_Occurrence( 222, dWARNING, "621101" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_TOP:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ104" );
			CTL_Lib.Alarm_Error_Occurrence( 223, dWARNING, "626104" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_MOVE_PLACE_BOTTOM:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ103" );
			CTL_Lib.Alarm_Error_Occurrence( 224, dWARNING, "626103" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_MOVE_ROTATE_BASE:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ102" );
			CTL_Lib.Alarm_Error_Occurrence( 225, dWARNING, "626102" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_ROTATE_BASE:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMR100" );
			CTL_Lib.Alarm_Error_Occurrence( 226, dWARNING, "621100" );
			return false;
		}
		break;

	case TRAY_UP_ELEV_MOVE_BASE:
		if( InTime_TrayUp( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TUB000" );
			CTL_Lib.Alarm_Error_Occurrence( 227, dWARNING, "621102" );
			return false;
		}
		break;
	}

	return true;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Time_TrayDown()
{
	switch( m_step_TrayDown )
	{
	case TRAY_DOWN_ELEV_MOVE_ROTATE:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ102" );
			CTL_Lib.Alarm_Error_Occurrence( 230, dWARNING, "626102" );
			return false;
		}
		break;

	case TRAY_DOWN_ELEV_ROTATE:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMR101" );
			CTL_Lib.Alarm_Error_Occurrence( 231, dWARNING, "621101" );
			return false;
		}
		break;

	case TRAY_DOWN_ELEV_MOVE_PLACE_TOP:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ104" );
			CTL_Lib.Alarm_Error_Occurrence( 232, dWARNING, "626104" );
			return false;
		}
		break;

	case TRAY_DOWN_ELEV_MOVE_ROTATE_BASE:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMR101" );
			CTL_Lib.Alarm_Error_Occurrence( 233, dWARNING, "621101" );
			return false;
		}
		break;

	case TRAY_DOWN_ELEV_ROTATE_BASE:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMR100" );
			CTL_Lib.Alarm_Error_Occurrence( 234, dWARNING, "621100" );
			return false;
		}
		break;
	
	case TRAY_DOWN_ELEV_MOVE_PUT_ON:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ101" );
			CTL_Lib.Alarm_Error_Occurrence( 235, dWARNING, "626101" );
			return false;
		}
		break;

	case TRAY_DOWN_ELEV_MOVE_BASE:
		if( InTime_TrayDown( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "TMZ100" );
			CTL_Lib.Alarm_Error_Occurrence( 236, dWARNING, "626100" );
			return false;
		}
		break;
	}

	return true;
}

bool CRun_Bin_Stacker::Run_Move_Chk_Time_TraySend()
{
	switch( m_step_TraySend )
	{
	case TRAY_SEND_ARRIVE_CHK:
		//2012,1221
//2013,0513 if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, IO_ON ) == IO_ON)
		if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, 0, 100 ) == IO_ON)
		{
			if( InTime_TraySend( 15000 ) == false )
			{
	// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "T0TA01" );
				CString strJamcode;
				strJamcode.Format( "%d", 580001 + m_nSiteID );
				CTL_Lib.Alarm_Error_Occurrence( 240, dWARNING, strJamcode );
				return false;
			}
		}
		else
		{
			m_dwTime_TraySend = GetCurrentTime();
		}
		break;

	case TRAY_SEND_ARRIVE_OFF_CHK:
		//2012,1221
//2013,0513 if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, IO_ON ) == IO_ON)
		if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, 0, 100 ) == IO_ON)
		{
			if( InTime_TraySend( 10000 ) == false )
			{
// 				CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "T0TA00" );
				CTL_Lib.Alarm_Error_Occurrence( 241, dWARNING, "580000" );
				return false;
			}
		}
		else
		{
			m_dwTime_TraySend = GetCurrentTime();
		}
		break;

	case TRAY_SEND_BUFFER_IN_CHK:
		if( InTime_TraySend( 10000 ) == false )
		{
// 			CTL_Lib.Alarm_Error_Occurrence( 0, dWARNING, "EBIN01" );
			CTL_Lib.Alarm_Error_Occurrence( 242, dWARNING, "390501" );
			//2013,0822
			g_client_next.SetUsable(false);
			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);

			return false;
		}
		break;
	}
	return true;
}

bool CRun_Bin_Stacker::InTime_RecvTray( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_RecvTray < 0)
	{
		m_dwTime_RecvTray = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_RecvTray < iMilliSec )
		return true;

	return false;
}

bool CRun_Bin_Stacker::InTime_TrayUp( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_TrayUp < 0)
	{
		m_dwTime_TrayUp = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TrayUp < iMilliSec )
		return true;

	return false;
}

bool CRun_Bin_Stacker::InTime_TrayDown( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_TrayDown < 0)
	{
		m_dwTime_TrayDown = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TrayDown < iMilliSec )
		return true;

	return false;
}

bool CRun_Bin_Stacker::InTime_TraySend( int iMilliSec )
{
	//2012,1220
	if(GetCurrentTime() - m_dwTime_TraySend < 0)
	{
		m_dwTime_TraySend = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TraySend < iMilliSec )
		return true;

	return false;
}

void CRun_Bin_Stacker::OnBinStacker_FinalPos()
{
	mn_reinstate_step = 0;
}

void CRun_Bin_Stacker::Run_Reinstate()
{
	if(st_work.mn_reinstate_stacker_robot[m_nSiteID] == CTL_YES) return;
	switch(mn_reinstate_step)
	{
	case 0:
		if(st_work.mn_reinstate_xyz_robot == CTL_YES)
		{
			mn_reinstate_step = 1000;
		}
		break;

	case 1000:
		if(st_work.mn_tray_pusher[m_nSiteID] == IO_ON)  ReqPusher(FORWARD);
		else											ReqPusher(BACKWARD);
		mn_reinstate_step = 1100;
		break;

	case 1100:
		if( GetPusher() != FB_NONE )	return;
		mn_reinstate_step = 2000;
		break;


	case 2000:
		st_work.mn_reinstate_stacker_robot[m_nSiteID] = CTL_YES;
		break;
	}
}


void CRun_Bin_Stacker::ReqElvMoveRotateBase(EFB efb )
{
	m_fbElvBase = efb;
	m_step_ElvBase = ESTEP_CYL_START;
}

void CRun_Bin_Stacker::Run_MoveElvRotateBase()
{
	if( m_fbElvBase == FB_NONE )	return;
	switch(m_step_ElvBase)
	{
	case ESTEP_CYL_START:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_R, st_motor[m_nMotor_R].d_pos[P_BIN_R_BASE], st_basic.nManualSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_ElvBase = ESTEP_CYL_MOVE;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "451%d00", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 243, CTL_dWARNING, strJamCode );
			}
		}
		
		break;
		
	case ESTEP_CYL_MOVE:
		{
			int nRet = CTL_Lib.Single_Move( m_nMotor_Z, st_motor[m_nMotor_Z].d_pos[P_BIN_Z_BASE], st_basic.nManualSpeed );
			if( nRet == BD_GOOD )
			{
				m_step_ElvBase = ESTEP_CYL_FINISH;
			}
			else if( nRet == BD_ERROR || nRet == BD_SAFETY )
			{
				CString strJamCode;
				strJamCode.Format( "446%d01", m_nSiteID+1 );
				CTL_Lib.Alarm_Error_Occurrence( 244, CTL_dWARNING, strJamCode );
			}
		}
		
		break;
		
	case ESTEP_CYL_FINISH:
		m_fbElvBase = FB_NONE;
		m_step_ElvBase = ESTEP_CYL_START;
		break;
	}
}

void CRun_Bin_Stacker::Run_Cok_Change()
{
	int nRet = 0;
	int i = 0;
	
	if (st_handler.mn_cok_chgstate[0 + m_nSiteID] != CTL_NO)		return;		// No 일때만 초기화 작업을 한다. 초기화가 끝나면 YES->READY가 되기 떄문에...
	
	switch (mn_CokChgStep)
	{
	case EBSI_START:					mn_CokChgStep = EBSI_CONVEYOR_STOP;	
		break;
		
	case EBSI_CONVEYOR_STOP:			EBSI_Conveyor_Stop();				break;
	case EBSI_STOPPER_UP:				EBSI_Stopper_Up();					break;
	case EBSI_STOPPER_UP_CHK:			EBSI_Stopper_Up_Chk();				break;
	case EBSI_TRAY_ALIGN_BWD:			EBSI_Tray_Align_Bwd();				break;
	case EBSI_TRAY_ALIGN_BWD_CHK:		EBSI_Tray_Align_Bwd_Chk();			break;
	case EBSI_RAIL_GUIDE_BACK:			EBSI_Rail_Guide_Back();				break;
	case EBSI_RAIL_GUIDE_BACK_CHK:		EBSI_Rail_Guide_Back_Chk();			break;
	case EBSI_RAIL_PUSHER_BACK:			EBSI_Rail_Pusher_Back();			break;
	case EBSI_RAIL_PUSHER_BACK_CHK:		EBSI_Rail_Pusher_Back_Chk();		break;
	case EBSI_ELEVATOR_UNCLAMP:			EBSI_Elevator_Unclamp();			break;
	case EBSI_ELEVATOR_UNCLAMP_CHK:		EBSI_Elevator_Unclamp_Chk();		break;
	case EBSI_HOMECHK_Z:				EBSI_HomeChk_Z();					break;
	case EBSI_ELEVATOR_UP_FOR_R:		EBSI_Elevator_Up_For_R();			break;
	case EBSI_HOMECHK_R:				EBSI_HomeChk_R();					break;
	case EBSI_ROTATE_BASE:				EBSI_Rotate_Base();					break;
	case EBSI_ELEVATOR_BASE:			EBSI_Elevator_Base();				break;
	case EBSI_FINISH:					
		EBSI_Finish();
		st_handler.mn_cok_chgstate[0 + m_nSiteID] = CTL_YES;
		break;
	}
}
