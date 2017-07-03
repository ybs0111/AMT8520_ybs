// AClient_BPC.cpp: implementation of the AClient_BPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AClient_BPC.h"
#include "AMTLotManager.h"
#include "SrcPart\PartFunction.h"
#include "SrcPart\APartTestSite.h"
#include "SrcPart\APartHandler.h"
#include "Variable.h"
#include "Srcbase\ALocalization.h"
#include "AMTRegistry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AClient_BPC g_client_bpc;

AClient_BPC::AClient_BPC()
{
	m_strLotID_NewLotIn = "";
	m_strLotID_OnlineEnd = "";
	m_strLotID_LotCancel = "";

	for ( int i  = 0; i < TSITE_COK_CNT; i++ )
	{
		m_bSocketOnOff[i] = false;
	}

	ClearRecvWaitCnt();
	
}

AClient_BPC::~AClient_BPC()
{

}

void AClient_BPC::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )
		return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION_RPY" );
	
	if( strFunction == "NEW_LOT_IN" )				OnReceived_NewLotIn( strRecv.c_str() );
	else if( strFunction == "ONLINE_END" )			OnReceived_OnlineEnd( strRecv.c_str() );
	else if( strFunction == "LOT_CANCEL" )			OnReceived_LotCancel( strRecv.c_str() );
	else if( strFunction == "SLOT_READY" )			OnReceived_SlotReady( strRecv.c_str() );
	else if( strFunction == "TEST_START" )			OnReceived_TestStart( strRecv.c_str() );
	else if( strFunction == "TEST_ABORT" )			OnReceived_TestAbort( strRecv.c_str() );
	else if( strFunction == "SLOT_POWER_OFF" )		OnReceived_SlotPowerOff( strRecv.c_str() );
	else if( strFunction == "SLOT_STATUS_CHANGE" )	OnReceived_SlotStatusChange( strRecv.c_str() );
	else if( strFunction == "SLOT_QUERY" )			OnReceived_SlotQuery( strRecv.c_str() );
	else if( strFunction == "TEST_MODE" )			OnReceived_TestModeQuery( strRecv.c_str() );

}

void AClient_BPC::Run_Move()
{
	ANetwork_Client::Run_Move();

	if( m_bUsable == false )
		return;

	Run_Move_NewLotIn();
	Run_Move_OnlineEnd();
	Run_Move_LotCancel();
	Run_Move_SlotReady();
	Run_Move_TestStart();
	Run_Move_TestAbort();
	Run_Move_SlotPowerOff();
	Run_Move_SlotStatusChange();
	Run_Move_TestMode();
	Run_Move_SocketOnOff();
}

void AClient_BPC::Run_Move_NewLotIn()
{
	if( m_strLotID_NewLotIn == "" )
		return;

	if( g_lotMgr.GetLotByLotID( m_strLotID_NewLotIn ).GetNewLotInPass() )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_NewLotIn < 0)
	{
		m_dwTime_NewLotIn = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_NewLotIn < 5000 )
		return;
	
	if( m_nCnt_NewLotIn >= 3 )
	{
		ClearRecvWaitCnt();
		
		// 5번 동안 응답이 없었습니다.
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Tester : New Lot In 3회 무응답" );
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Tester : New Lot In 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		
		return;
	}

	CString strSend;
	strSend.Format( "FUNCTION=NEW_LOT_IN EQP_ID=%s LOTID=%s PARTID=%s OPERID=AUTO", st_lamp.mstr_equip_id, 
		m_strLotID_NewLotIn, g_lotMgr.GetLotByLotID(m_strLotID_NewLotIn).GetPartID() );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_NewLotIn = GetCurrentTime();
	m_nCnt_NewLotIn++;
}

void AClient_BPC::OnReceived_NewLotIn( CString strRecv )
{
	ClearRecvWaitCnt();

	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );


	if( strResult != "PASS" && 
		strResult != "OK" )
		return;

	g_lotMgr.GetLotByLotID( m_strLotID_NewLotIn ).SetNewLotInPass( true );

	m_strLotID_NewLotIn = "";
	m_dwTime_NewLotIn = -1;

	if( st_handler.cwnd_data_lot )
		st_handler.cwnd_data_lot->PostMessage( WM_DRAW_DATA_LOT );
}

void AClient_BPC::Run_Move_OnlineEnd()
{
	if( m_strLotID_OnlineEnd == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_OnlineEnd < 0)
	{
		m_dwTime_OnlineEnd = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_OnlineEnd < 5000 )
		return;

	CString strSend;
	strSend.Format( "FUNCTION=ONLINE_END EQP_ID=%s LOTID=%s PARTID=%s", st_lamp.mstr_equip_id, 
		m_strLotID_OnlineEnd, g_lotMgr.GetLotByLotID(m_strLotID_OnlineEnd).GetPartID() );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_OnlineEnd = GetCurrentTime();
}

void AClient_BPC::OnReceived_OnlineEnd( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );	
	if( strResult == "OK" )
	{
		CString strLotQty = OnNetworkBodyAnalysis( strRecv, "LOTQTY" );	
		CString strBinSum = OnNetworkBodyAnalysis( strRecv, "BINSUM", NET_OPT_BRACKET );	

		// 랏삭제.
		if( g_lotMgr.GetLotCount() > 0 &&
			g_lotMgr.GetLotIDAt(0) == m_strLotID_OnlineEnd )
		{
			g_lotMgr.DeleteLot( 0 );
			
			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOTINFO, 0);

			g_handler.CalcTrayMoveNext_fromRejFull();
			g_handler.CalcTrayMoveNext_fromBin3();
			g_handler.CalcTrayMoveNext_fromReject();
			
			g_handler.CalcTrayMoveNext_fromBin();
		}

		m_strLotID_OnlineEnd = "";
		m_dwTime_OnlineEnd = -1;
	}
	else
	{
		CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );	
	}
}

void AClient_BPC::Run_Move_LotCancel()
{
	if( m_strLotID_LotCancel == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_LotCancel < 0)
	{
		m_dwTime_LotCancel = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_LotCancel < 5000 )
		return;

	CString strSend;
	strSend.Format( "FUNCTION=LOT_CANCEL EQP_ID=%s LOTID=%s PARTID=%s", st_lamp.mstr_equip_id, 
		m_strLotID_LotCancel, g_lotMgr.GetLotByLotID(m_strLotID_LotCancel).GetPartID() );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_LotCancel = GetCurrentTime();
}

void AClient_BPC::OnReceived_LotCancel( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	if( strResult == "OK" )
	{
		CString strLotQty = OnNetworkBodyAnalysis( strRecv, "LOTQTY" );

		m_strLotID_LotCancel = "";
		m_dwTime_LotCancel = -1;
	}
	else
	{
		CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	}
}

void AClient_BPC::Run_Move_SlotReady()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( g_site.GetModule(i).GetModuleState() != EMS_READY )
			continue;

		//2012,1220
		if(GetCurrentTime() - m_dwTime_SlotReady[i] < 0)
		{
			m_dwTime_SlotReady[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_SlotReady[i] < 10000 )
			continue;

		if( m_nRecvWaitCnt_SlotReady[i] >= 3 )
		{
			ClearRecvWaitCnt();

			// 5번 동안 응답이 없었습니다.
			m_bUsable = false;
			
			CString strErr;
			strErr.Format( "Slot Ready[%d] 3회 무응답", i + 1);
			if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Slot Ready[%d] 3count non-response", i + 1);
			st_msg.mstr_event_msg[0] = strErr;
			::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		}

		AModule module = g_site.GetModule(i);
		if( module.IsExist() == false )
			continue;

		CString strSend;
// 		strSend.Format( "FUNCTION=SLOT_READY EQP_ID=%s LOTID=%s PARTID=%s SLOT=%d ARRAY_SN=%s SERIAL=%s PPID=%s WWN=%s", st_lamp.mstr_equip_id, 
// 			module.GetLotNo(), g_lotMgr.GetLotByLotID(module.GetLotNo()).GetPartID(),
// 			i + 1, module.GetSN(), module.GetSerial(), module.GetPPID(), module.GetWWN() );
		//2015.1123

// 		if( st_basic.n_cserial2_mode == CTL_YES )//8S=%s
		if( module.GetCSN() != "" && module.GetCSN2() != "" )
		{
			strSend.Format( "FUNCTION=SLOT_READY EQP_ID=%s LOTID=%s PARTID=%s SLOT=%d ARRAY_SN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s C_SERIAL2=%s PSID=%s", st_lamp.mstr_equip_id, 
				module.GetLotNo(), g_lotMgr.GetLotByLotID(module.GetLotNo()).GetPartID(),
				i + 1, module.GetSN(), module.GetSerial(), module.GetPPID(), module.GetWWN(), module.GetCSN(), module.GetCSN2(), module.GetPSID() );
		}
		else if( module.GetCSN() == "" && module.GetCSN2() != "" )
		{
			strSend.Format( "FUNCTION=SLOT_READY EQP_ID=%s LOTID=%s PARTID=%s SLOT=%d ARRAY_SN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s C_SERIAL2= PSID=%s", st_lamp.mstr_equip_id, 
				module.GetLotNo(), g_lotMgr.GetLotByLotID(module.GetLotNo()).GetPartID(),
				i + 1, module.GetSN(), module.GetSerial(), module.GetPPID(), module.GetWWN(), module.GetCSN2(), module.GetPSID() );//8s를 serial로 보냄
		}
		else
		{
			strSend.Format( "FUNCTION=SLOT_READY EQP_ID=%s LOTID=%s PARTID=%s SLOT=%d ARRAY_SN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s C_SERIAL2= PSID=%s", st_lamp.mstr_equip_id, 
				module.GetLotNo(), g_lotMgr.GetLotByLotID(module.GetLotNo()).GetPartID(),
				i + 1, module.GetSN(), module.GetSerial(), module.GetPPID(), module.GetWWN(), module.GetCSN(), module.GetPSID() );
			
		}


		PushSendMsg( (LPCSTR)strSend, 0 );
		m_dwTime_SlotReady[i] = GetCurrentTime();
		m_nRecvWaitCnt_SlotReady[i]++;
	}
}

void AClient_BPC::OnReceived_SlotReady( CString strRecv )
{
	ClearRecvWaitCnt();
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strSlot = OnNetworkBodyAnalysis( strRecv, "SLOT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );//2016.0711
	int nSlot = atoi( strSlot ) - 1;
//	if( nSlot < 0 )
	if( nSlot < 0 || nSlot >= TSITE_COK_CNT)
		return;

	if( strResult == "OK" )
	{
		g_site.GetModule(nSlot).SetModuleState( EMS_READY_RECV );
		m_dwTime_SlotReady[nSlot] = -1;
	}
	
	if( LPCTSTR(strMsg) != NULL && strMsg.GetLength() > 5 )//2016.0711
	{
		strMsg.MakeUpper();
		strMsg.TrimLeft();
		strMsg.TrimRight();
		
		if( st_basic.mn_hotfix_pos < 1 ) st_basic.mn_hotfix_pos = 1;
		if( strMsg == "DPS CHECK FAIL" )
		{
			if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
			{
				sprintf(st_msg.c_normal_msg, "BPC)%s", strMsg);
				st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, NORMAL_MSG);  // 동작 실패 출력 요청
			}
			g_site.GetModule(nSlot).SetModuleState(EMS_ABORT);
			g_client_bpc.SetSocketOnOff(nSlot );
			g_site.SetEnableSocket( nSlot, -3 );

			CString strVal, m_strPrev;
			strVal.Format( "%s_%d_%2d", REG_VAL_SOCKET_TS, st_basic.mn_hotfix_pos-1, nSlot );
			m_strPrev = AMTRegistry::RegReadString(REG_KEY_SOCKET_CAUSE, strVal, "").c_str();

			strVal.Format( "%s_%d_%2d", REG_VAL_SOCKET_TS, st_basic.mn_hotfix_pos-1, nSlot );
			m_strPrev = AMTRegistry::RegWriteString(REG_KEY_SOCKET_CAUSE, strMsg, m_strPrev);

		}
	}	
}



void AClient_BPC::Run_Move_TestStart()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( g_site.GetModule(i).GetModuleState() != EMS_START )
			continue;

		//2012,1220
		if(GetCurrentTime() - m_dwTime_TestStart[i] < 0)
		{
			m_dwTime_TestStart[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_TestStart[i] < 5000 )
			continue;

		if( m_nRecvWaitCnt_TestStart[i] >= 3 )
		{
			ClearRecvWaitCnt();

			// 5번 동안 응답이 없었습니다.
			m_bUsable = false;
			
			CString strErr;
			strErr.Format( "Test Start[%d] 3회 무응답", i + 1);
			if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Test Start[%d] 3count non-response", i + 1);
			st_msg.mstr_event_msg[0] = strErr;
			::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		}

		AModule module = g_site.GetModule(i);
		if( module.IsExist() == false )
			continue;

		CString strSend;
		strSend.Format( "FUNCTION=TEST_START EQP_ID=%s LOTID=%s TEST_COUNT=%d SLOT=%d SERIAL=%s", st_lamp.mstr_equip_id, module.GetLotNo(),
			module.GetTestCnt(), i + 1, module.GetSerial() );
		
		PushSendMsg( (LPCSTR)strSend, 0 );
		m_dwTime_TestStart[i] = GetCurrentTime();
		m_nRecvWaitCnt_TestStart[i]++;
		st_handler.isTestRun[i] = CTL_YES; //2015.0914
	}
}

void AClient_BPC::OnReceived_TestStart( CString strRecv )
{
	ClearRecvWaitCnt();
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strSlot = OnNetworkBodyAnalysis( strRecv, "SLOT" );
	int nSlot = atoi( strSlot ) - 1;

	if( nSlot < 0 || nSlot >= TSITE_COK_CNT)
		return;


	if( strResult == "OK" )
	{
		m_dwTime_TestStart[nSlot] = -1;


		Run_Test_Site[nSlot / 10].m_dwTimeStartTest[nSlot % 10] = GetCurrentTime();
		g_site.GetModule(nSlot).SetModuleState( EMS_TEST );

		if( st_handler.cwnd_main )
		{
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
		}
	}
	else
	{
		CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	}
}

void AClient_BPC::Run_Move_TestAbort()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( g_site.GetModule(i).GetModuleState() != EMS_ABORT )
			continue;

		//2012,1220
		if(GetCurrentTime() - m_dwTime_TestAbort[i] < 0)
		{
			m_dwTime_TestAbort[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_TestAbort[i] < 5000 )
			continue;

		if( m_nRecvWaitCnt_TestAbort[i] >= 3 )
		{
			ClearRecvWaitCnt();

			// 5번 동안 응답이 없었습니다.
			m_bUsable = false;
			
			CString strErr;
			strErr.Format( "Test Abort[%d] 3회 무응답", i + 1);
			if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Test Abort[%d] 3count non-response", i + 1);
			st_msg.mstr_event_msg[0] = strErr;
			::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		}

		AModule module = g_site.GetModule(i);
		if( module.IsExist() == false )
			continue;

		CString strSend;
		strSend.Format( "FUNCTION=TEST_ABORT LOTID=%s SLOT=%d, SERIAL=%s", module.GetLotNo(), i + 1, module.GetSerial() );

		PushSendMsg( (LPCSTR)strSend, 0 );
		m_dwTime_TestAbort[i] = GetCurrentTime();
		m_nRecvWaitCnt_TestAbort[i]++;
	}
}

//Socket On -> FUNCTION=SOCKET CONDITION=ON SLOT=12
//Socket Off -> FUNCTION=SOCKET CONDITION=OFF SLOT=12
void AClient_BPC::Run_Move_SocketOnOff()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( m_bSocketOnOff[i] != true )
			continue;

		if(GetCurrentTime() - m_dwTime_SockOnOff[i] < 0)
		{
			m_dwTime_SockOnOff[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_SockOnOff[i] < 500 )
			continue;

		CString strSend,strOnOff;
		strOnOff = "ON";
		//if(g_site.GetEnableSocket(i) == false) strOnOff = "OFF";
		//2016.0709
		if(g_site.GetEnableSocket(i) < 1) strOnOff = "OFF";
		strSend.Format( "FUNCTION=SOCKET CONDITION=%s SLOT=%d", strOnOff, i + 1 );
		PushSendMsg( (LPCSTR)strSend, 0 );

		m_bSocketOnOff[i] = false;
		m_dwTime_SockOnOff[i] = GetCurrentTime();
	}

}

void AClient_BPC::OnReceived_TestAbort( CString strRecv )
{
	ClearRecvWaitCnt();
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strSlot = OnNetworkBodyAnalysis( strRecv, "SLOT" );
	int nSlot = atoi( strSlot ) - 1;

	if( nSlot < 0 || nSlot >= TSITE_COK_CNT)
		return;

	if( strResult == "OK" )
	{
		g_site.GetModule(nSlot).SetModuleState( EMS_ABORT_RECV );
		m_dwTime_TestAbort[nSlot] = -1;
		
		g_site.GetModule(nSlot).SetBin("8");
		g_site.GetModule(nSlot).SetTestedPos(nSlot);
	}
	else
	{
		CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	}
}

void AClient_BPC::Run_Move_SlotPowerOff()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( m_bSlotPowerOff[i] == false )
			continue;

		//2012,1220
		if(GetCurrentTime() - m_dwTime_SlotPowerOff[i] < 0)
		{
			m_dwTime_SlotPowerOff[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_SlotPowerOff[i] < 5000 )
			continue;

		CString strSend;
		strSend.Format( "FUNCTION=SLOT_POWER_OFF EQP_ID=%s SLOT=%d", st_lamp.mstr_equip_id, i + 1 );

		PushSendMsg( (LPCSTR)strSend, 0 );
		m_dwTime_SlotPowerOff[i] = GetCurrentTime();
	}
}

void AClient_BPC::OnReceived_SlotPowerOff( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strSlot = OnNetworkBodyAnalysis( strRecv, "SLOT" );
	int nSlot = atoi( strSlot ) - 1;
	if( nSlot < 0 || nSlot >= TSITE_COK_CNT)
		return;

	if( strResult == "OK" )
	{
		m_bSlotPowerOff[nSlot] = false;
		m_dwTime_SlotPowerOff[nSlot] = -1;
	}
}

void AClient_BPC::Run_Move_SlotStatusChange()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		if( m_useSlotStatusChange[i] == EUSE_NONE )
			continue;

		//2012,1220
		if(GetCurrentTime() - m_dwTime_SlotStatusChange[i] < 0)
		{
			m_dwTime_SlotStatusChange[i] = GetCurrentTime();
		}
		if( GetCurrentTime() - m_dwTime_SlotStatusChange[i] < 5000 )
			continue;

		CString strUse = "DISABLE";
		if( m_useSlotStatusChange[i] == EUSE_ENABLE )
			strUse = "ENABLE";

		CString strSend;
		strSend.Format( "FUNCTION=SLOT_STATUS_CHANGE EQP_ID=%s SLOT=%d STATUS=%s", st_lamp.mstr_equip_id, i + 1, strUse );
		PushSendMsg( (LPCSTR)strSend, 0 );
		m_dwTime_SlotStatusChange[i] = GetCurrentTime();
	}
}

void AClient_BPC::OnReceived_SlotStatusChange( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strSlot = OnNetworkBodyAnalysis( strRecv, "SLOT" );
	int nSlot = atoi( strSlot ) - 1;
	if( nSlot < 0 || nSlot >= TSITE_COK_CNT)
		return;

	if( strResult == "OK" )
	{
		bool bVal = false;
		if( m_useSlotStatusChange[nSlot] == EUSE_ENABLE )
			bVal = true;
		g_site.SetEnableSocket( nSlot, bVal );
		if( st_handler.cwnd_data_testsite )
			st_handler.cwnd_data_testsite->PostMessage( WM_DRAW_DATA_TESTSITE );

		m_useSlotStatusChange[nSlot] = EUSE_NONE;
		m_dwTime_SlotStatusChange[nSlot] = -1;
	}
}

void AClient_BPC::OnReceived_SlotQuery( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	if( strResult == "OK" )
	{
		CString strCnt = OnNetworkBodyAnalysis( strRecv, "SLOT_CNT" );
		CString strSlotInfo = OnNetworkBodyAnalysis( strRecv, "SLOT_INFO", NET_OPT_BRACKET );
		int nCnt = atoi(strCnt);
		for( int i=0; i<nCnt; i++ )
		{
			CString strFind;
			strFind.Format( "SLOT_%02d", i + 1 );
			CString strUse = OnNetworkBodyAnalysis( strSlotInfo, strFind );

			if( strUse == "ENABLE" )
			{
				g_site.SetEnableSocket( i, true );
			}
			else
			{
				g_site.SetEnableSocket( i, false );
			}

		}
		if( st_handler.cwnd_data_testsite )
			st_handler.cwnd_data_testsite->PostMessage( WM_DRAW_DATA_TESTSITE );
	}
}

void AClient_BPC::SendSlotQuery()
{
	CString strSend;
	strSend.Format( "FUNCTION=SLOT_QUERY EQP_ID=%s", st_lamp.mstr_equip_id );
	PushSendMsg( (LPCSTR)strSend, 0 );
}


void AClient_BPC::Run_Move_TestMode()
{
	if( m_str_TestMode == "" )
		return;
	
	if(GetCurrentTime() - m_dwTime_TestMode < 0)
	{
		m_dwTime_TestMode = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TestMode < 5000 )
		return;
	
	if( m_nCnt_TestMode >= 3 )
	{
		ClearRecvWaitCnt();		
		
		CString strErr;
		strErr.Format( "테스터에 HitFix=%d PCI모드 전환 요구 3회 무응답", st_basic.mn_hotfix_pos );
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Request HitFix=%d PCI mode change to Tester. 3count non-response", st_basic.mn_hotfix_pos );
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "%s", strErr);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
	}
	
	
	CString strSend;
	////2015.0518
	//strSend.Format("FUNCTION=TEST_MODE HIFIX=%d", st_basic.mn_hotfix_pos);
	strSend.Format("FUNCTION=TEST_MODE HIFIX=%d MODE=%s", st_basic.mn_hotfix_pos, st_basic.n_str_hifix_mode);
	////
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_TestMode = GetCurrentTime();
	m_nCnt_TestMode++;
}

//FUNCTION_RPY=TEST_MODE RESULT=OK HIFIX=2 MODE=PCIe
void AClient_BPC::OnReceived_TestModeQuery( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	if( strResult == "OK" || strResult == "FAIL")
	{
		CString strCnt = OnNetworkBodyAnalysis( strRecv, "HIFIX" );
		CString strSlotInfo = OnNetworkBodyAnalysis( strRecv, "MODE", NET_OPT_BRACKET );
		int nCnt = atoi(strCnt);
		m_str_TestMode = "";
		m_dwTime_TestMode = -1;
		m_nCnt_TestMode = 0;
		//2014,0310
		g_handler_cok.SetHifixPos( st_basic.mn_hotfix_pos );
	}
}

void AClient_BPC::ClearRecvWaitCnt()
{
	for( int i=0; i<TSITE_COK_CNT; i++ )
	{
		m_nRecvWaitCnt_SlotReady[i] = 0;
		m_nRecvWaitCnt_TestStart[i] = 0;
		m_nRecvWaitCnt_TestAbort[i] = 0;
	}

	m_nCnt_NewLotIn = 0;
	m_nCnt_TestMode = 0;//2013,0503
}


