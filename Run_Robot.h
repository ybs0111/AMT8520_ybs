// Run_Robot.h: interface for the CRun_Robot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_ROBOT_H__6C27D025_EA53_491D_8DD8_E23D102F4963__INCLUDED_)
#define AFX_RUN_ROBOT_H__6C27D025_EA53_491D_8DD8_E23D102F4963__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Variable.h"
#include "SrcPart/APartRobot.h"

#define VACUUM_CNT	4


enum ENUM_ROBOT_INIT
{
	ERI_START,
	ERI_VACUUM_OFF,
	ERI_EJECTOR_ON,
	ERI_EJECTOR_OFF,
	ERI_MODULE_EMPTY_CHK,
	ERI_PICKER_VACUUM_UP,
	ERI_PICKER_VACUUM_UP_CHK,
	ERI_PICKER_OFF,
	ERI_PICKER_OFF_CHK,
	ERI_HOMECHK_Z,
	ERI_HOMECHK_P,
	ERI_MOVECHK_P,
	ERI_HOMECHK_ZP,
	ERI_MOVECHK_ZP,
	ERI_HOMECHK_X,
	ERI_HOMECHK_Y,

	ERI_FINISH,
};

class CRun_Robot	: public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_Robot);					// 복구 동작을 위함...

	CRun_Robot();
	virtual ~CRun_Robot();

// Attributes
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_RunStep;
	int mn_home_x;
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 저장하지 않아도 되는 일반 변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;
	int mn_reinstate_step;
	int mn_safetyzone_step;
	int mn_safetyzoneY_step;

	double mdSpeedRatio[3];
	double md_motor_pos[2];

	bool m_bInitSafety;
	bool m_bInitXHome;

	DWORD m_dwTimeInit;
	DWORD m_dwTimeMoveX;
	DWORD m_dwTimeMoveY;

	double ml_picker_wait_time[3];
	int mn_retry;
	int md_MotFlag[2];


	double m_dMdl[2];
	bool m_bMoveStartChk;

	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void Run_Reinstate();
	void Run_Initial();
	void Thread_Run();
	void Thread_Variable_Initial();
	void OnRobot_FinalPos();
	int Robot_AutoMove_Safety_Zone(int n_mode, int n_site, int n_flag);


	void ReqGripperDown( EFB efb );
	EFB GetGripperDown() { return m_fbGripDown; }

	void ReqGripperOn( EFB efb );
	EFB GetGripperOn() { return m_fbGripOn; }

	void ReqVacDown( int iIdx, EFB efb );
	EFB GetVacDown( int iIdx ) { return m_fbVacDown[iIdx]; }

	void SetCokID( int iID );
	int GetCokID() { return m_iCokId; }


protected:
	void ERI_Vacuum_Off();
	void ERI_ModuleEmptyChk();
	void ERI_PickerVacuumUp();
	void ERI_PickerVacuumUp_Chk();
	void ERI_PickerOff();
	void ERI_PickerOff_Chk();
	void ERI_HomeChk_Z();
	void ERI_HomeChk_P();
	void ERI_MoveChk_P();
	void ERI_HomeChk_ZP();
	void ERI_MoveChk_ZP();
	void ERI_HomeChk_X();
	void ERI_HomeChk_Y();
	void ERI_Finish();


protected:
	void Run_Move_Grip_Down();
	void Run_Move_Grip_On();
	void Run_Move_Vac_Down( int iIdx );

	bool Run_Move_Chk_Jam( stJobPicker job );

	bool Run_Move_Chk_Time();
	bool Run_Move_Chk_Time(stJobPicker job);
	bool InTimeRobot( int iMilliSec );


	// JAM CHECK
public:
	bool Chk_Robot_Z_Safety();
	bool Chk_Robot_XY_Pos();
	bool Chk_Robot_Z_Pos();
	bool Chk_Robot_GripperDn( int iOn );
	bool Chk_Robot_GripperPick( int iOn );
	bool Chk_Robot_PickerDn( int iIdx, int iOn );
	bool Chk_Robot_TS_Plate( stJobPicker job, int iOn );
	bool Chk_Robot_Insert( stJobPicker job, int iOn );
	bool Chk_Robot_TS_Lock( stJobPicker job, int iOn );
	bool Chk_Robot_HF_Clamp( stJobPicker job, int iOn );
	bool Chk_Robot_Cok( stJobPicker job, int iOn );

	bool Chk_Robot_Guide( stJobPicker job, int iOn );
	bool Chk_Robot_Pusher( stJobPicker job, int iOn );
	bool Chk_Robot_Tray( stJobPicker job, int iOn );
	bool Chk_Robot_Rej_Z_Pos( stJobPicker job );
	bool Chk_Robot_Module( stJobPicker job, int iOn );
	//2016.0326
	int Robot_AutoMoveY_Safety_Zone(int n_mode, int n_site, int n_flag);


	CString			m_str_msg;


protected:
	ENUM_STEP_CYL	m_step_grip_down;
	EFB				m_fbGripDown;
	DWORD			m_dwTimeGripDown;
	DWORD			m_dwTimeGripDownStart;

	ENUM_STEP_CYL	m_step_grip_on;
	EFB				m_fbGripOn;
	DWORD			m_dwTimeGripOn;
	DWORD			m_dwTimeGripOnStart;

	ENUM_STEP_CYL	m_step_vac_down[VACUUM_CNT];
	EFB				m_fbVacDown[VACUUM_CNT];
	DWORD			m_dwTimeVacDown[VACUUM_CNT];
	DWORD			m_dwTimeVacDownStart[VACUUM_CNT];

	double			m_dPos[3];
	double			m_dcurr_pos[3];
	int				mn_reinstate_pos[3];

	double			m_zPos;
	double			m_zPlace;
	double			m_zPickup;

	PICKER_STEP		picker_step_old;
	DWORD			m_dwTime_Robot;

protected:
	int				m_iCokId;
};
extern CRun_Robot Run_Robot;

#endif // !defined(AFX_RUN_ROBOT_H__6C27D025_EA53_491D_8DD8_E23D102F4963__INCLUDED_)
