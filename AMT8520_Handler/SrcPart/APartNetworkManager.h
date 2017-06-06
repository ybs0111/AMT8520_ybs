// APartNetworkManager.h: interface for the APartNetworkManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APARTNETWORKMANAGER_H__CDC59BC2_D104_4106_9520_909C3D5010A7__INCLUDED_)
#define AFX_APARTNETWORKMANAGER_H__CDC59BC2_D104_4106_9520_909C3D5010A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\SrcBase\ANetwork.h"

#include "../AClient_BPC.h"
#include "../AClient_Front.h"
#include "../AClient_EC.h"
#include "../AClient_Next.h"
#include "../AClient_Router.h"
#include "../AClient_Xgem.h"

#include "../AServer_BPC.h"
#include "../AServer_Front.h"
#include "../AServer_Next.h"

class APartNetworkManager
{
public:
	APartNetworkManager();
	virtual ~APartNetworkManager();

	void OnServerReceived( CServerItem* psItem );
	void OnServerClosed( CServerItem* psItem );
	void OnServerAccept( CServerSocket* pServer );

	void OnClientReceived( CClientSocket* pClient );
	void OnClientClosed( CClientSocket* pClient );

	void Run_Move();

	void OnStartServer();
	void OnInitClientAddress();

	void ClientAccept( int iAddr );

	CString GetAddressName_C( int iAddr );
	CString GetAddressName_S( int iAddr );
};

extern APartNetworkManager g_Net;

#endif // !defined(AFX_APARTNETWORKMANAGER_H__CDC59BC2_D104_4106_9520_909C3D5010A7__INCLUDED_)
