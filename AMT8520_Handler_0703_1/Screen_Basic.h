#if !defined(AFX_SCREEN_BASIC_H__75EFE953_60C7_432F_9759_19FF79697AC2__INCLUDED_)
#define AFX_SCREEN_BASIC_H__75EFE953_60C7_432F_9759_19FF79697AC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Screen_Basic.h : header file
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
// CScreen_Basic form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CScreen_Basic : public CFormView
{
public:
	CScreen_Basic();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScreen_Basic)

// Form Data
public:
	//{{AFX_DATA(CScreen_Basic)
	enum { IDD = IDD_SCREEN_BASIC };
	CButton	m_radio_auto_timeout;
	CButton	m_radio_cserial2_notuse;
	CXPGroupBox	m_group_cserial2;
	CButton	m_radio_recipe_notuse;
	CButton	m_radio_recipe_use;
	CXPGroupBox	m_group_recipe_mode;
	CButton	m_radio_site2_both;
	CButton	m_radio_site1_both;
	CButton	m_radio_site2_2;
	CButton	m_radio_site2_1;
	CButton	m_radio_site1_2;
	CButton	m_radio_site1_1;
	CXPGroupBox	m_group_sie2;
	CXPGroupBox	m_group_sie1;
	CXPGroupBox	m_group_product_site2;
	CXPGroupBox	m_group_product_site1;
	CXPGroupBox	m_group_product;
	CButton	m_radio_site2_notuse;
	CButton	m_radio_site1_notuse;
	CButton	m_radio_site2_use;
	CButton	m_radio_site1_use;
	CButton	m_radio_product_use;
	CButton	m_radio_product_notuse;
	CButton	m_radio_notuse_rcmd;
	CButton	m_radio_notuse_xgem;
	CComboBox	m_combo_recipe;
	CButton	m_radio_use_xgem;
	CButton	m_radio_use_rcmd;
	CXPGroupBox	m_broup_rcmd_mode;
	CXPGroupBox	m_group_xgem_mode;
	CButton	m_radio_pt_1v1m_4use_2;
	CXPGroupBox	m_group_socket_max_count;
	CXPGroupBox	m_group_hotfix_pos;
	CXPGroupBox m_group_hifix_mode;
	CComboBox	m_cbohifix_mode;
	CButton	m_radio_recovery_notuse;
	CButton	m_radio_recovery_use;
	CXPGroupBox	m_group_recovery_mode;
	CButton	m_radio_jig_notuse;
	CButton	m_radio_jig_use;
	CXPGroupBox	m_group_jig_mode;
	CXPGroupBox	m_group_basic_manual_speed;
	CXPGroupBox	m_group_basic_run_speed;
	CXPGroupBox	m_group_basic_speed;
	CXPGroupBox	m_group_robot_init;
	CButton	m_radio_pogo_notuse;
	CButton	m_radio_pogo_mode;
	CXPGroupBox	m_group_pogo_mode;
	CXPGroupBox	m_group_empty_tray;
	CMacRadioButton	m_radio_bypass_notuse;
	CMacRadioButton	m_radio_bypass_rear;
	CMacRadioButton	m_radio_bypass_front;
	CMacRadioButton	m_radio_bypass_all;
	CXPGroupBox	m_group_time_out;
	CButton	m_radio_online_mode;
	CButton	m_radio_offline_mode;
	CXPGroupBox	m_group_online_mode;
	CButton	m_radio_pt_2v1m_2use;
	CButton	m_radio_pt_1v1m_2use;
	CButton	m_radio_pt_1v1m_4use;
	CXPGroupBox	m_group_pick_type;
	CButton	m_radio_degree_180;
	CButton	m_radio_degree_270;
	CButton	m_radio_degree_90;
	CButton	m_radio_degree_0;
	CXPGroupBox	m_group_degree;
	CButton	m_radio_cok_5;
	CButton	m_radio_cok_4;
	CButton	m_radio_cok_3;
	CButton	m_radio_cok_2;
	CButton	m_radio_cok_1;
	CButton	m_radio_cok_0;
	CXPGroupBox	m_group_cok_type;
	CXPGroupBox	m_group_recipe_select;
	CGradientStatic	m_msg_tray_y;
	CGradientStatic	m_msg_tray_x;
	CXPGroupBox	m_group_tray_size_setting;
	CGradientStatic	m_msg_port_ecserver;
	CXPGroupBox	m_group_alarm_delay_time;
	CXPGroupBox	m_group_network_wait_time;
	CXPGroupBox	m_group_network_retry_count;
	CGradientStatic	m_label_electrostatic_5;
	CGradientStatic	m_label_electrostatic_4;
	CGradientStatic	m_label_electrostatic_3;
	CGradientStatic	m_label_electrostatic_2;
	CGradientStatic	m_label_electrostatic_1;
	CGradientStatic	m_msg_electrostatic_5;
	CGradientStatic	m_msg_electrostatic_4;
	CGradientStatic	m_msg_electrostatic_3;
	CGradientStatic	m_msg_electrostatic_2;
	CGradientStatic	m_msg_electrostatic_1;
	CMacRadioButton	m_radio_device_mode_use;
	CMacRadioButton	m_radio_device_mode_notuse;
	CDigit	m_dgt_electrostatic_check_value;
	CListBoxST	m_list_device_type;
	CXPGroupBox	m_group_electrostatic_test;
	CXPGroupBox	m_group_electrostatic_check_value;
	CXPGroupBox	m_group_electrostatic_check_gap;
	CXPGroupBox	m_group_device_mode;
	CMacButton	m_btn_electostatic_test;
	CMacButton	m_btn_dvc_delete;
	CMacButton	m_btn_dvc_create;
	CMacButton	m_btn_basic_cancel;
	CMacButton	m_btn_basic_apply;
	CButton	m_radio_retest_jig_notuse;
	CButton	m_radio_retest_jig_use;
	CXPGroupBox	m_group_retest_jig_mode;
	CButton		m_radio_cserial2_use;
	//}}AFX_DATA

// Attributes
public:
// 	CFont* mp_basic_font;			// 폰트 정보 설정 변수
	
	CMyBasicData    mcls_basic;		// 기본 셋팅 정보 로딩 및 저장 클래스
	
	// **************************************************************************
	// 칼라 에디트 박스 클래스 변수 선언
	// **************************************************************************
	CEXDigitST  m_dgt_electrostatic_check_gap;			// 정전기 확인을 몇분에 한번씩 할것인가.
	CEXDigitST  m_dgt_network_wait_time;		// Network 대기 시간
	CEXDigitST  m_dgt_network_retry_cnt;		// Network Retry 횟수
	CEXDigitST	m_dgt_alarm_delay_time;
	CEXDigitST	m_dgt_time_out;
	CEXDigitST	m_dgt_empty_tray_out;
	CEXDigitST	m_dgt_robot_init_time;
	CEXDigitST	m_dgt_hotfix_pos;
	CEXDigitST	m_dgt_time_out_per;

	CEXDigitST	m_dgt_tray_x;
	CEXDigitST	m_dgt_tray_y;

	CEXDigitST m_dgt_basic_run_speed;
	CEXDigitST m_dgt_basic_manual_speed;
	CEXDigitST m_dgt_socket_max_count;

	CEditEx	 m_edit_device_type;
	
	// **************************************************************************
	// 리스트 박스 컨트롤 정보 저장 클래스 변수 선언
	// **************************************************************************
	CImageList mp_device_image_list;	// 이미지 리스트 컨트롤 선언
	
	CString mstr_temp_device;			// 선택된 디바이스 종류 임시 저장 변수
	CString mstr_load_file;				// 로딩할 파일에 대한 [폴더]+[파일명] 저장 변수
	int mn_device_name;					// 선택된 디바이스의 리스트 위치 저장 변수
	
	CString mstr_new_device;			// 새로 생성할 디바이스의 이름.
	// **************************************************************************
	
	// **************************************************************************
	// 칼라 에디트 박스 클래스 변수 선언                                         
	// **************************************************************************
	CEditEx  m_edit_ecserver_port;
	// **************************************************************************
	
	// **************************************************************************
	// 데이터 저장 클래스 변수 선언
	// **************************************************************************
	BYTE nIPFiled0[2], nIPFiled1[2], nIPFiled2[2], nIPFiled3[2];		// 0: ECSERVER, 1: NEXT MACHINE
	CString m_strTempMSG;
	CString m_strRecipe[RECIPE_CNT];

// Operations
public:
	int OnBasic_Input_Data_Check();
	void OnBasic_Data_Display();
	void OnBasic_DeviceData_Apply();
	void OnBasic_TitleBar_Info_Send();
	void OnBasic_Data_Apply();
	void OnBasic_Electrostatic_Data_View(int nChannel);
	int OnBasic_Input_Device_Check();
	BOOL AddItem(int nIndex, WIN32_FIND_DATA *pfd);
	int OnBasic_Init_List(LPCTSTR pszPath);
	void OnBasic_Create_ImageList();
	void OnBasic_Device_Display(CString str_dvc);
	void OnBasic_Device_Focus_Set();
	void OnBasic_Create_Device();
	int OnBasic_New_Device_Check();
	void OnIP_Address(CString str_ip, int n_section);
	void OnBasic_DeviceMode_Change();
	void OnBasic_ByPassMode_Change();
	void OnBasic_CokType_Change();
	void OnBasic_Data_Change();
	void OnBasic_Label_Set();
	void OnBasic_Data_Set();
	void OnBasic_Digital_Count_Set();
	void OnBasic_EditBox_Set();
	void OnBasic_GroupBox_Set();
	void OnBasic_Degree_Change();
	void OnBasic_PickType_Change();
	void OnBasic_RunMode_Change();

	void OnShow_Bypass_Box();
	void OnBasic_PogoMode_Change();
	void OnBasic_JigMode_Change();
	void OnBasic_RecoveryMode_Change();
	void OnBasic_XgemMode_Change();
	void OnBasic_RcmdMode_Change();
	void LoadRegRecipeIdData();

	void OnBasic_ProductMode_Change();
	void OnBasic_ProductSite1Mode_Change();
	void OnBasic_ProductSite2Mode_Change();

	void OnBasic_Retest_JigMode_Change();//2015.0914
	void OnBasic_Recipe_Mode_Change();
	void OnBasic_CSerial2Mode_Change(); //2015.1123
	void OnBasic_Auto_TimeOut_mode_change();

	// Grid
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridData(UINT nID, int row, int col, CString data);
	void GridFont(UINT nID, int row, int col, int size);
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	CString GetGridData( UINT nID, int row, int col );

	void OnBasic_Init_TrayOffset( ENUM_MODULE_POSITION empos );
	void OnBasic_TrayOffset_Change( ENUM_MODULE_POSITION empos );

	void OnClickTrayOffset( ENUM_MODULE_POSITION empos, int nx, int ny );
	void OnBasic_ProductSite1Use_Change();
	void OnBasic_ProductSite2Use_Change();

	//2015.0518
	void Loadhifixmode();
	void OnBasic_Hifix_Set();
	////
	void LoadRegHifixdData();//2015.0604

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreen_Basic)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CScreen_Basic();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScreen_Basic)
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRadioDeviceModeUse();
	afx_msg void OnRadioDeviceModeNotuse();
	afx_msg void OnDgtElectrostaticCheckGap();
	afx_msg void OnDgtElectrostaticCheckValue();
	afx_msg void OnBtnDvcCreate();
	afx_msg void OnSelchangeListDeviceType();
	afx_msg void OnBtnDvcDelete();
	afx_msg void OnBtnElectrostaticTest();
	afx_msg void OnBtnBasicApply();
	afx_msg void OnBtnBasicCancle();
	afx_msg void OnDgtNetworkWaitTime();
	afx_msg void OnDgtNetworkRetryCount();
	afx_msg void OnDgtAlarmDelayTime();
	afx_msg void OnBtnIpSaveEcserver();
	afx_msg void OnBtnPortSaveEcserver();
	afx_msg void OnBtnConnectEcserver();
	afx_msg void OnBtnTestEcserver();
	afx_msg void OnDgtTrayX();
	afx_msg void OnDgtTrayY();
	afx_msg void OnRadioCok0();
	afx_msg void OnRadioCok1();
	afx_msg void OnRadioCok2();
	afx_msg void OnRadioCok3();
	afx_msg void OnRadioCok4();
	afx_msg void OnRadioCok5();
	afx_msg void OnRadioDegree0();
	afx_msg void OnRadioDegree90();
	afx_msg void OnRadioDegree180();
	afx_msg void OnRadioDegree270();
	afx_msg void OnRadioPt1v1mUse4();
	afx_msg void OnRadio1v1mUse2();
	afx_msg void OnRadio2v1mUse2();
	afx_msg void OnRadioOnlineMode();
	afx_msg void OnRadioOfflineMode();
	afx_msg void OnDgtTimeOut();
	afx_msg void OnRadioBypassFront();
	afx_msg void OnRadioBypassRear();
	afx_msg void OnRadioBypassAll();
	afx_msg void OnRadioBypassNotuse();
	afx_msg void OnDgtEmpty();
	afx_msg void OnRadioPogoMode();
	afx_msg void OnRadioPogoNotuse();
	afx_msg void OnDgtRobotInitTime();
	afx_msg void OnDgtRunSpeed();
	afx_msg void OnDgtManualSpeed();
	afx_msg void OnRadioJigMode();
	afx_msg void OnRadioJigNotuse();
	afx_msg void OnRadioRecoveryMode();
	afx_msg void OnRadioRecoveryNotuse();
	afx_msg void OnDgtHotfixPos();
	afx_msg void OnDgtSocketMaxCount();
	afx_msg void OnRadioPt1v1mUse42();
	afx_msg void OnRadioUseXgem();
	afx_msg void OnRadioNotuseXgem();
	afx_msg void OnRadioUseRcmd();
	afx_msg void OnRadioNotuseRcmd();
	afx_msg void OnSelchangeComboAddRecipe();
	afx_msg void OnRadioUseProduct();
	afx_msg void OnRadioNotuseProduct();
	afx_msg void OnRadioUseSite1();
	afx_msg void OnRadioNotuseSite1();
	afx_msg void OnRadioUseSite2();
	afx_msg void OnRadioNotuseSite2();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadioBoth();
	afx_msg void OnRadio21();
	afx_msg void OnRadio22();
	afx_msg void OnRadioBoth2();
	afx_msg void OnSelchangeComboHifixMode();
	afx_msg void OnRadioRetestJigUse();
	afx_msg void OnRadioRetestJigNotuse();
	afx_msg void OnRadioRecipeUse();
	afx_msg void OnRadioRecipeNotuse();
	afx_msg void OnRadioCserial2Use();
	afx_msg void OnRadioCserial2Notuse();
	afx_msg void OnRadioAutoTimeout();
	afx_msg void OnDgtTimeOutPer();
	afx_msg void OnBtnSendHifix();
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBasic_Info_Apply(WPARAM wParam, LPARAM lParam);  // BASIC 화면 정보 갱신 요청 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREEN_BASIC_H__75EFE953_60C7_432F_9759_19FF79697AC2__INCLUDED_)
