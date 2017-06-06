// MgrSerialSocketDlg.h : header file
//

#if !defined(AFX_MGRSERIALSOCKETDLG_H__303BE267_126D_4466_A3DC_7745DF765EED__INCLUDED_)
#define AFX_MGRSERIALSOCKETDLG_H__303BE267_126D_4466_A3DC_7745DF765EED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include ".\\Ctrl\\Grid\\GridCtrl.h"
#include ".\\Ctrl\\GradientStatic.h"
#include ".\\Ctrl\\BtnST.h"
#include ".\\Ctrl\\XPGroupBox.h"
#include "GridControlAlg.h"
/////////////////////////////////////////////////////////////////////////////
// CMgrSerialSocketDlg dialog

class CMgrSerialSocketDlg : public CDialog
{
// Construction
public:
	CMgrSerialSocketDlg(CWnd* pParent = NULL);	// standard constructor

	TSpread				*m_pGridSerial;	
	
	GridControlAlg		m_pGrid;
	CFont				*m_pFont;
	void InitLabel();
	int OnKeyTotal(CString strKey, CString& strNew);
	BOOL KeyCheck(CString strKey);
	void InitGridManager(int nCount);
	void DisplayGridManager( CString m_StrSerial );
	void DisplaySerial(int nNum, CString* strSerial);
	BOOL GetSerialData();
	BOOL CreateFolder(LPCTSTR szPath);
	BOOL CheckHeader();
	void OutBarcode();

// Dialog Data
	//{{AFX_DATA(CMgrSerialSocketDlg)
	enum { IDD = IDD_MGRSERIALSOCKET_DIALOG };
	CGradientStatic	m_msgSerialNum;
	CGradientStatic	m_msgSerialData;
	CGradientStatic	m_msgSerial;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMgrSerialSocketDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMgrSerialSocketDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtn1();
	afx_msg void OnMsgSerialData();
	afx_msg void OnBtn5();
	afx_msg void OnBtn10();
	afx_msg void OnBtn20();
	afx_msg void OnMsgSerialNum();
	afx_msg void OnBtnRun();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MGRSERIALSOCKETDLG_H__303BE267_126D_4466_A3DC_7745DF765EED__INCLUDED_)
