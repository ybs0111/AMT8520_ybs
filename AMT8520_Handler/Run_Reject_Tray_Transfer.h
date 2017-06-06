// Run_Reject_Tray_Transfer.h: interface for the CRun_Reject_Tray_Transfer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_REJECT_TRAY_TRANSFER_H__4E54AE3D_95B5_46A8_A04A_8C937F619B96__INCLUDED_)
#define AFX_RUN_REJECT_TRAY_TRANSFER_H__4E54AE3D_95B5_46A8_A04A_8C937F619B96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ENUM_TRANSFER_INIT
{
	ETI_START,
	ETI_ROBOT_CHK,
	ETI_Z_UP,
	ETI_Z_UP_CHK,
	ETI_BIN_STACKER_BACKWARD,
	ETI_BIN_STACKER_BACKWARD_CHK,
	ETI_HOMECHK_CLAMP,
	ETI_HOMECHK_Y,
	ETI_HOMECHK_CLAMP1,
	ETI_CLAMP_SAFETY,
	ETI_BIN_STACKER_RAIL,
	ETI_BIN_STACKER_RAIL_CHK,

	ETI_FINISH,
};

enum ETTRANS_TO_REJECT
{
	ETT_REJ_NONE,
	ETT_REJ_READY,
	ETT_REJ_ROBOT_CHK,
	ETT_REJ_ROBOT_MOVE,
	ETT_REJ_ROBOT_MOVE_CHK,
	ETT_REJ_UNCLAMP,
	ETT_REJ_READY_UP,
	ETT_REJ_READY_UP_CHK,
	//ETT_REJ_STACKER,

	ETT_REJ_STACKER_GUIDE_BACK,
	ETT_REJ_STACKER_GUIDE_BACK_CHK,
	ETT_REJ_STACKER_CHK,
	
	ETT_REJ_STACKER_MOVE,

	ETT_REJ_PICK_MOVE_Y,
	ETT_REJ_PICK_DOWN,
	ETT_REJ_PICK_DOWN_CHK,
	ETT_REJ_PICK_CLAMP,
	ETT_REJ_PICK_CLAMP_CHK,
	ETT_REJ_PICK_UP,
	ETT_REJ_PICK_UP_CHK,
	ETT_REJ_PLACE_MOVE_Y,
	ETT_REJ_PLACE_DOWN,
	ETT_REJ_PLACE_DOWN_CHK,
	ETT_REJ_PLACE_UNCLAMP,
	ETT_REJ_PLACE_UP,
	ETT_REJ_PLACE_UP_CHK,
	ETT_REJ_PLACE_Y_BIN,
	ETT_REJ_PLACE_CLAMP_SAFETY,

	ETT_REJ_MOVE_WORK,

	ETT_REJ_STACKER_GUIDE_FORWARD,
	ETT_REJ_STACKER_GUIDE_FORWARD_CHK,

	ETT_REJ_FINISH,
};

enum ETTRANS_TO_BIN3
{
	ETT_BIN_NONE,
	ETT_BIN_READY,
	ETT_BIN_ROBOT_CHK,
	ETT_BIN_ROBOT_MOVE,
	ETT_BIN_ROBOT_MOVE_CHK,
	ETT_BIN_UNCLAMP,
	ETT_BIN_READY_UP,
	ETT_BIN_READY_UP_CHK,
	//ETT_BIN_STACKER,

	ETT_BIN_STACKER_GUIDE_BACK,
	ETT_BIN_STACKER_GUIDE_BACK_CHK,
	ETT_BIN_STACKER_CHK,
	
	ETT_BIN_STACKER_MOVE,

	ETT_BIN_PICK_MOVE_Y,
	ETT_BIN_PICK_DOWN,
	ETT_BIN_PICK_DOWN_CHK,
	ETT_BIN_CLAMP_HOME,
	ETT_BIN_PICK_CLAMP,
	ETT_BIN_PICK_CLAMP_CHK,
	ETT_BIN_PICK_UP,
	ETT_BIN_PICK_UP_CHK,
	ETT_BIN_STACKER_GUIDE_FORWARD,     //
	ETT_BIN_STACKER_GUIDE_FORWARD_CHK,  //
	ETT_BIN_PLACE_MOVE_Y,
	ETT_BIN_PLACE_GUIDE,
	ETT_BIN_PLACE_GUIDE_CHK,
	ETT_BIN_PLACE_DOWN,
	ETT_BIN_PLACE_DOWN_CHK,
	ETT_BIN_PLACE_UNCLAMP,
	ETT_BIN_PLACE_UP,
	ETT_BIN_PLACE_UP_CHK,
	ETT_BIN_PLACE_CLAMP_SAFETY,
	
	ETT_BIN_REJ_DOWN,

	ETT_BIN_FINISH,
};

enum ETTRANS_TO_SAFETY
{
	ETT_SAFETY_NONE,
	ETT_SAFETY_READY,
	ETT_SAFETY_ROBOT_CHK,
	ETT_SAFETY_ROBOT_MOVE,
	ETT_SAFETY_ROBOT_MOVE_CHK,
	ETT_SAFETY_UNCLAMP,
	ETT_SAFETY_READY_UP,
	ETT_SAFETY_READY_UP_CHK,
	//ETT_SAFETY_STACKER,

	ETT_SAFETY_STACKER_GUIDE_BACK,
	ETT_SAFETY_STACKER_GUIDE_BACK_CHK,
	ETT_SAFETY_STACKER_CHK,
	
	ETT_SAFETY_STACKER_MOVE,
	ETT_SAFETY_CLAMP_SAFETY,

	ETT_SAFETY_MOVE_Y,
	ETT_SAFETY_FINISH,
};

class CRun_Reject_Tray_Transfer	: public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_Reject_Tray_Transfer);				// 복구 동작을 위함...

	CRun_Reject_Tray_Transfer();
	virtual ~CRun_Reject_Tray_Transfer();

// Attributes
public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_RunStep;
	int mn_reinstate_step;
	
	int mn_retry_cnt;
	long ml_retry_wait[3];
	
	long ml_wait_time[3];
	long ml_stop_wait[3];

	long ml_clamp_wait_time[3];
	int mn_clamp_retry_cnt;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 저장하지 않아도 되는 일반 변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;
	
	char cJamcode[10];

	bool m_bInitVarBackward;

	double m_dcurr_pos[3];
	int	mn_reinstate_pos[3];	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void OnRobot_FinalPos();
	void Run_Reinstate();
	void Run_Initial();
	void Thread_Run();
	void Thread_Variable_Initial();
	BOOL Bin_Stacker_Rail();//2012,1225

protected:
	void ETI_Robot_Chk();
	void ETI_HomeChk_Clamp();
	void ETI_HomeChk_Clamp1();
	void ETI_Z_Up();
	void ETI_Z_Up_Chk();
	void ETI_Bin_Stacker_Backward();
	void ETI_Bin_Stacker_Backward_Chk();
	void ETI_HomeChk_Y();
	void ETI_Bin_Stacker_Rail();
	void ETI_Bin_Stacker_Rail_Chk();
	void ETI_Finish();


	// Run Move
	void Run_Move_To_Rej();
	void Run_Move_To_Bin();
	void Run_Move_Transfer_To_Rej();//ybs
	void Run_Move_Transfer_To_Bin();//ybs
	void Run_Move_Transfer_To_Safety();

	void Run_Move_Cyl_Z();
	void Run_Move_Rail();

protected:
	bool Run_Move_To_Rej_ChkJam();
	bool Run_Move_To_Bin_ChkJam();
	bool Run_Move_Transfer_To_Rej_ChkJam();//ybs
	bool Run_Move_Transfer_To_Bin_ChkJam();//ybs
	bool Run_Move_Transfer_To_Safety_ChkJam();

	bool Run_Move_To_Rej_ChkTime();
	bool Run_Move_To_Bin_ChkTime();
	bool Run_Move_Transfer_To_Rej_ChkTime();//ybs
	bool Run_Move_Transfer_To_Bin_ChkTime();//ybs
	bool Run_Move_Transfer_To_Safety_ChkTime();//ybs

	bool InTime_ToRej( int iMilliSec );
	bool InTime_ToBin( int iMilliSec );
	bool InTime_Transfer_ToRej( int iMilliSec );
	bool InTime_Transfer_ToBin( int iMilliSec );
	bool InTime_Transfer_ToSafety( int iMilliSec );


protected:
	bool Chk_Transfer_Y( int nPos );
	bool Chk_Transfer_Dn( int iOn );
	bool Chk_Guide( int iOn );
	bool Chk_TrayExist( int iOn );
	bool Chk_TransferTray( int iOn );
	bool Chk_Clamp( int nPos );
	bool Chk_Reject_Z( bool bAfterPlace );
	

public:
	void ReqMoveToRej();
	bool GetMoveToRej() { return (m_step_to_rej == ETT_REJ_NONE); }

	void ReqMoveToBin();
	bool GetMoveToBin() { return (m_step_to_bin == ETT_BIN_NONE); }

	void ReqMoveTransferToRej();
	bool GetMoveTransferToRej() { return (m_step_transfer_to_rej == ETT_REJ_NONE); }

	void ReqMoveTransferToBin();
	bool GetMoveTransferToBin() { return (m_step_transfer_to_bin == ETT_BIN_NONE); }

	void ReqMoveTransferToSafety();
	bool GetMoveTransferToSafety() { return (m_step_transfer_to_safety == ETT_SAFETY_NONE); }

	void ReqCylZ( EFB efb );
	EFB GetCylZ() { return m_fbCylZ; }

	void ReqRail( EFB efb );
	EFB GetRail() { return m_fbRail; }

	ETTRANS_TO_REJECT	m_step_to_rej;
	ETTRANS_TO_BIN3		m_step_to_bin;


	ETTRANS_TO_REJECT	m_step_to_rej_old;
	ETTRANS_TO_BIN3		m_step_to_bin_old;

	ETTRANS_TO_REJECT	m_step_transfer_to_rej;//ybs
	ETTRANS_TO_BIN3		m_step_transfer_to_bin;
	ETTRANS_TO_SAFETY	m_step_transfer_to_safety;

	ETTRANS_TO_REJECT	m_step_transfer_to_rej_old;//ybs
	ETTRANS_TO_BIN3		m_step_transfer_to_bin_old;
	ETTRANS_TO_SAFETY	m_step_transfer_to_safety_old;

	DWORD				m_dwTime_ToRej;
	DWORD				m_dwTime_ToBin;
	DWORD				m_dwTime_transfer_ToRej;//ybs
	DWORD				m_dwTime_transfer_ToBin;//ybs
	DWORD				m_dwTime_transfer_ToSafety;
protected:
	

	ENUM_STEP_CYL		m_step_cylz;
	EFB					m_fbCylZ;
	DWORD				m_dwTimeZ;
	DWORD				m_dwTimeZStart;

	ENUM_STEP_CYL		m_step_rail;
	EFB					m_fbRail;
	DWORD				m_dwTimeRail;
	DWORD				m_dwTimeRailStart;

};
extern CRun_Reject_Tray_Transfer Run_Reject_Tray_Transfer;

#endif // !defined(AFX_RUN_REJECT_TRAY_TRANSFER_H__4E54AE3D_95B5_46A8_A04A_8C937F619B96__INCLUDED_)
