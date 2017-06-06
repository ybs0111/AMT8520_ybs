// AServer_BPC.cpp: implementation of the AServer_BPC class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AServer_BPC.h"
#include "SrcPart/PartFunction.h"
#include "SrcPart/APartTestSite.h"
#include "IO_Manager.h"
#include "Srcbase/ALocalization.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AServer_BPC g_server_bpc;

AServer_BPC::AServer_BPC()
{

}

AServer_BPC::~AServer_BPC()
{

}

void AServer_BPC::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )	return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION" );
	if	   ( strFunction == "TEST_RESULT" )				OnReceived_TestResult( strRecv.c_str() );
	if	   ( strFunction == "INFO_CHECK" )				OnReceived_TestInfoCheck( strRecv.c_str() );
	//2016.1121
	if	   ( strFunction == "SOCKET" )					OnReceived_TestSocketCheck( strRecv.c_str() );
}


void AServer_BPC::OnReceived_TestResult( CString strRecv )
{
	CString strLotID =		OnNetworkBodyAnalysis( strRecv, "LOTID" );
	CString strSlot =		OnNetworkBodyAnalysis( strRecv, "SLOT" );
	CString strSerial =		OnNetworkBodyAnalysis( strRecv, "SERIAL" );
	CString strBin =		OnNetworkBodyAnalysis( strRecv, "BIN" );
	CString strScrapCode =	OnNetworkBodyAnalysis( strRecv, "SCRAP_CODE" );
// 	CString strCPPid	 =	OnNetworkBodyAnalysis( strRecv, "PPID" );
// 	CString strwwn	 =	OnNetworkBodyAnalysis( strRecv, "WWN" );
	CString strCserial =	OnNetworkBodyAnalysis( strRecv, "C_SERIAL" );
	CString strPsid	 =	OnNetworkBodyAnalysis( strRecv, "PSID" );

	//2016.0302
	//CString strCserial2 =   OnNetworkBodyAnalysis( strRecv, "8S" );
	CString strCserial2 =   OnNetworkBodyAnalysis( strRecv, "C_SERIAL2" );
// 	if( st_basic.n_cserial2_mode == CTL_YES )
// 	{
// 		strCserial2 =	OnNetworkBodyAnalysis( strRecv, "8S" );
// 	}

	// 처리
	int nSlot = atoi( strSlot );
	int nBin = atoi(strBin);
// 	if( nSlot < 1 || nSlot > 20 || nBin <= 0 || nBin > 9)
// 	{
// 		st_msg.mstr_event_msg[0].Format("테스터에서 Slot:%d Bin:%d입니다.우선 Abort(타임아웃)처리합니다.",nSlot,nBin);
// 		g_ioMgr.set_out_bit(st_io.o_buzzer_1, IO_ON);
// 		
// 		::SendMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
// 		
// 		return;
// 	}

	if( nSlot < 1 || nSlot > 20 || nBin <= 0 || nBin > 9)
	{
// 		CString str_msg;
// 		str_msg.Format("[Handler Abort] Slot:%d Bin:%d -> 8", nSlot, nBin);
// 		Func.On_LogFile_Add(0, str_msg);
// 		Func.On_LogFile_Add(99, str_msg);
		
		nBin = 8;
		strBin.Format("%d",nBin);
		CString strRpy;
		strRpy = _T( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL");
		PushSendMsg( (LPCSTR)strRpy );
		return;
	}

	
	AModule& mdl = g_site.GetModule(nSlot - 1);

// 	CString str_ybs;
// 	str_ybs.Format("[TestResult] %s %s %s %s %s %s %s",strLotID,strSlot,strSerial,strBin,strScrapCode,strCserial,strPsid);
// 	Func.On_LogFile_Add(99, str_ybs);
// 
// 	str_ybs.Format("[TestResult] %s %s",mdl.GetLotNo(),mdl.GetSerial());
// 	Func.On_LogFile_Add(99, str_ybs);

	if( strLotID != mdl.GetLotNo() )
	{
		CString strRpy;

		//2015.1123
// 		if( st_basic.n_cserial2_mode == CTL_YES )//8S=%s
// 		{
// 			strRpy.Format( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL LOTID=%s SLOT=%d SERIAL=%s C_Serial=%s 8S=%s PSID=%s MSG=\"NOT EQUAL LOT_ID\"", strLotID, nSlot, 
// 				mdl.GetSerial(), strCserial, strCserial2, strPsid);
// 		}
// 		else
// 		{
// 			strRpy.Format( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL LOTID=%s SLOT=%d SERIAL=%s C_Serial=%s PSID=%s MSG=\"NOT EQUAL LOT_ID\"", strLotID, nSlot, 
// 				mdl.GetSerial(),strCserial,strPsid);
// 		}
		strRpy.Format( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL LOTID=%s SLOT=%d SERIAL=%s C_Serial=%s C_SERIAL2=%s PSID=%s MSG=\"NOT EQUAL LOT_ID\"", strLotID, nSlot, 
			mdl.GetSerial(), strCserial, strCserial2, strPsid);
		PushSendMsg( (LPCSTR)strRpy );
		
		//		PushSendMsg( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL MSG=\"NOT EQUAL LOT_ID\"" );
		return;
	}
	else if( strSerial != mdl.GetSerial() )
	{
		CString strRpy;
		strRpy.Format( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL LOTID=%s SLOT=%d SERIAL=%s MSG=\"NOT EQUAL SERIAL\"", strLotID, nSlot, 
			mdl.GetSerial());
		PushSendMsg( (LPCSTR)strRpy );
		// 		PushSendMsg( "FUNCTION_RPY=TEST_RESULT RESULT=FAIL MSG=\"NOT EQUAL SERIAL\"" );
		return;
	}

	mdl.SetScrapCode( strScrapCode );//2015.0604
	if( mdl.GetBin() == "" )
	{
		mdl.SetBin( strBin );
//		mdl.SetScrapCode( strScrapCode );//2015.0604
		//2013,0715
// 		mdl.SetCSN(strCserial);
// 		mdl.SetPSID(strPsid);
 		mdl.SetTestedPos( nSlot - 1 );
	}
	
	st_handler.isTestRun[nSlot - 1] = CTL_NO; //2015.0914

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDS_TESTSITE);
	
	CString strRpy;
	strRpy.Format( "FUNCTION_RPY=TEST_RESULT RESULT=OK LOTID=%s SLOT=%d SERIAL=%s BIN=%s SCRAP_CODE=%s", strLotID, nSlot, 
		mdl.GetSerial(), mdl.GetBin(), "" );
	PushSendMsg( (LPCSTR)strRpy );
}

void AServer_BPC::OnReceived_TestInfoCheck( CString strRecv )
{
	CString strPass = strRecv;
	strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
	strPass += "RESULT=PASS";
	PushSendMsg( (LPCSTR)strPass );


	CString strErr;
	strErr = _T("Tester Critical Error. Call ENG");
	if ( g_local.GetLocalType() == LOCAL_ENG ) strErr = _T("Tester Critical Error. Call ENG");
	st_msg.mstr_event_msg[0] = strErr;
	::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
	Func.OnSet_IO_Port_Stop(); // 장비 상태 : 정지 상태인 경우 I/O 출력 내보내는 함수
	if(st_handler.cwnd_title != NULL)   st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, 2, dSTOP);
}

void AServer_BPC::OnReceived_TestSocketCheck( CString strRecv )
{
	CString strPass = strRecv;
	strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
	strPass += "RESULT=PASS";
	PushSendMsg( (LPCSTR)strPass );

	int n_sock = 0;

	CString strCon = OnNetworkBodyAnalysis( strRecv, "CONDITION" );
	CString strSocket = OnNetworkBodyAnalysis( strRecv, "SLOT" );

	if( strCon == "ON" )
	{
		n_sock = atoi(strSocket);
		n_sock = n_sock -1;
		if( n_sock >= 0 && n_sock < 20)
		{
			g_site.SetEnableSocket( n_sock, true );
		}
	}
	else if( strCon == "OFF")
	{
		n_sock = atoi(strSocket);
		n_sock = n_sock -1;
		if( n_sock >=0 && n_sock < 20)
		{
			g_site.SetEnableSocket( n_sock, -3 );
		}
	}
}
