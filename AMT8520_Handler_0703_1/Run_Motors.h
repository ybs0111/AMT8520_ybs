// Run_Motors.h: interface for the CRun_Motors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_MOTORS_H__F00E63EC_0E48_4D89_87BB_D6B1D190F5F9__INCLUDED_)
#define AFX_RUN_MOTORS_H__F00E63EC_0E48_4D89_87BB_D6B1D190F5F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SrcBase/AMotor.h"
#include "COMI_Manager.h"

#define GETMOTOR( motorID )		g_Run_Motors.GetMotor( motorID )

class CRun_Motors  
{
public:
	CRun_Motors();
	virtual ~CRun_Motors();

public:
	void Thread_Run();
	AMotor& GetMotor( ENUM_MOTOR motorID ) { return m_motor[motorID]; }

protected:
	AMotor	m_motor[MOTOR_COUNT];
};

extern CRun_Motors	g_Run_Motors;

#endif // !defined(AFX_RUN_MOTORS_H__F00E63EC_0E48_4D89_87BB_D6B1D190F5F9__INCLUDED_)
