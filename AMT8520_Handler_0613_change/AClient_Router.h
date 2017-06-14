// AClient_Router.h: interface for the AClient_Router class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACLIENT_ROUTER_H__43723790_4E4E_4645_8651_86C6C7421B6E__INCLUDED_)
#define AFX_ACLIENT_ROUTER_H__43723790_4E4E_4645_8651_86C6C7421B6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AClient_Router : public ANetwork_Client 
{
public:
	AClient_Router();
	virtual ~AClient_Router();

	virtual void Run_Move();
	void Run_Move_TrayReq();

	virtual void OnParseComplete(std::string strRecv );
	void OnReceived_TrayReq( CString strRecv );



	void OnReqEmptyTray( CString strLotID) { m_strLotID_TrayReq = strLotID; }

protected:
	DWORD	m_dwTime_TrayReq;
	CString	m_strLotID_TrayReq;
	int		m_nCnt_TrayReq;
};


extern AClient_Router g_client_router;

#endif // !defined(AFX_ACLIENT_ROUTER_H__43723790_4E4E_4645_8651_86C6C7421B6E__INCLUDED_)
