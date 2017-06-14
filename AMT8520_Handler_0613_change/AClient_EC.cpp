// AClient_EC.cpp: implementation of the AClient_EC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AClient_EC.h"
#include "AMTLotManager.h"
#include "Srcbase\ALocalization.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AClient_EC g_client_ec;

AClient_EC::AClient_EC()
{
	m_nCnt_ToolInform = 0;
	m_nCnt_ReqEmpty = 0;
	m_bToolInform = false;
	m_bReqEmptyTray = false;

}

AClient_EC::~AClient_EC()
{

}

void AClient_EC::Run_Move()
{
	ANetwork_Client::Run_Move();

	Run_Move_TestEnd();
	Run_Move_BufferIn();
	Run_Move_AlarmReport();
	Run_Move_VersionUpdate();

	//EMPTY REQ
	Run_Move_ReqEmptyTray();
	//TOOL INFORM SEND
	Run_Move_SendTool();

}

void AClient_EC::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )
		return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION_RPY" );
	
	if( strFunction == "TEST_END" )				OnReceived_TestEnd( strRecv.c_str() );
	else if( strFunction == "BUFFER_IN" )		OnReceived_BufferIn( strRecv.c_str() );
	else if( strFunction == "ALARM_REPORT" )	OnReceived_AlarmReport( strRecv.c_str() );
	else if( strFunction == "VERSION_UPDATE" )	OnReceived_VersionUpdate( strRecv.c_str() );
	else if( strFunction == "EMPTY_CARRIER_MOVE_REQ" )	OnReceived_ReqEmptrTray( strRecv.c_str() );
	else if( strFunction == "TOOL_CHANGE" )		OnReceived_SendToolChange( strRecv.c_str() );

}

// Lot End
void AClient_EC::Run_Move_TestEnd()
{
	if( m_strLotID_TestEnd == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_TestEnd < 0)
	{
		m_dwTime_TestEnd = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_TestEnd < 5000 )
		return;

	CString strSend;
	strSend.Format( "FUNCTION=TEST_END EQPID=%s LOTID=%s PARTNO=%s", st_lamp.mstr_equip_id, 
		m_strLotID_TestEnd, m_strPartID_TestEnd );

	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_TestEnd = GetCurrentTime();
}

void AClient_EC::OnReceived_TestEnd( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );

	if( strResult == "FAIL" )
	{
		CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	}
	else if( strResult == "OK" || strResult == "PASS" )
	{
		CString strLotQty = OnNetworkBodyAnalysis( strRecv, "LOTQTY" );
		CString strBinSum = OnNetworkBodyAnalysis( strRecv, "BINSUM", NET_OPT_BRACKET );

		// 랏 삭제.
		if( g_lotMgr.GetLotCount() > 0 &&
			g_lotMgr.GetLotIDAt(0) == m_strLotID_TestEnd )
		{
			g_lotMgr.DeleteLot( 0 );
			
			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOTINFO, 0);
		}

		m_strLotID_TestEnd = "";
		m_strPartID_TestEnd = "";
		m_dwTime_TestEnd = -1;
	}
}

void AClient_EC::Run_Move_BufferIn()
{
	if( m_strBufferIn == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_BufferIn < 0)
	{
		m_dwTime_BufferIn = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_BufferIn < 5000 )
		return;

	PushSendMsg( (LPCSTR)m_strBufferIn, 0 );
	m_dwTime_BufferIn = GetCurrentTime();
}

void AClient_EC::OnReceived_BufferIn( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	if( strResult == "PASS" )
	{
		m_dwTime_BufferIn = -1;
		m_strBufferIn = "";
	}
}

void AClient_EC::Run_Move_AlarmReport()
{
	if( m_strAlarmReport == "" )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_AlarmReport < 0)
	{
		m_dwTime_AlarmReport = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_AlarmReport < 5000 )
		return;

	CString strSend;
	strSend.Format( "FUNCTION=ALARM_REPORT EQP=%s LINE=%s ALARM_CODE=%s ALARM_NAME=<%s>", st_lamp.mstr_equip_id, "A", "JAMCODE", "JAM_NAME" );

	PushSendMsg( (LPCSTR) strSend, 0 );
	m_dwTime_AlarmReport = GetCurrentTime();
}

void AClient_EC::OnReceived_AlarmReport( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );

	if( strResult == "PASS" )
	{
		m_dwTime_AlarmReport = -1;
		m_strAlarmReport = "";
	}
}

void AClient_EC::Run_Move_VersionUpdate()
{
	if( m_bVersionUpdate == false )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_VersionUpdate < 0)
	{
		m_dwTime_VersionUpdate = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_VersionUpdate < 5000 )
		return;

	CString strSend;
	//strSend.Format( "FUNCTION=VERSION_UPDATE EQPID=%s EQPMODEL=IFT DESC=\"%s\"", st_lamp.mstr_equip_id, g_ver.GetDate())

	//	strMsg.Format( "FUNCTION=VERSION_UPDATE EQP_ID=%s MODEL=\"SSD AUTO SORTER\" VERSION=\"%s_%s\" DESC=\"%s\"", st_lamp.mstr_equip_id, m_strVersion,
	//		m_strDate, m_strUpdate );
}

void AClient_EC::Run_Move_ReqEmptyTray()
{
	if( m_bReqEmptyTray == false )
		return;
	
	//2012,1220
	if(GetCurrentTime() - m_dwTime_ReqEmptyTray < 0)
	{
		m_dwTime_ReqEmptyTray = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_ReqEmptyTray < 60000 )
		return;
	
	
	if( m_nCnt_ReqEmpty >= 3 )
	{
		m_nCnt_ReqEmpty = 0;		
		
		CString strErr;
		strErr = "Empty 요청 요구 3회 무응답";
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr = "Request Empty 3 count non-response ";
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "%s", strErr);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
	}
	
	CString strSend;
	strSend.Format( "FUNCTION=EMPTY_CARRIER_MOVE_REQ EQPID=%s LOTID=EFTRAY PORTID=%s_I1 CARRIERID= ", st_lamp.mstr_empty_id, st_lamp.mstr_empty_id );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_ReqEmptyTray = GetCurrentTime();
	m_nCnt_ReqEmpty++;
}

//TOOL INFORM SEND
void AClient_EC::Run_Move_SendTool()
{
	if( m_bToolInform == false )
		return;
	
	//2012,1220
	if(GetCurrentTime() - m_dwTime_ToolInform < 0)
	{
		m_dwTime_ToolInform = GetCurrentTime();
	}
	if( GetCurrentTime() - m_dwTime_ToolInform < 62000 )
		return;
	
	if( m_nCnt_ToolInform >= 3 )
	{
		m_nCnt_ToolInform = 0;		
		
		CString strErr;
		strErr = "Tool 정보 전송 요구 3회 무응답";
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Request Tool=%s mode change 3count non-response", st_lamp.mstr_tool_id );
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if (st_handler.cwnd_list != NULL)	// 리스트 바 화면 존재
		{
			sprintf(st_msg.c_abnormal_msg, "%s", strErr);
			st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, ABNORMAL_MSG);  // 동작 실패 출력 요청
		}
	}
	
	//2014,0623//Tool_Send
	CString mstr_tool_id;
	
	int nLenTool = st_basic.mstr_revipe_name.GetLength();
	if(nLenTool < 11) nLenTool = 11-st_basic.mstr_revipe_name.GetLength();
	
	st_lamp.mstr_tool_id.Format("IF-%s", st_basic.mstr_revipe_name);
	CString strZero;
	
	for (int i = 0; i < nLenTool; i++)
	{
		strZero += "0";
	}
	
	if(nLenTool < 11) st_lamp.mstr_tool_id = st_lamp.mstr_tool_id + strZero;	
	CString strSend;
	strSend.Format( "FUNCTION=TOOL_CHANGE EQPID=%s TOOLID=%s ", st_lamp.mstr_empty_id, st_lamp.mstr_tool_id );
	
	PushSendMsg( (LPCSTR)strSend, 0 );
	m_dwTime_ToolInform = GetCurrentTime();
	m_nCnt_ToolInform++;
}

void AClient_EC::OnReceived_VersionUpdate( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );

	if( strResult == "PASS" )
	{
		m_dwTime_VersionUpdate = -1;
		m_bVersionUpdate = false;
	}	
}

void AClient_EC::OnReceived_ReqEmptrTray( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	
	if( strResult == "PASS" )
	{
		m_dwTime_ToolInform = -1;
		m_bReqEmptyTray = false;
		m_nCnt_ReqEmpty = 0;
	}	
	else if( strResult == "FAIL" )
	{
		m_dwTime_ToolInform = -1;
		m_bReqEmptyTray = false;
		m_nCnt_ReqEmpty = 0;
	}	
}

void AClient_EC::OnReceived_SendToolChange( CString strRecv )
{
	CString strResult = OnNetworkBodyAnalysis( strRecv, "RESULT" );
	CString strMsg = OnNetworkBodyAnalysis( strRecv, "MSG", NET_OPT_QUOTES );
	
	if( strResult == "PASS" )
	{
		m_dwTime_ReqEmptyTray = -1;
		m_bToolInform = false;
		m_nCnt_ToolInform = 0;
	}	
	else if( strResult == "FAIL" )
	{
		m_dwTime_ReqEmptyTray = -1;
		m_bToolInform = false;
		m_nCnt_ToolInform = 0;
	}	
}

