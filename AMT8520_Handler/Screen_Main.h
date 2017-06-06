#if !defined(AFX_SCREEN_MAIN_H__7A483F1B_61FF_4EB9_B41D_251A637D6074__INCLUDED_)
#define AFX_SCREEN_MAIN_H__7A483F1B_61FF_4EB9_B41D_251A637D6074__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Screen_Main.h : header file
//
#include "Variable.h"		// 전역 변수 정의 클래스 추가 
#include "Public_Function.h"

#include "NewLabel.h"		// 칼라 텍스트 박스 생성 클래스 추가 
#include "editex.h"			// 칼라 에디트 박스 생성 클래스 추가 
#include "BtnST.h"          // 칼라 버튼 생성 클래스 추가 
#include "XPGroupBox.h"		// 칼라 그룹 박스 생성 클래스 추가 
#include "sxlogfont.h"
#include "LedButton.h"      // LED 버튼 클래스
#include "EXDigitST.h"		// 디지털 카운터 생성 클래스 추가 
#include "MacButtons.h"
#include "Digit.h"
#include "GradientStatic.h" // 그라데이션 칼라 텍스트 박스 생성 클래스 추가 

#include "FloatST.h"
#include "FtpClient.h"//2015.0604

enum ENUM_MAIN_VIEW_TYPE
{
	EMVT_SERIAL,
	EMVT_LOTNO,
	EMVT_SN,
	EMVT_PPID,
	EMVT_WWN,
	EMVT_CSN,
	EMVT_PSID,
	EMVT_BIN,
	EMVT_SCRAPCODE,
	EMVT_TESTCNT,
	EMVT_TESTEDPOS,
	
	EMVT_USABLE,
	EMVT_BOARDOK,
	EMVT_TS_TESTCNT,
	EMVT_BINHISTORY,
	
	EMVT_MAX,
};

class AModule;

/////////////////////////////////////////////////////////////////////////////
// CScreen_Main form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CScreen_Main : public CFormView
{
public:
	CScreen_Main();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScreen_Main)

// Form Data
public:
	//{{AFX_DATA(CScreen_Main)
	enum { IDD = IDD_SCREEN_MAIN };
	CButton	m_btn_abort;
	CButton	m_btnBufferIn;
	CMacButton	m_btn_stacker_off;
	CXPGroupBox	m_group_p_lot_info;				// P_LOT 2013,0910
	CGradientStatic	m_msg_p_lot_info;			// P_LOT 2013,0910
	CXPGroupBox	m_group_work3;
	CXPGroupBox	m_group_work2;
	CXPGroupBox	m_group_work1;
	CXPGroupBox	m_group_conv3;
	CXPGroupBox	m_group_conv2;
	CXPGroupBox	m_group_conv1;
	CXPGroupBox	m_group_bin_tray;
	CXPGroupBox	m_group_robot;
	CXPGroupBox	m_group_reference;
	CXPGroupBox	m_group_site;
	CXPGroupBox	m_group_tray_move_rej;
	CXPGroupBox	m_group_tray_move;
	CXPGroupBox	m_group_rej_stacker;
	CXPGroupBox	m_group_selected_tray;
	CXPGroupBox	m_group_ssd_auto_sorter;
	CXPGroupBox	m_group_bin_info;
	CMacButton	m_btn_door_open;
	CXPGroupBox	m_group_b_lot_info;
	CDigit	m_dgt_alarm;
	CGradientStatic	m_msg_wrun;
	CGradientStatic	m_msg_stop;
	CGradientStatic	m_msg_run;
	CGradientStatic	m_msg_brun;
	CGradientStatic	m_msg_change;
	CGradientStatic	m_msg_alarm;
	CGradientStatic	m_msg_mtbi;
	CXPGroupBox	m_group_daily_yield_info;
	CXPGroupBox	m_group_time_info;
	CXPGroupBox	m_group_lot_info;
	CXPGroupBox	m_group_lot_yield_info;
	CDigit	m_dgt_stop;
	CDigit	m_dgt_run;
	CDigit	m_dgt_mtbi;
	//}}AFX_DATA

// Attributes
public:
// 	CFont* mp_main_font;			// 폰트 정보 설정 변수
// 	CFont* mp_main_big_font;		// 폰트 정보 설정 변수

	int mn_time_select;

	int mn_air_blow_check[2];

	CString mstr_rear_buffer[2];

	int mn_rear_buffer[2];

	GridControlAlg m_pGrid;
	CString	m_strTempMSG;

	////2015.0604
	CFtpClient	m_pFtp;
	CString		m_strPathRemote;
	////

// Operations
public:
	void OnMain_Init_Bin_Info();
	void OnMain_Display_Bin_Info();

	void OnMain_Display_Daily_Yield_info();
	void OnMain_Display_Lot_Yield_info();
	void OnMain_Display_Lot_Info();
	void OnMain_Init_B_Lot_Info();
	void Device_Lot_Display(int part, int pos);
	void Device_Info_Display(int part, int pos);
	void OnMain_Init_Lot_Info();
	void OnMain_Init_Daily_Yield_info();
	void OnMain_Init_Lot_Yield_info();
	void OnMain_TimeInfo_Display();
	void OnMain_Time_Display(int n_state);
	void OnMain_Digital_Count_Set();
	void OnMain_Label_Set();

	void OnMain_Init_COK_TSITE();
	void OnMain_Init_COK_CNT();
	void OnMain_Init_TrayMove();
	void OnMain_Init_TrayMove( int iIdx );
	void OnMain_Init_Reject_Floor();
	void OnMain_Init_SelectedTray();
	void OnMain_Init_TestSite( int iIdx );
	void OnMain_Init_COK_Robot();
	void OnMain_Init_Robot_Module();
	void OnMain_Init_BinTray( int iIdx );
	void OnMain_Init_NetUsable();

	void OnMain_Init_Door();
	void OnMain_Display_Door();

	void OnMain_Init_SMema();
	void OnMain_Display_Smema();

	void OnMain_Init_Bypass();
	void OnMain_Display_Bypass();

	void OnMain_Init_RecvNext();
	void OnMain_Display_RecvNext();


	void OnMain_Change_Reject_Floor();
	void OnMain_Change_Selected_Tray();
	void OnMain_Change_BinTray( int iIdx );

	void OnMain_display_RobotPicker();
	void OnMain_Display_TestSite();
	void OnMain_Display_NetUsable();

	void OnMain_Init_Ref_Color();
	
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	void GridFont(UINT nID, int row, int col, int size);
	void GridData(UINT nID, int row, int col, CString data);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void OnMain_GroupBox_Set();

	void OnDrawCOKSite( int iIdx );
	void OnDrawCokStacker( int iIdx );
	void OnDrawVideo();

	void SetTrayPos( int iIdx );
	void OnMain_Init_P_Lot_Info();	// P_LOT 20130910
	void OnMain_Display_P_Lot_Info();	// P_LOT 20130910

	////2015.0604
	void FtpConnect();
	void FtpFileDown();
	void OnUpdateRemote(CString strPath);
	////

protected:
	void CalcPopupMenuByClick( int iCustom );

	CString GetStringEMVT( ENUM_MAIN_VIEW_TYPE emvt );

protected:
	int m_iClick;
	DWORD m_dwTimeClicked;
	int m_iClickCnt;

	ENUM_MAIN_VIEW_TYPE m_emvt_ts;
	ENUM_MAIN_VIEW_TYPE m_emvt_mdl;

public:
	int GetCustom_TrayMove( int iIdx );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreen_Main)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScreen_Main();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScreen_Main)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnDoorOpen();
	afx_msg void OnBtnTest1();
	afx_msg void OnBtnTest2();
	afx_msg void OnBtnTest3();
	afx_msg void OnBtnTest4();
	afx_msg void OnBtnTest5();
	afx_msg void OnBtnTest6();
	afx_msg void OnBtnTrayminus();
	afx_msg void OnMsgPLotInfo();
	afx_msg void OnBtnStackerOff();
	afx_msg void OnButtonXgem();
	afx_msg void OnButtonFillratio();
	afx_msg void OnBtnAbort();
	afx_msg void OnDoubleclickedBtnDoorOpen();
	afx_msg void OnBtnManagerSocket();
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMain_Work_Info_Display(WPARAM wParam,LPARAM lParam);  // 테스트 결과 정보 화면에 출력하기 위한 사용자 정의 메시지 추가 
//	afx_msg LRESULT OnReceivedNewLot( WPARAM wParam, LPARAM lParam );
//	afx_msg LRESULT OnDrawLotTime( WPARAM wParam, LPARAM lParam );
//	afx_msg LRESULT OnDrawWorkSite( WPARAM wParam, LPARAM lParam );

	afx_msg LRESULT OnMain_Draw_UI(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMain_Draw_Data(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnMain_Change_View( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREEN_MAIN_H__7A483F1B_61FF_4EB9_B41D_251A637D6074__INCLUDED_)
