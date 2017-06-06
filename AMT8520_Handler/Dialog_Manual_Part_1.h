#if !defined(AFX_DIALOG_MANUAL_PART_1_H__21BCA4EB_454A_4AC2_BB4E_62D63C2476ED__INCLUDED_)
#define AFX_DIALOG_MANUAL_PART_1_H__21BCA4EB_454A_4AC2_BB4E_62D63C2476ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Manual_Part_1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Manual_Part_1 dialog
#include "BtnST.h"          // 칼라 버튼 생성 클래스 추가  
#include "NewLabel.h"		// 칼라 텍스트 박스 생성 클래스 추가 
#include "EditEx.h"			// 칼라 에디트 박스 생성 클래스 추가 
#include "EXDigitST.h"		// 디지털 카운터 클래스
#include "LedButton.h"      // LED 버튼 클래스
#include "XPGroupBox.h"		// 칼라 그룹 박스 생성 클래스 추가 
#include "SxLogFont.h"
#include "MacButtons.h"
#include "GradientStatic.h" // 그라데이션 칼라 텍스트 박스 생성 클래스 추가 
#include "XpButton.h"		// XP 칼라 버튼 클래스
#include "Dialog_Message.h"
#include "FastechPublic_IO.h"		// Fastech 관련 동작 Class
#include "ComizoaPublic.h"
#include "GridControlAlg.h"
#include "Digit.h"

#include "Dialog_Select.h"

class CDialog_Manual_Part_1 : public CDialog
{
// Construction
public:
	CDialog_Manual_Part_1(CWnd* pParent = NULL);   // standard constructor

// Attributes
public:
	CFont*	mp_manual_font;			// 폰트 설정 변수 선언 
	CPoint	mcp_coord;	
	int		mn_part;

	int n_response;					// 대화 상자 리턴 플래그

	CDialog_Select  select_dlg;

public:
	BOOL Create();
// Dialog Data
	//{{AFX_DATA(CDialog_Manual_Part_1)
	enum { IDD = IDD_DIALOG_MANUAL_PART_1 };
	CMacButton	m_btn_fm_u_smema_tr_signal_off;
	CMacButton	m_btn_fm_l_smema_req_signal_off;
	CMacButton	m_btn_buf2_boat_clmap_off;
	CMacButton	m_btn_buf1_boat_clamp_off;
	CMacRadioButton	m_radio_warpage_chk;
	CMacRadioButton	m_radio_uld_clamp_up;
	CMacRadioButton	m_radio_uld_clamp_on;
	CMacRadioButton	m_radio_uld_clamp_off;
	CMacRadioButton	m_radio_uld_clamp_down;
	CMacRadioButton	m_radio_uld_boat_chk;
	CMacRadioButton	m_radio_ld_clamp_on;
	CMacRadioButton	m_radio_ld_clamp_off;
	CMacRadioButton	m_radio_ld_boat_pos_2;
	CMacRadioButton	m_radio_ld_boat_pos_1;
	CMacRadioButton	m_radio_fm_u_smema_tr_signal_chk;
	CMacRadioButton	m_radio_fm_u_smema_req_signal_chk;
	CMacRadioButton	m_radio_fm_l_smema_tr_signal_chk;
	CMacRadioButton	m_radio_fm_l_smema_req_signal_chk;
	CMacRadioButton	m_radio_buf_2_unclamp_chk;
	CMacRadioButton	m_radio_buf_2_rotator_chk;
	CMacRadioButton	m_radio_buf_2_clamp_chk;
	CMacRadioButton	m_radio_buf_2_boat_chk;
	CMacRadioButton	m_radio_buf_1_unclamp_chk;
	CMacRadioButton	m_radio_buf_1_rotator_chk;
	CMacRadioButton	m_radio_buf_1_clamp_chk;
	CMacRadioButton	m_radio_buf_1_boat_check;
	CMacRadioButton	m_radio_boat_id_8;
	CMacRadioButton	m_radio_boat_id_7;
	CMacRadioButton	m_radio_boat_id_6;
	CMacRadioButton	m_radio_boat_id_5;
	CMacRadioButton	m_radio_boat_id_4;
	CMacRadioButton	m_radio_boat_id_3;
	CMacRadioButton	m_radio_boat_id_2;
	CMacRadioButton	m_radio_boat_id_1;
	CMacRadioButton	m_radio_boat_chk_2;
	CMacRadioButton	m_radio_boat_chk_1;
	CMacButton	m_btn_uld_m_boat_place;
	CMacButton	m_btn_uld_m_boat_pick_2;
	CMacButton	m_btn_uld_m_boat_pick_1;
	CMacButton	m_btn_ld_m_boat_place_2;
	CMacButton	m_btn_ld_m_boat_place_1;
	CMacButton	m_btn_ld_m_boat_pick;
	CMacButton	m_btn_fm_u_smema_tr_signal;
	CMacButton	m_btn_fm_l_smema_req_signal;
	CMacButton	m_btn_buf2_boat_clamp;
	CMacButton	m_btn_buf1_boat_clamp;
	CMacButton	m_btn_1_exit;
	CMacButton	m_btn_buf2_uld;
	CMacButton	m_btn_buf2_ld;
	CMacButton	m_btn_buf2_flipper;
	CMacButton	m_btn_buf2_deg_180;
	CMacButton	m_btn_buf2_deg_0;
	CMacButton	m_btn_buf1_uld;
	CMacButton	m_btn_buf1_ld;
	CMacButton	m_btn_buf1_flipper;
	CMacButton	m_btn_buf1_deg_180;
	CMacButton	m_btn_buf1_deg_0;
	CMacButton	m_btn_ld_x_wp_8;
	CMacButton	m_btn_ld_x_wp_7;
	CMacButton	m_btn_ld_x_wp_6;
	CMacButton	m_btn_ld_x_wp_5;
	CMacButton	m_btn_ld_x_wp_4;
	CMacButton	m_btn_ld_x_wp_3;
	CMacButton	m_btn_ld_x_wp_2;
	CMacButton	m_btn_ld_x_wp_1;
	CGradientStatic	m_label_warpage_check;
	CGradientStatic	m_label_id_check;
	CGradientStatic	m_label_boat_check;
	CMacButton	m_btn_uld_x_place;
	CMacButton	m_btn_uld_x_buf_2;
	CMacButton	m_btn_uld_x_buf_1;
	CMacButton	m_btn_uld_updn_place_dn;
	CMacButton	m_btn_uld_updn_buf_2_pick_dn;
	CMacButton	m_btn_uld_updn_buf_1_pick_dn;
	CMacButton	m_btn_uld_boat_clamp_on;
	CMacButton	m_btn_uld_boat_clamp_off;
	CMacButton	m_btn_ld_z_place_buf_2;
	CMacButton	m_btn_ld_z_place_buf_1;
	CMacButton	m_btn_ld_z_pick;
	CMacButton	m_btn_ld_x_pick;
	CMacButton	m_btn_ld_x_id_check;
	CMacButton	m_btn_ld_x_buf_2;
	CMacButton	m_btn_ld_x_buf_1;
	CMacButton	m_btn_ld_boat_clamp_on;
	CMacButton	m_btn_ld_boat_clamp_off;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Manual_Part_1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnManualPart1_Label_Set();
	void OnManualPart1_IO_Check();

	// Generated message map functions
	//{{AFX_MSG(CDialog_Manual_Part_1)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtn1Exit();
	afx_msg void OnBtnLdBoatClampOn();
	afx_msg void OnBtnLdBoatClampOff();
	afx_msg void OnBtnUldBoatClampOn();
	afx_msg void OnBtnUldBoatClampOff();
	afx_msg void OnBtnBuf1BoatClamp();
	afx_msg void OnBtnBuf2BoatClamp();
	afx_msg void OnBtnFmLSmemaReqSignal();
	afx_msg void OnBtnFmUSmemaTrSignal();
	afx_msg void OnBtnBuf1BoatClampOff();
	afx_msg void OnBtnBuf2BoatClampOff();
	afx_msg void OnBtnFmLSmemaReqSignalOff();
	afx_msg void OnBtnFmUSmemaTrSignalOff();
	afx_msg void OnBtnLdZPick();
	afx_msg void OnBtnLdZPlaceBuf1();
	afx_msg void OnBtnLdZPlaceBuf2();
	afx_msg void OnBtnLdXPick();
	afx_msg void OnBtnLdXBuf1();
	afx_msg void OnBtnLdXBuf2();
	afx_msg void OnBtnLdXIdCheck();
	afx_msg void OnBtnLdXWp1();
	afx_msg void OnBtnLdXWp2();
	afx_msg void OnBtnLdXWp3();
	afx_msg void OnBtnLdXWp4();
	afx_msg void OnBtnLdXWp5();
	afx_msg void OnBtnLdXWp6();
	afx_msg void OnBtnLdXWp7();
	afx_msg void OnBtnLdXWp8();
	afx_msg void OnBtnLdMBoatPick();
	afx_msg void OnBtnLdMBoatPlace1();
	afx_msg void OnBtnLdMBoatPlace2();
	afx_msg void OnBtnUldUpdnPlaceDn();
	afx_msg void OnBtnUldUpdnBuf1PickDn();
	afx_msg void OnBtnUldUpdnBuf2PickDn();
	afx_msg void OnBtnUldXPlace();
	afx_msg void OnBtnUldXBuf1();
	afx_msg void OnBtnUldXBuf2();
	afx_msg void OnBtnUldMBoatPlace();
	afx_msg void OnBtnUldMBoatPick1();
	afx_msg void OnBtnUldMBoatPick2();
	afx_msg void OnBtnBuf1Ld();
	afx_msg void OnBtnBuf1Flipper();
	afx_msg void OnBtnBuf1Uld();
	afx_msg void OnBtnBuf1Deg0();
	afx_msg void OnBtnBuf1Deg180();
	afx_msg void OnBtnBuf2Ld();
	afx_msg void OnBtnBuf2Flipper();
	afx_msg void OnBtnBuf2Uld();
	afx_msg void OnBtnBuf2Deg0();
	afx_msg void OnBtnBuf2Deg180();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_MANUAL_PART_1_H__21BCA4EB_454A_4AC2_BB4E_62D63C2476ED__INCLUDED_)
