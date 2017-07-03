// APartHandler.cpp: implementation of the APartHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "APartHandler.h"
#include "../IO_Manager.h"
#include "../ComizoaPublic.h"
#include "APartHandler_COK.h"
#include "APartTray.h"
#include "APartRobot.h"
#include "APartTestSite.h"
#include "APartNetworkManager.h"
#include "../AMTLotManager.h"
#include "../AMTRegistry.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APartHandler	g_handler;

APartHandler::APartHandler()
{
	m_empos = EMPOS_FRONT;

	m_step_conv[0] = EC_NONE;
	m_step_conv[1] = EC_NONE;
	m_step_conv[2] = EC_NONE;

	m_step_ttrans = ETT_TO_NONE;

	m_epriority_bin = TPOS_NONE;
	m_esecondary_bin = TPOS_NONE;

	m_step_virtual_supply = EVS_NONE;

	m_nVedio = -1;

	m_bRejAlarm = false;

	m_bReady_SendNext = false;

	m_bRecv_NextMachine = true;

	m_nAlarmCnt = AMTRegistry::RegReadInt( REG_KEY_COUNT, REG_VAL_COUNT_ALARM );
}

APartHandler::~APartHandler()
{

}

void APartHandler::Thread_Run()
{
	Run_Move_SMEMA();

	Run_Bin_Stacker[2].CalcRearSmema();

	if( COMI.mn_run_status != dRUN )	
	{
		if( g_handler_cok.GetChangingCOK() )
		{
			g_handler_cok.ClearOldStep();
		}

		return;
	}

	if( g_handler_cok.GetChangingCOK() == false )
	{
		Run_Move_Main();

		Run_Move_Conv(0);
		Run_Move_Conv(1);
		Run_Move_Conv(2);

		Run_Move_Tray_Transfer();
		Run_Move_Robot();

	}
	else
	{
		g_handler_cok.Run_Move();
	}
}

void APartHandler::Run_Move_SMEMA()
{
	BOOL bRequest = IO_ON;

	if( COMI.mn_run_status != dRUN)										{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	if( g_handler_cok.GetChangingCOK() )								{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	if( Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_OFF )	
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; 
	}
	if( m_step_conv[0] != EC_NONE && m_step_conv[0] != EC_RECEIVE &&
		m_step_conv[0] != EC_RECEIVE_CHECK )							{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
// 	if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON )
// 																		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON || g_ioMgr.get_in_bit( st_io.i_bin_tray1_in_chk ) == IO_ON)
		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	//ybs	
	int iBin1_1 = g_Tray.GetIdxByPos( TPOS_BIN1 );	
	int iBin1_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN1 );
	int iBin2_1 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin2_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN2 );
	int iBin3_1 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int iBin3_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN3 );
	if(m_bReady_SendNext)
	{
		if(g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
		{
			g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
		}
		if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || m_bEmptyTray_SendNext == true || 
			g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
		{
// 			if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
// 			{
// 				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
// 			}
			//2013,0201
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
			{
				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
				{
					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
				}
			}
			else
			{
				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
				{
					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
				}
			}
		}
	}
	else
	{
		if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
		{
			//ybs
			if(m_strLotID_SendNext == "" && g_handler.GetMachinePos() == EMPOS_FRONT)
			{
				if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
				{
// 					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
// 					{				
// 						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
// 					}
					//2013,0201
					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
					{				
						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
					}
				}

			}
			//ybs
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
			{
				//2012,1128
// 				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
// 				{				
// 					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
// 				}
				if(g_handler.GetMachinePos() == EMPOS_FRONT)
				{
					if(st_handler.mn_emptynext_cnt >= 1 || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
					{
						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
						{				
							g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
						}
					}
					else if(st_handler.mn_emptynext_cnt < 1)
					{
						st_handler.mn_emptynext_cnt++;
					}
					else
					{
						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
					}
				}
				else
				{
					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
					{				
						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
					}
				}

			}
		}
	}
	
	if( //g_handler.GetMachinePos() == EMPOS_FRONT && 
		g_lotMgr.GetLotCount() > 1 &&
		g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" &&
		g_lotMgr.GetLotAt( 1 ).GetCokType() != st_basic.n_cok_type )
																		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
		
	if( m_bReady_SendNext )
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON );
		return;
	}

	// Load 트레이가 뒤로 간다면.... 뒤에 작업 트레이가 하나라도 있다면.
	if( g_handler.GetMachinePos() == EMPOS_FRONT &&
		m_bRecv_NextMachine == false && 
		(g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 ||
		g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1  || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1) &&
		( st_basic.mn_bypass_mode == BYPASS_FRONT ||
//		 (( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
//		( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 ) )) )
		 (( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
		( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 )  &&
		( g_Tray.GetIdxByPos( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN3 ) > -1 ) )))  //ybs B1,B2,B3 자재가 있으면 투입정지
	{
		for( int j=0; j<MAX_TRAY; j++ )
		{
			if( g_Tray.GetTray(j).GetPos() == TPOS_NONE )
				continue;
			
			if( g_Tray.GetTray(j).IsAllState( EMS_LOAD ) && g_Tray.GetTray(j).GetTargetPos() == TPOS_NEXT )
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
				return;
			}
		}
	}
	
	if( st_basic.mn_bypass_mode == BYPASS_ALL )
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); return;
	}

	// 타겟이 BIN1, BIN2 있고, (front )뒤에 꺼져 있으면 받지 않는다. 뒤에 Recv 중이면 ... 또는 뒷설비 BYPASS면
	if( ( g_handler.GetMachinePos() == EMPOS_REAR ||
		g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF ||
		m_bRecv_NextMachine ||
		( ( (g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode == BYPASS_REAR) ||
		  ( (g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1 ) &&
		  (g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ) &&
		  (g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1 ) ) ) ) ) &&
		
		  ( (g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode == BYPASS_FRONT) ||
//		  ( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
//		  ( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 ) ) )
		  ( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
		  ( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 ) &&
		  ( g_Tray.GetIdxByPos( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN3 ) > -1 )) ) //ybs B1,B2,B3 자재가 있으면 투입정지
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return;
	}


	g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); return;
}

void APartHandler::Run_Move_Conv( int nIdx )
{
	Func.ThreadFunctionStepTrace(41+nIdx, m_step_conv[nIdx]);
	switch( m_step_conv[nIdx] )
	{
	case EC_NONE:		break;

	// Receive
	case EC_RECEIVE:
		{
			Run_Bin_Stacker[nIdx].ReqRecvTray();
			m_step_conv[nIdx] = EC_RECEIVE_CHECK;
		}
		break;

	case EC_RECEIVE_CHECK:
		{
			if( Run_Bin_Stacker[nIdx].GetRecvTrayNone() )
				m_step_conv[nIdx] = EC_NONE;
		}
		break;

	// Up
	case EC_UP:
		Run_Bin_Stacker[nIdx].ReqTrayUp();
		m_step_conv[nIdx] = EC_UP_CHK;
		break;

	case EC_UP_CHK:
		if( Run_Bin_Stacker[nIdx].GetTrayUpNone() )
		{
			m_step_conv[nIdx] = EC_NONE;
		}
		break;

	// Down
	case EC_DOWN:
		Run_Bin_Stacker[nIdx].ReqTrayDown();
		m_step_conv[nIdx] = EC_DOWN_CHK;
		break;

	case EC_DOWN_CHK:
		if( Run_Bin_Stacker[nIdx].GetTrayDownNone() )
		{
			m_step_conv[nIdx] = EC_NONE;
		}
		break;

	// Send
	case EC_SEND:
		Run_Bin_Stacker[nIdx].ReqTraySend();
		m_step_conv[nIdx] = EC_SEND_CHK;
		break;

	case EC_SEND_CHK:
		if( Run_Bin_Stacker[nIdx].GetTraySendNone() )
		{
			m_step_conv[nIdx] = EC_NONE;
		}
		break;
	}
}

void APartHandler::Run_Move_Main()
{
	// Receive from Front
//2013,0513 	if( m_step_conv[0] == EC_NONE &&
// 		g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk) == IO_ON &&
// 		g_ioMgr.get_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ) == IO_ON &&
// 		st_handler.m_nfront_request == CTL_CLEAR)
// 	{
// 		ControlConv(0, EC_RECEIVE );
// 		return;
// 	}
	if( m_step_conv[0] == EC_NONE &&
		g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, 0, 100 ) == IO_ON &&
		g_ioMgr.get_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ) == IO_ON &&
		st_handler.m_nfront_request == CTL_CLEAR)
	{
		ControlConv(0, EC_RECEIVE );
		return;
	}

	Control_byTray();

	// Robot
	ControlRobot();

	Run_Move_Virtual_Supply();
}

void APartHandler::ControlConv( int iIdx, ESTEP_CONV ec )
{
	m_step_conv[iIdx] = ec;
}

void APartHandler::Control_byTray()
{
	bool bConvUsable[3];
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 ); //ybs
	bConvUsable[0] = (m_step_conv[0] == EC_NONE && m_step_conv[1] == EC_NONE);
	bConvUsable[1] = (m_step_conv[0] == EC_NONE && m_step_conv[1] == EC_NONE && m_step_conv[2] == EC_NONE);
	bConvUsable[2] = (m_step_conv[1] == EC_NONE && m_step_conv[2] == EC_NONE);

	for( int iTray=0; iTray<MAX_TRAY; iTray++ )
	{
		ETRAY_POS ePos = g_Tray.GetTray(iTray).GetPos();
		ETRAY_POS eTargetPos = g_Tray.GetTray(iTray).GetTargetPos();

		// EC_UP
		if( ePos >= TPOS_CONV1 && ePos <= TPOS_CONV3 && ETRAY_POS(ePos - TPOS_CONV1) == ETRAY_POS(eTargetPos - TPOS_BIN1) &&
			bConvUsable[ePos - TPOS_CONV1] &&
			Run_Reject_Tray_Transfer.m_step_to_rej == ETT_REJ_NONE && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_NONE )
		{
			//2013,1222
			if( g_Tray.GetIdxByPos( TPOS_BIN3 ) > -1 && eTargetPos == TPOS_BIN3 )
			{
				g_Tray.GetTray(iTray).SetTargetPos( TPOS_NEXT );
				if (st_handler.cwnd_list != NULL)
				{
					sprintf(st_msg.c_normal_msg, "ControlByTray1) %d EC_UP", ePos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
				}
			}
			else
			{
				ControlConv( ePos - TPOS_CONV1, EC_UP );
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "ControlByTray1) %d EC_UP", ePos);
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
			}
			return;
		}

		// EC_UP for REJECT
		if( ePos == TPOS_CONV3 && eTargetPos == TPOS_REJECT &&
			bConvUsable[ePos - TPOS_CONV1] &&
			Run_Reject_Tray_Transfer.m_step_to_rej == ETT_REJ_NONE && Run_Reject_Tray_Transfer.m_step_to_bin == ETT_BIN_NONE)
		{
			ControlConv( ePos - TPOS_CONV1, EC_UP );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray2) %d EC_UP", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}

		// EC_DOWN
		if( ePos >= TPOS_BIN1 && ePos <= TPOS_BIN3 && eTargetPos == TPOS_NEXT &&
			bConvUsable[ePos - TPOS_BIN1] &&
			g_Tray.GetIdxByPos( (ETRAY_POS)(TPOS_CONV1 + (ePos - TPOS_BIN1)) ) < 0 )
		{
			ControlConv( ePos - TPOS_BIN1, EC_DOWN );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray3) %d EC_DOWN", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}
		else if( (ePos == TPOS_BIN1 || ePos == TPOS_BIN2) && (eTargetPos == TPOS_BIN3 || eTargetPos == TPOS_REJECT) &&
			bConvUsable[ePos - TPOS_BIN1] &&
			g_Tray.GetIdxByPos( (ETRAY_POS)(TPOS_CONV1 + (ePos - TPOS_BIN1)) ) < 0 )
		{
			ControlConv( ePos - TPOS_BIN1, EC_DOWN );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray4) %d EC_DOWN", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}
		// RECV & SEND 1 to 2
		if( bConvUsable[0] && ePos == TPOS_CONV1 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN1) &&
			g_ioMgr.get_in_bit(st_io.i_bin_tray2_stop_position_chk ) == IO_OFF )
		{
			ControlConv( ePos - TPOS_CONV1, EC_SEND );
			ControlConv( ePos - TPOS_CONV1 + 1, EC_RECEIVE );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray5) %d EC_SEND EC_RECEIVE", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}
		// RECV & SEND 2 to 3
		if( bConvUsable[2] && ePos == TPOS_CONV2 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN2) &&
			g_ioMgr.get_in_bit(st_io.i_bin_tray3_stop_position_chk ) == IO_OFF &&
			( (g_Tray.IsWorkingRejToNext() == false && g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0) || eTargetPos == TPOS_NEXT ) )
		{
			ControlConv( ePos - TPOS_CONV1, EC_SEND );
			ControlConv( ePos - TPOS_CONV1 + 1, EC_RECEIVE );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray6) %d EC_SEND EC_RECEIVE", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}

		// Send to Next
		if( m_step_conv[2] == EC_NONE && ePos == TPOS_CONV3 && eTargetPos == TPOS_NEXT && 
			g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_ON )
		{
			ControlConv(2, EC_SEND );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray7) %d EC_SEND", 2, ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}

		// Transfer
		if( ePos == TPOS_BIN3 && eTargetPos == TPOS_REJECT && m_step_ttrans == ETT_TO_NONE )
		{
			m_step_ttrans = ETT_TO_REJECT;
			return;
		}
		//ybs
		//if( ePos == TPOS_BIN3 && eTargetPos == TPOS_BIN3 && m_step_ttrans == ETT_TO_NONE )
		//{
		//	m_step_ttrans = ETT_TRANSFER_TO_REJECT;
		//	return;
		//}

//ybs//		if( ePos == TPOS_REJECT && (eTargetPos == TPOS_NEXT || eTargetPos == TPOS_BIN3 ) && m_step_ttrans == ETT_TO_NONE && 
			//g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0 && m_step_conv[2] == EC_NONE )
		if( ePos == TPOS_REJECT && (eTargetPos == TPOS_NEXT || eTargetPos == TPOS_BIN3 ) && m_step_ttrans == ETT_TO_NONE && 
			g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0 && m_step_conv[2] == EC_NONE )
		{
			m_step_ttrans = ETT_TO_BIN3;
			return;
		}
		if( (ePos == TPOS_BIN3 || ePos == TPOS_REJECT) && (eTargetPos == TPOS_NEXT || eTargetPos == TPOS_BIN3 ) && m_step_ttrans == ETT_TO_NONE && 
			g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0 && m_step_conv[2] == EC_NONE )
		{
			m_step_ttrans = ETT_TO_BIN3;
			return;
		}
	}
}

void APartHandler::Run_Move_Tray_Transfer()
{
	Func.ThreadFunctionStepTrace(44, m_step_ttrans);
	switch( m_step_ttrans )
	{
	case ETT_TO_NONE:		break;

	// to Reject
	case ETT_TO_REJECT:
		Run_Reject_Tray_Transfer.ReqMoveToRej();
		m_step_ttrans = ETT_TO_REJECT_CHK;
		break;

	case ETT_TO_REJECT_CHK:
		if( Run_Reject_Tray_Transfer.GetMoveToRej() )
			m_step_ttrans = ETT_TO_NONE;
		break;

	// to Bin3
	case ETT_TO_BIN3:
		Run_Reject_Tray_Transfer.ReqMoveToBin();
		m_step_ttrans = ETT_TO_BIN3_CHK;
		break;

	case ETT_TO_BIN3_CHK:
		if( Run_Reject_Tray_Transfer.GetMoveToBin())
			m_step_ttrans = ETT_TO_NONE;
		break;

	case ETT_TRANSFER_TO_REJECT:
		Run_Reject_Tray_Transfer.ReqMoveTransferToRej();
		m_step_ttrans = ETT_TRANSFER_TO_REJECT_CHK;
		break;

	case ETT_TRANSFER_TO_REJECT_CHK:
		if( Run_Reject_Tray_Transfer.GetMoveTransferToRej())
			m_step_ttrans = ETT_TO_NONE;
		break;
	}
}

void APartHandler::CalcPriorityBinByIn( ETRAY_POS epos )
{
//	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 )
//		return;
//
//	ETRAY_POS posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
//	int nSideIdx = g_Tray.GetIdxByPos( posSide );
//
//	if( nSideIdx > -1 )
//		m_epriority_bin = posSide;
//	else
//		m_epriority_bin = epos;
	//ybs
// 	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 && epos != TPOS_BIN3 )
// 		return;
// 
// 	ETRAY_POS posSide;
// 	int nSideIdx;
// 
// 	if(epos == TPOS_BIN3) 
// 	{
// 		if(g_robot.GetJobCount() == 0)
// 		{
// 			m_esecondary_bin = m_epriority_bin;
// 			m_epriority_bin = TPOS_BIN3;
// 		}
// 	}
// 	else if(epos == TPOS_BIN1)
// 	{
// 	    nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
// 		if(nSideIdx > -1)
// 		{
// 			m_epriority_bin = TPOS_BIN3;
// 			nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
// 			if(nSideIdx > -1)
// 			{
// 				m_esecondary_bin = TPOS_BIN1;
// 			}
// 			else
// 			{
// 				m_esecondary_bin = TPOS_BIN2;
// 			}
// 		}
// 		else
// 		{
// 			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			if( nSideIdx > -1 )
// 			{
// 				m_epriority_bin = posSide;
// 				m_esecondary_bin = TPOS_BIN1;
// 			}
// 			else
// 			{
// 				m_epriority_bin = epos;
// 			}
// 		}
// 	}
// 	else if(epos == TPOS_BIN2)
// 	{
// 	    nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
// 		if(nSideIdx > -1)
// 		{
// 			m_epriority_bin = TPOS_BIN3;
// 			nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
// 			if(nSideIdx > -1)
// 			{
// 				m_esecondary_bin = TPOS_BIN1;
// 			}
// 			else
// 			{
// 				m_esecondary_bin = TPOS_BIN2;
// 			}
// 		}
// 		else
// 		{
// 			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			if( nSideIdx > -1 )
// 			{
// 				m_epriority_bin = posSide;
// 				m_esecondary_bin = TPOS_BIN2;
// 			}
// 			else
// 			{
// 				m_epriority_bin = epos;
// 			}
// 		}
// 	}
	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 && epos != TPOS_BIN3 )
		return;

	ETRAY_POS posSide;
	int nSideIdx;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
// 		if(epos == TPOS_BIN1)
// 		{
// 			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN3;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			
// 			if( nSideIdx > -1 )
// 			{
// 				m_epriority_bin = posSide;
// 			}
// 			else
// 			{
// 				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
// 				if( nSideIdx > -1) m_epriority_bin = TPOS_BIN3;
// 				else               m_epriority_bin = epos;
// 			}
// 			
// 		}
// 		else if(epos == TPOS_BIN2)
// 		{
// 			posSide = (epos == TPOS_BIN2) ? TPOS_BIN3 : TPOS_BIN1;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			
// 			if( nSideIdx > -1)
// 			{
// 				m_epriority_bin = posSide;
// 			}
// 			else
// 			{
// 				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
// 				if( nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN1;
// 				else                m_epriority_bin = epos;
// 			}
// 		}
// 		else if(epos == TPOS_BIN3)
// 		{
// 			posSide = (epos == TPOS_BIN3) ? TPOS_BIN1 : TPOS_BIN2;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			
// 			if( nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE)
// 			{
// 				m_epriority_bin = posSide;
// 			}
// 			else
// 			{
// 				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
// 				if( nSideIdx > -1 ) m_epriority_bin = TPOS_BIN2;
// 				else                m_epriority_bin = epos;
// 			}
// 		}
		if(epos == TPOS_BIN3)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN3;
			}
			else
			{
				if(m_epriority_bin == TPOS_BIN2)
				{
					posSide = TPOS_BIN2;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
						if( nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN1;
						else                m_epriority_bin = epos;
					}

				}
				else if(st_handler.mb_rej_3stacker == FALSE && m_epriority_bin == TPOS_BIN1)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN1;
					else                m_epriority_bin = epos;

				}
				else
					m_epriority_bin = epos;
			}
		}
		else if(epos == TPOS_BIN2)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN2;
			}
			else
			{
				if(st_handler.mb_rej_3stacker == FALSE && m_epriority_bin == TPOS_BIN1)
				{
					posSide = TPOS_BIN1;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
						if( nSideIdx > -1) m_epriority_bin = TPOS_BIN3;
						else               m_epriority_bin = epos;
					}
					
				}
				else if(m_epriority_bin == TPOS_BIN3)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN3;
					else               m_epriority_bin = epos;
					
				}
				else
					m_epriority_bin = epos;
			}
		}
		else if(st_handler.mb_rej_3stacker == FALSE && epos == TPOS_BIN1)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN1;
			}
			else
			{
				if(m_epriority_bin == TPOS_BIN3)
				{
					posSide = TPOS_BIN3;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
						if( nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
						else               m_epriority_bin = epos;
					}
					
				}
				else if(m_epriority_bin == TPOS_BIN2)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
					else               m_epriority_bin = epos;
					
				}
				else
					m_epriority_bin = epos;
			}
		}
	}
	else
	{
		if(epos == TPOS_BIN1)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN1;
			}
			else
			{
				if(m_epriority_bin == TPOS_BIN2)
				{
					posSide = TPOS_BIN2;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
						if( nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN3;
						else                m_epriority_bin = epos;
					}

				}
				else if(st_handler.mb_rej_3stacker == FALSE && m_epriority_bin == TPOS_BIN3)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN3;
					else                m_epriority_bin = epos;

				}
				else
					m_epriority_bin = epos;
			}
		}
		else if(epos == TPOS_BIN2)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN2;
			}
			else
			{
				if(st_handler.mb_rej_3stacker == FALSE && m_epriority_bin == TPOS_BIN3)
				{
					posSide = TPOS_BIN3;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
						if( nSideIdx > -1) m_epriority_bin = TPOS_BIN1;
						else               m_epriority_bin = epos;
					}
					
				}
				else if(m_epriority_bin == TPOS_BIN1)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN1;
					else               m_epriority_bin = epos;
					
				}
				else
					m_epriority_bin = epos;
			}
		}
		else if(st_handler.mb_rej_3stacker == FALSE && epos == TPOS_BIN3)
		{
			if( g_handler.GetPriorityBin() == TPOS_NONE )
			{
				m_epriority_bin = TPOS_BIN3;
			}
			else
			{
				if(m_epriority_bin == TPOS_BIN1)
				{
					posSide = TPOS_BIN1;
					nSideIdx = g_Tray.GetIdxByPos( posSide );
					
					if( nSideIdx > -1 )
					{
						m_epriority_bin = posSide;
					}
					else
					{
						nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
						if( nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
						else               m_epriority_bin = epos;
					}
					
				}
				else if(m_epriority_bin == TPOS_BIN2)
				{
					nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
					if( nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
					else               m_epriority_bin = epos;
					
				}
				else
					m_epriority_bin = epos;
			}

// 			posSide = (epos == TPOS_BIN3) ? TPOS_BIN1 : TPOS_BIN2;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			
// 			if( nSideIdx > -1 )
// 			{
// 				m_epriority_bin = posSide;
// 			}
// 			else
// 			{
// 				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
// 				if( nSideIdx > -1 ) m_epriority_bin = TPOS_BIN2;
// 				else                m_epriority_bin = epos;
// 			}
		}
	}
}

void APartHandler::CalcPriorityBinByOut( ETRAY_POS epos )
{
//	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 )
//		return;
//
//	ETRAY_POS posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
//	int nSideIdx = g_Tray.GetIdxByPos( posSide );
//
//	if( nSideIdx > -1 )
//		m_epriority_bin = posSide;
//	else
//		m_epriority_bin = TPOS_NONE;
//ybs
	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 && epos != TPOS_BIN3 )
		return;

	ETRAY_POS posSide;
	int nSideIdx;
	if(epos == TPOS_BIN3) 
	{
		m_epriority_bin = m_esecondary_bin;

		posSide = (m_epriority_bin == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
		nSideIdx = g_Tray.GetIdxByPos( posSide );
		if( nSideIdx > -1 )
			m_esecondary_bin = posSide;
		else
			m_esecondary_bin = TPOS_NONE;
	}
	else if(epos == TPOS_BIN1)
	{
	    nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
		if(nSideIdx > -1)
		{
			m_epriority_bin = TPOS_BIN3;
			m_esecondary_bin = TPOS_BIN2;
		}
		else
		{
			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
			nSideIdx = g_Tray.GetIdxByPos( posSide );			
			if( nSideIdx > -1 )
			{
				m_epriority_bin = posSide;
				posSide = (posSide == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
				m_esecondary_bin = posSide;
			}
			else
				m_epriority_bin = TPOS_NONE;
		}
	}
	else if(epos == TPOS_BIN2)
	{
	    nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
		if(nSideIdx > -1)
		{
			m_epriority_bin = TPOS_BIN3;
			m_esecondary_bin = TPOS_BIN2;
		}
		else
		{
			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
			nSideIdx = g_Tray.GetIdxByPos( posSide );			
			if( nSideIdx > -1 )
			{
				m_epriority_bin = posSide;
				posSide = (posSide == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN1;
				m_esecondary_bin = posSide;
			}
			else
				m_epriority_bin = TPOS_NONE;
		}
	}
}

void APartHandler::CalcPriorityBin( ETRAY_POS eFrom, ETRAY_POS eTo )
{
	ETRAY_POS posOld = m_epriority_bin;

//	if( eFrom == TPOS_BIN1 || eFrom == TPOS_BIN2 )
//		CalcPriorityBinByOut( eFrom );
//
//	if( eTo == TPOS_BIN1 || eTo == TPOS_BIN2 )
//		CalcPriorityBinByIn( eTo );
//ybs
	if( eFrom == TPOS_BIN1 || eFrom == TPOS_BIN2 || eFrom == TPOS_BIN3 )
		CalcPriorityBinByOut( eFrom );

	if( eTo == TPOS_BIN1 || eTo == TPOS_BIN2 || eTo == TPOS_BIN3 )
		CalcPriorityBinByIn( eTo );

	// 바뀌면, UI
	if( posOld != m_epriority_bin )
	{

	}
}

void APartHandler::ControlRobot()
{

	if( g_robot.GetJobCount() > 0 )
		return;

	// 잡고 있는게 없을 때
	if( g_robot.IsPicking() == false )
	{
		// std::vector<int> vecPreBarcode = g_Tray.GetVecPreBarcode();
		std::vector<int> vecEmptyInsert = g_site.GetVecEmptyInsert();
		std::vector<int> vecEmptyInsert_S = g_site.GetVecEmptyInsert_Single();

		std::vector<int> vecMustTestModule_A = g_Tray.GetVecMustTestModule(0);
		std::vector<int> vecTestEndInsert_A = g_site.GetVecTestEndInsert(0);
		std::vector<int> vecTrayEmptyCell_A = g_Tray.GetVecEmptyCell(0);
		std::vector<int> vecRejEmptyCell = g_Tray.GetVecRejEmptyCell();
		std::vector<int> vecFail = g_Tray.GetVecFailinTray();

		std::vector<int> vecMustTestModule_B = g_Tray.GetVecMustTestModule(1);
		std::vector<int> vecTestEndInsert_B = g_site.GetVecTestEndInsert(1);
		std::vector<int> vecTrayEmptyCell_B = g_Tray.GetVecEmptyCell(1);

		// Sort
		std::vector<int> vecSubGood = g_Tray.GetVecGoodCell_Sub();
		std::vector<int> vecPriEmpty = g_Tray.GetVecEmptyCell_Pri();

		// Barcode
		// if( CR_forBarcode( vecPreBarcode ) )	return;

		// A 랏
		if( CR_forReject( vecFail, vecRejEmptyCell ) )											return;				// 불량을 리젝트로 뺀다.
		if( CR_forMoveRejectDvc( vecFail, vecRejEmptyCell,  vecTrayEmptyCell_A) )				return;				// 불량을 리젝트로 뺀다.

		if( CR_forTest_Single( vecEmptyInsert_S, vecMustTestModule_A ) )						return;				// 한 쪽이 DISABLE 된 소켓에 보낸다.
		if( CR_forTest( 4, vecEmptyInsert, vecMustTestModule_A ) )								return;				// 테스트 할 모듈을 사이트에 보낸다.
		if( CR_forEject_4( vecTestEndInsert_A, vecTrayEmptyCell_A ) )							return;				// 테스트 완료된 것 트레이 빈 곳으로 뺀다.
		if( CR_forExchange( vecTestEndInsert_A, vecMustTestModule_A, vecEmptyInsert, 0 ) )		return;				// 테스트 할 것과, 테스트 완료된 것이 있으면 교체한다.
		if( CR_forTest( 2, vecEmptyInsert, vecMustTestModule_A ) )								return;				// 테스트 할 모듈을 사이트에 보낸다.
		if( CR_forEject_2( vecTestEndInsert_A, vecTrayEmptyCell_A ) )							return;				// 테스트 완료된 것 트레이 빈 곳으로 뺀다.

		// B 랏
		if( CR_forTest_Single( vecEmptyInsert_S, vecMustTestModule_B ) )						return;
		if( CR_forTest( 4, vecEmptyInsert, vecMustTestModule_B ) )								return;				// 테스트 할 모듈을 사이트에 보낸다.
		if( CR_forEject_4( vecTestEndInsert_B, vecTrayEmptyCell_B ) )							return;				// 테스트 완료된 것 트레이 빈 곳으로 뺀다.
		if( CR_forExchange( vecTestEndInsert_B, vecMustTestModule_B, vecEmptyInsert, 1 ) )		return;				// 테스트 할 것과, 테스트 완료된 것이 있으면 교체한다.
		if( CR_forTest( 2, vecEmptyInsert, vecMustTestModule_B ) )								return;				// 테스트 할 모듈을 사이트에 보낸다.
		if( CR_forEject_2( vecTestEndInsert_B, vecTrayEmptyCell_B ) )							return;				// 테스트 완료된 것 트레이 빈 곳으로 뺀다.

		// Sort ( Sub Tray에 있는 Good 을 Pri Tray 빈칸에 넣는다.
		if( CR_forSort( vecPriEmpty, vecSubGood ) )												return;

	}

}

void APartHandler::Run_Move_Robot()
{
	
	
}

void APartHandler::CalcTrayMoveNext_fromBin()
{
	int nTotal = st_basic.n_tray_x * st_basic.n_tray_y;
	int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	bool bBin1End = true;
	bool bBin2End = true;
	bool bBin3End = true;
	for( int i=0; i<nTotal; i++ )
	{
		if( iBin1 == -1 )														{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetModule(i).IsExist() == false )				{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetModule(i).GetModuleState() != EMS_GOOD )	{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetStrLotID() != g_lotMgr.GetLotIDAt(0) )		{bBin1End = false;		break;}
	}
	for( i=0; i<nTotal; i++ )
	{
		if( iBin2 == -1)														{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetModule(i).IsExist() == false )				{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetModule(i).GetModuleState() != EMS_GOOD )	{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetStrLotID() != g_lotMgr.GetLotIDAt(0) )		{bBin2End = false;		break;}
	}
	for( i=0; i<nTotal; i++ )
	{
		if( iBin3 == -1)														{bBin3End = false;		break;}
		if( g_Tray.GetTray(iBin3).GetModule(i).IsExist() == false )				{bBin3End = false;		break;}
		if( g_Tray.GetTray(iBin3).GetModule(i).GetModuleState() != EMS_GOOD )	{bBin3End = false;		break;}
		if( g_Tray.GetTray(iBin3).GetStrLotID() != g_lotMgr.GetLotIDAt(0) )		{bBin3End = false;		break;}
	}

	//ybs
	if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true))
	{
//		bBin1End = true;
	}
	if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true))
	{
//		bBin2End = true;
	}

	// 라스트 모듈일 때,
	if( iBin1 > -1 && bBin1End == false && 
		g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) && 
		g_lotMgr.GetLotCount() > 0 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" )
	{
		if( g_Tray.GetLoadModuleCnt() == 0 && g_site.GetModuleCnt() == 0 && g_robot.GetModuleCnt() == 0 &&
			g_Tray.GetTray( iBin1 ).IsAllGood( true ) )
		{
//			bBin1End = true;
			//ybs
			if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin2_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin3_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
				return;
			}
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "3)Bin1_TPOS_NEXT");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
			return;
		}
	}
	if( iBin2 > -1 && bBin2End == false && 
		g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) && 
		g_lotMgr.GetLotCount() > 0 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" )
	{
		if( g_Tray.GetLoadModuleCnt() == 0 && g_site.GetModuleCnt() == 0 && g_robot.GetModuleCnt() == 0 &&
			g_Tray.GetTray( iBin2 ).IsAllGood( true ) )
		{
//			bBin2End = true;
			//ybs
			if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin1_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin3_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
				return;
			}
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "3)Bin1_TPOS_NEXT");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

			g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
			return;
		}
	}
	if( iBin3 > -1 && bBin3End == false && 
		g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) && 
		g_lotMgr.GetLotCount() > 0 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" )
	{
		if( g_Tray.GetLoadModuleCnt() == 0 && g_site.GetModuleCnt() == 0 && g_robot.GetModuleCnt() == 0 &&
			g_Tray.GetTray( iBin3 ).IsAllGood( true ) )
		{
			//			bBin2End = true;
			//ybs
			if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin1_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty())
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin2_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
				return;
			}
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "3)Bin3_TPOS_NEXT");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			
			g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
			return;
		}
	}
	if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true) && !g_Tray.GetTray(iBin1).GetEmptyTray() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
		g_robot.GetJobCount() == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin1).GetTargetPos() != TPOS_NEXT)
	{
		st_handler.mn_emptyout_cnt++;
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Bin1 empty tray cnt = %d",st_handler.mn_emptyout_cnt);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}
		bBin1End = true;
		if (st_handler.cwnd_title != NULL)	
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
		}
	}
	if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true) && !g_Tray.GetTray(iBin2).GetEmptyTray() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
		g_robot.GetJobCount() == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin2).GetTargetPos() != TPOS_NEXT)
	{
		st_handler.mn_emptyout_cnt++;
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Bin2 empty tray cnt = %d",st_handler.mn_emptyout_cnt);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}
		bBin2End = true;
		if (st_handler.cwnd_title != NULL)	
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
		}
	}
	if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty(true) && !g_Tray.GetTray(iBin3).GetEmptyTray() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
		g_robot.GetJobCount() == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin3).GetTargetPos() != TPOS_NEXT)
	{
		st_handler.mn_emptyout_cnt++;
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "Bin3 empty tray cnt = %d",st_handler.mn_emptyout_cnt);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}
		bBin3End = true;
		if (st_handler.cwnd_title != NULL)	
		{
			st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
		}
	}

	
	if( st_handler.mn_virtual_supply )
	{
		if( bBin1End )
		{
			g_Tray.TrayMoved( TPOS_BIN1, TPOS_NEXT );
		}
		if( bBin2End )
		{
			g_Tray.TrayMoved( TPOS_BIN2, TPOS_NEXT );
		}
		if( bBin3End )
		{
			g_Tray.TrayMoved( TPOS_BIN3, TPOS_NEXT );
		}
	}
	else
	{
		if( bBin1End )
		{
			g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
		}
		if( bBin2End )
		{
			g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
		}
		if( bBin3End )
		{
			g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
		}
	}
}

void APartHandler::Run_Move_Virtual_Supply()
{
	if( st_handler.mn_virtual_supply == 0 )	
		return;

	Func.ThreadFunctionStepTrace(40, m_step_virtual_supply);
	switch( m_step_virtual_supply )
	{
	case EVS_NONE:		m_step_virtual_supply = EVS_CALC;	break;

	case EVS_CALC:
		if( g_Tray.IsEnablePos( TPOS_BIN1 ) || g_Tray.IsEnablePos( TPOS_BIN2 ) || g_Tray.IsEnablePos( TPOS_BIN3 ))
		{
			m_dwTime_virtual_supply = GetCurrentTime();
			m_step_virtual_supply = EVS_ADDTRAY;
		}
		break;

	case EVS_ADDTRAY:
		if( GetCurrentTime() - m_dwTime_virtual_supply > 5000 )
		{
			ETRAY_POS eTargetPos = TPOS_NONE;
			if( g_Tray.IsEnablePos( TPOS_BIN1 ) )
			{
				eTargetPos = TPOS_BIN1;
			}
			else if( g_Tray.IsEnablePos( TPOS_BIN2 ) )
			{
				eTargetPos = TPOS_BIN2;
			}
			else if( g_Tray.IsEnablePos( TPOS_BIN3 ) )
			{
				eTargetPos = TPOS_BIN3;
			}

			if( eTargetPos != TPOS_NONE )
			{
				ATray tray;
				tray.SetPos( TPOS_FRONT_TO_CONV1 );
				tray.SetTrayInfo( g_lotMgr.GetLotIDAt(0), 0, false, false );
				
				for( int i=0; i<MAX_MODULE; i++ )
				{
					
					CString strArraySN;		strArraySN.Format( "MZCPA2561127000000%02d_3", i);
					CString strSerial;		strSerial.Format( "S11ZNYAC5000%02d", i );
					CString strPPID;		strPPID.Format( "KR0NMY6F018511CJ0219A%02d", i);
					CString strWWN =		"";
					CString strCSN;			strCSN.Format( "YBS11000000%02d", i );
					CString strPSID;		strPSID.Format( "YYK22000000%02d", i );
					CString strBIN =		"";
					
					// 모듈 셋
					AModule mdl;
					mdl.SetModuleInfo( g_lotMgr.GetLotIDAt(0), strArraySN, strSerial, strPPID, strWWN, strBIN );
					tray.SetModule(i, mdl);
				}
				
				tray.SetTargetPos( eTargetPos );
				g_Tray.AddTray( tray );
				g_Tray.TrayMoved( TPOS_FRONT_TO_CONV1, eTargetPos );

				m_step_virtual_supply = EVS_FINISH;
			}
		}
		break;

	case EVS_FINISH:
		m_step_virtual_supply = EVS_NONE;
		break;
		
	}
}

bool APartHandler::IsSkipable_Z_Safety()
{
	if( g_robot.GetJobCount() < 2 )	
		return false;

	stJobPicker& job1 = g_robot.GetJob(0);
	stJobPicker& job2 = g_robot.GetJob(1);

	if( job1.GetPosByTarget() != job2.GetPosByTarget() )
		return false;

	if( job1.GetPosByTarget() == MPOS_INDEX_TESTSITE && job2.GetPosByTarget() == MPOS_INDEX_TESTSITE &&
		(job1.iPosFirst / 5) != (job2.iPosFirst / 5) )
		return false;

	job2.bUpSkip = true;
	return true;	
}

double APartHandler::CalcTargetPosition( int nAxis, BOOL bPick, int nTarget, int nPickerIdx )
{
	int nRobot = nPickerIdx;
	if( nPickerIdx < 0 )
		nPickerIdx = 0;

	int nTargetPart = GetPosPartByTarget( nTarget );

	int nPosStart = 0;
	int nPosEnd = 0;

	int nDivide = 1;
	nDivide = GetDivide(nAxis, nTarget );

	double dAdd = 0;
	double dGapPlace = 0;
	if( nAxis == MOTOR_ROBOT_Z )
	{
		switch( nTargetPart )
		{
		case MPOS_INDEX_SITE1:			nPosStart = P_Z_SITE1_PICK_START;		nPosEnd = P_Z_SITE1_PICK_END;	dGapPlace = GetPosRel_zPlace_Site();	break;
		case MPOS_INDEX_SITE2:			nPosStart = P_Z_SITE2_PICK_START;		nPosEnd = P_Z_SITE2_PICK_END;	dGapPlace = GetPosRel_zPlace_Site();	break;
		case MPOS_INDEX_COK_SITE1_0:	
		case MPOS_INDEX_COK_SITE1_1:	
			nPosStart = CT_ROBOT_SITE1_START_Z;	nPosEnd = CT_ROBOT_SITE1_END_Z;	dGapPlace = GetPosRel_zPlace_cokSite();	
			break;
			
		case MPOS_INDEX_COK_SITE2_0:	
		case MPOS_INDEX_COK_SITE2_1:	
			nPosStart = CT_ROBOT_SITE2_START_Z;	nPosEnd = CT_ROBOT_SITE2_END_Z;	dGapPlace = GetPosRel_zPlace_cokSite();	
			break;
			
		case MPOS_INDEX_COK_STACKER:
			nPosStart = CT_ROBOT_COK_START_Z;	nPosEnd = CT_ROBOT_COK_END_Z; dGapPlace = GetPosRel_zPlace_cokStacker();
			break;

		case MPOS_INDEX_BIN1:
			nPosStart = P_Z_TRAY1_PICK;	nPosEnd = P_Z_TRAY1_PICK;	dGapPlace = GetPosRel_zPlace_Tray();
			break;

		case MPOS_INDEX_BIN2:
			nPosStart = P_Z_TRAY2_PICK;	nPosEnd = P_Z_TRAY2_PICK;	dGapPlace = GetPosRel_zPlace_Tray();
			break;

		case MPOS_INDEX_BIN3://ybs
			nPosStart = P_Z_TRAY3_PICK;	nPosEnd = P_Z_TRAY3_PICK;	dGapPlace = GetPosRel_zPlace_Tray();
			break;

		case MPOS_INDEX_REJ:
			nPosStart = P_Z_REJ_PLACE;		nPosEnd = P_Z_REJ_PLACE;		//dGapPlace = GetPosRel_zPlace_Tray();
		}
	}
	else if( nAxis == MOTOR_ROBOT_X || nAxis == MOTOR_ROBOT_Y )
	{
		switch( nTargetPart )
		{
		case MPOS_INDEX_SITE1:		nPosStart = P_XY_SITE1_START;	nPosEnd = P_XY_SITE1_END;	break;
		case MPOS_INDEX_SITE2:		nPosStart = P_XY_SITE2_START;	nPosEnd = P_XY_SITE2_END;	break;
		case MPOS_INDEX_COK_SITE1_0:
		case MPOS_INDEX_COK_SITE1_1:
			nPosStart = CT_ROBOT_SITE1_START_X;	nPosEnd = CT_ROBOT_SITE1_END_X;
			if( nAxis == MOTOR_ROBOT_Y )
			{
				nPosStart = CT_ROBOT_SITE1_START_Y;	nPosEnd = CT_ROBOT_SITE1_END_Y;
			}
			break;

		case MPOS_INDEX_COK_SITE2_0:
		case MPOS_INDEX_COK_SITE2_1:
			nPosStart = CT_ROBOT_SITE2_START_X;	nPosEnd = CT_ROBOT_SITE2_END_X;
			if( nAxis == MOTOR_ROBOT_Y )
			{
				nPosStart = CT_ROBOT_SITE2_START_Y;	nPosEnd = CT_ROBOT_SITE2_END_Y;
			}
			break;

		case MPOS_INDEX_COK_STACKER:
			nPosStart = CT_ROBOT_COK_START_X;	nPosEnd = CT_ROBOT_COK_END_X;
			if( nAxis == MOTOR_ROBOT_Y )
			{
				nPosStart = CT_ROBOT_COK_START_Y;	nPosEnd = CT_ROBOT_COK_END_Y;
			}
			break;

		case MPOS_INDEX_REJ:
			nPosStart = P_XY_REJ_START;			nPosEnd = P_XY_REJ_END;			break;

		case MPOS_INDEX_BIN1:
			nPosStart = P_XY_TRAY1_START;		nPosEnd = P_XY_TRAY1_END;		break;

		case MPOS_INDEX_BIN2:
			nPosStart = P_XY_TRAY2_START;		nPosEnd = P_XY_TRAY2_END;		break;

		case MPOS_INDEX_BIN3://ybs
			nPosStart = P_XY_TRAY3_START;		nPosEnd = P_XY_TRAY3_END;		break;
		}
	}

	if( nAxis == MOTOR_ROBOT_Z && bPick == PLACE )
	{
		dAdd -= dGapPlace;
	}

	int nIdx = CalctargetIdx(nAxis, nTarget);

	int nTrayCell = nTarget % MPOS_DIVISION;

	if		( nAxis == MOTOR_ROBOT_X && nTargetPart == MPOS_INDEX_BIN1 )	dAdd += st_basic.md_tray_offset_1_x[nTrayCell];
	else if ( nAxis == MOTOR_ROBOT_X && nTargetPart == MPOS_INDEX_BIN2 )	dAdd += st_basic.md_tray_offset_2_x[nTrayCell];
	else if ( nAxis == MOTOR_ROBOT_X && nTargetPart == MPOS_INDEX_BIN3 )	dAdd += st_basic.md_tray_offset_3_x[nTrayCell]; //ybs
	else if ( nAxis == MOTOR_ROBOT_X && nTargetPart == MPOS_INDEX_REJ )		dAdd += st_basic.md_tray_offset_rej_x[nTrayCell];
	else if	( nAxis == MOTOR_ROBOT_Y && nTargetPart == MPOS_INDEX_BIN1 )	dAdd += st_basic.md_tray_offset_1_y[nTrayCell];
	else if ( nAxis == MOTOR_ROBOT_Y && nTargetPart == MPOS_INDEX_BIN2 )	dAdd += st_basic.md_tray_offset_2_y[nTrayCell];
	else if ( nAxis == MOTOR_ROBOT_Y && nTargetPart == MPOS_INDEX_BIN3 )	dAdd += st_basic.md_tray_offset_3_y[nTrayCell]; //ybs
	else if ( nAxis == MOTOR_ROBOT_Y && nTargetPart == MPOS_INDEX_REJ )		dAdd += st_basic.md_tray_offset_rej_y[nTrayCell];
	
	if( nAxis == MOTOR_ROBOT_Y && nTargetPart != MPOS_INDEX_COK_SITE1_0 &&
		nTargetPart != MPOS_INDEX_COK_SITE1_1 &&
		nTargetPart != MPOS_INDEX_COK_SITE2_0 &&
		nTargetPart != MPOS_INDEX_COK_SITE2_1 &&
		nTargetPart != MPOS_INDEX_COK_STACKER)
		dAdd += st_handler.md_picker_position[nPickerIdx];

	if( (nAxis == MOTOR_ROBOT_X || nAxis == MOTOR_ROBOT_Y) &&
		bPick == PLACE &&
		(nTargetPart == MPOS_INDEX_BIN1 || nTargetPart == MPOS_INDEX_BIN2 || nTargetPart == MPOS_INDEX_BIN3 || nTargetPart == MPOS_INDEX_REJ ) )
	{
		int nTPIdx = 0;
		if( nAxis == MOTOR_ROBOT_Y )
			nTPIdx = 1;

		dAdd += st_handler.md_tray_place_offset[nTPIdx];
	}

	double dStart = 0.0f;
	double dEnd = 0.0f;
	if( nTargetPart == MPOS_INDEX_COK_SITE1_0 ||
		nTargetPart == MPOS_INDEX_COK_SITE1_1 ||
		nTargetPart == MPOS_INDEX_COK_SITE2_0 ||
		nTargetPart == MPOS_INDEX_COK_SITE2_1 ||		
		nTargetPart == MPOS_INDEX_COK_STACKER )
	{
		dStart = st_handler.md_common_teaching[nPosStart];
		dEnd = st_handler.md_common_teaching[nPosEnd];
	}
	else
	{
		dStart = st_motor[nAxis].d_pos[nPosStart];
		dEnd = st_motor[nAxis].d_pos[nPosEnd];
	}

	double dGap = (dEnd - dStart) / (double)nDivide;

	double nRtn = dStart + dGap * (double)nIdx + (double)dAdd;

	double dTemp = 0;
	if( (nAxis == MOTOR_ROBOT_X || nAxis == MOTOR_ROBOT_Y ) )
	{
		// 테스트 사이트 
		if( (nTargetPart == MPOS_INDEX_SITE1 || nTargetPart == MPOS_INDEX_SITE2 ) &&
			( nIdx == 1 || nIdx == 2 || nIdx == 3 ) )
		{
			if( nTargetPart == MPOS_INDEX_SITE1)
			{
				nRtn = st_motor[nAxis].d_pos[ P_XY_SITE_1_2 + nIdx - 1 ] + (double)dAdd;
			}
			else
			{
				nRtn = st_motor[nAxis].d_pos[ P_XY_SITE_2_2 + nIdx - 1 ] + (double)dAdd;
			}
		}

		// 트레이 비틀림.
		if( nTargetPart == MPOS_INDEX_BIN1 || nTargetPart == MPOS_INDEX_BIN2 || nTargetPart == MPOS_INDEX_BIN3 || nTargetPart == MPOS_INDEX_REJ )
		{
			int npos = P_XY_TRAY1_CALC;
			if( nTargetPart == MPOS_INDEX_BIN2)		npos = P_XY_TRAY2_CALC;
			if( nTargetPart == MPOS_INDEX_BIN3)		npos = P_XY_TRAY3_CALC; //ybs
			if( nTargetPart == MPOS_INDEX_REJ )		npos = P_XY_REJ_CALC;
				
			int nRevAxis = MOTOR_ROBOT_X;
			double dOrignalPos = dStart;
			if( nAxis == MOTOR_ROBOT_X )
			{
				nRevAxis = MOTOR_ROBOT_Y;
				dOrignalPos = dEnd;
			}
			int nRev_Divide = GetDivide(nRevAxis, nTarget );
			int nRev_Idx = CalctargetIdx(nRevAxis, nTarget);

			double dTrayTwist = ( st_motor[nAxis].d_pos[npos] - dOrignalPos ) / (double)nRev_Divide;

			if( nAxis == MOTOR_ROBOT_X )
			{
				dTemp += dTrayTwist * (double)(nRev_Idx) / (double)nDivide * (double)(nDivide - nIdx);
				dTemp -= dTrayTwist * (double)(nRev_Divide - nRev_Idx) / (double)nDivide * (double)nIdx;
				nRtn -= dTemp;
			}
			else
			{
				dTemp += dTrayTwist * (double)(nRev_Idx) / (double)nDivide * (double)(nDivide - nIdx);
				dTemp -= dTrayTwist * (double)(nRev_Divide - nRev_Idx) / (double)nDivide * (double)nIdx;
				nRtn += dTemp;
			}
		}
	} 

	return nRtn;
}

int APartHandler::GetDivide( int nAxis, int nTarget )
{
	int nTargetPart = GetPosPartByTarget( nTarget );

	int nBin_x = st_basic.n_tray_x;
	int nBin_y = st_basic.n_tray_y;
	if( st_basic.n_degree == EDEG_90 ||
		st_basic.n_degree == EDEG_270 )
	{
		nBin_x = st_basic.n_tray_y;
		nBin_y = st_basic.n_tray_x;
	}
	
	switch( nTargetPart )
	{
	case MPOS_INDEX_SITE1:	
	case MPOS_INDEX_SITE2:	
		return (INSERT_CNT - 1);
		
	case MPOS_INDEX_COK_SITE1_0:
	case MPOS_INDEX_COK_SITE1_1:
	case MPOS_INDEX_COK_SITE2_0:
	case MPOS_INDEX_COK_SITE2_1:
		return (SITE_SOCKET_CNT - 1 - 1);
		
		
	case MPOS_INDEX_COK_STACKER:
		return (6 - 1);
	}
	
	if( nAxis == MOTOR_ROBOT_X )
	{
		switch( nTargetPart )
		{
		case MPOS_INDEX_BIN1:
		case MPOS_INDEX_BIN2:
		case MPOS_INDEX_BIN3://ybs
		case MPOS_INDEX_REJ:
			return nBin_x - 1;
		}
	}
	else if( nAxis == MOTOR_ROBOT_Y )
	{
		switch( nTargetPart )
		{
		case MPOS_INDEX_BIN1:
		case MPOS_INDEX_BIN2:
		case MPOS_INDEX_BIN3://ybs
		case MPOS_INDEX_REJ:
			return nBin_y - 1;
		}
	}
	return 1;
}

int APartHandler::CalctargetIdx( int nAxis, int nTarget )
{
	int nTargetPart = GetPosPartByTarget( nTarget );
	int nTotal = st_basic.n_tray_x * st_basic.n_tray_y;
	switch( nTargetPart )
	{
	case MPOS_INDEX_SITE1:
	case MPOS_INDEX_SITE2:
		return nTarget % INSERT_CNT;

	case MPOS_INDEX_COK_SITE1_0:
	case MPOS_INDEX_COK_SITE1_1:
	case MPOS_INDEX_COK_SITE2_0:
	case MPOS_INDEX_COK_SITE2_1:
		return nTarget % SITE_SOCKET_CNT;
		
	case MPOS_INDEX_BIN1:
	case MPOS_INDEX_BIN2:
	case MPOS_INDEX_BIN3://ybs
	case MPOS_INDEX_REJ:
		{
			if( nAxis == MOTOR_ROBOT_X )
			{
				switch( st_basic.n_degree )
				{
				case EDEG_0:	return (nTarget % MPOS_DIVISION) % st_basic.n_tray_x;
				case EDEG_90:	return (nTarget % MPOS_DIVISION) / st_basic.n_tray_x;
				case EDEG_180:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) % st_basic.n_tray_x;
				case EDEG_270:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) / st_basic.n_tray_x;
				}
			}
			else if( nAxis == MOTOR_ROBOT_Y )
			{
				switch( st_basic.n_degree )
				{
				case EDEG_0:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) / st_basic.n_tray_x;
 				case EDEG_90:	return (nTarget % MPOS_DIVISION) % st_basic.n_tray_x;
		 		case EDEG_180:	return (nTarget % MPOS_DIVISION) / st_basic.n_tray_x;
		 		case EDEG_270:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) % st_basic.n_tray_x;
				}
			}
		}

	default:
		return (nTarget % MPOS_DIVISION );
	}
	return 0;
}

double APartHandler::GetPosRel_Cok( int nAxis )
{
	if( nAxis == MOTOR_ROBOT_X )
		return -(st_motor[MOTOR_ROBOT_X].d_pos[P_XY_SITE1_START] - st_motor[MOTOR_ROBOT_X].d_pos[P_XY_COK_SITE1_START]);
	else if( nAxis == MOTOR_ROBOT_Y )
		return -(st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START] - st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_COK_SITE1_START]);
	else if( nAxis == MOTOR_ROBOT_Z )
		return -(st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_COK_SITE1_PICK_START]);

	return 0.0f;
}

double APartHandler::GetPosRel_zPlace_Site()
{
	return st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PICK_START] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_SITE1_PLACE_START];
}

double APartHandler::GetPosRel_zPlace_Tray()
{
	return st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PICK] - st_motor[MOTOR_ROBOT_Z].d_pos[P_Z_TRAY1_PLACE];
}

double APartHandler::GetPosRel_zPlace_cokSite()
{
	return st_handler.md_common_teaching[CT_ROBOT_SITE1_START_Z] - st_handler.md_common_teaching[CT_ROBOT_PLACE_Z];
}

double APartHandler::GetPosRel_zPlace_cokStacker()
{
	return st_handler.md_common_teaching[CT_ROBOT_SITE1_START_Z] - st_handler.md_common_teaching[CT_ROBOT_PLACE_Z];
}

bool APartHandler::CR_forExchange( std::vector<int> vecTestEndInsert, std::vector<int> vecMustTestMdl, std::vector<int> vecEmptyInsert, int nLotIdx )
{
	if( st_basic.n_pick_type != EPT_1V1M_4USE )
		return false;
	
	std::vector<int> vecEnableTest;
	if( vecTestEndInsert.size() >= 1 )
	{
// 		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == false ||
// 			g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == false )
		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) <= 0 ||
			g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) <= 0 )
		{
			return false;
		}

		for( int i=0; i< vecMustTestMdl.size(); i++ )
		{

			int nPos = GetPosByTarget( vecMustTestMdl[i] );
			ETRAY_POS tpos = TPOS_BIN1;
			if( nPos == MPOS_INDEX_BIN2 )
				tpos = TPOS_BIN2;
			if( nPos == MPOS_INDEX_BIN3 )//ybs
				tpos = TPOS_BIN3;

			int nTrayIdx = g_Tray.GetIdxByPos( tpos );
			ATray& tray = g_Tray.GetTray( nTrayIdx );
			AModule& tmdl = tray.GetModule( vecMustTestMdl[i] % MPOS_DIVISION );
				
			bool bEnable = true;
			if( tmdl.GetModuleState() == EMS_RELOAD )
			{
// 				for( int it=0; it< tmdl.GetTestCnt(); it++ )
// 				{
// 					if( tmdl.GetTestedPos(it) / 2 == vecTestEndInsert[0] )
// 					{
// 						bEnable = false;
// 						break;
// 					}
// 				}
			}
			if( bEnable )
			{
				vecEnableTest.push_back( vecMustTestMdl[i] );
			}
		}

		if( vecEnableTest.size() < 2 )		return false;

		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );

		AModule mdl[2];
		mdl[0] = g_site.GetModule(vecTestEndInsert[0] * 2);
		mdl[1] = g_site.GetModule(vecTestEndInsert[0] * 2 + 1);
			
		int nTestEndCnt = 0;
		int nRobotIdx_forWork[2];
		for( i=0; i<2; i++ )
		{
			if( mdl[i].IsExist() )
			{
				nTestEndCnt++;
				nRobotIdx_forWork[i] = i * 2 + 1;
			}
			else
			{
				nRobotIdx_forWork[i] = -1;
			}
		}

		int nTargetInsert = vecTestEndInsert[0];
		if( vecEmptyInsert.size() > 0 )
			nTargetInsert = vecEmptyInsert[0];
				
		g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[1], vecTestEndInsert[0] );
		g_robot.AddJob( JOB_PLACE, 0, 2, nTargetInsert );

		std::vector<int> vecEmptyCell = g_Tray.GetVecEmptyCell( nLotIdx, vecEnableTest[0], vecEnableTest[1] );

		int nvecidx = 0;
		for( i=0; i<2; i++ )
		{
			if( mdl[i].GetModuleState() == EMS_GOOD )
			{
				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
				nvecidx++;
			}
		}
		for( i=0; i<2; i++ )
		{
			if( mdl[i].GetModuleState() == EMS_RELOAD )
			{
				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
				nvecidx++;
			}
		}
		for( i=0; i<2; i++ )
		{
			if( mdl[i].GetModuleState() == EMS_FAIL )
			{
				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
				nvecidx++;
			}
		}
		return true;

	}
	return false;
}

bool APartHandler::CR_forTest( int nPickerCnt, std::vector<int> vecEmptyInsert, std::vector<int> vecMustTestMdl )
{
	if( st_basic.n_pick_type != EPT_1V1M_4USE && nPickerCnt == 4 )
		return false;

	// 빈 사이트가 2개 있고, 테스트할 모듈 4개 있다.
	std::vector<int> vecEnableTest;
	for( int i=0; i< vecMustTestMdl.size(); i++ )
	{
		int nPos = GetPosByTarget( vecMustTestMdl[i] );
		ETRAY_POS tpos = TPOS_BIN1;
		if( nPos == MPOS_INDEX_BIN2 )
			tpos = TPOS_BIN2;
		if( nPos == MPOS_INDEX_BIN3 )//ybs
			tpos = TPOS_BIN3;
		
		int nTrayIdx = g_Tray.GetIdxByPos( tpos );
		ATray& tray = g_Tray.GetTray( nTrayIdx );
		AModule& tmdl = tray.GetModule( vecMustTestMdl[i] % MPOS_DIVISION );
		
		bool bEnable = true;
		int nChkInsert = vecEnableTest.size() / 2;
		if( nChkInsert >= vecEmptyInsert.size() )
			break;

		if( tmdl.GetModuleState() == EMS_RELOAD )
		{
			
// 			for( int it=0; it< tmdl.GetTestCnt(); it++ )
// 			{
// 				if( tmdl.GetTestedPos(it) / 2 == vecEmptyInsert[nChkInsert] )
// 				{
// 					bEnable = false;
// 					break;
// 				}
// 			}
		}
		if( bEnable )
		{
			vecEnableTest.push_back( vecMustTestMdl[i] );
		}
	}


	if( nPickerCnt == 4 && vecEnableTest.size() >= 4 )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PICK, 1, -1, vecEnableTest[1] );
		g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[2] );
		g_robot.AddJob( JOB_PICK, 3, -1, vecEnableTest[3] );
		
		g_robot.AddJob( JOB_PLACE, 0, 2, vecEmptyInsert[0] );
		g_robot.AddJob( JOB_PLACE, 1, 3, vecEmptyInsert[1] );
		return true;
	}

	if( nPickerCnt == 2 && vecEnableTest.size() >= 2 )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );
		
		g_robot.AddJob( JOB_PLACE, 0, 2, vecEmptyInsert[0] );
		return true;
	}

	if( nPickerCnt == 2 && //g_lotMgr.GetLotCount() > 0 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" &&
		vecEnableTest.size() == 1 /*&& g_Tray.GetLoadModuleCnt() == 1*/ )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, vecEmptyInsert[0] );
		return true;
	}

	return false;
}

bool APartHandler::CR_forEject_4( std::vector<int> vecTestEndInsert, std::vector<int> vecEmptyCell )
{
	if( st_basic.n_pick_type != EPT_1V1M_4USE )
		return false;

	// 인서트 두 개.
	do 
	{
		if( vecTestEndInsert.size() >= 2 )
		{
			AModule mdl[4];
			mdl[0] = g_site.GetModule(vecTestEndInsert[0] * 2);
			mdl[1] = g_site.GetModule(vecTestEndInsert[1] * 2);
			mdl[2] = g_site.GetModule(vecTestEndInsert[0] * 2 + 1);
			mdl[3] = g_site.GetModule(vecTestEndInsert[1] * 2 + 1);
			
			int nTestEndCnt = 0;
			int nRobotIdx_forWork[4];
			for( int i=0; i<4; i++ )
			{
				if( mdl[i].IsExist() )
				{
					nTestEndCnt++;
					nRobotIdx_forWork[i] = i;
				}
				else
				{
					nRobotIdx_forWork[i] = -1;
				}
			}
			
			if( vecEmptyCell.size() < nTestEndCnt )
				break;
			
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[2], vecTestEndInsert[0] );
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[1], nRobotIdx_forWork[3], vecTestEndInsert[1] );
			
			int nvecidx = 0;
			for( i=0; i<4; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_GOOD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<4; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_RELOAD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<4; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_FAIL )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			
			return true;
		}
	} while ( false );

	return false;
}

bool APartHandler::CR_forEject_2( std::vector<int> vecTestEndInsert, std::vector<int> vecEmptyCell )
{
	// 인서트 한 개
	do 
	{
		if( vecTestEndInsert.size() > 0 )
		{
			AModule mdl[2];
			mdl[0] = g_site.GetModule(vecTestEndInsert[0] * 2);
			mdl[1] = g_site.GetModule(vecTestEndInsert[0] * 2 + 1);
			
			int nTestEndCnt = 0;
			int nRobotIdx_forWork[2];
			for( int i=0; i<2; i++ )
			{
				if( mdl[i].IsExist() )
				{
					nTestEndCnt++;
					nRobotIdx_forWork[i] = i * 2;
				}
				else
				{
					nRobotIdx_forWork[i] = -1;
				}
			}
			
			if( vecEmptyCell.size() < nTestEndCnt )
				break;
			
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[1], vecTestEndInsert[0] );
		
			int nvecidx = 0;
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_GOOD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_RELOAD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_FAIL )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			
			return true;
		}
	} while ( false );
	

	return false;
}

bool APartHandler::CR_forReject( std::vector<int> vecFail, std::vector<int> vecEmptyRej )
{
	if( GetStepTTrans() != ETT_TO_NONE )
		return false;

	if( vecEmptyRej.size() == 0 )
		return false;

	if( vecFail.size() > 0 && vecEmptyRej.size() > 0 )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, vecFail[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, vecEmptyRej[0] );
		return true;
	}
	else if( vecFail.size() > 0 && g_Tray.GetRejCnt() == 0 && GetRejAlarm() == false )
	{
		st_msg.mstr_event_msg[0] = "불량 모듈을 담을 리젝트 트레이가 없습니다.";
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);

		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

		SetRejAlarm( true );
		return false;
	}
	return false;
}

bool APartHandler::CR_forMoveRejectDvc( std::vector<int> vecFail, std::vector<int> vecEmptyRej, std::vector<int> vecEmptyCell )
{
	if( vecEmptyRej.size() == 0 )
	{
		if( vecFail.size() > 0)
		{
			if(vecFail[0] >= MPOS_INDEX_BIN3)
			{
				g_robot.AddJob( JOB_PICK, 0, -1, vecFail[0] );
				g_robot.AddJob( JOB_PLACE, 0, -1, vecEmptyCell[0]);
				return true;
			}
		}
	}

	return false;
}

void APartHandler::CalcTrayMoveNext_fromBin3()
{
	int iIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
	if( iIdx < 0 )											return;

	if( g_lotMgr.GetLotCount() == 0 )						return;
	if( g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )	return;
	if( g_Tray.GetLoadModuleCnt() > 0 )						return;
	if( g_site.GetModuleCnt() > 0 )							return;
	if( g_robot.GetModuleCnt() > 0 )						return;

	std::vector<int> vecFailInTray = g_Tray.GetVecFailinTray();
	if( vecFailInTray.size() > 0 )	
		return;

	for( int i=0; i<MAX_TRAY; i++ )
	{
		ATray& tray = g_Tray.GetTray( i );
		if( tray.GetPos() != TPOS_REJECT && tray.GetPos() != TPOS_BIN3 && tray.GetPos() != TPOS_NONE )
		{
			return;
		}
	}

	ATray& trayBin3 = g_Tray.GetTray( iIdx );
	if( trayBin3.GetEmptyTray() )							return;

	trayBin3.SetTargetPos( TPOS_NEXT );
}

void APartHandler::CalcTrayMoveNext_fromReject()
{
	if( g_Tray.GetSendNextFromRejForce() == false && g_lotMgr.GetLotCount() == 0 )						return;
	if( g_Tray.GetSendNextFromRejForce() == false && g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )	return;
	if( g_Tray.GetSendNextFromRejForce() == false && g_Tray.GetLoadModuleCnt() > 0 )						return;
	if( g_Tray.GetSendNextFromRejForce() == false && g_site.GetModuleCnt() > 0 )							return;
	if( g_Tray.GetSendNextFromRejForce() == false && g_robot.GetModuleCnt() > 0 )							return;

	std::vector<int> vecFailInTray = g_Tray.GetVecFailinTray();
	if( g_Tray.GetSendNextFromRejForce() == false && vecFailInTray.size() > 0 )							return;

	if( g_Tray.GetSendNextFromRejForce() == false )
	{
		for( int i=0; i<MAX_TRAY; i++ )
		{
			ATray& tray = g_Tray.GetTray( i );
			if( tray.GetPos() != TPOS_REJECT && tray.GetPos() != TPOS_NONE &&
				tray.GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				return;
			}
		}
	}

	for( int i=0; i<MAX_TRAY; i++ )
	{
		ATray& tray = g_Tray.GetTray( i );
		if( tray.GetPos() == TPOS_REJECT && tray.GetTargetPos() == TPOS_REJECT )
		{
			if( tray.GetEmptyTray() )
				continue;

			tray.SetTargetPos( TPOS_NEXT );
		}
	}
}

void APartHandler::CheckLotEnd()
{
	if( g_lotMgr.GetLotCount() == 0 )								return;
	if( g_Tray.GetTrayCnt_byLotID( g_lotMgr.GetLotIDAt(0) ) > 0 )	return;
	if( g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )			return;

	if( g_handler.GetMachinePos() == EMPOS_REAR ||
		g_handler.GetMachinePos() == EMPOS_ALONE )
		g_client_ec.OnTestEnd( g_lotMgr.GetLotIDAt(0), g_lotMgr.GetLotAt(0).GetPartID() );

	g_client_bpc.SetOnlineEnd( g_lotMgr.GetLotIDAt(0) );

	if( st_basic.mn_mode_xgem == CTL_YES )
	{
		g_client_xgem.SetLotEndStatus(g_lotMgr.GetLotAt(0).GetLotID(), g_lotMgr.GetLotAt(0).GetPartID());
	}
}

bool APartHandler::Chk_VarRail()
{
	int iOn = IO_OFF;
	if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180 )
	{
		iOn = IO_ON;
	}

	if( g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_variable_fwd, iOn ) == iOn &&
		g_ioMgr.get_out_bit( st_io.o_bin_stacker_rail_variable_bwd, !iOn ) != iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_fwd_chk, iOn ) == iOn &&
		g_ioMgr.get_in_bit( st_io.i_bin_stacker_rail_variable_bwd_chk, !iOn ) != iOn )
	{
		return true;
	}

	CString strJamCode = GetJamCode_Variable( iOn );
	CTL_Lib.Alarm_Error_Occurrence( 704, dWARNING, strJamCode );
	return false;
}

void APartHandler::CalcTrayMoveNext_fromRejFull()
{
	if( g_Tray.GetRejCnt() > 0 && g_Tray.GetVecRejEmptyCell().size() == 0 )
	{
		// 맨 위 트레이를 뺀다.
		ATray& trayTop = g_Tray.GetRejTopTray();
		trayTop.SetTargetPos( TPOS_NEXT );

		if( g_Tray.GetRejCnt() > 1 )
			return;

		// pri 트레이가 비었으면 empty 트레이로 설정하고
		// next로 보낸다.
		bool bBin1Send = false;
		if( g_handler.GetPriorityBin() == TPOS_BIN2 )
		{
			int iIdxBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
			if( iIdxBin1 > -1 )
			{
				ATray& trayBin1 = g_Tray.GetTray( iIdxBin1 );
				if( trayBin1.GetModuleCnt() == 0 )
				{
					trayBin1.SetTargetPos( TPOS_NEXT );
					bBin1Send = true;
				}
			}
		}

		if( bBin1Send == false &&
			g_handler.GetPriorityBin() == TPOS_BIN1 )
		{
			int iIdxBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
			if( iIdxBin2 > -1 )
			{
				ATray& trayBin2 = g_Tray.GetTray( iIdxBin2 );
				if( trayBin2.GetModuleCnt() == 0 )
				{
					trayBin2.SetTargetPos( TPOS_NEXT );
				}
			}
		}
	}
}

void APartHandler::AddAlarmCnt()
{
	 m_nAlarmCnt++;
	 AMTRegistry::RegWriteInt( REG_KEY_COUNT, REG_VAL_COUNT_ALARM, m_nAlarmCnt );
}

void APartHandler::ClearAlarmCnt()
{
	m_nAlarmCnt = 0;
	 AMTRegistry::RegWriteInt( REG_KEY_COUNT, REG_VAL_COUNT_ALARM, m_nAlarmCnt );
}

bool APartHandler::CR_forTest_Single( std::vector<int> vecEmptyInsert_Single, std::vector<int> vecMustTestMdl )
{
	if( vecEmptyInsert_Single.size() == 0 || vecMustTestMdl.size() == 0 )
		return false;

	if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2) > 0 /*g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2)*/ )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, vecMustTestMdl[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, vecEmptyInsert_Single[0] );
		return true;
	}
	else if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2 + 1) > 0 /*g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2 + 1)*/ )
	{
		g_robot.AddJob( JOB_PICK, 2, -1, vecMustTestMdl[0] );
		g_robot.AddJob( JOB_PLACE, -1, 2, vecEmptyInsert_Single[0] );
		return true;
	}

	return false;
}

bool APartHandler::CR_forSort( std::vector<int> vecPriEmptyCell, std::vector<int> vecSubGoodCell )
{
//	if( vecPriEmptyCell.size() == 0 || vecSubGoodCell.size() == 0 )
//		return false;
//
//	int iIdx1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
//	int iIdx2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
//	if( iIdx1 < 0 || iIdx2 < 0 )
//		return false;
//
//	ATray trayBin1 = g_Tray.GetTray( iIdx1 );
//	ATray trayBin2 = g_Tray.GetTray( iIdx2 );
//
//	if( trayBin1.GetStrLotID() != trayBin2.GetStrLotID() )
//		return false;
//
//
//	g_robot.AddJob( JOB_PICK, 0, -1, vecSubGoodCell[0] );
//	g_robot.AddJob( JOB_PLACE, 0, -1, vecPriEmptyCell[0] );
//	return true;

	if( vecPriEmptyCell.size() == 0 || vecSubGoodCell.size() == 0 )
		return false;

	int iIdx1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
	int iIdx2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iIdx3 = g_Tray.GetIdxByPos( TPOS_BIN3 );  //ybs
	if( (iIdx1 < 0 && iIdx2 < 0) || (iIdx2 < 0 && iIdx3 < 0) || (iIdx3 < 0 && iIdx1 < 0) )
		return false;

	ATray trayBin1;
	ATray trayBin2;
	ATray trayBin3;

	if(iIdx1 > -1)
	{
		trayBin1 = g_Tray.GetTray( iIdx1 );
	}
	if(iIdx2 > -1)
	{
		trayBin2 = g_Tray.GetTray( iIdx2 );
	}
	if(iIdx3 > -1)
	{
		trayBin3 = g_Tray.GetTray( iIdx3 ); //ybs
	}

	if(iIdx1 > -1 && iIdx2 > -1 && iIdx3 > -1)
	{
		if( trayBin1.GetStrLotID() != trayBin2.GetStrLotID() && trayBin2.GetStrLotID() != trayBin3.GetStrLotID() &&
			trayBin3.GetStrLotID() != trayBin1.GetStrLotID())
			return false;
		
		if( trayBin1.GetStrLotID() == trayBin2.GetStrLotID() )
		{
		}
		else if( trayBin2.GetStrLotID() == trayBin3.GetStrLotID() )
		{
		}
		else if( trayBin3.GetStrLotID() == trayBin1.GetStrLotID())
		{
		}
		else
		{
			return false;
		}
	}
	else if(iIdx1 > -1 && iIdx2 > -1)
	{
		if( trayBin1.GetStrLotID() != trayBin2.GetStrLotID() )
			return false;
	}
	else if(iIdx2 > -1 && iIdx3 > -1)
	{
		if( trayBin2.GetStrLotID() != trayBin3.GetStrLotID() )
			return false;
	}
	else if(iIdx3 > -1 && iIdx1 > -1)
	{
		if( trayBin3.GetStrLotID() != trayBin1.GetStrLotID() )
			return false;
	}
	else
	{
		return false;
	}


	g_robot.AddJob( JOB_PICK, 0, -1, vecSubGoodCell[0] );
	g_robot.AddJob( JOB_PLACE, 0, -1, vecPriEmptyCell[0] );
	return true;
}
