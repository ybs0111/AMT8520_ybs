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

#include "../SrcBase\ALocalization.h"


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
	m_step_trayin = 0;
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
		if(st_handler.m_nfront_smema == CTL_NO) { g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); }
		else                                    { g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); }
		//2013,0525
// 		if(st_handler.m_nfront_smema == CTL_NO || st_handler.m_nfront_request == CTL_CHANGE || st_handler.m_nfront_request == CTL_FREE)
// 		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); }
// 		else                                    { g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); }
		if( g_handler_cok.GetChangingCOK() )
		{
			g_handler_cok.ClearOldStep();
		}

		return;
	}

	if( g_handler_cok.GetChangingCOK() == false )
	{
// 		Run_Move_SMEMA();
// 		Run_Bin_Stacker[2].CalcRearSmema();
		if(st_handler.mb_rej_3stacker == TRUE || m_step_ttrans == ETT_TO_BIN3 || m_step_ttrans == ETT_TO_BIN3_CHK)//2013,0814 ybs
		{
			g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
		}
		else if(st_handler.m_nfront_smema == CTL_NO && st_handler.m_nfront_request == CTL_CHANGE)
		{ 
			g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
		}
		else if(st_handler.m_nfront_smema == CTL_YES && st_handler.m_nfront_request == CTL_CLEAR )
		{
			if(st_handler.mb_rej_3stacker != TRUE && m_step_ttrans != ETT_TO_BIN3 && m_step_ttrans != ETT_TO_BIN3_CHK)//2013,0814 ybs
			{
				st_handler.m_nfront_request = CTL_REQ;
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON );
			}
			else
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
			}
		}
		else if(st_handler.m_nfront_request == CTL_FREE)
		{
			//if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, 0, 100 ) == IO_OFF)
			//{
				st_handler.m_nfront_request = CTL_CLEAR;
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
			//}



		}
		else                                    
		{ 
			if((st_handler.m_nfront_smema == CTL_YES && st_handler.m_nfront_request == CTL_REQ) || 
				st_handler.m_nfront_request == CTL_READY)
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON );
			}
			else
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
			}
		}
		Run_Move_Main();
		
		Run_Move_Conv(0);
		Run_Move_Conv(1);
		Run_Move_Conv(2);
		
		Run_Move_Tray_Transfer();
		Run_Move_Robot();


	}
	else
	{
		if(st_handler.m_nfront_smema == CTL_NO || st_handler.m_nfront_request == CTL_CHANGE)
		{
			g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
		}
		else
		{ 
			if((st_handler.m_nfront_smema == CTL_YES && st_handler.m_nfront_request == CTL_REQ) || 
				st_handler.m_nfront_request == CTL_READY)
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); 
			}
			else
			{
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF );
			}
		}
		g_handler_cok.Run_Move();
	}
}

void APartHandler::Run_Move_SMEMA()
{
/*	BOOL bRequest = IO_ON;

	st_handler.m_nfront_smema = CTL_YES;

	if( COMI.mn_run_status != dRUN)						                { g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
	if( g_handler_cok.GetChangingCOK() )								{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
// 	if( Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_OFF)		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	if( Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_OFF)// || Run_Bin_Stacker[0].m_nfront_smema == CTL_YES)
	{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
	if( m_step_conv[0] != EC_NONE && m_step_conv[0] != EC_RECEIVE &&
		m_step_conv[0] != EC_RECEIVE_CHECK )							{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
	if(g_handler.GetMachinePos() == EMPOS_FRONT)
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON || g_ioMgr.get_in_bit( st_io.i_bin_tray1_in_chk ) == IO_ON ||
			Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_ON)
		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
	}
	else
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON)
		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
	}
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
			g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
		}
		if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || m_bEmptyTray_SendNext == true || 
			g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
		{
// 			if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
// 			{
// 				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
// 			}
			//2013,0201
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
			{
				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
				{
					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
				}
			}
			else
			{
				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
				{
					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
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
// 						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
// 					}
					//2013,0201
					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
					{				
						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
					}
				}

			}
			//ybs
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
			{
				//2012,1128
// 				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
// 				{				
// 					g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
// 				}
				if(g_handler.GetMachinePos() == EMPOS_FRONT)
				{
// 					//2012,1210
// 					if( st_handler.mn_emptynext_cnt == -1)
// 					{
// 						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
// 					}
					if(st_handler.mn_emptynext_cnt >= 1 || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
					{
						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
						{				
							g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
						}
					}
// 					else if(st_handler.mn_emptynext_cnt < 1)
// 					{
// 						st_handler.mn_emptynext_cnt++;
// 					}
					else
					{
						int iNBin1_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN1 );	
						int iNBin1_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 );
						int iNBin2_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN2 );
						int iNBin2_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 );
						int iNBin3_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN3 );
						int iNBin3_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 );
						//2012,1221
// 						if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1))
						if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1) && m_bReady_SendNext == false)
						{
							g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
						}
					}
				}
				else
				{
					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
					{				
						g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
					}
				}
			}
		}
	}
	
	if( //g_handler.GetMachinePos() == EMPOS_FRONT && 
		g_lotMgr.GetLotCount() > 1 &&
		g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" &&
		g_lotMgr.GetLotAt( 1 ).GetCokType() != st_basic.n_cok_type )
	{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return; }
		
	if( m_bReady_SendNext )
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); st_handler.m_nfront_smema = CTL_YES; 
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
				g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; 
				return;
			}
		}
	}
	
	if( st_basic.mn_bypass_mode == BYPASS_ALL )
	{
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); st_handler.m_nfront_smema = CTL_YES; return;
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
		g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); st_handler.m_nfront_smema = CTL_NO; return;
	}


	g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ); return;*/

	BOOL bRequest = IO_ON;

	if( COMI.mn_run_status != dRUN)						                { st_handler.m_nfront_smema = CTL_NO; return; }
	if( g_handler_cok.GetChangingCOK() )								{ st_handler.m_nfront_smema = CTL_NO; return; }
// 	if( Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_OFF)		{ g_ioMgr.set_out_bit( st_io.o_front_shuttle_request_signal, IO_OFF ); return; }
	if( Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_OFF)// || Run_Bin_Stacker[0].m_nfront_smema == CTL_YES)
	{ st_handler.m_nfront_smema = CTL_NO; return; }
	if( m_step_conv[0] != EC_NONE && m_step_conv[0] != EC_RECEIVE &&
		m_step_conv[0] != EC_RECEIVE_CHECK )							{ st_handler.m_nfront_smema = CTL_NO; return; }
	if(g_handler.GetMachinePos() == EMPOS_FRONT)
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON || g_ioMgr.get_in_bit( st_io.i_bin_tray1_in_chk ) == IO_ON ||
			Run_Bin_Stacker[0].GetStepRecvTray() >= RECV_TRAY_CONV_ON)
		{ 
			st_handler.m_nfront_smema = CTL_NO; return; 
		}
	}
	else
	{
		if( g_ioMgr.get_in_bit( st_io.i_bin_tray1_stop_position_chk ) == IO_ON)
		{ 
			st_handler.m_nfront_smema = CTL_NO; return; 
		}
	}
	//0502
	if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" && g_lotMgr.GetLotAt(1).GetStrLastModule() == "YES")
	{ 
		st_handler.m_nfront_smema = CTL_NO; return; 
	}
	//2015.0715
	if(Func.SemeaWork() != CTLBD_RET_GOOD)
	{ 
		st_handler.m_nfront_smema = CTL_NO; return; 
	}

	//ybs	
	int iBin1_1 = g_Tray.GetIdxByPos( TPOS_BIN1 );	
	int iBin1_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN1 );
	int iBin2_1 = g_Tray.GetIdxByPos( TPOS_BIN2 );
	int iBin2_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN2 );
	int iBin3_1 = g_Tray.GetIdxByPos( TPOS_BIN3 );
	int iBin3_2 = g_Tray.GetIdxByTargetPos( TPOS_BIN3 );
	//2013,1027
	int iBin4_1 = g_Tray.GetIdxByPos( TPOS_REJECT_TO_BIN );
	int iBin4_2 = g_Tray.GetIdxByTargetPos( TPOS_REJECT_TO_BIN );
	if(m_bReady_SendNext)
	{
		//2015.0829
		if( m_bEmptyTray_SendNext == true && g_lotMgr.GetLotIDAt(0) == m_strLotID_SendNext )
		{
			st_handler.m_nfront_smema = CTL_YES; 
		}
		else
		{
			st_handler.m_nfront_smema = CTL_NO;
		}
		return;

		if(g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext)
		{
//			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
//			{
//				sprintf(st_msg.c_abnormal_msg, "[SMEMA1]Lot = %s, NextLot = %s",g_lotMgr.GetLotIDAt(0),m_strLotID_SendNext);
//				Func.On_LogFile_Add(99, st_msg.c_abnormal_msg);
//				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
//			}
			st_handler.m_nfront_smema = CTL_NO; return;
		}

		//2013,0105
// 		if( m_bEmptyTray_SendNext == true && g_lotMgr.GetLotIDAt(0) == m_strLotID_SendNext )
// 		{
// 			st_handler.m_nfront_smema = CTL_YES; 
// 			return;
// 		}
//		if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" || m_bEmptyTray_SendNext == true)
		//2013,0621
// 		if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" && st_basic.mn_empty_tray_out > 1)
// 		{
// 			if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
// 			{
// 				st_handler.m_nfront_smema = CTL_NO; return;
// 			}
// 		}

		if(g_lotMgr.GetLotCount() > 1)
		{
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if(g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext &&
					(iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
				{				
					st_handler.m_nfront_smema = CTL_NO; return;
				}
			}
			else
			{
				if(g_lotMgr.GetLotIDAt(0) != m_strLotID_SendNext &&
					(iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
				{				
					st_handler.m_nfront_smema = CTL_NO; return;
				}
			}
		}
		else if(g_lotMgr.GetLotCount() < 2 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
		{
//			if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
//			{
//				st_handler.m_nfront_smema = CTL_NO; return;
//			}
			//2013,1027
			if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && ( (iBin3_1 > -1 || iBin3_2 > -1) || (iBin4_1 > -1 || iBin4_2 > -1) ))
			{
				st_handler.m_nfront_smema = CTL_NO; return;
			}
		}
// 		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
// 		{
// 			sprintf(st_msg.c_abnormal_msg, "[SMEMA]front_smema = %d, front_request = %d.",st_handler.m_nfront_smema,st_handler.m_nfront_request);
// 			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
// 		}

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
// 						st_handler.m_nfront_smema = CTL_NO; return;
// 					}
//					if(st_handler.mn_mirror_type == CTL_YES)
//					{
//						if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1) && (iBin1_1 > -1 || iBin1_1 > -1))
//						{				
//							st_handler.m_nfront_smema = CTL_NO; return;
//						}
//					}
//					else
//					{
//						//2013,0201
//						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
//						{				
//							st_handler.m_nfront_smema = CTL_NO; return;
//						}
//					}
					//2013,1027
					if(st_handler.mn_mirror_type == CTL_YES)
					{
						if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1) && ( (iBin1_1 > -1 || iBin1_1 > -1) || (iBin4_1 > -1 || iBin4_1 > -1) ) )
						{				
							st_handler.m_nfront_smema = CTL_NO; return;
						}
					}
					else
					{
						//2013,0201
						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && ( (iBin3_1 > -1 || iBin3_2 > -1) || (iBin4_1 > -1 || iBin4_1 > -1) ) )
						{				
							st_handler.m_nfront_smema = CTL_NO; return;
						}
					}
				}

			}//2013,0621
			else if(g_lotMgr.GetLotCount() > 1 && g_handler.GetMachinePos() == EMPOS_REAR)
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
					{				
						st_handler.m_nfront_smema = CTL_NO; return;
					}
				}
				else
				{
					if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
					{				
						st_handler.m_nfront_smema = CTL_NO; return;
					}
				}
			}
			//ybs
			if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES")
			{
				//2012,1128
// 				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
// 				{				
// 					st_handler.m_nfront_smema = CTL_NO; return;
// 				}
				if(g_handler.GetMachinePos() == EMPOS_FRONT)
				{
// 					//2012,1210
// 					if( st_handler.mn_emptynext_cnt == -1)
// 					{
// 						st_handler.m_nfront_smema = CTL_NO; return;
// 					}
					int iNBin1_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN1 );	
					int iNBin1_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 );
					int iNBin2_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN2 );
					int iNBin2_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 );
					int iNBin3_1 = g_Tray.GetIdxByPos_Extra( TPOS_BIN3 );
					int iNBin3_2 = g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 );
					//2013,1027
					int iNBin4_1 = g_Tray.GetIdxByPos( TPOS_REJECT_TO_BIN );
					int iNBin4_2 = g_Tray.GetIdxByTargetPos( TPOS_REJECT_TO_BIN );
					
					int num = 0;
// 					for( int it=0; it<MAX_TRAY; it++ )
// 					{
// 						ATray tray = g_Tray.GetTray( it );
// 						if(tray.GetTargetPos() == TPOS_NEXT )
// 						{
// 							num++;
// 						}
// 					}

					if(iNBin1_1 > -1 || iNBin1_2 > -1) num += 1;
					if(iNBin2_1 > -1 || iNBin2_2 > -1) num += 1;
					if(iNBin3_1 > -1 || iNBin3_2 > -1) num += 1;
					//2013,1027
					if(iNBin4_1 > -1 || iNBin4_2 > -1) num += 1;

					//2013,0320
					if(st_handler.mn_mirror_type == CTL_YES)
					{
						//2013,0715
						//if(g_lotMgr.GetLotIDAt(1).GetLength() < 3)
						if(g_lotMgr.GetLotCount() < 2)
						{
//							if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
//							{
//								//2013,0718
//// 								st_handler.m_nfront_smema = CTL_NO; return;
//								if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
//								{
//									st_handler.m_nfront_smema = CTL_NO; return;
//								}
//							}
							//2013,1027
							if( (iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && ( (iBin3_1 > -1 || iBin3_2 > -1) || (iBin4_1 > -1 || iBin4_2 > -1) ) )
							{
								if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
								{
									st_handler.m_nfront_smema = CTL_NO; return;
								}
							}
						}
						//2013,1222
// 						else if(st_handler.mn_emptynext_cnt >= 1 || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
						else if(st_handler.mn_emptynext_cnt >= st_basic.mn_empty_tray_out || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
						{
							if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
							{							
// 								if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
// 								{
// 									sprintf(st_msg.c_normal_msg, "[SMEMA1] BLOCK Tray IN.");
// 									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
// 								}

//								st_handler.m_nfront_smema = CTL_NO; return;//2015.0521
							}
						}
						else if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
						{				
							//2010,0710
							if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
							{
								st_handler.m_nfront_smema = CTL_NO; return;
							}

							if( num >=2 )
							{
								st_handler.m_nfront_smema = CTL_NO; return;
							}
 //							st_handler.m_nfront_smema = CTL_NO; return;
							// 						{
							// 						}
							// 						else if(num >= 2) st_handler.m_nfront_smema = CTL_NO; return;
							// 						else if(num >= 1 && (iNBin1_1 > -1 || iNBin1_2 > -1) ||  (iNBin2_1 > -1 || iNBin2_2 > -1))
							// 						{
							// 							st_handler.m_nfront_smema = CTL_NO; return;
							// 						}
						}
						else
						{
							//2012,1221
							// 						if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1) && m_bReady_SendNext == false )
 							if((iNBin2_1 > -1 || iNBin2_2 > -1) && (iNBin3_1 > -1 || iNBin3_2 > -1))
 							{
 								st_handler.m_nfront_smema = CTL_NO; return;
 							}
							// 						else if((iNBin1_1 > -1 || iNBin1_2 > -1) || (iNBin2_1 > -1 || iNBin2_2 > -1))
							// 						{
							// 							if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
							// 							{
							// 								sprintf(st_msg.c_normal_msg, "[SMEMA2] BLOCK Tray IN.");
							// 								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
							// 							}
							// 							st_handler.m_nfront_smema = CTL_NO; return;
							// 						}
						}

					}
					else
					{
						//2013,0715
// 						if(g_lotMgr.GetLotIDAt(1).GetLength() < 3)
						if(g_lotMgr.GetLotCount() < 2)
						{
//	 						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
	 						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && ( (iBin3_1 > -1 || iBin3_2 > -1) || (iBin4_1 > -1 || iBin4_2 > -1) ) )
	 						{
								//2013,0718
//								st_handler.m_nfront_smema = CTL_NO; return;
								if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
								{
									st_handler.m_nfront_smema = CTL_NO; return;
								}
	 						}
						}
						//2013,1222
// 						else if(st_handler.mn_emptynext_cnt >= 1 || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
						else if(st_handler.mn_emptynext_cnt >= st_basic.mn_empty_tray_out || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
						{
							if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
							{
// 								if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
// 								{
// 									sprintf(st_msg.c_normal_msg, "[SMEMA1] BLOCK Tray IN.");
// 									st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
// 								}
//								st_handler.m_nfront_smema = CTL_NO; return;//2015.0521
							}
						}
						else if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
						{
	// 						if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1))
							//2010,0718
							if(num >=2 || g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF)
	 						{
								st_handler.m_nfront_smema = CTL_NO; return;
	 						}
	// 						else if(num >= 2) st_handler.m_nfront_smema = CTL_NO; return;
	// 						else if(num >= 1 && (iNBin1_1 > -1 || iNBin1_2 > -1) ||  (iNBin2_1 > -1 || iNBin2_2 > -1))
	// 						{
	// 							st_handler.m_nfront_smema = CTL_NO; return;
	// 						}
						}
						else
						{
							//2012,1221
	// 						if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1) && m_bReady_SendNext == false )
							if((iNBin1_1 > -1 || iNBin1_2 > -1) && (iNBin2_1 > -1 || iNBin2_2 > -1))
							{
								st_handler.m_nfront_smema = CTL_NO; return;
							}
	// 						else if((iNBin1_1 > -1 || iNBin1_2 > -1) || (iNBin2_1 > -1 || iNBin2_2 > -1))
	// 						{
	// 							if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
	// 							{
	// 								sprintf(st_msg.c_normal_msg, "[SMEMA2] BLOCK Tray IN.");
	// 								st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
	// 							}
	// 							st_handler.m_nfront_smema = CTL_NO; return;
	// 						}
						}
					}
				}
				else
				{
					//2013,0320  2013,0718
// 					if(st_handler.mn_mirror_type == CTL_YES)
// 					{
// 						if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
// 						{				
// 							st_handler.m_nfront_smema = CTL_NO; return;
// 						}
// 					}
// 					else
// 					{
// 						if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
// 						{				
// 							st_handler.m_nfront_smema = CTL_NO; return;
// 						}
// 					}
				}
			}
		}
	}
	//2010,0710
//  	if(g_lotMgr.GetLotCount() > 1 && g_handler.GetMachinePos() == EMPOS_REAR)
// // 		if(g_lotMgr.GetLotCount() > 1)
// 	{
// 		if(st_handler.mn_mirror_type == CTL_YES)
// 		{
// 			if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
// 			{				
// 				st_handler.m_nfront_smema = CTL_NO; return;
// 			}
// 		}
// 		else
// 		{
// 			if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
// 			{				
// 				st_handler.m_nfront_smema = CTL_NO; return;
// 			}
// 		}
// 	}

	int j = 0, front = 0, rear = 0;
	if(g_lotMgr.GetLotCount() > 1 && g_handler.GetMachinePos() == EMPOS_FRONT)
	{
		for( j=0; j<MAX_TRAY; j++ )
		{
			if( g_Tray.GetTray(j).GetPos() == TPOS_NONE )
				continue;
			
			if( g_Tray.GetTray(j).IsAllState( EMS_LOAD ) && g_Tray.GetTray(j).GetTargetPos() == TPOS_NEXT )
			{
				rear++;
			}
		}		

		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF && 
				(iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
			{				
				st_handler.m_nfront_smema = CTL_NO; return;
			}
			else if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) != IO_OFF)
			{
// 				if( ((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1) )||
// 					(rear >= 1 && ((iBin2_1 > -1 || iBin2_2 > -1) || (iBin3_1 > -1 || iBin3_2 > -1))) )
// 				{
// 					st_handler.m_nfront_smema = CTL_NO; return;
// 				}
				if((iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1) )
				{
					if(rear >= 2 || (rear >= 1 && 
						((g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1 ) ||
						(g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ))) )
					{
						st_handler.m_nfront_smema = CTL_NO; return;
					}
				}
				else if((g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1 ) &&
					(g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ))
				{
					st_handler.m_nfront_smema = CTL_NO; return;
				}
			}
		}
		else
		{
			if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF &&
				(iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1))
			{				
				st_handler.m_nfront_smema = CTL_NO; return;
			}
			else if(g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) != IO_OFF)
			{
				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) )
				{
					if(rear >= 2 || (rear >= 1 && 
						((g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1 ) ||
						(g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ))) )
					{
						st_handler.m_nfront_smema = CTL_NO; return;
					}
					else if((g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1 ) &&
						(g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ))
					{
						st_handler.m_nfront_smema = CTL_NO; return;
					}
				}
			}
		}

	}

	//2013,0802
	if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" && g_handler.GetMachinePos() == EMPOS_FRONT)
	{
		if(st_handler.mn_mirror_type == CTL_YES)
		{

		}
		else
		{

		}
		if(st_basic.mn_empty_tray_out > 0)// && st_handler.mn_emptyout_cnt > 0)
		{
//			if(st_handler.mn_emptynext_cnt > 0)
//			{
//				if((iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && (iBin3_1 > -1 || iBin3_2 > -1))
//				{
//					st_handler.m_nfront_smema = CTL_NO; return;
//				}
//
//			}
			//2013,1027
			if(st_handler.mn_emptynext_cnt > 0)
			{
				if( (iBin1_1 > -1 || iBin1_2 > -1) && (iBin2_1 > -1 || iBin2_2 > -1) && ( (iBin3_1 > -1 || iBin3_2 > -1) || (iBin4_1 > -1 || iBin4_2 > -1) ) ) 
				{
					st_handler.m_nfront_smema = CTL_NO; return;
				}
			}
		}
	}

//2013,0514 ybs	
// 	if( //g_handler.GetMachinePos() == EMPOS_FRONT && 
// 		g_lotMgr.GetLotCount() > 1 &&
// 		g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" &&
// 		g_lotMgr.GetLotAt( 1 ).GetCokType() != st_basic.n_cok_type )
// 	{ 
// 		st_handler.m_nfront_smema = CTL_NO; return; 
// 	}
	
	if( m_bReady_SendNext )
	{
//		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
//		{
//			if(m_strLotID_SendNext != "" && g_lotMgr.GetLotIDAt(0) != "")
//			{
//				sprintf(st_msg.c_abnormal_msg, "[SMEMA2]Lot = %s, NextLot = %s",g_lotMgr.GetLotIDAt(0),m_strLotID_SendNext);
//				Func.On_LogFile_Add(99, st_msg.c_abnormal_msg);
//				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
//			}
//			else
//			{
//				sprintf(st_msg.c_abnormal_msg, "[SMEMA]Ready_SendNext = %d",m_bReady_SendNext);
//				Func.On_LogFile_Add(99, st_msg.c_abnormal_msg);
//				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
//			}
//		}
		st_handler.m_nfront_smema = CTL_YES; 
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
				st_handler.m_nfront_smema = CTL_NO; 
				return;
			}
		}
	}
	
	if( st_basic.mn_bypass_mode == BYPASS_ALL )
	{
		st_handler.m_nfront_smema = CTL_YES; return;
	}

	// 타겟이 BIN1, BIN2 있고, (front )뒤에 꺼져 있으면 받지 않는다. 뒤에 Recv 중이면 ... 또는 뒷설비 BYPASS면
//2013,0513
// 	if( ( g_handler.GetMachinePos() == EMPOS_REAR ||
// 		g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_OFF ||
// 		m_bRecv_NextMachine ||
// 		( ( (g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode == BYPASS_REAR) ||
// 		  ( (g_Tray.GetIdxByPos_Extra( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN1 ) > -1 ) &&
// 		  (g_Tray.GetIdxByPos_Extra( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN2 ) > -1 ) &&
// 		  (g_Tray.GetIdxByPos_Extra( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos_Extra( TPOS_BIN3 ) > -1 ) ) ) ) ) &&
// 		
// 		  ( (g_handler.GetMachinePos() == EMPOS_FRONT && st_basic.mn_bypass_mode == BYPASS_FRONT) ||
// //		  ( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
// //		  ( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 ) ) )
// 		  ( g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN1 ) > -1 ) &&
// 		  ( g_Tray.GetIdxByPos( TPOS_BIN2 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN2 ) > -1 ) &&
// 		  ( g_Tray.GetIdxByPos( TPOS_BIN3 ) > -1 || g_Tray.GetIdxByTargetPos( TPOS_BIN3 ) > -1 )) ) //ybs B1,B2,B3 자재가 있으면 투입정지
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
		st_handler.m_nfront_smema = CTL_NO; return;
	}

	st_handler.m_nfront_smema = CTL_YES; return;
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
			{
				m_step_conv[nIdx] = EC_NONE;
			}
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
			//2013,0316
			if(st_handler.mn_mirror_type == CTL_YES)
			{
				if(nIdx == 0) st_handler.mb_rej_3stacker = FALSE;
			}
			else
			{
				//2012,1211
				if(nIdx == 2) st_handler.mb_rej_3stacker = FALSE;
			}
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
	//if( m_step_conv[0] == EC_NONE &&
	//	g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk ) == IO_ON &&
	//	g_ioMgr.get_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ) == IO_ON )
	//{
	//	ControlConv(0, EC_RECEIVE );
	//	return;
	//}
	//if( m_step_conv[0] == EC_NONE && g_ioMgr.get_out_bit( st_io.o_front_shuttle_request_signal, IO_ON ) == IO_ON )
// 	if( m_step_conv[0] == EC_NONE && st_handler.m_nfront_request == CTL_REQ )
// 	{
// 		if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, 0, 50 ) == IO_ON)
// 		{
// 			ControlConv(0, EC_RECEIVE );
// 			st_handler.m_nfront_request = CTL_READY;
// 			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
// 			{
// 				sprintf(st_msg.c_normal_msg, "[Front Tray IN] Tray IN.");
// 				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
// 			}
// 			return;
// 		}
// 	}
	Run_Move_TrayIn();
	
	Control_byTray();
	
	// Robot
	ControlRobot();
	
	Run_Move_Virtual_Supply();
}

void APartHandler::Run_Move_TrayIn()
{
	// 	if( m_step_conv[0] == EC_NONE && st_handler.m_nfront_request == CTL_REQ )
	// 	{
	// 		if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, 0, 50 ) == IO_ON)
	// 		{
	// 			ControlConv(0, EC_RECEIVE );
	// 			st_handler.m_nfront_request = CTL_READY;
	// 			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
	// 			{
	// 				sprintf(st_msg.c_normal_msg, "[Front Tray IN] Tray IN.");
	// 				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
	// 			}
	// 			return;
	// 		}
	// 	}
	
	switch( m_step_trayin )
	{
	case 0:
		if( m_step_conv[0] == EC_NONE && st_handler.m_nfront_request == CTL_REQ && !st_handler.mn_virtual_supply)
		{
			m_dwTime_tray_in = GetCurrentTime();
			m_step_trayin = 100;
		}
		break;
		
	case 100:
		if( GetCurrentTime() - m_dwTime_tray_in <= 0) m_dwTime_tray_in = GetCurrentTime();
		if( m_step_conv[0] == EC_NONE && st_handler.m_nfront_request == CTL_REQ && !st_handler.mn_virtual_supply)
		{
			if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, IO_OFF ) == IO_ON)
			{
				if( GetCurrentTime() - m_dwTime_tray_in < 50) break;
				m_dwTime_tray_in = GetCurrentTime();
				m_step_trayin = 200;				
			}
		}
		else
		{
			m_step_trayin = 0;
		}
		break;
		
	case 200:
		if( GetCurrentTime() - m_dwTime_tray_in <= 0) m_dwTime_tray_in = GetCurrentTime();
		if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, IO_OFF ) == IO_ON)
		{
			if( GetCurrentTime() - m_dwTime_tray_in < 50) break;
			m_dwTime_tray_in = GetCurrentTime();
			m_step_trayin = 300;
		}
		else
		{
			m_step_trayin = 100;
		}
		break;

	case 300:
		if( GetCurrentTime() - m_dwTime_tray_in <= 0) m_dwTime_tray_in = GetCurrentTime();
		if( m_step_conv[0] == EC_NONE && st_handler.m_nfront_request == CTL_REQ && 
			g_Tray.TrayPosCheck( TPOS_CONV1 ) == CTL_NO && g_Tray.TrayPosCheck( TPOS_CONV1_TO_CONV2 ) == CTL_NO &&
			g_Tray.TrayPosCheck( TPOS_FRONT_TO_CONV1 ) == CTL_NO && g_Tray.TrayPosCheck( TPOS_CONV_TO_BIN1 ) == CTL_NO &&
			g_Tray.TrayPosCheck( TPOS_BIN_TO_CONV1 ) == CTL_NO)
		{
			if(g_ioMgr.get_in_bit( st_io.i_front_shuttle_transfer_signal_chk, IO_OFF ) == IO_ON)
			{
				if( GetCurrentTime() - m_dwTime_tray_in < 50) break;
				ControlConv(0, EC_RECEIVE );
				st_handler.m_nfront_request = CTL_READY;
				if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "[Front Tray IN] Tray IN.");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}

				m_step_trayin = 0;
			}			
		}
		else
		{
			m_step_trayin = 0;
		}
		break;
	}
	
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
					sprintf(st_msg.c_normal_msg, "ControlByTray_NEXT) %d TPOS_NEXT", ePos);
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
// 		if(ePos == TPOS_BIN1 && st_handler.m_nfront_request >= CTL_READY)
// 		{
// 		}
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
// 			if(ePos == TPOS_BIN1)
// 			{
// 				g_client_front.SendSmemaInfo();
// 			}
			return;
		}
		else if( (ePos == TPOS_BIN1 || ePos == TPOS_BIN2) && (eTargetPos == TPOS_BIN3 || eTargetPos == TPOS_REJECT) &&
			bConvUsable[ePos - TPOS_BIN1] &&
			g_Tray.GetIdxByPos( (ETRAY_POS)(TPOS_CONV1 + (ePos - TPOS_BIN1)) ) < 0 )
		{
			if(ePos == TPOS_BIN1 && st_handler.m_nfront_request == CTL_READY) return;
			ControlConv( ePos - TPOS_BIN1, EC_DOWN );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray4) %d EC_DOWN", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			return;
		}
		// RECV & SEND 1 to 2
// 		if(ePos == TPOS_CONV1 && st_handler.m_nfront_request >= CTL_READY)
// 		{
// 		}
////2015.0325
// 		if( bConvUsable[0] && ePos == TPOS_CONV1 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN1) &&
// 			g_ioMgr.get_in_bit(st_io.i_bin_tray2_stop_position_chk ) == IO_OFF )
		if( bConvUsable[0] && ePos == TPOS_CONV1 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN1) &&
			g_ioMgr.get_in_bit(st_io.i_bin_tray2_stop_position_chk ) == IO_OFF &&
			( g_Tray.TrayPosCheck( TPOS_CONV2 ) == CTL_NO && g_Tray.TrayPosCheck( TPOS_CONV2_TO_CONV3 ) == CTL_NO && 
			  g_Tray.TrayPosCheck( TPOS_CONV_TO_BIN2 ) == CTL_NO))
////
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
		////2015.0325
		// 		if( bConvUsable[2] && ePos == TPOS_CONV2 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN2) &&
		// 			g_ioMgr.get_in_bit(st_io.i_bin_tray3_stop_position_chk ) == IO_OFF &&
		// 			( (g_Tray.IsWorkingRejToNext() == false && g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0) || eTargetPos == TPOS_NEXT ))
		if( bConvUsable[2] && ePos == TPOS_CONV2 && (eTargetPos == TPOS_NEXT || eTargetPos > TPOS_BIN2) &&
			g_ioMgr.get_in_bit(st_io.i_bin_tray3_stop_position_chk ) == IO_OFF &&
			( (g_Tray.IsWorkingRejToNext() == false && g_Tray.GetIdxByPos( TPOS_BIN3 ) < 0) || eTargetPos == TPOS_NEXT ) &&
			( g_Tray.TrayPosCheck( TPOS_CONV3 ) == CTL_NO && g_Tray.TrayPosCheck( TPOS_CONV3_TO_NEXT ) == CTL_NO &&
			  g_Tray.TrayPosCheck( TPOS_CONV_TO_BIN3 ) == CTL_NO))
			////		
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
//2013,0513		if( m_step_conv[2] == EC_NONE && ePos == TPOS_CONV3 && eTargetPos == TPOS_NEXT && 
// 			g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk ) == IO_ON )
		if( m_step_conv[2] == EC_NONE && ePos == TPOS_CONV3 && eTargetPos == TPOS_NEXT && 
			g_ioMgr.get_in_bit( st_io.i_rear_shuttle_request_signal_chk, 0, 100 ) == IO_ON )
		{
			ControlConv(2, EC_SEND );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "ControlByTray7) %d EC_SEND", ePos);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
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
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			// Transfer
			if( ePos == TPOS_BIN1 && eTargetPos == TPOS_REJECT && m_step_ttrans == ETT_TO_NONE )
			{
				m_step_ttrans = ETT_TO_REJECT;
				return;
			}
			if( ePos == TPOS_REJECT && (eTargetPos == TPOS_NEXT || eTargetPos == TPOS_BIN1 ) && m_step_ttrans == ETT_TO_NONE && 
				g_Tray.GetIdxByPos( TPOS_BIN1 ) < 0 && m_step_conv[0] == EC_NONE )
			{
				m_step_ttrans = ETT_TO_BIN3;
				return;
			}
			if( (ePos == TPOS_BIN1 || ePos == TPOS_REJECT) && (eTargetPos == TPOS_NEXT || eTargetPos == TPOS_BIN1 ) && m_step_ttrans == ETT_TO_NONE && 
				g_Tray.GetIdxByPos( TPOS_BIN1 ) < 0 && m_step_conv[0] == EC_NONE )
			{
				m_step_ttrans = ETT_TO_BIN3;
				return;
			}
		}
		else
		{
			// Transfer
			if( ePos == TPOS_BIN3 && eTargetPos == TPOS_REJECT && m_step_ttrans == ETT_TO_NONE )
			{
				m_step_ttrans = ETT_TO_REJECT;
				return;
			}
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
// 	if(st_handler.mn_mirror_type == CTL_YES)
// 	{
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
// 	}
// 	else
// 	{
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
// 				if( nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN3;
// 				else                m_epriority_bin = epos;
// 			}
// 			
// 		}
// 		else if(epos == TPOS_BIN2)
// 		{
// 			posSide = (epos == TPOS_BIN2) ? TPOS_BIN3 : TPOS_BIN1;
// 			nSideIdx = g_Tray.GetIdxByPos( posSide );
// 			
// 			if( nSideIdx > -1  && st_handler.mb_rej_3stacker == FALSE)
// 			{
// 				m_epriority_bin = posSide;
// 			}
// 			else
// 			{
// 				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
// 				if( nSideIdx > -1 ) m_epriority_bin = TPOS_BIN1;
// 				else                m_epriority_bin = epos;
// 			}
// 		}
// 		else if(epos == TPOS_BIN3)
// 		{
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

	if( epos != TPOS_BIN1 && epos != TPOS_BIN2 && epos != TPOS_BIN3 )
		return;

	ETRAY_POS posSide;
	int nSideIdx;

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if(epos == TPOS_BIN1 && st_handler.mb_rej_3stacker == TRUE)
		{

		}
		else if(epos == TPOS_BIN1)
		{
			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN3;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1 )
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
				if(nSideIdx > -1) m_epriority_bin = TPOS_BIN3;
				else			  m_epriority_bin = TPOS_NONE;
			}
		}
		else if(epos == TPOS_BIN2)
		{
			posSide = (epos == TPOS_BIN2) ? TPOS_BIN3 : TPOS_BIN1;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1)
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
				if(nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN1;
				else													 m_epriority_bin = TPOS_NONE;
			}
			
		}
		else if(epos == TPOS_BIN3)
		{
			posSide = (epos == TPOS_BIN3) ? TPOS_BIN1 : TPOS_BIN2;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1  && st_handler.mb_rej_3stacker == FALSE)
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
				if(nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
				else			  m_epriority_bin = TPOS_NONE;
			}
		}

	}
	else
	{
		if(epos == TPOS_BIN1)
		{
			posSide = (epos == TPOS_BIN1) ? TPOS_BIN2 : TPOS_BIN3;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1 )
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
				if(nSideIdx > -1 && st_handler.mb_rej_3stacker == FALSE) m_epriority_bin = TPOS_BIN3;
				else			  m_epriority_bin = TPOS_NONE;
			}
		}
		else if(epos == TPOS_BIN2)
		{
			posSide = (epos == TPOS_BIN2) ? TPOS_BIN3 : TPOS_BIN1;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1  && st_handler.mb_rej_3stacker == FALSE)
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
				if(nSideIdx > -1) m_epriority_bin = TPOS_BIN1;
				else			  m_epriority_bin = TPOS_NONE;
			}
			
		}
		else if(epos == TPOS_BIN3 && st_handler.mb_rej_3stacker == TRUE)
		{
		}
		else if(epos == TPOS_BIN3)
		{
			posSide = (epos == TPOS_BIN3) ? TPOS_BIN1 : TPOS_BIN2;
			nSideIdx = g_Tray.GetIdxByPos( posSide );
			
			if( nSideIdx > -1 )
			{
				m_epriority_bin = posSide;
			}
			else
			{
				nSideIdx = g_Tray.GetIdxByPos( TPOS_BIN2 );
				if(nSideIdx > -1) m_epriority_bin = TPOS_BIN2;
				else			  m_epriority_bin = TPOS_NONE;
			}
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
	{
		CalcPriorityBinByOut( eFrom );
	}

	if( eTo == TPOS_BIN1 || eTo == TPOS_BIN2 || eTo == TPOS_BIN3 )
	{
		if(eFrom != TPOS_REJECT_TO_BIN)
		{
			CalcPriorityBinByIn( eTo );
		}
	}

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

		// A 랏
		if( CR_forReject( vecFail, vecRejEmptyCell ) )											return;				// 불량을 리젝트로 뺀다.
		if( CR_forStacker3MoveDvc(vecRejEmptyCell, vecEmptyInsert_S, vecEmptyInsert, vecFail))	return;
//// 		if( CR_forMoveRejectDvc( vecFail, vecRejEmptyCell,  vecTrayEmptyCell_A) )				return;				// 불량을 리젝트로 뺀다.
//
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
		if( vecRejEmptyCell.size() != 0)//2013,1223
		{
			if( CR_forSort( vecPriEmpty, vecSubGood ) )												return;
		}

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
	//2013,1219
	int iBin4_1 = g_Tray.GetIdxByPos( TPOS_REJECT_TO_BIN );
	int iBin4_2 = g_Tray.GetIdxByTargetPos( TPOS_REJECT_TO_BIN );

	bool bBin1End = true;
	bool bBin2End = true;
	bool bBin3End = true;
	for( int i=0; i<nTotal; i++ )
	{
		if( iBin1 < 0 )															{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetModule(i).IsExist() == false )				{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetModule(i).GetModuleState() != EMS_GOOD )	{bBin1End = false;		break;}
		if( g_Tray.GetTray(iBin1).GetStrLotID() != g_lotMgr.GetLotIDAt(0) )		{bBin1End = false;		break;}
	}
	for( i=0; i<nTotal; i++ )
	{
		if( iBin2 < 0 )															{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetModule(i).IsExist() == false )				{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetModule(i).GetModuleState() != EMS_GOOD )	{bBin2End = false;		break;}
		if( g_Tray.GetTray(iBin2).GetStrLotID() != g_lotMgr.GetLotIDAt(0) )		{bBin2End = false;		break;}
	}
	for( i=0; i<nTotal; i++ )
	{
		if( iBin3 < 0 )															{bBin3End = false;		break;}
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
			if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin2_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin3_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
				return;
			}

			//2013,1219
			if( g_handler.GetMachinePos() == EMPOS_FRONT )
			{
				if(st_handler.mn_emptynext_cnt < st_basic.mn_empty_tray_out)
				{
					return;
				}
			}
			g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "3)Bin1_TPOS_NEXT");
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}

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
			if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin1_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin3_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
				return;
			}
			//2013,1219
			if( g_handler.GetMachinePos() == EMPOS_FRONT )
			{
				if(st_handler.mn_emptynext_cnt < st_basic.mn_empty_tray_out)
				{
					return;
				}
			}
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "3)Bin2_TPOS_NEXT");
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
			if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "1)Bin1_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
				return;
			}
			if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0))
			{
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "2)Bin2_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
				g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
				return;
			}
			//2013,1219
			if( g_handler.GetMachinePos() == EMPOS_FRONT )
			{
				if(st_handler.mn_emptynext_cnt < st_basic.mn_empty_tray_out)
				{
					return;
				}
			}
			
//			g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
			//2013,1028
			if( IsTrayOthers() == true)
			{
				g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
				if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
				{
					sprintf(st_msg.c_normal_msg, "3)Bin3_TPOS_NEXT");
					st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
				}
			}
			return;
		}
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
// 		if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true) && !g_Tray.GetTray(iBin1).GetEmptyTray() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
// 			g_robot.GetJobCount() == 0 && (st_handler.mn_emptynext_cnt == 0 || g_Tray.GetVecRejEmptyCell().size() == 0) && (st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out || g_Tray.GetVecRejEmptyCell().size() == 0) && g_Tray.GetTray(iBin1).GetTargetPos() != TPOS_NEXT)
		//2013,0710
		if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true) && !g_Tray.GetTray(iBin1).GetEmptyTray() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) && g_robot.GetJobCount() == 0 && 
			((st_handler.mn_emptynext_cnt == 0 && (st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out)) || (g_Tray.GetVecRejEmptyCell().size() == 0 && g_Tray.GetRejCnt() > 0)) && 
			g_Tray.GetTray(iBin1).GetTargetPos() != TPOS_NEXT)
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
		//	if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true) && !g_Tray.GetTray(iBin2).GetEmptyTray() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
		//		g_lotMgr.GetLotCount() == 1 && g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin2).GetTargetPos() != TPOS_NEXT)
		if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true) && !g_Tray.GetTray(iBin2).GetEmptyTray() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
			g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin2).GetTargetPos() != TPOS_NEXT)
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
			g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin3).GetTargetPos() != TPOS_NEXT)
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

	}
	else
	{
	//	if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true) && !g_Tray.GetTray(iBin1).GetEmptyTray() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
	//		g_lotMgr.GetLotCount() == 1 && g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin1).GetTargetPos() != TPOS_NEXT)
		if(iBin1 > -1 && g_Tray.GetTray(iBin1).IsAllEmpty(true) && !g_Tray.GetTray(iBin1).GetEmptyTray() && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
			g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin1).GetTargetPos() != TPOS_NEXT)
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
	//	if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true) && !g_Tray.GetTray(iBin2).GetEmptyTray() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
	//		g_lotMgr.GetLotCount() == 1 && g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin2).GetTargetPos() != TPOS_NEXT)
		if(iBin2 > -1 && g_Tray.GetTray(iBin2).IsAllEmpty(true) && !g_Tray.GetTray(iBin2).GetEmptyTray() && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
			g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out && g_Tray.GetTray(iBin2).GetTargetPos() != TPOS_NEXT)
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
	//	if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty(true) && !g_Tray.GetTray(iBin3).GetEmptyTray() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
	//		g_lotMgr.GetLotCount() == 1 && g_robot.GetJobCount() == 0 && st_handler.mn_emptynext_cnt == 0 && (st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out || g_Tray.GetVecRejEmptyCell().size() == 0) && g_Tray.GetTray(iBin3).GetTargetPos() != TPOS_NEXT)
// 		if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty(true) && !g_Tray.GetTray(iBin3).GetEmptyTray() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
// 			g_robot.GetJobCount() == 0 && (st_handler.mn_emptynext_cnt == 0 || g_Tray.GetVecRejEmptyCell().size() == 0) && (st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out || g_Tray.GetVecRejEmptyCell().size() == 0) && g_Tray.GetTray(iBin3).GetTargetPos() != TPOS_NEXT)
		//2013,0710
		if(iBin3 > -1 && g_Tray.GetTray(iBin3).IsAllEmpty(true) && !g_Tray.GetTray(iBin3).GetEmptyTray() && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) && g_robot.GetJobCount() == 0 && 
			((st_handler.mn_emptynext_cnt == 0 && (st_handler.mn_emptyout_cnt < st_basic.mn_empty_tray_out)) || (g_Tray.GetVecRejEmptyCell().size() == 0 && g_Tray.GetRejCnt() > 0)) && 
			g_Tray.GetTray(iBin3).GetTargetPos() != TPOS_NEXT)
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

	case EVS_CALC://2013,0131
// 		if( g_Tray.IsEnablePos( TPOS_BIN1 ) || g_Tray.IsEnablePos( TPOS_BIN2 ) )
		if( g_Tray.IsEnablePos( TPOS_BIN1 ) || g_Tray.IsEnablePos( TPOS_BIN2 ) || g_Tray.IsEnablePos( TPOS_BIN3 ) )
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
			else if( g_Tray.IsEnablePos( TPOS_BIN3 ) )//2013,0131
			{
				eTargetPos = TPOS_BIN3;
			}

			if( eTargetPos != TPOS_NONE )
			{
				ATray tray;
				tray.SetPos( TPOS_FRONT_TO_CONV1 );
// 				tray.SetTrayInfo( g_lotMgr.GetLotIDAt(0), 0, false, false );
				//2017.0602
				tray.SetTrayInfo( g_lotMgr.GetLotIDAt(0), g_lotMgr.GetPartIDAt(0), 0, false, false );
				
				for( int i=0; i<MAX_MODULE; i++ )
				{
					
					CString strArraySN;		strArraySN.Format( "MZCPA256112700000%d%02d_3", eTargetPos - TPOS_BIN1, i);
					CString strSerial;		strSerial.Format( "S11ZNYAC500%d%02d", eTargetPos - TPOS_BIN1, i );
					CString strPPID;		strPPID.Format( "KR0NMY6F018511CJ0219A%d%02d", eTargetPos - TPOS_BIN1, i);
					CString strWWN;			strWWN.Format( "5002538500000%d%02d", eTargetPos - TPOS_BIN1, i);
					CString strCSN;			strCSN.Format( "CSN1_100000%d%02d", eTargetPos - TPOS_BIN1, i );
					CString strPSID;		strPSID.Format( "YYK2200000%d%02d", eTargetPos - TPOS_BIN1, i );
					CString strBIN =		"";
					CString strCSN2 = "";
					//2015.1123
					if( st_basic.n_cserial2_mode == CTL_YES )
					{
						strCSN2.Format( "CSN2_100000%d%02d", eTargetPos - TPOS_BIN1, i );
					}
					
					// 모듈 셋
					AModule mdl;
// 					mdl.SetModuleInfo( g_lotMgr.GetLotIDAt(0), strArraySN, strSerial, strPPID, strWWN, strBIN );
					//2015.1123
					if( st_basic.n_cserial2_mode == CTL_YES )
					{
						mdl.SetModuleInfo( g_lotMgr.GetLotIDAt(0), strArraySN, strSerial, strPPID, strWWN, strCSN, strPSID, strBIN, strCSN2 );
					}
					else
					{
						mdl.SetModuleInfo( g_lotMgr.GetLotIDAt(0), strArraySN, strSerial, strPPID, strWWN, strCSN, strPSID, strBIN );
					}
					tray.SetModule(i, mdl);
				}
				
				tray.SetTargetPos( eTargetPos );
				g_Tray.AddTray( tray );
				g_Tray.TrayMoved( TPOS_FRONT_TO_CONV1, eTargetPos );

				m_step_virtual_supply = EVS_FINISH;
			}
		}//2012,1220
		else if( GetCurrentTime() - m_dwTime_virtual_supply < 0 )
		{
			m_dwTime_virtual_supply = GetCurrentTime();
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
	
	//2014,0717
	if( nAxis == MOTOR_ROBOT_Y && nTargetPart != MPOS_INDEX_COK_SITE1_0 &&
		nTargetPart != MPOS_INDEX_COK_SITE1_1 &&
		nTargetPart != MPOS_INDEX_COK_SITE2_0 &&
		nTargetPart != MPOS_INDEX_COK_SITE2_1 &&
		nTargetPart != MPOS_INDEX_COK_STACKER)
	{
		if(nTargetPart < MPOS_INDEX_BIN1)
		{
			dAdd += st_motor[MOTOR_ROBOT_Y].d_pos[28]*nPickerIdx;
			//dAdd += ((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE1_START]- st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_SITE_1_2])/4) * nPickerIdx;
		}
		else
		{
			dAdd += st_motor[MOTOR_ROBOT_Y].d_pos[27]*nPickerIdx;
// 			if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )
// 			{
// 				dAdd += ((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START]- st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END])/4) * nPickerIdx;
// 			}
// 			else
// 			{
// 				dAdd += ((st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_START]- st_motor[MOTOR_ROBOT_Y].d_pos[P_XY_TRAY1_END])/3) * nPickerIdx;
// 			}
// 			dAdd = 0;
// 			dAdd += st_motor[MOTOR_ROBOT_Y].d_pos[27]*nPickerIdx;
		}
	}
		//2014,0726	dAdd += st_handler.md_picker_position[nPickerIdx];
	

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
			if( nAxis == MOTOR_ROBOT_X     )
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					switch( st_basic.n_degree )
					{
					case EDEG_0:	return (nTarget % MPOS_DIVISION) % st_basic.n_tray_x;
					case EDEG_90:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) / st_basic.n_tray_x;
					case EDEG_180:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) % st_basic.n_tray_x;
					case EDEG_270:	return (nTarget % MPOS_DIVISION) / st_basic.n_tray_x;
					}
				}
				else
				{
					switch( st_basic.n_degree )
					{
					case EDEG_0:	return (nTarget % MPOS_DIVISION) % st_basic.n_tray_x;
					case EDEG_90:	return (nTarget % MPOS_DIVISION) / st_basic.n_tray_x;
					case EDEG_180:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) % st_basic.n_tray_x;
					case EDEG_270:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) / st_basic.n_tray_x;
					}
 				}
			}
			else if( nAxis == MOTOR_ROBOT_Y )
			{
				if(st_handler.mn_mirror_type == CTL_YES)
				{
					switch( st_basic.n_degree )
					{
					case EDEG_0:	return (nTarget % MPOS_DIVISION) / st_basic.n_tray_x;
					case EDEG_90:	return (nTarget % MPOS_DIVISION) % st_basic.n_tray_x;
					case EDEG_180:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) / st_basic.n_tray_x;						
					case EDEG_270:	return (nTotal - (nTarget % MPOS_DIVISION) - 1) % st_basic.n_tray_x;						
					}
				}
				else
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

bool APartHandler::CR_forExchange( const std::vector<int>& vecTestEndInsert, const std::vector<int>& vecMustTestMdl, const std::vector<int>& vecEmptyInsert, int nLotIdx )
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

			/////////////////////////////////////////////////////
			//2017.0102
			if( nTrayIdx < 0 ) return false;
			/////////////////////////////////////////////////////

			ATray& tray = g_Tray.GetTray( nTrayIdx );
			AModule& tmdl = tray.GetModule( vecMustTestMdl[i] % MPOS_DIVISION );
				
			bool bEnable = true;
 			if( tmdl.GetModuleState() == EMS_RELOAD )
 			{
				//2014,0825
				int nC=0;
				if(tmdl.GetTestCnt() <= 0) nC = 0;
				else					   nC = tmdl.GetTestCnt()-1;
//  			for( int it=0; it< tmdl.GetTestCnt(); it++ )
 				for( int it=0; it< tmdl.GetTestCnt(); it++ )
 				{
 					if( tmdl.GetTestedPos(it) / 2 == vecTestEndInsert[0] && vecMustTestMdl.size() > 2)
 					{
 						bEnable = false;
 						break;
 					}
 				}
 			}
			if( bEnable )
			{
				vecEnableTest.push_back( vecMustTestMdl[i] );
			}
		}

		if( vecEnableTest.size() < 2 )		return false;

		int j = 0, k = 0, n_First_Picker_X_Num = 0, nval = 1;
		int n_First_Tray_Y_Num=0,n_First_Tray_X_Num=0, n_First_Bin = -1, nPosY = 0,nPos = 0,nvcNum = 0;
		int nPickeruse[PICKER_NUM], nTargetPos[4];
		int nBinNum = 0, ix,iy,x,y,col,row;
		
		for ( i =0 ; i < PICKER_NUM; i++)
		{
			nPickeruse[i] = CTL_NO;
			nTargetPos[i] = -1;
		}

		for ( i = 0; i < 3; i++) {
			for ( j = 0; j < 5; j++) {
				for ( k =0; k < 5; k++) {
					nExBinMap[i][j][k] = -1;//3,row,col
				}
			}
		}
		for ( i = 0; i < PICKER_NUM; i++) {				
			nPickeruse[i] = CTL_NO;
		}

		x = col = st_basic.n_tray_x;
		y = row = st_basic.n_tray_y;
		
		if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
		{
			y = row = st_basic.n_tray_x;
			x = col = st_basic.n_tray_y;
		}

		std::vector<int> vecPicker;
		std::vector<int> vecIdxPicker;
		std::vector<int>::iterator it = vecEnableTest.begin();		

		std::vector<int> vecEmptyCell = g_Tray.GetVecEmptyCell( nLotIdx);//2014,0812 , vecEnableTest[0], vecEnableTest[1] );


//			g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
//			g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[1] );

			ArrangeExforTest(vecEnableTest, n_First_Tray_Y_Num, n_First_Tray_X_Num, n_First_Bin);
			bool a=false;
			for(i=0; i<PICKER_NUM; i++)	{
				if (nPickeruse[i] == CTL_NO) {
					if(a == false) {
						n_First_Picker_X_Num = i;
						a = true;
					}
					if(a){ i = PICKER_NUM; }
				}
			}

			nBinNum = 0;
			int mn_chkretry = 0;
			do {
				for(i=n_First_Picker_X_Num; i<PICKER_NUM; i+=2)
				{
					if (nPickeruse[i] == CTL_NO)
					{
						if(n_First_Tray_Y_Num + i >= 0 && ( n_First_Tray_Y_Num + (i - n_First_Picker_X_Num) ) < row)
						{
							if(nExBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] >= 0)
							{
								nTargetPos[i] = (n_First_Bin+1)*MPOS_DIVISION + nExBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)];
								nExBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] = -1;
								nPickeruse[i] = CTL_YES;
								vecPicker.push_back(i);
							}
							else nPickeruse[i] = CTL_NO;
						}
					}
				}
				mn_chkretry++;
				if(mn_chkretry > 100)
				{
					if(nPickeruse[0] == CTL_NO && nPickeruse[2] == CTL_NO)
					{
						nTargetPos[0] = vecEnableTest[0];
						nTargetPos[2] = vecEnableTest[1];
						vecPicker.push_back(0);
						vecPicker.push_back(2);
					}
					else if(nPickeruse[0] == CTL_NO || nPickeruse[2] == CTL_NO)
					{
						for (i = 0; i <vecEnableTest.size(); i++)
						{
							for(j = 0;j < PICKER_NUM; j+=2)
							{
								if(nPickeruse[j] == CTL_NO) {
									if(j == 0) { 
										if(nTargetPos[2] != vecEnableTest[i]) {
											nTargetPos[0] = vecEnableTest[i];
											vecPicker.push_back(0);
										}
									}
									else {
										if(nTargetPos[0] != vecEnableTest[i]) {
											nTargetPos[2] = vecEnableTest[i];
											vecPicker.push_back(2);
										}

									}
								}

							}	
						}
					}
				}

				if(vecPicker.size() < 2 )
				{
					nBinNum = BinNumExforTest(vecEnableTest, n_First_Bin);
					if(nBinNum <= 0)
					{
						n_First_Bin++;
						if(n_First_Bin>=3 || n_First_Bin<0) n_First_Bin=0;
					}

					a=false;		
					for(i=0; i<PICKER_NUM; i+=2)	{						
						if (nPickeruse[i] == CTL_NO) {
							if(a == false) {
								n_First_Picker_X_Num = i;
								a = true;
							}
							if(a) {	i = PICKER_NUM; }
						}
					}
					a = false;
					for( ix=0; ix<x; ix++ ){
						for ( iy=0; iy<y; iy++ )	{
							if (nExBinMap[n_First_Bin][ix][iy] != -1){
								if(a == false){
									n_First_Tray_X_Num = ix;
									n_First_Tray_Y_Num = iy;
									a = true;
								}
								if(a) {	ix = x;	iy = y;	}
							}
						}
					}
				}
				
			} while(vecPicker.size()<2);


			nval = 1;
			if( st_handler.mn_mirror_type == CTL_YES )
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_180) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval = -1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						}
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
			}
			else
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_0) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval=-1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						}
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
			}
				


		//Reset
		x = col = st_basic.n_tray_x;
		y = row = st_basic.n_tray_y;
		
		if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
		{
			y = row = st_basic.n_tray_x;
			x = col = st_basic.n_tray_y;
		}
		
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
				nRobotIdx_forWork[i] = (i * 2) + 1;
			}
			else
			{
				nRobotIdx_forWork[i] = -1;
			}
		}

		int nTargetInsert = vecTestEndInsert[0];
		if( vecEmptyInsert.size() > 0 )
			nTargetInsert = vecEmptyInsert[0];
				
		g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[1], -1, -1, vecTestEndInsert[0] );
		g_robot.AddJob( JOB_PLACE, 0, 2,  -1, -1, nTargetInsert );

		//vecEmptyCell = g_Tray.GetVecEmptyCell( nLotIdx, nTargetPos[0], nTargetPos[2] );//2014,0805
		std::vector<int> vecEmptyTray = g_Tray.GetVecEmptyCell( nLotIdx, nTargetPos[0], nTargetPos[2] );//2014,0812

		int nvecidx = 0;


			for ( i = 0; i < PICKER_NUM; i++) {				
				nPickeruse[i] = CTL_NO;
				if(nRobotIdx_forWork[i] > -1) vecIdxPicker.push_back(i);
			}
			for ( i = 0; i < 3; i++) {
				for ( j = 0; j < 5; j++) {
					for ( k =0; k < 5; k++) {
						nExEmpBinMap[i][j][k] = -1;//3,row,col
					}
				}
			}

			ArrangeExEmpforTest(vecEmptyTray, n_First_Tray_Y_Num, n_First_Tray_X_Num, n_First_Bin);			

			a=false;
			for(i=0; i<PICKER_NUM; i++)	{
				if (nPickeruse[i] == CTL_NO) {
					if(a == false) {
						n_First_Picker_X_Num = i;
						a = true;
					}
					if(a){ i = PICKER_NUM; }
				}
			}
			nBinNum = 0;			
			do {
				for(i=n_First_Picker_X_Num; i<PICKER_NUM; i++)
				{
					if (nPickeruse[i] == CTL_NO && nRobotIdx_forWork[i] != -1)
					{
						if(n_First_Tray_Y_Num + i >= 0 && ( n_First_Tray_Y_Num + (i - n_First_Picker_X_Num) ) < row)
						{
							if(nExEmpBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] > -1)
							{
								nTargetPos[i] = (n_First_Bin+1)*MPOS_DIVISION + nExEmpBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)];
								nExEmpBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] = -1;
								nPickeruse[i] = CTL_YES;
								vecPicker.push_back(i);
							}
							else nPickeruse[i] = CTL_NO;
						}			
					}
				}
			
				if(vecPicker.size() < 4 && vecPicker.size() < vecIdxPicker.size())
				{
					nBinNum = BinNumExEmpforTest( vecEmptyTray, n_First_Bin);					
					
					if(nBinNum <= 0)
					{
						n_First_Bin++;
						if(n_First_Bin>=3 || n_First_Bin<0) n_First_Bin=0;
					}

					a=false;		
					for(i=0; i<PICKER_NUM; i++)	{						
						if (nPickeruse[i] == CTL_NO && nRobotIdx_forWork[i] != -1) {
							if(a == false) {
								n_First_Picker_X_Num = i;
								a = true;
							}
							if(a) {	i = PICKER_NUM; }
						}
					}

					a = false;
					for( ix=0; ix<x; ix++ ){
						for ( iy=0; iy<y; iy++ )	{
							if (nExEmpBinMap[n_First_Bin][ix][iy] > -1){
								if(a == false){
									n_First_Tray_X_Num = ix;
									n_First_Tray_Y_Num = iy;
									a = true;
								}
								if(a) {	ix = x;	iy = y;	}
							}
						}
					}
				}
			}while(vecPicker.size()<4 && vecPicker.size() < vecIdxPicker.size());

			nval = 1;
			if( st_handler.mn_mirror_type == CTL_YES )
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_180) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, nRobotIdx_forWork[1], -1, nTargetPos[0], -1, nTargetPos[2], -1);
					}
					else
					{
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_GOOD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_RELOAD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_FAIL )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval = -1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, nRobotIdx_forWork[1], -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}						
						else
						{
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_GOOD ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_RELOAD ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_FAIL ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
						}
					}
					else
					{
						for( i=0; i<2; i++ ){
							if( mdl[i].GetModuleState() == EMS_GOOD ){
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ ){
							if( mdl[i].GetModuleState() == EMS_RELOAD ){
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ ){
							if( mdl[i].GetModuleState() == EMS_FAIL ){
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
					}
				}
			}
			else
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_0) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, nRobotIdx_forWork[1], -1, nTargetPos[0], -1, nTargetPos[2], -1);

					}
					else
					{
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_GOOD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_RELOAD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_FAIL )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
					}
				}//2016.0504
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval = -1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, nRobotIdx_forWork[1], -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}						
						else
						{
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_GOOD ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_RELOAD ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
							for( i=0; i<2; i++ ){
								if( mdl[i].GetModuleState() == EMS_FAIL ){
									g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
									nvecidx++;
								}
							}
						}
					}
					else
					{
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_GOOD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_RELOAD )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
						for( i=0; i<2; i++ )
						{
							if( mdl[i].GetModuleState() == EMS_FAIL )
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyTray[nvecidx] );
								nvecidx++;
							}
						}
					}
				}

			}			

		

		return true;

	}
	
	return false;
}

//2013,0405
// bool APartHandler::CR_forExchange( std::vector<int> vecTestEndInsert, std::vector<int> vecMustTestMdl, std::vector<int> vecEmptyInsert, int nLotIdx )
// {
// 	if( st_basic.n_pick_type != EPT_1V1M_4USE )
// 		return false;
// 	
// 	std::vector<int> vecEnableTest;
// 	if( vecTestEndInsert.size() >= 1 )
// 	{
// 		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == false &&
// 			g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == false )
// 		{
// 			return false;
// 		}
// 
// 		for( int i=0; i< vecMustTestMdl.size(); i++ )
// 		{
// 
// 			int nPos = GetPosByTarget( vecMustTestMdl[i] );
// 			ETRAY_POS tpos = TPOS_BIN1;
// 			if( nPos == MPOS_INDEX_BIN2 )
// 				tpos = TPOS_BIN2;
// 			if( nPos == MPOS_INDEX_BIN3 )//ybs
// 				tpos = TPOS_BIN3;
// 
// 			int nTrayIdx = g_Tray.GetIdxByPos( tpos );
// 			ATray& tray = g_Tray.GetTray( nTrayIdx );
// 			AModule& tmdl = tray.GetModule( vecMustTestMdl[i] % MPOS_DIVISION );
// 				
// 			bool bEnable = true;
// 			if( tmdl.GetModuleState() == EMS_RELOAD )
// 			{
// // 				for( int it=0; it< tmdl.GetTestCnt(); it++ )
// // 				{
// // 					if( tmdl.GetTestedPos(it) / 2 == vecTestEndInsert[0] )
// // 					{
// // 						bEnable = false;
// // 						break;
// // 					}
// // 				}
// 			}
// 			if( bEnable )
// 			{
// 				vecEnableTest.push_back( vecMustTestMdl[i] );
// 			}
// 		}
// 
// 		if( vecEnableTest.size() < 2 )		return false;
// 
// 		//g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
// 		//g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );
// 
// 		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == true)	g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
// 		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == true)  g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );
// 
// 		AModule mdl[2];
// 		mdl[0] = g_site.GetModule(vecTestEndInsert[0] * 2);
// 		mdl[1] = g_site.GetModule(vecTestEndInsert[0] * 2 + 1);
// 
// 			
// 		int nTestEndCnt = 0;
// 		int nRobotIdx_forWork[2];
// 		for( i=0; i<2; i++ )
// 		{
// 			if( mdl[i].IsExist() )
// 			{
// 				nTestEndCnt++;
// 				nRobotIdx_forWork[i] = i * 2 + 1;
// 			}
// 			else
// 			{
// 				nRobotIdx_forWork[i] = -1;
// 			}
// 		}
// 
// 		int nTargetInsert = vecTestEndInsert[0];
// 		if( vecEmptyInsert.size() > 0 )
// 			nTargetInsert = vecEmptyInsert[0];
// 				
// 		g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[1], vecTestEndInsert[0] );
// 		//g_robot.AddJob( JOB_PLACE, 0, 2, nTargetInsert );
// 
// 		if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == true && g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == true)
// 		{
// 			g_robot.AddJob( JOB_PLACE, 0, 2, nTargetInsert );
// 		}
// 		else if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == true && g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == false)
// 		{
// 			g_robot.AddJob( JOB_PLACE, 0, -1, nTargetInsert );
// 		}
// 		else if( g_site.GetEnableSocket( vecTestEndInsert[0] * 2 ) == false && g_site.GetEnableSocket( vecTestEndInsert[0] * 2 + 1) == true)
// 		{
// 			g_robot.AddJob( JOB_PLACE, -1, 2, nTargetInsert );
// 		}
// 
// 
// 		std::vector<int> vecEmptyCell = g_Tray.GetVecEmptyCell( nLotIdx, vecEnableTest[0], vecEnableTest[1] );
// 
// 		int nvecidx = 0;
// 		for( i=0; i<2; i++ )
// 		{
// 			if( mdl[i].GetModuleState() == EMS_GOOD )
// 			{
// 				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
// 				nvecidx++;
// 			}
// 		}
// 		for( i=0; i<2; i++ )
// 		{
// 			if( mdl[i].GetModuleState() == EMS_RELOAD )
// 			{
// 				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
// 				nvecidx++;
// 			}
// 		}
// 		for( i=0; i<2; i++ )
// 		{
// 			if( mdl[i].GetModuleState() == EMS_FAIL )
// 			{
// 				g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
// 				nvecidx++;
// 			}
// 		}
// 		return true;
// 
// 	}
// 	return false;
// }

bool APartHandler::CR_forTest( int nPickerCnt, const std::vector<int>& vecEmptyInsert, const std::vector<int>& vecMustTestMdl )
{
	if( (st_basic.n_pick_type != EPT_1V1M_4USE && st_basic.n_pick_type != EPT_1V1M_4USE_2)  && nPickerCnt == 4 )
		return false;                                                                                           

	// 빈 사이트가 2개 있고, 테스트할 모듈 4개 있다.
	std::vector<int> vecEnableTest;
	int i,j;
	for( i=0; i< vecMustTestMdl.size(); i++ )
	{
		int nPos = GetPosByTarget( vecMustTestMdl[i] );
		ETRAY_POS tpos = TPOS_BIN1;
		if( nPos == MPOS_INDEX_BIN2 )
			tpos = TPOS_BIN2;
		if( nPos == MPOS_INDEX_BIN3 )//ybs
			tpos = TPOS_BIN3;
		
		int nTrayIdx = g_Tray.GetIdxByPos( tpos );

		/////////////////////////////////////////////////////
		//2017.0102
		if( nTrayIdx < 0 ) return false;
		/////////////////////////////////////////////////////


		ATray& tray = g_Tray.GetTray( nTrayIdx );
		AModule& tmdl = tray.GetModule( vecMustTestMdl[i] % MPOS_DIVISION );
		
		bool bEnable = true;
		int nChkInsert = vecEnableTest.size() / 2;
		if( nChkInsert >= vecEmptyInsert.size() )
			break;

 		if( tmdl.GetModuleState() == EMS_RELOAD )
 		{
			//2014,0825
			int nC=0;
			if(tmdl.GetTestCnt() <= 0) nC = 0;
			else					   nC = tmdl.GetTestCnt()-1;
// 			for( int it=0; it< tmdl.GetTestCnt(); it++ )
			for(int it= nC; it< tmdl.GetTestCnt(); it++ )
  			{
				//2014,0825
// 				if( (tmdl.GetTestedPos(it) / 2 == vecEmptyInsert[nChkInsert]) )
// 				{
//					if(nPickerCnt == 4 && vecMustTestMdl.size() > 4)
//					{						
// 						bEnable = false;
// 						break;
//					}
//					else if(nPickerCnt == 2 && vecMustTestMdl.size() > 2)
// 					{						
// 						bEnable = false;
// 						break;
//					}
//				}
  			}
 		}
		if( bEnable )
		{
			vecEnableTest.push_back( vecMustTestMdl[i] );
		}
	}

	if( vecEmptyInsert.size() <= 0 ) return false;

	int k, ePosOld, iMdlIdx, n_First_Picker_X_Num = 0, nval = 1;
	int n_First_Tray_Y_Num=0,n_First_Tray_X_Num=0, n_First_Bin = -1, nPosY = 0,nPos = 0,nvcNum = 0;
	int nPickeruse[PICKER_NUM], nTargetPos[4];
	int nBinNum = 0, ix,iy,x,y,col,row;	

	for ( i =0 ; i < PICKER_NUM; i++)
	{
		nPickeruse[i] = CTL_NO;
		nTargetPos[i] = -1;
	}

	for ( i = 0; i < 3; i++) {
		for ( j = 0; j < 5; j++) {
			for ( k =0; k < 5; k++) {
				nBinMap[i][j][k] = -1;//3,row,col
			}
		}
	}

	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	std::vector<int> vecPicker;
	std::vector<int>::iterator it = vecEnableTest.begin();
	if( st_basic.n_pick_type == EPT_1V1M_4USE )
	{
		if( nPickerCnt == 4 && vecEnableTest.size() >= 4 )
		{
			ArrangeCRforTest(vecEnableTest, n_First_Tray_Y_Num, n_First_Tray_X_Num, n_First_Bin);
			
			bool a=false;
			for(i=0; i<PICKER_NUM; i++)	{
				if (nPickeruse[i] == CTL_NO) {
					if(a == false) {
						n_First_Picker_X_Num = i;
						a = true;
					}
					if(a){ i = PICKER_NUM; }
				}
			}

			nBinNum = 0;
			do {
				for(i=n_First_Picker_X_Num; i<PICKER_NUM; i++)
				{
					if (nPickeruse[i] == CTL_NO)
					{
						if(n_First_Tray_Y_Num + i >= 0 && ( n_First_Tray_Y_Num + (i - n_First_Picker_X_Num) ) < row)
						{
							if(nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] >= 0)
							{
								nTargetPos[i] = (n_First_Bin+1)*MPOS_DIVISION + nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)];
								nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] = -1;
								nPickeruse[i] = CTL_YES;
								vecPicker.push_back(i);
							}
							else nPickeruse[i] = CTL_NO;
						}			
					}
				}
				
				if(vecPicker.size() < 4 )
				{
					nBinNum = BinNumCRforTest(vecEnableTest, n_First_Bin);					

					if(nBinNum <= 0)
					{
						n_First_Bin++;
						if(n_First_Bin>=3 || n_First_Bin<0) n_First_Bin=0;
					}

					a=false;		
					for(i=0; i<PICKER_NUM; i++)	{						
						if (nPickeruse[i] == CTL_NO) {
							if(a == false) {
								n_First_Picker_X_Num = i;
								a = true;
							}
							if(a) {	i = PICKER_NUM; }
						}
					}
					a=false;					
					for( ix=0; ix<x; ix++ ){
						for ( iy=0; iy<y; iy++ )	{
							if (nBinMap[n_First_Bin][ix][iy] > -1){
								if(a == false){
									n_First_Tray_X_Num = ix;
									n_First_Tray_Y_Num = iy;
									a = true;
								}
								if(a) {	ix = x;	iy = y;	}
							}
						}
					}

				}
				
			} while(vecPicker.size()<4);

			nval = 1;
			if( st_handler.mn_mirror_type == CTL_YES )
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_180) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+2*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						(nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3]) )
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+2*x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+3*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[1]+2*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else
					{
						if(nTargetPos[0]+x != nTargetPos[1])
						{
							if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
								(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
								nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
							}
							else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
								nTargetPos[1]+x == nTargetPos[2])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
							}
							else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
								nTargetPos[2]+x == nTargetPos[3])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
								g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
							}
							else
							{
// 									g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
// 									g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
// 									g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
// 									g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
								g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
							}
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
							g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
							g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
						}
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval = -1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] && nTargetPos[2]+nval == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) &&
						(nTargetPos[2] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval*2 == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[2] / MPOS_DIVISION == nTargetPos[3] / MPOS_DIVISION &&
						(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3] ) &&
						(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						(nTargetPos[0]+nval == nTargetPos[1]) && nTargetPos[2]+nval == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						(nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3]) &&
						(nTargetPos[0] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+nval*2 == nTargetPos[2] && (nTargetPos[0] / row == nTargetPos[2] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval*3 == nTargetPos[3] && (nTargetPos[0] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[1]+nval*2 == nTargetPos[3] && (nTargetPos[1] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && (nTargetPos[0] / row == nTargetPos[1] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[2]+nval == nTargetPos[3] && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else
					{
						if(nTargetPos[0] % MPOS_DIVISION / y != nTargetPos[1] % MPOS_DIVISION / y)
						{
							if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
								(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
								nTargetPos[1]+nval == nTargetPos[2] && nTargetPos[2]+nval == nTargetPos[3] &&
								(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
							}
							else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
								(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[1] / row == nTargetPos[2] / row) )
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
							}
							else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
								(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2] / row == nTargetPos[3] / row))
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
								g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
							}
							else
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
								g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
							}
						}
						else//2014,0805
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
							g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
							g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
						}
					}
				}

			}
			else
			{			
				if(st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_0) x = -x;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+2*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						(nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3]) )
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+2*x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[0]+3*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[1]+2*x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						nTargetPos[0]+x == nTargetPos[1])
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
						nTargetPos[2]+x == nTargetPos[3])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else
					{
						if(nTargetPos[0]+x != nTargetPos[1])
						{
							if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
								(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
								nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
							}
							else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
								nTargetPos[1]+x == nTargetPos[2])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
							}
							else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) && 
								nTargetPos[2]+x == nTargetPos[3])
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
								g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
							}
							else
							{
// 								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
// 								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
// 								g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
// 								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
								g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
							}
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
							g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
							g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
						}

					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval = -1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] && nTargetPos[2]+nval == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) &&
						(nTargetPos[2] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval*2 == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[2] / MPOS_DIVISION == nTargetPos[3] / MPOS_DIVISION &&
						(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3] ) &&
						(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[2]+nval == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval*2 == nTargetPos[2] && nTargetPos[2]+nval == nTargetPos[3] &&
						(nTargetPos[0] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
					}
					else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
						nTargetPos[0]+nval*2 == nTargetPos[2] && (nTargetPos[0] / row == nTargetPos[2] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						nTargetPos[0]+nval*3 == nTargetPos[3] && (nTargetPos[0] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						(nTargetPos[1]+nval*2 == nTargetPos[3]) && (nTargetPos[1] / row == nTargetPos[3] / row) )
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
					else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
						nTargetPos[0]+nval == nTargetPos[1] && (nTargetPos[0] / row == nTargetPos[1] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
					}
					else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
						(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2] / row == nTargetPos[3] / row))
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
						g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
					}
					else
					{
						if(nTargetPos[0]+1 != nTargetPos[1])
						{
							if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
								(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
								nTargetPos[1]+nval == nTargetPos[2] && nTargetPos[2]+nval == nTargetPos[3] &&
								(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
							}
							else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
								(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[1] / row == nTargetPos[2] / row) )
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, nTargetPos[3] );
							}
							else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
								(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2] / row == nTargetPos[3] / row))
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, nTargetPos[1] );
								g_robot.AddJob( JOB_PICK, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
							}
							else
							{
								g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
								g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
								g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
								g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
							}
						}
						else//2014,0805
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
							g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[1] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[2] );
							g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
						}
					}
				}


			}
		


			g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, vecEmptyInsert[0] );
			g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, vecEmptyInsert[1] );

			return true;
		}
	}
	else if(st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치일 경우
	{
		if( nPickerCnt == 4 && vecEnableTest.size() >= 4 )
		{

			ePosOld = vecEnableTest[0] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[0] % MPOS_DIVISION;
			if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecEnableTest) == CTL_YES )
			{
				g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, vecEnableTest[0] );
				ePosOld = vecEnableTest[2] / MPOS_DIVISION;
				iMdlIdx = vecEnableTest[2] % MPOS_DIVISION;

				std::vector<int> vecMustTest = g_Tray.GetVecMustTestModule(0, vecEnableTest[0], vecEnableTest[1]);
				if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecMustTest) == CTL_YES )
				{
					g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, vecMustTest[0] );
				}
				else
				{
					g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[2] );
					g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
				}
				
//				g_robot.AddJob( JOB_PLACE, 0, 2,-1, -1, vecEmptyInsert[0] );
//				return true;
			}
			else
			{
				g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
				g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[1] );

				std::vector<int> vecMustTest = g_Tray.GetVecMustTestModule(0, vecEnableTest[0], vecEnableTest[1]);
				if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecMustTest) == CTL_YES )
				{
					g_robot.AddJob( JOB_PICK, 1, 3, -1, -1, vecMustTest[0] );
				}
				else
				{
					g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[2] );
					g_robot.AddJob( JOB_PICK, 3, -1, -1, -1, vecEnableTest[3] );
				}
//				g_robot.AddJob( JOB_PLACE, 0, 2, -1,-1, vecEmptyInsert[0] );
//				return true;
			}				


			g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, vecEmptyInsert[0] );
			g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, vecEmptyInsert[1] );

			return true;
		}		
	}
	if( nPickerCnt == 2 && vecEnableTest.size() >= 2 )
	{
		if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
		{
			ePosOld = vecEnableTest[0] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[0] % MPOS_DIVISION;
////2015.0717
// 			if( GetChkeckMdlHalfVPick( ePosOld , iMdlIdx, vecEnableTest) == CTL_YES )
			if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecEnableTest) == CTL_YES )
			{
				g_robot.AddJob( JOB_PICK, 0, 2, -1, -1, vecEnableTest[0] );
				g_robot.AddJob( JOB_PLACE, 0, 2,-1, -1, vecEmptyInsert[0] );
				return true;
			}
			else
			{
				g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
				g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[1] );

				g_robot.AddJob( JOB_PLACE, 0, 2, -1,-1, vecEmptyInsert[0] );
				return true;
			}
		}
		else
		{
			ArrangeCRforTest(vecEnableTest, n_First_Tray_Y_Num, n_First_Tray_X_Num, n_First_Bin);
			bool a=false;
			for(i=0; i<PICKER_NUM; i++)	{
				if (nPickeruse[i] == CTL_NO) {
					if(a == false) {
						n_First_Picker_X_Num = i;
						a = true;
					}
					if(a){ i = PICKER_NUM; }
				}
			}

			nBinNum = 0;			
			do {
				for(i=n_First_Picker_X_Num; i<PICKER_NUM; i+=2)
				{
					if (nPickeruse[i] == CTL_NO)
					{
						if(n_First_Tray_Y_Num + i >= 0 && ( n_First_Tray_Y_Num + (i - n_First_Picker_X_Num) ) < row)
						{
							if(nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] >= 0)
							{
								nTargetPos[i] = (n_First_Bin+1)*MPOS_DIVISION + nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)];
								nBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] = -1;
								nPickeruse[i] = CTL_YES;
								vecPicker.push_back(i);
							}
							else nPickeruse[i] = CTL_NO;
						}			
					}
				}	

				if(vecPicker.size() < 2 )
				{
					nBinNum = BinNumCRforTest(vecEnableTest, n_First_Bin);
					if(nBinNum <= 0)
					{
						n_First_Bin++;
						if(n_First_Bin>=3 || n_First_Bin<0) n_First_Bin=0;
					}

					a=false;		
					for(i=0; i<PICKER_NUM; i+=2)	{						
						if (nPickeruse[i] == CTL_NO) {
							if(a == false) {
								n_First_Picker_X_Num = i;
								a = true;
							}
							if(a) {	i = PICKER_NUM; }
						}
					}					
					a = false;
					for( ix=0; ix<x; ix++ ){
						for ( iy=0; iy<y; iy++ )	{
							if (nBinMap[n_First_Bin][ix][iy] > -1){
								if(a == false){
									n_First_Tray_X_Num = ix;
									n_First_Tray_Y_Num = iy;
									a = true;
								}
								if(a) {	ix = x;	iy = y;	}
							}
						}
					}
				}
				
			} while(vecPicker.size()<2);



			int nval = 1;
			if( st_handler.mn_mirror_type == CTL_YES )
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_180) x = -x;
					if( nTargetPos[0] / MPOS_DIVISION == nTargetPos[2] / MPOS_DIVISION &&
						nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval=-1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						}
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
			}
			else
			{
				if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
				{
					if(st_basic.n_degree == EDEG_0) x = -x;
					if( nTargetPos[0]+2*x == nTargetPos[2])
					{
						g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
				else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
				{
					if(st_basic.n_degree == EDEG_270) nval=-1;
					if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) )
					{
						if( (nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PICK, 0, -1, 2, -1, nTargetPos[0], -1, nTargetPos[2], -1);
						}
						else
						{
							g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
						}
					}
					else
					{
						g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, nTargetPos[0] );
						g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, nTargetPos[2] );
					}
				}
			}
				
		}

		g_robot.AddJob( JOB_PLACE, 0, 2, -1,-1, vecEmptyInsert[0] );
// 		if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
// 		{
// 			g_robot.AddJob( JOB_PLACE, 0, 2, -1,-1, vecEmptyInsert[0] );
// 		}
// 		else
// 		{
// 			g_robot.AddJob( JOB_PLACE, 0, 2, -1,-1, vecEmptyInsert[0] );
// 		}
		return true;
	}

	if( nPickerCnt == 2 && vecEnableTest.size() == 1 )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecEmptyInsert[0] );
		return true;
	}


//	if( st_basic.n_pick_type == EPT_1V1M_4USE )
//	{
//		if( nPickerCnt == 4 && vecEnableTest.size() >= 4 )
//		{
//			g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
//			g_robot.AddJob( JOB_PICK, 1, -1, vecEnableTest[1] );
//			g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[2] );
//			g_robot.AddJob( JOB_PICK, 3, -1, vecEnableTest[3] );
//			
//			g_robot.AddJob( JOB_PLACE, 0, 2, vecEmptyInsert[0] );
//			g_robot.AddJob( JOB_PLACE, 1, 3, vecEmptyInsert[1] );
//			return true;
//		}
//
//	}
//	else if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치일 경우
//	{
//		if( nPickerCnt == 4 && vecEnableTest.size() >= 4 )
//		{
//			g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
//			g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );
//			g_robot.AddJob( JOB_PICK, 1, -1, vecEnableTest[2] );
//			g_robot.AddJob( JOB_PICK, 3, -1, vecEnableTest[3] );
//			
//			g_robot.AddJob( JOB_PLACE, 1, 3, vecEmptyInsert[1] );
//			g_robot.AddJob( JOB_PLACE, 0, 2, vecEmptyInsert[0] );
//			return true;
//		}
//	}
//
//	if( nPickerCnt == 2 && vecEnableTest.size() >= 2 )
//	{
//		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
//		g_robot.AddJob( JOB_PICK, 2, -1, vecEnableTest[1] );
//		
//		g_robot.AddJob( JOB_PLACE, 0, 2, vecEmptyInsert[0] );
//		return true;
//	}
//
//	if( nPickerCnt == 2 && //g_lotMgr.GetLotCount() > 0 && g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" &&
//		vecEnableTest.size() == 1 /*&& g_Tray.GetLoadModuleCnt() == 1*/ )
//	{
//		g_robot.AddJob( JOB_PICK, 0, -1, vecEnableTest[0] );
//		g_robot.AddJob( JOB_PLACE, 0, -1, vecEmptyInsert[0] );
//		return true;
//	}

	return false;
}

bool APartHandler::CR_forEject_4( const std::vector<int>& vecTestEndInsert, const std::vector<int>& vecEmptyCell )
{
	//2013,1031
//	if( st_basic.n_pick_type != EPT_1V1M_4USE )
//		return false;
	if( st_basic.n_pick_type != EPT_1V1M_4USE && st_basic.n_pick_type != EPT_1V1M_4USE_2)
		return false;

	std::vector<int> vecPicker;
	std::vector<int> vecSitePicker;	

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
			int nRobotIdx_forWork[4];//,nRobotIdx[2][2];
			int nTypeNum=0, i = 0;

			for( i=0; i<4; i++ )
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
		
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[2], -1, -1, vecTestEndInsert[0] );
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[1], nRobotIdx_forWork[3], -1, -1, vecTestEndInsert[1] );

//			if( st_basic.n_pick_type == EPT_1V1M_4USE )
//			{
//				for( i=0; i<4; i++ )
//				{
//					if( mdl[i].GetModuleState() == EMS_GOOD )
//					{
//						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
//						nvecidx++;
//					}
//				}
//				for( i=0; i<4; i++ )
//				{
//					if( mdl[i].GetModuleState() == EMS_RELOAD )
//					{
//						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
//						nvecidx++;
//					}
//				}
//				for( i=0; i<4; i++ )
//				{
//					if( mdl[i].GetModuleState() == EMS_FAIL )
//					{
//						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, vecEmptyCell[nvecidx] );
//						nvecidx++;
//					}
//				}
//			}
			int nvecidx = 0, nvcNum = 0, nval = 1;
			int j = 0, ii = 0, k = 0, l = 0, iMdlIdx = 0, ePosNew = 0, ePosOld = 0, nPosY = 0,nPosX = 0, iPosFst = 0;
			int n_First_Tray_X_Num = 0, n_First_Tray_Y_Num = 0, n_First_Picker_X_Num = 0, nPos = 0, nSkip = CTL_NO;
			int nPickeruse[PICKER_NUM], nTargetPos[4], n_First_Bin = 0, nBinNum = 0;
			int ix,iy,x,y,col,row;

			std::vector<int> vecPicker; 
			std::vector<int> vecIdxPicker;
			
			for (j = 0; j < PICKER_NUM; j++)
			{
				nPickeruse[j] = CTL_NO;
				nTargetPos[j] = -1;
			}			

			x = col = st_basic.n_tray_x;
			y = row = st_basic.n_tray_y;
			
			if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
			{
				y = row = st_basic.n_tray_x;
				x = col = st_basic.n_tray_y;
			}

			if( st_basic.n_pick_type == EPT_1V1M_4USE )
			{
				for ( i = 0; i < PICKER_NUM; i++) {				
					nPickeruse[i] = CTL_NO;
					if(nRobotIdx_forWork[i] != -1) vecIdxPicker.push_back(i);//디바이스가 있는 피커
				}

				for ( i = 0; i < 3; i++) {
					for ( j = 0; j < 5; j++) {
						for ( k =0; k < 5; k++) {
							nEjectBinMap[i][j][k] = -1;//3,row,col
						}
					}
				}
				ArrangeEjectCRforTest(vecEmptyCell, n_First_Tray_Y_Num, n_First_Tray_X_Num, n_First_Bin);

				bool a=false;
				for(i=0; i<PICKER_NUM; i++)	{
					if (nPickeruse[i] == CTL_NO) {
						if(a == false) {
							n_First_Picker_X_Num = i;
							a = true;
						}
						if(a){ i = PICKER_NUM; }
					}
				}
				
				nBinNum = 0;			
				do {
					for(i=n_First_Picker_X_Num; i<PICKER_NUM; i++)
					{
						if (nPickeruse[i] == CTL_NO && nRobotIdx_forWork[i] != -1)
						{
							if(n_First_Tray_Y_Num + i >= 0 && ( n_First_Tray_Y_Num + (i - n_First_Picker_X_Num) ) < row)
							{
								if(nEjectBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] > -1)
								{
									nTargetPos[i] = (n_First_Bin+1)*MPOS_DIVISION + nEjectBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)];
									nEjectBinMap[n_First_Bin][n_First_Tray_X_Num][n_First_Tray_Y_Num + (i - n_First_Picker_X_Num)] = -1;
									nPickeruse[i] = CTL_YES;
									vecPicker.push_back(i);
								}
								else nPickeruse[i] = CTL_NO;
							}			
						}
					}

					nBinNum = 0;
					if(vecPicker.size() < 4 && vecPicker.size() < vecIdxPicker.size())
					{
						nBinNum = BinNumEjectCRforTest(vecEmptyCell, n_First_Bin);						

						if(nBinNum <= 0)
						{
							n_First_Bin++;
							if(n_First_Bin>=3 || n_First_Bin<0) n_First_Bin=0;
						}

						a=false;		
						for(i=0; i<PICKER_NUM; i++)	{						
							if (nPickeruse[i] == CTL_NO && nRobotIdx_forWork[i] != -1) {
								if(a == false) {
									n_First_Picker_X_Num = i;
									a = true;
								}
								if(a) {	i = PICKER_NUM; }
							}
						}
						a = false;
						for( ix=0; ix<x; ix++ ){
							for ( iy=0; iy<y; iy++ )	{
								if (nEjectBinMap[n_First_Bin][ix][iy] > -1){
									if(a == false){
										n_First_Tray_X_Num = ix;
										n_First_Tray_Y_Num = iy;
										a = true;
									}
									if(a) {	ix = x;	iy = y;	}
								}
							}
						}
					}
				}while(vecPicker.size()<4 && vecPicker.size() < vecIdxPicker.size());


				nval = 1;
				if( st_handler.mn_mirror_type == CTL_YES )
				{
					if( st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
					{
						if(st_basic.n_degree == EDEG_180) x = -x;
						if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+2*x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3]) )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+x == nTargetPos[1] && nTargetPos[2]+x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+2*x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							nTargetPos[0]+2*x == nTargetPos[2] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+3*x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[1]+2*x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							nTargetPos[0]+x == nTargetPos[1] )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[2]+x == nTargetPos[3] )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else
						{
							if(nTargetPos[0]+x != nTargetPos[1])
							{
								if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3] )
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
								}
								else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									nTargetPos[1]+x == nTargetPos[2] )
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
									g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
								}
								else if( (nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									nTargetPos[2]+x == nTargetPos[3] )
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
									g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
								}
								else
								{
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_GOOD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_RELOAD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_FAIL )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
								}
							}
							else
							{
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_GOOD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_RELOAD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_FAIL )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}

							}
						}	
					}
					else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
					{
						if(st_basic.n_degree == EDEG_270) nval = -1;
						if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] &&
							nTargetPos[2]+nval == nTargetPos[3] && (nTargetPos[0] / row == nTargetPos[1] / row) &&
							(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
						}				
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval == nTargetPos[1]) && (nTargetPos[1]+nval == nTargetPos[2]) && 
							(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[1]+nval  == nTargetPos[2]) && (nTargetPos[2]+nval  == nTargetPos[3]) &&
							(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval  == nTargetPos[1]) && (nTargetPos[2]+nval  == nTargetPos[3]) &&
							(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3]) &&
							(nTargetPos[0] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*3 == nTargetPos[3]) && (nTargetPos[0] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[1]+nval*2 == nTargetPos[3]) && (nTargetPos[1] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval == nTargetPos[1]) && (nTargetPos[0] / row == nTargetPos[1] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else
						{
							if(nTargetPos[0]+nval != nTargetPos[1])
							{
								if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3]) && 
									(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
								}
								else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[1]/row == nTargetPos[3]/row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
									g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
								}
								else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2]/row == nTargetPos[3]/row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
									g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
								}
								else
								{
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_GOOD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_RELOAD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_FAIL )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
								}
							}
							else
							{
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_GOOD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_RELOAD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_FAIL )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
							}
						}
					}
				}
				else
				{
					if(st_basic.n_degree == EDEG_0 || st_basic.n_degree == EDEG_180)
					{
						if(st_basic.n_degree == EDEG_0) x = -x;
						if(nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
						}
						else if(nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+x == nTargetPos[2])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if(nTargetPos[0]+x == nTargetPos[1] && nTargetPos[1]+2*x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 3, -1, nTargetPos[0], nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if( (nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3]) )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
						}
						else if(nTargetPos[0]+x == nTargetPos[1] && nTargetPos[2]+x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else if(nTargetPos[0]+2*x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
						}
						else if(nTargetPos[0]+2*x == nTargetPos[2])
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if(nTargetPos[0]+3*x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if(nTargetPos[1]+2*x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if(nTargetPos[0]+x == nTargetPos[1])
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if(nTargetPos[2]+x == nTargetPos[3])
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else
						{
							if(nTargetPos[0]+x != nTargetPos[1])
							{
								if( nTargetPos[1]+x == nTargetPos[2] && nTargetPos[2]+x == nTargetPos[3])
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
								}
								else if(nTargetPos[1]+x == nTargetPos[2])
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
									g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
								}
								else if(nTargetPos[2]+x == nTargetPos[3])
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
									g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
								}
								else
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
									g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
									g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
								}
							}
						}
					}
					else if(st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270)
					{
						if(st_basic.n_degree == EDEG_270) nval = -1;
						if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) && 
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) && 
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							nTargetPos[0]+nval == nTargetPos[1] && nTargetPos[1]+nval == nTargetPos[2] &&
							nTargetPos[2]+nval == nTargetPos[3] && (nTargetPos[0] / row == nTargetPos[1] / row) &&
							(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, 3, nTargetPos[0], nTargetPos[1], nTargetPos[2], nTargetPos[3]);
						}				
						else if( (nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval == nTargetPos[1]) && (nTargetPos[1]+nval == nTargetPos[2]) && 
							(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[1] / row == nTargetPos[2] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, 2, -1, nTargetPos[0], nTargetPos[1], nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[1]+nval  == nTargetPos[2]) && (nTargetPos[2]+nval  == nTargetPos[3]) &&
							(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row) )
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval  == nTargetPos[1]) && (nTargetPos[2]+nval  == nTargetPos[3]) &&
							(nTargetPos[0] / row == nTargetPos[1] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3]) &&
							(nTargetPos[0] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, 3, -1, nTargetPos[0], nTargetPos[2], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*2 == nTargetPos[2]) && (nTargetPos[0] / row == nTargetPos[2] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 2, -1, -1, nTargetPos[0], nTargetPos[2]);
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval*3 == nTargetPos[3]) && (nTargetPos[0] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 3, -1, -1, nTargetPos[0], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[1]+nval*2 == nTargetPos[3]) && (nTargetPos[1] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, 3, -1, -1, nTargetPos[1], nTargetPos[3] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
						}
						else if((nTargetPos[0] / MPOS_DIVISION) == (nTargetPos[1] / MPOS_DIVISION) &&
							(nTargetPos[0]+nval == nTargetPos[1]) && (nTargetPos[0] / row == nTargetPos[1] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, 1, -1, -1, nTargetPos[0], nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, nTargetPos[2] );
							g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
						}
						else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
							(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2] / row == nTargetPos[3] / row))
						{
							g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
							g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
							g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
						}
						else
						{
							if(nTargetPos[0]+nval != nTargetPos[1])
							{
								if( (nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									(nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[2]+nval == nTargetPos[3]) && 
									(nTargetPos[1] / row == nTargetPos[2] / row) && (nTargetPos[2] / row == nTargetPos[3] / row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, 3, -1, nTargetPos[1], nTargetPos[2], nTargetPos[3] );
								}
								else if((nTargetPos[1] / MPOS_DIVISION) == (nTargetPos[2] / MPOS_DIVISION) &&
									(nTargetPos[1]+nval == nTargetPos[2]) && (nTargetPos[1]/row == nTargetPos[3]/row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, 2, -1, -1, nTargetPos[1], nTargetPos[2] );
									g_robot.AddJob( JOB_PLACE, 3, -1, -1, -1, nTargetPos[3] );
								}
								else if((nTargetPos[2] / MPOS_DIVISION) == (nTargetPos[3] / MPOS_DIVISION) &&
									(nTargetPos[2]+nval == nTargetPos[3]) && (nTargetPos[2]/row == nTargetPos[3]/row))
								{
									g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, nTargetPos[0] );
									g_robot.AddJob( JOB_PLACE, 1, -1, -1, -1, nTargetPos[1] );
									g_robot.AddJob( JOB_PLACE, 2, 3, -1, -1, nTargetPos[2], nTargetPos[3] );
								}
								else
								{
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_GOOD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_RELOAD )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
									for( i=0; i<4; i++ )
									{
										if( mdl[i].GetModuleState() == EMS_FAIL )
										{
											g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
											nvecidx++;
										}
									}
								}
							}
							else
							{
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_GOOD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_RELOAD )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
								for( i=0; i<4; i++ )
								{
									if( mdl[i].GetModuleState() == EMS_FAIL )
									{
										g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1,vecEmptyCell[nvecidx] );
										nvecidx++;
									}
								}
							}
						}
					}					
				}
			}
			else//st_basic.n_pick_type == // EPT_1V1M_4USE_2
			{////2014,1027
				int nvecidx = 0;
				ePosOld = vecEmptyCell[0] / MPOS_DIVISION;
				iMdlIdx = vecEmptyCell[0] % MPOS_DIVISION;
				if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecEmptyCell) == CTL_YES )
				{
					if(nRobotIdx_forWork[1] != -1 && nRobotIdx_forWork[3] != -1)
					{
						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[1], nRobotIdx_forWork[3], -1, -1, vecEmptyCell[0] );
						nvecidx = 2;
					}
					else
					{
						if(nRobotIdx_forWork[1] != -1)
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[1], -1, -1, -1, vecEmptyCell[nvecidx] );
						else
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[3], -1, -1, -1, vecEmptyCell[nvecidx] );

						nvecidx++;
					}
					ePosOld = vecEmptyCell[2] / MPOS_DIVISION;
					iMdlIdx = vecEmptyCell[2] % MPOS_DIVISION;

// 					std::vector<int> vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0], vecEmptyCell[1]);
					std::vector<int> vecMustEmpty;
					if(nvecidx == 2)
					{
						vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0], vecEmptyCell[1]);
					}
					else
					{
						vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0]);
					}

					if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecMustEmpty) == CTL_YES )
					{
						//2015.0717
						if(nRobotIdx_forWork[0] != -1 && nRobotIdx_forWork[2] != -1)
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], nRobotIdx_forWork[2], -1, -1, vecMustEmpty[0] );
						else
						{
							if(nRobotIdx_forWork[0] != -1)
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, -1, -1, vecMustEmpty[0] );
							else
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[2], -1, -1, -1, vecMustEmpty[0] );

						}
					}
					else
					{
						if(nRobotIdx_forWork[0] != -1)
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, -1, -1, vecEmptyCell[nvecidx] );
							nvecidx++;
						}
						if(nRobotIdx_forWork[2] != -1)
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[2], -1, -1, -1, vecEmptyCell[nvecidx] );
							nvecidx++;
						}
					}					
				}
				else
				{
					if(nRobotIdx_forWork[1] != -1)
					{
						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[1], -1, -1, -1, vecEmptyCell[nvecidx] );
						nvecidx++;
					}
					if(nRobotIdx_forWork[3] != -1)
					{
						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[3], -1, -1, -1, vecEmptyCell[nvecidx] );
						nvecidx++;
					}

					ePosOld = vecEmptyCell[2] / MPOS_DIVISION;
					iMdlIdx = vecEmptyCell[2] % MPOS_DIVISION;
					
					
// 					std::vector<int> vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0], vecEmptyCell[1]);
					std::vector<int> vecMustEmpty;
					if(nvecidx == 2)
					{
						vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0], vecEmptyCell[1]);
					}
					else
					{
						vecMustEmpty = g_Tray.GetVecPutEmptyCell(0, vecEmptyCell[0]);
					}

					if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecMustEmpty) == CTL_YES )
					{
// 						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], nRobotIdx_forWork[2], -1, -1, vecMustEmpty[0] );

//2015.0717
						if(nRobotIdx_forWork[0] != -1 && nRobotIdx_forWork[2] != -1)
 							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], nRobotIdx_forWork[2], -1, -1, vecMustEmpty[0] );
 						else
 						{
// 							if(nRobotIdx_forWork[0] != -1)
// 							{
// 								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, -1, -1, vecMustEmpty[nvecidx] );
// 								nvecidx++;
// 							}
// 							else
// 							{
// 								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[2], -1, -1, -1, vecMustEmpty[nvecidx] );
// 								nvecidx++;
// 							}
// 							
							if(nRobotIdx_forWork[0] != -1)
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, -1, -1, vecEmptyCell[nvecidx] );
								nvecidx++;
							}
							if(nRobotIdx_forWork[2] != -1)
							{
								g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[2], -1, -1, -1, vecEmptyCell[nvecidx] );
								nvecidx++;
							}
						}
					}
					else
					{
						if(nRobotIdx_forWork[0] != -1)
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], -1, -1, -1, vecEmptyCell[nvecidx] );
							nvecidx++;
						}
						if(nRobotIdx_forWork[2] != -1)
						{
							g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[2], -1, -1, -1, vecEmptyCell[nvecidx] );
							nvecidx++;
						}
					}
				}

			}// EPT_1V1M_4USE_2
			return true;
		}
	} while ( false );

	return false;
}

bool APartHandler::CR_forEject_2( const std::vector<int>& vecTestEndInsert, const std::vector<int>& vecEmptyCell )
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
			
			g_robot.AddJob( JOB_PICK, nRobotIdx_forWork[0], nRobotIdx_forWork[1], -1, -1, vecTestEndInsert[0] );
		
			int nvecidx = 0, iMdlIdx = 0, ePosOld = 0, nTypeNum = 0;

			//2015.0717
 			if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
 			{
 				//만약 BIN3에 놓는다면 왼쪽인지 오른쪽인지 체크 후 만약 오른쪽이면 지그에 놓은 후 다시 픽업 후 2,3으로 픽업후 놓는다.
 				ePosOld = vecEmptyCell[0] / MPOS_DIVISION;
 				iMdlIdx = vecEmptyCell[0] % MPOS_DIVISION;
				if( (nRobotIdx_forWork[0] == 0 && nRobotIdx_forWork[1] == 2) )// || (nRobotIdx_forWork[0] == 2 && nRobotIdx_forWork[1] == 3))//세로 정렬
 				{
					if( OnCheckModuleVPick( ePosOld , iMdlIdx, vecEmptyCell) == CTL_YES )//2014,1027
 					{
 						g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[0], nRobotIdx_forWork[1], -1,-1, vecEmptyCell[0] );
 						return true;
 					}
 				}
 			}
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_GOOD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_RELOAD )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			for( i=0; i<2; i++ )
			{
				if( mdl[i].GetModuleState() == EMS_FAIL )
				{
					g_robot.AddJob( JOB_PLACE, nRobotIdx_forWork[i], -1, -1, -1, vecEmptyCell[nvecidx] );
					nvecidx++;
				}
			}
			return true;
		}
	} while ( false );
	

	return false;
}

bool APartHandler::CR_forReject( const std::vector<int>& vecFail, const std::vector<int>& vecEmptyRej )
{
	if( GetStepTTrans() != ETT_TO_NONE )
		return false;

	if( g_Tray.GetRejCnt() == 0 && GetRejAlarm() == false )
	{
		//2016.703
		st_msg.mstr_event_msg[0] = "불량 모듈을 담을 리젝트 트레이가 없습니다.";
		if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_event_msg[0] = "There is no reject tray.";
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
		
 		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		SetRejAlarm( true );
		//2014,0623
		if(g_client_ec.GetReqEmptyTray() == false)
			g_client_ec.OnReqEmptyTray(true);
		SetReqEmpty( true );
		return false;
	}
	else if( g_Tray.GetRejCnt() == 1 && GetReqEmpty() == false )
	{
		if(g_client_ec.GetReqEmptyTray() == false)
			g_client_ec.OnReqEmptyTray(true);
		SetReqEmpty( true );
	}
	else if( g_Tray.GetRejCnt() >= 2 )
	{
		if(GetReqEmpty() != false)
			ClearReqEmpty();
	}

	if( vecEmptyRej.size() == 0 )
		return false;

	if( vecFail.size() > 0 && g_Tray.GetRejCnt() == 0 && GetRejAlarm() == false )
	{
		st_msg.mstr_event_msg[0] = "불량 모듈을 담을 리젝트 트레이가 없습니다.";
		if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_event_msg[0] = "There is no reject tray.";
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
		
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		SetRejAlarm( true );
		COMI.mn_run_status = dSTOP;
		return false;
	}


	if( vecFail.size() > 0 && vecEmptyRej.size() > 0 )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecFail[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecEmptyRej[0] );
		return true;
	}
	else if( vecFail.size() > 0 && g_Tray.GetRejCnt() == 0 && GetRejAlarm() == false )
	{
		st_msg.mstr_event_msg[0] = "불량 모듈을 담을 리젝트 트레이가 없습니다.";
		if ( g_local.GetLocalType() == LOCAL_ENG ) st_msg.mstr_event_msg[0] = "There is no reject tray.";
		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);

		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

		SetRejAlarm( true );
		COMI.mn_run_status = dSTOP;
		return false;
	}
	return false;
}

bool APartHandler::CR_forStacker3MoveDvc(const std::vector<int>& vecEmptyRej,const std::vector<int>& vecEmptyInsert_Single, const std::vector<int>& vecEmptyInsert, const std::vector<int>& vecFail)
{
	int nCnt = 0;
	if( g_Tray.GetRejCnt() > 0 && vecEmptyRej.size() == 0 )
	{
		if(st_handler.mn_mirror_type == CTL_YES)
		{
			if(g_Tray.GetIdxByPos( TPOS_BIN1 ) > -1)
			{
				std::vector<int> vecMustTestM_A = g_Tray.GetVecStacker3MustTestModule(0);
				std::vector<int> vecTrayEmptyC_A = g_Tray.GetVecStacker3EmptyCell(0);

				//2013,0321
				std::vector<int> vecSubGood_A = g_Tray.GetVecGoodCell_3Stacker();
				if(vecSubGood_A.size() > 0  && vecTrayEmptyC_A.size() > 0)
				{
					if( CR_forSort( vecTrayEmptyC_A, vecSubGood_A ) )								return true;
				}
				else if(vecTrayEmptyC_A.size() < vecSubGood_A.size())
				{
					if(GetRejAlarm() == false)
					{
						CString strErr;
						strErr.Format( "Bin1의 tray에서 양품을 골라 빼주시고 트레이에 없음표시를 해 주세요.");
						if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Pick out the good device in Bin1 tray and no device mark in tray.");
						st_msg.mstr_event_msg[0] = strErr;
						::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
						{
							sprintf(st_msg.c_normal_msg,"Bin1의 tray에서 양품을 골라 빼주시고 트레이에 없음표시를 해 주세요.");
							if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Pick out the good device in Bin1 tray and no device mark in tray.");
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
						}
						SetRejAlarm(true);
					}
					return true;
				}

				std::vector<int> vecEmptyInsert_S = g_site.GetVecEmptyInsert_Single();
				std::vector<int> vecEmptyInsert = g_site.GetVecEmptyInsert();
				std::vector<int> vecFail = g_Tray.GetVecFailinTray();

				if(vecTrayEmptyC_A.size() >= 4 && vecMustTestM_A.size() >= 4)	nCnt = 4;
				else if(vecTrayEmptyC_A.size() >= 2 && vecMustTestM_A.size() >= 2)	nCnt = 2;
				else if(vecTrayEmptyC_A.size() >= 1 && vecMustTestM_A.size() >= 1)	nCnt = 1;
				else
				{
					nCnt = vecMustTestM_A.size();
					if(nCnt >= 4) nCnt = 4;
				}
				
				if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && st_basic.n_pick_type == EPT_1V1M_4USE && nCnt >= 4)
				{
					for (int i = 0; i < nCnt; i++)
					{
						g_robot.AddJob( JOB_PICK, i, -1, -1, -1, vecMustTestM_A[i] );
					}
					for (i = 0; i < nCnt; i++)
					{
						g_robot.AddJob( JOB_PLACE, i, -1, -1, -1, vecTrayEmptyC_A[i] );
					}
					return true;
				}
				else if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && nCnt >= 2)
				{
					g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecMustTestM_A[0] );
					g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecMustTestM_A[1] );
					
					g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecTrayEmptyC_A[0] );
					g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, vecTrayEmptyC_A[1] );
					return true;
				}
				else if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && (nCnt > 0 && nCnt < 2))
				{
					g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecMustTestM_A[0] );
					g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecTrayEmptyC_A[0] );
					return true;
				}

				int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
				int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
				int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
				
				
				if( iBin1 > -1 && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
					g_Tray.GetTray(iBin1).IsAllEmpty(true) == true)
				{
					g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
					if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
					{
						sprintf(st_msg.c_normal_msg,"!!!Bin1의 tray를 next로 보낸다.");
						if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Bin1 tray move to next");
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
					}
					return true;
				}
				
				if(vecTrayEmptyC_A.size() ==0 && (vecMustTestM_A.size() > 0 || vecFail.size() > 0))
				{
					if( CR_forTest_Single( vecEmptyInsert_S, vecMustTestM_A ) )					return true;				// 한 쪽이 DISABLE 된 소켓에 보낸다.
					if( CR_forTest( 4, vecEmptyInsert, vecMustTestM_A ) )						return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest( 2, vecEmptyInsert, vecMustTestM_A ) )						return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest_Single( vecEmptyInsert_S, vecFail ) )						return true;				// 한 쪽이 DISABLE 된 소켓에 보낸다.
					if( CR_forTest( 4, vecEmptyInsert, vecFail ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest( 2, vecEmptyInsert, vecFail ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
				}
			}
		}
		else
		{
			if(g_Tray.GetIdxByPos( TPOS_BIN3 ) > -1)
			{
				std::vector<int> vecMustTestM_A = g_Tray.GetVecStacker3MustTestModule(0);
				std::vector<int> vecTrayEmptyC_A = g_Tray.GetVecStacker3EmptyCell(0);
				// Sort
				std::vector<int> vecSubGood_A = g_Tray.GetVecGoodCell_3Stacker();
				
				if(vecSubGood_A.size() > 0  && vecTrayEmptyC_A.size() > 0)
				{
					if( CR_forSort( vecTrayEmptyC_A, vecSubGood_A ) )								return true;
				}
				else if(vecTrayEmptyC_A.size() < vecSubGood_A.size())
				{
					if(GetRejAlarm() == false)
					{
						CString strErr;
						strErr.Format( "Bin3의 tray에서 양품을 골라 빼주시고 트레이에 없음표시를 해 주세요.");
						if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Pick out the good device in Bin3 tray and no device mark in tray.");
						st_msg.mstr_event_msg[0] = strErr;
						::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
						{
							sprintf(st_msg.c_normal_msg,"Bin3의 tray에서 양품을 골라 빼주시고 트레이에 없음표시를 해 주세요.");
							if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Pick out the good device in Bin3 tray and no device mark in tray.");
							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
						}
						SetRejAlarm(true);
					}
					return true;
				}
				
				std::vector<int> vecEmptyInsert = g_site.GetVecEmptyInsert();
				std::vector<int> vecEmptyInsert_S = g_site.GetVecEmptyInsert_Single();
				std::vector<int> vecFail = g_Tray.GetVecFailinTray();
				
				
				if(vecTrayEmptyC_A.size() >= 4 && vecMustTestM_A.size() >= 4)	nCnt = 4;
				else if(vecTrayEmptyC_A.size() >= 2 && vecMustTestM_A.size() >= 2)	nCnt = 2;
				else if(vecTrayEmptyC_A.size() >= 1 && vecMustTestM_A.size() >= 1)	nCnt = 1;
				else
				{
					nCnt = vecMustTestM_A.size();
					if(nCnt >= 4)
					{
						nCnt = 4;
					}
				}
				
				if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && st_basic.n_pick_type == EPT_1V1M_4USE && nCnt >= 4)
				{
					for (int i = 0; i < nCnt; i++)
					{
						g_robot.AddJob( JOB_PICK, i, -1, -1, -1, vecMustTestM_A[i] );
					}
					for (i = 0; i < nCnt; i++)
					{
						g_robot.AddJob( JOB_PLACE, i, -1, -1, -1, vecTrayEmptyC_A[i] );
					}
					return true;
				}
				else if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && nCnt >= 2)
				{
					g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecMustTestM_A[0] );
					g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecMustTestM_A[1] );
					
					g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecTrayEmptyC_A[0] );
					g_robot.AddJob( JOB_PLACE, 2, -1, -1, -1, vecTrayEmptyC_A[1] );
					return true;
				}
				else if( vecTrayEmptyC_A.size() > 0 && vecMustTestM_A.size() > 0 && (nCnt > 0 && nCnt < 2))
				{
					g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecMustTestM_A[0] );
					g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecTrayEmptyC_A[0] );
					return true;
				}

				//2013,0106
				int iBin1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
				int iBin2 = g_Tray.GetIdxByPos( TPOS_BIN2 );
				int iBin3 = g_Tray.GetIdxByPos( TPOS_BIN3 );

				
				if( iBin3 > -1 && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) &&
					g_Tray.GetTray(iBin3).IsAllEmpty(true) == true)
				{
					g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
					if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
					{
						sprintf(st_msg.c_normal_msg,"!!!Bin3의 tray를 next로 보낸다.");
						if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Bin3 tray move to next");
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);
					}
					return true;
				}
			
				if(vecTrayEmptyC_A.size() == 0 && (vecMustTestM_A.size() > 0 || vecFail.size() > 0))
				{
// 					if(g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" && vecTrayEmptyC_A.size() == 0 && vecMustTestM_A.size() == 0 &&
// 						vecFail.size() > 0 && g_Tray.GetLoadModuleCnt() == 0)
// 					{
// 						if( iBin1 > -1 && g_Tray.GetTray(iBin1).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
// 						{
// 							g_Tray.GetTray(iBin1).SetTargetPos( TPOS_NEXT );
// 							sprintf(st_msg.c_normal_msg,"Bin1의 tray를 next로 보낸다.");
// 							if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Bin1 tray move to next");
// 						}
// 						else if( iBin2 > -1 && g_Tray.GetTray(iBin2).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
// 						{
// 							g_Tray.GetTray(iBin2).SetTargetPos( TPOS_NEXT );
// 							sprintf(st_msg.c_normal_msg,"Bin2의 tray를 next로 보낸다.");
// 							if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Bin2 tray move to next");
// 						}
// 						else if( iBin3 > -1 && g_Tray.GetTray(iBin3).GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
// 						{
// 							g_Tray.GetTray(iBin3).SetTargetPos( TPOS_NEXT );
// 							sprintf(st_msg.c_normal_msg,"Bin3의 tray를 next로 보낸다.");
// 							if ( g_local.GetLocalType() == LOCAL_ENG ) sprintf(st_msg.c_normal_msg, "Bin3 tray move to next");
// 						}
// 						if (st_handler.cwnd_list != NULL)
// 						{
// 							st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청	
// 						}
// 						return true;
// 					}
					// Sort ( Sub Tray에 있는 Good 을 Pri Tray 빈칸에 넣는다.
					if( CR_forTest_Single( vecEmptyInsert_S, vecMustTestM_A ) )					return true;				// 한 쪽이 DISABLE 된 소켓에 보낸다.
					if( CR_forTest( 4, vecEmptyInsert, vecMustTestM_A ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest( 2, vecEmptyInsert, vecMustTestM_A ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest_Single( vecEmptyInsert_S, vecFail ) )							return true;				// 한 쪽이 DISABLE 된 소켓에 보낸다.
					if( CR_forTest( 4, vecEmptyInsert, vecFail ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
					if( CR_forTest( 2, vecEmptyInsert, vecFail ) )								return true;				// 테스트 할 모듈을 사이트에 보낸다.
				}

			}
		}
	}
	return false;
}

bool APartHandler::CR_forMoveRejectDvc( const std::vector<int>& vecFail, const std::vector<int>& vecEmptyRej, const std::vector<int>& vecEmptyCell )
{
	if( vecEmptyRej.size() == 0 )
	{
		if( vecFail.size() > 0)
		{
			//if(vecFail[0] >= MPOS_INDEX_BIN3)
			//{
				g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecFail[0] );
				g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecEmptyCell[0]);
				return true;
			//}
		}
	}

	return false;
}

void APartHandler::CalcTrayMoveNext_fromBin3()
{
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		int iIdx = g_Tray.GetIdxByPos( TPOS_BIN1 );
		if( iIdx < 0 )											return;
		
		if( g_lotMgr.GetLotCount() == 0 )						return;
		if( g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )	return;
		if( g_Tray.GetLoadModuleCnt() > 0 )						return;

		//2013,0718
		int iIdx1 = g_Tray.GetIdxByPos( TPOS_BIN1 );
		ATray trayB1;
		if(iIdx1 > -1)
		{
			trayB1 = g_Tray.GetTray( iIdx1 );
			if( trayB1.GetStrLotID() != g_lotMgr.GetLotIDAt(0) )  return;
		}

		if( g_site.GetModuleCnt() > 0 )							return;
		if( g_robot.GetModuleCnt() > 0 )						return;
		
		std::vector<int> vecFailInTray = g_Tray.GetVecFailinTray();
		if( vecFailInTray.size() > 0 )	
			return;
		
		for( int i=0; i<MAX_TRAY; i++ )
		{
			ATray& tray = g_Tray.GetTray( i );
			if( tray.GetPos() != TPOS_REJECT && tray.GetPos() != TPOS_BIN1 && tray.GetPos() != TPOS_NONE )
			{
				return;
			}
		}
		
		ATray& trayBin1 = g_Tray.GetTray( iIdx );
		if( trayBin1.GetEmptyTray() )							return;
		
		CString strLog;
		strLog.Format( "[FromBin1]TPOS NEXT :	[%d] [%d]", iIdx, trayBin1.GetPos() );
		Func.On_LogFile_Add(99, strLog);
		trayBin1.SetTargetPos( TPOS_NEXT );

	}
	else
	{
		int iIdx = g_Tray.GetIdxByPos( TPOS_BIN3 );
		if( iIdx < 0 )											return;
		
		if( g_lotMgr.GetLotCount() == 0 )						return;
		if( g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )	return;
		if( g_Tray.GetLoadModuleCnt() > 0 )						return;
		//2013,0718
		int iIdx3 = g_Tray.GetIdxByPos( TPOS_BIN3 );
		ATray trayB3;
		if(iIdx3 > -1)
		{
			trayB3 = g_Tray.GetTray( iIdx3 );
			if( trayB3.GetStrLotID() != g_lotMgr.GetLotIDAt(0) )  return;
		}
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
		
		CString strLog;
		strLog.Format( "[FromBin3]TPOS NEXT :	[%d] [%d]", iIdx, trayBin3.GetPos() );
		Func.On_LogFile_Add(99, strLog);
		trayBin3.SetTargetPos( TPOS_NEXT );
	}

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
			if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "CalcTrayMoveNext_fromReject) %d TPOS_NEXT", i);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
		}
	}
}

void APartHandler::CheckLotEnd()
{
	if( g_lotMgr.GetLotCount() == 0 )								return;
	if( g_Tray.GetTrayCnt_byLotID( g_lotMgr.GetLotIDAt(0) ) > 0 )	return;
	if( g_lotMgr.GetLotAt(0).GetStrLastModule() != "YES" )			return;
	//2013,0405
// 	if( st_handler.mn_emptyin_cnt == 0)								return;
	//2013,0105
	int iMdlCnt = g_site.GetModuleCnt();
	if( iMdlCnt > 0)
	{
		for( int i=0; i<TSITE_SOCKET_CNT; i++ )
		{
			if(g_site.GetModule(i).IsExist())
			{
				if(g_site.GetModule(i).GetLotNo() == g_lotMgr.GetLotIDAt(0))
				{
					return;
				}
			}
		}
	}

	if( g_handler.GetMachinePos() == EMPOS_REAR ||
		g_handler.GetMachinePos() == EMPOS_ALONE )
		g_client_ec.OnTestEnd( g_lotMgr.GetLotIDAt(0), g_lotMgr.GetLotAt(0).GetPartID() );

	//2013,0913
	if( g_lotMgr.GetLotAt(0).GetStrLastModule() == "YES" && g_lotMgr.GetLotAt(0).GetStrRProtyModule() == "YES")
	{
		st_work.m_bPLotDel = true;
		for (int i = 0; i < MAX_PLOT; i++ )
		{
			memset(&st_work.cPLotName[i], NULL, sizeof(st_work.cPLotName[i]));
			st_work.nPLotInputCount[i][PRIME] = 0;
			st_work.nPLotPassCount[i][PRIME] = 0;
			st_work.nPLotFailCount[i][PRIME] = 0;
			st_work.fPLotYield[i][PRIME] = 0;
			st_work.nPLotInputCount[i][CUM] = 0;
			st_work.nPLotPassCount[i][CUM] = 0;
			st_work.nPLotFailCount[i][CUM] = 0;
			st_work.fPLotYield[i][CUM] = 0;
		}

	}

	if( st_basic.m_bFDC_Data == 1)//2013,1105
	{
		st_xgem.m_lTactTime = st_handler.m_tLotR[0].GetHours()*3600 + st_handler.m_tLotR[0].GetMinutes()*60 + st_handler.m_tLotR[0].GetSeconds();
		st_xgem.m_lTotCnt = g_lotMgr.GetLotAt(0).GetInputCnt(PRIME);
		st_xgem.m_lPassCnt = g_lotMgr.GetLotAt(0).GetPassCnt(PRIME) + g_lotMgr.GetLotAt(0).GetPassCnt(CUM);
		st_xgem.m_lFailCnt = g_lotMgr.GetLotAt(0).GetFailCnt(CUM);
		st_handler.m_tLotR[0] = 0;
	}
	//2017.0602
	if(st_handler.mn_xgem_mode == CTL_YES)
	{
		g_client_xgem.SetLotEndStatus(g_lotMgr.GetLotAt(0).GetLotID(), g_lotMgr.GetLotAt(0).GetPartID(),st_xgem.m_lTotCnt,st_xgem.m_lPassCnt,st_xgem.m_lFailCnt);
	}

	g_client_bpc.SetOnlineEnd( g_lotMgr.GetLotIDAt(0) );
	//2016.0703
	g_site.WriteSocketCount();//Save SocketCount
	Func.SaveSocketSkipRule();

	for ( int i = 0; i < g_lotMgr.GetLotCount(); i++)//2013,1105
	{
		if( g_lotMgr.GetLotCount() > (i+1) )
		st_handler.m_tLotR[i] = st_handler.m_tLotR[i + 1];
	}
	st_handler.m_tLotR[i] = 0;

	//2013,0311
	st_handler.mn_emptyout_cnt = 0;
	st_handler.mn_emptynext_cnt = 0;
	st_handler.mn_emptyin_cnt = 0;
	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
	}

	if(st_basic.mn_mode_xgem == CTL_YES && g_lotMgr.GetLotCount() > 0)
	{
		Func.OnSavePPID(true);
	}

	//2016.0709
	Func.SaveSocketSkipRule();

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
		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "CalcTrayMoveNext_fromRejFull1) %d TPOS_NEXT", 1);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}

		if( g_Tray.GetRejCnt() > 1 )
			return;

		if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
		{
			sprintf(st_msg.c_normal_msg, "CalcTrayMoveNext_fromRejFull2)  %d TPOS_NEXT", 2);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
		}
		//2013,1220
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
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_normal_msg, "CalcTrayMoveNext_fromRejFull2) %d Bin1Send TPOS_NEXT", 2);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
					}
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
					if (st_handler.cwnd_list != NULL)  // 리스트 바 화면 존재
					{
						sprintf(st_msg.c_normal_msg, "CalcTrayMoveNext_fromRejFull3) %d trayBin2 TPOS_NEXT", 3);
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
					}
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

bool APartHandler::CR_forTest_Single( const std::vector<int>& vecEmptyInsert_Single, const std::vector<int>& vecMustTestMdl )
{
	if( vecEmptyInsert_Single.size() == 0 || vecMustTestMdl.size() == 0 )
		return false;
//2014,0902
//	if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2) )
//	{
//		g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecMustTestMdl[0] );
//		g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecEmptyInsert_Single[0] );
//		return true;
//	}
//	else if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2 + 1) )
//	{
//		if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
//		{
//			g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecMustTestMdl[0] );
//			g_robot.AddJob( JOB_PLACE, -1, 1, -1, -1, vecEmptyInsert_Single[0] );
//		}
//		else
//		{
//			g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecMustTestMdl[0] );
//			g_robot.AddJob( JOB_PLACE, -1, 2, -1, -1, vecEmptyInsert_Single[0] );
//		}
//		return true;
//	}
	std::vector<int> vecEnableTest;	
	int i = 0, nC=0;
	for( i=0; i< vecMustTestMdl.size(); i++ )
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
			if(tmdl.GetTestCnt() <= 0) nC = 0;
			else			   nC = tmdl.GetTestCnt()-1;
			
			for( int it=nC; it< tmdl.GetTestCnt(); it++ )
			{
				int nPos = 0;
//				if(tmdl.GetTestedPos(it) >= 10) nPos = tmdl.GetTestedPos(it) - 10;
//				else 				nPos = tmdl.GetTestedPos(it);
				if( tmdl.GetTestedPos(it)%2 != 0) nPos = 1;

				if( ( ( tmdl.GetTestedPos(it)/2) == vecEmptyInsert_Single[0] ) && vecMustTestMdl.size() > 1)
				{
					bEnable = false;
					break;
				}
			}
		}
		if(bEnable == true)
		{
			vecEnableTest.push_back( vecMustTestMdl[i] );
			break;
		}
	}
	if( vecEnableTest.size() <= 0 ) return false;
	if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2) > 0 /*g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2)*/ )
	{
		g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecEnableTest[0] );
		g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecEmptyInsert_Single[0] );
		return true;
	}
	else if( g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2 + 1) > 0 /*g_site.GetEnableSocket( vecEmptyInsert_Single[0] * 2 + 1)*/ )
	{
//		if( st_basic.n_pick_type == EPT_1V1M_4USE_2 )//2.5인치
//		{
//			g_robot.AddJob( JOB_PICK, 1, -1, -1, -1, vecEnableTest[0] );
//			g_robot.AddJob( JOB_PLACE, -1, 1, -1, -1, vecEmptyInsert_Single[0] );
//		}
//		else
//		{
			g_robot.AddJob( JOB_PICK, 2, -1, -1, -1, vecEnableTest[0] );
			g_robot.AddJob( JOB_PLACE, -1, 2, -1, -1, vecEmptyInsert_Single[0] );
//		}
		return true;
	}
	return false;
}

bool APartHandler::CR_forSort( const std::vector<int>& vecPriEmptyCell, const std::vector<int>& vecSubGoodCell )
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


	g_robot.AddJob( JOB_PICK, 0, -1, -1, -1, vecSubGoodCell[0] );
	g_robot.AddJob( JOB_PLACE, 0, -1, -1, -1, vecPriEmptyCell[0] );

	return true;
}

bool APartHandler::IsTrayOthers()
{
	bool bRet = true;
	int i = 0, nTray = 0, iBin[20] = {0,};
	for( i=0; i<MAX_TRAY; i++ )
	{		
		ATray& tray = g_Tray.GetTray( i );
		if ( tray.GetStrLotID() == g_lotMgr.GetLotIDAt(0) )
		{
			if( tray.GetPos() != TPOS_REJECT && tray.GetPos() != TPOS_NONE && tray.GetPos() != TPOS_CONV3_TO_NEXT )
			{
				nTray++;
				//break;
			}
		}
	}

	if(nTray > 1) bRet = false;

	return bRet;
}

bool APartHandler::GetAllPickEnable( std::vector<int> vecDVCMdl, int& nDvcMdl )
{
	int nSiteNum = 0, ePosNew = 0, iMdlIdx = 0, numVec = 0;
	std::vector<int> n4APicker;
	std::vector<int> n4BPicker;
	int ePosOld = vecDVCMdl[0] / MPOS_DIVISION;
	for( int i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosNew = vecDVCMdl[i] / MPOS_DIVISION;
		if( ePosOld == ePosNew )
		{
			nSiteNum++;
		}		
	}

	if(nSiteNum >= 4 )
	{
		for( i = 0; i < vecDVCMdl.size(); i++ )
		{
			ePosNew = vecDVCMdl[i] / MPOS_DIVISION;
			iMdlIdx = vecDVCMdl[i] % MPOS_DIVISION;
			if(ePosOld == ePosNew)
			{
				if(iMdlIdx == 1)
				{
					n4APicker.push_back(iMdlIdx);
					n4BPicker.push_back(iMdlIdx);				
				}
				if(iMdlIdx == 0)	n4APicker.push_back(iMdlIdx);
				if(iMdlIdx == 3)
				{
					numVec = i;
					n4APicker.push_back(iMdlIdx);
					//n4BPicker.push_back(iMdlIdx);
				}
				if(iMdlIdx == 2)
				{
					n4APicker.push_back(iMdlIdx);
					n4BPicker.push_back(iMdlIdx);
				}
				if(iMdlIdx == 5)	n4BPicker.push_back(iMdlIdx);
				if(iMdlIdx == 4)
				{
					n4APicker.push_back(iMdlIdx);
					n4BPicker.push_back(iMdlIdx);				
					numVec = i;
				}		
			}
		}
	}

	if( n4APicker.size() >= 4)
	{
		nDvcMdl = numVec; return true;
	}
	if( n4BPicker.size() >= 4)
	{
		nDvcMdl = numVec; return true;
	}
	else
		return false;
}

bool APartHandler::GetChkeckMdlAllPick( int nSite, int nMdl, std::vector<int> vecDVCMdl )
{
	int ePosOld = 0, ePosNew = 0, iPosFst = 0, nPos;
	std::vector<int> n4Picker;	
	for( int i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosOld = vecDVCMdl[i] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[i] % MPOS_DIVISION;
		if( ePosOld == nSite )
		{
			if(iPosFst == nMdl)
			{
				nPos = i;
				//한번에 4개를 픽업할 수 있는가?
				for( int ii = nPos; ii < vecDVCMdl.size(); ii++ )
				{
					ePosNew = vecDVCMdl[ii] / MPOS_DIVISION;
					iPosFst = vecDVCMdl[ii] % MPOS_DIVISION;
					if( ePosNew == ePosOld)
					{
						if( nMdl - st_basic.n_tray_x == iPosFst )
						{
							n4Picker.push_back(iPosFst);
						}
						if( nMdl + 1 == iPosFst && ( (nMdl + 1) < st_basic.n_tray_x * st_basic.n_tray_y) )
						{
							n4Picker.push_back(iPosFst);
						}
						if( nMdl -2 == iPosFst && (nMdl -2 > 0 && (nMdl - 2) < st_basic.n_tray_x * st_basic.n_tray_y))
						{
							n4Picker.push_back(iPosFst);
						}
						if(n4Picker.size() >= 3) return true;
					}
					else
					{
						return false;
					}
				}				
			}
		}
	}
	return false;
}

bool APartHandler::GetChkeckMdlHalfVPick( int nSite, int nMdl, std::vector<int> vecDVCMdl )
{
	int ePosOld, ePosNew, iPosFst, nPos;
	bool bAllPick = false;
	std::vector<int> n2Picker;
	std::vector<int> n4Picker;
	
	for( int i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosOld = vecDVCMdl[i] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[i] % MPOS_DIVISION;

		if( ePosOld == nSite && iPosFst == nMdl)
		{		
			nPos = i;
			//한번에 2개를 픽업할 수 있는가?
			for(int ii = nPos; ii < vecDVCMdl.size(); ii++ )
			{
				bAllPick = true;
				ePosNew = vecDVCMdl[ii] / MPOS_DIVISION;
				iPosFst = vecDVCMdl[ii] % MPOS_DIVISION;
				if( ePosNew == ePosOld )
				{
					if( nMdl+1 == iPosFst && (nMdl+1 < st_basic.n_tray_x * st_basic.n_tray_y) )
					{
						n4Picker.push_back(iPosFst);
						return true;
					}				
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}

bool APartHandler::OnCheckModuleVPick( int nSite, int nMdl, std::vector<int> vecDVCMdl )
{
	if( st_basic.n_pick_type != EPT_1V1M_4USE_2 )
		return false;
	
	int ePosOld, ePosNew, iPosFst, nPos;
	bool bAllPick = false;
	std::vector<int> n2Picker;
	std::vector<int> n4Picker;

	int x = st_basic.n_tray_x;
	int y = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = st_basic.n_tray_x;
		x = st_basic.n_tray_y;
	}

//2015.0717	
//	for( int i = 0; i < vecDVCMdl.size(); i++ )
//	{
		ePosOld = vecDVCMdl[0] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[0] % MPOS_DIVISION;

		int nIdx = 1;
		if( ePosOld == nSite && iPosFst == nMdl)
		{		
			nPos = 1;//2015.0706 i -> 1
			//한번에 2개를 픽업할 수 있는가?
			for(int ii = nPos; ii < vecDVCMdl.size(); ii++ )
			{
				bAllPick = true;
				ePosNew = vecDVCMdl[ii] / MPOS_DIVISION;
				iPosFst = vecDVCMdl[ii] % MPOS_DIVISION;
				if( ePosNew == ePosOld )
				{
					if( st_handler.mn_mirror_type == CTL_YES )
					{
						if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
						{
							//2015.1002
//							if( st_basic.n_degree == EDEG_90 )	nIdx = -1;
//							if( nMdl+nIdx == iPosFst && (nMdl+nIdx < st_basic.n_tray_x * st_basic.n_tray_y) )
							if( st_basic.n_degree == EDEG_270 )	nIdx = -1;
							if( nMdl+nIdx == iPosFst && ((nMdl+nIdx) % y != 0) && (nMdl+nIdx < st_basic.n_tray_x * st_basic.n_tray_y) )
							{
								n4Picker.push_back(iPosFst);
								return true;
							}				
						}
					}
					else
					{
						if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
						{
							if( st_basic.n_degree == EDEG_270 )	nIdx = -1;
							if( nMdl+nIdx == iPosFst && ((nMdl+nIdx) % y != 0) && (nMdl+nIdx < st_basic.n_tray_x * st_basic.n_tray_y) )
							{
								n4Picker.push_back(iPosFst);
								return true;
							}				
						}
					}
				}
				else
				{
					return false;
				}
			}
//		}
	}
	return false;
}

bool APartHandler::GetChkeckMdlHalfHPick( int nSite, int nMdl, std::vector<int> vecDVCMdl )
{
	int ePosOld, ePosNew, iPosFst, nPos;
	bool bAllPick = false;
	std::vector<int> n2Picker;
	std::vector<int> n4Picker;
	
	for( int i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosOld = vecDVCMdl[i] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[i] % MPOS_DIVISION;

		if( ePosOld == nSite )
		{
			if(iPosFst == nMdl)
			{
				nPos = i;
				//한번에 4개를 픽업할 수 있는가?
				for( int ii = nPos; ii < vecDVCMdl.size(); ii++ )
				{
					bAllPick = true;
					ePosNew = vecDVCMdl[ii] / MPOS_DIVISION;
					iPosFst = vecDVCMdl[ii] % MPOS_DIVISION;
					if( ePosNew == ePosOld )
					{
						if( nMdl-st_basic.n_tray_x > 0 && nMdl-st_basic.n_tray_x == iPosFst && (nMdl-st_basic.n_tray_x  < st_basic.n_tray_x * st_basic.n_tray_y) )					
						{
							n4Picker.push_back(iPosFst);
							return true;
						}
					}
					else
					{
						return false;
					}
				}
				
			}
		}
	}
	return false;
}

bool APartHandler::IsCheckMdlAllPick( int nSite, int* picker, std::vector<int> vecDVCMdl, int* nSameX, int (*nSendfPicker)[4] )
{
	int ePosOld = 0, ePosNew = 0, iPosFst = 0, nfPicker[PICKER_NUM], nsPicker[PICKER_NUM], nfSameX[PICKER_NUM], nsSameX[PICKER_NUM];
	int i = 0, j = 0, n_First_Tray_Y_Num = 0, nfPickerTrayInfo[10][PICKER_NUM], nsPickerTrayInfo[10][PICKER_NUM];
	std::vector<int> fPicker;
	std::vector<int> sPicker;

	for ( i = 0 ; i < 10; i++ )//트레이 세로로 몇번째
	{
		for (j = 0; j < PICKER_NUM; j++)
		{
			nfPickerTrayInfo[i][j] = -1;
			nsPickerTrayInfo[i][j] = -1;
		}
	}

	for ( i = 0; i < PICKER_NUM; i++ )
	{
		nfSameX[i] = CTL_NO;
		nsSameX[i] = CTL_NO;
		nfPicker[i] = picker[i];
		nsPicker[i] = picker[i];
	}

	for( i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosOld = vecDVCMdl[i] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[i] % MPOS_DIVISION;
		if( ePosOld == nSite )
		{
			for ( j = 0 ; j < PICKER_NUM; j++)
			{
				if( j == 0 && iPosFst == 1 && nfPicker[0] == CTL_NO)
				{
					fPicker.push_back(j);
					nfPicker[j] = CTL_YES;
					nfSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nfPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
				else if( j == 1 && iPosFst == 3 && nfPicker[1] == CTL_NO)
				{
					fPicker.push_back(j);
					nfPicker[j] = CTL_YES;
					nfSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nfPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
				else if( j == 2 && iPosFst == 0 && nfPicker[2] == CTL_NO)
				{
					fPicker.push_back(j);
					nfPicker[j] = CTL_YES;
					nfSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nfPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
				else if( j == 3 && iPosFst == 2 && nfPicker[3] == CTL_NO)
				{
					fPicker.push_back(j);
					nfPicker[j] = CTL_YES;
					nfSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nfPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
			}
		}
	}
	
	for( i = 0; i < vecDVCMdl.size(); i++ )
	{
		ePosOld = vecDVCMdl[i] / MPOS_DIVISION;
		iPosFst = vecDVCMdl[i] % MPOS_DIVISION;
		if( ePosOld == nSite )
		{
			for ( j = 0 ; j < PICKER_NUM; j++)
			{
				if( j == 0 && iPosFst == 3 && nsPicker[0] == CTL_NO)
				{
					sPicker.push_back(j);
					nsPicker[j] = CTL_YES;
					nsSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nsPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
				else if( j == 1 && iPosFst == 5 && nsPicker[1] == CTL_NO)
				{
					sPicker.push_back(j);
					nsPicker[j] = CTL_YES;
					nsSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nsPickerTrayInfo[n_First_Tray_Y_Num][j] = j;
					break;
				}
				else if( j == 2 && iPosFst == 2 && nsPicker[2] == CTL_NO)
				{
					sPicker.push_back(j);
					nsPicker[j] = CTL_YES;
					nsSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nsPickerTrayInfo[n_First_Tray_Y_Num][j] = j;					
				}
				else if( j == 3 && iPosFst == 4 && nsPicker[3] == CTL_NO)
				{
					sPicker.push_back(j);
					nsPicker[j] = CTL_YES;
					nsSameX[j] = vecDVCMdl[i];
					n_First_Tray_Y_Num = iPosFst/st_basic.n_tray_x; 
					nsPickerTrayInfo[n_First_Tray_Y_Num][j] = j;					
				}
			}
		}
	}

	if(fPicker.size() > 0 && fPicker.size() >= sPicker.size())
	{
		for ( i = 0 ; i < PICKER_NUM; i++)
		{
			picker[i] = nfPicker[i];
			nSameX[i] = nfSameX[i];
		}
		for ( i = 0 ; i < 10; i++ )//트레이 세로로 몇번째
		{
			for (j = 0; j < PICKER_NUM; j++)
			{
				nSendfPicker[i][j] = nfPickerTrayInfo[i][j];
			}
		}
		return true;
	}
	else if(sPicker.size() > 0 && fPicker.size() < sPicker.size())
	{
		for ( i = 0 ; i < PICKER_NUM; i++)
		{
			picker[i] = nsPicker[i];
			nSameX[i] = nsSameX[i];
		}
		for ( i = 0 ; i < 10; i++ )//트레이 세로로 몇번째
		{
			for (j = 0; j < PICKER_NUM; j++)
			{
				nSendfPicker[i][j] = nsPickerTrayInfo[i][j];
			}
		}
		return true;
	}

	return false;
}

void APartHandler::ArrangeCRforTest( std::vector<int> vecEnableTest, int& n_First_Tray_Y_Num, int& n_First_Tray_X_Num, int& n_First_Bin)
{
	int i = 0, ePosNew, iMdlIdx, ix,iy,x,y,col,row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][(x-1)-ix][(y-1)-iy] = iMdlIdx;//2014,0805

				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;//2014,0805
				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
		}
	}
	else
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][(x-1)-ix][(y-1)-iy] = iMdlIdx;//2014,0805
				
// 				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
// 				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);

			}
		}
		
	}
	n_First_Bin = vecEnableTest[0] / MPOS_DIVISION - 1;
}

int APartHandler::BinNumCRforTest( std::vector<int> vecEnableTest, int n_First_Bin)
{
	int i, ePosNew, iMdlIdx, ix, iy, x, y, col, row, nBinNum = 0;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( i = 0; i < vecEnableTest.size(); i++ )
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	else
	{
		for( i = 0; i < vecEnableTest.size(); i++ )
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nBinMap[ePosNew-1][ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	return nBinNum;	
}


void APartHandler::FirstNumCRForTest(int& n_First_Tray_X_Num, int& n_First_Tray_Y_Num, int n_First_Bin)
{
	bool a = false;
	int  ix, iy, x, y, col, row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}
	if(st_handler.mn_mirror_type == CTL_YES)
	{
		if( st_basic.n_degree == EDEG_0 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=y-1; iy>=0; iy-- )	{
					if (nBinMap[n_First_Bin][ix][(y-1)-iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_90 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=0; iy<y; iy++ )	{
					if (nBinMap[n_First_Bin][ix][iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_180 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=0; iy<y; iy++ )	{
					if (nBinMap[n_First_Bin][ix][iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_270 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=y-1; iy>=0; iy-- )	{
					if (nBinMap[n_First_Bin][(x-1)-ix][(y-1)-iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = (x-1)-ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = y;	}
					}
				}
			}
		}
	}
	else
	{
		if( st_basic.n_degree == EDEG_0 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=y-1; iy>=0; iy-- )	{
					if (nBinMap[n_First_Bin][ix][(y-1)-iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_90 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=0; iy<y; iy++ )	{
					if (nBinMap[n_First_Bin][ix][iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_180 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=0; iy<y; iy++ )	{
					if (nBinMap[n_First_Bin][ix][iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = ix;
							n_First_Tray_Y_Num = iy;
							a = true;
						}
						if(a) {	ix = x;	iy = 0;	}
					}
				}
			}
		}
		else if( st_basic.n_degree == EDEG_270 )
		{
			for( ix=0; ix<x; ix++ ){
				for ( iy=y-1; iy>=0; iy-- )	{
					if (nBinMap[n_First_Bin][(x-1)-ix][(y-1)-iy] > -1){
						if(a == false){
							n_First_Tray_X_Num = (x-1)-ix;
							n_First_Tray_Y_Num = (y-1)-iy;
							a = true;
						}
						if(a) {	ix = x;	iy = y;	}
					}
				}
			}
		}
	}
}

void APartHandler::ArrangeEjectCRforTest( std::vector<int> vecEmptyCell, int& n_First_Tray_Y_Num, int& n_First_Tray_X_Num, int& n_First_Bin)
{
	int i = 0, ePosNew, iMdlIdx, ix,iy,x,y,col,row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for ( i = 0; i < vecEmptyCell.size(); i++)
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nEjectBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nEjectBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nEjectBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nEjectBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
		}
	}
	else
	{
		for ( i = 0; i < vecEmptyCell.size(); i++)
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nEjectBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nEjectBinMap[ePosNew-1][ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nEjectBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nEjectBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;
				
// 				n_First_Tray_X_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
// 				n_First_Tray_Y_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
		}
		
	}
	n_First_Bin = vecEmptyCell[0] / MPOS_DIVISION - 1;
}

int APartHandler::BinNumEjectCRforTest( std::vector<int> vecEmptyCell, int n_First_Bin)
{
	int i, ePosNew, iMdlIdx, ix, iy, x, y, col, row, nBinNum = 0;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
					
				}
			}
		}
	}
	else
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	return nBinNum;	
}


void APartHandler::ArrangeExforTest( std::vector<int> vecEnableTest, int& n_First_Tray_Y_Num, int& n_First_Tray_X_Num, int& n_First_Bin)
{
	int i = 0, ePosNew, iMdlIdx, ix,iy,x,y,col,row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) % y);

			}
		}
	}
	else
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExBinMap[ePosNew-1][ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;
				
// 				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
// 				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
		}
		
	}
	n_First_Bin = vecEnableTest[0] / MPOS_DIVISION - 1;
}

int APartHandler::BinNumExforTest( std::vector<int> vecEnableTest, int n_First_Bin)
{
	int i, ePosNew, iMdlIdx, ix, iy, x, y, col, row, nBinNum = 0;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( i = 0; i < vecEnableTest.size(); i++ )
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}

				}
			}
		}
	}
	else
	{
		for( i = 0; i < vecEnableTest.size(); i++ )
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExBinMap[ePosNew-1][ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	return nBinNum;	
}

void APartHandler::ArrangeExEmpforTest( std::vector<int> vecEmptyCell, int& n_First_Tray_Y_Num, int& n_First_Tray_X_Num, int& n_First_Bin)
{
	int i = 0, ePosNew, iMdlIdx, ix,iy,x,y,col,row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for ( i = 0; i < vecEmptyCell.size(); i++)
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) % y);

			}
		}
	}
	else
	{
		for ( i = 0; i < vecEmptyCell.size(); i++)
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

				n_First_Tray_Y_Num = (y-1) - ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExEmpBinMap[ePosNew-1][ix][iy] = iMdlIdx;

				n_First_Tray_X_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

				n_First_Tray_Y_Num = ((vecEmptyCell[0] % MPOS_DIVISION) / x);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;
				
// 				n_First_Tray_X_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
// 				n_First_Tray_Y_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
				n_First_Tray_X_Num = (x-1)-((vecEmptyCell[0] % MPOS_DIVISION) / y);
				n_First_Tray_Y_Num = (y-1)-((vecEmptyCell[0] % MPOS_DIVISION) % y);
			}
		}
		
	}
	n_First_Bin = vecEmptyCell[0] / MPOS_DIVISION - 1;
}

int APartHandler::BinNumExEmpforTest( std::vector<int> vecEmptyCell, int n_First_Bin)
{
	int i, ePosNew, iMdlIdx, ix, iy, x, y, col, row, nBinNum = 0;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	else
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExEmpBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExEmpBinMap[ePosNew-1][ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nExEmpBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nExEmpBinMap[ePosNew-1][(x-1)-ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	return nBinNum;	
}

void APartHandler::Arrange25CRforTest( std::vector<int> vecEnableTest, int& n_First_Tray_Y_Num1, int& n_First_Tray_Y_Num2, int& n_First_Tray_X_Num1, int& n_First_Tray_X_Num2, int& n_First_Bin)
{
	int i = 0, ePosNew, iMdlIdx, ix,iy,x,y,col,row;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

//				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
//				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

//				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
//				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

//				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
//				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

//				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
//				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
		}
	}
	else
	{
		for ( i = 0; i < vecEnableTest.size(); i++)
		{
			ePosNew = vecEnableTest[i] / MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			iMdlIdx = vecEnableTest[i] % MPOS_DIVISION;
			if( st_basic.n_degree == EDEG_0 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][ix][(y-1)-iy] = iMdlIdx;

//				n_First_Tray_Y_Num = (y-1) - ((vecEnableTest[0] % MPOS_DIVISION) / x);
//				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_90 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][ix][iy] = iMdlIdx;

//				n_First_Tray_X_Num = ((vecEnableTest[0] % MPOS_DIVISION) / y);
//				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
			else if( st_basic.n_degree == EDEG_180 )
			{
				iy = iMdlIdx / x;
				ix = iMdlIdx % x;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;

//				n_First_Tray_Y_Num = ((vecEnableTest[0] % MPOS_DIVISION) / x);
//				n_First_Tray_X_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % x);
			}
			else if( st_basic.n_degree == EDEG_270 )
			{
				ix = iMdlIdx / y;
				iy = iMdlIdx % y;
				nBinMap[ePosNew-1][(x-1)-ix][iy] = iMdlIdx;
				
//				n_First_Tray_X_Num = (y-1)-((vecEnableTest[0] % MPOS_DIVISION) / y);
//				n_First_Tray_Y_Num = (x-1)-((vecEnableTest[0] % MPOS_DIVISION) % y);
			}
		}
		
	}

	n_First_Bin = vecEnableTest[0] / MPOS_DIVISION - 1;

	bool a = false, b = false;
	for( int j=0; j<col; j++ ){
		for ( int k=0; k<row; k++ )	{
			if (nBinMap[n_First_Bin][j][k] > -1){
				if(j == 0 && a == false)//2,3피커
				{
					n_First_Tray_X_Num1 = j;
					n_First_Tray_Y_Num1 = k;
					a = true;
				}
				if(j == 1 && b == false)//1,2피커
				{
					n_First_Tray_X_Num2 = j;
					n_First_Tray_Y_Num2 = k;
					b = true;
				}
				if(a&&b)
				{
					i = 4;
					j = col+1;
				}
			}
		}
	}
}

int APartHandler::BinNum25CRforTest( std::vector<int> vecEmptyCell, int n_First_Bin)
{
	int i, ePosNew, iMdlIdx, ix, iy, x, y, col, row, nBinNum = 0;	
	
	x = col = st_basic.n_tray_x;
	y = row = st_basic.n_tray_y;
	
	if( (st_basic.n_degree == EDEG_90 || st_basic.n_degree == EDEG_270 ) )
	{
		y = row = st_basic.n_tray_x;
		x = col = st_basic.n_tray_y;
	}

	if(st_handler.mn_mirror_type == CTL_YES)
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	else
	{
		for( i = 0; i < vecEmptyCell.size(); i++ )
		{
			ePosNew = vecEmptyCell[i] / MPOS_DIVISION;
			iMdlIdx = vecEmptyCell[i] % MPOS_DIVISION;
			if(ePosNew > 3 || ePosNew <= 0) {
				ePosNew = 1;
			}
			
			if(n_First_Bin == ePosNew-1)
			{
				if( st_basic.n_degree == EDEG_0 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][ix][(y-1)-iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_90 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_180 )
				{
					iy = iMdlIdx/x;
					ix = iMdlIdx%x;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
				else if( st_basic.n_degree == EDEG_270 )
				{
					ix = iMdlIdx / y;
					iy = iMdlIdx % y;
					if(nEjectBinMap[ePosNew-1][(x-1)-ix][iy] > -1)
					{
						nBinNum++;
					}
				}
			}
		}
	}
	return nBinNum;	
}

// int APartHandler::GetPickTotalPos(std::vector<int> vecDVC, int nPicker[4], int nTotal[4], int nPickerCnt)
// {
// 	std::vector<int> vecPicker;
// 	int i,j;
// 	if(nPickerCnt == 2)
// 	{
// 		if(nPicker[0] == CTL_NO || nPicker[2] == CTL_NO)
// 		{
// 			for (i = 0; i <vecDVC.size(); i++)
// 			{
// 				for(j = 0;j < PICKER_NUM; j+=2;)
// 				{
// 					if(nPicker[j] == CTL_NO) {
// 						if(j == 0) { 
// 							if(nTotal[2] != vecDVC[i]) {
// 								nTotal[0] = vecDVC[i];
// 								vecPicker.push_back(0);
// 							}
// 						}
// 						else {
// 							if(nTotal[0] != vecDVC[i]) {
// 								nTotal[2] = vecDVC[i];
// 								vecPicker.push_back(2);
// 							}
// 							
// 						}
// 					}
// 					
// 				}
// 				if(vecPicker.size() >= 1) break;
// 			}
// 		}
// 	}
// 	else// if(nPickerCnt == 2)
// 	{
// 		if(nPicker[0] == CTL_NO || nPicker[1] == CTL_NO || nPicker[2] == CTL_NO || nPicker[3] == CTL_NO)
// 		{
// 			for (i = 0; i <vecDVC.size(); i++)
// 			{
// 				for(j = 0;j < PICKER_NUM; j++;)
// 				{
// 					if(nPicker[j] == CTL_NO) {
// 						if(j == 0) { 
// 							if(nTotal[1] != vecDVC[i] && nTotal[2] != vecDVC[i] && nTotal[3] !- vecDVC[i])
// 							{ nTotal[0] = vecDVC[i]; vecPicker.push_back(0); }
// 						}else if(j == 1) { 
// 							if(nTotal[0] != vecDVC[i] && nTotal[2] != vecDVC[i] && nTotal[3] !- vecDVC[i])
// 							{ nTotal[1] = vecDVC[i]; vecPicker.push_back(0); }
// 						}
// 						else if(j == 2) { 
// 							if(nTotal[1] != vecDVC[i]) { nTotal[] = vecDVC[i]; vecPicker.push_back(0); }
// 						}
// 						else if(j == 3) { 
// 							if(nTotal[1] != vecDVC[i]) { nTotal[] = vecDVC[i]; vecPicker.push_back(0); }
// 						}
// 					}
// 					
// 				}
// 				if(vecPicker.size() >= 1) break;
// 			}
// 		}
// 	}
// }













