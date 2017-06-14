// AClient_EC.h: interface for the AClient_EC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACLIENT_EC_H__D7836B27_6E6F_4A80_8E8E_B78E7FFED9DC__INCLUDED_)
#define AFX_ACLIENT_EC_H__D7836B27_6E6F_4A80_8E8E_B78E7FFED9DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"

class AClient_EC : public ANetwork_Client 
{
public:
	AClient_EC();
	virtual ~AClient_EC();

	virtual void Run_Move();
	void Run_Move_TestEnd();
	void Run_Move_BufferIn();
	void Run_Move_AlarmReport();
	void Run_Move_VersionUpdate();
	void Run_Move_ReqEmptyTray();
	void Run_Move_SendTool();

	virtual void OnParseComplete(std::string strRecv );
	void OnReceived_TestEnd( CString strRecv );
	void OnReceived_BufferIn( CString strRecv );
	void OnReceived_AlarmReport( CString strRecv );
	void OnReceived_VersionUpdate( CString strRecv );
	void OnReceived_ReqEmptrTray( CString strRecv );
	void OnReceived_SendToolChange( CString strRecv );



	void OnTestEnd( CString strLotID, CString strPartID ) { m_strLotID_TestEnd = strLotID; m_strPartID_TestEnd = strPartID; }
	void OnBufferIn( CString strBufferIn ) { m_strBufferIn = strBufferIn; }
	void OnAlarmReport( CString strAlarmCode ) { m_strAlarmReport = strAlarmCode; }
	void OnVersionUpdate( bool bVal ) { m_bVersionUpdate = bVal; }
	void OnReqEmptyTray(bool bVal) { m_bReqEmptyTray = bVal; }
	bool GetReqEmptyTray() { return m_bReqEmptyTray; }
	void OnSendToolInform(bool bVal) { m_bToolInform = bVal; }
	bool GetToolInform() { return m_bToolInform; }

protected:
	DWORD	m_dwTime_TestEnd;
	CString m_strLotID_TestEnd;
	CString m_strPartID_TestEnd;

	DWORD	m_dwTime_BufferIn;
	CString m_strBufferIn;

	DWORD	m_dwTime_AlarmReport;
	CString m_strAlarmReport;

	DWORD	m_dwTime_VersionUpdate;
	bool	m_bVersionUpdate;

	bool	m_bReqEmptyTray;
	DWORD	m_dwTime_ReqEmptyTray;
	int		m_nCnt_ReqEmpty;
	
	bool	m_bToolInform;
	DWORD	m_dwTime_ToolInform;
	int		m_nCnt_ToolInform;


};

extern AClient_EC g_client_ec;

#endif // !defined(AFX_ACLIENT_EC_H__D7836B27_6E6F_4A80_8E8E_B78E7FFED9DC__INCLUDED_)
