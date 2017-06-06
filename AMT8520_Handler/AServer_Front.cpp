// AServer_Front.cpp: implementation of the AServer_Front class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AServer_Front.h"
#include "AMTLotManager.h"
#include "SrcPart/APartNetworkManager.h"
#include "SrcPart/APartTray.h"
#include "SrcPart/APartHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AServer_Front g_server_front;

AServer_Front::AServer_Front()
{
	m_nrBufferIn = NR_NONE;
	m_nTrayNum = 0;
}

AServer_Front::~AServer_Front()
{

}

void AServer_Front::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )	return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION" );
	if	   ( strFunction == "NEW_LOT_IN" )				OnReceived_NewLotIn( strRecv.c_str() );
	else if( strFunction == "BUFFER_IN" )				OnReceived_BufferIn( strRecv.c_str() );
	else if( strFunction == "BUFFER_READY" )			OnReceived_BufferReady( strRecv.c_str() );
	else if( strFunction == "TRAY_INFO_REQ"	)			OnReceived_TrayInfoReq( strRecv.c_str() );
	else if( strFunction == "RECV_STATE_REQ" )			OnReceived_RecvStateReq( strRecv.c_str() );
	else if( strFunction == "RECIPE_INFO" )				OnReceived_RecipeInfo( strRecv.c_str() );//2015.1006
}

void AServer_Front::OnReceived_NewLotIn( CString strRecv )
{
	CString strEqpID = OnNetworkBodyAnalysis( strRecv, "EQPID" );
	CString strLotID = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	CString strPartID = OnNetworkBodyAnalysis( strRecv, "PARTID" );
	CString strProcessID = OnNetworkBodyAnalysis( strRecv, "PROCESS_ID" );

	// LOT 추가.
	g_lotMgr.OnNewLotIn( strLotID, strPartID );

	// Reply
	CString strRpy;

	if(st_handler.mn_fullinline_mode == 1)
	{
		strRpy.Format( "FUNCTION_RPY=NEW_LOT_IN EQP_ID=%s LOTID=%s PARTID=%s PROCESS_ID=%s RESULT=PASS MSG=\"\"", st_lamp.mstr_equip_id, strLotID, strPartID, strProcessID);
	
		g_client_next.SetProcessID( strProcessID );
	}
	else
	{
		strRpy.Format( "FUNCTION_RPY=NEW_LOT_IN EQP_ID=%s LOTID=%s PARTID=%s RESULT=PASS MSG=\"\"", st_lamp.mstr_equip_id, strLotID, strPartID);
	}
	PushSendMsg( (LPCSTR)strRpy );

	// 트레이 요청
	//g_client_router.OnReqEmptyTray( strLotID );

	// 뒷 설비로 전달.
	g_client_next.SetNewLotInID( strLotID );
	

	// bpc로 전달.
	g_client_bpc.SetNewLotInID( strLotID );
// 	st_handler.mn_emptyout_cnt = 0; //ybs
// 	st_handler.mn_emptynext_cnt = 0;//2012,1128
// 	st_handler.mn_emptyin_cnt = 0;//2012,1128

	if (st_handler.cwnd_title != NULL)	
	{
		st_handler.cwnd_title->PostMessage(WM_STATUS_CHANGE, EMPTYTRAY_MODE, 0);
	}

	//2017.0602
	m_nTrayNum = 0;
}

void AServer_Front::OnReceived_BufferIn( CString strRecv )
{
	if( m_nrBufferIn != NR_WAITING )
	{
		CString strPass = strRecv;
		strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
		strPass += "RESULT=FAIL MSG=\"NOT READY\"";
		
		PushSendMsg( (LPCSTR)strPass );						// 응답.
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	CString strEqpID =		OnNetworkBodyAnalysis( strRecv, "EQPID" );
	CString strLotID =		OnNetworkBodyAnalysis( strRecv, "LOTID" );
	CString strPartID =		OnNetworkBodyAnalysis( strRecv, "PARTID" );
	//2013,0715
	CString strBufferNo = _T("");
	CString strRPropEnd = _T("");
	if(st_handler.mn_fullinline_mode == 1)
	{
		strBufferNo =	OnNetworkBodyAnalysis( strRecv, "TRAY_NO" );
		//if(strBufferNo == "") strBufferNo = _T("01");
		//2017.0602
		m_nTrayNum++;
		if( m_nTrayNum <= 0) m_nTrayNum = 1;
		strBufferNo.Format("%d",m_nTrayNum);

		strRPropEnd =	OnNetworkBodyAnalysis( strRecv, "R_PROPERTY_END" );
		if(strRPropEnd == "") strRPropEnd = _T("NO");
		st_handler.m_strRPropEnd = strRPropEnd;
		//2013,0910
		g_lotMgr.GetLotByLotID( strLotID ).SetRProtyModule( strRPropEnd );	
		if(strRPropEnd == "NO" || strRPropEnd == "YES")//2013,0913
		{
			if(st_work.m_bPLotDel == true)
			{
				g_lotMgr.DeletePLot(strRPropEnd);
				st_work.m_bPLotDel = false;
			}
			g_lotMgr.OnNewPLotin(strLotID, strPartID); //2013,0912
			if( st_handler.cwnd_main )
				st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOTINFO, 0);

		}
	}
	else
	{
		strBufferNo =	OnNetworkBodyAnalysis( strRecv, "BUFFER_NO" );
		//if(strBufferNo == "") strBufferNo = _T("01");
		//2017.0602
		m_nTrayNum++;
		if( m_nTrayNum <= 0) m_nTrayNum = 1;
		strBufferNo.Format("%d",m_nTrayNum);

	}
// 	CString strBufferNo =	OnNetworkBodyAnalysis( strRecv, "BUFFER_NO" );
	CString strLastMdl =	OnNetworkBodyAnalysis( strRecv, "LAST_MDL" );
	CString strEmptyTray =	OnNetworkBodyAnalysis( strRecv, "EMPTY_TRAY" );
	CString strPocketCnt =	OnNetworkBodyAnalysis( strRecv, "POCKET_CNT" );
	CString strBypass =     OnNetworkBodyAnalysis( strRecv, "BYPASS" );

// 	if( g_handler.GetMachinePos() == EMPOS_FRONT &&
// 		g_lotMgr.GetLotCount() > 1 &&
// 		strLotID == g_lotMgr.GetLotIDAt(0) )
// 	{
// 		CString strPass = strRecv;
// 		strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
// 		strPass += "RESULT=FAIL MSG=\"CHECK LOTID PLEASE\"";
// 		
// 		PushSendMsg( (LPCSTR)strPass );						// 응답.
// 		return;
// 	}

// 	// 응답 2013,0104
// 	CString strPass = strRecv;
// 	strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
// 	strPass += "RESULT=PASS";
// 	
// 	PushSendMsg( (LPCSTR)strPass );						// 응답.

	//2014,0314
	//////////////////////////////////////////////////////////////////////////
// 	if(CHINA_VER == 0)//중국버전에는 사용 않한다.
// 	{
		if(g_lotMgr.GetLotCount() <= 0)
		{
			g_lotMgr.OnNewLotIn(strLotID, strPartID);
			g_client_next.SetNewLotInID( strLotID );
			g_client_bpc.SetNewLotInID( strLotID );
		}
		else
		{
			bool bLot = false;
			for ( int i = 0; i < g_lotMgr.GetLotCount(); i++)
			{
				if(g_lotMgr.GetLotAt(i).GetLotID() == strLotID)
				{
					bLot = true;
					break;
				}
			}
			if(bLot == false)
			{
				g_lotMgr.OnNewLotIn(strLotID, strPartID);
				g_client_next.SetNewLotInID( strLotID );
				g_client_bpc.SetNewLotInID( strLotID );
			}
		}
// 	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	CString strEC = strRecv;
	CString strReplace; strReplace.Format( "EQPID=%s", strEqpID );
	CString strReplaceTo; strReplaceTo.Format( "EQPID=%s", st_lamp.mstr_equip_id );
	strEC.Replace( strReplace, strReplaceTo );
// 	g_client_ec.OnBufferIn(strEC);//2013,0916
	//////////////////////////////////////////////////////////////////////////

	// Lot LastModule Set
	if( strLastMdl == "YES" )
	{
		g_lotMgr.GetLotByLotID( strLotID ).SetLastModule( strLastMdl );
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage(WM_WORK_END, MAIN_LOTINFO, 0);
	}

	// 바이패스
	bool bBypass = false;
	if( g_handler.GetMachinePos() == EMPOS_FRONT )
	{
		if( st_basic.mn_bypass_mode == BYPASS_ALL )
		{
			bBypass = true;
		}
	}
	else if( g_handler.GetMachinePos() == EMPOS_REAR )
	{
		if( strBypass == "YES" )
		{
			bBypass = true;
		}
	}
	
	// 트레이 추가.
	ATray tray;
	tray.SetPos( TPOS_FRONT_TO_CONV1 );
// 	tray.SetTrayInfo( strLotID, atoi((LPCSTR)strBufferNo), (strLastMdl == "YES"), (strEmptyTray == "YES"), bBypass );
	//2017.0602
	tray.SetTrayInfo( strLotID, strPartID, atoi((LPCSTR)strBufferNo), (strLastMdl == "YES"), (strEmptyTray == "YES"), bBypass );

	//2013,0715
	if(st_handler.mn_fullinline_mode == 1)
	{
		CString strProcessID =	OnNetworkBodyAnalysis( strRecv, "PROCESS_ID" );
// 		strProcessID = "SMFA";
		tray.SetProcessID(strProcessID);
		st_handler.m_strProcessID = strProcessID;
		g_lotMgr.GetLotByLotID( strLotID ).SetProcessidModule( strProcessID );
		//2013,0910
		if(strRPropEnd == "YES")
		{
			tray.SetPropertyEnd(strRPropEnd);
			st_handler.m_strRPropEnd = strRPropEnd;
		}
	}

	CString strPocket[MAX_MODULE];
	int nProductCnt = 0;
	for( int i=0; i<MAX_MODULE; i++ )
	{
		CString strPocketIdx;
		strPocketIdx.Format( "POCKET_%02d", i + 1);

		strPocket[i] = OnNetworkBodyAnalysis( strRecv, strPocketIdx, NET_OPT_BRACKET );
		if( strPocket[i] == "NONE" || strPocket[i] == "" ) continue;

		CString strArraySN =	OnNetworkBodyAnalysis( strPocket[i], "ARRAYSN" );
		CString strSerial =		OnNetworkBodyAnalysis( strPocket[i], "SERIAL" );
		CString strPPID =		OnNetworkBodyAnalysis( strPocket[i], "PPID" );
		CString strWWN =		OnNetworkBodyAnalysis( strPocket[i], "WWN" );
		CString strCSerial =	OnNetworkBodyAnalysis( strPocket[i], "C_SERIAL" );
		CString strPSID =		OnNetworkBodyAnalysis( strPocket[i], "PSID" );
		CString strBIN =		OnNetworkBodyAnalysis( strPocket[i], "BIN" );

		//2015.1123
		CString strCSerial2 =   OnNetworkBodyAnalysis( strPocket[i], "8S" );
// 		if(st_basic.n_cserial2_mode == CTL_YES)
// 		{
// 			strCSerial2 =		OnNetworkBodyAnalysis( strPocket[i], "8S" );
// 		}

		// 모듈 셋
		AModule mdl;
// 		mdl.SetModuleInfo( strLotID, strArraySN, strSerial, strPPID, strWWN, strBIN );
// 		//2015.1123
// 		if(st_basic.n_cserial2_mode == CTL_YES)
// 		{
			mdl.SetModuleInfo( strLotID, strArraySN, strSerial, strPPID, strWWN, strCSerial, strPSID, strBIN, strCSerial2 );
// 		}
// 		else
// 		{
// 			mdl.SetModuleInfo( strLotID, strArraySN, strSerial, strPPID, strWWN, strCSerial, strPSID, strBIN );
// 		}

		if( mdl.IsExist() && strBIN == "" )
		{
			nProductCnt++;//2017.0602
			mdl.SetModuleState( EMS_LOAD );
		}
		else if( st_handler.mn_fullinline_mode == 1 && atoi( strBIN ) >	1 )
		{
			mdl.SetModuleState( EMS_FAIL );
		}
		else if( st_handler.mn_fullinline_mode == 0 && atoi( strBIN ) >	3 )
		{
			mdl.SetModuleState( EMS_FAIL );
		}
		else if( mdl.IsExist() )
		{
			mdl.SetModuleState( EMS_GOOD );
		}
// 		else
// 		{
// 			CString strNextData;
// 			strNextData.Format("SERIAL=%s, BIN=%s",strSerial,strBIN);	
// 			Func.On_Log_Title_Msg(LOG_TOTAL, "[Server_Front::OnReceived_Buffer]", strNextData);
// 		}

		tray.SetModule(i, mdl);
	}
	
	// 트레이 추가.
	ETRAY_POS eTargetPos = g_Tray.CalcTargetPos( tray );
//	eTargetPos = TPOS_NEXT;
	tray.SetTargetPos( eTargetPos );
	//2017.0602
	tray.SetProductCnt( nProductCnt );

	g_Tray.AddTray( tray );


	// 응답
	CString strPass = strRecv;
	strPass.Replace( "FUNCTION=", "FUNCTION_RPY=" );
	strPass += "RESULT=PASS";
	
	PushSendMsg( (LPCSTR)strPass );						// 응답.


	//2017.0602
	if(st_basic.mn_mode_xgem == CTL_YES)
	{
		//전장비에서 생산된 자재가 올때 일단 로딩 후 언로딩한다
// 		if( tray.IsAllState( EMS_GOOD ) || tray.IsAllState( EMS_FAIL ) )
// 		{
// 			g_client_xgem.SetUldTrayStart( strLotID, strPartID, m_nTrayNum );
// 		}
// 		else
// 		{
			g_client_xgem.SetTrayLoadingStart(strLotID, strPartID, m_nTrayNum);
// 		}			
	}


	m_nrBufferIn = NR_RECEIVED;
}

void AServer_Front::OnReceived_BufferReady( CString strRecv )
{
	// 	CString strReason = OnNetworkBodyAnalysis( strRecv, "REASON" );
	// 
	// 	PushSendMsg( "FUNCTION_RPY=BUFFER_READY RESULT=PASS" );
	// 	g_handler.SetReadySendNext( true );
	// 	CString strReason, strEtcLotID, strEmptyTray;
	// 	strReason = OnNetworkBodyAnalysis( strRecv, "REASON" );
	// 	strEtcLotID = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	// 	strEmptyTray = OnNetworkBodyAnalysis( strRecv, "EMPTYTRAY" );
	// 	st_work.mn_prev_step[98] = 0;
	// 	st_work.mn_prev_step[99] = 0;
	// 	if(strEmptyTray == "YES")
	// 	{
	// 		st_work.mn_prev_step[99] = 1;
	// 	}
	// 	if(g_lotMgr.GetLotIDAt(0) == strEtcLotID)
	// 	{
	// 		st_work.mn_prev_step[98] = 1;
	// 	}
	// 	
	// 	PushSendMsg( "FUNCTION_RPY=BUFFER_READY RESULT=PASS" );
	// 	g_handler.SetReadySendNext( true );
	//ybs
	
	CString strReason, strEmptyTray, strLotId;
	
	strReason = OnNetworkBodyAnalysis( strRecv, "REASON" );
	strEmptyTray = OnNetworkBodyAnalysis( strRecv, "EMPTY_TRAY" );
	
	if(strEmptyTray != "YES")
	{
		strLotId = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	}
	else
	{
		strLotId = OnNetworkBodyAnalysis( strRecv, "LOTID" );
	}

	//2013,0910
	if(g_lotMgr.GetLotIDAt(0) != strLotId)
	{
		PushSendMsg( "FUNCTION_RPY=BUFFER_READY RESULT=FAIL Lot different Stop Please." );
	}
	else
	{
		PushSendMsg( "FUNCTION_RPY=BUFFER_READY RESULT=PASS" );
	}
	
	g_handler.SetReadySendNext( true, (strEmptyTray == "YES"), strLotId);
}

void AServer_Front::OnReceived_TrayInfoReq( CString strRecv )
{
	g_client_front.SendTrayInfo();
}

void AServer_Front::OnReceived_RecvStateReq( CString strRecv )
{
	g_client_front.SendRecvState();
}

void AServer_Front::OnReceived_RecipeInfo( CString strRecv )
{
	CString strPartid, strData;	

	strPartid = OnNetworkBodyAnalysis( strRecv, "PARTID" );
	if(g_handler.GetMachinePos() == EMPOS_FRONT)
	{
		g_client_next.SetRecipeInfoID(strPartid);
	}
	if(st_basic.n_recipe_mode == CTL_YES)
	{
		bool bLotCnt = false;
		if( g_lotMgr.GetLotCount() > 0 )
			bLotCnt = true;
		//2016.0129

		if( bLotCnt == true)
		{
			strData = strRecv;
			strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
			strData += _T(" ");
			strData += _T("RESULT=FAIL");
			strData += _T(" ");
			strData += _T("MSG=ALREADY RUNNING");
			
			PushSendMsg( (LPCSTR)strData );
		}
		else
		{
			CString strRecipe = Func.OnRecipeCheck(strPartid);
			if ( strRecipe == _T("") )
			{
				strData = strRecv;
				strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
				strData += _T(" ");
				strData += _T("RESULT=FAIL");
				strData += _T(" ");
				strData += _T("MSG=RECIPE CHECK ERROR");
				
				PushSendMsg( (LPCSTR)strData );
				
				st_msg.mstr_event_msg[0] = _T("Recipe Check Error");
				st_msg.mstr_event_msg[1] = _T("Plase Change The Recipe");
				::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
				
				Func.OnSet_IO_Port_Stop();
			}
			else
			{

				if (st_basic.mstr_device_name != strRecipe)
				{
					Func.OnSet_IO_Port_Stop();

					CString strOldRecipe = st_basic.mstr_device_name;
					st_basic.mstr_device_name = strRecipe;

					
					
					mcls_server_front.OnDaily_Count_Save();
					mcls_server_front.OnBasic_Data_Load();
					int nRet_1 = mcls_server_front.On_Teach_Data_Load();
					if( nRet_1 == -1 )
					{
						st_msg.mstr_event_msg[0] = _T("Recipe change error!");
						st_msg.mstr_event_msg[1] = _T("Plase change model in basic menu manually");
						::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
						
						Func.OnSet_IO_Port_Stop();

						strData = strRecv;
						strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
						strData += _T(" ");
						strData += _T("RESULT=FAIL");
						strData += _T(" ");
						strData += _T("MSG=RECIPE CHANGE ERROR");
						
						PushSendMsg( (LPCSTR)strData );

						st_basic.mstr_device_name = strOldRecipe;
						mcls_server_front.OnBasic_Data_Load();
						mcls_server_front.On_Teach_Data_Load();
						return;
					}
					mcls_server_front.OnAdmin_Data_Load();
					mcls_server_front.OnMotorSpeed_Set_Data_Load();
					mcls_server_front.OnWaitTime_Data_Load();
					mcls_server_front.OnMaintenance_Data_Load();

					strData = strRecv;
					strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
					strData += _T(" ");
					strData += _T("RESULT=OK");
					strData += _T(" ");
					strData += _T("MSG=");
					
					PushSendMsg( (LPCSTR)strData );

					//g_client_next.SetNewLotInID( strPartid );

					if (st_handler.cwnd_list != NULL)
					{
						st_handler.cwnd_list->PostMessage(WM_LIST_DATA, 0, MACHINE_INFO);
					}
					st_msg.mstr_event_msg[0] = _T("Change recipe.");
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

					//2016.0316
					if( g_lotMgr.GetLotCount() == 0 )
					{
						g_client_bpc.SetTestMode( "TestMode_HotFix" );//2016.0223		
						st_handler.mstr_device_name = st_basic.mstr_device_name;
					}

					


				}
				else
				{

					strData = strRecv;
					strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
					strData += _T(" ");
					strData += _T("RESULT=OK");
					strData += _T(" ");
					strData += _T("MSG=LOT START");
					
					PushSendMsg( (LPCSTR)strData );

					st_msg.mstr_event_msg[0] = _T("Recipe is same.");
					::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);

				}
			}
		}
	}
	else
	{
		strData = strRecv;
		strData.Replace(_T("FUNCTION"), _T("FUNCTION_RPY"));
		strData += _T(" ");
		strData += _T("RESULT=OK");
		strData += _T(" ");
		strData += _T("MSG=");

		PushSendMsg( (LPCSTR)strData );

		st_msg.mstr_event_msg[0] = _T("Basic Menu : Recipe check mode");
		st_msg.mstr_event_msg[1] = _T("Plase change the Recipe mode");
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		Func.OnSet_IO_Port_Stop();

	}
}