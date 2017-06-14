// AClient_Router.cpp: implementation of the AClient_Router class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AClient_Router.h"
#include "Srcbase/ALocalization.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AClient_Router g_client_router;

AClient_Router::AClient_Router()
{

}

AClient_Router::~AClient_Router()
{

}

void AClient_Router::Run_Move()
{
	ANetwork_Client::Run_Move();

	if( m_bUsable == false )
		return;

	Run_Move_TrayReq();
}

void AClient_Router::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )
		return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION_RPY" );
	
	if( strFunction == "TRAY_REQ" )				OnReceived_TrayReq( strRecv.c_str() );
}

void AClient_Router::Run_Move_TrayReq()
{
	if( m_strLotID_TrayReq == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_TrayReq < 0)
	{
		m_dwTime_TrayReq = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TrayReq < 5000 )
		return;

	if( m_nCnt_TrayReq >= 3 )
	{
		m_nCnt_TrayReq = 0;
		
		// 5번 동안 응답이 없었습니다.
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Tray Req 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Tray Req 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);

		return;
	}

	int nReqCnt = 1;
	if( st_basic.n_tray_x * st_basic.n_tray_y > 10 )
		nReqCnt = 2;

	CString strSend;
	strSend.Format( "FUNCTION=TRAY_REQ EQP_ID=%s LOTID=%s TRAY_CNT=%d", st_lamp.mstr_equip_id, 
		m_strLotID_TrayReq, nReqCnt );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_TrayReq = GetCurrentTime();
	m_nCnt_TrayReq++;
}

void AClient_Router::OnReceived_TrayReq( CString strRecv )
{
	m_nCnt_TrayReq = 0;

	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strLotID = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	CString strEqpID = OnNetworkBodyAnalysis( strRecv, "EQPID" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );

	if( strResult == "PASS" )
	{
		m_strLotID_TrayReq = "";
		m_dwTime_TrayReq = -1;
	}
}
