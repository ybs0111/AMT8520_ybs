// AServer_BPC.h: interface for the AServer_BPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASERVER_BPC_H__D7B595B1_6A4F_4EB6_B45D_BB2A33AC45B1__INCLUDED_)
#define AFX_ASERVER_BPC_H__D7B595B1_6A4F_4EB6_B45D_BB2A33AC45B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AServer_BPC : public ANetwork_Server
{
public:
	AServer_BPC();
	virtual ~AServer_BPC();

	virtual void OnParseComplete( std::string strRecv );

	void OnReceived_TestResult( CString strRecv );
	void OnReceived_TestInfoCheck( CString strRecv );//2015.1218
	void OnReceived_TestSocketCheck( CString strRecv );//2016.1121

};

extern AServer_BPC g_server_bpc;

#endif // !defined(AFX_ASERVER_BPC_H__D7B595B1_6A4F_4EB6_B45D_BB2A33AC45B1__INCLUDED_)
