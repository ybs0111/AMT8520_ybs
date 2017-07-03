#if !defined(AFX_DIALOG_DATA_COK_H__E35B2265_1676_4826_BAF5_5D5BBB9645A3__INCLUDED_)
#define AFX_DIALOG_DATA_COK_H__E35B2265_1676_4826_BAF5_5D5BBB9645A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Data_Cok.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_Cok dialog

class CDialog_Data_Cok : public StandardDialog
{
// Construction
public:
	CDialog_Data_Cok(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Data_Cok)
	enum { IDD = IDD_DIALOG_DATA_COK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Data_Cok)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Grid
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridData(UINT nID, int row, int col, CString data);
	void GridFont(UINT nID, int row, int col, int size);
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	CString GetGridData( UINT nID, int row, int col );
	// Generated message map functions

	void OnInitTestsite();
	void OnInitRobot();
	void OnInitCokStacker();

	void OnDisplayTestsite();
	void OnDisplayRobot();
	void OnDisplayStacker();

protected:
	int m_nNumMax;


	//{{AFX_MSG(CDialog_Data_Cok)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_DATA_COK_H__E35B2265_1676_4826_BAF5_5D5BBB9645A3__INCLUDED_)
