#if !defined(AFX_DIALOGADD_H__0EBC64E8_CFE9_4500_A8E1_AE350EFDD957__INCLUDED_)
#define AFX_DIALOGADD_H__0EBC64E8_CFE9_4500_A8E1_AE350EFDD957__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogAdd.h : header file
//

#include ".\\Ctrl\\GradientStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogAdd dialog

class CDialogAdd : public CDialog
{
// Construction
public:
	CDialogAdd(CWnd* pParent = NULL);   // standard constructor
	CFont				*m_pFont;
// Dialog Data
	//{{AFX_DATA(CDialogAdd)
	enum { IDD = IDD_DIALOG_ADD };
	CGradientStatic	m_msgCountData;
	CGradientStatic	m_msgCount;
	CGradientStatic	m_msgHifixData;
	CGradientStatic	m_msgHifix;
	CGradientStatic	m_msgSerialData;
	CGradientStatic	m_msgSerial;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAdd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAdd)
	virtual BOOL OnInitDialog();
	afx_msg void OnMsgSerialData();
	afx_msg void OnMsgHifixData();
	afx_msg void OnMsgCountData();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void	InitLabel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGADD_H__0EBC64E8_CFE9_4500_A8E1_AE350EFDD957__INCLUDED_)
