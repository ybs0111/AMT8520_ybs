// Run_Handler_Check.h: interface for the CRun_Handler_Check class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUN_HANDLER_CHECK_H__C7C7B3DE_3046_4089_8C04_0D4F36A1D947__INCLUDED_)
#define AFX_RUN_HANDLER_CHECK_H__C7C7B3DE_3046_4089_8C04_0D4F36A1D947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRun_Handler_Check  
{
public:
	CRun_Handler_Check();
	virtual ~CRun_Handler_Check();

// Attributes
public:
	int StartStep;
	int StopStep;
	int LoaderStep;
	int AlaemClearStep;
	int BuzzerStep;
	
	int MachineResetStep;
	
	int DoorStep;
	int AreaStep;
	
	long SwitchWaitTime[3];
	
	int n_emo_chk;
	int n_air_chk;

	int n_light_curtain_chk;

	char cJamcode[10];

	int n_front_key;
	int n_rear_key;
	int n_menu;
	int n_status;

	BOOL m_bJog_Up_old;
	BOOL m_bJog_Dn_old;
	
	int i_emo_check_flag;
	long EMO_CheckTime[3];

	DWORD mdTimeXgem;

// Operations
public:
	void ButtonCheck_ManualAutoKey();
	void ButtonCheck_Alarm_Clear();
	void ButtonCheck_BuzzerOff();
	void ButtonCheck_Stop();
	void ButtonCheck_Start();

	void ButtonCheck_Z_Axis_Up();
	void ButtonCheck_Z_Axis_Down();

	void SafetyChk();
};
extern CRun_Handler_Check Run_Handler_Check;
extern CRun_Handler_Check Run_Handler_Check_Manual;
#endif // !defined(AFX_RUN_HANDLER_CHECK_H__C7C7B3DE_3046_4089_8C04_0D4F36A1D947__INCLUDED_)
