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

	void SetLotStartStatus(CString strLotid, CString strPartid, int nLotQty=0 )
	{
		m_strSLotid = strLotid;
		m_strSPartid = strPartid;
		m_dwTime_GemLotStart_Send = -1;
		m_nCnt_GemLotStart = 0;
		m_nLotQty = nLotQty;
		m_bGemLotStart = true;
	}

	void SetLotEndStatus(CString strLotid, CString strPartid, int nTotCnt=0, int nPassCnt=0, int nFailCnt=0)
	{
		m_strELotid = strLotid;
		m_strEPartid = strPartid;
		/////////////////////////////
		//2017.0602
// 		m_lTacTime = lTacTime;
		m_nTotCnt = nTotCnt;
		m_nPassCnt = nPassCnt;
		m_nFailCnt = nFailCnt;
		/////////////////////////////
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

	////////////////////////////////////////////////////////////////////////////
	//2017.0602
	void SetTrayLoadingStart(CString strLotid, CString strPartid, int nTrayNum )
	{
		m_strSLotid = strLotid;
		m_strSPartid = strPartid;
		m_nTrayStartNum = nTrayNum;
		m_dwTime_TrayLoadingStart = -1;
		m_nCnt_TrayLoadingStart = 0;
		m_bGemTrayLoadingStart = true;
	}
	void SetTrayLoadingEnd(CString strLotid, CString strPartid, int nTrayNum )
	{
		m_strSLotid = strLotid;
		m_strSPartid = strPartid;
		m_nTrayEndNum = nTrayNum;
		m_dwTime_TrayLoadingEnd = -1;
		m_nCnt_TrayLoadingEnd = 0;
		m_bGemTrayLoadingEnd = true;
	}

	void SetTrayMoveLdBufferStart( int nTrayNum, int PocketCnt, int ProductCnt )
	{
		m_nTrayPocketCntBufferStart = nTrayNum;
		m_nPocketCntBufferStart = PocketCnt;
		m_nProductCntBufferStart = ProductCnt;
		m_nCnt_TrayMoveBufferStart = 0;
		m_bGemTrayMoveBufferStart = true;
	}

	void SetTrayMoveLdBufferEnd( int nTrayNum, int PocketCnt, int ProductCnt )
	{
		m_nTrayPocketCntBufferEnd = nTrayNum;
		m_nPocketCntBufferEnd = PocketCnt;
		m_nProductCntBufferEnd = ProductCnt;
		m_nCnt_TrayMoveBufferEnd = 0;
		m_bGemTrayMoveBufferEnd = true;
	}

	//9811 LD Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.
	//void XgemLdRbtPickupDvcStart();//EQPID, TRAYNO, SERIALNO
	void SetLdRbtPickupDVCStart(  int nTrayNum, CString StrSreial )
	{
		m_nCntTrayPickDVCStart = 0;
		m_nTrayNumLdPickDVCStart = nTrayNum;
		m_strTrayLdPickDVCSerailStart = StrSreial;
		m_bGemTrayLdPickStart = true;
	}
	//9812 LD Robot�� ��ǰ�� L/D Buffer�� �̵� �� �÷��ش�.
	//void XgemLdRbtPickupDvcEnd();
	//9813 TestSite Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.

	//9814 TestSite Robot�� ��ǰ�� Socket�� �̵� �� �÷��ش�.
	void SetLdRbtPickupDVCEnd(  int nTrayNum, CString StrSreial )
	{
		m_nCntTrayPickDVCEnd = 0;
		m_nTrayNumLdPickDVCEnd = nTrayNum;
		m_strTrayLdPickDVCSerailEnd = StrSreial;
		m_bGemTrayLdPickEnd = true;
	}
	//9815 Test ���� �� �÷��ش�.
	//void XgemTesterStart();//EQPID, SERIALNO, SocketNo
	void SetDVCTestStart( int nSocketNo, CString StrSreial )
	{
		m_nCntTestStart = 0;
		m_nTestSocketNoStart = nSocketNo;
		m_strTestSerailStart = StrSreial;
		m_bGemTestStart = true;
	}

	void SetDVCTestEnd( int nSocketNo, CString StrSreial )
	{
		m_nCntTestEnd = 0;
		m_nTestSocketNoEnd = nSocketNo;
		m_strTestSerailEnd = StrSreial;
		m_bGemTestEnd = true;
	}

	//9817 TestSite Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.
	//EQPID, TRAYNO SERIALNO, SocketNo
	void SetDVCTestPickStart( int nTrayNum, CString StrSreial, int nSocketNo )
	{
		m_nTestNumPickDVCStart = nTrayNum;
		m_strTestPicktSerailStart = StrSreial;
		m_nTestPickSocketNoStart = nSocketNo;
		m_nCntTestPickDVCStart = 0;
		m_bGemTestPickStart = true;
	}
	//9820 ULD Robot�� ��ǰ�� Tray �Ǵ� Reject,Retest Buffer�� �̵� �� �÷��ش�.
	void SetDVCTestPlaceEnd( int nTrayNum, CString StrSreial ) //EQPID,TRAYNO SERIALNO
	{
		m_nTestNumPlaceDVCEnd = nTrayNum;
		m_strTestPlaceSerailEnd = StrSreial;		
		m_nCntTestPlaceDVCEnd = 0;
		m_bGemTestPlaceEnd = true;
	}
	//9821 Retest Buffer���� ��ǰ Pick up �� �÷��ش�.
	//void XGemLdRbtRetestPickupDVCStart();//EQPID, SERIALNO
	void SetDVCRetestPickStart( CString StrSreial )
	{
		m_strDVCRetestSerailStart = StrSreial;		
		m_nCntDVCRetestPickStart = 0;
		m_bGemDVCRetestPickStart = true;
	}

	void SetDVCRetestResult( int nTrayNum, CString StrSreial, int nSocketNo )
	{
		m_nTestNumRetestResult = nTrayNum;
		m_strTestRetestSerailResult = StrSreial;
		m_nTestRetestkSocketNoResult = nSocketNo;
		m_nCntTestRetestDVCResult = 0;
		m_bGemTestResestStartResult = true;
	}
	//9823 Tray�� UnLoading ���� ��(���� �Ǵ� ����) �÷��ش�.
	//void XGemUldTrayStart();//EQPID, PartNO, LOTNO, TRAYNO
	void SetUldTrayStart( CString StrPartNo, CString StrNotNo, int nTrayNum )
	{
		m_strUldTrayPartNoStart = StrPartNo;		
		m_strUldTrayLotNoStart = StrNotNo;
		m_nUldTrayNumStart = nTrayNum;
		m_nCntUldTrayStart = 0;
		m_bGemUldTrayStart = true;
	}

	void SetUldTrayEnd( CString StrPartNo, CString StrNotNo, int nTrayNum )
	{
		m_strUldTrayPartNoEnd = StrPartNo;		
		m_strUldTrayLotNoEnd = StrNotNo;
		m_nUldTrayNumEnd = nTrayNum;
		m_nCntUldTrayEnd = 0;
		m_bGemUldTrayEnd = true;
	}
	//9825 Socket�� �츱 ��� �÷��ش�.
	//void XGemSocketOn();//EQPID, SokSerialNo
	void SetSocketOn( CString StrSokSerialNo )
	{
		m_strSocketSerialOn = StrSokSerialNo;		
		m_nCntSocketSerialOn = 0;
		m_bGemSocketSerialOn = true;
	}

	void SetSocketOff( CString StrSokSerialNo )
	{
		m_strSocketSerialOff = StrSokSerialNo;		
		m_nCntSocketSerialOff = 0;
		m_bGemSocketSerialOff = true;
	}

	////////////////////////////////////////////////////////////////////////////


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
	////////////////////////////////////////////////////////////////////////////
	//2017.0602
	void XgemTrayLoadingStart();
	void XgemTrayLoadingEnd();
	//Tray�� Barcode Reading Zone���� LD Buffer Zone���� �̵� �� �÷��ش�
	void XgemTrayMoveLdBufferStart();
	//Tray�� LD Buffer Zone���� LD Zone���� �̵� �� �÷��ش�.
	void XgemTrayMoveLdBufferEnd();

	//9811 LD Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.
	void XgemLdRbtPickupDvcStart();//EQPID, TRAYNO, SERIALNO

	//9812 LD Robot�� ��ǰ�� L/D Buffer�� �̵� �� �÷��ش�.//NOT USE

	//9813 TestSite Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.(pickup �� �÷��ش�.)//NOT USE

	//9814 TestSite Robot�� ��ǰ�� Socket�� �̵� �� �÷��ش�.
	void XgemLdRbtPickupDvcEnd();


	//9815 Test ���� �� �÷��ش�.
	void XgemTesterStart();//EQPID, SERIALNO, SocketNo

	//9816 Test �Ϸ� �� �÷��ش�.
	void XgemTesterEnd();

	//9817 TestSite Robot�� ��ǰ�� Pick up ���� �� �÷��ش�.
	//EQPID, TRAYNO SERIALNO, SocketNo
	void XgemUldRbtPickupDvcStart();

	//9818 TestSite Robot�� ��ǰ�� U/D Buffer�� �̵� �� �÷��ش�.

	//9819 ULD Robot�� ��ǰ�� Pick up ���� �� �÷��ش�. incase Retest

	//9820 ULD Robot�� ��ǰ�� Tray �Ǵ� Reject,Retest Buffer�� �̵� �� �÷��ش�.
	void XgemUldRbtPlaceDvcEnd(); //EQPID, TRAYNO SERIALNO


	//9821 Retest Buffer���� ��ǰ Pick up �� �÷��ش�.
	void XGemLdRbtRetestPickupDVCStart();//EQPID, SERIALNO


	//9822 Retest ���� �� ��ǰ�� Pass �Ǵ� Fail �� �÷��ش�.
	void XGemLdRbtRetestResult();//EQPID, TRAYNO, SERIALNO, SOCKETNO

	//9823 Tray�� UnLoading ���� ��(���� �Ǵ� ����) �÷��ش�.
	void XGemUldTrayStart();//EQPID, PartNO, LOTNO, TRAYNO

	//9824 Tray�� UnLoading ���� �Ϸ� ��(���� �Ǵ� ����) �÷��ش�.
	void XGemUldTrayEnd();//EQPID, PartNO, LOTNO, TRAYNO

	//9825 Socket�� �츱 ��� �÷��ش�.
	void XGemSocketOn();//EQPID, SokSerialNo

	//9826 Socket�� ���� ��� �÷��ش�.
	void XGemSocketOff();//EQPID, SokSerialNo



	////////////////////////////////////////////////////////////////////////////

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
	int		m_nLotQty;

	bool	m_bGemLotEnd;
	DWORD	m_dwTime_GemLotEnd_Send;
	int		m_nCnt_GemLotEnd;
	CString	m_strELotid;
	CString m_strEPartid;
	long	m_lTacTime;
	int		m_nTotCnt;
	int		m_nPassCnt;
	int		m_nFailCnt;

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

	////////////////////////////////////////////////////////////////////////////
	//2017.0602
	//9803 eqpid,partno,lotno,trayno
	int		m_nTrayStartNum;
	int		m_nCnt_TrayLoadingStart;
	DWORD	m_dwTime_TrayLoadingStart;
	bool	m_bGemTrayLoadingStart;

	//9804
	int		m_nTrayEndNum;
	int		m_nCnt_TrayLoadingEnd;
	DWORD	m_dwTime_TrayLoadingEnd;
	bool	m_bGemTrayLoadingEnd;

	//9805 //9806 Barcode

	//9809
	int		m_nTrayPocketCntBufferStart;
	int     m_nPocketCntBufferStart;
	int     m_nProductCntBufferStart;
	int		m_nCnt_TrayMoveBufferStart;
	DWORD	m_dwTime_TrayMoveBufferStart;
	bool	m_bGemTrayMoveBufferStart;

	//9810
	int		m_nTrayPocketCntBufferEnd;
	int     m_nPocketCntBufferEnd;
	int     m_nProductCntBufferEnd;
	int		m_nCnt_TrayMoveBufferEnd;
	DWORD	m_dwTime_TrayMoveBufferEnd;
	bool	m_bGemTrayMoveBufferEnd;

	//9811
	int		m_nTrayNumLdPickDVCStart;
	CString	m_strTrayLdPickDVCSerailStart;
	int     m_nCntTrayPickDVCStart;
	DWORD	m_dwTime_TrayLdPickDvcStart;
	bool	m_bGemTrayLdPickStart;

	//9814
	int		m_nTrayNumLdPickDVCEnd;
	CString	m_strTrayLdPickDVCSerailEnd;
	int		m_nCntTrayPickDVCEnd;
	DWORD	m_dwTime_TrayLdPickDvcEnd;
	bool	m_bGemTrayLdPickEnd;

	//9815
	int		m_nTestSocketNoStart;
	CString m_strTestSerailStart;
	int		m_nCntTestStart;
	DWORD	m_dwTime_TestStart;
	bool	m_bGemTestStart;

	//9816
	int		m_nTestSocketNoEnd;
	CString m_strTestSerailEnd;
	int		m_nCntTestEnd;
	DWORD	m_dwTime_TestEnd;
	bool	m_bGemTestEnd;

	//9817
	int		m_nTestNumPickDVCStart;
	CString	m_strTestPicktSerailStart;
	int		m_nTestPickSocketNoStart;
	int		m_nCntTestPickDVCStart;
	DWORD	m_dwTime_PickTestStart;
	bool	m_bGemTestPickStart;

	//9820
	int		m_nTestNumPlaceDVCEnd;
	CString	m_strTestPlaceSerailEnd;
	int		m_nCntTestPlaceDVCEnd;
	DWORD	m_dwTime_PlaceTestEnd;
	bool	m_bGemTestPlaceEnd;

	//9821
	CString	m_strDVCRetestSerailStart;
	int		m_nCntDVCRetestPickStart;
	DWORD	m_dwTime_DVCRetestPickStart;
	bool	m_bGemDVCRetestPickStart;

	//9822
	int		m_nTestNumRetestResult;
	CString	m_strTestRetestSerailResult;
	int		m_nTestRetestkSocketNoResult;
	int		m_nCntTestRetestDVCResult;
	DWORD	m_dwTime_DVCRetestResult;
	bool	m_bGemTestResestStartResult;

	//9823
	CString	m_strUldTrayPartNoStart;
	CString m_strUldTrayLotNoStart;
	int		m_nUldTrayNumStart;
	int		m_nCntUldTrayStart;
	DWORD	m_dwTime_UldTrayStart;
	bool	m_bGemUldTrayStart;

	//9824
	CString	m_strUldTrayPartNoEnd;
	CString m_strUldTrayLotNoEnd;
	int		m_nUldTrayNumEnd;
	int		m_nCntUldTrayEnd;
	DWORD	m_dwTime_UldTrayEnd;
	bool	m_bGemUldTrayEnd;

	//9825
	CString	m_strSocketSerialOn;
	int		m_nCntSocketSerialOn;
	DWORD	m_dwTime_SocketOn;
	bool	m_bGemSocketSerialOn;

	//9826
	CString	m_strSocketSerialOff;
	int		m_nCntSocketSerialOff;
	DWORD	m_dwTime_SocketOff;
	bool	m_bGemSocketSerialOff;

	////////////////////////////////////////////////////////////////////////////


};

extern AClient_Xgem g_client_xgem;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACLIENT_XGEM_H__238D4A82_C5D7_4D2C_9DE6_4BFB62652E38__INCLUDED_)
