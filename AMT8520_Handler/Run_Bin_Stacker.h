// Run_Bin_Stacker.h: interface for the CRun_Bin_Stacker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_BIN_STACKER_H__8075E46C_9CDC_4E48_B268_249CF4FF9CD0__INCLUDED_)
#define AFX_RUN_BIN_STACKER_H__8075E46C_9CDC_4E48_B268_249CF4FF9CD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define BIN_STACKER_CNT		3
#define BIN_STACKER_CNT		4  //ybs

enum ESTEP_RECV_TRAY
{
	RECV_TRAY_NONE,
	RECV_TRAY_READY,
	RECV_TRAY_READY_CHK,
	RECV_TRAY_CONV_ON,
	RECV_TRAY_IN_CHK,
	RECV_TRAY_STOP_CHK,
	RECV_TRAY_CONV_OFF,
	RECV_TRAY_BUFFER_IN_CHK,
	RECV_TRAY_DIRECTION_CHK,
	RECV_TRAY_FINISH,
};

enum ESTEP_TRAY_UP
{
	TRAY_UP_NONE,
	TRAY_UP_READY,
	TRAY_UP_READY_CHK,
	TRAY_UP_ALIGN,
	TRAY_UP_ALIGN_CHK,
	TRAY_UP_ALIGN_BACK,
	TRAY_UP_ALIGN_BACK_CHK,
	TRAY_UP_ELEV_MOVE_PUT_ON,
	TRAY_UP_ELEV_CLAMP_ON,
	TRAY_UP_ELEV_CLAMP_ON_CHK,
	TRAY_UP_ELEV_MOVE_ROTATE,
	TRAY_UP_ELEV_ROTATE,
	TRAY_UP_ELEV_PLACE_READY,
	TRAY_UP_ELEV_PLACE_READY_CHK,
	TRAY_UP_ELEV_MOVE_PLACE_TOP,
	TRAY_UP_ELEV_GUIDE_CLAMP,
	TRAY_UP_ELEV_GUIDE_CLAMP_CHK,
	TRAY_UP_ELEV_MOVE_PLACE_BOTTOM,
	TRAY_UP_ELEV_PUSHER_ON,
	TRAY_UP_ELEV_PUSHER_ON_CHK,
	TRAY_UP_ELEV_BEAT,//2013,0201
	TRAY_UP_ELEV_BEAT_CHK,
	TRAY_UP_ELEV_MOVE_ROTATE_BASE,
	TRAY_UP_ELEV_ROTATE_BASE,
	TRAY_UP_ELEV_MOVE_BASE,
	TRAY_UP_FINISH,
};

enum ESTEP_TRAY_DOWN
{
	TRAY_DOWN_NONE,
	TRAY_DOWN_READY,
	TRAY_DOWN_READY_CHK,

	TRAY_DOWN_ELEV_MOVE_ROTATE,
	TRAY_DOWN_ELEV_ROTATE,
	TRAY_DOWN_ELEV_MOVE_WAIT,

	TRAY_DOWN_BEAT_READY,
	TRAY_DOWN_BEAT_READY_CHK,
	TRAY_DOWN_BEAT,
	TRAY_DOWN_BEAT_CHK,
	
	TRAY_DOWN_ELEV_MOVE_PLACE_TOP,
	TRAY_DOWN_ELEV_CLAMP_GUIDE,
	TRAY_DOWN_ELEV_CLAMP_GUIDE_CHK,
	TRAY_DOWN_ELEV_MOVE_ROTATE_BASE,
	TRAY_DOWN_ELEV_ROTATE_BASE,
	TRAY_DOWN_ELEV_MOVE_PUT_ON,
	TRAY_DOWN_ELEV_CLAMP_OFF,
	TRAY_DOWN_ELEV_CLAMP_OFF_CHK,
	TRAY_DOWN_ELEV_MOVE_BASE,
	TRAY_DOWN_FINISH,
};

enum ESTEP_TRAY_SEND
{
	TRAY_SEND_NONE,
	TRAY_SEND_READY,
	TRAY_SEND_READY_CHK,
	TRAY_SEND_BUFFER_READY,//2013,0910
	TRAY_SEND_NEXT_CHK,
	TRAY_SEND_CONV_ON,
	TRAY_SEND_ARRIVE_CHK,
	TRAY_SEND_ARRIVE_OFF_CHK,
	TRAY_SEND_CONV_MORE,
	TRAY_SEND_CONV_OFF,
	TRAY_SEND_BUFFER_IN,
	TRAY_SEND_BUFFER_IN_CHK,
	TRAY_SEND_FINISH,
};

enum ENUM_BIN_STACKER_INIT
{
	EBSI_START,
	EBSI_CONVEYOR_STOP,
	EBSI_STOPPER_UP,
	EBSI_STOPPER_UP_CHK,
	EBSI_TRAY_ALIGN_BWD,
	EBSI_TRAY_ALIGN_BWD_CHK,
	EBSI_RAIL_GUIDE_BACK,
	EBSI_RAIL_GUIDE_BACK_CHK,
	EBSI_RAIL_PUSHER_BACK,
	EBSI_RAIL_PUSHER_BACK_CHK,
	EBSI_ELEVATOR_UNCLAMP,
	EBSI_ELEVATOR_UNCLAMP_CHK,
	EBSI_HOMECHK_Z,
	EBSI_ELEVATOR_UP_FOR_R,
	EBSI_HOMECHK_R,
	EBSI_ROTATE_BASE,
	EBSI_ELEVATOR_BASE,

	EBSI_FINISH,
};

class CRun_Bin_Stacker : public CObject			// 복구 동작을 위함
{
public:
	DECLARE_SERIAL(CRun_Bin_Stacker);				// 복구 동작을 위함...

	CRun_Bin_Stacker();
	virtual ~CRun_Bin_Stacker();

public:
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//저장해야 하는변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_RunStep;
	int mn_reinstate_step;
	int mn_CokChgStep;
	
	int mn_retry_cnt;
	long ml_retry_wait[3];
	
	long ml_wait_time[3];
	long ml_stop_wait[3];
	//2017.0117
	DWORD m_dwTimeRearPushStart;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// 저장하지 않아도 되는 일반 변수 
	/////////////////////////////////////////////////////////////////////////////////////////////////
	int mn_InitStep;
	
	char cJamcode[10];

	int m_nMotor_Z;
	int m_nMotor_R;

	int m_nSiteID;
	
// Operations
public:
	void Serialize(CArchive &ar);
	void Run_Move();
	void Run_Reinstate();
	void OnBinStacker_FinalPos();
	

	void Run_Initial();
	void Thread_Run();
	void Thread_Variable_Initial();

	void SetMotID( int nZ, int nR ) { m_nMotor_Z = nZ; m_nMotor_R = nR; }
	void SetSiteID( int nVal ) { m_nSiteID = nVal - INIT_BIN_STACKER_1; }

protected:
	void EBSI_Conveyor_Stop();
	void EBSI_Stopper_Up();
	void EBSI_Stopper_Up_Chk();
	void EBSI_Tray_Align_Bwd();
	void EBSI_Tray_Align_Bwd_Chk();
	void EBSI_Rail_Guide_Back();
	void EBSI_Rail_Guide_Back_Chk();
	void EBSI_Rail_Pusher_Back();
	void EBSI_Rail_Pusher_Back_Chk();
	void EBSI_Elevator_Unclamp();
	void EBSI_Elevator_Unclamp_Chk();
	void EBSI_HomeChk_Z();
	void EBSI_Elevator_Up_For_R();
	void EBSI_HomeChk_R();
	void EBSI_Rotate_Base();
	void EBSI_Elevator_Base();
	void EBSI_Finish();

	//Run Move
	void Run_Move_RecvTray();
	void Run_Move_TrayUp();
	void Run_Move_TrayDown();
	void Run_Move_TraySend();
	void Run_Cok_Change();//2013,0213


	void Run_Move_Align();
	void Run_Move_Stopper();
	void Run_Move_Clamp();
	void Run_Move_Guide();
	void Run_Move_Pusher();
	void Run_MoveElvRotateBase();

protected:
	bool Run_Move_Chk_Jam_RecvTray();
	bool Run_Move_Chk_Jam_TrayUp();
	bool Run_Move_Chk_Jam_TrayDown();
	bool Run_Move_Chk_Jam_TraySend();

	bool Run_Move_Chk_Time_RecvTray();
	bool Run_Move_Chk_Time_TrayUp();
	bool Run_Move_Chk_Time_TrayDown();
	bool Run_Move_Chk_Time_TraySend();

	bool InTime_RecvTray( int iMilliSec );
	bool InTime_TrayUp( int iMilliSec );
	bool InTime_TrayDown( int iMilliSec );
	bool InTime_TraySend( int iMilliSec );

	// JAM CHECK
protected:
	bool Chk_Align( int iOn );
	bool Chk_Stopper( int iOn );
	bool Chk_Clamp( int iOn );
	bool Chk_Pusher( int iOn );
	bool Chk_Guide( int iOn );

	bool Chk_Elev_Z( int nPos );
	bool Chk_Elev_R( int nPos );

	bool Chk_Conv_Tray( int iOn );
	bool Chk_Bin_Tray( int iOn );
public:
	void ReqRecvTray();
	bool GetRecvTrayNone() { return (m_step_recvTray == RECV_TRAY_NONE); }
	void SetRecvTrayNone(ESTEP_RECV_TRAY m_recv_tray) { m_step_recvTray = m_recv_tray; }//2013,0503

	void ReqTrayUp();
	bool GetTrayUpNone() { return (m_step_TrayUp == TRAY_UP_NONE); }

	void ReqTrayDown();
	bool GetTrayDownNone() { return (m_step_TrayDown == TRAY_DOWN_NONE); }

	void ReqTraySend();
	bool GetTraySendNone() { return (m_step_TraySend == TRAY_SEND_NONE); }
	void SetTraySendNone(ESTEP_TRAY_SEND m_send_tray) { m_step_TraySend = m_send_tray; }//2013,0503


	void ReqAlign( EFB efb );
	EFB GetAlign() { return m_fbAlign; }

	void ReqStopper( EFB efb );
	EFB GetStopper() { return m_fbStopper; }

	void ReqClamp( EFB efb );
	EFB GetClamp() { return m_fbClamp; }

	void ReqGuide( EFB efb );
	EFB GetGuide() { return m_fbGuide; }

	void ReqPusher( EFB efb );
	EFB GetPusher() { return m_fbPusher; }

	void ReqElvMoveRotateBase( EFB efb );
	EFB GetElvMoveRotateBaser() { return m_fbElvBase; }


	ESTEP_RECV_TRAY		GetStepRecvTray()	{ return m_step_recvTray; }

	bool CalcRearSmema();

protected:
	ESTEP_RECV_TRAY		m_step_recvTray;
	ESTEP_TRAY_UP		m_step_TrayUp;
	ESTEP_TRAY_DOWN		m_step_TrayDown;
	ESTEP_TRAY_SEND		m_step_TraySend;

	ESTEP_RECV_TRAY		m_step_recvTray_old;
	ESTEP_TRAY_UP		m_step_TrayUp_old;
	ESTEP_TRAY_DOWN		m_step_TrayDown_old;
	ESTEP_TRAY_SEND		m_step_TraySend_old;

	DWORD				m_dwTime_RecvTray;
	DWORD				m_dwTime_TrayUp;
	DWORD				m_dwTime_TrayDown;
	DWORD				m_dwTime_TraySend;


	ENUM_STEP_CYL		m_step_align;
	EFB					m_fbAlign;
	DWORD				m_dwTimeAlign;
	DWORD				m_dwTimeAlignStart;

	ENUM_STEP_CYL		m_step_stopper;
	EFB					m_fbStopper;
	DWORD				m_dwTimeStopper;
	DWORD				m_dwTimeStopperStart;

	ENUM_STEP_CYL		m_step_clamp;
	EFB					m_fbClamp;
	DWORD				m_dwTimeClamp;
	DWORD				m_dwTimeClampStart;

	ENUM_STEP_CYL		m_step_guide;
	EFB					m_fbGuide;
	DWORD				m_dwTimeGuide;
	DWORD				m_dwTimeGuideStart;

	ENUM_STEP_CYL		m_step_pusher;
	EFB					m_fbPusher;
	DWORD				m_dwTimePusher;
	DWORD				m_dwTimePusherStart;

	int					m_nDownBeat;
	int					m_nUpBeat;

	ENUM_STEP_CYL		m_step_ElvBase;
	EFB					m_fbElvBase;
};

extern CRun_Bin_Stacker Run_Bin_Stacker[BIN_STACKER_CNT];

#endif // !defined(AFX_RUN_BIN_STACKER_H__8075E46C_9CDC_4E48_B268_249CF4FF9CD0__INCLUDED_)
