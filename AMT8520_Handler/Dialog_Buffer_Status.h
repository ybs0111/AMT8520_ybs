#if !defined(AFX_DIALOG_BUFFER_STATUS_H__01AD0781_DA70_4C1E_875A_6836FF4BCB0F__INCLUDED_)
#define AFX_DIALOG_BUFFER_STATUS_H__01AD0781_DA70_4C1E_875A_6836FF4BCB0F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Buffer_Status.h : header file
//
#include "Variable.h"		// 전역 변수 정의 클래스 추가 
#include "Public_Function.h"

#include "MacButtons.h"
#include "GradientStatic.h" // 그라데이션 칼라 텍스트 박스 생성 클래스 추가 

/////////////////////////////////////////////////////////////////////////////
// CDialog_Buffer_Status dialog

class CDialog_Buffer_Status : public StandardDialog
{
// Construction
public:
	CDialog_Buffer_Status(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Buffer_Status)
	enum { IDD = IDD_DIALOG_BUFFER_STATUS };
	CGradientStatic	m_label_title;
	//}}AFX_DATA

// Attributes
public:
// 	CFont* mp_buffer_font;			// 폰트 정보 설정 변수
	
	GridControlAlg m_pGrid;

// Operations
public:
	int On_Buffer_Bin_Change(CString strBin);
	void OnChangeValue(int nid, int nrow, int ncol);
	void On_Display_Buffer();
	void OnBuffer_Label_Set();
	void On_Init_Buffer();
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	void GridFont(UINT nID, int row, int col, int size);
	void GridData(UINT nID, int row, int col, CString data);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Buffer_Status)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Buffer_Status)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnApply();
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_BUFFER_STATUS_H__01AD0781_DA70_4C1E_875A_6836FF4BCB0F__INCLUDED_)
