// AServer_Next.cpp: implementation of the AServer_Next class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AServer_Next.h"
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
AServer_Next g_server_next;

AServer_Next::AServer_Next()
{

}

AServer_Next::~AServer_Next()
{

}

void AServer_Next::OnParseComplete( std::string strRecv )
{
	if( strRecv == "" )	return;

	CString strFunction = OnNetworkBodyAnalysis( strRecv.c_str(), "FUNCTION" );
	if	   ( strFunction == "TRAY_INFO" )				OnReceived_TrayInfo( strRecv.c_str() );	
	else if( strFunction == "RECV_STATE" )				OnReceived_RecvState( strRecv.c_str() ); 
}

void AServer_Next::OnReceived_TrayInfo( CString strRecv )
{
	for( int i=0; i<MAX_TRAY; i++ )
	{
		CString strTrayName;
		strTrayName.Format( "TRAY_%02d", i );

		CString strInfo = OnNetworkBodyAnalysis( strRecv, strTrayName, NET_OPT_BRACKET );

		CString strPos = OnNetworkBodyAnalysis( strInfo, "CURRENT" );
		CString strTargetPos = OnNetworkBodyAnalysis( strInfo, "TARGET" );

		g_Tray.GetTrayExtra(i).SetPos( g_Tray.GetPosByStr( strPos ) );
		g_Tray.GetTrayExtra(i).SetTargetPos( g_Tray.GetPosByStr( strTargetPos ) );
	}

	// Extra Info Display
}

void AServer_Next::OnReceived_RecvState( CString strRecv )
{
	CString strState = OnNetworkBodyAnalysis( strRecv, "STATE" );
	if( strState == "1" )
		g_handler.SetRecvStateNextMachine( true );
	else
		g_handler.SetRecvStateNextMachine( false );
}


