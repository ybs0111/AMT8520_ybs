// Run_Network.h: interface for the CRun_Network class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_NETWORK_H__10BD3C05_178B_44C7_B508_2A802C2B580D__INCLUDED_)
#define AFX_RUN_NETWORK_H__10BD3C05_178B_44C7_B508_2A802C2B580D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>


//#define SENDMSG_TYPE_ALL	0xff

#include "SrcBase/ANetwork.h"

class CRun_Network  
{
public:
	
	CRun_Network();
	virtual ~CRun_Network();

	void Thread_Run();

};
extern CRun_Network		Run_Network;

#endif // !defined(AFX_RUN_NETWORK_H__10BD3C05_178B_44C7_B508_2A802C2B580D__INCLUDED_)
