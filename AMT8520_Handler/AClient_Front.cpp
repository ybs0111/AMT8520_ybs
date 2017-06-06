// AClient_Front.cpp: implementation of the AClient_Front class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "AClient_Front.h"
#include "SrcPart/APartTray.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AClient_Front g_client_front;

AClient_Front::AClient_Front()
{

}

AClient_Front::~AClient_Front()
{

}

// void AClient_Front::Run_Move()
// {
// 	ANetwork_Client::Run_Move();
// 	
// 	if( m_bUsable == false )
// 		return;
// 	
// 	Run_Move_NewLotIn();
// 	Run_Move_BufferIn();
// 	Run_Move_BufferReady();
// }

void AClient_Front::SendTrayInfo()
{
	CString strTrayInfo = "FUNCTION=TRAY_INFO";
	for( int i=0; i<MAX_TRAY; i++ )
	{
		CString strTrayName;
		strTrayName.Format( "TRAY_%02d", i );

		CString strInfo;
		ETRAY_POS ePos = g_Tray.GetTray(i).GetPos();
		ETRAY_POS ePosTarget = g_Tray.GetTray(i).GetTargetPos();

		strInfo.Format( " %s=(CURRENT=%s TARGET=%s)", strTrayName, g_Tray.GetStrPos( ePos ), g_Tray.GetStrPos( ePosTarget ) );

		strTrayInfo += strInfo;
	}

	PushSendMsg( (LPCSTR)strTrayInfo );
}

void AClient_Front::SendRecvState()
{
	CString strState;

	int nRecv = Run_Bin_Stacker[0].GetRecvTrayNone();
	if( nRecv == 0 ) nRecv = 1;
	else			 nRecv = 0;

	strState.Format( "FUNCTION=RECV_STATE STATE=%d", nRecv);
	PushSendMsg( (LPCSTR)strState );
}
