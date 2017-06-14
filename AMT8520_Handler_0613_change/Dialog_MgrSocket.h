#if !defined(AFX_DIALOG_MGRSOCKET_H__A8CCD504_3489_48A7_ABF2_A0304113771C__INCLUDED_)
#define AFX_DIALOG_MGRSOCKET_H__A8CCD504_3489_48A7_ABF2_A0304113771C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_MgrSocket.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_MgrSocket dialog

class CDialog_MgrSocket : public CDialog
{
// Construction
public:
	CDialog_MgrSocket(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_MgrSocket)
	enum { IDD = IDD_DIALOG_MANAGER_SOCKET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_MgrSocket)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_MgrSocket)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnButtonView();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_MGRSOCKET_H__A8CCD504_3489_48A7_ABF2_A0304113771C__INCLUDED_)
