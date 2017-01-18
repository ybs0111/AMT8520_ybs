#if !defined(AFX_DIALOG_DATA_TESTSITE_H__AF438B8A_7162_409A_83FB_08489F472A51__INCLUDED_)
#define AFX_DIALOG_DATA_TESTSITE_H__AF438B8A_7162_409A_83FB_08489F472A51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Data_TestSite.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Data_TestSite dialog

class CDialog_Data_TestSite : public StandardDialog
{
// Construction
public:
	CDialog_Data_TestSite(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Data_TestSite)
	enum { IDD = IDD_DIALOG_DATA_TESTSITE };
	CButton	m_chk_update_by_bpc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Data_TestSite)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnInit_TestSite();
	void OnDisplay_TestSite();

	void OnUse( int iIdx, bool bVal );
	//2016.0709
// 	void OnUse( int iIdx, int bVal );
	void OnBoardOn( int iIdx, bool bVal );

	// Grid
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridData(UINT nID, int row, int col, CString data);
	void GridFont(UINT nID, int row, int col, int size);
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);

	// Generated message map functions
	//{{AFX_MSG(CDialog_Data_TestSite)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnBtnQuery();
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDraw_Data_TestSite(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_DATA_TESTSITE_H__AF438B8A_7162_409A_83FB_08489F472A51__INCLUDED_)
