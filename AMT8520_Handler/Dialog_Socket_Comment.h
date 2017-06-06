#if !defined(AFX_DIALOG_SOCKET_COMMENT_H__1FDFE6F8_955D_4244_9CDC_959E90815AF9__INCLUDED_)
#define AFX_DIALOG_SOCKET_COMMENT_H__1FDFE6F8_955D_4244_9CDC_959E90815AF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dialog_Socket_Comment.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialog_Socket_Comment dialog

class CDialog_Socket_Comment : public StandardDialog
{
// Construction
public:
	CDialog_Socket_Comment(int nId, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialog_Socket_Comment)
	enum { IDD = IDD_DIALOG_SOCKET_COMMENT };
	CEdit	m_editPrevStr;
	CEdit	m_editNextStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialog_Socket_Comment)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
private:
	CButton *mp_msg_ok;  // 버튼에 대한 포인터 선언
	int mn_dialog;

protected:
	CString m_strPrev;
	CString m_strNext;
	int m_niid;


public:
	void OnMsg_Button_Set();

protected:

	// Generated message map functions
	//{{AFX_MSG(CDialog_Socket_Comment)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void SaveOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOG_SOCKET_COMMENT_H__1FDFE6F8_955D_4244_9CDC_959E90815AF9__INCLUDED_)
