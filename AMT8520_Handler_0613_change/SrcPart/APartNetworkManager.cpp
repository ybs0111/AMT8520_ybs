// APartNetworkManager.cpp: implementation of the APartNetworkManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "handler.h"
#include "APartNetworkManager.h"
#include "../Run_Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
APartNetworkManager g_Net;

APartNetworkManager::APartNetworkManager()
{

}

APartNetworkManager::~APartNetworkManager()
{

}

void APartNetworkManager::OnServerReceived( CServerItem* psItem )
{
	if( psItem == NULL )	return;
	if( psItem->m_ServerSocket == NULL )	return;

	if( (CSocket*)g_server_bpc.GetPointer_Server() == psItem->m_ServerSocket )
		g_server_bpc.OnServerReceived( psItem );

	else if( (CSocket*)g_server_front.GetPointer_Server() == psItem->m_ServerSocket )
		g_server_front.OnServerReceived( psItem );

	else if( (CSocket*)g_server_next.GetPointer_Server() == psItem->m_ServerSocket )
		g_server_next.OnServerReceived( psItem );

	else
		g_server_front.OnServerReceived( psItem );
}

#include "../Public_Function.h"
void APartNetworkManager::OnServerClosed( CServerItem* psItem )
{
	// 로그 찍는 용도.

	CString str_msg;
	
	if( psItem == NULL )	return;
	if( psItem->m_ServerSocket == NULL )	return;
	
	if( (CSocket*)g_server_bpc.GetPointer_Server() == psItem->m_ServerSocket )
	{
		str_msg = "g_server_bpc::GetPointer_Server";
		Func.On_LogFile_Add(99, str_msg);
	}
	
	else if( (CSocket*)g_server_front.GetPointer_Server() == psItem->m_ServerSocket )
	{
		str_msg = "g_server_front::GetPointer_Server";
		Func.On_LogFile_Add(99, str_msg);
	}
	
	else if( (CSocket*)g_server_next.GetPointer_Server() == psItem->m_ServerSocket )
	{
		str_msg = "g_server_next::GetPointer_Server";
		Func.On_LogFile_Add(99, str_msg);
	}
	
	else
	{
		str_msg = "g_server_bpc::ETC";
		Func.On_LogFile_Add(99, str_msg);
	}


}

void APartNetworkManager::OnServerAccept( CServerSocket* pServer )
{
	// 로그
}

void APartNetworkManager::OnClientReceived( CClientSocket* pClient )
{
	if( pClient == NULL )	return;


	if( g_client_bpc.GetPointer_Client() == pClient )
		g_client_bpc.OnClientReceived();

	else if( g_client_ec.GetPointer_Client() == pClient )
		g_client_ec.OnClientReceived();

	else if( g_client_front.GetPointer_Client() == pClient )
		g_client_front.OnClientReceived();

	else if( g_client_next.GetPointer_Client() == pClient )
		g_client_next.OnClientReceived();

	else if( g_client_router.GetPointer_Client() == pClient )
		g_client_router.OnClientReceived();

	else if( g_client_xgem.GetPointer_Client() == pClient )
		g_client_xgem.OnClientReceived();

}

void APartNetworkManager::OnClientClosed(CClientSocket* pClient )
{
	// close...
	if( pClient == NULL )	return;
	CString str_msg;

	if( g_client_bpc.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_bpc.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		g_client_bpc.OnClientClose();
	}

	else if( g_client_ec.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_ec.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		g_client_ec.OnClientClose();
	}

	if( g_client_front.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_front.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		g_client_front.OnClientClose();
	}

	if( g_client_next.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_next.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		g_client_next.OnClientClose();
	}

	if( g_client_router.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_router.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		g_client_router.OnClientClose();
	}

	if( g_client_xgem.GetPointer_Client() == pClient )
	{
		str_msg = "g_client_xgem.GetPointer_Client()";
		Func.On_LogFile_Add(99, str_msg);
		//2016.0117
		g_client_xgem.SetUsable(false);
		st_handler.mn_xgem_connect = CTL_NO;
		g_client_xgem.OnClientClose();
	}

	if( st_handler.cwnd_main )
		st_handler.cwnd_main->PostMessage( WM_DRAW_DATA_MAIN, EDDN_NETUSABLE);

}

void APartNetworkManager::Run_Move()
{
	g_client_router.Run_Move();
	g_client_bpc.Run_Move();
	g_client_ec.Run_Move();
	g_client_front.Run_Move();
	g_client_next.Run_Move();
	g_client_xgem.Run_Move();//2013,1028

	g_server_bpc.Run_Move();
	g_server_front.Run_Move();
	g_server_next.Run_Move();
}

void APartNetworkManager::OnStartServer()
{
	g_server_bpc.OnServerOpen(0);
	g_server_front.OnServerOpen(1);
	g_server_next.OnServerOpen(2);
}

void APartNetworkManager::OnInitClientAddress()
{
	g_client_router.SetAddress( 0 );
	g_client_ec.SetAddress( 1 );
	g_client_bpc.SetAddress( 2 );
	g_client_front.SetAddress( 3 );
	g_client_next.SetAddress( 4 );	
	g_client_xgem.SetAddress( 5 );
}

void APartNetworkManager::ClientAccept( int iAddr )
{
	switch( iAddr )
	{
	case 0: g_client_router.OnClientAccept();	break;
	case 1:	g_client_ec.OnClientAccept();		break;
	case 2:	g_client_bpc.OnClientAccept();		break;
	case 3:	g_client_front.OnClientAccept();	break;
	case 4:	g_client_next.OnClientAccept();		break;
	case 5:	g_client_xgem.OnClientAccept();		break;
	}
}	

CString APartNetworkManager::GetAddressName_C( int iAddr )
{
	switch( iAddr )
	{
	case 0:	return "R SORTER";
	case 1: return "EC SERVER";
	case 2: return "TESTER";
	case 3: return "FRONT";
	case 4: return "NEXT";
	case 5: return "XGEM";
	}

	return "UNKNOWN";
}

CString APartNetworkManager::GetAddressName_S( int iAddr )
{
	switch( iAddr )
	{
	case 0 :	return "TESTER";
	case 1:		return "FRONT";
	case 2:		return "NEXT";
	}
	
	return "UNKNOWN";
}
