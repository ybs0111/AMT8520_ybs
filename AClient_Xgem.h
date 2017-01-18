#if !defined(AFX_ACLIENT_XGEM_H__238D4A82_C5D7_4D2C_9DE6_4BFB62652E38__INCLUDED_)
#define AFX_ACLIENT_XGEM_H__238D4A82_C5D7_4D2C_9DE6_4BFB62652E38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AClient_Xgem.h : header file
//

#include "SrcBase/ANetwork.h"

/////////////////////////////////////////////////////////////////////////////
// AClient_Xgem command target
#include "XGem.h"

class AClient_Xgem : public ANetwork_Client
{
public:
	AClient_Xgem();           // protected constructor used by dynamic creation
	virtual ~AClient_Xgem();

	virtual void Run_Move();
	virtual void OnParseComplete(std::string strRecv );

public:
	int		m_nXgemOpenFlag;
	int		m_nXgemStartFlag;
	int		m_nXgemMachine;

	CXGem	*m_hXgem;
	CWnd	*m_hXgemInterfaceDlg;

public:
	void SetGemInit() { m_nCnt_GemInit = 0; m_bGemInit = true; }
	void SetGemState(int nMode) { m_nCnt_GemState = 0; m_bGemState[nMode] = true; }
	void SetGemAlarmState(CString strAlarmCode, int nState) 
	{ 
		m_nCnt_GemAlarmState = 0;
		m_strAlarmCode = strAlarmCode; 
		m_nAlarmState = nState;
		m_bGemAlarmState = true; 
	}
	void SetMachineStatus(int nMode) 
	{
		m_nCnt_GemMachineState = 0;
		m_nGemMachineState = nMode;
		m_bGemMachineState = true; 
	}

	void SetLotStartStatus(CString strLotid, CString strPartid)
	{
		m_strSLotid = strLotid;
		m_strSPartid = strPartid;
		m_dwTime_GemLotStart_Send = -1;
		m_nCnt_GemLotStart = 0;
		m_bGemLotStart = true;
	}

	void SetLotEndStatus(CString strLotid, CString strPartid)
	{
		m_strELotid = strLotid;
		m_strEPartid = strPartid;
		m_dwTime_GemLotEnd_Send = -1;
		m_nCnt_GemLotEnd = 0;
		m_bGemLotEnd = true;
	}

	void SetPPID()
	{
		m_dwTime_GemPPid_Send = -1;
		m_nCnt_GemPPid = 0;
		m_bGemPPid = true;
	}

	void SetBufferInInfo()
	{
		m_dwTime_GemBufferIn_Send = -1;
		m_nCnt_GemBufferin = 0;
		m_bGemBufferin = true;
	}

	void SetSetOnSocketStatus(CString strSerial, int xpos, int ypos)
	{
		m_strOnSokSerial = (LPCTSTR) strSerial;
		m_nXpos = xpos;
		m_nYpos = ypos;
		m_dwTime_GemSokOn_Send = -1;
		m_nCnt_GemSokOn = 0;
		m_bGemSokOn = true;
	}

	void SetSetOffSocketStatus(CString strSerial, int xpos, int ypos)
	{
		m_strOffSokSerial = (LPCTSTR) strSerial;
		m_nXpos = xpos;
		m_nYpos = ypos;
		m_dwTime_GemSokOff_Send = -1;
		m_nCnt_GemSokOff = 0;
		m_bGemSokOff = true;
	}

	void XgemMachineStatus();
	void XgemAlarmStatus();
	void XgemEqStatus(int nMode);
	void XgemInitialize();
	void XgemLotStart();
	void XgemLotEnd();
	void XgemSendPPID();
	void XgemSendBufferIn();
	void XgemSendOnSokStatus();
	void XgemSendOffSokStatus();

	void OnReceived_XgemInitialize( CString strRecv );
	void OnReceived_XgemEqStatus( CString strRecv );
	void OnReceived_XgemAlarmStatus( CString strRecv );
	void OnReceived_XgemRunStatus( CString strRecv );
	void OnReceived_XgemAnalysis( CString strRecv );
	void OnReceived_XgemPPIDChange( CString strRecv );
	void OnReceived_XgemRCMDChange( CString strRecv);
	CString OnEqToXgemFunction(CString strFind, CString strRecv);
	CString OnEqToXgemCommand(CString strRecv);



protected:
	CString m_strEqid;

	bool	m_bGemInit;
	DWORD	m_dwTime_GemInit_Send;
	int		m_nCnt_GemInit;
	
	bool	m_bGemState[3];
	DWORD	m_dwTime_GemState_Send;
	int		m_nCnt_GemState;

	CString m_strAlarmCode;
	int		m_nAlarmState;
	bool	m_bGemAlarmState;
	int		m_nCnt_GemAlarmState;
	DWORD	m_dwTime_GemAlarmState_Send;

	bool	m_bGemMachineState;
	int		m_nGemMachineState;
	DWORD	m_dwTime_GemMachineState_Send;
	int		m_nCnt_GemMachineState;

	bool	m_bGemLotStart;
	DWORD	m_dwTime_GemLotStart_Send;
	int		m_nCnt_GemLotStart;
	CString	m_strSLotid;
	CString m_strSPartid;

	bool	m_bGemLotEnd;
	DWORD	m_dwTime_GemLotEnd_Send;
	int		m_nCnt_GemLotEnd;
	CString	m_strELotid;
	CString m_strEPartid;

	bool	m_bGemPPid;
	DWORD	m_dwTime_GemPPid_Send;
	int		m_nCnt_GemPPid;
	CString	m_strPPid;

	bool	m_bGemBufferin;
	DWORD	m_dwTime_GemBufferIn_Send;
	int		m_nCnt_GemBufferin;

	bool	m_bGemSokOn;
	DWORD	m_dwTime_GemSokOn_Send;
	int		m_nCnt_GemSokOn;
	CString	m_strOnSokSerial;

	bool	m_bGemSokOff;
	DWORD	m_dwTime_GemSokOff_Send;
	int		m_nCnt_GemSokOff;
	CString	m_strOffSokSerial;
	int		m_nXpos;
	int		m_nYpos;
};

extern AClient_Xgem g_client_xgem;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACLIENT_XGEM_H__238D4A82_C5D7_4D2C_9DE6_4BFB62652E38__INCLUDED_)
