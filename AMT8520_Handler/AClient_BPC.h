// AClient_BPC.h: interface for the AClient_BPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACLIENT_BPC_H__B19FDF76_6C1B_4096_B3ED_14DA771DDFF5__INCLUDED_)
#define AFX_ACLIENT_BPC_H__B19FDF76_6C1B_4096_B3ED_14DA771DDFF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/ANetwork.h"
#include "SrcPart/APartHandler_COK.h"

enum ENUM_USE
{
	EUSE_NONE,
	EUSE_ENABLE,
	EUSE_DISABLE,
};

class AClient_BPC : public ANetwork_Client 
{
public:
	AClient_BPC();
	virtual ~AClient_BPC();

	// Run Move
	virtual void Run_Move();
	void Run_Move_NewLotIn();
	void Run_Move_OnlineEnd();
	void Run_Move_LotCancel();
	void Run_Move_SlotReady();
	void Run_Move_TestStart();
	void Run_Move_TestAbort();
	void Run_Move_SlotPowerOff();
	void Run_Move_SlotStatusChange();
	void Run_Move_TestMode();
	void Run_Move_SocketOnOff();

	// OnReceived
	virtual void OnParseComplete(std::string strRecv );
	void OnReceived_NewLotIn( CString strRecv );
	void OnReceived_OnlineEnd( CString strRecv );
	void OnReceived_LotCancel( CString strRecv );
	void OnReceived_SlotReady( CString strRecv );
	void OnReceived_TestStart( CString strRecv );
	void OnReceived_TestAbort( CString strRecv );
	void OnReceived_SlotPowerOff( CString strRecv );
	void OnReceived_SlotStatusChange( CString strRecv );
	void OnReceived_SlotQuery( CString strRecv );
	void OnReceived_TestModeQuery( CString strRecv );

	// OnSend
	void SetNewLotInID( CString strLotID ) { m_strLotID_NewLotIn = strLotID; }
	void SetOnlineEnd( CString strLotID ) { m_strLotID_OnlineEnd = strLotID; }
	void SetLotCancel( CString strLotID ) { m_strLotID_LotCancel = strLotID; }
	void SetSlotPowerOff( int i, bool bVal ) { m_bSlotPowerOff[i] = bVal; }
	void SetSlotStatusChange( int i, ENUM_USE use ) { m_useSlotStatusChange[i] = use; }
	void SetTestMode( CString strTestMode ) { m_str_TestMode = strTestMode; }
	void CleaeTestMode() {m_str_TestMode = _T("");}
	void SetSocketOnOff( int nSite) { m_bSocketOnOff[nSite] = true; }

	
	// OnSend 1 Time
	void SendSlotQuery();

	void ClearRecvWaitCnt();

protected:
	DWORD	m_dwTime_SockOnOff[TSITE_COK_CNT];
	bool	m_bSocketOnOff[TSITE_COK_CNT];
	
	DWORD	m_dwTime_TestMode;
	CString m_str_TestMode;
	int		m_nCnt_TestMode;

	DWORD	m_dwTime_NewLotIn;
	CString m_strLotID_NewLotIn;
	int		m_nCnt_NewLotIn;

	DWORD	m_dwTime_OnlineEnd;
	CString m_strLotID_OnlineEnd;

	DWORD	m_dwTime_LotCancel;
	CString m_strLotID_LotCancel;

	DWORD	m_dwTime_SlotReady[TSITE_COK_CNT];
	int		m_nRecvWaitCnt_SlotReady[TSITE_COK_CNT];

	DWORD	m_dwTime_TestStart[TSITE_COK_CNT];
	int		m_nRecvWaitCnt_TestStart[TSITE_COK_CNT];

	DWORD	m_dwTime_TestAbort[TSITE_COK_CNT];
	int		m_nRecvWaitCnt_TestAbort[TSITE_COK_CNT];

	DWORD	m_dwTime_SlotPowerOff[TSITE_COK_CNT];
	bool	m_bSlotPowerOff[TSITE_COK_CNT];

	DWORD	m_dwTime_SlotStatusChange[TSITE_COK_CNT];
	ENUM_USE	m_useSlotStatusChange[TSITE_COK_CNT];
};

extern AClient_BPC g_client_bpc;

#endif // !defined(AFX_ACLIENT_BPC_H__B19FDF76_6C1B_4096_B3ED_14DA771DDFF5__INCLUDED_)
