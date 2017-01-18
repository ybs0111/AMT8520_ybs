// AServer_Front.h: interface for the AServer_Front class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASERVER_FRONT_H__FCB85D24_2210_483A_A8AF_8D443CBDAD77__INCLUDED_)
#define AFX_ASERVER_FRONT_H__FCB85D24_2210_483A_A8AF_8D443CBDAD77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcPart/APartNetworkManager.h"
#include "SrcBase/ANetwork.h"
#include "MyBasicData.h"
#include <string>

class AServer_Front : public ANetwork_Server
{
public:
	AServer_Front();
	virtual ~AServer_Front();

	CMyBasicData    mcls_server_front;

	virtual void OnParseComplete( std::string strRecv );

	void OnReceived_NewLotIn( CString strRecv );
	void OnReceived_BufferIn( CString strRecv );
	void OnReceived_BufferReady( CString strRecv );
	void OnReceived_TrayInfoReq( CString strRecv );
	void OnReceived_RecvStateReq( CString strRecv );
	void OnReceived_RecipeInfo( CString strRecv );

	void SetBufferIn( NET_RECEIVED nr ) { m_nrBufferIn = nr; }
	bool GetBufferInReceived() { return (m_nrBufferIn == NR_RECEIVED); }

protected:
	NET_RECEIVED	m_nrBufferIn;
};

extern AServer_Front g_server_front;

#endif // !defined(AFX_ASERVER_FRONT_H__FCB85D24_2210_483A_A8AF_8D443CBDAD77__INCLUDED_)
