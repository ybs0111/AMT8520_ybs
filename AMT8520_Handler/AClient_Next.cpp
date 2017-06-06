// AClient_Next.cpp: implementation of the AClient_Next class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AClient_Next.h"
#include "AMTLotManager.h"
#include "SrcPart/APartTray.h"
#include "SrcPart/APartHandler.h"
#include "Srcbase/ALocalization.h"

#include "AClient_Xgem.h"
#include "Public_Function.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AClient_Next g_client_next;

AClient_Next::AClient_Next()
{
	m_dwTime_NewLotIn_Send = -1;
	m_dwTime_Buffer_In_Send = -1;
	m_dwTime_Buffer_Ready_Send = -1;

	m_bBufferIn = false;
	m_bBufferReady = false;
	m_bBufferEmpty = false;
	m_strLotid = _T("");

	m_nCnt_NewLotIn = 0;
	m_nCnt_BufferIn = 0;
	m_nCnt_RecipeIfo = 0;
}

AClient_Next::~AClient_Next()
{

}

void AClient_Next::Run_Move()
{
	ANetwork_Client::Run_Move();

	if( m_bUsable == false )
		return;

	Run_Move_NewLotIn();
	Run_Move_BufferIn();
	Run_Move_BufferReady();
	Run_Move_RecipeInfo();
}

void AClient_Next::Run_Move_NewLotIn()
{
	if( m_strLotID_NewLotIn == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_NewLotIn_Send < 0)
	{
		m_dwTime_NewLotIn_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_NewLotIn_Send + 5000 )
		return;
	
	if( m_nCnt_NewLotIn >= 3 )
	{
		m_nCnt_NewLotIn = 0;
		
		// 5번 동안 응답이 없었습니다.
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : New_Lot_In 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : New_Lot_In 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);

		return;
	}

	CString strSend;
	if(st_handler.mn_fullinline_mode == 1)
	{
		strSend.Format( "FUNCTION=NEW_LOT_IN EQPID=%s LOTID=%s PARTID=%s PROCESS_ID=%s", 
			st_lamp.mstr_equip_id, m_strLotID_NewLotIn, g_lotMgr.GetLotByLotID( m_strLotID_NewLotIn ).GetPartID(),m_strProcessID);
	}
	else
	{
		strSend.Format( "FUNCTION=NEW_LOT_IN EQP_ID=%s LOTID=%s PARTID=%s", st_lamp.mstr_equip_id, 
			m_strLotID_NewLotIn, g_lotMgr.GetLotByLotID(m_strLotID_NewLotIn).GetPartID() );
	}

	
	PushSendMsg( (LPCSTR)strSend );
	m_dwTime_NewLotIn_Send = GetCurrentTime();
	m_nCnt_NewLotIn++;
}

void AClient_Next::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )	return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION_RPY" );
	
	if( strFunction == "NEW_LOT_IN" )	OnReceived_NewLotIn( strRecv.c_str() );
	if( strFunction == "BUFFER_IN" )	OnReceived_BufferIn( strRecv.c_str() );
	if( strFunction == "BUFFER_READY" )	OnReceived_BufferReady( strRecv.c_str() );
	if( strFunction == "RECIPE_INFO" )	OnReceived_RecipeInfo( strRecv.c_str() );
}

void AClient_Next::OnReceived_NewLotIn( CString strRecv )
{
	m_nCnt_NewLotIn = 0;
	CString strEqpID = OnNetworkBodyAnalysis( strRecv, "EQPID" );
	CString strLotID = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	CString strPartID = OnNetworkBodyAnalysis( strRecv, "PARTID" );
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strProcessID = OnNetworkBodyAnalysis( strRecv, "PROCESS_ID" );

	
	if( strResult != "PASS")
		return;

	if(st_basic.mn_mode_xgem == CTL_YES)
	{
		//2017.0602
		if(st_handler.mn_xgem_mode == CTL_YES)
		{
			g_client_xgem.SetLotStartStatus(strLotID, strPartID, 100);
		}
		else
		{
			g_client_xgem.SetLotStartStatus(strLotID, strPartID);
		}

		if(g_lotMgr.GetLotCount() == 1)
 			Func.OnSavePPID();
	}
	
	if( strLotID == m_strLotID_NewLotIn )
	{
		m_strLotID_NewLotIn = "";
		m_dwTime_NewLotIn_Send = -1;
	}
}

void AClient_Next::Run_Move_BufferIn()
{
	if( m_bBufferIn == false )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_Buffer_In_Send < 0)
	{
		m_dwTime_Buffer_In_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_Buffer_In_Send + 5000 )
		return;

	int iIdx = g_Tray.GetIdxByPos( TPOS_CONV3_TO_NEXT );
	if( iIdx < 0 )
		return;

	if( m_nCnt_BufferIn >= 3 )
	{
		m_nCnt_BufferIn = 0;
		
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : Buffer Lot 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : Buffer Lot 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		
		return;
	}

	ATray tray = g_Tray.GetTray( iIdx );
	CString strLastModule = "NO";
	CString strRProtyModule = "NO";//2013,0910
	if( g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetStrLastModule() == "YES" &&
		g_Tray.CalcLastTray( iIdx, tray.GetStrLotID() ) )
	{
		strLastModule = "YES";
		if( g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetStrRProtyModule() == "YES")//2013,0910
			strRProtyModule = "YES";
	}

	CString strEmptyTray = "NO";
	if( tray.GetEmptyTray() )
		strEmptyTray = "YES";

	CString strBypass = "NO";
	if( tray.GetBypass() )
		strBypass = "YES";

	int nCnt = st_basic.n_tray_x * st_basic.n_tray_y;
	CString strBufferIn;
	//2013,0311
	if(strEmptyTray == "YES")
	{
		if(tray.GetModuleCnt() > 0) strEmptyTray = "NO";
		//2013,1222
// 		else                        strLastModule = "NO";
// 		else if(strLastModule != "YES") strLastModule = "NO";
	}
// 	strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s BUFFER_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s BYPASS=%s POCKET_CNT=%d ", 
// 		st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),
// 		tray.GetBufferNo(), strLastModule, strEmptyTray, strBypass, tray.GetModuleCnt() );
	//2013,0715
	if(st_handler.mn_fullinline_mode == 1)
	{
//		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s PROCESS_ID=%s TRAY_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s BYPASS=%s POCKET_CNT=%d ", 
//			st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),tray.GetProcessID(),
//			tray.GetBufferNo(), strLastModule, strEmptyTray, strBypass, tray.GetModuleCnt() );
//		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s PROCESS_ID=%s TRAY_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s R_PROPERTY_END=%s BYPASS=%s POCKET_CNT=%d ", 
//			st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),tray.GetProcessID(),
//			tray.GetBufferNo(), strLastModule, strEmptyTray, tray.GetPropertyEnd(), strBypass, tray.GetModuleCnt() );
		//2013,0910
		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s PROCESS_ID=%s TRAY_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s R_PROPERTY_END=%s BYPASS=%s POCKET_CNT=%d ", 
			st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetStrProcessidModule()/*tray.GetProcessID()*/,
			tray.GetBufferNo(), strLastModule, strEmptyTray, strRProtyModule/*tray.GetPropertyEnd()*/, strBypass, tray.GetModuleCnt() );

	}
	else
	{
//		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s BUFFER_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s BYPASS=%s POCKET_CNT=%d ", 
//			st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),
//			tray.GetBufferNo(), strLastModule, strEmptyTray, strBypass, tray.GetModuleCnt() );
		strBufferIn.Format( "FUNCTION=BUFFER_IN EQPID=%s LOTID=%s PARTID=%s BUFFER_NO=%02d LAST_MDL=%s EMPTY_TRAY=%s BYPASS=%s POCKET_CNT=%d ", 
			st_lamp.mstr_equip_id, tray.GetStrLotID(), g_lotMgr.GetLotByLotID( tray.GetStrLotID() ).GetPartID(),
			tray.GetBufferNo(), strLastModule, strEmptyTray, strBypass, tray.GetModuleCnt() );
	}

	for( int i=1; i<=nCnt; i++ )
	{
		CString strTemp;
		AModule mdl = tray.GetModule(i - 1);
// 		strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s BIN=%s)", i, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
// 			mdl.GetWWN(), mdl.GetBin() );

		//2015.1123
// 		if( st_basic.n_cserial2_mode == CTL_YES )//8S=%s
// 		{
// 			strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s 8S=%s, PSID=%s BIN=%s)", i, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
// 				mdl.GetWWN(), mdl.GetCSN(), mdl.GetCSN2(), mdl.GetPSID(), mdl.GetBin() );
// 			strBufferIn += strTemp;
// 		}
// 		else
// 		{
// 			strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s PSID=%s BIN=%s)", i, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
// 				mdl.GetWWN(), mdl.GetCSN(), mdl.GetPSID(), mdl.GetBin() );
// 			strBufferIn += strTemp;
// 		}
		strTemp.Format( " POCKET_%02d=(ARRAYSN=%s SERIAL=%s PPID=%s WWN=%s C_SERIAL=%s 8S=%s PSID=%s BIN=%s)", i, mdl.GetSN(), mdl.GetSerial(), mdl.GetPPID(),
			mdl.GetWWN(), mdl.GetCSN(), mdl.GetCSN2(), mdl.GetPSID(), mdl.GetBin() );
		strBufferIn += strTemp;

	}

	//2013,1027
	st_xgem.m_str_bufferin = "";
	st_xgem.m_str_bufferin.Format("%s", (LPCSTR)strBufferIn);
	
	PushSendMsg( (LPCSTR)strBufferIn );
	m_dwTime_Buffer_In_Send = GetCurrentTime();
	m_nCnt_BufferIn++;
}

void AClient_Next::OnReceived_BufferIn( CString strRecv )
{
	m_nCnt_BufferIn = 0;
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );

	if( strResult != "PASS")
		return;

	m_bBufferIn = false;
	m_dwTime_Buffer_In_Send = -1;
}

void AClient_Next::OnReceived_RecipeInfo( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
// 	if( strResult == "FAIL") //"OK"
// 		return;

// 	if( strLotID == m_strLotID_NewLotIn )
// 	{
// 		m_strLotID_NewLotIn = "";
// 		m_dwTime_NewLotIn_Send = -1;
// 	}

	m_strRecipeInfoID = "";
	m_dwTime_RecipeInfo_Send = -1;
	m_nCnt_RecipeIfo = 0;
}

void AClient_Next::Run_Move_BufferReady()
{
	// 	if( m_bBufferReady == false )
	// 		return;
	// 
	// 	if( GetCurrentTime() <= m_dwTime_Buffer_Ready_Send + 5000 )
	// 		return;
	// 
	// 
	// 
	// 	CString strBufferReady = "FUNCTION=BUFFER_READY REASON=SEND_TO_NEXT";
	// 
	// 	PushSendMsg( (LPCSTR)strBufferReady );
	// 	m_dwTime_Buffer_Ready_Send = GetCurrentTime();
	
	if( m_bBufferReady == false )
		return;
	
	//2012,1220
	if(GetCurrentTime() - m_dwTime_Buffer_Ready_Send < 0)
	{
		m_dwTime_Buffer_Ready_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_Buffer_Ready_Send + 5000 )
		return;
	
	//ybs
	CString strBufferReady;
	if(m_bBufferEmpty == true)
	{
		strBufferReady.Format("FUNCTION=BUFFER_READY REASON=SEND_TO_NEXT EMPTY_TRAY=YES LOTID=%s",m_strLotid);
	}
	else
	{
		strBufferReady.Format("FUNCTION=BUFFER_READY REASON=SEND_TO_NEXT EMPTY_TRAY=NO LOTID=%s",m_strLotid);
	}
	
	PushSendMsg( (LPCSTR)strBufferReady );
	m_dwTime_Buffer_Ready_Send = GetCurrentTime();
}

void AClient_Next::OnReceived_BufferReady( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );

	if( strResult != "PASS")
		return;

	m_bBufferReady = false;
	m_dwTime_Buffer_Ready_Send = -1;
}

void AClient_Next::SendTrayInfoReq()
{
	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		PushSendMsg( "FUNCTION=TRAY_INFO_REQ" );
	}
}

void AClient_Next::SendRecvStateReq()
{
	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		PushSendMsg( "FUNCTION=RECV_STATE_REQ" );
	}
}

//2016.0129
void AClient_Next::Run_Move_RecipeInfo()
{
	if( m_strRecipeInfoID == "" )
		return;
	
	if(GetCurrentTime() - m_dwTime_RecipeInfo_Send < 0)
	{
		m_dwTime_NewLotIn_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_RecipeInfo_Send + 5000 )
		return;
	
	if( m_nCnt_RecipeIfo >= 3 )
	{
		m_nCnt_RecipeIfo = 0;
		
		// 5번 동안 응답이 없었습니다.
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : Recipe Info 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : Recipe Info 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		
		return;
	}
	
	CString strSend;
	strSend.Format( "FUNCTION=RECIPE_INFO PARTID=%s ", m_strRecipeInfoID);
	
	PushSendMsg( (LPCSTR)strSend );
	
	m_dwTime_RecipeInfo_Send = GetCurrentTime();
	m_nCnt_RecipeIfo++;
}