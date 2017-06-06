#if !defined(AFX_SCREEN_MOTOR_WITH_IO_H__3BB5E94D_A9C3_4BAF_8BE2_41D8CC732614__INCLUDED_)
#define AFX_SCREEN_MOTOR_WITH_IO_H__3BB5E94D_A9C3_4BAF_8BE2_41D8CC732614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Screen_Motor_With_IO.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScreen_Motor_With_IO form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define TMR_WITH_IO_READ				950

class CScreen_Motor_With_IO : public CFormView
{
public:
	CScreen_Motor_With_IO();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CScreen_Motor_With_IO)

// Form Data
public:
	//{{AFX_DATA(CScreen_Motor_With_IO)
	enum { IDD = IDD_SCREEN_MOTOR_WITH_IO };
	CButton	m_chk_pick;
	CButton	m_btn_down;
	CButtonST	m_btn_plus;
	CButtonST	m_btn_minus;
	CButton	m_btn_go;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void OnInitMotor();
	void OnInitIO();
	void OnInitPOGOIO();

	void OnInitTarget_TestSite();
	void OnInitTarget_Robot();
	void OnInitTarget_Tray(int iGrid);
	void OnInitMotorInfo();
	void OnInitTarget_cokStacker();



	void OnUpdateIO();
	void OnUpdateTarget_testSite();
	void OnUpdateTarget_Robot();
	void OnUpdateTarget_Tray(int iGrid);
	void OnUpdateMotorInfo();
	void OnUpdateTarget_cokStacker();



	void On_GoMotor( int nAxis, double dGotoPos, int row, int col );

	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	void GridFont(UINT nID, int row, int col, int size);
	void GridData(UINT nID, int row, int col, CString data);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);


protected:
	int m_nGo_col;
	int m_nGo_row;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScreen_Motor_With_IO)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	int m_nRobot;
	int m_nTarget;

	int m_nAxis;
	int m_nAxis_old;
	CString m_strPos_old;
	CString m_strTempMSG;

// Implementation
protected:
	virtual ~CScreen_Motor_With_IO();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CScreen_Motor_With_IO)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBtnRobotGo();
	afx_msg void OnBtnHome();
	afx_msg void OnBtnEmgStop();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnTeachS1Start();
	afx_msg void OnBtnTeachS1End();
	afx_msg void OnBtnTeachS2Start();
	afx_msg void OnBtnTeachS2End();
	afx_msg void OnBtnTeachBin1Start();
	afx_msg void OnBtnTeachBin1End();
	afx_msg void OnBtnTeachBin2Start();
	afx_msg void OnBtnTeachBin2End();
	afx_msg void OnBtnTeachRejStart();
	afx_msg void OnBtnTeachRejEnd();
	afx_msg void OnBtnTeachCokstStart();
	afx_msg void OnBtnTeachCokstEnd();
	afx_msg void OnBtnTeachS1Cok();
	afx_msg void OnBtnTeachS1Place();
	afx_msg void OnBtnTeachBin1Place();
	afx_msg void OnBtnTeachS1CokPlace();
	afx_msg void OnBtnTeachCokstPlace();
	afx_msg void OnBtnTeachS1Calc();
	afx_msg void OnBtnTeachS2Calc();
	afx_msg void OnBtnTeachBin3Start();
	afx_msg void OnBtnTeachBin3End();
	afx_msg void OnBtnDoorOpen();
	//}}AFX_MSG

	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMotorCompletion(WPARAM wParam,LPARAM lParam);

	LONG OnUserLButtonDown(WPARAM wParam, LPARAM lParam);
	LONG OnUserLButtonUp(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCREEN_MOTOR_WITH_IO_H__3BB5E94D_A9C3_4BAF_8BE2_41D8CC732614__INCLUDED_)
