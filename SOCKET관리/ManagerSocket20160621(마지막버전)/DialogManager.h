#if !defined(AFX_DIALOGMANAGER_H__B475FCAB_0000_4013_BD00_8FBDCAEBE168__INCLUDED_)
#define AFX_DIALOGMANAGER_H__B475FCAB_0000_4013_BD00_8FBDCAEBE168__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogManager.h : header file
//

#include ".\\Ctrl\\Grid\\GridCtrl.h"
#include ".\\Ctrl\\GradientStatic.h"
#include ".\\Ctrl\\BtnST.h"
#include ".\\Ctrl\\XPGroupBox.h"
#include "GridControlAlg.h"
/////////////////////////////////////////////////////////////////////////////
// CDialogManager dialog

class CDialogManager : public CDialog
{
// Construction
public:
	CDialogManager(CWnd* pParent = NULL);   // standard constructor

	int					m_nMode;
	int					m_nRow;
	int					m_nHifixNum;
	int					m_nHifixData;
	CFont				*m_pFont;
	
	CPoint				m_cpXY;

	TSpread				*m_pGridManager;
	TSpread				*m_pGridSocket;
	TSpread				*m_pGridHifix;	

	GridControlAlg		m_pGrid;

	BOOL				Create();
// Dialog Data
	//{{AFX_DATA(CDialogManager)
	enum { IDD = IDD_DIALOG_MANAGER_SOCKET };
	CComboBox	m_cboHifix;
	CGradientStatic	m_msgHifixNumData;
	CGradientStatic	m_msgHifixNum;
	CXPGroupBox	m_groupSocket;
	CButtonST	m_btnRemove;
	CButtonST	m_btnInsert;
	CButtonST	m_btnDel;
	CButtonST	m_btnAdd;
	CGradientStatic	m_msgSlotData;
	CGradientStatic	m_msgSlot;
	CGradientStatic	m_msgSerialData;
	CGradientStatic	m_msgSerial;
	CGradientStatic	m_msgCountData;
	CGradientStatic	m_msgCount;
	CGradientStatic	m_msgHifixData;
	CGradientStatic	m_msgHifix;
	CGradientStatic		m_msgType;
	CGradientStatic		m_msgTypeData;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogManager)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogManager)
	afx_msg void OnMsgHifixData();
	afx_msg void OnMsgSerialData();
	afx_msg void OnMsgSlotData();
	afx_msg void OnMsgCountData1();
	afx_msg void OnBtnSocketInsert();
	afx_msg void OnBtnSocketRemove();
	afx_msg void OnBtnAddSocket();
	afx_msg void OnBtnDeleteSocket();
	virtual BOOL OnInitDialog();
	afx_msg void OnMsgHifixNumData();
	afx_msg void OnBtnApply();
	afx_msg void OnBtnChangeSocket();
	//}}AFX_MSG
	afx_msg void OnCellClick(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	LOGFONT				LogFont(int nFontSize);
	
	BOOL				KeyCheck(CString strKey);
	void				DisplaySocket();
	void				DisplayManager();
	void				DisplayHifix(int nHifix);
	void				InitGridManager();
	void				InitGridSocket(int nCount);
	void				InitGridHifix();
	void				InitLabel();
	void				InitButton();
	void				InitGroupBox();
	void				GridColor(UINT nID, int row, int col, COLORREF bk, COLORREF tk);
	void				On_LogFile_Add( CString str_msg );
	int					FileSizeCheck(char * cp_filename, int n_size, int n_check);
	BOOL				CreateFolder(LPCTSTR szPath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGMANAGER_H__B475FCAB_0000_4013_BD00_8FBDCAEBE168__INCLUDED_)
