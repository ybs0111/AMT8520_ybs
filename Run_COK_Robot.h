// Run_COK_Robot.h: interface for the CRun_COK_Robot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_COK_ROBOT_H__0E28AFD1_AD84_42A3_9920_366EB720FC75__INCLUDED_)
#define AFX_RUN_COK_ROBOT_H__0E28AFD1_AD84_42A3_9920_366EB720FC75__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ENUM_COK_INIT
{
	ECI_START,
	ECI_LOCK,
	ECI_LOCK_CHK,
	ECI_HOMECHK_Z,
	ECI_HOMECHK_Y,
	ECI_FINISH,
};

class CRun_COK_Robot	: public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_COK_Robot);					// 복구 동작을 위함...

	CRun_COK_Robot();
	virtual ~CRun_COK_Robot();

// Attributes
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_RunStep;
	
	int mn_retry_cnt;
	long ml_retry_wait[3];
	
	long ml_wait_time[3];
	long ml_stop_wait[3];
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 저장하지 않아도 되는 일반 변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;
	
	char cJamcode[10];
	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void Run_Initial();
	void Thread_Run();
	void Thread_Variable_Initial();

protected:
	void ECI_Lock();
	void ECI_Lock_Chk();
	void ECI_HomeChk_Z();
	void ECI_HomeChk_Y();
	void ECI_Finish();

	// Run_Move
	void Run_Move_CokLock();


	// Cylinder
	void ReqCokLock( EFB efb );
	EFB GetCokLock() { return m_fbCokLock; }



protected:
	ENUM_STEP_CYL	m_step_cokLock;
	EFB				m_fbCokLock;
	DWORD			m_dwTimeCokLock;
	DWORD			m_dwTimeCokLockStart;






	DWORD m_dwInitTime;

};
extern CRun_COK_Robot Run_COK_Robot;

#endif // !defined(AFX_RUN_COK_ROBOT_H__0E28AFD1_AD84_42A3_9920_366EB720FC75__INCLUDED_)
