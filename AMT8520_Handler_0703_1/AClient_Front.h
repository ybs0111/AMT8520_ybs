// AClient_Front.h: interface for the AClient_Front class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACLIENT_FRONT_H__C3E56278_5C83_4B61_A065_5B63DBD2CF7E__INCLUDED_)
#define AFX_ACLIENT_FRONT_H__C3E56278_5C83_4B61_A065_5B63DBD2CF7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AClient_Front : public ANetwork_Client 
{
public:
	AClient_Front();
	virtual ~AClient_Front();

	virtual void OnParseComplete(std::string strRecv ){}

	void SendTrayInfo();
	void SendRecvState();
};

extern AClient_Front g_client_front;

#endif // !defined(AFX_ACLIENT_FRONT_H__C3E56278_5C83_4B61_A065_5B63DBD2CF7E__INCLUDED_)
