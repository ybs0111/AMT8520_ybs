// Run_Test_Site.h: interface for the CRun_Test_Site class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_TEST_SITE_H__EA126622_F937_41C3_925A_0824756FE7BD__INCLUDED_)
#define AFX_RUN_TEST_SITE_H__EA126622_F937_41C3_925A_0824756FE7BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define TEST_SITE_CNT	2
#define SITE_COK_CNT	10
#define SITE_SOCKET_CNT	10
#define INSERT_CNT		5
// #define MAX_MODULE		20
#define MAX_MODULE		30//2014,0310

#include "Variable.h"

enum ENUM_TEST_SITE_INIT
{
	ETSI_START,
	ETSI_CHECK_ROBOT,
	ETSI_POGO_BACKWARD,//2012,1225
	ETSI_POGO_BACKWARD_CHK,//2012,1225
	ETSI_PLATE_BACKWARD,
	ETSI_PLATE_BACKWARD_CHK,
	ETSI_COK_INSERT_FORWARD,
	ETSI_COK_INSERT_FORWARD_CHK,
	ETSI_COK_UNLOCK,
	ETSI_COK_UNLOCK_CHK,
	
	ETSI_HOMECHK_Z,

	ETSI_FINISH,
};

enum ENUM_MOVE_Z_MYCOK
{
	EMOVE_Z_NONE,
	EMOVE_Z_CHK,
	EMOVE_Z_BACK_PLATE,
	EMOVE_Z_BACK_PLATE_CHK,
	EMOVE_Z_MOVE,
	EMOVE_FINISH,
};

enum ESTEP_TEST
{
	ETEST_NONE,

	ETEST_MUSTTEST_CHK,
	ETEST_READY,
	ETEST_READY_CHK,
	ETEST_SEND_READY,
	ETEST_RECV_READY,
	ETEST_INSERT_IN,
	ETEST_INSERT_IN_POGO,
	ETEST_INSERT_IN_POGO_CHK,
	ETEST_INSERT_IN_CHK,
	
	ETEST_INSERT_POGO_CHK,//2012,1125
	ETEST_POGO_ON,//2012,1125
	ETEST_POGO_ON_CHK,//2012,1125
	ETEST_TEST_READY_CHK,//2012,1125

	ETEST_SEND_TEST_START,
	ETEST_RECV_TEST_START,
	ETEST_TEST_END_CHK,

	ETEST_TEST_TIMEOUT,
	ETEST_TEST_TIMEOUT_CHK,

	ETEST_TEST_END,

	ETEST_BOARD_POWER_OFF,
	ETEST_BOARD_POWER_OFF_CHK,

	ETEST_POGO_OUT,
	ETEST_POGO_OUT_CHK,
	ETEST_INSERT_OUT,
	ETEST_INSERT_OUT_CHK,
	ETEST_FINISH,
};

class CRun_Test_Site : public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_Test_Site);				// 복구 동작을 위함...
	CRun_Test_Site();
	virtual ~CRun_Test_Site();

	
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_RunStep;
	int mn_reinstate_step[2];
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 저장하지 않아도 되는 일반 변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;
	
	char cJamcode[10];

	int m_nMotID;
	int m_nSiteID;

	CString m_strOldLotID[MAX_MODULE];
	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void Run_Initial();
	void Run_Reinstate();
	void OnRobot_FinalPos();

	void Thread_Run();
	void Thread_Variable_Initial();

	void SetMotID( int nVal ) { m_nMotID = nVal; }
	void SetSiteID( int nVal ) { m_nSiteID = nVal; }

	int GetMotID()	{ return m_nMotID; }

	void ReqInsert( int iIdx, EFB efb );
	EFB GetInsert( int iIdx ) { return m_fbInsert[iIdx]; }

	void ReqLock( EFB efb );
	EFB GetLock() { return m_fbLock; }

	void ReqPlate( EFB efb );
	EFB GetPlate() { return m_fbPlate; }
	
	void ReqPogo( int iIdx, EFB efb );
	EFB GetPogo(int iIdx) { return m_fbPogoOnOff[iIdx]; }


	void ReqMoveToSocket();
	bool IsMovingToSocket() { return (m_step_move != EMOVE_Z_NONE); }

	bool IsPlateForward();

	//2015.0914
	bool isOutStandBy[INSERT_CNT*2];
	bool isSocketOff[INSERT_CNT*2];
	int	mn_Pass[INSERT_CNT*2];
	int	mn_Fail[INSERT_CNT*2];
	int	mn_Input[INSERT_CNT*2];
	bool m_nFlag[20];
	////
protected:
	void ETSI_Check_Robot();
	void ETSI_Check_Pogo_Backward();
	void ETSI_Check_Pogo_Backward_Chk();
	void ETSI_COK_UnLock();
	void ETSI_COK_UnLock_Chk();
	void ETSI_COK_Insert_Forward();
	void ETSI_COK_Insert_Forward_Chk();
	void ETSI_Plate_Backward();
	void ETSI_Plate_Backward_Chk();
	void ETSI_HomeChk_Z();
	void ETSI_Finish();

	// Run_Move
	void Run_Move_Pogo(int iIdx); //2012,1225
	void Run_Move_Insert(int iIdx);
	void Run_Move_Lock();

	void Run_Move_Plate();

	void Run_Move_Test( int iIdx );
	void Run_Move_ToSocket();

	// Func
	void SetInsert(int iIdx, int iOnOff );
	bool ChkInsert(int iIdx, int iOnOff );
	void SetPogoOnOff( int iIdx, int iOnOff );
	bool ChkPogoOnOff( int nSite, int iIdx );

protected:
	bool Run_Move_Test_JamChk( int iIdx );
	bool Run_Move_ToSocket_JamChk();

	bool Run_Move_ToSocket_TimeChk();

	bool InTime_ToSocket( int iMilliSec );

public:
	//2012,1225
	bool Chk_PogoOnOff( int nSite, int iIdx, int iOn );
	bool Chk_Insert( int nSite, int iIdx, int iOn );
	bool Chk_Plate( int nSite, int iOn );
	bool Chk_Lock( int nSite, int iOn );
	bool Chk_HifixClamp( int nSite, int iOn );

	bool Chk_Robot_Safety();

	//2013,0924
	void SetRejAlarm( bool bVal ) { m_bRejAlarm = bVal; }
	bool GetRejAlarm() { return m_bRejAlarm; }
	void ClearRejAlarm() { if( m_bRejAlarm ) m_bRejAlarm = false; }


protected:
	//2012,1225
	ENUM_STEP_CYL	m_step_pogo[INSERT_CNT];
	EFB				m_fbPogoOnOff[INSERT_CNT];
	DWORD			m_dwTimePogo[INSERT_CNT];
	DWORD			m_dwTimePogoStart[INSERT_CNT];

	ENUM_STEP_CYL	m_step_insert[INSERT_CNT];
	EFB				m_fbInsert[INSERT_CNT];
	DWORD			m_dwTimeInsert[INSERT_CNT];
	DWORD			m_dwTimeInsertStart[INSERT_CNT];

	ENUM_STEP_CYL	m_step_lock;
	EFB				m_fbLock;
	DWORD			m_dwTimeLock;
	DWORD			m_dwTimeLockStart;

	ENUM_STEP_CYL	m_step_plate;
	EFB				m_fbPlate;
	DWORD			m_dwTimePlate;
	DWORD			m_dwTimePlateStart;

	ENUM_MOVE_Z_MYCOK	m_step_move;
	ENUM_MOVE_Z_MYCOK	m_step_move_old;
	DWORD				m_dwTime_Move;


	ESTEP_TEST		m_step_test[INSERT_CNT];

	// Time
	
	DWORD			m_dwTimeInit;
	DWORD			m_dwTimeTestEnd[INSERT_CNT];
	bool			m_bRejAlarm;

public:
	DWORD			m_dwTimeStartTest[SITE_SOCKET_CNT];
	DWORD			m_dwTimeStartModule[SITE_SOCKET_CNT];
};

extern CRun_Test_Site Run_Test_Site[TEST_SITE_CNT];

#endif // !defined(AFX_RUN_TEST_SITE_H__EA126622_F937_41C3_925A_0824756FE7BD__INCLUDED_)
