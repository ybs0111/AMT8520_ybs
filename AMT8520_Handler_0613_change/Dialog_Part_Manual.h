#if !defined(AFX_DIALOG_PART_MANUAL_H__A53A0EB9_29A2_49B7_8A75_CFCC8AD06A03__INCLUDED_)
#define AFX_DIALOG_PART_MANUAL_H__A53A0EB9_29A2_49B7_8A75_CFCC8AD06A03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Part_Manual.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Part_Manual dialog
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

class CDialog_Part_Manual : public CDialog
{
public:
// 	CFont			*m_p_font;
	CPoint			m_cp_coord;

	int				m_n_part;
// Construction
public:
	BOOL Create();
	CDialog_Part_Manual(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Part_Manual)
	enum { IDD = IDD_DIALOG_PART_MANUAL };
	CMacButton	m_btn_io_move_12;
	CMacButton	m_btn_io_move_11;
	CMacButton	m_btn_io_move_10;
	CMacButton	m_btn_io_move_9;
	CMacButton	m_btn_io_move_8;
	CMacButton	m_btn_io_move_7;
	CMacButton	m_btn_io_move_6;
	CMacButton	m_btn_io_move_5;
	CMacButton	m_btn_io_move_4;
	CMacButton	m_btn_io_move_3;
	CMacButton	m_btn_io_move_2;
	CMacButton	m_btn_io_move_1;
	CMacButton	m_btn_motor_move_12;
	CMacButton	m_btn_motor_move_11;
	CMacButton	m_btn_motor_move_10;
	CMacButton	m_btn_motor_move_9;
	CMacButton	m_btn_motor_move_8;
	CMacButton	m_btn_motor_move_7;
	CMacButton	m_btn_motor_move_6;
	CMacButton	m_btn_motor_move_5;
	CMacButton	m_btn_motor_move_4;
	CMacButton	m_btn_motor_move_3;
	CMacButton	m_btn_motor_move_2;
	CMacButton	m_btn_motor_move_1;
	CXPGroupBox	m_group_motor;
	CXPGroupBox	m_group_io;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Part_Manual)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	void OnPManual_Button_Set();
	void OnPManual_GroupBox_Set();

	// Generated message map functions
	//{{AFX_MSG(CDialog_Part_Manual)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtnExit();
	afx_msg void OnBtnMotorMove1();
	afx_msg void OnBtnMotorMove2();
	afx_msg void OnBtnMotorMove3();
	afx_msg void OnBtnMotorMove4();
	afx_msg void OnBtnMotorMove5();
	afx_msg void OnBtnMotorMove6();
	afx_msg void OnBtnMotorMove7();
	afx_msg void OnBtnMotorMove8();
	afx_msg void OnBtnMotorMove9();
	afx_msg void OnBtnMotorMove10();
	afx_msg void OnBtnMotorMove11();
	afx_msg void OnBtnMotorMove12();
	afx_msg void OnBtnIoMove1();
	afx_msg void OnBtnIoMove2();
	afx_msg void OnBtnIoMove3();
	afx_msg void OnBtnIoMove4();
	afx_msg void OnBtnIoMove5();
	afx_msg void OnBtnIoMove6();
	afx_msg void OnBtnIoMove7();
	afx_msg void OnBtnIoMove8();
	afx_msg void OnBtnIoMove9();
	afx_msg void OnBtnIoMove10();
	afx_msg void OnBtnIoMove11();
	afx_msg void OnBtnIoMove12();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_PART_MANUAL_H__A53A0EB9_29A2_49B7_8A75_CFCC8AD06A03__INCLUDED_)
