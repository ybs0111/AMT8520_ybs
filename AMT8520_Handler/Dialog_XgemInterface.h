//{{AFX_INCLUDES()
#include "exgem.h"
//}}AFX_INCLUDES
#if !defined(AFX_DIALOG_XGEMINTERFACE_H__11A5370B_E148_47AC_B894_1C39B2902F7C__INCLUDED_)
#define AFX_DIALOG_XGEMINTERFACE_H__11A5370B_E148_47AC_B894_1C39B2902F7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_XgemInterface.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_XgemInterface dialog

class CDialog_XgemInterface : public CDialog
{
// Construction
public:
	CDialog_XgemInterface(CWnd* pParent = NULL);   // standard constructor
	int			m_nXgemOpenFlag;
	int			m_nXgemStartFlag;
	CFont* m_pFont;

// Dialog Data
	//{{AFX_DATA(CDialog_XgemInterface)
	enum { IDD = IDD_XGEMINTERFACE_DIALOG };
	CGradientStatic	m_msgHostConnt;
	CGradientStatic	m_msgMachineStatus;
	CGradientStatic	m_msgEqConnt;
	CeXGem	m_hXgem;
	CString	m_strLotID;
	CString	m_strPartID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_XgemInterface)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Init();
	void OnLabelSet();
	void OnInit_xgem_info();
	void OnXGemInitialize(CString strpath);
	void OnEqConnectStatus(int nStatus);
	void OnHostConnectStatus(int nStatus);
	void OnMachineStatus(int nStatus);

protected:
	int m_nNumMax;
	CString m_strCombo;
	// Grid
	void GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void GridControl(UINT nID, int type, int row, int col, int pos);
	void GridData(UINT nID, int row, int col, CString data);
	void GridFont(UINT nID, int row, int col, int size);
	void GridMerge(UINT nID, int srow, int scol, int nrow, int ncol);
	CString GetGridData( UINT nID, int row, int col );

	void OnDisplay_XgemInfo();

	// Generated message map functions
	//{{AFX_MSG(CDialog_XgemInterface)
	afx_msg void OnButtonInitialize();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOffline();
	afx_msg void OnButtonOnlineLocal();
	afx_msg void OnButtonOnlineRemote();
	afx_msg void OnButtonAlarmSet();
	afx_msg void OnButtonAlarmClear();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonRun();
	afx_msg void OnButtonLotstart();
	afx_msg void OnButtonLotend();
	afx_msg void OnButtonPpid();
	afx_msg void OnButtonBufferIn();
	afx_msg void OnButtonLoadTray();
	afx_msg void OnButtonUnloadTray();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OneGEMCommStateChangedExgemctrlGem(long nState);
	afx_msg void OneGEMControlStateChangedExgemctrlGem(long nState);
	afx_msg void OneXGEMStateEventExgemctrlGem(long nState);
	afx_msg void OnButtonSocketOn();
	afx_msg void OnButtonSocketOff();
	afx_msg void OnButtonEqstatus();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDraw_Data_Xgem(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_XGEMINTERFACE_H__11A5370B_E148_47AC_B894_1C39B2902F7C__INCLUDED_)
