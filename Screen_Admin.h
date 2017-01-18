#if !defined(AFX_SCREEN_ADMIN_H__EB1175A4_638C_47E2_B09B_2AB54A977A29__INCLUDED_)
#define AFX_SCREEN_ADMIN_H__EB1175A4_638C_47E2_B09B_2AB54A977A29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Screen_Admin.h : header file
//
#include "Variable.h"		// 전역 변수 정의 클래스 추가 
#include "Public_Function.h"
#include "NewLabel.h"		// 칼라 텍스트 박스 생성 클래스 추가 
#include "editex.h"			// 칼라 에디트 박스 생성 클래스 추가 
#include "BtnST.h"          // 칼라 버튼 생성 클래스 추가 
#include "XPGroupBox.h"		// 칼라 그룹 박스 생성 클래스 추가 
#include "sxlogfont.h"
#include "MacButtons.h"
#include "ListBoxST.h"      // 칼라 아이콘 리스트 박스 클래스
#include "xpbutton.h"		// XP 칼라 버튼 클래스
#include "EXDigitST.h"		// 디지털 카운터 클래스
#include "GradientStatic.h" // 그라데이션 칼라 텍스트 박스 생성 클래스 추가 
#include "FloatST.h"
#include "LedButton.h"				// LED 버튼 생성 클래스 추가 

#include "Digit.h"

// ******************************************************************************
// 설정 정보 로딩 및 저장 클래스
// ******************************************************************************
#include "MyBasicData.h"
// ******************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CScreen_Admin form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CScreen_Admin : public CFormView
{
public:
	CScreen_Admin();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScreen_Admin)

// Form Data
public:
	//{{AFX_DATA(CScreen_Admin)
	enum { IDD = IDD_SCREEN_ADMIN };
	CXPGroupBox	m_group_load_socket;
	CButton	m_btn_fileload;
	CButton	m_btn_filesave;
	CXPGroupBox	m_group_y_safety;
	CXPGroupBox	m_group_x_safety;
	CXPGroupBox	m_group_rbtxy_safety;
	CMacButton	m_btn_connect_xgem;
	CMacButton	m_btn_cip_set_xgem;
	CMacButton	m_btn_cport_set_xgem;
	CMacButton	m_btn_test_xgem;
	CIPAddressCtrl	m_cip_xgem;
	CGradientStatic	m_msg_cport_xgem;
	CGradientStatic	m_msg_cip_xgem;
	CXPGroupBox	m_group_client_xgem;
	CXPGroupBox	m_group_align_type;
	CXPGroupBox	m_group_pogo_type;
	CXPGroupBox	m_group_mirror_type;
	CButton	m_chk_fullinline;
	CButton	m_radio_alignnotuse;
	CButton	m_radio_alignuse;
	CButton	m_radio_pogo_use;
	CButton	m_radio_pogo_notuse;
	CButton	m_radio_mirror_use;
	CButton	m_radio_mirror_notuse;
	CButton	m_chk_bufferin_chk;
	CIPAddressCtrl	m_cip_router;
	CGradientStatic	m_msg_cip_router;
	CGradientStatic	m_msg_cport_router;
	CMacButton	m_btn_test_router;
	CMacButton	m_btn_cport_set_router;
	CMacButton	m_btn_connect_router;
	CMacButton	m_btn_cip_set_router;
	CXPGroupBox	m_group_client_router;
	CMacButton	m_btn_sport_set_router;
	CGradientStatic	m_msg_sport_router;
	CButton	m_chk_virtual_supply;
	CButton	m_radio_pos_rear;
	CButton	m_radio_pos_front;
	CButton	m_radio_pos_alone;
	CXPGroupBox	m_group_client_next;
	CXPGroupBox	m_group_client_front;
	CXPGroupBox	m_group_client_ec;
	CGradientStatic	m_msg_cip_next;
	CGradientStatic	m_msg_cport_next;
	CXPGroupBox	m_group_server;
	CIPAddressCtrl	m_cip_next;
	CMacButton	m_btn_test_next;
	CMacButton	m_btn_cport_set_next;
	CMacButton	m_btn_connect_next;
	CMacButton	m_btn_cip_set_next;
	CMacButton	m_btn_sport_set_next;
	CMacButton	m_btn_sport_set_front;
	CMacButton	m_btn_sport_set_bpc;
	CGradientStatic	m_msg_sport_next;
	CGradientStatic	m_msg_sport_front;
	CGradientStatic	m_msg_sport_bpc;
	CMacButton	m_btn_test_front;
	CMacButton	m_btn_test_ec;
	CMacButton	m_btn_test_bpc;
	CMacButton	m_btn_cport_set_front;
	CMacButton	m_btn_cport_set_ec;
	CMacButton	m_btn_cport_set_bpc;
	CMacButton	m_btn_connect_front;
	CMacButton	m_btn_connect_ec;
	CMacButton	m_btn_connect_bpc;
	CMacButton	m_btn_cip_set_front;
	CMacButton	m_btn_cip_set_ec;
	CMacButton	m_btn_cip_set_bpc;
	CIPAddressCtrl	m_cip_front;
	CIPAddressCtrl	m_cip_ec;
	CIPAddressCtrl	m_cip_bpc;
	CGradientStatic	m_msg_cport_front;
	CGradientStatic	m_msg_cport_ec;
	CGradientStatic	m_msg_cport_bpc;
	CGradientStatic	m_msg_cip_front;
	CGradientStatic	m_msg_cip_ec;
	CGradientStatic	m_msg_cip_bpc;
	CButton	m_chk_Virtual;
	CMacButton	m_btn_bufferdb_delete;
	CXPGroupBox	m_group_next_apct_set;
	CButton m_radio_version_mode_use;
	CButton m_radio_version_mode_notuse;
	//}}AFX_DATA

// Attributes
public:
	TSpread			*m_grid_motor_info;
	GridControlAlg	m_p_grid;


	CEXDigitST		m_dgt_retest_cnt;
	CEXDigitST		m_dgt_multi_retest_cnt;//2015.0518
	CEXDigitST		m_dgt_beat_cnt;
	CEXDigitST		m_dgt_autosocketoff_cnt;

// 	CFont* mp_admin_font;			// 폰트 정보 설정 변수
	
	CMyBasicData    mcls_basic;		// 기본 셋팅 정보 로딩 및 저장 클래스
	
	// **************************************************************************
	// 칼라 에디트 박스 클래스 변수 선언
	// **************************************************************************
	CEditEx	 m_edit_cport_bpc;
	CEditEx	 m_edit_cport_ec;
	CEditEx	 m_edit_cport_front;
	CEditEx	 m_edit_cport_next;
	CEditEx  m_edit_cport_router;
	CEditEx	 m_edit_cport_xgem;//2013,1028

	CEditEx	 m_edit_sport_bpc;
	CEditEx	 m_edit_sport_front;
	CEditEx	 m_edit_sport_next;
	// **************************************************************************
	
	// **************************************************************************
	// 데이터 저장 클래스 변수 선언
	// **************************************************************************
	CString mstr_epc_server_port, mstr_next_machine_server_port, mstr_next_apct_server_port;

	int nAPCTType;

	int mn_buffer_ready_step;
	int mn_buffer_in_step;

	long mlWait_Time[3];

// Operations
public:
	void OnAdmin_Data_Set();
	void OnAdmin_Label_Set();
	void OnAdmin_EditBox_Set();
	void OnAdmin_GroupBox_Set();
	void OnAdmin_Dgt_Set();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreen_Admin)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScreen_Admin();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnAdmin_Init_PickerPosition();
	void OnAdmin_Display_PickerPosition();

	void OnAdmin_Init_TrayPlaceOffset();
	void OnAdmin_Display_TrayPlaceOffset();

	void OnAdmin_Init_COK_Teaching();
	void OnAdmin_Display_COK_Teaching();

	void OnAdmin_Init_CokIndex();
	void OnAdmin_Display_CokIndex();

	//2016.0329
	void OnAdmin_Init_XSafetyPosition();
	void OnAdmin_Init_YSafetyPosition();
	void OnAdmin_Display_SafetyPosition();

	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridData(UINT nID, int row, int col, CString data);
	void GridFont(UINT nID, int row, int col, int size);
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	CString GetGridData( UINT nID, int row, int col );

	// Generated message map functions
	//{{AFX_MSG(CScreen_Admin)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnServerPortSaveNmachine();
	afx_msg void OnChkVirtual();
	afx_msg void OnBtnFileEditor();
	afx_msg void OnBtnCipSetBpc();
	afx_msg void OnBtnTstBpc();
	afx_msg void OnBtnCipSetEc();
	afx_msg void OnBtnCipSetFront();
	afx_msg void OnBtnCipSetNext();
	afx_msg void OnBtnCportSetBpc();
	afx_msg void OnBtnCportSetEc();
	afx_msg void OnBtnCportSetFront();
	afx_msg void OnBtnCportSetNext();
	afx_msg void OnBtnSportSetFront();
	afx_msg void OnBtnSportSetBpc();
	afx_msg void OnBtnSportSetNext();
	afx_msg void OnRadioPositionFront();
	afx_msg void OnRadioPositionRear();
	afx_msg void OnRadioPositionAlone();
	afx_msg void OnBtnTstNext();
	afx_msg void OnChkVirtualSupply();
	afx_msg void OnBtnCipSetRouter();
	afx_msg void OnBtnCportSetRouter();
	afx_msg void OnBtnTstRouter();
	afx_msg void OnDgtRetestCnt();
	afx_msg void OnDgtAutoSocketOff();
	afx_msg void OnDgtBeatCnt();
	afx_msg void OnBtnGetCommonTeaching();
	afx_msg void OnBufferInChk();
	afx_msg void OnBtnPgminfo();
	afx_msg void OnRadioMirrorUse();
	afx_msg void OnRadioMirrorNotuse();
	afx_msg void OnRadioPogoUse();
	afx_msg void OnRadioPogoNotuse();
	afx_msg void OnRadioAlignUse();
	afx_msg void OnRadioAlignNotuse();
	afx_msg void OnBtnDelfrontBuffer();
	afx_msg void OnBtnDelrearBuffer();
	afx_msg void OnBtnTestMode();
	afx_msg void OnBtnSendTestMode();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnBtnVersion();
	afx_msg void OnFullInlineChk();
	afx_msg void OnBTNCIPSETXGem();
	afx_msg void OnBTNCPORTSETXGem();
	afx_msg void OnBTNTSTXGem();
	afx_msg void OnBtnReqEmpty();
	afx_msg void OnBtnSendTool();
	afx_msg void OnBtnEmptyCancle();
	afx_msg void OnBtnToolCancle();
	afx_msg void OnDgtRetestCnt2();
	afx_msg void OnRadioVersionModeUse();
	afx_msg void OnRadioVersionModeNotuse();
	afx_msg void OnBtnSaveFile();
	afx_msg void OnBtnLoadFile();
	afx_msg void OnBtnBufferdbDelete();
	afx_msg void OnBtnLoadSok();
	afx_msg void OnBtnSaveSok();
	//}}AFX_MSG

	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREEN_ADMIN_H__EB1175A4_638C_47E2_B09B_2AB54A977A29__INCLUDED_)
