// AClient_Xgem.cpp : implementation file
//

#include "stdafx.h"
#include "handler.h"
#include "AClient_Xgem.h"
#include "Srcbase/ALocalization.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AClient_Xgem
AClient_Xgem g_client_xgem;

AClient_Xgem::AClient_Xgem()
{
	m_bGemInit = false;
	m_dwTime_GemInit_Send = -1;
	m_nCnt_GemInit = 0;

	m_bGemState[0] = false;
	m_bGemState[1] = false;
	m_bGemState[2] = false;
	m_dwTime_GemState_Send = -1;
	m_nCnt_GemState = 0;

	m_bGemAlarmState = false;
	m_strAlarmCode = _T("");
	m_nAlarmState = 0;
	m_nCnt_GemAlarmState = 0;
	m_dwTime_GemAlarmState_Send = -1;

	m_bGemMachineState = false;
	m_nGemMachineState = 0;
	m_dwTime_GemMachineState_Send = -1;
	m_nCnt_GemMachineState = 0;
}

AClient_Xgem::~AClient_Xgem()
{
}

/////////////////////////////////////////////////////////////////////////////
// AClient_Xgem message handlers
void AClient_Xgem::Run_Move()
{
	ANetwork_Client::Run_Move();

	if( m_bUsable == false )
		return;

	XgemInitialize();
	XgemMachineStatus();
	XgemAlarmStatus();
	for ( int i = 0; i < 3; i++)
	{
		XgemEqStatus( i );
	}
	XgemLotStart();
	XgemLotEnd();
	XgemSendPPID();
	XgemSendBufferIn();
// 	XgemSendOnSokStatus();
// 	XgemSendOffSokStatus();
}

void AClient_Xgem::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )	return;

	CString strFunction = OnEqToXgemCommand( strRecv.c_str());
	if	   ( strFunction == "COMMUNICATION" )			OnReceived_XgemInitialize( strRecv.c_str() );
	else if( strFunction == "INTERFACE")/*"OPERATE" )*/	OnReceived_XgemEqStatus( strRecv.c_str() );
	else if( strFunction == "ALARM" )					OnReceived_XgemAlarmStatus( strRecv.c_str() );
	else if( strFunction == "CEID"	)					OnReceived_XgemAnalysis( strRecv.c_str() );
	else if( strFunction == "RUN_STATUS" )				OnReceived_XgemRunStatus( strRecv.c_str() );
	else if( strFunction == "PPID_CHANGE" )				OnReceived_XgemPPIDChange( strRecv.c_str() );
	else if( strFunction == "RCMD" )					OnReceived_XgemRCMDChange( strRecv.c_str() );
}

void AClient_Xgem::OnReceived_XgemRCMDChange( CString strRecv)
{
	int		nPos[2];
	int		nLength;
	CString strConfig;
	CString strMsg;

	nPos[0] = strRecv.Find("RCMD_NUM=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);

	nLength = (nPos[1] - nPos[0]) - 9;
	strConfig = strRecv.Mid(nPos[0] + 9, nLength);
	if( strConfig == "START")
	{
		st_handler.m_nRemote = CTL_YES;
	}
	else if( strConfig == "STOP")                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
	{
		//nPos[0] = strRecv.Find("MSG=", 0);
		CString strCmp = "MSG,";
		nPos[0] = strRecv.Find(strCmp, 0);//2015.1212
		nPos[1] = strRecv.Find(",", nPos[0] + strCmp.GetLength());

		nLength = (nPos[1] - nPos[0]) - 4;
		strMsg = strRecv.Mid(nPos[0] + 4, nLength);
		st_handler.m_nRemote = CTL_NO;
		strMsg = "RCMD Remote STOP.." + strMsg;////2015.1211
		st_handler.m_xgemstrErrMsg = strMsg;
		

		st_msg.mstr_event_msg[0] = strMsg;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
	}
	else
	{
		return;
	}
}

//Ex)HD=000048,FUNC=COMMUNICATION,CONFIG=C:\\1234.cfg,
void AClient_Xgem::OnReceived_XgemInitialize( CString strRecv )
{
	int		nPos[2];
	int		nLength;
	CString strConfig;
	
	nPos[0] = strRecv.Find("CONFIG=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);

	nLength = (nPos[1] - nPos[0]) - 7;

	strConfig	= "";
	strConfig = strRecv.Mid(nPos[0] + 7, nLength);

	if (strConfig == "") return;

// 	m_bGemInit = false;
	m_dwTime_GemInit_Send = -1;
	st_xgem.m_str_xgmInit = "";

	st_xgem.m_str_xgmInit = (LPCTSTR)strConfig;

	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}
//Ex)HD=000023,FUNC=OPERATE,CONNECT=1,
void AClient_Xgem::OnReceived_XgemEqStatus( CString strRecv )
{
	int		nPos[2];
	int		nLength;
	CString strConnect;
	nPos[0] = strRecv.Find("CONNECT=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 8;
	
	strConnect	= "";
	strConnect = strRecv.Mid(nPos[0] + 8, nLength);
	
	if (strConnect == "") return;

	int nStatus = atoi(strConnect);

	m_bGemState[nStatus] = 0;
	m_dwTime_GemState_Send = -1;

	if( nStatus == 0)
	{
		st_xgem.m_str_connect = "off-line";
	}
	else if( nStatus == 1)
	{
		st_xgem.m_str_connect = "off-local";
	}
	else// if( nStatus == 2)
	{
		st_xgem.m_str_connect = "on-line";
	}
	
	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}
//Ex)HD=000000,FUNC=ALARM,CODE=100000,STATUS=1,
//STATUS	0 - CLEAR
//			1 - SET
void AClient_Xgem::OnReceived_XgemAlarmStatus( CString strRecv )
{
	int		nPos[2];
	int		nLength;
	long	nCode;
	long	nStatus;	
	CString strValue[10], strAlarm;

	nPos[0] = strRecv.Find("CODE=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 5;	
	nCode = atoi(strRecv.Mid(nPos[0] + 5, nLength));

	nPos[0] = strRecv.Find("STATUS=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 7;
	
	nStatus = atoi(strRecv.Mid(nPos[0] + 7, nLength));
	
//	if (m_hXgem->GEMSetAlarm(nCode, nStatus) < 0)
//	{
//		return;
//	}

	if(nStatus == 1)
	{
		//OnEqMachineStatus(MC_ALARM);
		strAlarm = "MC_ALARM";
	}
	else 
	{
		//OnEqMachineStatus(MC_STOP);
		strAlarm = "MC_STOP";
	}

	m_bGemAlarmState = false;
	m_dwTime_GemAlarmState_Send = -1;


	st_xgem.m_str_alarm.Format("%d %s", nCode, strAlarm);

	
	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}
//HD=000025,FUNC=RUN_STATUS,STATUS=0,EPORT=1
//HD=000025,FUNC=RUN_STATUS,STATUS=1,EPORT=1
void AClient_Xgem::OnReceived_XgemRunStatus( CString strRecv )
{
	int nLength;
	int nPos[2];
	int nStatus;

	nPos[0] = strRecv.Find("STATUS=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	
	nLength = (nPos[1] - nPos[0]) - 7;
	
	nStatus = atoi(strRecv.Mid(nPos[0] + 7, nLength));
//	clsXgem.OnEqMachineStatus(nStatus);

	m_bGemMachineState = false;
	m_dwTime_GemMachineState_Send = -1;

	
	if( nStatus == 0) st_xgem.m_str_machine_status = "STOP";
	else			  st_xgem.m_str_machine_status = "RUN";

	
	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}

void AClient_Xgem::OnReceived_XgemPPIDChange( CString strRecv )
{
	CString strJobChange;
	int nLength;
	int nPos[2];

	nPos[0]				= strRecv.Find("PP_NAME=", 0);
	nPos[1]				= strRecv.Find(",", nPos[0]);
	nLength				= (nPos[1] - nPos[0]) - 8;
	CString m_strPPID	= strRecv.Mid(nPos[0] + 8, nLength);

	nPos[0]				= strRecv.Find("PP_PATH=", 0);
	nPos[1]				= strRecv.Find(",", nPos[0]);
	nLength				= (nPos[1] - nPos[0]) - 8;
	CString strPath		= strRecv.Mid(nPos[0] + 8, nLength);
	st_xgem.m_str_ppid  = strPath;

	m_bGemPPid = false;
	
	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}

void AClient_Xgem::OnReceived_XgemAnalysis( CString strRecv )
{
	int nCeid;
	int nReport;
	int nReportCnt[10];
	int nVidCnt[10][10];
	int nPos[2];
	int nLength;
	int i, j;

	CString strReport[10][10];
	CString strTmp;
	CString strRpt;
	CString strVid;
	CString strValue[10];

	nPos[0] = strRecv.Find("CEID_NUM=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	nLength = (nPos[1] - nPos[0]) - 9;
	nCeid = atoi(strRecv.Mid(nPos[0] + 9, nLength));

	nPos[0] = strRecv.Find("REPORT=", 0);
	nPos[1] = strRecv.Find(",", nPos[0]);
	nLength = (nPos[1] - nPos[0]) - 7;
	nReport = atoi(strRecv.Mid(nPos[0] + 7, nLength));

	for(i=0; i<nReport; i++)
	{
		strTmp.Format("REP_%02d={", i+1);
		nPos[0]			= strRecv.Find(strTmp, 0);
		nPos[1]			= strRecv.Find("}", nPos[0]);
		nLength			= (nPos[1] - nPos[0]) - 8;
		strRpt			= strRecv.Mid(nPos[0] + 8, nLength);

		nPos[0]			= strRpt.Find("CNT=", 0);
		nPos[1]			= strRpt.Find(",", nPos[0]);
		nLength			= (nPos[1] - nPos[0]) - 4;
		nReportCnt[i]	= atoi(strRpt.Mid(nPos[0] + 4, nLength));

		for(j=0; j<nReportCnt[i]; j++)
		{
			strTmp.Format("[DA_%02d=", j+1);
			nPos[0]			= strRpt.Find(strTmp, 0);
			nPos[1]			= strRpt.Find("]", nPos[0]);
			nLength			= (nPos[1] - nPos[0]) - 7;
			strVid			= strRpt.Mid(nPos[0] + 7, nLength);

			nVidCnt[i][j]	= atoi(strVid.Mid(0, 2));
			strReport[i][j].Format("[%s]", strVid.Mid(3, strVid.GetLength()-3));
		}
	}

	if	   ( nCeid == 1001)//Equipment State Change RTPID 1000
	{
	}
	else if( nCeid == 1002 )//Lot Start RTPID 1001
	{
		st_xgem.m_str_lotid = "";
		for(i=0; i<nReport; i++)
		{
			for(j=0; j<nReportCnt[i]; j++)
			{
				st_xgem.m_str_lotid += strReport[i][j];
			}			
		}
		m_bGemLotStart = false;
	}
	else if( nCeid == 1003 )//Lot End RTPID 1002
	{
		st_xgem.m_str_lotid = "";
		for(i=0; i<nReport; i++)
		{
			for(j=0; j<nReportCnt[i]; j++)
			{
				st_xgem.m_str_lotid += strReport[i][j];
			}			
		}
		m_bGemLotEnd = false;
	}
	else if ( nCeid == 1004 )//Socket Off RTPID 1003
	{
		st_xgem.m_str_onSocket = "";
		for(i=0; i<nReport; i++)
		{
			for(j=0; j<nReportCnt[i]; j++)
			{
				st_xgem.m_str_onSocket += strReport[i][j];
			}			
		}
		m_bGemSokOff = false;
	}
	else if ( nCeid == 1005 )//Socket On RTPID 1004
	{
		st_xgem.m_str_offSocket = "";
		for(i=0; i<nReport; i++)
		{
			for(j=0; j<nReportCnt[i]; j++)
			{
				st_xgem.m_str_offSocket += strReport[i][j];
			}			
		}
		m_bGemSokOn = false;
	}
	else if ( nCeid == 1100 )//ProcessProgramSelect(JobChange) RTPID 1005
	{
	}
	
	if( st_handler.cwnd_data_xgem )
		st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );
}

void AClient_Xgem::XgemInitialize()
{
	if( m_bGemInit == false )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_GemInit_Send < 0)
	{
		m_dwTime_GemInit_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemInit_Send + 5000 )
		return;

	CString strHead, strFunc, strGemInit;
	
	if( m_nCnt_GemInit >= 3 )
	{
		m_nCnt_GemInit = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem Init 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem Init 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);

		if( st_handler.cwnd_data_xgem )
			st_handler.cwnd_data_xgem->PostMessage( WM_DRAW_DATA_XGEM );

		return;
	}

	CString strPath = "C:\\XGEM\\EqSample.cfg";

	strFunc = "FUNC=COMMUNICATION,CONFIG=" + strPath + ",";
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strGemInit = strHead+strFunc;
	PushSendMsg( (LPCTSTR) strGemInit);
	m_dwTime_GemInit_Send = GetCurrentTime();
	m_nCnt_GemInit++;

	m_bGemInit = false;
}

void AClient_Xgem::XgemEqStatus(int nMode)
{
	CString strHead, strFunc, strState;

	if( m_bGemState[nMode] == false )
		return;

	//2012,1220
	if(GetCurrentTime() - m_dwTime_GemState_Send < 0)
	{
		m_dwTime_GemState_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemState_Send + 5000 )
		return;

	if( m_nCnt_GemState >= 3 )
	{
		m_nCnt_GemState = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

	switch(nMode)
	{
	case 0:
		strFunc = "FUNC=OPERATE,CONNECT=0,";
		break;

	case 1:
		strFunc = "FUNC=OPERATE,CONNECT=1,";
		break;

	case 2:
		strFunc = "FUNC=OPERATE,CONNECT=2,";
		break;
	}

	strHead.Format( "HD=%06d,", strFunc.GetLength() );

	strState = strHead + strFunc;

	PushSendMsg( (LPCTSTR)strState );
	m_dwTime_GemState_Send = GetCurrentTime();
	m_nCnt_GemState++;
}

void AClient_Xgem::XgemAlarmStatus()
{
	CString strHead, strFunc, strAlarmState;

	if( m_bGemAlarmState == false )
		return;

	if(GetCurrentTime() - m_dwTime_GemAlarmState_Send < 0)
	{
		m_dwTime_GemAlarmState_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemAlarmState_Send + 5000 )
		return;

	if( m_nCnt_GemAlarmState >= 3 )
	{
		m_nCnt_GemAlarmState = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem Alarm State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem Alarm State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}
	
	strFunc.Format("FUNC=ALARM,CODE=%s,STATUS=%d,", m_strAlarmCode, m_nAlarmState);
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strAlarmState = strHead + strFunc;


	PushSendMsg( (LPCTSTR)strAlarmState );
	m_dwTime_GemAlarmState_Send = GetCurrentTime();
	m_nCnt_GemAlarmState++;
	m_bGemAlarmState=false;
}

void AClient_Xgem::XgemMachineStatus()
{
	CString strHead, strFunc, strMachineState;

	if( m_bGemMachineState == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemMachineState_Send < 0)
	{
		m_dwTime_GemMachineState_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemMachineState_Send + 5000 )
		return;

	if( m_nCnt_GemMachineState >= 3 )
	{
		m_nCnt_GemMachineState = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem Machine State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem Machine State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}
	strFunc.Format("FUNC=RUN_STATUS,STATUS=%d,", m_nGemMachineState);
	strHead.Format("HD=%06d,", strFunc.GetLength());
	
	strMachineState = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strMachineState );
	m_dwTime_GemMachineState_Send = GetCurrentTime();
	m_nCnt_GemMachineState++;

	m_bGemMachineState = false;
}

void AClient_Xgem::XgemLotStart()
{
	CString strHead, strFunc, strLotstart;
	
	if( m_bGemLotStart == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemLotStart_Send < 0)
	{
		m_dwTime_GemLotStart_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemLotStart_Send + 5000 )
		return;

	if( m_nCnt_GemLotStart >= 3 )
	{
		m_nCnt_GemLotStart = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem LotStart State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem LotStart State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

//NewLotInn
	strFunc.Format("FUNC=CEID,CEID_NUM=1002,REPORT=1,REP_01={CNT=03,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%s]},", st_lamp.mstr_equip_id, m_strSPartid, m_strSLotid);
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strLotstart = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strLotstart );
	m_dwTime_GemLotStart_Send = GetCurrentTime();
	m_nCnt_GemLotStart++;


	m_bGemLotStart = false;

}

void AClient_Xgem::XgemLotEnd()
{
	CString strHead, strFunc, strLotend;
	
	if( m_bGemLotEnd == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemLotEnd_Send < 0)
	{
		m_dwTime_GemLotEnd_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemLotEnd_Send + 5000 )
		return;

	if( m_nCnt_GemLotEnd >= 3 )
	{
		m_nCnt_GemLotEnd = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem LotEnd State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem LotEnd State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}


	strFunc.Format("FUNC=CEID,CEID_NUM=1003,REPORT=1,REP_01={CNT=07,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%s],[DA_04=01,%s],[DA_05=01,%s],[DA_06=01,%s],[DA_07=01,%s]},",
		st_lamp.mstr_equip_id, m_strSLotid, m_strSPartid, st_xgem.m_lTactTime, st_xgem.m_lTotCnt, st_xgem.m_lPassCnt, st_xgem.m_lFailCnt );
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strLotend = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strLotend );
	m_dwTime_GemLotStart_Send = GetCurrentTime();
	m_nCnt_GemLotEnd++;

	m_bGemLotEnd = false;
}

void AClient_Xgem::XgemSendPPID()
{
	CString strHead, strFunc, strPPid, strRecipe;
	
	if( m_bGemPPid == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemPPid_Send < 0)
	{
		m_dwTime_GemPPid_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemPPid_Send + 10000 )
		return;

	if( m_nCnt_GemPPid >= 3 )
	{
		m_nCnt_GemPPid = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem PPid State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem PPid State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

// 	int n_pos = st_basic.mstr_device_name.Find(".");  // 확장자 위치 검사
// 	
// 	if (n_pos == -1)
// 	{
// 		st_basic.mstr_device_name += _T(".txt");  // 확장자 추가
// 		n_pos = st_basic.mstr_device_name.Find(".");  // 확장자 위치 검사
// 	}
// 	
// 	CString str_device_name = st_basic.mstr_device_name.Mid(0, n_pos);

// 	strFunc.Format( "FUNC=PPID_CHANGE,PP_NAME=%s,PP_PATH=%s%s,", st_basic.mstr_device_name, st_path.mstr_recipe_path, st_basic.mstr_device_name);
	//2013,1216
	strRecipe = st_basic.mstr_revipe_name + ".txt";
	int n_pos = strRecipe.Find(".");  // 확장자 위치 검사
	
	if (n_pos == -1)
	{
		strRecipe += _T(".txt");  // 확장자 추가
	}
	strFunc.Format( "FUNC=PPID_CHANGE,PP_NAME=%s,PP_PATH=%s%s,", st_basic.mstr_revipe_name, st_path.mstr_recipe_path, strRecipe);

	strHead.Format("HD=%06d,", strFunc.GetLength());	
	strPPid = strHead+strFunc;
	
	PushSendMsg( (LPCTSTR)strPPid );
	m_dwTime_GemPPid_Send = GetCurrentTime();
	m_nCnt_GemPPid++;

	strFunc.Format("FUNC=CEID,CEID_NUM=1100,REPORT=1,REP_01={CNT=02,[DA_01=01,%s],[DA_02=01,%s]},", st_lamp.mstr_equip_id, st_basic.mstr_revipe_name);
	strHead.Format("HD=%06d,", strFunc.GetLength());

	CString strJobChange = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strJobChange );


	m_bGemPPid = false;
}

void AClient_Xgem::XgemSendBufferIn()
{
	CString strHead, strFunc, strBufferin;
	
	if( m_bGemPPid == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemBufferIn_Send < 0)
	{
		m_dwTime_GemBufferIn_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemBufferIn_Send + 5000 )
		return;

	if( m_nCnt_GemBufferin >= 3 )
	{
		m_nCnt_GemBufferin = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem PPid State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem PPid State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

//	strFunc = "FUNC=PPID_CHANGE,PP_NAME=파일명,PP_PATH=C:\\XGEM\AMT8520\\AMT8520\\XWork\\Recipe\\파일명,";

	strHead.Format("HD=%06d,", strFunc.GetLength());
	
	strBufferin = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strBufferin );
	m_dwTime_GemBufferIn_Send = GetCurrentTime();
	m_nCnt_GemPPid++;

	m_bGemPPid = false;
}

void AClient_Xgem::XgemSendOnSokStatus()
{
	CString strHead, strFunc, strSocketOn;
	
	if( m_bGemSokOn == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemSokOn_Send < 0)
	{
		m_dwTime_GemSokOn_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemSokOn_Send + 5000 )
		return;

	if( m_nCnt_GemSokOn >= 3 )
	{
		m_nCnt_GemSokOn = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem Socket On State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem Socket On State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

	strFunc.Format("FUNC=CEID,CEID_NUM=1005,REPORT=1,REP_01={CNT=04,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%d],[DA_04=01,%d]},", 
		st_lamp.mstr_equip_id, m_strOnSokSerial, m_nXpos, m_nYpos);
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strHead.Format("HD=%06d,", strFunc.GetLength());
	
	strSocketOn = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strSocketOn );
	m_dwTime_GemSokOn_Send = GetCurrentTime();
	m_nCnt_GemSokOn++;

	m_bGemSokOn = false;
}

void AClient_Xgem::XgemSendOffSokStatus()
{
	CString strHead, strFunc, strSocketOn;
	
	if( m_bGemSokOff == false )
		return;
	
	if(GetCurrentTime() - m_dwTime_GemSokOff_Send < 0)
	{
		m_dwTime_GemSokOff_Send = GetCurrentTime();
	}
	if( GetCurrentTime() <= m_dwTime_GemSokOff_Send + 5000 )
		return;

	if( m_nCnt_GemSokOff >= 3 )
	{
		m_nCnt_GemSokOff = 0;
		m_bUsable = false;
		
		CString strErr;
		strErr.Format( "Next : XGem Socket On State 3회 무응답");
		if ( g_local.GetLocalType() == LOCAL_ENG ) strErr.Format( "Next : XGem Socket On State 3count non-response");
		st_msg.mstr_event_msg[0] = strErr;
		::PostMessage(st_handler.hWnd, WM_MAIN_EVENT, CTL_YES, 0);
		
		if( st_handler.cwnd_main )
			st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);
		return;
	}

	strFunc.Format("FUNC=CEID,CEID_NUM=1004,REPORT=1,REP_01={CNT=04,[DA_01=01,%s],[DA_02=01,%s],[DA_03=01,%d],[DA_04=01,%d]},", 
		st_lamp.mstr_equip_id, m_strOffSokSerial, m_nXpos, m_nYpos);
	strHead.Format("HD=%06d,", strFunc.GetLength());

	strHead.Format("HD=%06d,", strFunc.GetLength());
	
	strSocketOn = strHead+strFunc;
	PushSendMsg( (LPCTSTR)strSocketOn );
	m_dwTime_GemSokOff_Send = GetCurrentTime();
	m_nCnt_GemSokOff++;

	m_bGemSokOff = false;
}


CString AClient_Xgem::OnEqToXgemFunction(CString strFind, CString strRecv)
{
	int		nPos[2];
	int		nLength;
	CString strData;

	nPos[0] = strRecv.Find(strFind, 0);
	nPos[1] = strRecv.Find(",", nPos[0]+1);

	nLength	= (nPos[1] - nPos[0]) - (strFind.GetLength()+1);
	strData = strRecv.Mid(nPos[0]+strFind.GetLength()+1, nLength);

	return strData;
}

CString AClient_Xgem::OnEqToXgemCommand(CString strRecv)
{
	int nLength;
	int nHead;
	int nPos[2];

	CString strFunc;
	CString strFind;
	CString strConnect;
	CString strPath;

	nLength	= strRecv.GetLength() - 10;

	nPos[0]	= -1;
	nPos[0]	= strRecv.Find("HD=", 0);

	if(nPos[0] > -1)
	{
		nHead = atoi(strRecv.Mid(nPos[0]+3, 6));
		if(nHead == nLength)
		{
			strFunc = OnEqToXgemFunction("FUNC", strRecv);
			return strFunc;
		}
	}

	return "NULL";
}


//void AClient_Xgem::OnEqToXgemCommand(CString strRecv)
//{
//	FILE *fp;
//
//	int nLength;
//	int nHead;
//	int nPos[2];
//	int nStatus;
//
//	char chBody[100];
//
//	CString strFunc;
//	CString strFind;
//	CString strConnect;
//	CString strPath;
//
//	nLength	= strRecv.GetLength() - 10;
//
//	nPos[0]	= -1;
//	nPos[0]	= strRecv.Find("HD=", 0);
//
//	if(nPos[0] > -1)
//	{
//		nHead = atoi(strRecv.Mid(nPos[0]+3, 6));
//		if(nHead == nLength)
//		{
//			strFunc = OnEqToXgemFunction("FUNC", strRecv);
//
//			if (strFunc == "COMMUNICATION")
//			{
//				clsXgem.OnEqCommunication(&m_hXgem, strRecv);
//			}
//			else if (strFunc == "OPERATE")
//			{
//				nPos[0] = strRecv.Find("CONNECT=", 0);
//				nPos[1] = strRecv.Find(",", nPos[0]);
//				
//				nLength = (nPos[1] - nPos[0]) - 8;
//				
//				strConnect	= "";
//				strConnect = strRecv.Mid(nPos[0] + 8, nLength);
//
//				if (strConnect == "0")
//				{
//					clsXgem.OnEqMachineOffLine();
//				}
//				else if(strConnect == "1")
//				{
//					clsXgem.OnEqMachineLocal();
//				}
//				else if(strConnect == "2")
//				{
//					clsXgem.OnEqMachineRemote();
//				}
//				m_bGemState[strConnect] = 0;
//				m_dwTime_GemState_Send = -1;
//				if( st_handler.cwnd_data_xgem )
//					st_handler.cwnd_data_lot->PostMessage( WM_DRAW_DATA_LOT );
//			}
//			else if (strFunc == "ALARM")
//			{
//				clsXgem.OnEqAlarm(strRecv);
//			}
//			else if (strFunc == "CEID")
//			{
//				clsXgem.OnEqCEID(strRecv);
//			}
//			else if (strFunc == "HOST_TIME")
//			{
//				clsXgem.OnHostGetCurrentTime();
//			}
//			else if (strFunc == "RUN_STATUS")
//			{
//				nPos[0] = strRecv.Find("STATUS=", 0);
//				nPos[1] = strRecv.Find(",", nPos[0]);
//				
//				nLength = (nPos[1] - nPos[0]) - 7;
//				
//				nStatus = atoi(strRecv.Mid(nPos[0] + 7, nLength));
//				clsXgem.OnEqMachineStatus(nStatus);
//			}
//			else if (strFunc == "PPID_CHANGE")
//			{
//				nPos[0]				= strRecv.Find("PP_NAME=", 0);
//				nPos[1]				= strRecv.Find(",", nPos[0]);
//				nLength				= (nPos[1] - nPos[0]) - 8;
//				clsXgem.m_strPPID	= strRecv.Mid(nPos[0] + 8, nLength);
//
//				nPos[0]				= strRecv.Find("PP_PATH=", 0);
//				nPos[1]				= strRecv.Find(",", nPos[0]);
//				nLength				= (nPos[1] - nPos[0]) - 8;
//				strPath				= strRecv.Mid(nPos[0] + 8, nLength);
//
//				fp = fopen ( strPath, "r" );
//				
//				if ( fp == (FILE *)NULL ) 
//				{
//					return ;
//				}
//
//				clsXgem.m_strPPBody = "";
//				while ( !feof(fp) )
//				{
//					if ( fgets (chBody, 100, fp ) == NULL )
//					{
//						break;
//					}
//					clsXgem.m_strPPBody += chBody;
//				}
//				fclose(fp);
//
//				if(clsXgem.OnEqPPIDLoad(clsXgem.m_strPPID) == CTLBD_RET_GOOD)
//				{
//					sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] S7F1 SEND SUCCESS");
//				}
//				else
//				{
//					sprintf(st_msg_info.c_normal_msg, "[XGEM->EQ] S7F1 SEND FAIL");
//				}
//				::PostMessage(this->m_hWnd, WM_LIST_DATA, NORMAL_MSG, 0);
//			}
//
//			sprintf(st_server_info[0].ch_send, strRecv);
//			::PostMessage(this->m_hWnd, WM_SERVER_MSG_1, SERVER_SEND, 0);
//		}
//	}
//}