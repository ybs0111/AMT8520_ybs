// AMotor.h: interface for the AMotor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AMOTOR_H__D3DDFFAE_2163_4EDD_B5BE_DAD2BAFDBDA3__INCLUDED_)
#define AFX_AMOTOR_H__D3DDFFAE_2163_4EDD_B5BE_DAD2BAFDBDA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum EMOTSTOP					// enum_Motor_Stop
{
	MOTSTOP_SLOWDOWN,
	MOTSTOP_EMERGENCY,
};

enum EMOTMOVE_MODE				// enum_Motor_Move_Mode
{
	MMMODE_NONE,
	MMMODE_MANUAL,
	MMMODE_DISTANCE,
	MMMODE_HOME,

	MMMODE_WITH_IO_MOVE,
};

enum EMOTRET					// enum_Motor_Result
{
	MOTRET_ERROR = -1,
	MOTRET_PROCEED = 0,
	MOTRET_GOOD,
	MOTRET_RETRY,
	MOTRET_SAFETY,
	//MOTRET_ABORT,
	//MOT_RET_SKIP,
};

enum EMOTMOVE_STEP				//enum_Motor_Move_Step
{
	MMSTEP_NONE,

	// MOVE
	MMSTEP_MOVE_START,
	MMSTEP_MOVE_CHECK,
	MMSTEP_MOVE_FINISH,

	// LINEAR
	MMSTEP_LINEAR_PICKER_UP,
	MMSTEP_LINEAR_PICKER_UP_CHK,
	MMSTEP_LINEAR_SAFETY,
	MMSTEP_LINEAR_SAFETY_CHK,
	MMSTEP_LINEAR_MDL_CALC,
	MMSTEP_LINEAR_MDL_START,
	MMSTEP_LINEAR_MDL_CHK,
	MMSTEP_LINEAR_START,
	MMSTEP_LINEAR_CHK,
	MMSTEP_LINEAR_FINISH,

	// COK_Y
	MMSTEP_COKY_Z_SAFETY,
	MMSTEP_COKY_START,
	MMSTEP_COKY_FINISH,

	// HOME
	MMSTEP_HOME_HOMECHECK,

	// STOP
	MMSTEP_STOP,
	MMSTEP_STOP_FINISH,
	
	// FAIL
	MMSTEP_MOVE_FAIL,
	MMSTEP_STOP_FAIL,
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class AMotor  
{
public:
	AMotor();
	virtual ~AMotor();

protected:
	void MmStep_Move_Start();
	void MmStep_Move_Check();
	void MmStep_Move_Finish();
	EMOTRET MmStep_Home_HomeCheck();
	void MmStep_Stop();
	void MmStep_Stop_Finish();
	void MmStep_Move_Fail();
	void MmStep_Stop_Fail();

	void MmStep_Linear_Safety();
	void MmStep_Linear_Safety_Chk();
	void MmStep_Linear_Mdl_Calc();
	void MmStep_Linear_Mdl_Start();
	void MmStep_Linear_Mdl_Chk();
	void MmStep_Linear_Start();
	void MmStep_Linear_Chk();
	void MmStep_Linear_Finish();

	void _repeat_Finish();
	void _finish_manual();

public:
	void SetMotorID( int iId ) { m_nMotorId = iId; }
	EMOTRET MotorMoving_repeat();

	void SetMotGotoPos( double dVal ) { m_dMotGotoPos = dVal; }
	void MotorStepClear();
	void MotorMoveStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );
	void MotorHomeStart();
	void MotorLinearStart( double dPosX, double dPosY, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );
	void MotorCokYStart( double dPos, double dSpeed, EMOTMOVE_MODE motMoveMode = MMMODE_NONE );

	void SetStopReq( bool bVal ) { m_bStopReq = bVal; }

	EMOTRET JogMoveStart( long lDir );

	static bool CheckTeachSafety( int nAxis, int nPos, double dVal );

protected:
	EMOTMOVE_STEP	m_step;

	bool			m_bStopReq;

	int				m_nMotorId;

	double			m_dMotGotoPos;
	double			m_dMotGotoSpeed;
	
	EMOTMOVE_MODE	m_MotMoveMode;

	double			m_dLinearPos[2];
};

#endif // !defined(AFX_AMOTOR_H__D3DDFFAE_2163_4EDD_B5BE_DAD2BAFDBDA3__INCLUDED_)
