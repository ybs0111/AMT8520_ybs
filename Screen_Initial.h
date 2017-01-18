#if !defined(AFX_SCREEN_INITIAL_H__C8338AC3_B7DD_4866_93A2_DF84D57A5B3E__INCLUDED_)
#define AFX_SCREEN_INITIAL_H__C8338AC3_B7DD_4866_93A2_DF84D57A5B3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Screen_Initial.h : header file
//
#include "Variable.h"				// 전역 변수 정의 클래스 추가 
#include "PictureEx.h"				// GIF 파일 로딩 클래스 추가 
#include "GradientStatic.h"			// 그라데이션 칼라 텍스트 박스 생성 클래스 추가 
#include "XPButton.h"				// XP 형태 칼라 버튼 생성 클래스 추가 
#include "LedButton.h"				// LED 버튼 생성 클래스 추가 
#include "Public_Function.h"

#include "MacButtons.h"
#include "SuperProgressCtrl.h"		// ProgressCtrl 생성 클래스 추가

#include "Thread.h"					// 쓰레드 생성 클래스 추가 
// ******************************************************************************
// 쓰레드 동작 관련 클래스 추가                                                  
// ******************************************************************************
#include "Run_Handler_Check.h"
#include "Run_TowerLamp.h"

#include "Run_COK_Robot.h"
#include "Run_Reject_Stacker.h"
#include "Run_Reject_Tray_Transfer.h"
#include "Run_Robot.h"
#include "Run_Test_Site.h"
#include "Run_Bin_Stacker.h"
// ******************************************************************************

// ******************************************************************************
// 설정 정보 로딩 및 저장 클래스                                                 
// ******************************************************************************
#include "MyBasicData.h"

// ******************************************************************************
// 초기화 화면 관련 타이머 ID 정의                                               
// ******************************************************************************
#define TMR_INIT	710  // 초기화 작업 진행 타이머 ID 선언 
// ******************************************************************************
/////////////////////////////////////////////////////////////////////////////
// CScreen_Initial form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif



enum ENUM_INIT_STEP
{
	EIS_START,
	EIS_ERRMSG,
	EIS_IO,
	EIS_MOTOR,
	EIS_CHK_STATE,
	EIS_RECOVERY,
	EIS_CREATE_THREAD,
	EIS_SET_THREAD_STEP,
	EIS_CHK_ALL_FINISH,
	
	EIS_FINISH,
};

class CScreen_Initial : public CFormView
{
public:
	CScreen_Initial();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScreen_Initial)

// Form Data
public:
	//{{AFX_DATA(CScreen_Initial)
	enum { IDD = IDD_SCREEN_INITIAL };
	CLedButton	m_radio_robot_p;
	CLedButton	m_radio_cnt_tray;
	CLedButton	m_radio_tt_y;
	CLedButton	m_radio_tt_clamp;
	CLedButton	m_radio_tt_rail;
	CLedButton	m_radio_ts2_z;
	CLedButton	m_radio_ts2_plate;
	CLedButton	m_radio_ts2_lock;
	CLedButton	m_radio_ts2_insert;
	CLedButton	m_radio_ts1_z;
	CLedButton	m_radio_ts1_plate;
	CLedButton	m_radio_ts1_lock;
	CLedButton	m_radio_ts1_insert;
	CLedButton	m_radio_tt;
	CLedButton	m_radio_ts2;
	CLedButton	m_radio_ts1;
	CLedButton	m_radio_rs_z;
	CLedButton	m_radio_rs;
	CLedButton	m_radio_cok_z;
	CLedButton	m_radio_cok_y;
	CLedButton	m_radio_cok_lock;
	CLedButton	m_radio_cok;
	CLedButton	m_radio_bs3_z;
	CLedButton	m_radio_bs3_rail;
	CLedButton	m_radio_bs3_r;
	CLedButton	m_radio_bs3_conv;
	CLedButton	m_radio_bs2_z;
	CLedButton	m_radio_bs2_rail;
	CLedButton	m_radio_bs2_r;
	CLedButton	m_radio_bs2_conv;
	CLedButton	m_radio_bs1_z;
	CLedButton	m_radio_bs1_rail;
	CLedButton	m_radio_bs1_r;
	CLedButton	m_radio_bs1_conv;
	CLedButton	m_radio_bs3;
	CLedButton	m_radio_bs2;
	CLedButton	m_radio_bs1;
	CLedButton	m_radio_robot_z;
	CLedButton	m_radio_robot_y;
	CLedButton	m_radio_robot_x;
	CLedButton	m_radio_robot_picker_vacuum;
	CLedButton	m_radio_robot_module;
	CLedButton	m_radio_robot;
	CLedButton	m_radio_mot_bd;
	CLedButton	m_radio_io_bd;
	CPictureEx	m_gif_initializing;
	CMacButton	m_btn_init_skip;
	CMacButton	m_btn_init_retry;
	CGradientStatic	m_msg_initial;
	//}}AFX_DATA

// Attributes
public:
// 	CFont* mp_init_font;  // 폰트 설정 변수 선언 
	
	// **************************************************************************
	// 환경 설정 데이터 로딩 및 저장 클래스 변수 선언                            
	// **************************************************************************
	CMyBasicData     mcls_m_basic;		// 기본 셋팅 정보 로딩 및 저장 클래스
	// **************************************************************************
	
	UINT m_nDataSendCnt;
	
	int mn_pos_step;				// 프로그레서 위치 정보 저장 변수
	
	// **************************************************************************
	// 초기화 관련 : Step 데이터 저장 변수 및 리턴 플래그 선언                   
	// **************************************************************************
	long ml_init_step ;
	int mn_init_flag;

	bool bRecoveryInit;
	// **************************************************************************

// Operations
public:
	int OnInitial_Init_Excution();
	void OnInitial_Initial_Ready();
	void OnInitial_Step_Clear();
	void OnInitial_Change_Status(int n_status);
	void OnInitial_Controls_Enable(int n_init);
	void OnInitial_RadioButton_Set();
	void OnInitial_Progress_Set();
	void OnInitial_Picture_Set();
	void OnInitial_Label_Set();

protected:
	int OnLevelCheck();
	bool ChkInitReady();

	void EIS_Start();
	void EIS_ErrMsg();
	void EIS_InitIO();
	void EIS_Motor();
	void EIS_Chk_State();
	void EIS_Recovery();
	void EIS_Create_Thread();
	void EIS_Set_Thread_Step();
	void EIS_Chk_All_Finish();
	void EIS_Finish();
	
private:
	double NetworkingTime[3];
	char cJamcode[10];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreen_Initial)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CSuperProgressCtrl m_ctrlProgress;
	CSuperProgressCtrl m_ctrlProgress1;

	virtual ~CScreen_Initial();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScreen_Initial)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnInitSkip();
	afx_msg void OnBtnInitRetry();
	//}}AFX_MSG

	afx_msg LRESULT OnInitBtn(WPARAM wParam,LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

enum ENUM_INIT_BTN
{
	RBTN_ALL_OFF,
		
	RBTN_IO,
	RBTN_MOTOR,

	RBTN_ROBOT,
	RBTN_ROBOT_MODULE_EMPTY,
	RBTN_ROBOT_PICKER_VACUUM,
	RBTN_ROBOT_Z,
	RBTN_ROBOT_P,
	RBTN_ROBOT_X,
	RBTN_ROBOT_Y,

	RBTN_BIN_STACKER1,
	RBTN_BS1_CONV,
	RBTN_BS1_RAIL,
	RBTN_BS1_Z,
	RBTN_BS1_R,

	RBTN_BIN_STACKER2,
	RBTN_BS2_CONV,
	RBTN_BS2_RAIL,
	RBTN_BS2_Z,
	RBTN_BS2_R,

	RBTN_BIN_STACKER3,
	RBTN_BS3_CONV,
	RBTN_BS3_RAIL,
	RBTN_BS3_Z,
	RBTN_BS3_R,

	RBTN_REJECT_STACKER,
	RBTN_RS_Z,
	RBTN_RS_CNT_TRAY,

	RBTN_TRAY_TRANSFER,
	RBTN_TT_CLAMP,
	RBTN_TT_Y,
	RBTN_STACKER_RAIL,

	RBTN_COK,
	RBTN_COK_LOCK,
	RBTN_COK_Z,
	RBTN_COK_Y,

	RBTN_SITE1,
	RBTN_SITE1_LOCK,
	RBTN_SITE1_COK_INSERT,
	RBTN_SITE1_PLATE_FORWARD,
	RBTN_SITE1_Z,

	RBTN_SITE2,
	RBTN_SITE2_LOCK,
	RBTN_SITE2_COK_INSERT,
	RBTN_SITE2_PLATE_FORWARD,
	RBTN_SITE2_Z,
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREEN_INITIAL_H__C8338AC3_B7DD_4866_93A2_DF84D57A5B3E__INCLUDED_)
