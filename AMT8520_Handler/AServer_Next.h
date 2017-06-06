// AServer_Next.h: interface for the AServer_Next class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASERVER_NEXT_H__4A0E72F0_147C_429B_83BE_A16A7F62DCB1__INCLUDED_)
#define AFX_ASERVER_NEXT_H__4A0E72F0_147C_429B_83BE_A16A7F62DCB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AServer_Next : public ANetwork_Server
{
public:
	AServer_Next();
	virtual ~AServer_Next();

	virtual void OnParseComplete(std::string strRecv );

	void OnReceived_TrayInfo( CString strRecv );
	void OnReceived_RecvState( CString strRecv );
};

extern AServer_Next g_server_next;
#endif // !defined(AFX_ASERVER_NEXT_H__4A0E72F0_147C_429B_83BE_A16A7F62DCB1__INCLUDED_)
