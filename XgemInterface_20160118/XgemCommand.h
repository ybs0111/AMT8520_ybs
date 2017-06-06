// XgemCommand.h: interface for the CXgemCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XGEMCOMMAND_H__3504B0D1_4F11_4CFF_BECD_3E9189E14FB3__INCLUDED_)
#define AFX_XGEMCOMMAND_H__3504B0D1_4F11_4CFF_BECD_3E9189E14FB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XGem.h"

class CXgemCommand  
{
public:
	CString m_strtest;
	CString m_strEqID;
	CString m_strPPID;
	CString m_strPPBody;

	int		m_nProcessState[2];
	int		m_nVid[5000][100][100];
	int		m_nXgemOpenFlag;
	int		m_nXgemStartFlag;
	int		m_nXgemMachine;
	CXGem	*m_hXgem;

public:
	int OnEqPPIDSend(CString strPPID, CString strBody);
	int OnEqPPIDLoad(CString strPPID);
	int OnXgemEvent(int nState);
	int OnXgemMessage(long nObjectID, long nStream, long nFunction, long nSysbyte);
	int OnEqMachineStatus(int nStatus);
	int OnEqMachineRemote();
	int OnEqMachineLocal();
	int OnEqMachineOffLine();
	int OnEqPpidChange(CString strID);
	int OnHostRCMD(long lMsgID, long lReport, CString strRcmd, CString strRecv);
	int OnEqGetCurrentTime(long lMsgID);
	int OnHostSetCurrentTime(long lMsgID, int nRes);
	int OnHostGetCurrentTime();
	int OnEqCEID(CString strRecv);
	int OnEqAlarm(CString strRecv);
	int OnEqMachineOperate(CString strRecv);
	int OnEqCommunication(CXGem *hXgem, CString strRecv);
	CXgemCommand();
	virtual ~CXgemCommand();

};

extern CXgemCommand clsXgem;
#endif // !defined(AFX_XGEMCOMMAND_H__3504B0D1_4F11_4CFF_BECD_3E9189E14FB3__INCLUDED_)
