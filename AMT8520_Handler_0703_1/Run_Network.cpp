// Run_Network.cpp: implementation of the CRun_Network class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "handler.h"
#include "Run_Network.h"

#include "Public_Function.h"
#include "AVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "SrcPart/APartNetworkManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRun_Network		Run_Network;

CRun_Network::CRun_Network()
{
}

CRun_Network::~CRun_Network()
{

}

void CRun_Network::Thread_Run()
{	
	g_Net.Run_Move();
}